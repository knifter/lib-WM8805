#ifndef WM8805_H
#define WM8805_H

#include <Arduino.h>

#include <TwoWireDevice.h>


#define WM8805_ADDRESS_DEFAULT              (0x3A)

class WM8805: public TwoWireDevice
{
public:
	typedef enum : byte
	{
		RATE_192K = 0x00,
		RATE_96K  = 0x10,
		RATE_48K  = 0x20,
		RATE_32K  = 0x30,
		RATE_UNLOCKED = 0xFF
	} SampleRate;

	WM8805(TwoWire *wire, const uint8_t addr = WM8805_ADDRESS_DEFAULT) : TwoWireDevice(wire, addr) {};
    WM8805(const uint8_t addr = WM8805_ADDRESS_DEFAULT) : TwoWireDevice(addr) {};

    bool begin();
	void powerUp();
	void powerDown();
	void printStatus(bool print_int = false);

    // status
	bool isLocked();
	bool isAudio();
	bool isDeemph();
	byte getSampleRate();
	byte getChanSampleRate();
	
	// actions
	bool handleInterrupt();

	bool selectInput(byte num);
	void autoSelectInput();

	byte input;
	// bool configure_pll();

	// bool isValid = false;
	// bool isLocked = false;
private:
	void set_pll(byte pll_n = 7, unsigned long pll_k = 0x36FD21);
	void set_enable192K(bool enabled);

	byte _spdstat = 0;
	byte _intstat = 0;

};

#endif // WM8805_H
