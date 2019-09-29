#ifndef __WM8805_REG_H
#define __WM8805_REG_H

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

#endif // __WM8805_REG_H