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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_registers.h"
#include "buffer.h"
#include <assert.h>
#define TFA98XX_PATCH_EXTRA   6
#ifdef START_CONFIG_TFA_BYPASS
Tfa98xx_Registers_t OptimalSettingReg_Bypass[] =
{
	{0x010f, 0x15da}, //POR=0x0245
	{0x020f, 0x52e8}, //POR=0x83ca
	{0x040f, 0x0200}, //POR=0x83ca
	{0x520f, 0xbe17}, //POR=0x2828
	{0x530f, 0x0dbe}, //POR=0x2828
	{0x570f, 0x0344}, //POR=0x545c
	{0x610f, 0x0032}, //POR=0x0010
	{0x710f, 0x6ecf}, //POR=0x0080
	{0x720f, 0xb4a9}, //POR=0x1afc
	{0x730f, 0x38c8}, //POR=0x0018
	{0x760f, 0x0067},
	{0x800f, 0x0000}, //POR=0x0a9a
	{0x810f, 0x5799}, //POR=0x0a8b
	{0x820f, 0x0104},
	{TFA2_BF_TDMNBCK,0},
	{TFA2_BF_TDMSLLN,15},
	{TFA2_BF_TDMSSIZE,15},
	{TFA2_BF_TDMSPKE,1},
	{TFA2_BF_TDMDCE,1},
	{TFA2_BF_TDMCSE,0},
	{TFA2_BF_TDMVSE,0},
	{TFA2_BF_TDMCF2E,1},
	{TFA2_BF_TDMSPKS,0},
	{TFA2_BF_TDMDCS,1},
	{TFA2_BF_TDMCSS,0},
	{TFA2_BF_TDMVSS,1},
	{TFA2_BF_TDMCF2S,1},
	{TFA2_BF_FRACTDEL,20},
	{TFA2_BF_TDMSPKG,4},
	{TFA2_BF_TDMDCG,4},
	{TFA2_BF_DCTRIP,9},
	{TFA2_BF_DCTRIPT,13},
	{TFA2_BF_DCVOS,63},
	{TFA2_BF_AMPE,1},
#ifdef BROWNOUT_DETECT_SUPPORT
	{TFA2_BF_BODE,1},
	{TFA2_BF_BODTHLVL,2},
	{TFA2_BF_MANROBOD,1},
#endif//BROWNOUT_DETECT_SUPPORT
	{0xffff, 0}

};
#else
Tfa98xx_Registers_t OptimalSettingReg_Dev_prof0Reg[] =
{
	{0x010f, 0x15da}, //POR=0x0245
	{0x020f, 0x52e8}, //POR=0x83ca
	{0x040f, 0x0200}, //POR=0x83ca
	{0x520f, 0xbe17}, //POR=0x2828
	{0x530f, 0x0dbe}, //POR=0x2828
	{0x570f, 0x0344}, //POR=0x545c
	{0x610f, 0x0032}, //POR=0x0010
	{0x710f, 0x6ecf}, //POR=0x0080
	{0x720f, 0xb4a9}, //POR=0x1afc
	{0x730f, 0x38c8}, //POR=0x0018
	{0x760f, 0x0067},
	{0x800f, 0x0000}, //POR=0x0a9a
	{0x810f, 0x5799}, //POR=0x0a8b
	{0x820f, 0x0104},
	{TFA2_BF_TDMNBCK,0},
	{TFA2_BF_TDMSLLN,15},
	{TFA2_BF_TDMSSIZE,15},
	{TFA2_BF_TDMSPKE,1},
	{TFA2_BF_TDMDCE,1},
	{TFA2_BF_TDMCSE,0},
	{TFA2_BF_TDMVSE,0},
	{TFA2_BF_TDMCF2E,1},
	{TFA2_BF_TDMSPKS,0},
	{TFA2_BF_TDMDCS,1},
	{TFA2_BF_TDMCSS,0},
	{TFA2_BF_TDMVSS,1},
	{TFA2_BF_TDMCF2S,1},
	{TFA2_BF_FRACTDEL,20},
	{TFA2_BF_TDMSPKG,4},
	{TFA2_BF_TDMDCG,4},
	{TFA2_BF_DCTRIP,9},
	{TFA2_BF_DCTRIPT,13},
	{TFA2_BF_DCVOS,63},
	{TFA2_BF_AMPE,1},
#ifdef BROWNOUT_DETECT_SUPPORT
	{TFA2_BF_BODE,1},
	{TFA2_BF_BODTHLVL,2},
	{TFA2_BF_MANROBOD,1},
#endif//BROWNOUT_DETECT_SUPPORT
	{0xffff, 0}

};
Tfa98xx_Registers_t OptimalSettingReg_Dev_Cal[] =
{
	{0x010f, 0x15da}, //POR=0x0245
	{0x020f, 0x52e8}, //POR=0x83ca
	{0x040f, 0x0200}, //POR=0x83ca
	{0x520f, 0xbe17}, //POR=0x2828
	{0x530f, 0x0dbe}, //POR=0x2828
	{0x570f, 0x0344}, //POR=0x545c
	{0x610f, 0x0032}, //POR=0x0010
	{0x710f, 0x6ecf}, //POR=0x0080
	{0x720f, 0xb4a9}, //POR=0x1afc
	{0x730f, 0x38c8}, //POR=0x0018
	{0x760f, 0x0067},
	{0x800f, 0x0000}, //POR=0x0a9a
	{0x810f, 0x5799}, //POR=0x0a8b
	{0x820f, 0x0104},
	{TFA2_BF_TDMNBCK,0},
	{TFA2_BF_TDMSLLN,15},
	{TFA2_BF_TDMSSIZE,15},
	{TFA2_BF_TDMSPKE,1},
	{TFA2_BF_TDMDCE,1},
	{TFA2_BF_TDMCSE,0},
	{TFA2_BF_TDMVSE,0},
	{TFA2_BF_TDMCF2E,1},
	{TFA2_BF_TDMSPKS,0},
	{TFA2_BF_TDMDCS,1},
	{TFA2_BF_TDMCSS,0},
	{TFA2_BF_TDMVSS,1},
	{TFA2_BF_TDMCF2S,1},
	{TFA2_BF_FRACTDEL,20},
	{TFA2_BF_TDMSPKG,4},
	{TFA2_BF_TDMDCG,4},
	{TFA2_BF_DCTRIP,9},
	{TFA2_BF_DCTRIPT,13},
	{TFA2_BF_DCVOS,63},
	{TFA2_BF_AMPE,1},
	{TFA2_BF_DCA,0},
	{TFA2_BF_AMPE,1},
	{TFA2_BF_TROS,1},
	{TFA2_BF_LPM1MODE,1},
	{TFA2_BF_LNMODE,1},
	{enbl_dc_filter,0},
	{TFA2_BF_TROS,1},
	{TFA2_BF_EXTTS,25},
	{0xffff, 0}

};

