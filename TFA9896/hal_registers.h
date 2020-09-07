/************************************************************************/
/* Copyright 2020 GOODIX                                                */
/*                                                                      */
/* GOODIX Confidential. This software is owned or controlled by GOODIX  */
/* and may only be used strictly in accordance with the applicable      */
/* license terms.  By expressly accepting such terms or by downloading, */
/* installing, activating and/or otherwise using the software, you are  */
/* agreeing that you have read, and that you agree to comply with and   */
/* are bound by, such license terms.                                    */
/* If you do not agree to be bound by the applicable license terms,     */
/* then you may not retain, install, activate or otherwise use the      */
/* software.                                                            */
/*                                                                      */
/************************************************************************/
#include<stdint.h>
#if defined(WIN32) || defined(_X64)
#include <windows.h>
#define TFA98XX_SLEEP Sleep
#else
#include<unistd.h>
#define TFA98XX_SLEEP(msecs) usleep(1000 * msecs)
#endif
//#define TFA9891_DEVICE
#define TFA9896_DEVICE
//#define TFA9897_DEVICE
#define PLL_READ_TRIAL 10
#define TFA98XX_PRINT printf

//#define  SUPPORT_EXCURSION_FILTER
//#define MINIMIZE_TRANSACTION_CHUNCK
//#define TFA_DEBUG_DATA
//#define FACTORY_CALIBRATION_ONLY
//#define ENABLE_LIVEDATA_CAPTURE
#define TFA_LEFT_DEVICE 0x36
#define TFA_RIGHT_DEVICE 0x37 
#define TFA1_FW_ReZ_SCALE             	16384
#define MAX_PARAM_SIZE (145*3)
#define TFA1_MAX_PARAM_SIZE (145*3) /* TFA1 */
#define TFA_I2C_MAX_SIZE 254
#define MAX_I2C_LENGTH			TFA_I2C_MAX_SIZE
#define ROUND_DOWN(a,n) (((a)/(n))*(n))
#define TFA98XX_API_WAITRESULT_NTRIES        3000
#define TFA1_BF_PWDN 0x0900
/* RPC Status results */
#define STATUS_OK                  0
#define STATUS_INVALID_MODULE_ID   2
#define STATUS_INVALID_PARAM_ID    3
#define STATUS_INVALID_INFO_ID     4
/* module Ids */
#define MODULE_FRAMEWORK        0
#define MODULE_SPEAKERBOOST     1
#define MODULE_BIQUADFILTERBANK 2
#define SB_PARAM_GET_STATE        0xC0
#define FW_PARAM_GET_STATE        0x84
/* RPC commands */
#define SB_PARAM_GET_RE0          0x85
#define SB_PARAM_SET_LSMODEL      0x06  // Load a full model into SpeakerBoost.
#define SB_PARAM_SET_EQ			  0x0A  // 2 Equaliser Filters. 
#define SB_PARAM_SET_PRESET       0x0D  // Load a preset
#define SB_PARAM_SET_CONFIG		  0x0E  // Load a config 
#define SB_PARAM_SET_DRC          0x0F
#define SB_PARAM_SET_AGCINS       0x10 
#define PARAM_GET_LSMODEL           0x86        // Gets current LoudSpeaker impedance Model.
#define PARAM_GET_LSMODELW        0xC1  // Gets current LoudSpeaker xcursion Model.

