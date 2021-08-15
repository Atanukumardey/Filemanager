/*
    Author  Atanu Kumar Dey
            University of Chittagong
            Department of CSE (19th batch)
            atanukumar251@gmail.com
*/

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <SdFat.h>
#include "../Display/Display.h"



#define _UP 1

#define _DOWN 2

#define _ENTER 3

#define _BACKWARD 4

#define _NEW 5

#define _COPY 6

#define _MOVE 7

#define _PASTE 8

#define _RENAME 9

#define _DELETE 10



/*#define _down 2
#define _back 3
#define _enter 4
#define _up 5
#define _new 6
#define _copy 7
#define _move 8
#define _paste 9
#define _rename 10
#define _edit 11
#define _read 12
#define _delete 13*/

#define TypeFile 100
// #define _EDIT 110

#define MAX_File_NAME_SIZE 23
#define SD_CS_PIN 4
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(40))

extern SdFat32 SD;

#ifdef __cplusplus
extern "C++" { 
#endif

bool SDsetup();

uint8_t get_navigationkey();

bool FileManager();

bool Filenameprint(File32 *entryFile);

uint8_t _getFilename(File32 *entryFile, char nameofFile[]);

uint8_t GetButtonState(uint8_t input);

uint8_t FileHandle(File32 *entryFile);

void sdevent_new(File32 *entryFile, bool *_emptyfolder);

uint8_t sdevent_up(uint16_t *Fileposition, File32 *entryFile);

uint8_t sdevent_down(uint16_t *Fileposition, File32 *entryFile, bool *_emptyfolder);

uint8_t sdevent_enter(File32 *entryFile, uint16_t depthcount[], uint8_t *depthcountposition, char *Dirname, uint8_t *Dirdatalength, bool *_emptyfolder);

bool sdevent_back(File32 *entryFile, uint16_t depthcount[], uint8_t *depthcountposition, char *Dirname, uint8_t *Dirdatalength, bool *_emptyfolder);

int8_t sdevent_rename(File32 *entryFile);

bool sdevent_movecopy(File32 *entryFile, File32 *transitionFile, char *Dirname);

bool sdevent_paste(File32 *transitionFile, File32 *entryFile, uint8_t *transitionflag, char *Dirname, uint8_t *Dirdatalength);

int8_t sdevent_delete(File32 *entryFile);

bool Filecopy(File32 *entryFile, File32 *transitionFile, char *namearray);

bool recurse_copy(File32 *entryFile, File32 *transitionFile, char *namearray, char *Dirname, uint8_t *Dirdatalength);

#ifdef __cplusplus
}
#endif

#endif