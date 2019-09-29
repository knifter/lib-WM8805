#ifndef WM8805_H
#define WM8805_H

#include <Arduino.h>
#include <stdint.h>

#include <TwoWireDevice.h>


#define WM8805_ADDRESS_DEFAULT              (0x3A)

class WM8805: public TwoWireDevice
{
public:
	typedef enum : uint8_t
	{
		RATE_192K = 0x00,
		RATE_96K  = 0x10,
		RATE_48K  = 0x20,
		RATE_32K  = 0x30,
		RATE_UNLOCKED = 0xFF
	} samplerate_t;

	WM8805(TwoWire& wire, const uint8_t addr = WM8805_ADDRESS_DEFAULT) : TwoWireDevice(wire, addr) {};
    WM8805(const uint8_t addr = WM8805_ADDRESS_DEFAULT) : TwoWireDevice(addr) {};

    bool begin();
	uint8_t getRevision();
	void powerUp();
	void powerDown();
	void printStatus(bool print_int = false);

    // status
	bool isLocked();
	bool isAudio();
	bool isDeemph();
	samplerate_t getSampleRate();
	uint8_t getChanSampleRateKHz();
	
	// actions
	bool handleInterrupt();

	bool selectInput(uint8_t num);
	void autoSelectInput();

	uint8_t input;

private:
	void set_pll(uint8_t pll_n = 7, unsigned long pll_k = 0x36FD21);
	void set_enable192K(bool enabled);

	uint8_t _spdstat = 0;
	uint8_t _intstat = 0;

};

#endif // WM8805_H
