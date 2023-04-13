/** 
 *@section DESCRIPTION
 * RCSwitch - Ported from the Arduino libary for remote control outlet switches
 *  Contributors:
 * - Andre Koehler / info(at)tomate-online(dot)de
 * - Gordeev Andrey Vladimirovich / gordeev(at)openpyro(dot)com
 * - Skineffect / http://forum.ardumote.com/viewtopic.php?f=2&t=46
 * - Dominik Fischer / dom_fischer(at)web(dot)de
 * - Frank Oltmanns / <first name>.<last name>(at)gmail(dot)com
 * - Chris Dick / Porting to mbed
 *
 * Project home: http://code.google.com/p/rc-switch/
 * @section LICENSE
 * Copyright (c) 2011 Suat Özgür.  All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * @file "RCSwitch.h"
 */
#ifndef _RCSwitch_h
#define _RCSwitch_h
#include "mbed.h"

// might need to change this...
// We can handle up to (unsigned long) => 32 bit * 2 H/L changes per bit + 2 for sync
#define RCSWITCH_MAX_CHANGES 67 /**< Number of maximum High/Low changes per packet. */

#define PROTOCOL3_SYNC_FACTOR   71 /**< Protocol 3 Sync Factor */
#define PROTOCOL3_0_HIGH_CYCLES  4 /**< Protocol 3 number of high cycles in a 0 */
#define PROTOCOL3_0_LOW_CYCLES  11 /**< Protocol 3 number of low cycles in a 0*/
#define PROTOCOL3_1_HIGH_CYCLES  9 /**< Protocol 3 number of high cycles in a 1*/
#define PROTOCOL3_1_LOW_CYCLES   6 /**< Protocol 3 number of low cycles in a 1*/
/** RCSwitch Class 
 *
 * Example:
 * @code
 * #include "mbed.h"
 * #include "RCSwitch.h"
 *
 * // This Example should only do one of either transmit or receive
 * //#define TRANSMIT
 * #define RECEIVE
 *
 * Serial pc(USBTX, USBRX); // tx, rx
 * RCSwitch mySwitch = RCSwitch( p11, p21 ); //tx, rx
 *
 * int main()
 * {
 *     pc.printf("Setup");
 *     while(1) {
 *         #ifdef RECEIVE
 *         if (mySwitch.available()) {
 *
 *             int value = mySwitch.getReceivedValue();
 *
 *             if (value == 0) {
 *                 pc.printf("Unknown encoding");
 *             } else {
 *                 pc.printf("Received %d \n\r", mySwitch.getReceivedValue());
 *                 pc.printf(" bit %d \n\r", mySwitch.getReceivedBitlength());
 *                 pc.printf(" Protocol: %d \n\r", mySwitch.getReceivedProtocol());
 *             }
 *             mySwitch.resetAvailable();
 *         }
 *         #endif
 *         #ifdef TRANSMIT
 *         // Example: TypeA_WithDIPSwitches
 *         mySwitch.switchOn("11111", "00010");
 *         wait(1);
 *         mySwitch.switchOn("11111", "00010");
 *         wait(1);
 *
 *         // Same switch as above, but using decimal code
 *         mySwitch.send(5393, 24);
 *         wait(1);
 *         mySwitch.send(5396, 24);
 *         wait(1);
 *
 *         // Same switch as above, but using binary code
 *         mySwitch.send("000000000001010100010001");
 *         wait(1);
 *         mySwitch.send("000000000001010100010100");
 *         wait(1);
 *
 *         // Same switch as above, but tri-state code
 *         mySwitch.sendTriState("00000FFF0F0F");
 *         wait(1);
 *         mySwitch.sendTriState("00000FFF0FF0");
 *         wait(1);
 *         #endif
 *     }
 * }
 *
 * @endcode
 */
 
 /**
 *
 *
 */
class RCSwitch {