#define FW_STATE_SIZE 9
#define FW_STATE_MAX_SIZE FW_STATE_SIZE
#define SPKRBST_HEADROOM			7					  /* Headroom applied to the main input signal */
#define SPKRBST_AGCGAIN_EXP			SPKRBST_HEADROOM	  /* Exponent used for AGC Gain related variables */
#define SPKRBST_TEMPERATURE_EXP     9
#define SPKRBST_LIMGAIN_EXP			4			          /* Exponent used for Gain Corection related variables */
#define SPKRBST_TIMECTE_EXP         1
#define TFA1_BF_ACS   0x00b0 
#define TFA1_BF_DMEM  0x7011
#define TFA1_BF_MADD  0x710f
#define TFA1_BF_I2SDOE 0x04b0
#define TFA1_BF_PLLS  0x0010
#define TFA1_BF_SBSL  0x0950
#define TFA1_BF_RST 0x7000
#define TFA1_BF_I2CR 0x0910
#define TFA1_BF_AMPS  0x00e0
#define TFA1_BF_MTPB  0x0080
#define TFA1_BF_MTPK   0x0b07
#define TFA1_BF_SPKS    0x00a0
#define TFA1_BF_MTPOTC 0x8000
#define TFA1_BF_MTPEX  0x8010
#define TFA1_BF_MTP0 TFA1_BF_MTPOTC 
#define TFA1_BF_CIMTP   0x62b0
#define TFA_MTPOTC_POS 0
#define TFA_MTPEX_POS  1
#ifndef MIN
#define MIN(A,B)(A<B?A:B)
#endif//
#define TFA98XX_CURRENTSENSE4_CTRL_CLKGATECFOFF (1<<2)
#define TFA1_BF_RST 0x7000
#define TFA1_BF_AREFS 0x00f0
#define TFA1_BF_CLKS  0x0060
#define TFA1_BF_AMPS  0x00e0
#define TFA1_BF_MTPB  0x0080
#ifdef ENABLE_AEC_DIRECTION
#define TFA1_BF_DOLS     0xa02
#define TFA1_BF_DORS    0xa32
#endif//
#if defined (TFA9896_DEVICE) || defined (TFA9897_DEVICE)
#define TFA1_BF_TDMSAMSZ 0x10a4
#define TFA1_BF_NBCK 0x14c3
#define TFA1_BF_AUDFS 0x04c3
#define TFA1_BF_CLIP 0x4900
#define TFA1_BF_CHS12 0x0431
#define TFA1_BF_CFE 0x0920
#define TFA1_BF_CHSA 0x0461
#define TFA1_BF_AMPC 0x0960
#define TFA1_BF_IPLL  0x09e0
#define PLL_READ_TRIAL 10
#define TFA1_BF_ATTEN 0x0613
#define TFA1_BF_DCTRIP 0x0a04
#define TFA1_BF_DCPWM 0x07d0
#define  bst_slpcmplvl 0xaa1
#define  ignore_flag_voutcomp86 0x7e0
#endif //(TFA9896_DEVICE) || defined (TFA9897_DEVICE)
#define TFA98XX_AUDIO_CTR           0x06
#define TFA98XX_SYS_CTRL            0x09
#define TFA98XX_CURRENTSENSE4       0x49
#define TFA98XX_AUDIO_CTR_CFSM_MSK        0x20
#define TFA98XX_SYS_CTRL_AMPE_MSK         0x8
#define TFA98XX_SYS_CTRL_DCA_MSK          0x10

#define OPTIMIZED_RPC
typedef int int24;
typedef struct uint24M {
	uint8_t b[3];
}uint24M_t;
typedef struct TfaBiquadM {
	uint8_t bytes[6 * sizeof(uint24M_t)];
}TfaBiquadM_t;

typedef struct TfaFilterM {
	TfaBiquadM_t biquad;
	uint8_t enabled;
	uint8_t type; // (== enum FilterTypes, assure 8bits length)
	float frequency;
	float Q;
	float gain;
} TfaFilterM_t;  //8 * float + int32 + byte == 37
/************/
#ifdef SUPPORT_EXCURSION_FILTER
typedef struct TfaContAntiAliasM {
	unsigned char index; 	/**< index determines destination type; anti-alias, integrator,eq */
	unsigned char type;
	float cutOffFreq;   // cut off frequency
	float samplingFreq;
	float rippleDb;     // integrator leakage
	float rolloff;
	uint8_t bytes[5 * 3];	// payload 5*24buts coeffs
}TfaContAntiAliasM_t;

typedef struct TfaContIntegratorM {
	int8_t index; 	/**< index determines destination type; anti-alias, integrator,eq */
	uint8_t type;
	float cutOffFreq;   // cut off frequency
	float samplingFreq;
	float leakage;     // integrator leakage
	float reserved;
	uint8_t bytes[5 * 3];	// payload 5*24buts coeffs
}TfaContIntegratorM_t;
typedef struct TfaContEqM {
	int8_t index;
	uint8_t type;       // (== enum FilterTypes, assure 8bits length)
	float cutOffFreq;   // cut off frequency, // range: [100.0 4000.0]
	float samplingFreq; // sampling frequency
	float Q;            // range: [0.5 5.0]
	float gainDb;       // range: [-10.0 10.0]
	uint8_t bytes[5 * 3];	// payload 5*24buts coeffs
} TfaContEqM_t;  //8 * float + int32 + byte == 37

