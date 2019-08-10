#include "wm8805.h"

#include <Arduino.h>
#include <TwoWireDevice.h>

#define REG_DEVID1					0x00
#define REG_DEVID2					0x01
#define REG_DEVID3					0x02

#define REG_PLL1					0x04	// PLL K[7:0]
#define REG_PLL2					0x05	// PLL K[15:8]
#define REG_PLL3					0x06	// PLL K[21:16]

#define REG_PLL4					0x06
	#define PLL4_N_MASK				0x0F
	#define PLL4_PRESCALE			0x10
	#define PLL4_SD_MASK				0x60
	#define PLL4_OVWR				0x80
#define REG_PLL4_CFG				0x00
#define REG_PLL4_INIT				(REG_PLL4_CFG)

#define REG_PLL5					0x07
	#define PLL5_FREQMODE_MASK		0x03
		#define FREQMODE_00			0x00
		#define FREQMODE_01			0x01
		#define FREQMODE_10			0x02
		#define FREQMODE_11			0x03
	#define PLL5_FRACEN				0x04
	#define PLL5_MCLKDIV			0x08
	#define PLL5_CLKOUTDIV_MASK		0x30
#define REG_PLL5_INIT				(PLL5_FRACEN | FREQMODE_10)

#define REG_PLL6					0x08
	#define PLL6_RXINSEL_MASK		0x07
	#define PLL6_CLKOUTSRC			0x80
	#define PLL6_CLKOUTDIS			0x10
	#define PLL6_FILLMODE			0x20	// 1=Write '0's if when INVALID/TRANS_ERR and they are masked (p43)
	#define PLL6_ALWSVALID			0x40	// 
	#define PLL6_MCLKSRC			0x80
#define REG_PLL6_CFG				(PLL6_FILLMODE)
#define REG_PLL6_INIT				(REG_PLL6_CFG)

#define REG_SPDMODE					0x09
#define REG_SPDMODE_INIT			0x00 // CMOS Input levels

#define REG_INTSTAT					0x0B
#define REG_INTMASK					0x0A	// Mask[7:0]
	#define INT_UPD_UNLOCK			0x01
	#define INT_INVALID				0x02
	#define INT_CSUD				0x04
	#define INT_TRANS_ERR			0x08
	#define INT_UPD_NON_AUDIO		0x10
	#define INT_UPD_CPY_N			0x20
	#define INT_UPD_DEEMPH			0x40
	#define INT_UPD_REC_FREQ		0x80
#define REG_INTMASK_INIT			(INT_INVALID | INT_TRANS_ERR)

#define REG_SPDSTAT			0x0C
	#define SPDSTAT_AUDIO_N			0x01
	#define SPDSTAT_PCM_N			0x02
	#define SPDSTAT_CPY_N			0x04
	#define SPDSTAT_DEEMPH			0x08
	#define SPDSTAT_FREQ_MASK		0x30 // 0x10 | 0x20
		#define SPDSTAT_FREQMODE_192K		0x00
		#define SPDSTAT_FREQMODE_96K		0x10
		#define SPDSTAT_FREQMODE_48K		0x20
		#define SPDSTAT_FREQMODE_32K		0x30
	#define SPDSTAT_UNLOCK			0x40 // 7 = 01SPDSSPDSTAT_TAT_ 11SSPDSTAT_PDSTAT_

#define REG_RXCHAN1					0x0D // R-O
	#define RXCHAN1_CONPRO			0x01 // Consumer / Proffesional
	#define RXCHAN1_AUDIO_N			0x02
	#define RXCHAN1_CPY_N			0x04
	#define RXCHAN1_DEEMPH			0x08
	#define RXCHAN1_CHSTMODE_MASK	0xC0	// Channel Mode = 00
#define REG_RXCHAN2					0x0E // R-O Category Code
#define REG_RXCHAN3					0x0F // R-O
	#define RXCHAN3_SRCNUM_MASK		0x0F
	#define RXCHAN3_CHNUM1_LEFT		0x10
	#define RXCHAN3_CHNUM1_RIGHT	0x20
	#define RXCHAN3_CHNUM2_LEFT		0x40
	#define RXCHAN3_CHNUM2_RIGHT	0x80
#define REG_RXCHAN4					0x10 // R-O
	#define RXCHAN4_FREQ_MASK		0x0F
	#define RXCHAN4_CLKACU_MASK		0x30
