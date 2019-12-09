/******************************************************************************

	biosmenu.c

	MVS BIOS選択メニュー

******************************************************************************/

#include "mvs.h"


/******************************************************************************
	グローバル変数
******************************************************************************/

//const char *bios_name[BIOS_MAX] =
const char *bios_name[] =
{
	"Europe MVS (Ver. 2)",
	"Europe MVS (Ver. 1)",
	"US MVS (Ver. 2)",
	"US MVS (Ver. 1)",
	"Asia MVS New (Ver. 3)",/* Latest Asia bios */
	"Asia MVS (Ver. 3)",
	"Japan MVS (J3)",/* Latest Japan bios; correct chip label unknown */
	"Japan MVS (Ver. 3)",
	"Japan MVS (Ver. 2)",
	"Japan MVS (Ver. 1)",

	"Neo Geo Git(Ver. 1.1)",/* 'rare MVS found in japanese hotels' shows v1.3 in test mode */
	"Asia AES",
	"Japan AES",

	"Unibios MVS (Hack, Ver. 3.2)",
	"Unibios MVS (Hack, Ver. 3.1)",
	"Unibios MVS (Hack, Ver. 3.0)",
	"Unibios MVS (Hack, Ver. 2.3)",
	"Unibios MVS (Hack, Ver. 2.2)",
	"Unibios MVS (Hack, Ver. 2.1)",
	"Unibios MVS (Hack, Ver. 2.0)",
	"Unibios MVS (Hack, Ver. 1.3)",
	"Unibios MVS (Hack, Ver. 1.2)",
	"Unibios MVS (Hack, Ver. 1.1)",
	"Unibios MVS (Hack, Ver. 1.0)",
	"Debug MVS (Hack?)"
};

//const UINT32 bios_crc[BIOS_MAX] =
const UINT32 bios_crc[] =
{
	0x9036d879,	// Europe Ver.2
	0xc7f2fa45,	// Europe Ver.1
	0xe72943de,	// USA Ver.2
	0x2723a5b5,	// USA Ver.1
	0x03cc9f6a,	// Asia MVS NEW(Ver. 3)
	0x91b64be3,	// Asia Ver.3
	0xdff6d41f,	// Japan MVS (J3)
	0xf0e8f27d,	// Japan Ver.3
	0xacede59c,	// Japan Ver.2
	0x9fb0abe4,	// Japan ver.1

	0x162f0ebe,	// NEO GEO GIT Custom Japanese Hotel
	0xd27a71f1,	// Asia AES
	0x16d0c132,	// Japan AES

	0xa4e8b9b3,	// Unibios V3.2
	0x0c58093f,	// Unibios V3.1
	0xa97c89a9,	// Unibios V3.0
	0x27664eb5,	// Unibios V2.3
	0x2d50996a,	// Unibios V2.2
	0x8dabf76b,	// Unibios V2.1
	0x0c12c2ad,	// Unibios V2.0
	0xb24b44a0,	// Unibios V1.3
	0x4fa698e9,	// Unibios V1.2
	0x5dda0d84,	// Unibios V1.1
	0x0ce453a0,	// Unibios V1.0
	0x698ebb7d	// Debug BIOS
};

//const UINT32 bios_patch_address[BIOS_MAX] =
const UINT32 bios_patch_address[] =
{
	0x011c62,	// Europe Ver.2
	0x011c62,	// Europe Ver.1
	0x011c38,	// USA Ver.2
	0x011c38,	// USA Ver.1
	0x010d86,	// Asia MVS NEW(Ver. 3)
	0x010c62,	// Asia Ver.3
	0x010d86,	// Japan MVS (J3)
	0x011d8a,	// Japan Ver.3
	0x011c62,	// Japan Ver.2
	0x011c62,	// Japan Ver.1

	0x000000,	// NEO GEO GIT Custom Japanese Hotel
	0x000000,	// Asia AES
	0x000000,	// Japan AES

	0x000000,	// Unibios V3.2
	0x000000,	// Unibios V3.1
	0x000000,	// Unibios V3.0
	0x000000,	// Unibios V2.3
	0x000000,	// Unibios V2.2
	0x000000,	// Unibios V2.1
	0x000000,	// Unibios V2.0
	0x000000,	// Unibios V1.3
	0x000000,	// Unibios V1.2
	0x000000,	// Unibios V1.1
	0x000000,	// Unibios V1.0
	0x000000	// Debug BIOS
};


const UINT32 sfix_crc  = 0xc2ea0cfd;//old 0x354029fc
const UINT32 lorom_crc = 0x5a86cff2;//old 0xe09e253c

const char *bios_zip   = "neogeo";
const char *sfix_name  = "sfix.sfx";
const char *lorom_name = "000-lo.lo";


/******************************************************************************
	ローカル変数
******************************************************************************/

static UINT8 bios_exist[BIOS_MAX];


/******************************************************************************
	ローカル関数
******************************************************************************/

/*------------------------------------------------------
	エラーメッセージ表示
------------------------------------------------------*/

static void bios_error(const char *rom_name, int error)
{
	char mes[128];

	zip_close();

	if (error == -2)
		sprintf(mes, "CRC32_NOT_CORRECT:%s", rom_name);
	else
		sprintf(mes, "FILE_NOT_FOUND:%s", rom_name);

    printf("%s\n",mes);
}


/*------------------------------------------------------
	BIOS存在チェック
------------------------------------------------------*/

static int bios_check(int idx)
{
	int i, err, count = 0, check_max = DEBUG_BIOS;
	char *fname;
#if 0
	for (i = 0; i < BIOS_MAX; i++)
		bios_exist[i] = 0;

	for (i = 0; i <= check_max; i++)
	{
		if (file_open(bios_zip, NULL, bios_crc[i], NULL) >= 0)
		{
			count++;
			bios_exist[i] = 1;
			file_close();
		}
	}

	if (count == 0)
	{
        printf("BIOS_NOT_FOUND");
		return 0;
	}
#else
    if((err = file_open(bios_zip, NULL, bios_crc[idx], NULL)) >= 0)
    {
        file_close();
    }else{
        printf("BIOS_NOT_FOUND err = %d\n",err);
        return 0;
    }
#endif

    fname = (char *)sfix_name;
    //if ((err = file_open(bios_zip, NULL, sfix_crc, fname)) >= 0)
    if ((err = file_open(bios_zip, NULL, sfix_crc, NULL)) >= 0)
    {
        file_close();
	}
	else
	{
		bios_error(sfix_name, err);
		return 0;
	}

	fname = (char *)lorom_name;
	//if ((err = file_open(bios_zip, NULL, lorom_crc, fname)) >= 0)
	if ((err = file_open(bios_zip, NULL, lorom_crc, NULL)) >= 0)
	{
		file_close();
	}
	else
	{
		bios_error(lorom_name, err);
		return 0;
	}

	return 1;
}


/******************************************************************************
	グローバル関数
******************************************************************************/

/*------------------------------------------------------
	BIOS選択メニュー
------------------------------------------------------*/

void bios_select()
{
	int old_bios = neogeo_bios;

    //neogeo_bios = 5;//"Asia MVS (Ver. 3)"
    neogeo_bios = 12;//"Japan AES"

    printf("neogeo_bios = %d\n",neogeo_bios);
	if (!bios_check(neogeo_bios)){
        neogeo_bios = -1;
        return;
    }

    if (old_bios != neogeo_bios)
    {
        printf("ALL_NVRAM_FILES_ARE_REMOVED\n");
        //delete_files("nvram", ".nv");
    }
}
