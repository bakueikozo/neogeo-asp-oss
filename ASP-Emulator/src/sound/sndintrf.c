/******************************************************************************

	sndintrf.c

	サウンドインタフェース

******************************************************************************/

#include "emumain.h"


#define FRAC_BITS	14
#define FRAC_ONE	(1 << FRAC_BITS)
#define FRAC_MASK	(FRAC_ONE - 1)

#define LINEAR_INTERPORATION	0

#define SAFETY	32

/******************************************************************************
	ローカル変数
******************************************************************************/

static INT32 ALIGN_DATA stream_buffer_left[SOUND_BUFFER_SIZE + SAFETY];
static INT32 ALIGN_DATA stream_buffer_right[SOUND_BUFFER_SIZE + SAFETY];
static INT32 ALIGN_DATA *stream_buffer[2];

static float samples_per_update;
static float samples_left_over;
static UINT32 samples_this_update;

/******************************************************************************
	ローカル関数
******************************************************************************/

/*------------------------------------------------------
	サンプルをクリップ
------------------------------------------------------*/

static void clip_stream(INT32 *buffer)
{
	UINT32 samples = samples_this_update;
	INT32 sample;

	while (samples--)
	{
		sample = *buffer;
		Limit(sample, MAXOUT, MINOUT);
		*buffer++ = sample;
	}
}


/*------------------------------------------------------
	リサンプリング
------------------------------------------------------*/

static void resample_stream(INT32 *src, INT16 *dst)
{
	UINT32 pos = 0;
	UINT32 src_step = (samples_this_update << FRAC_BITS) / sound->samples;
	UINT32 samples = sound->samples;
#if LINEAR_INTERPORATION 
	INT32 sample;
#endif

#if LINEAR_INTERPORATION
	src[samples_this_update] = src[samples_this_update - 1];
#endif

	{
		while (samples--)
		{
#if LINEAR_INTERPORATION
			sample  = src[(pos >> FRAC_BITS) + 0] * (FRAC_ONE - (pos & FRAC_MASK));
			sample += src[(pos >> FRAC_BITS) + 1] * (pos & FRAC_MASK);
			sample >>= FRAC_BITS;
			*dst = sample;
#else
			*dst = src[pos >> FRAC_BITS];
#endif
			dst += 2;
			pos += src_step;
		}
	}
}



/*------------------------------------------------------
	サウンド更新(ステレオ)
------------------------------------------------------*/

static void sound_update_stereo(INT16 *buffer)
{
	(*sound->callback)(stream_buffer, samples_this_update);

	clip_stream(stream_buffer[0]);
	clip_stream(stream_buffer[1]);

	resample_stream(stream_buffer[0], &buffer[0]);
	resample_stream(stream_buffer[1], &buffer[1]);

	samples_left_over  += samples_per_update;
	samples_this_update = (UINT32)samples_left_over;
	samples_left_over  -= samples_this_update;
}

/*------------------------------------------------------
	サウンド更新(モノラル)
------------------------------------------------------*/


/******************************************************************************
	サウンドインタフェース関数
******************************************************************************/

/*------------------------------------------------------
	サウンドエミュレーション初期化
------------------------------------------------------*/
int sound_init(void)
{
	YM2610_sh_start();

    sound->update = sound_update_stereo;

	memset(stream_buffer_left, 0, sizeof(stream_buffer_left));
	memset(stream_buffer_right, 0, sizeof(stream_buffer_right));

	stream_buffer[0] = stream_buffer_left;
	stream_buffer[1] = stream_buffer_right;

	samples_per_update = ((float)sound->frequency / FPS) * 2; 

	samples_left_over   = samples_per_update;
	samples_this_update = (UINT32)samples_per_update;
	samples_left_over  -= samples_this_update;

	return sound_thread_start();
}


/*------------------------------------------------------
	サウンドエミュレーション終了
------------------------------------------------------*/

void sound_exit(void)
{
	YM2610_sh_stop();

	sound_thread_stop();
}


/*------------------------------------------------------
	サウンドエミュレーションリセット
------------------------------------------------------*/

void sound_reset(void)
{
	YM2610_sh_reset();
}


/*------------------------------------------------------
	サウンド再生レート設定
------------------------------------------------------*/

void sound_set_samplerate(void)
{
	//int sample_shift;

	YM2610_set_samplerate();

	samples_per_update = ((float)sound->frequency / FPS) * 2;

	samples_left_over   = samples_per_update;
	samples_this_update = (UINT32)samples_per_update;
	samples_left_over  -= samples_this_update;
}


/*------------------------------------------------------
	サウンドミュート
------------------------------------------------------*/