#define REG_RXCHAN5					0x11 // R-O
	#define RXCHAN5_MAXWL			0x01 // Max Audio Sample Word Length: 1=24b, 0=20b
	#define RXCHAN5_RXWL2016		0x04
	#define RXCHAN5_RXWL2218		0x08
	#define RXCHAN5_RXWL2319		0x10
	#define RXCHAN5_RXWL2420		0x14
	#define RXCHAN5_RXWL2117		0x18
	#define RXCHAN5_ORGSAMP_MASK	0xE0

#define REG_GPO01					0x17
#define REG_GPO23					0x18
#define REG_GPO45					0x19
#define REG_GPO67					0x1A
	#define GPOFUNC_INT_N			0x00
	#define GPOFUNC_V				0x01
	#define GPOFUNC_U				0x02
	#define GPOFUNC_C				0x03
	#define GPOFUNC_TRANS_ERR		0x04
	#define GPOFUNC_SFRM_CLK		0x05
	#define GPOFUNC_192BLK			0x06
	#define GPOFUNC_UNLOCK			0x07
	#define GPOFUNC_NON_AUDIO		0x08
	#define GPOFUNC_CSUD			0x09
	#define GPOFUNC_DEEMPH			0x0A
	#define GPOFUNC_CPY_N			0x0B
	#define GPOFUNC_ZEROFLAG		0x0C
	#define GPOFUNC_NONE			0x0F
#define REG_GPO01_INIT				(GPOFUNC_NONE | GPOFUNC_ZEROFLAG << 4)	// GPO1=ZERO
#define REG_GPO23_INIT				(GPOFUNC_INT_N | GPOFUNC_NONE << 4)	    // GPO2=INT_N
#define REG_GPO45_INIT				(GPOFUNC_NONE | GPOFUNC_NONE << 4) 
#define REG_GPO67_INIT				(GPOFUNC_NONE | GPOFUNC_NONE << 4) 
      
#define REG_AIFTX					0x1B
#define REG_AIFTX_INIT				0x0E

#define REG_AIFRX					0x1C
#define REG_AIFRX_INIT				0xCE

#define REG_SPDRX1					0x1D
	#define SPDRX1_READMUX_MASK		0x07
	#define SPDRX1_CONT				0x08
	#define SPDRX1_WITHFLAG			0x10
	#define SPDRX1_SPDGPO			0x20	// 0 = SPDIF, 1 = GPO
	#define SPDRX1_WL_MASK			0x40	
	#define SPDRX1_SPD_192_EN		0x80
#define REG_SPDRX1_CFG				(0x00)
#define REG_SPDRX1_INIT				(REG_SPDRX1_CFG)

#define	REG_PWRDN					0x1E
	#define PWRDN_PLLPD				0x01
	#define PWRDN_SPDIFRXPD			0x02
	#define PWRDN_SPDIFTXPD			0x04
	#define PWRDN_OSCPD				0x08
	#define PWRDN_AIFPD				0x10
	#define PWRDN_TRIOP				0x20
#define REG_PWRDN_POWERDOWN			(0x3F)
#define REG_PWRDN_POWERUP			(PWRDN_SPDIFTXPD)

#define PLL_LOCKTIME				100		// Timeout to wait in ms for the PLL to lock
#define PLL_N_MODE1					8 // 8.192
#define PLL_K_MODE1					0x0C49BA
#define PLL_N_MODE234				7 // 7.8592
#define PLL_K_MODE234				0x36FD21

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
	uint8_t rev = readreg8(REG_DEVID3);
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
    writereg8(REG_PWRDN, REG_PWRDN_POWERUP);

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

void WM8805::autoSelectInput()
{
	for(byte rx = 0; rx < 8; rx++)
	{
		// DBG("Testing input ");
		// DBGLN(rx, DEC);
		if(selectInput(rx))
			return;
	};
}

bool WM8805::selectInput(byte num)
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

void WM8805::set_pll(byte pll_n, unsigned long pll_k)
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

byte WM8805::getSampleRate()
{
	if(_spdstat & SPDSTAT_UNLOCK)
		return RATE_UNLOCKED;
	return (_spdstat & SPDSTAT_FREQ_MASK);
}

void WM8805::printStatus(bool print_int /* = false */)
{
    DBGLN("\nWM8805.printStatus:");

    if(print_int)
    {
	    DBG(" INT: ");
	    byte in = readreg8(REG_INTSTAT);
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
    byte st = readreg8(REG_SPDSTAT);
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

byte WM8805::getChanSampleRate()
{
	if(!isLocked())
		return 0;

	byte rx = readreg8(REG_RXCHAN4);
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
