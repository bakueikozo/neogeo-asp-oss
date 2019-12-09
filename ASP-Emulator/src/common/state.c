/******************************************************************************

	state.c

	ステートセーブ/ロード

******************************************************************************/

#ifdef SAVE_STATE

#include "emumain.h"
#include "zlib/zlib.h"
#include <time.h>


/******************************************************************************
	グローバル変数
******************************************************************************/

char date_str[16];
char time_str[16];
char stver_str[16];
int state_version;
UINT8 *state_buffer;
int current_state_version;
int  state_reload_bios;


/******************************************************************************
	ローカル変数
******************************************************************************/

static const char *current_version_str = "MVS_SV23";


/******************************************************************************
	ステートセーブ/ロード関数
******************************************************************************/

/*------------------------------------------------------
	ステートセーブ
------------------------------------------------------*/

int state_save(int slot)
{
	FILE* fd = NULL;
	char path[MAX_PATH];
	UINT8 *state_buffer_base;
	UINT32 size;

	sprintf(path, "%sstate/%s.sv%d", launchDir, game_name, slot);
	remove(path);

	printf("STATE_SAVING %s.sv%d", game_name, slot);
	if ((fd = fopen(path, "wb")) >= 0)
	{
		state_buffer = state_buffer_base =(UINT8*)malloc(STATE_BUFFER_SIZE);
		if (!state_buffer)
		{
			printf("COULD_NOT_ALLOCATE_STATE_BUFFER\n");
			goto error;
		}
		memset(state_buffer, 0, STATE_BUFFER_SIZE);

		state_save_byte(current_version_str, 8);

		state_save_memory();
		state_save_m68000();
		state_save_z80();
		state_save_input();
		state_save_timer();
		state_save_driver();
		state_save_video();
		state_save_ym2610();
		state_save_pd4990a();

		size = (UINT32)state_buffer - (UINT32)state_buffer_base;
		fwrite(state_buffer_base, 1, size, fd);
		fclose(fd);
		free(state_buffer_base);
		
		return 1;
	}
	else
	{
		printf("COULD_NOT_CREATE_STATE_FILE %s.sv%d\n", game_name, slot);
	}

error:
	if (fd != NULL){
		fclose(fd);
		remove(path);
	}
	return 0;
}


/*------------------------------------------------------
	ステートロード
------------------------------------------------------*/

int state_load(int slot)
{
	FILE *fp;
	char path[MAX_PATH];

	sprintf(path, "%sstate/%s.sv%d", launchDir, game_name, slot);

	state_reload_bios = 0;

	printf("STATE_LOADING %s.sv%d\n", game_name, slot);

	if ((fp = fopen(path, "rb")) != NULL)
	{
		state_load_skip(8);

		state_load_memory(fp);
		state_load_m68000(fp);
		state_load_z80(fp);
		state_load_input(fp);
		state_load_timer(fp);
		state_load_driver(fp);
		state_load_video(fp);
		state_load_ym2610(fp);
		state_load_pd4990a(fp);
		fclose(fp);

		if (state_reload_bios)
		{
			state_reload_bios = 0;

			if (!reload_bios())
			{
				printf("COULD_NOT_RELOAD_BIOS\n");
				Loop = LOOP_EXIT;
				return 0;
			}
		}

		return 1;
	}
	else
	{
		printf("COULD_NOT_OPEN_STATE_FILE %s.sv%d\n", game_name, slot);
	}

	return 0;
}


#endif /* SAVE_STATE */
