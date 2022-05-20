/*
 * cmd.h
 *
 *  Created on: 2021. 3. 2.
 *      Author: GP01
 */
#include "fatfs/src/ff.h"
#include "common.h"

#ifndef HEADER_CMD_H_
#define HEADER_CMD_H_

#define PATH_BUF_SIZE   80

#define CMD_BUF_SIZE    64

#define CMD_DW_ERR_FILESIZE   1
#define CMD_DW_ERR_TIMEOUT    2
#define CMD_DW_NORMAL         3

int Cmd_help(int argc, char *argv[]);
int Cmd_download(int argc, char *argv[]);
int Cmd_flash(int argc, char *argv[]);
int Cmd_ls(int argc, char *argv[]);
int Cmd_cat(int argc, char *argv[]);
int Cmd_rm(int argc, char *argv[]);
int Cmd_jump(int argc, char *argv[]);
int Cmd_upload(int argc, char *argv[]);
int Cmd_sd(int argc, char *argv[]);
int Cmd_info(int argc, char *argv[]);
int Cmd_filev(int argc, char *argv[]);
int Cmd_Verify(int argc, char *argv[]);
int Cmd_Erase(int argc, char *argv[]);
int Cmd_Eeprom(int argc, char *argv[]);

int ReadLine();

void Cmd_bootflagGet(void);



extern Uint32 entryAddress;
extern FATFS g_sFatFs;
extern FIL g_sFileObject;
extern unsigned short usBytesRead;
extern FRESULT fresult;



#endif /* HEADER_CMD_H_ */
