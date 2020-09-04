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
#define TFA98XX_SLEEP sleep
#endif
//#define TFA98XX_PRINT_DEBUG
#define TFA98XX_PRINT printf
//#define START_CONFIG_TFA_BYPASS
//#define TFA_RETRIEVE_MODEL_MODEL 
#define SAME_CONFIG_LEFT_RIGHT
//#define TFA_ENABLE_LIVEDATA_CAPTURE
#define LEFT_AMP  0x34
#define RIGHT_AMP 0x35
#define PLL_READ_TRIAL 10
#define BROWNOUT_DETECT_SUPPORT
#define TFA_MAX_VSTEP_MSG_MARKER 100
#define SB_PARAM_SET_ALGO_PARAMS        0x00
#define SB_PARAM_SET_LAGW               0x01
#define SB_PARAM_SET_ALGO_PARAMS_WITHOUT_RESET	0x02
#define SB_PARAM_SET_LSMODEL            0x06
#define SB_PARAM_SET_MBDRC              0x07
#define SB_PARAM_SET_MBDRC_WITHOUT_RESET	0x08
#define SB_PARAM_SET_DRC                0x0F
#define TFA2_BF_MEMA  0x920f
#define FAM_TFA98XX_CF_MEM      (TFA2_BF_MEMA>>8)
#define TFA2_BF_AIF 0x9030
#define TFA2_BF_REQCMD 0x9080
#define TFA2_BF_CFINT  0x9040
#define TFA2_BF_MADD 0x910f
#define TFA2_BF_DMEM 0x9011
#define TFA2_BF_SBSL 0x0050
#define TFA2_BF_AUDFS 0x0203
#define TFA2_BF_ACS 0x1090
#define TFA2_BF_RST 0x9000
#define FAM_TFA98XX_CF_CONTROLS (TFA2_BF_RST >> 8)
#define TFA2_BF_FRACTDEL 0x0255
#define TFA2_BF_CFSM 0x5130
#define TFA2_BF_CFE  0x0020
#define TFA2_BF_AMPE 0x0030
#define TFA2_BF_DCA 0x0040
#define TFA2_BF_DCIE 0x7090
#define TFA2_BF_DCINSEL 0x70c1
#define TFA2_BF_AMPC 0x0060
#define TFA2_BF_AMPINSEL 0x0101
#define enbl_clk_range_chk 0xd580
#define TFA2_BF_DCVOF 0x7635
#define TFA2_BF_DCVOS 0x7695
#define TFA2_BF_TDMSSIZE 0x2224
#define TFA2_BF_TDMNBCK 0x20c3
#define TFA2_BF_TDMSLLN 0x2144
#define TFA2_BF_MTPK 0xa107
/******************************/
#define TFA2_BF_TDMCSE  0x2330
#define TFA2_BF_TDMVSE  0x2340
#define TFA2_BF_TDMCFE  0x2380
#define TFA2_BF_BSST  0x5023
#define TFA2_BF_BSSBY  0x50f0
#define TFA2_BF_BSSS  0x5100
#define TFA22_BF_TDMSPKG 0x6123
#define bypass_ovp  0xb030
#define clipfast 0x5340
#define TFA2_BF_DCMCC 0x7033
#define TFA2_BF_DCCV  0x7071
#define TFA2_BF_DCTRIP 0x7504
#define TFA2_BF_DCTRIP2 0x7554
#define	TFA2_BF_SSFAIME  0x5c0
#define TFA2_BF_MANAOOSC 0x0140
#define TFA2_BF_LPM1DIS 0x65c0
#define TFA2_BF_TROS 0xb190
#define lownoisegain_mode 0x62e1
#define TFA2_BF_EXTTS 0xb108
#ifdef BROWNOUT_DETECT_SUPPORT
#define TFA2_BF_BODE 0x0160 
#define TFA2_BF_BODTHLVL 0x01a1 
#define TFA2_BF_MANROBOD 0x0150      
#endif//
#define TFA2_BF_MANSTATE 0x1433
/******************************/
#define TFA2_BF_PWDN 0x0000
#define TFA2_BF_AREFS 0x10d0
#define TFA2_BF_CLKS  0x1050    /*!< Clocks stable                                      */
#define TFA2_BF_MTPB  0x1060
#define TFA2_BF_ACK 0x9387
#define TFA2_BF_MANSCONF 0x0120
#define TFA2_BF_I2CR 0x0010
#define TFA2_BF_MANCOLD 0x0130
#define TFA2_FW_ReZ_SCALE             	65536
#define TFA2_BF_MTPOTC 0xf000    /*!< Calibration schedule                               */
#define TFA2_BF_MTPEX  0xf010
#define TFA2_BF_SPKS   0x1410
#define TFA2_BF_CIMTP  0xa360
#define TFA2_BF_MTPB 0x1060
#define TFA2_MTP_RE25  0xf40f
#define TFA98XX_WAITRESULT_NTRIES  40/*40*/
#define MAX_PARAM_SIZE (145*3)
#define TFA1_MAX_PARAM_SIZE (145*3) /* TFA1 */
#define TFA_I2C_MAX_SIZE 254
#define MAX_I2C_LENGTH			TFA_I2C_MAX_SIZE
#define ROUND_DOWN(a,n) (((a)/(n))*(n))
#define TFA98XX_API_WAITRESULT_NTRIES        3000
/* RPC Status results */
#define STATUS_OK                  0
#define STATUS_INVALID_MODULE_ID   2
#define STATUS_INVALID_PARAM_ID    3
#define STATUS_INVALID_INFO_ID     4
/* module Ids */
#define FW_PAR_ID_SET_SENSES_DELAY      0x04
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
#ifndef MIN
#define MIN(A,B)(A<B?A:B)
#endif//
/* Data structure */
typedef struct tfa98xx_registers {
	uint16_t reg;
    uint16_t val;
} Tfa98xx_Registers_t;
/*******************/
enum Tfa98xx_Status_IDTfa98xx {
        DSP_Not_Running = -1,           /* No response from DSP */
	    I2C_Req_Done = 0,               /* Request executed correctly and result, if any, is available for download */
        I2C_Req_Busy = 1,               /* Request is being processed, just wait for result */
	    I2C_Req_Invalid_M_ID = 2,       /* Provided M-ID does not fit in valid rang [0..2] */
        I2C_Req_Invalid_P_ID = 3,       /* Provided P-ID isn�t valid in the given M-ID context */
        I2C_Req_Invalid_CC = 4,         /* Invalid channel configuration bits (SC|DS|DP|DC) combination */
        I2C_Req_Invalid_Seq = 5,        /* Invalid sequence of commands, in case the DSP expects some commands in a specific order */
        I2C_Req_Invalid_Param = 6,      /* Generic error */
        I2C_Req_Buffer_Overflow = 7    /* I2C buffer has overflowed: host has sent too many parameters, memory integrity is not guaranteed */
};
/***********************/
#define OPTIMIZED_RPC
typedef int int24;
typedef struct uint24M {
  uint8_t b[3];
}uint24M_t;
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
	DMEM_PMEM=0, 
	DMEM_XMEM=1,
	DMEM_YMEM=2,
	DMEM_IOMEM=3,
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
Tfa9890_Error_t read_register(uint8_t slave_address, uint8_t subaddress,unsigned short * value);
Tfa9890_Error_t  write_register(uint8_t slave_address, uint8_t subaddress, uint16_t value);
void waitCalibration(uint8_t slave_address, int *calibrateDone);
Tfa9890_Error_t tfa_dsp_msg_tfa98xxx(uint8_t slave_address,int length, const char *buf);
Tfa9890_Error_t  tfa98xxdsp_get_calibration_impedance(uint8_t slave_address,float *pRe25);
Tfa9890_Error_t tfa98xxRunWaitCalibration(uint8_t slave_address, int *calibrateDone);
Tfa9890_Error_t tfa98xx_dsp_write_mem_word_tfa98xx(uint8_t slave_address, unsigned short address, int value, int memtype);
void SetOneTimeCalibration(uint8_t slave_address);
Tfa9890_Error_t ClearOneTimeCalibration(uint8_t slave_address);
#pragma pack(push, 1)
typedef struct TfaHeaderTfa98xx {
	uint16_t id;
    char version[2];     // "V_" : V=version, vv=subversion
    char subversion[2];  // "vv" : vv=subversion
    uint16_t size;       // data size in bytes following CRC
    uint32_t CRC;        // 32-bits CRC for following data
    char customer[8];    // “name of customer”
    char application[8]; // “application name”
    char type[8];		 // “application type name”
} TfaHeaderTfa98xx_t;
/*
 * volumestepMax2 file
 */
