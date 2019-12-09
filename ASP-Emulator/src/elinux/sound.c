/******************************************************************************

	sound.c

	PSP サウンドスレッド

******************************************************************************/
#include "emumain.h"

static INT16 sound_buffer[SOUND_BUFFER_SIZE];

static struct sound_t sound_info;
struct sound_t *sound = &sound_info;

static pthread_t gSoundThread = (pthread_t)-1;
pthread_mutex_t gAudioMutex;
char *gAudioBuffer = NULL;
int gAudioBlockLen;
int gReadIDX = 0;
int gWriteIDX = 0;
int gExitSoundThread;

#define AUDIO_BLOCKS    8

static int dspfd = -1;

//#define WRITESOUND
FILE* pcmfd = NULL;
int lidx = 0;

//void SndFillAudioData(char* data){
void SndFillAudioData(void){

    (*sound->update)(sound_buffer);

    pthread_mutex_lock(&gAudioMutex);
    if(gAudioBuffer){
        if(gWriteIDX < gReadIDX){
            gWriteIDX = gReadIDX = 0;
            memset(gAudioBuffer,0,gAudioBlockLen * AUDIO_BLOCKS);
            printf("Maybe Error...\n");
        }
        if(gWriteIDX == gReadIDX+AUDIO_BLOCKS){
            printf("Audio data too fast,discard...\n");
        }else{
            memcpy(gAudioBuffer+gAudioBlockLen*(gWriteIDX%AUDIO_BLOCKS),sound_buffer,gAudioBlockLen);
            gWriteIDX++;
        }
    }
    pthread_mutex_unlock(&gAudioMutex);
}
static void SndReadAudioData(){
    pthread_mutex_lock(&gAudioMutex);
    if(/*dspfd > 0 &&*/ gAudioBuffer){
        if(gWriteIDX < gReadIDX){
            gWriteIDX = gReadIDX = 0;
            memset(gAudioBuffer,0,gAudioBlockLen * AUDIO_BLOCKS);
            printf("Maybe Error...\n");
        }
        if(gReadIDX == gWriteIDX){
            pthread_mutex_unlock(&gAudioMutex);
            usleep(1000);
            return;
        }else{
            //write(dspfd, (void *)gAudioBuffer+gAudioBlockLen*(gReadIDX%AUDIO_BLOCKS),gAudioBlockLen);
#ifdef WRITESOUND
			if(pcmfd){ 
				lidx++;
				if(lidx <= 3600){
					fwrite((void *)gAudioBuffer+gAudioBlockLen*(gReadIDX%AUDIO_BLOCKS),1,gAudioBlockLen,pcmfd);
				}
				if(lidx > 3600){
					fflush(pcmfd);
					fclose(pcmfd);
					pcmfd = NULL;
				}		
			}
#endif
			gReadIDX++;
		}
	}
	pthread_mutex_unlock(&gAudioMutex);
}

/*--------------------------------------------------------
  サウンド更新スレッド
  --------------------------------------------------------*/

static void *sound_update_thread(void *argp)
{
    gExitSoundThread = 0;
    
    while(!gExitSoundThread){
        SndReadAudioData();
    }
    
    if(gAudioBuffer){
        free(gAudioBuffer);
        gAudioBuffer = NULL;
    }
    pthread_mutex_destroy(&gAudioMutex);
    
    return NULL;
}


/******************************************************************************
  グローバル関数
******************************************************************************/

/*--------------------------------------------------------
	サウンド有効/無効切り替え
--------------------------------------------------------*/

void sound_thread_enable(int enable)
{
}


/*--------------------------------------------------------
	サウンドのボリューム設定
--------------------------------------------------------*/

void sound_thread_set_volume(void)
{
}


/*--------------------------------------------------------
	サウンドスレッド開始
--------------------------------------------------------*/

int sound_thread_start(void)
{
#ifdef WRITESOUND
	pcmfd = fopen("testsound.pcm","wb");
	if(pcmfd <= 0)
		printf("open write sound file fail...\n");
#endif
    gAudioBlockLen = SOUND_BUFFER_SIZE;
    gAudioBuffer = (char*)malloc(gAudioBlockLen * AUDIO_BLOCKS);
    memset(gAudioBuffer,0,gAudioBlockLen * AUDIO_BLOCKS);
    gReadIDX = 0;
    gWriteIDX = 0;

	memset(sound_buffer, 0, sizeof(sound_buffer));

    pthread_mutex_init(&gAudioMutex, NULL);

    pthread_create(&gSoundThread, NULL, sound_update_thread, NULL);

	return 1;
}


/*--------------------------------------------------------
	サウンドスレッド停止
--------------------------------------------------------*/

void sound_thread_stop(void)
{
    gExitSoundThread = 1;
}
