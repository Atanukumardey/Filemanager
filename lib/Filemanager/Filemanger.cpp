/*
    Author : Atanu Kumar Dey
            University of Chittagong
            Department of CSE (19th batch)
            atanukumar251@gmail.com
*/

#include <Arduino.h>
#include "Filemanager.h"

SdFat32 SD;

bool SDsetup()
{
    return (SD.begin(SD_CONFIG));
}

bool FileManager()
{
    Serial.println(F("FileManager"));
    //Serial.println(FreeStack());
    File32 entryFile, transitionFile;
    char Dirname[150];
    memset(Dirname, '\0', sizeof(Dirname));
    Dirname[0] = '/';
    uint8_t Dirdatalength = 0; // To track the slashposition also the current working string length
    SD.chdir(Dirname);
    uint8_t buttonstate = _DOWN, getbuttonstate = _DOWN, transitionflag = 0;
    uint16_t Fileposition = 0;
    uint16_t depthcount[20];
    uint8_t depthcountposition = 0;
    bool _emptyfolder = false;
    memset(depthcount, 0, sizeof(depthcount));
    while (true)
    {
        buttonstate = GetButtonState(getbuttonstate);
        getbuttonstate = 0;

        switch (buttonstate)
        {
        case _UP:
            getbuttonstate = sdevent_up(&Fileposition, &entryFile);
            break;

        case _DOWN:
            getbuttonstate = sdevent_down(&Fileposition, &entryFile, &_emptyfolder);
            break;

        case _ENTER:
            getbuttonstate = sdevent_enter(&entryFile, depthcount, &depthcountposition, Dirname, &Dirdatalength, &_emptyfolder);
            break;

        case _BACKWARD:
            if (sdevent_back(&entryFile, depthcount, &depthcountposition, Dirname, &Dirdatalength, &_emptyfolder))
                return true;
            break;

        case _NEW:
            sdevent_new(&entryFile, &_emptyfolder);
            break;

        case _COPY:
            sdevent_movecopy(&entryFile, &transitionFile, Dirname);
            transitionflag = 2;
            break;

        case _MOVE:
            sdevent_movecopy(&entryFile, &transitionFile, Dirname);
            transitionflag = 1;
            break;

        case _PASTE:
            sdevent_paste(&transitionFile, &entryFile, &transitionflag, Dirname, &Dirdatalength);
            break;

        case _RENAME:
            sdevent_rename(&entryFile);
            break;

        case _DELETE:
            getbuttonstate = sdevent_delete(&entryFile);
            break;

        default:
            break;
        }
    }
    return true;
}

bool Filenameprint(File32 *entryFile)
{
    char printdata[MAX_File_NAME_SIZE] = {'\0'};
    uint8_t namelength = _getFilename(entryFile, printdata);
    if (!Displaydata(printdata, namelength))
        return false;
    return true;
}

uint8_t _getFilename(File32 *entryFile, char nameofFile[])
{
    return entryFile->getName(nameofFile, MAX_File_NAME_SIZE);
}


uint8_t get_navigationkey(){
      while (true) {
    if (Serial.available()) {
      delay(10);
        char databuffer = (char)Serial.read();
      switch (databuffer) {
        case 'd':
          return _DOWN;
        case 'b':
          return _BACKWARD;
        case 'e':
          return _ENTER;
        case 'u':
          return _UP;
        case 'r':
          return _RENAME;
        case 'w':
          return _DELETE;
        case 'm':
          return _MOVE;
        case 'c':
          return _COPY;
        case 'p':
          return _PASTE;
        case 'n':
          return _NEW;
        default:
          break;
      }
    }
  }
}

uint8_t GetButtonState(uint8_t input)
{
    if (input)
        return input;
    input = get_navigationkey();
    return input;
}

void sdevent_new(File32 *entryFile, bool *_emptyfolder)
{
    char namearray[MAX_File_NAME_SIZE];
    const static char M1[] = "Make Folder";
    const static char M2[] = "Make File";
    const static char M3[] = "Enter Name";
    const static char M4[] = "File already exists";
    const static char M5[] = "Folder already exists";
    bool phase = false;
    uint8_t buttonstate = 0;
    while (1)
    {
        if (phase)
            Displaydata(M1, sizeof(M1) - 1);
        else
            Displaydata(M2, sizeof(M2) - 1);
        buttonstate = GetButtonState(0);
        if (buttonstate == _DOWN)
        {
            phase = !phase;
        }
        else if (buttonstate == _ENTER)
        {
            break;
        }
        else if (buttonstate == _BACKWARD)
        {
            Serial.println("here");
            Filenameprint(entryFile);
            return;
        }
    }
    Displaydata(M3, sizeof(M3) - 1);
    uint8_t i = 0;
    while (1)
    {
        if (Serial.available())
        {
            delay(10);
            while (Serial.available() && i < 22)
            {
                namearray[i++] = (char)Serial.read();
            }
            namearray[i] = '\0';
            break;
        }
    }
    entryFile->close();
    *_emptyfolder = false;
    if (phase)
    {
        if (!entryFile->mkdir(SD.vwd(), namearray))
        {
            Displaydata(M5, sizeof(M5) - 1);
            return;
        }
    }
    else
    {
        i = i > 18 ? 18 : i;
        namearray[i++] = '.';
        namearray[i++] = 't';
        namearray[i++] = 'x';
        namearray[i++] = 't';
        namearray[i] = '\0';
        if (!entryFile->open(namearray, O_WRONLY | O_CREAT | O_EXCL))
        {
            Displaydata(M4, sizeof(M4) - 1);
            return;
        }
    }
    int16_t index = entryFile->dirIndex();
    entryFile->close();
    entryFile->open(SD.vwd(), index, O_RDONLY);
    Displaydata(namearray, i - 1);
    return;
}

