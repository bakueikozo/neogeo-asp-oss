/******************************************************************************

	mvs.c

	MVSエミュレーションコア

******************************************************************************/

#include "mvs.h"


/******************************************************************************
	グローバル変数
******************************************************************************/

int neogeo_bios = -1;
int neogeo_region;
int neogeo_save_sound_flag;

/******************************************************************************
	ローカル変数
******************************************************************************/

/******************************************************************************
	ローカル関数
******************************************************************************/

/*--------------------------------------------------------
	MVSエミュレーション初期化
--------------------------------------------------------*/

static int neogeo_init(void)
{
	FILE* fd;
	char path[MAX_PATH];

	{
		sprintf(path, "%smemcard/%s.bin", launchDir, game_name);
		if ((fd = fopen(path, "rb")) != NULL)
		{
		    fread(neogeo_memcard, 1, 0x800, fd);
			fclose(fd);
		}
		//sprintf(path, "%snvram/%s.nv", launchDir, game_name);
		sprintf(path, "%snvram/%s.nv", launchDir, game_name);
		if ((fd = fopen(path, "rb")) != NULL)
		{
			fread(neogeo_sram16, 1, 0x2000, fd);
			fclose(fd);
			swab((char*)neogeo_sram16, (char*)neogeo_sram16, 0x2000);
		}else{
            printf("ERROR:Open %s fail...\n",path);
        }
	}

	neogeo_driver_init();
	neogeo_video_init();
	
    printf("DONE2\n");

	video_clear_buffer();
	
	return 1;
}


/*--------------------------------------------------------
	MVSエミュレーションリセット
--------------------------------------------------------*/

static void neogeo_reset(void)
{

	video_clear_buffer();
	
	timer_reset();
	input_reset();

	neogeo_driver_reset();
	neogeo_video_reset();

	sound_reset();

	Loop = LOOP_EXEC;
}


/*--------------------------------------------------------
	MVSエミュレーション終了
--------------------------------------------------------*/

static void neogeo_exit(void)
{
	FILE* fd;
	char path[MAX_PATH];

    video_clear_buffer();

	printf("PLEASE_WAIT2\n");

	{
		sprintf(path, "%smemcard/%s.bin", launchDir, game_name);
		if ((fd = fopen(path, "wb")) != NULL)
		{
			fwrite(neogeo_memcard, 1, 0x800, fd);
			fclose(fd);
		}
		sprintf(path, "%snvram/%s.nv", launchDir, game_name);
		if ((fd = fopen(path, "wb")) != NULL)
		{
			swab((char*)neogeo_sram16, (char*)neogeo_sram16, 0x2000);
			fwrite(neogeo_sram16, 1, 0x2000, fd);
			fclose(fd);
		}
		if (neogeo_save_sound_flag) option_sound_enable = 1;
	}

	printf("DONE2\n");
}

/*--------------------------------------------------------
	MVSエミュレーション実行
--------------------------------------------------------*/

static void neogeo_run(void)
{
	while (Loop >= LOOP_RESET)
	{
		neogeo_reset();

#define FRAME_TIME_59_2 16892 //1000000/59.2
#define MAXSKIPFRAME 3
		//zyh
		int lFPSCTime,lFPSLTime;
		int lCurTime,lLastTime;
		int lRealFrame;
		int lVirFrame;
		int lIntervalTime;
		int lSkipFrame;
		int lTotalFrame;
		int FRAME_TIME;

		FRAME_TIME = FRAME_TIME_59_2;
		lRealFrame = 0;
		lVirFrame = 0;
		lSkipFrame = 0;
		lTotalFrame = 0;

		lFPSLTime = lLastTime = ZYH_GetTicks();

		while (Loop == LOOP_EXEC)
		{
			update_inputport();

			lFPSCTime = ZYH_GetTicks();
			if((lFPSCTime-lFPSLTime) > 1000000)
			{
				if((lVirFrame < 60) || (lRealFrame < 60))
					printf("Real = %d Virtual = %d\n",lRealFrame,lVirFrame);
				lRealFrame = 0;
				lVirFrame = 0;
				lFPSLTime = lFPSCTime;
			}
			lCurTime = ZYH_GetTicks();
			lIntervalTime = lCurTime-lLastTime-lTotalFrame*FRAME_TIME;
			if(lCurTime < lLastTime || lTotalFrame > 72000/*60*60*20*/){
				ZYH_StartTicks();
				lLastTime = lCurTime = ZYH_GetTicks();
				lTotalFrame = 0;
				lSkipFrame = 0;
				lIntervalTime = 0;
			}
			lTotalFrame++;
			if(lIntervalTime > 0 && lSkipFrame < MAXSKIPFRAME)
			{
				timer_update_cpu(1);
				update_screen(1);
				SndFillAudioData();
				
				lSkipFrame++;
			}else{
				int lFStart,lFEnd;
				lFStart = ZYH_GetTicks();

				timer_update_cpu(0);
				update_screen(0);
				SndFillAudioData();

				lRealFrame++;
				if(lSkipFrame < MAXSKIPFRAME){
					lFEnd = ZYH_GetTicks();
					if(lFEnd < lFStart) lFStart = lFEnd;
					int lSleepTime = FRAME_TIME-(lFEnd - lFStart) - lIntervalTime - FRAME_TIME/2;
					if(lSleepTime > 1000){
						usleep(lSleepTime);
					}
				}else{
					lTotalFrame = 0;
					ZYH_StartTicks();
					lLastTime = lCurTime = ZYH_GetTicks();
				}
				lSkipFrame = 0;
			}
			lVirFrame++;
		}

		video_clear_buffer();
	}
}


/******************************************************************************
  グローバル関数
 ******************************************************************************/

/*--------------------------------------------------------
  MVSエミュレーションメイン
  --------------------------------------------------------*/

void neogeo_main(void)
{
	Loop = LOOP_RESET;

	while (Loop >= LOOP_RESTART)
	{
		Loop = LOOP_EXEC;

		if (memory_init())
		{
			if (sound_init())
			{
				if (input_init())
				{
					if (neogeo_init())
					{
						neogeo_run();
					}
					neogeo_exit();
				}
				input_shutdown();
			}
			sound_exit();
		}
		memory_shutdown();
	}
}

void neogeo_emulator_exit(void){
	Loop = LOOP_EXIT;
}
