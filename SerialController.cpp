#include "StdAfx.h"
#include "SerialController.h"

SerialController::SerialController(const char* portName) : m_hSerial(INVALID_HANDLE_VALUE)
{
    strcpy_s(m_portName, sizeof(m_portName), portName);
    memset(buf, 0, sizeof(buf));
}

SerialController::~SerialController(void)
{
    if (m_hSerial != INVALID_HANDLE_VALUE) close();
}

int SerialController::open(void)
{
    DCB dcbSerial;
    COMMTIMEOUTS timeouts;
    memset(&dcbSerial, 0, sizeof(dcbSerial));
    memset(&timeouts, 0, sizeof(timeouts));

    if (m_hSerial != INVALID_HANDLE_VALUE) return -1; // Already open

    m_hSerial = CreateFile(m_portName,
                           GENERIC_READ | GENERIC_WRITE,
                           0,
                           0,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           0);

    if (m_hSerial==INVALID_HANDLE_VALUE) {
        if (GetLastError()==ERROR_FILE_NOT_FOUND){
            return -1; // Invalid port
        }
        return -1; // Unknown I/O error
    }

    dcbSerial.DCBlength = sizeof(dcbSerial);

    if (!GetCommState(m_hSerial, &dcbSerial)) {
        CloseHandle(m_hSerial);
        m_hSerial = INVALID_HANDLE_VALUE;
        return -1; // Unknown I/O error
    }

    dcbSerial.BaudRate = CBR_9600;
    dcbSerial.ByteSize = 8;
    dcbSerial.StopBits = ONESTOPBIT;
    dcbSerial.Parity = NOPARITY;

    if (!GetCommState(m_hSerial, &dcbSerial)) {
        CloseHandle(m_hSerial);
        m_hSerial = INVALID_HANDLE_VALUE;
        return -1; // Serial config error
    }

    timeouts.ReadIntervalTimeout=50;
    timeouts.ReadTotalTimeoutConstant=50;
    timeouts.ReadTotalTimeoutMultiplier=10;
    timeouts.WriteTotalTimeoutConstant=50;
    timeouts.WriteTotalTimeoutMultiplier=10;

    if (!SetCommTimeouts(m_hSerial, &timeouts)) {
        CloseHandle(m_hSerial);
        m_hSerial = INVALID_HANDLE_VALUE;
        return -1; // Timeout config error
    }

    return 0;
}

void SerialController::close(void)
{
    if (m_hSerial != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hSerial);
        m_hSerial = INVALID_HANDLE_VALUE;
    }
}

int SerialController::enableInterfacePower(void)
{
    setCommand("RSPW");
    setParam(0, '1'); // Enable RS-232 mode

    return writeCommand();
}

int SerialController::disableInterfacePower(void)
{
    setCommand("RSPW");
    setParam(0, '0');

    return writeCommand();
}

int SerialController::setPower(bool power)
{
    setCommand("POWR");
    setParam(0, power ? '1' : '0');

    return writeCommand();
}

int SerialController::setInput(Input input)
{
    int val = (int) input;
    if (val < 1 || val > 8) return -1; // Invalid input
    
    setCommand("IAVD");
    setParam(0, val + 48); // Add 48 to convert to ASCII

    return writeCommand();
}

int SerialController::setVolume(int vol)
{
    char vbuf[3];
    if (vol < 0 || vol > 60) return -1; // Invalid volume
    sprintf_s(vbuf, sizeof(vbuf), "%i", vol);
    
    setCommand("VOLM");
    setParam(0, vbuf[0]);
    setParam(1, vbuf[1]);

    return writeCommand();
}


int SerialController::setMute(bool mute)
{
    setCommand("MUTE");
    setParam(0, mute ? '1' : '0');

    return writeCommand();
}


int SerialController::setSleep(Sleep sleep)
{
    int val = (int) sleep;
    if (val < 1 || val > 8) return -1; // Invalid sleep code
    
    setCommand("OFTM");
    setParam(0, val + 48); // See setInput

    return writeCommand();
}

// Implementation of remoteCode
int SerialController::remoteCode(int code)
{
    char cbuf[3];
    if (code < 0 || code > 99) return -1; // Invalid code, spec only allows 2 chars
    sprintf_s(cbuf, sizeof(cbuf), "%i", code);

    setCommand("RCKY");
    setParam(0, cbuf[0]);
    setParam(1, cbuf[1]);

    return writeCommand();
}

// Implementation of setCommand
int SerialController::setCommand(char* cmd)
{
    if (cmd[4] != 0) return -1;
    memcpy(buf, cmd, 4);
    // Clear parameters
    memset(buf+4, 0, 4);
    return 0;
}

// Implementation of setParam
int SerialController::setParam(UINT8 param, char val)
{
    if (param > 3) return -1;
    buf[param+4] = val; // Parameters start on the 5th byte
}

// Implementation of writeCommand
int SerialController::writeCommand()
{
    DWORD numWritten = 0;
    buf[9] = 0x0D; // Always end with a CR

    if (m_hSerial == INVALID_HANDLE_VALUE) return -1; // Not initialized
    WriteFile(m_hSerial, buf, 9, &numWritten, NULL);
    if (numWritten != 9) return -1; // IO Error
    return checkOK(); // TODO: proper return codes
}

// Implementation of checkOK
int SerialController::checkOK()
{
    return -1;
}