uint8_t sdevent_up(uint16_t *Fileposition, File32 *entryFile) //, File *DirFile
{
    *Fileposition = *Fileposition ? --*Fileposition < 2 ? 2 : *Fileposition : (entryFile->dirIndex() - 1);
    entryFile->close();
    entryFile->open(SD.vwd(), *Fileposition, O_RDONLY);
    if ((entryFile->isFile() || entryFile->isSubDir()) && !entryFile->isHidden())
    {
        Filenameprint(entryFile);
        *Fileposition = 0;
    }
    else if (*Fileposition != 2)
        return (_UP);
    else
    {
    }
    return 0;
}

uint8_t sdevent_down(uint16_t *Fileposition, File32 *entryFile, bool *_emptyfolder)
{
    if (*_emptyfolder)
        return 0;
    entryFile->close();
    *Fileposition = 0;
    if (entryFile->openNext(SD.vwd(), O_RDONLY))
    {
        if (entryFile->isHidden())
            return (_DOWN);
        else
            Filenameprint(entryFile);
    }
    else
    {

        SD.vwd()->rewind();
        return (_DOWN);
    }
    return 0;
}

uint8_t sdevent_enter(File32 *entryFile, uint16_t depthcount[], uint8_t *depthcountposition, char *Dirname, uint8_t *Dirdatalength, bool *_emptyfolder)
{
    if (entryFile->isSubDir())
    {
        depthcount[(*depthcountposition)++] = entryFile->dirIndex();
        char Filename[MAX_File_NAME_SIZE];
        uint8_t i = 0;
        _getFilename(entryFile, Filename);
        while (Filename[i] != '\0')
            Dirname[++*Dirdatalength] = Filename[i++];
        Dirname[++*Dirdatalength] = '/';
        //Serial.println(Dirname);
        *SD.vwd() = *entryFile;
        entryFile->close();
        while (true)
        {
            if (entryFile->openNext(SD.vwd(), O_RDONLY))
            {
                if (entryFile->isHidden())
                    continue;
                else
                {
                    Filenameprint(entryFile);
                    *_emptyfolder = false;
                    return 0;
                }
            }
            else
            {
                Displaydata("empty folder", 12);
                *_emptyfolder = true;
                return 0;
            }
        }
    }
    else
    {
        return TypeFile;
    }
    return 0;
}

bool sdevent_back(File32 *entryFile, uint16_t depthcount[], uint8_t *depthcountposition, char *Dirname, uint8_t *Dirdatalength, bool *_emptyfolder)
{
    *_emptyfolder = false;
    if (*Dirdatalength == 0)
    {
        entryFile->close();
        return true;
    }
    Dirname[*Dirdatalength] = '\0';
    while (Dirname[--*Dirdatalength] != '/')
        Dirname[*Dirdatalength] = '\0';
    SD.chdir(Dirname);
    entryFile->close();
    entryFile->open(SD.vwd(), depthcount[--(*depthcountposition)], O_RDONLY);
    Filenameprint(entryFile);
    return false;
}

int8_t sdevent_rename(File32 *entryFile)
{
    char namearray[MAX_File_NAME_SIZE];
    while (true)
    {
        if (Serial.available())
        {
            delay(10);
            uint8_t i = 0;
            while (Serial.available() && i < 22)
            {
                namearray[i++] = (char)Serial.read();
            }
            namearray[i] = '\0';
            break;
        }
    }
    entryFile->rename(namearray);
    uint16_t newindex = entryFile->dirIndex();
    entryFile->close();
    entryFile->open(SD.vwd(), newindex, O_RDONLY);
    Filenameprint(entryFile);
    return 0;
}
bool sdevent_movecopy(File32 *entryFile, File32 *transitionFile, char *Dirname)
{
    const static char M1[] = "transfer error";
    char namearray[MAX_File_NAME_SIZE];
    _getFilename(entryFile, namearray);
    transitionFile->close();
    if (!transitionFile->open(SD.vwd(), namearray, O_RDONLY))
        Displaydata(M1, sizeof(M1) - 1);
    return true;
}

