#include "wm8805.h"

#include <Arduino.h>
#include <TwoWireDevice.h>

#include "wm8805_reg.h"

#define DBG(x)          			Serial.print(x);
#define DBGLN(x)					Serial.println(x);

bool WM8805::begin()
{
    // if(!I2CDevice::init(0x3A))
    // 	return false;
	TwoWireDevice::begin();

 	writereg8(0x00, 0x00); // reset

	// Check that Wim is there
	uint8_t devid1 = readreg8(REG_DEVID1);
	uint8_t devid2 = readreg8(REG_DEVID2);
	if(devid1 != 0x05 || devid2 != 0x88)
	{
		// DBG("WM8805 Device not found or wrong response: 0x");
		Serial.println((uint16_t) devid1 << 8 | devid2, HEX);
		return false;
	}
	// uint8_t rev = readreg8(REG_DEVID3);
	// DBG("WN8805 Revision ");
	// DBGLN(rev);

    set_pll(); // PLL1-4
    writereg8(REG_PLL5, REG_PLL5_INIT);
    writereg8(REG_PLL6, REG_PLL6_INIT);
	writereg8(REG_SPDMODE, REG_SPDMODE_INIT);
    writereg8(REG_INTMASK, REG_INTMASK_INIT);
    writereg8(REG_AIFTX, REG_AIFTX_INIT);
    writereg8(REG_AIFRX, REG_AIFRX_INIT);
    writereg8(REG_GPO01, REG_GPO01_INIT);
    writereg8(REG_GPO23, REG_GPO23_INIT);
    writereg8(REG_GPO45, REG_GPO45_INIT);
    writereg8(REG_GPO67, REG_GPO67_INIT);
    writereg8(REG_SPDRX1, REG_SPDRX1_INIT);
    writereg8(REG_PWRDN, REG_PWRDN_POWERDOWN);

    return true;
}

uint8_t WM8805::getRevision()
{
	return readreg8(REG_DEVID3);
}

void WM8805::powerDown()
{
	writereg8(REG_PWRDN, REG_PWRDN_POWERDOWN);
}

void WM8805::powerUp()
{
	writereg8(REG_PWRDN, REG_PWRDN_POWERUP);
}

bool WM8805::selectInput(uint8_t num)
{
	input = num;
	writereg8(REG_PLL6, REG_PLL6_CFG | (input & PLL6_RXINSEL_MASK));
	return true;
}

bool WM8805::handleInterrupt()
{
	_intstat = readreg8(REG_INTSTAT);
	_spdstat = readreg8(REG_SPDSTAT);

	// nothing to do if there were no interrupts
	if(!_intstat)
		return false;

	// Samplerate changed?
	if(_intstat & INT_UPD_REC_FREQ)
	{
		// DBG("INT:UPD_FREQ\n");
		switch(_spdstat & SPDSTAT_FREQ_MASK)
   		{
        	case RATE_192K: // Mode 1: 192K R = 8.192
				set_enable192K(true);
	        	set_pll(PLL_N_MODE1, PLL_K_MODE1);
	        	break;
	        case RATE_96K: // Mode 2: 96K
	        case RATE_48K: // Mode 3: 48K R = 7.675
	        case RATE_32K: // Mode 4: 32K
				set_enable192K(false);
	        	set_pll(PLL_N_MODE234, PLL_K_MODE234);
	        	break;
	    };
	    // return true;
	};

	// // FIXME, click!
	// static int cnt = 0;
	// cnt++;
	// if(cnt == 2)
	// {
	// 	delay(300);
	// 	DBG("*WAIT*\n");
	// };

	// Got locked/unlocked?
	if(_intstat & INT_UPD_UNLOCK)
	{
		// DBG("INT:UPD_UNLOCK")
		if(!isLocked())
		{
			// DBG("=(unlocked)\n");
		}else{
			// DBG("=(locked)\n");
		}
	};

#ifdef WM8805_DEBUG
	// masked
	if(_intstat & INT_INVALID)
		DBG("INT:INVALID\n");

    if(_intstat & INT_CSUD)
        DBG("INT:CSUD\n");
    if(_intstat & INT_TRANS_ERR)
        DBG("INT:TRANS_ERR\n");
    if(_intstat & INT_UPD_CPY_N)
        DBG("INT:UPD_CPY_N\n");
    if(_intstat & INT_UPD_DEEMPH)
        DBG("INT:UPD_DEEMPH\n");
    if(_intstat & INT_UPD_NON_AUDIO)
	{
        DBG("INT:UPD_NON_AUDIO\n");
		// return false;
	};
#endif

	return true;
}