typedef struct TfaContBiquadM {
	TfaContAntiAliasM_t aa;
}TfaContBiquadM_t;
#endif
typedef enum Tfa9890_error {
	Error_Ok = 0,
	Error_DSP_not_running,  /* communication with the DSP failed, presumably because DSP not running */
	Error_Bad_Parameter,
	Error_NotOpen,  /* the given handle is not open */
	Error_OutOfHandles,     /* too many handles */
	Error_StateTimedOut,	/* the expected response did not occur within the expected time */

	Error_RpcBase = 100,
	Error_RpcBusy = 101,
	Error_RpcModId = 102,
	Error_RpcParamId = 103,
	Error_RpcInfoId = 104,
	Error_RpcNotAllowedSpeaker = 105,

	Error_Not_Implemented,
	Error_Not_Supported,
	Error_I2C_Fatal,        /* Fatal I2C error occurred */
	Error_I2C_NonFatal,     /* Nonfatal I2C error, and retry count reached */
	Error_Other = 1000
} Tfa9890_Error_t;
typedef enum {
	DMEM_PMEM = 0,
	DMEM_XMEM = 1,
	DMEM_YMEM = 2,
	DMEM_IOMEM = 3,
} Tfa98xx_DMEM;
typedef enum Tfa9891_Mute {
	Mute_Off,
	Mute_Digital,
	Mute_Amplifier
} Tfa9891_Mute_t;
enum TFA_I2C_Error {
	TFA_I2C_UnassignedErrorCode,		/**< UnassignedErrorCode*/
	TFA_I2C_Ok,				/**< no error */
	TFA_I2C_NoAck, 		/**< no I2C Ack */
	TFA_I2C_TimeOut,		/**< bus timeout */
	TFA_I2C_ArbLost,		/**< bus arbritration lost*/
	TFA_I2C_NoInit,		/**< init was not done */
	TFA_I2C_UnsupportedValue,		/**< UnsupportedValu*/
	TFA_I2C_UnsupportedType,		/**< UnsupportedType*/
	TFA_I2C_NoInterfaceFound,		/**< NoInterfaceFound*/
	TFA_I2C_NoPortnumber,			/**< NoPortnumber*/
	TFA_I2C_BufferOverRun,			/**< BufferOverRun*/
	TFA_I2C_ErrorMaxValue				/**<impossible value, max enum */
};
typedef enum TFA_I2C_Error TFA_I2C_Error_t;
typedef struct Tfa98xx_StateInfoLive {
	/* SpeakerBoost State */
	float agcGain;  /* Current AGC Gain value */
	float limGain;  /* Current Limiter Gain value */
	float sMax;     /* Current Clip/Lim threshold */
	int T;          /* Current Speaker Temperature value */
	int statusFlag; /* Masked bit word, see Tfa98xx_SpeakerBoostStatusFlags */
	float X1;       /* Current estimated Excursion value caused by Speakerboost gain control */
	float X2;       /* Current estimated Excursion value caused by manual gain setting */
	float Re;       /* Current Loudspeaker blocked resistance */
	/* Framework state */
	int   shortOnMips; /* increments each time a MIPS problem is detected on the DSP, only presenton newer ICs */
} Tfa98xx_StateInfoLive_t;
typedef enum Tfa98xx_SpeakerBoostStatusFlagsLive {
	Tfa98xx_SpeakerBoost_ActivityLive = 0,      /* Input signal activity. */
	Tfa98xx_SpeakerBoost_S_CtrlLive,    /* S Control triggers the limiter */
	Tfa98xx_SpeakerBoost_MutedLive,     /* 1 when signal is muted */
	Tfa98xx_SpeakerBoost_X_CtrlLive,    /* X Control triggers the limiter */
	Tfa98xx_SpeakerBoost_T_CtrlLive,    /* T Control triggers the limiter */
	Tfa98xx_SpeakerBoost_NewModelLive,  /* New model is available */
	Tfa98xx_SpeakerBoost_VolumeRdyLive, /* 0 means stable volume, 1 means volume is still smoothing */
	Tfa98xx_SpeakerBoost_DamagedLive,   /* Speaker Damage detected  */
	Tfa98xx_SpeakerBoost_SignalClippingLive     /* Input Signal clipping detected */
} Tfa98xx_SpeakerBoostStatusFlagsLive_t;

typedef struct TFA9891SPKRBST_SpkrModel {
	double pFIR[128];       /* Pointer to Excurcussion  Impulse response or
							   Admittance Impulse response (reversed order!!) */
	int Shift_FIR;          /* Exponent of HX data */
	float leakageFactor;    /* Excursion model integration leakage */
	float ReCorrection;     /* Correction factor for Re */
	float xInitMargin;      /*(1)Margin on excursion model during startup */
	float xDamageMargin;    /* Margin on excursion modelwhen damage has been detected */
	float xMargin;          /* Margin on excursion model activated when LookaHead is 0 */
	float Bl;               /* Loudspeaker force factor */
	int fRes;               /*(1)Estimated Speaker Resonance Compensation Filter cutoff frequency */
	int fResInit;           /* Initial Speaker Resonance Compensation Filter cutoff frequency */
	float Qt;               /* Speaker Resonance Compensation Filter Q-factor */
	float xMax;             /* Maximum excursion of the speaker membrane */
	float tMax;             /* Maximum Temperature of the speaker coil */
	float tCoefA;           /*(1)Temperature coefficient */
} TFA9891SPKRBST_SpkrModel_t;          /* (1) this value may change dynamically */

Tfa9890_Error_t read_register(uint8_t slave_address, uint8_t subaddress, unsigned short* value);
Tfa9890_Error_t  write_register(uint8_t slave_address, uint8_t subaddress, uint16_t value);
void waitCalibration(uint8_t slave_address, int* calibrateDone);
Tfa9890_Error_t Tfa9890_SetOVPBypass(uint8_t slave_address);
