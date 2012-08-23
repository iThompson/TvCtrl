#pragma once

/**
 *  Interface for controlling the TV
 */
class ITVController {
public:
    /**
     *  Input selection
     */
    typedef enum {kHD1 = 1, kHD2, kHD3, kHD4, kComp, kVid1, kVid2, kVGA} Input;

    /**
     *  Power off timer constants
     */
    typedef enum {kOff = 1, k30, k60, k90, k120} Sleep;

    /**
     *  Opens the connection to the TV
     *  @return 0 on success, nonzero on failure
     */
    virtual int open() = 0;

    /**
     *  Closes the connection to the TV
     */
    virtual void close() = 0;

    virtual int enableInterfacePower() = 0;
    virtual int disableInterfacePower() = 0;

    virtual int setPower(bool power) = 0;

    virtual int setInput(Input input) = 0;

    virtual int setVolume(int vol) = 0;

    virtual int setMute(bool mute) = 0;

    virtual int setSleep(Sleep sleep) = 0;

    virtual int remoteCode(int code) = 0;
};