#endif//
#if defined(START_CONFIG_TFA_BYPASS)
Tfa9890_Error_t error tfa_generic_bypass(uint16_t slave_address)
{
	uint16_t regRead = 0;
	Tfa9890_Error_t error;
	Tfa98xx_Registers_t *pRegs = NULL;
	pRegs = OptimalSettingReg_Bypass;
	/*************************/
	error = tfa98xx_set_bf_value(slave_address, TFA2_BF_I2CR, 1, &regRead);
	error = tfa_write_reg_tfa98xx(slave_address, TFA2_BF_I2CR, regRead);
	assert(error == Error_Ok);
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_MANSCONF, 0);
	assert(error == Error_Ok);
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_MANCOLD, 1);
	assert(error == Error_Ok);
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_RST, 1);
	assert(error == Error_Ok);


	/* Unlock keys to write settings */
	write_register(slave_address, 0x0F, (uint16_t)0x5A6B);
	read_register(slave_address, 0xFB, &regRead);
	xor = regRead ^ 0x005A;
	error = write_register(slave_address, 0xA0, xor);
	while (pRegs->reg != 0xffff) {

		error = tfa_set_bf_tfa98xx(slave_address, pRegs->reg, pRegs->val);
		assert(error == Error_Ok);
		pRegs += 1;
	}
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_PWDN, 0);
	assert(error == Error_Ok);
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_MANSCONF, 1);
	assert(error == Error_Ok);
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_SBSL, 1);
	assert(error == Error_Ok);
	return error;

}
#else
Tfa9890_Error_t Tfa_generic_start(uint16_t slave_address, uint8_t calibrate)
{
	uint8_t trial = 10, sc = 0, cnt = 0,tries = 0;
	uint16_t regRead = 0, xor = 0;
	uint32_t acsv = 1, status;
	Tfa9890_Error_t error = Error_Ok;
	int calibrateDone = 0;
	unsigned short mtp;
	float re25;
	Tfa98xx_Registers_t *pRegs = NULL;
	if (calibrate)
		pRegs = OptimalSettingReg_Dev_Cal;
	else
		pRegs = OptimalSettingReg_Dev_prof0Reg;
	/*************************/
	error = tfa98xx_set_bf_value(slave_address, TFA2_BF_I2CR, 1, &regRead);
	error = tfa_write_reg_tfa98xx(slave_address, TFA2_BF_I2CR, regRead);
	assert(error == Error_Ok);
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_MANSCONF, 0);
	assert(error == Error_Ok);
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_MANCOLD, 1);
	assert(error == Error_Ok);
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_RST, 1);
	assert(error == Error_Ok);


	/* Unlock keys to write settings */
	write_register(slave_address, 0x0F, (uint16_t)0x5A6B);
	read_register(slave_address, 0xFB, &regRead);
	xor = regRead ^ 0x005A;
	error = write_register(slave_address, 0xA0, xor);
	while (pRegs->reg != 0xffff) {

		error = tfa_set_bf_tfa98xx(slave_address, pRegs->reg, pRegs->val);
		assert(error == Error_Ok);
		pRegs += 1;
	}
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_MANSCONF, 1);
	tfa_updown_tfa98xx(slave_address, 0);
	/* Check the PLL is powered up from status register 10h*/
	read_register(slave_address, 0x10, &regRead);
	while ((regRead & 0x2000) == 0 && (tries <= PLL_READ_TRIAL))
	{
		tries++;
		read_register(slave_address, 0x10, &regRead);

	}
	if (tries > PLL_READ_TRIAL)
	{
		TFA98XX_PRINT("PLL is not locked\n");
		return Error_StateTimedOut;
	}

	/*************Force Coldstart*************/
	while (tfa_get_bf_tfa98xx(slave_address, TFA2_BF_ACS) != 1) {
		/*******************************************/
		error = tfa98xx_dsp_write_mem_word_tfa98xx(slave_address, 0x8100, 1, DMEM_IOMEM);
		/*****************************************************/
		if (trial-- == 0) {
			TFA98XX_PRINT("Cannot Coldboot Device\n");
			return Error_StateTimedOut;
		}
	}
	assert(error == Error_Ok);
	for (trial = 1; trial < PLL_READ_TRIAL; trial++) {
		error = tfa_dsp_stability_check(slave_address, &status);
		if (status)
			break;
		else
			TFA98XX_SLEEP(10);
	}
	if (trial >= PLL_READ_TRIAL)
	{
		TFA98XX_PRINT("Timed out , exit, exit\n");
		return Error_StateTimedOut;
	}
	else
		TFA98XX_PRINT("DSP STABILITY OK (trial=%d)\n", trial);

	if (calibrate)
	{
		ClearOneTimeCalibration(slave_address);
		SetOneTimeCalibration(slave_address);
	}
	error = checkICROMversion(slave_address, Patch_patch);
	assert(error == Error_Ok);
	TFA98XX_PRINT("SC:%x PATCH loading Start--length to be  loaded:%d\n",slave_address, Patch_patch_len - TFA98XX_PATCH_EXTRA);
	error = processPatchFile(slave_address, Patch_patch_len - 6, Patch_patch + TFA98XX_PATCH_EXTRA);
	assert(error == Error_Ok);
	TFA98XX_PRINT(" PATCH OK \n");
	error = tfa98xx_dsp_write_mem_word_tfa98xx(slave_address, 512, 0, DMEM_XMEM);
	assert(error == Error_Ok);
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_RST, 0);
	assert(error == Error_Ok);

	error = tfa_mute_tfa98xx(slave_address, Mute_Digital);
	assert(error == Error_Ok);
	TFA98XX_PRINT("SC:%x Message loading Start--length to be  loaded:%d\n",slave_address, Left_SetInputSelector_msg_len);
	error = tfa_dsp_msg_tfa98xxx(slave_address, Left_SetInputSelector_msg_len, Left_SetInputSelector_msg);
	assert(error == Error_Ok);
	TFA98XX_PRINT("MessageOk1\n");
	TFA98XX_PRINT("SC:%x Message loading Start--length to be  loaded:%d\n",slave_address, SetOutputSelector_msg_len);
	error = tfa_dsp_msg_tfa98xxx(slave_address, SetOutputSelector_msg_len, SetOutputSelector_msg);
	assert(error == Error_Ok);
	TFA98XX_PRINT("MessageOk2\n");

	TFA98XX_PRINT("SC:%x Message loading Start--length to be  loaded:%d\n",slave_address, SetProgramConfig_msg_len);
	error = tfa_dsp_msg_tfa98xxx(slave_address, SetProgramConfig_msg_len, SetProgramConfig_msg);
	assert(error == Error_Ok);
	TFA98XX_PRINT("MessageOk3\n");

	TFA98XX_PRINT("SC:%x Message loading Start--length to be  loaded:%d\n",slave_address, SetLagW_msg_len);
	error = tfa_dsp_msg_tfa98xxx(slave_address, SetLagW_msg_len, SetLagW_msg);
	assert(error == Error_Ok);
	TFA98XX_PRINT("MessageOk4\n");
	/*****************/
	TFA98XX_PRINT("SC:%x Speaker loading Start--length to be  loaded:%d\n",slave_address, Speaker_speaker_len);
	error = tfa_dsp_msg_tfa98xxx(slave_address, Speaker_speaker_len, Speaker_speaker);
	assert(error == Error_Ok);
	TFA98XX_PRINT("SpeakerOk\n");
	/*********************/
	if (calibrate)
	{
		error = tfaContWriteVstepMax2_tfa98xx(slave_address, (TfaVolumeStepMax2_1FileTfa98xx_t *)Calibration_vstep, 0, TFA_MAX_VSTEP_MSG_MARKER);
		assert(error == Error_Ok);
	}
	else
	{
		error = tfaContWriteVstepMax2_tfa98xx(slave_address, (TfaVolumeStepMax2_1FileTfa98xx_t *)Left_vstep, 0, TFA_MAX_VSTEP_MSG_MARKER);
		assert(error == Error_Ok);
	}
	TFA98XX_PRINT("VSTEPOk\n");
	/****************************/
	TFA98XX_PRINT("SC:%x Message loading Start--length to be  loaded:%d\n",slave_address, SetGains_msg_len);
	error = tfa_dsp_msg_tfa98xxx(slave_address, SetGains_msg_len, SetGains_msg);
	assert(error == Error_Ok);
	TFA98XX_PRINT("MessageOk9\n");

	TFA98XX_PRINT("SC:%x Message loading Start--length to be  loaded:%d\n",slave_address, SetSensesDelay_msg_len);
	error = tfa_dsp_msg_tfa98xxx(slave_address, SetSensesDelay_msg_len, SetSensesDelay_msg);
	assert(error == Error_Ok);
	TFA98XX_PRINT("MessageOk10\n");


	TFA98XX_PRINT("SC:%x Message loading Start--length to be  loaded:%d\n", slave_address, SetSensesCal_msg_len);
	error = tfa_dsp_msg_tfa98xxx(slave_address, SetSensesCal_msg_len, SetSensesCal_msg);
	assert(error == Error_Ok);
	TFA98XX_PRINT("MessageOk11\n");

	TFA98XX_PRINT("SC:%x Message loading Start--length to be  loaded:%d\n",slave_address, SetvBatFactors_msg_len);
	error = tfa_dsp_msg_tfa98xxx(slave_address, SetvBatFactors_msg_len, SetvBatFactors_msg);
	assert(error == Error_Ok);
	TFA98XX_PRINT("MessageOk12\n");

	TFA98XX_PRINT("SC:%x Message loading Start--length to be  loaded:%d\n",slave_address, Left_excursion_filter_msg_len);
	error = tfa_dsp_msg_tfa98xxx(slave_address, Left_excursion_filter_msg_len, Left_excursion_filter_msg);
	assert(error == Error_Ok);
	TFA98XX_PRINT("MessageOk15\n");

	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_PWDN, 0);
	assert(error == Error_Ok);
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_MANSCONF, 1);
	assert(error == Error_Ok);
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_SBSL, 1);
	assert(error == Error_Ok);
	error = tfa_mute_tfa98xx(slave_address, Mute_Off);
	assert(error == Error_Ok);
	if (calibrate)
	{

		error = tfa98xxRunWaitCalibration(slave_address, &calibrateDone);
		assert(error == Error_Ok);

		if (calibrateDone)
		{
			error = tfa98xxdsp_get_calibration_impedance(slave_address, &re25);
			assert(error == Error_Ok);
		}
		else
		{
			re25 = 0;
		}
	}
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_OPENMTP, 0);
	TFA98XX_PRINT("MTPOTC :%d -- MTPEX:%d:--TFA2_BF_OPENMTP:%d\n", tfa_get_bf_tfa98xx(slave_address, TFA2_BF_MTPOTC), tfa_get_bf_tfa98xx(slave_address, TFA2_BF_MTPEX), tfa_get_bf_tfa98xx(slave_address, TFA2_BF_OPENMTP));
    return error;
}
#endif//

