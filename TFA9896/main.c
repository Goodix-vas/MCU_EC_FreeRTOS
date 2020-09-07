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
#define TFA98XX_PATCH_EXTRA 6
Tfa9890_Error_t Tfa_generic_start(uint16_t slave_address, uint8_t calibrate)
{

	uint8_t factory_done = 0,trial = 10;
	uint16_t regRead = 0, tries = 0;
	uint32_t status;
	Tfa9890_Error_t error;
	int calibrateDone = 0;
#ifdef ENABLE_LIVEDATA_CAPTURE 
	Tfa98xx_StateInfoLive_t stateInfo;
	TFA9891SPKRBST_SpkrModel_t  recordInfo;
#endif//
	float re25;
	/*************************/
	error = tfa_set_bf(slave_address, TFA1_BF_I2CR, 1);
	assert(error == Error_Ok);
	error = tfa_set_bf(slave_address, TFA1_BF_RST, 1);
	assert(error == Error_Ok);
	error = write_register(slave_address, 0x06, (uint16_t)0x000b);
	assert(error == Error_Ok);
	error = write_register(slave_address, 0x07, (uint16_t)0x3e7f);
	assert(error == Error_Ok);
	error = write_register(slave_address, 0x0a, (uint16_t)0x0d8a);
	assert(error == Error_Ok);
	error = write_register(slave_address, 0x48, (uint16_t)0x0300);
	assert(error == Error_Ok);
	error = write_register(slave_address, 0x88, (uint16_t)0x0100);
	assert(error == Error_Ok);
	error = read_register(slave_address, 0x49, &regRead);
	regRead &= ~0x1;
	error = write_register(slave_address, 0x49, regRead);
	assert(error == Error_Ok);
	error = tfa_set_bf(slave_address, TFA1_BF_AUDFS, 8);//  32 FS(15)
	assert(error == Error_Ok);
	error = tfa_set_bf(slave_address, TFA1_BF_TDMSAMSZ, 23);//64 FS FS(15)
	assert(error == Error_Ok);
	error = tfa_set_bf(slave_address, TFA1_BF_NBCK, 2);//  64FS
	assert(error == Error_Ok);

	if (slave_address == TFA_LEFT_DEVICE)
	{
		error = tfa_set_bf(slave_address, TFA1_BF_CHS12, 1);//  
		assert(error == Error_Ok);
	}
	else  if (slave_address == TFA_RIGHT_DEVICE)
	{
		error = tfa_set_bf(slave_address, TFA1_BF_CHS12, 2);
		assert(error == Error_Ok);
	}

	error = tfa_set_bf(slave_address, TFA1_BF_CLIP, 1);// 
	assert(error == Error_Ok);
	error = tfa_set_bf(slave_address, TFA1_BF_ATTEN, 5);// 
	assert(error == Error_Ok);
	error = tfa_set_bf(slave_address, TFA1_BF_DCTRIP, 7);// 
	assert(error == Error_Ok);
	error = tfa_set_bf(slave_address, bst_slpcmplvl, 3);// 
	assert(error == Error_Ok);
	error = tfa_set_bf(slave_address, ignore_flag_voutcomp86, 0);// 
	assert(error == Error_Ok);
	error = tfa_set_bf(slave_address, TFA1_BF_DCPWM, 1);// 
	assert(error == Error_Ok);
	error = tfa_set_bf(slave_address, TFA1_BF_CFE, 1);//  
	assert(error == Error_Ok);
	error = tfa_set_bf(slave_address, TFA1_BF_CHSA, 2);// 
	assert(error == Error_Ok);
	error = tfa_set_bf(slave_address, TFA1_BF_AMPC, 1);//
	assert(error == Error_Ok);

	/*******PWUP**********/
	tfa98xx_powerdown(slave_address, 0);
	/* Check the PLL is powered up from status register 0*/
	read_register(slave_address, 0x00, &regRead);
	while ((regRead & 0x8000) == 0 && (tries < PLL_READ_TRIAL))
	{
		tries++;
		read_register(slave_address, 0x00, &regRead);
	}
	if (tries >= PLL_READ_TRIAL)
	{
		/* something wrong with communication with DSP */
		error = Error_StateTimedOut;
	}
	/*************************************/

		/*************Force Coldstart*************/
	while (tfa_get_bf(slave_address, TFA1_BF_ACS) != 1) {
		/*******************************************/
		error = tfa98xx_dsp_write_mem_word(slave_address, 0x8100, 1, DMEM_IOMEM);
		/*****************************************************/
		if (trial-- == 0) {
			return Error_StateTimedOut;;
		}
	}
	for (trial = 1; trial < PLL_READ_TRIAL; trial++) {
		error = tfa98xx_dsp_system_stable(slave_address, &status);
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
		TFA98XX_PRINT(" OK (trial=%d)\n", trial);
	factory_done = tfa_get_bf(slave_address, TFA1_BF_MTPEX);

	if (calibrate)
	{
		ClearOneTimeCalibration(slave_address);
		SetOneTimeCalibration(slave_address);
	}
	else if (!factory_done)
	{
		error = SetFactoryValues(slave_address);
		assert(error == Error_Ok);
	}
	error = checkICROMversion(slave_address, patch_patch);
	assert(error == Error_Ok);
	TFA98XX_PRINT("SC:%x PATCH loading Start--length to be  loaded:%d\n", slave_address, patch_patch_len);
	error = processPatchFile(slave_address, patch_patch_len - TFA98XX_PATCH_EXTRA, patch_patch + TFA98XX_PATCH_EXTRA);
	TFA98XX_PRINT(" PATCH OK \n");
	error = tfa98xx_dsp_write_mem_word(slave_address, 512, 0, DMEM_XMEM);
	assert(error == Error_Ok);


	/* Reset DSP once for sure after initializing */

	if (error == Error_Ok) {
		error = tfa_set_bf(slave_address, TFA1_BF_RST, 0);
		assert(error == Error_Ok);
	}
	error = tfa_dsp_write_tables96_97(slave_address);
	assert(error == Error_Ok);
	error = Tfa98xx_SetMute(slave_address, Mute_Digital);
	assert(error == Error_Ok);
	TFA98XX_PRINT("SC:%x CONFIG loading Start--length to be  loaded:%d\n", slave_address, config_config_len);
	error = Tfa98xx_DspWriteConfig(slave_address, config_config_len, config_config);

	assert(error == Error_Ok);
	TFA98XX_PRINT(" CONFIG OK \n");
	TFA98XX_PRINT("SC:%x Speaker loading Start--length to be  loaded:%d\n", slave_address, speaker_speaker_len);
	error = Tfa98xx_DspWriteSpeakerParameters(slave_address, speaker_speaker_len, speaker_speaker);
	assert(error == Error_Ok);
	TFA98XX_PRINT("SpeakerOk\n");

	TFA98XX_PRINT("SC:%x Preset loading Start--length to be  loaded:%d\n", slave_address, preset_preset_len);
	error = Tfa98xx_DspWritePreset(slave_address, preset_preset_len, preset_preset);
	assert(error == Error_Ok);
	TFA98XX_PRINT("PresetOk\n");
	/******************************/
	error = Tfa98xx_Write_FilterBank(slave_address, (TfaFilterM_t*)(/*Eq_eq*/ eq_eq));
	assert(error == Error_Ok);
	/*************Wait for AMPS*************/

	error = tfa_set_bf(slave_address, TFA1_BF_SBSL, 1);
	assert(error == Error_Ok);
	if (calibrate)
	{
		//Wait for calibration to complete
		wait4Calibration(slave_address, &calibrateDone);
		if (calibrateDone)
		{
			error = Tfa98xx_DspGetCalibrationImpedance(slave_address, &re25);
			assert(error == Error_Ok);
		}
		else
		{
			re25 = 0;
		}
	}

	TFA98XX_PRINT("TFA9896 0x%02X Calibration value is %2.2f ohm\n", slave_address, re25);


	Tfa9890_printCalibration(slave_address);
	error = Tfa98xx_SetMute(slave_address, Mute_Off);
	assert(error == Error_Ok);


    return error;	
}
#ifdef ENABLE_LIVEDATA_CAPTURE 
Tfa9890_Error_t Tfa_get_LiveData(uint16_t slave_address, uint16_t iter_nb)
{
	Tfa9890_Error_t error = Error_Ok;
	/*Starting to retrieve the live data info for 100 loop*/
	for (cnt = 0; cnt < iter_nb; ++cnt)
	{
		if (!cnt)
			TFA98XX_PRINT("Starting Live Data Check for Slave:%d\n", slave_address);
		error = Tfa98xx_DspGetStateInfo(slave_address, &stateInfo);
		assert(error == Error_Ok);
		error = Tfa98xxGetSpeakerModel(slave_address, 1, &recordInfo);
		assert(error == Error_Ok);
		dump_state_infoLive(&stateInfo);
		dump_speaker_model(&recordInfo, slave_address);
		TFA98XX_SLEEP(1000);

	}
	return error;
}
#endif//
Tfa9890_Error_t main(void)
{
	Tfa9890_Error_t error = Error_Ok;
	const uint8_t tfa_d[] = { TFA_LEFT_DEVICE, TFA_RIGHT_DEVICE };
	uint8_t d = 0;
	uint8_t calibrate = 1, iter = 10;
	for (d = 0; d < (sizeof(tfa_d) / sizeof(tfa_d[0])); d++)
	{
		error = Tfa_generic_start(tfa_d[d], calibrate);
		assert(error == Error_Ok);
	}
#ifdef ENABLE_LIVEDATA_CAPTURE 		
	for (d = 0; d < (sizeof(tfa_d) / sizeof(tfa_d[0])); d++)
	{
		error = Tfa_get_LiveData(tfa_d[d], iter);
		assert(error == Error_Ok);
	}
#endif//		
	return error;
}