void WM8805::set_enable192K(bool enabled)
{
	writereg8(REG_SPDRX1, REG_SPDRX1_CFG | enabled ? SPDRX1_SPD_192_EN : 0);
}

void WM8805::set_pll(uint8_t pll_n, unsigned long pll_k)
{
#ifdef WM8805_DEBUG
	Serial.print("set_pll(");
	Serial.print(pll_n, HEX);
	Serial.print(", ");
	Serial.print(pll_k, HEX);
	Serial.println(")");
#endif

    // writereg8(REG_PWRDN, 0x07);
	writereg8(REG_PLL1, (pll_k >> 0) & 0x0000FF); // K[7:0]
	writereg8(REG_PLL2, (pll_k >> 8) & 0x0000FF); // K[15:8]
	writereg8(REG_PLL3, (pll_k >> 16) & 0x0000FF); // K[24:16]
	writereg8(REG_PLL4, REG_PLL4_CFG | (pll_n & PLL4_N_MASK));
    // writereg8(REG_PWRDN, 0x04);
}

bool WM8805::isLocked()
{
	return !(_spdstat & SPDSTAT_UNLOCK);
}

bool WM8805::isAudio()
{
	return !(_spdstat & SPDSTAT_AUDIO_N);
}

bool WM8805::isDeemph()
{
	return !(_spdstat & SPDSTAT_DEEMPH);
}

WM8805::samplerate_t WM8805::getSampleRate()
{
	if(_spdstat & SPDSTAT_UNLOCK)
		return RATE_UNLOCKED;
	return (samplerate_t)(_spdstat & SPDSTAT_FREQ_MASK);
}

void WM8805::printStatus(bool print_int /* = false */)
{
    DBGLN("\nWM8805.printStatus:");

    if(print_int)
    {
	    DBG(" INT: ");
	    uint8_t in = readreg8(REG_INTSTAT);
	    if(in & INT_UPD_REC_FREQ)
	        DBG("UPD_REC_FREQ ");
	    if(in & INT_UPD_DEEMPH)
	        DBG("UPD_DEEMPH ");
	    if(in & INT_UPD_CPY_N)
	        DBG("UPD_CPY_N ");
	    if(in & INT_UPD_NON_AUDIO)
	        DBG("UPD_NON_AUDIO ");
	    if(in & INT_TRANS_ERR)
	        DBG("INT_TRANS_ERR ");
	    if(in & INT_CSUD)
	        DBG("INT_CSUD ");
	    if(in & INT_INVALID)
	        DBG("INT_INVALID ");        
	    if(in & INT_UPD_UNLOCK)
	        DBG("UPD_UNLOCK ");
	};
	
    DBG(" SPDSTAT: ");
    uint8_t st = readreg8(REG_SPDSTAT);
    if(st & SPDSTAT_UNLOCK) {
		DBG("UNLOCK ");
	}else{
		DBG("LOCK "); 
	};
    switch(st & SPDSTAT_FREQ_MASK)
    {
        case SPDSTAT_FREQMODE_192K: /*00*/ DBG("192K "); break;
        case SPDSTAT_FREQMODE_96K:  /*01*/ DBG("96K "); break;
        case SPDSTAT_FREQMODE_48K:  /*10*/ DBG("48K "); break;
        case SPDSTAT_FREQMODE_32K:  /*11*/ DBG("32K "); break;
    };
    if(st & SPDSTAT_DEEMPH)
        DBG("DEEMPH ");
    if(st & SPDSTAT_CPY_N)
        DBG("CPY_N ");
    if(st & SPDSTAT_PCM_N)
        DBG("PCM_N ");        
    if(st & SPDSTAT_AUDIO_N)
        DBG("AUDIO_N ");

    DBGLN("");
}

// Returns the samplerate specified in the channel (stream)
uint8_t WM8805::getChanSampleRateKHz()
{
	if(!isLocked())
		return 0;

	uint8_t rx = readreg8(REG_RXCHAN4);
    switch(rx & 0x0F)
    {
        case 0:
          return 44;
        case 1:
          return 0;
        case 2:
          return 48;
        case 3:
          return 32;
        case 4:
          return 22;
        case 6:
          return 24;
        case 8:
          return 88;
        case 10:
          return 96;
        case 12:
          return 176;
        case 14:
          return 192;
        default:
          return 0;
    }
}