#ifdef TFA_ENABLE_LIVEDATA_CAPTURE
	Tfa9890_Error_t Tfa_get_LiveData(uint16_t slave_address, uint16_t iter_nb)
	{
        Tfa9890_Error_t error = Error_Ok;
		float live_data[150] = { 0 };
		error = tfa_dsp_msg_tfa98xxx(slave_address, setmemtrack_mt_len, setmemtrack_mt);
		assert(error == Error_Ok);
		tfa98xxRetrieveLiveData(slave_address, live_data);
		TFA98XX_SLEEP(500);
	}
#endif//TFA_ENABLE_LIVEDATA_CAPTURE
	Tfa9890_Error_t main(void)
	{
		Tfa9890_Error_t error = Error_Ok;
		const uint8_t tfa_d[] = { LEFT_AMP, RIGHT_AMP };
		uint8_t d = 0;
		uint8_t calibrateNeded = 0, iter = 10;
		for (d = 0; d < (sizeof(tfa_d) / sizeof(tfa_d[0])); d++)
		{
#ifdef START_CONFIG_TFA_BYPASS
			error = tfa_generic_bypass(tfa_d[d]);
			assert(error == Error_Ok);
#else		
			calibrateNeded = tfa_get_bf_tfa98xx(tfa_d[d], TFA2_BF_MTPEX);
				if (calibrateNeded)
				{
					error = Tfa_generic_start(tfa_d[d], 1);
					assert(error == Error_Ok);
					TFA98XX_SLEEP(1000);
					tfa_updown_tfa98xx(tfa_d[d], 1);
				}
			error = Tfa_generic_start(tfa_d[d], 0);
			assert(error == Error_Ok);
			TFA98XX_SLEEP(1000);
#endif		
		}
#ifdef TFA_ENABLE_LIVEDATA_CAPTURE 		
		for (d = 0; d < (sizeof(tfa_d) / sizeof(tfa_d[0])); d++)
		{
			error = Tfa_get_LiveData(tfa_d[d], iter);
			assert(error == Error_Ok);
		}
#endif//
		return error;
	}