typedef struct TfaBfEnumTfa98xx {
	unsigned int  len:4;		// this is the actual length-1
	unsigned int  pos:4;
	unsigned int  address:8;
} TfaBfEnumTfa98xx_t;

typedef struct TfaVolumeStepMax2FileTfa98xx {
	TfaHeaderTfa98xx_t hdr;
	uint8_t version[3]; 
	uint8_t NrOfVsteps;
	uint8_t vstepsBin[]; 
}TfaVolumeStepMax2FileTfa98xx_t;

/*
 * volumestepMax2 file
 * This volumestep should ONLY be used for the use of bin2hdr!
 * This can only be used to find the messagetype of the vstep (without header)
 */
typedef struct TfaVolumeStepMax2_1FileTfa98xx {
	uint8_t version[3]; 
	uint8_t NrOfVsteps;
	uint8_t vstepsBin[]; 
}TfaVolumeStepMax2_1FileTfa98xx_t;

struct TfaVolumeStepRegisterInfoTfa98xx {
	uint8_t NrOfRegisters;
	uint16_t registerInfo[];
};

struct TfaVolumeStepMessageInfoTfa98xx {
	uint8_t NrOfMessages;
	uint8_t MessageType;
	uint24M_t MessageLength;
	uint8_t CmdId[3];
	uint8_t ParameterData[];
};
typedef struct TfaBitfieldTfa98xx {
	uint16_t  value;
	uint16_t  field; // ==datasheet defined, 16 bits
} TfaBitfieldTfa98xx_t;
#pragma pack(pop)

/******totest********/
typedef struct TfaBitfield4 {
	uint16_t  value;
	uint16_t  field; // ==datasheet defined, 16 bits
} TfaBitfield4_t;

typedef struct TfaHeader4 {
	uint16_t id;
    char version[2];     // "V_" : V=version, vv=subversion
    char subversion[2];  // "vv" : vv=subversion
    uint16_t size;       // data size in bytes following CRC
    uint32_t CRC;        // 32-bits CRC for following data
    char customer[8];    // “name of customer”
    char application[8]; // “application name”
    char type[8];		 // “application type name”
} TfaHeader4_t;

typedef struct TfaVolumeStepMax2File4 {
	TfaHeader4_t hdr;
	uint8_t version[3]; 
	uint8_t NrOfVsteps;
	uint8_t vstepsBin[]; 
}TfaVolumeStepMax2File4_t;
struct TfaVolumeStepRegisterInfo4 {
	uint8_t NrOfRegisters;
	uint16_t registerInfo[];
};

struct TfaVolumeStepMessageInfo4 {
	uint8_t NrOfMessages;
	uint8_t MessageType;
	uint24M_t MessageLength;
	uint8_t CmdId[3];
	uint8_t ParameterData[];
};