bool sdevent_paste(File32 *transitionFile, File32 *entryFile, uint8_t *transitionflag, char *Dirname, uint8_t *Dirdatalength)
{

    const static char M1[] = "move error";
    const static char M2[] = "copy error";
    const static char M3[] = "File already exists";

    char namearray[MAX_File_NAME_SIZE];
    char thisnamearray[MAX_File_NAME_SIZE];
    uint8_t namesize = _getFilename(transitionFile, namearray);
    for (uint8_t i = 0; i < namesize; i++)
        thisnamearray[i] = namearray[i];

    if (SD.vwd()->exists(namearray))
    {
        Displaydata(M3, sizeof(M3) - 1);
        return false;
    }

    else if (*transitionflag == 1)
    {
        entryFile->close();
        if (!transitionFile->rename(SD.vwd(), namearray))
            Displaydata(M1, sizeof(M1) - 1);
    }
    else if (*transitionflag == 2)
    {
        if (!recurse_copy(entryFile, transitionFile, namearray, Dirname, Dirdatalength))
            Displaydata(M2, sizeof(M2) - 1);
    }
    entryFile->close();
    entryFile->open(thisnamearray, O_RDONLY);
    uint16_t dirindex = entryFile->dirIndex();
    entryFile->close();
    entryFile->open(SD.vwd(), dirindex, O_RDONLY);
    Displaydata(thisnamearray, namesize);
    transitionFile->close();
    return true;
}

int8_t sdevent_delete(File32 *entryFile)
{
    char nameofFile[MAX_File_NAME_SIZE];
    _getFilename(entryFile, nameofFile);
    uint8_t button = _NULL;
    Displaydata("confirm", 7);
    while(true)
    {
        button = GetButtonState(0);
        if (button == _ENTER)
        {
            if (entryFile->isSubDir())
            {
                if (!SD.rmdir(nameofFile))
                {
                    const static char M1[] = "Folder not empty";
                    Displaydata(M1, sizeof(M1) - 1);
                    while(true)
                    {
                        button = GetButtonState(0);
                        if (button == _ENTER)
                            entryFile->rmRfStar();

                        else if(button == _BACKWARD)
                        {
                            Filenameprint(entryFile);
                            return 0;
                        }
                    }
                }
            }
            else
            {
                SD.remove(nameofFile);
            }
        }
        else if(button == _BACKWARD)
        {
            Filenameprint(entryFile);
            return 0;
        }
    }

    Displaydata("Deleted", 7);
    entryFile->close();
    return _DOWN;
}

bool Filecopy(File32 *entryFile, File32 *transitionFile, char *namearray)
{

    const static char M4[] = "read error";
    const static char M5[] = "write error";

    entryFile->close();
    _getFilename(transitionFile, namearray);
    entryFile->open(SD.vwd(), namearray, O_WRONLY | O_CREAT);
    transitionFile->rewind();
    uint8_t buf[255];
    //Serial.println(FreeStack());
    while (1)
    {
        uint8_t n = transitionFile->read(buf, sizeof(buf));
        if (n < 0)
        {
            Displaydata(M4, 11);
            return false;
        }
        else if (n == 0)
        {
            break;
        }
        else if ((int)entryFile->write(buf, n) != n)
        {
            Displaydata(M5, 12);
            return false;
        }
    }
    return true;
}

bool recurse_copy(File32 *entryFile, File32 *transitionFile, char *namearray, char *Dirname, uint8_t *Dirdatalength)
{
    File32 DirFile;

    if (transitionFile->isDir())
    {
        uint8_t thiscount = *Dirdatalength;
        uint8_t i = 0;
        _getFilename(transitionFile, namearray);
        SD.mkdir(namearray);
        while (namearray[i] != '\0')
            Dirname[++*Dirdatalength] = namearray[i++];
        Dirname[++*Dirdatalength] = '/';
        SD.chdir(Dirname);
        while (DirFile.openNext(transitionFile, O_RDONLY))
        {
            if (DirFile.isDir())
            {
                if (!recurse_copy(entryFile, &DirFile, namearray, Dirname, Dirdatalength))
                    return false;
                do
                {
                    Dirname[(*Dirdatalength)--] = '\0';
                } 
                while (*Dirdatalength > thiscount);
                
                SD.chdir(Dirname);
            }
            else
            {
                if (!Filecopy(entryFile, &DirFile, namearray))
                    return false;
            }
            DirFile.close();
        }
    }
    else
    {
        if (!Filecopy(entryFile, transitionFile, namearray))
            return false;
    }
    return true;
}