  public:
 /** Class constructor.
 * The constructor assigns the specified pinout, attatches 
 * an Interrupt to the receive pin. for the LPC1768 this must not 
 * be pins 19 and 20. For the KL25Z, the pin must be on ports A or C
 * @param tx Transmitter pin of the RF module.
 * @param rx Receiver pin of the RF module.
 */
    RCSwitch(PinName tx, PinName rx);
 /** Class constructor.
 * The constructor assigns the specified pinout, attatches 
 * an Interrupt to the receive pin. for the LPC1768 this must not 
 * be pins 19 and 20. For the KL25Z, the pin must be on ports A or C
 * @param tx    Transmitter pin of the RF module.
 * @param rx    Receiver pin of the RF module.
 * @param tx_en Enable pin of the transmitter
 */
    RCSwitch(PinName tx, PinName rx, PinName rx_en);
/**
 * Set protocol to be used in transmission
 * @param nProtocol Protocol type ot transmit
 */  
    void setProtocol(int nProtocol);
/**
 * Set protocol to be used in transmission
 * @param nProtocol Protocol type ot transmit
 * @param nPulseLength Length of each pulse
 */  
    void setProtocol(int nProtocol, int nPulseLength);
/**
 * Switch a remote switch on (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param sDevice       Code of the switch device (refers to DIP switches 6..10 (A..E) where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 */
    void switchOn(char* sGroup, char* sDevice);
/**
 * Switch a remote switch off (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param sDevice       Code of the switch device (refers to DIP switches 6..10 (A..E) where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 */
    void switchOff(char* sGroup, char* sDevice);
/**
 * Deprecated, use switchOn(char* sGroup, char* sDevice) instead!
 * Switch a remote switch on (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param nChannelCode  Number of the switch itself (1..5)
 */
    void switchOn(char* sGroup, int nChannelCode);
/**
 * Deprecated, use switchOff(char* sGroup, char* sDevice) instead!
 * Switch a remote switch off (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param nChannelCode  Number of the switch itself (1..5)
 */
    void switchOff(char* sGroup, int nChannelCode);
/**
 * Switch a remote switch on (Type B with two rotary/sliding switches)
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 */
    void switchOn(int nAddressCode, int nChannelCode);
/**
 * Switch a remote switch off (Type B with two rotary/sliding switches)
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 */
    void switchOff(int nAddressCode, int nChannelCode);
/**
 * Switch a remote switch on (Type C Intertechno)
 *
 * @param sFamily  Familycode (a..f)
 * @param nGroup   Number of group (1..4)
 * @param nDevice  Number of device (1..4)
  */
    void switchOn(char sFamily, int nGroup, int nDevice);
/**
 * Switch a remote switch off (Type C Intertechno)
 *
 * @param sFamily  Familycode (a..f)
 * @param nGroup   Number of group (1..4)
 * @param nDevice  Number of device (1..4)
 */
    void switchOff(char sFamily, int nGroup, int nDevice);

/**
 * Switch a remote switch off (Type D REV)
 *
 * @param sGroup        Code of the switch group (A,B,C,D)
 * @param nDevice       Number of the switch itself (1..3)
 */
    void switchOn(char sGroup, int nDevice);
/**
 * Switch a remote switch on (Type D REV)
 *
 * @param sGroup        Code of the switch group (A,B,C,D)
 * @param nDevice       Number of the switch itself (1..3)
 */  
    void switchOff(char sGroup, int nDevice);
/**
 * Sends a codeword
 * @param sCodeWord   Codeword to be sent
 */
    void sendTriState(char* Code);
/**
 * Converts a CodeWord to a set Length and sends it
 * @param Code   CodeWord to be sent
 * @param length Length of CodeWord to send
 */
    void send(unsigned long Code, unsigned int length);
/**
 * Sends a CodeWord 
 * @param Code CodeWord to send
 */
    void send(char* Code);
/**
 * Enable receiving data This clear message storage 
 * and enables the interrupt, which may enable the port
 */
    void enableReceive();
/**
 * Disable receiving data This disables the interrupt
 * which may disable the port
 */
    void disableReceive();
/**
 * Message availiable
 * @return bool Message availiability
 */
    bool available();
/**
 * Clear Messages
 */
    void resetAvailable();
/**
 * Get Message Value
 * @return unsigned long The value of the message received
 */
    unsigned long getReceivedValue();
/**
 * Get bit length
 * @return unsigned int Number of bits received
 */
    unsigned int getReceivedBitlength();
/**
 * Get the delay 
 * @Return unsigned int The delay
 */
    unsigned int getReceivedDelay();
/**
 * Get Protocol
 * @return unsigned int The protocol used in the message
 */
    unsigned int getReceivedProtocol();
/**
 * Get Raw data
 * @return unsinged int The raw data of the message recieved
 */
    unsigned int* getReceivedRawdata();
/**
 * Enable the transmitter
 */  
    void enableTransmit();
/**
 * Disable the transmitter
 */
    void disableTransmit();
/**
 *  Set pulse length in micro seconds
 * @param nPulseLength the Length of the pulse
 */  
    void setPulseLength(int nPulseLength);
/**
 * Set number of times to repeat transmission
 * @param nRepeat Number of repeats
 */  
    void setRepeatTransmit(int nRepeatTransmit);
/**
 * Set receive tolerance
 * @param nPercent Percentage tolerance of the receiver
 */  
    void setReceiveTolerance(int nPercent);

    static int nReceiveTolerance;           /**< Tolerance of the receiver  */ 
    static unsigned long nReceivedValue;    /**< Value Recieved  */
    static unsigned int nReceivedBitlength; /**< Length in bits of value reveived  */
    static unsigned int nReceivedDelay;     /**< Delay in receive  */
    static unsigned int nReceivedProtocol;  /**< Protocol of message recieved  */
    static bool ReceiveEnabled;             /**< Receive enabled  */
    static bool TransmitEnable;             /**< Transmit enabled  */
    static bool TransmitEnablePin;          /**< Pin of transmitter enable pin  */
    static unsigned int timings[RCSWITCH_MAX_CHANGES]; /**< timings[0] contains sync timing, followed by a number of bits */

  private:
    DigitalOut _tx;
    InterruptIn _rx;
    DigitalOut _tx_en;
    
    char* getCodeWordB(int nGroupNumber, int nSwitchNumber, bool bStatus);

    char* getCodeWordA(char* sGroup, int nSwitchNumber, bool bStatus);

    char* getCodeWordA(char* sGroup, char* sDevice, bool bStatus);

    char* getCodeWordC(char sFamily, int nGroup, int nDevice, bool bStatus);

    char* getCodeWordD(char group, int nDevice, bool bStatus);

    void sendT0();

    void sendT1();

    void sendTF();

    void send0();

    void send1();

    void sendSync();

    void transmit(int nHighPulses, int nLowPulses);

    void RCSwitchRxPinChange();

    static char* dec2binWzerofill(unsigned long dec, unsigned int length);

    static char* dec2binWcharfill(unsigned long dec, unsigned int length, char fill);
    
    static void handleInterrupt();

    static bool receiveProtocol1(unsigned int changeCount);

    static bool receiveProtocol2(unsigned int changeCount);

    static bool receiveProtocol3(unsigned int changeCount);

    int nReceiverInterrupt;
    int nTransmitterPin;
    int nPulseLength;
    int nRepeatTransmit;
    char nProtocol;
    Timer timer;
};

#endif
