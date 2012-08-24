#pragma once
#include <windows.h>
#include "ITVController.h"

class SerialController :
    public ITVController
{
public:
    SerialController(const TCHAR* portName);
    ~SerialController(void);

    int open();
    void close();

    int enableInterfacePower();
    int disableInterfacePower();

    int setPower(bool power);

    int setInput(Input input);

    int setVolume(int vol);
    int setMute(bool mute);

    int setSleep(Sleep sleep);

    int remoteCode(int code);

private:
    HANDLE m_hSerial;
    TCHAR m_portName[10];
    char m_buf[10]; // 9 bytes of data and 1 null

    int setCommand(char* cmd);
    int setParam(UINT8 param, char val);
    int writeCommand();
    int checkOK();
};
