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


#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hal_registers.h"
/* translate a I2C driver error into an error for Tfa9887 API */
Tfa9890_Error_t classify_i2c_error(TFA_I2C_Error_t i2c_error)
{

#ifdef TFA98XX_PRINT_DEBUG
	if (i2c_error != TFA_I2C_Ok)
		TFA98XX_PRINT("i2c_error:%d\n", i2c_error);
#endif//TFA98XX_PRINT_DEBUG
	switch (i2c_error) {
	case TFA_I2C_Ok:
		return Error_Ok;
	case TFA_I2C_NoAck:
	case TFA_I2C_ArbLost:
	case TFA_I2C_TimeOut:
		return Error_I2C_NonFatal;
	default:
		return Error_I2C_Fatal;
	}
}
Tfa9890_Error_t read_register(uint8_t slave_address, uint8_t subaddress, unsigned short * value)
{
	TFA_I2C_Error_t i2c_error;
	uint8_t write_data[1]; /* subaddress */
	uint8_t read_buffer[2]; /* 2 data bytes */

	write_data[0] = subaddress;
	i2c_error = TFA_I2C_WriteRead(slave_address,
		sizeof(write_data), write_data, sizeof(read_buffer), read_buffer);

	if (classify_i2c_error(i2c_error) != Error_Ok) {
		return classify_i2c_error(i2c_error);
	}
	else {

		*value = (read_buffer[0] << 8) + read_buffer[1];
		return Error_Ok;
	}
}
int tfa98xx_read_reg(uint8_t slave_address, const uint16_t bf)
{
	Tfa9890_Error_t err = Error_Ok;
	uint16_t regvalue;

	/* bitfield enum - 8..15 : address */
	uint8_t address = (bf >> 8) & 0xff;

	err = read_register(slave_address, address, &regvalue);
	if (err)
		return -err;

	return regvalue;
}
Tfa9890_Error_t write_register(uint8_t slave_address, uint8_t subaddress, uint16_t value)
{
	uint8_t write_data[1 + 2]; /* subaddress */
	TFA_I2C_Error_t i2c_error;
	write_data[0] = subaddress;
	write_data[1] = (value >> 8);
	write_data[2] = value;
	i2c_error = TFA_I2C_WriteRead(slave_address,
		sizeof(write_data), write_data, 0, 0);
	return classify_i2c_error(i2c_error);
}
uint16_t tfa_get_bf_tfa98xx(uint8_t slave_address, const uint16_t bf)
{
	uint16_t regvalue, msk;
	uint16_t value;
	uint8_t len = bf & 0x0f;
	uint8_t pos = (bf >> 4) & 0x0f;
	uint8_t address = (bf >> 8) & 0xff;
	read_register(slave_address, address, &regvalue);
	msk = ((1 << (len + 1)) - 1) << pos;
	regvalue &= msk;
	value = regvalue >> pos;
	return value;
}
Tfa9890_Error_t tfa_set_bf_tfa98xx(uint8_t slave_address, const uint16_t bf, const uint16_t value)
{

	Tfa9890_Error_t err = Error_Ok;
	uint16_t regvalue, msk, oldvalue;
	uint8_t len = (bf & 0x0f);
	uint8_t pos = (bf >> 4) & 0x0f;
	uint8_t address = (bf >> 8) & 0xff;
	err = read_register(slave_address, address, &regvalue);
	oldvalue = regvalue;
	msk = ((1 << (len + 1)) - 1) << pos;
	regvalue &= ~msk;
	regvalue |= value << pos;

	if (oldvalue != regvalue)
		err = write_register(slave_address, (bf >> 8) & 0xff, regvalue);

	return err;


}
int tfa98xx_set_bf_value(uint8_t slave_address, const uint16_t bf, const uint16_t bf_value, uint16_t *p_reg_value)
{
	uint16_t regvalue, msk;

	/*
	 * bitfield enum:
	 * - 0..3  : len
	 * - 4..7  : pos
	 * - 8..15 : address
	 */
	uint8_t len = bf & 0x0f;
	uint8_t pos = (bf >> 4) & 0x0f;
	regvalue = *p_reg_value;
	msk = ((1 << (len + 1)) - 1) << pos;
	regvalue &= ~msk;
	regvalue |= bf_value << pos;
	*p_reg_value = regvalue;

	return 0;
}
/***********************/
Tfa9890_Error_t
Tfa98xx_ReadData(uint8_t slave_address,
	unsigned char subaddress, int num_bytes, unsigned char data[])
{
	TFA_I2C_Error_t i2c_error;
	const int bytes2write = 1;	/* subaddress */
	unsigned char write_data[1];
	if (num_bytes > MAX_I2C_LENGTH)
		return Error_Bad_Parameter;
	write_data[0] = subaddress;
	i2c_error =
		TFA_I2C_WriteRead(slave_address, bytes2write,
			write_data, num_bytes, data);
	if (classify_i2c_error(i2c_error) != Error_Ok)
		return classify_i2c_error(i2c_error);
	else
		return Error_Ok;
}
void tfa98xx_convert_data2bytes(int num_data, const int data[],
	unsigned char bytes[])
{
	int i;            /* index for data */
	int k;            /* index for bytes */
	int d;
	/* note: cannot just take the lowest 3 bytes from the 32 bit
	 * integer, because also need to take care of clipping any
	 * value > 2&23 */
	for (i = 0, k = 0; i < num_data; ++i, k += 3) {
		if (data[i] >= 0)
			d = MIN(data[i], (1 << 23) - 1);
		else {
			/* 2's complement */
			d = (1 << 24) - MIN(-data[i], 1 << 23);
		}
		bytes[k] = (d >> 16) & 0xFF;    /* MSB */
		bytes[k + 1] = (d >> 8) & 0xFF;
		bytes[k + 2] = (d) & 0xFF;    /* LSB */
	}
}

/**
 convert memory bytes to signed 24 bit integers
   input:  bytes contains "num_bytes" byte elements
   output: data contains "num_bytes/3" int24 elements
*/
void tfa98xx_convert_bytes2data(int num_bytes, const unsigned char bytes[],
	int data[])
{
	int i;            /* index for data */
	int k;            /* index for bytes */
	int d;
	int num_data = num_bytes / 3;
	//    _ASSERT((num_bytes % 3) == 0);
	for (i = 0, k = 0; i < num_data; ++i, k += 3) {
		d = (bytes[k] << 16) | (bytes[k + 1] << 8) | (bytes[k + 2]);
		//        _ASSERT(d >= 0);
		//        _ASSERT(d < (1 << 24));    /* max 24 bits in use */
		if (bytes[k] & 0x80)    /* sign bit was set */
			d = -((1 << 24) - d);

		data[i] = d;
	}
}
#if 1
Tfa9890_Error_t
processPatchFile(uint8_t slave_address, int length,
	const unsigned char *bytes)
{
	unsigned short size;
	int index;

	TFA_I2C_Error_t i2c_error = TFA_I2C_Ok;
	/* expect following format in patchBytes:
	 * 2 bytes length of I2C transaction in little endian, then the bytes, excluding the slave address which is added from the handle
	 * This repeats for the whole file
	 */
	index = 0;
	while (index < length) {
		/* extract little endian length */
		size = bytes[index] + bytes[index + 1] * 256;
		index += 2;
		if ((index + size) > length) {
			/* too big, outside the buffer, error in the input data */
			return Error_Bad_Parameter;
		}
		if ((size + 1) > MAX_I2C_LENGTH) {
			/* too big, must fit buffer */
			return Error_Bad_Parameter;
		}
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("Bytes to be written_patch:%d\n", size);
#endif//
		i2c_error = TFA_I2C_WriteRead(slave_address, size, bytes + index, 0, 0);

		if (i2c_error != TFA_I2C_Ok) {
			break;
		}
		index += size;
	}
	return classify_i2c_error(i2c_error);
}
#endif//
uint16_t tfa_get_bf_value(const uint16_t bf, const uint16_t reg_value)
{
	uint16_t msk, value;

	/*
	 * bitfield enum:
	 * - 0..3  : len
	 * - 4..7  : pos
	 * - 8..15 : address
	 */
	uint8_t len = bf & 0x0f;
	uint8_t pos = (bf >> 4) & 0x0f;

	msk = ((1 << (len + 1)) - 1) << pos;
	value = (reg_value & msk) >> pos;

	return value;
}
int tfa_write_reg_tfa98xx(uint8_t slave_address, const uint16_t bf, const uint16_t reg_value)
{
	Tfa9890_Error_t err;

	/* bitfield enum - 8..15 : address */
	uint8_t address = (bf >> 8) & 0xff;

	err = write_register(slave_address, address, reg_value);
	if (err)
		return -err;

	return 0;
}
/***************************** xmem only **********************************/
Tfa9890_Error_t
tfa98xx_dsp_read_mem_tfa98xx(uint8_t slave_address,
	unsigned int start_offset, int num_words, int *pValues)
{
	Tfa9890_Error_t  error = Error_Ok;
	unsigned char *bytes;
	int burst_size;		/* number of words per burst size */
	const int bytes_per_word = 3;
	int dmem;
	int num_bytes;
	int *p;
	bytes = (unsigned char *)malloc(num_words*bytes_per_word);
	if (bytes == NULL)
		return Error_Other;

	/* If no offset is given, assume XMEM! */
	if (((start_offset >> 16) & 0xf) > 0)
		dmem = (start_offset >> 16) & 0xf;
	else
		dmem = DMEM_XMEM;

	/* Remove offset from adress */
	start_offset = start_offset & 0xffff;
	num_bytes = num_words * bytes_per_word;
	p = pValues;

	tfa_set_bf_tfa98xx(slave_address, TFA2_BF_DMEM, (uint16_t)dmem);
	error = -tfa_write_reg_tfa98xx(slave_address, TFA2_BF_MADD, (unsigned short)start_offset);
	if (error != Error_Ok)
		goto tfa98xx_dsp_read_mem_exit;
	for (; num_bytes > 0;) {
		burst_size = ROUND_DOWN(TFA_I2C_MAX_SIZE, bytes_per_word);
		if (num_bytes < burst_size)
			burst_size = num_bytes;
		//_ASSERT(burst_size <= sizeof(bytes));
		error = Tfa98xx_ReadData(slave_address, FAM_TFA98XX_CF_MEM, burst_size, bytes);
		if (error != Error_Ok)
			goto tfa98xx_dsp_read_mem_exit;

		tfa98xx_convert_bytes2data(burst_size, bytes, p);

		num_bytes -= burst_size;
		p += burst_size / bytes_per_word;
	}

tfa98xx_dsp_read_mem_exit:
	free(bytes);
	return error;
}
Tfa9890_Error_t
checkICROMversion(uint8_t slave_address, const unsigned char patchheader[])
{
	Tfa9890_Error_t error = Error_Ok;
	unsigned short checkrev;
	unsigned short checkaddress;
	int checkvalue;
	int value = 0;
	unsigned short status;
	checkrev = patchheader[0];
	if ((checkrev != 0xFF) && (checkrev != 0x13)) {
		return Error_Not_Supported;
	}
	checkaddress = (patchheader[1] << 8) + patchheader[2];
	checkvalue =
		(patchheader[3] << 16) + (patchheader[4] << 8) + patchheader[5];
	if (checkaddress != 0xFFFF) {
		/* before reading XMEM, check if we can access the DSP */
		error = read_register(slave_address, 0x00, &status);
		if (error == Error_Ok) {
			if ((status & 0x8000) != 0x8000) {
				/* one of Vddd, PLL and clocks not ok */
				error = Error_DSP_not_running;
			}
		}
		/* read some address register that allows checking the correct ROM version */
		if (error == Error_Ok) {
			error =
				tfa98xx_dsp_read_mem_tfa98xx(slave_address, checkaddress, 1, &value);
		}
		if (error == Error_Ok) {
			if (value != checkvalue) {
				error = Error_Not_Supported;
			}
		}
	}
	return error;
}
Tfa9890_Error_t
tfa_dsp_stability_check(uint8_t slave_address,
	int *ready)
{
	Tfa9890_Error_t  error = Error_Ok;
	unsigned short status;
	int value;

	/* check the contents of the STATUS register */
	value = tfa98xx_read_reg(slave_address, TFA2_BF_AREFS);
	if (value < 0) {
		error = -value;
		*ready = 0;
		//_ASSERT(error);		/* an error here can be fatal */
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("FATAL ERROR#######\n");
#endif//TFA98XX_PRINT_DEBUG
		return error;
	}
	status = (unsigned short)value;

	/* check AREFS and CLKS: not ready if either is clear */
	*ready = !((tfa_get_bf_value(TFA2_BF_AREFS, status) == 0)
		|| (tfa_get_bf_value(TFA2_BF_CLKS, status) == 0)
		|| (tfa_get_bf_value(TFA2_BF_MTPB, status) == 1));

	return error;
}
void tfa_updown_tfa98xx(uint8_t slave_address, int powerdown)
{
	tfa_set_bf_tfa98xx(slave_address, TFA2_BF_PWDN, (uint16_t)powerdown);
}
Tfa9890_Error_t  tfa_mute_tfa98xx(uint8_t slave_address, Tfa9891_Mute_t mute)
{
	Tfa9890_Error_t error;

	switch (mute) {
	case Mute_Off:
		error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_CFSM, (uint16_t)0);
		break;
	case Mute_Digital:
		/* expect the amplifier to run */
		/* set the cf_mute bit */
		error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_CFSM, (uint16_t)1);
		break;
	default:
		error = Error_Bad_Parameter;
	}


	return error;
}
Tfa9890_Error_t
tfa98xx_write_data_tfa98xx(uint8_t slave_address,
	unsigned char subaddress, int num_bytes,
	const unsigned char data[])
{
	TFA_I2C_Error_t i2c_error;
	const int bytes2write = num_bytes + 1;
	unsigned char *write_data;

	write_data = (unsigned char *)malloc(bytes2write);
	if (write_data == NULL)
		return Error_Other;

	write_data[0] = subaddress;
	memcpy(&write_data[1], data, num_bytes);
	i2c_error = TFA_I2C_WriteRead(slave_address, bytes2write, write_data, 0, NULL);
	free(write_data);
	return classify_i2c_error(i2c_error);
}
Tfa9890_Error_t
tfa98xx_dsp_write_mem_word_tfa98xx(uint8_t slave_address, unsigned short address, int value, int memtype)
{
	Tfa9890_Error_t error = Error_Ok;
	unsigned char bytes[3];

	tfa_set_bf_tfa98xx(slave_address, TFA2_BF_DMEM, (uint16_t)memtype);
	error = -tfa_write_reg_tfa98xx(slave_address, TFA2_BF_MADD, address);
	if (error != Error_Ok)
		return error;
	tfa98xx_convert_data2bytes(1, &value, bytes);
	error = tfa98xx_write_data_tfa98xx(slave_address, 0x920f >> 8, 3, bytes);

	return error;
}
Tfa9890_Error_t  tfa_dsp_msg_write_id_tfa98xx(uint8_t slave_address, int length, const char *buffer, uint8_t cmdid[3])
{
	int offset = 0;
	int chunk_size = ROUND_DOWN(TFA_I2C_MAX_SIZE, 3 /* XMEM word size */);

	int remaining_bytes = length;
	Tfa9890_Error_t error = Error_Ok;
	uint16_t cfctl;
	int value;

	value = tfa98xx_read_reg(slave_address, TFA2_BF_DMEM);
	if (value < 0) {
		error = -value;
		return error;
	}
	cfctl = (uint16_t)value;
	/* assume no I2C errors from here */
	tfa98xx_set_bf_value(slave_address, TFA2_BF_DMEM, (uint16_t)DMEM_XMEM, &cfctl); /* set cf ctl to DMEM  */
	tfa98xx_set_bf_value(slave_address, TFA2_BF_AIF, 0, &cfctl); /* set to autoincrement */
	tfa_write_reg_tfa98xx(slave_address, TFA2_BF_DMEM, cfctl);
	/* xmem[1] is start of message
	 *  direct write to register to save cycles avoiding read-modify-write
	 */
	tfa_write_reg_tfa98xx(slave_address, TFA2_BF_MADD, 1);
	/* write cmd-id */
	if (cmdid != NULL)
		error = tfa98xx_write_data_tfa98xx(slave_address, FAM_TFA98XX_CF_MEM, 3, (const unsigned char *)cmdid);

	/* due to autoincrement in cf_ctrl, next write will happen at
	 * the next address */
	while ((error == Error_Ok) && (remaining_bytes > 0)) {
		if (remaining_bytes < chunk_size)
			chunk_size = remaining_bytes;
		/* else chunk_size remains at initialize value above */
		error = tfa98xx_write_data_tfa98xx(slave_address, FAM_TFA98XX_CF_MEM,
			chunk_size, (const unsigned char *)buffer + offset);
		remaining_bytes -= chunk_size;
		offset += chunk_size;
	}

	/* notify the DSP */
	if (error == Error_Ok) {
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("Probably here 70 71 72, flow__Flow\n");
#endif//TFA98XX_PRINT_DEBUG
		/* cf_int=0, cf_aif=0, cf_dmem=XMEM=01, cf_rst_dsp=0 */
		/* set the cf_req1 and cf_int bit */
		tfa98xx_set_bf_value(slave_address, TFA2_BF_REQCMD, 0x01, &cfctl); /* bit 0 */
		tfa98xx_set_bf_value(slave_address, TFA2_BF_CFINT, 1, &cfctl);
		error = -tfa_write_reg_tfa98xx(slave_address, TFA2_BF_CFINT, cfctl);
	}

	return error;
}
Tfa9890_Error_t
tfa98xx_wait_result_tfa98xx(uint8_t slave_address, int wait_retry_count)
{
	Tfa9890_Error_t  error = Error_Ok;
	int cf_status; /* the contents of the CF_STATUS register */
	int tries = 0;
	do {
		cf_status = tfa_get_bf_tfa98xx(slave_address, TFA2_BF_ACK);
		if (cf_status < 0)
			error = -cf_status;
		tries++;
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("We are inside tfa98xx_wait_result_tfa98xx,triesCF:%d--error:%d--cf_status & 0x01:%d\n", tries, error, cf_status & 0x01);
#endif//
	}
	// i2c_cmd_ack
	/* don't wait forever, DSP is pretty quick to respond (< 1ms) */
	while ((error == Error_Ok) && ((cf_status & 0x01) == 0)
		&& (tries < wait_retry_count));
	if (tries >= wait_retry_count) {
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("Returning Error_DSP_not_running\n");
#endif//TFA98XX_PRINT_DEBUG
		/* something wrong with communication with DSP */
		error = Error_DSP_not_running;
	}
	return error;
}
/* read the return code for the RPC call */
Tfa9890_Error_t
tfa98xx_check_rpc_status_tfa98xx(uint8_t slave_address, int *pRpcStatus)
{
	Tfa9890_Error_t  error = Error_Ok;
	/* the value to sent to the * CF_CONTROLS register: cf_req=00000000,
	 * cf_int=0, cf_aif=0, cf_dmem=XMEM=01, cf_rst_dsp=0 */
	unsigned short cf_ctrl = 0x0002;
	/* memory address to be accessed (0: Status, 1: ID, 2: parameters) */
	unsigned short cf_mad = 0x0000;
	if (pRpcStatus == 0)
		return Error_Bad_Parameter;
	/* 1) write DMEM=XMEM to the DSP XMEM */
	{
		/* minimize the number of I2C transactions by making use of the autoincrement in I2C */
		unsigned char buffer[4];
		/* first the data for CF_CONTROLS */
		buffer[0] = (unsigned char)((cf_ctrl >> 8) & 0xFF);
		buffer[1] = (unsigned char)(cf_ctrl & 0xFF);
		/* write the contents of CF_MAD which is the subaddress following CF_CONTROLS */
		buffer[2] = (unsigned char)((cf_mad >> 8) & 0xFF);
		buffer[3] = (unsigned char)(cf_mad & 0xFF);
		error = tfa98xx_write_data_tfa98xx(slave_address, FAM_TFA98XX_CF_CONTROLS, sizeof(buffer), buffer);
	}
	if (error == Error_Ok) {
		/* read 1 word (24 bit) from XMEM */
		error = tfa98xx_dsp_read_mem_tfa98xx(slave_address, 0, 1, pRpcStatus);
	}
	return error;
}
Tfa9890_Error_t tfa_dsp_msg_status_tfa98xx(uint8_t slave_address, int *pRpcStatus)
{
	Tfa9890_Error_t error = Error_Ok;
	error = tfa98xx_wait_result_tfa98xx(slave_address, 10); /* 10 to avoid DSP message execution latency*/
	if (error == Error_DSP_not_running) {
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("return_not_running\n");
#endif//
		*pRpcStatus = -1;
		return Error_Ok;
	}
	else if (error != Error_Ok)
	{
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("Error NtOk_NOOk:%d\n", error);
#endif//
		return error;
	}
	error = tfa98xx_check_rpc_status_tfa98xx(slave_address, pRpcStatus);
#ifdef TFA98XX_PRINT_DEBUG
	TFA98XX_PRINT("rpcStatus:%d\n", *pRpcStatus);
#endif//
	return error;
}
Tfa9890_Error_t tfa_dsp_msg_tfa98xxx(uint8_t slave_address, int length, const char *buf)
{
	Tfa9890_Error_t error;
	int tries, rpc_status = I2C_Req_Done;

	/* write the message and notify the DSP */
	error = tfa_dsp_msg_write_id_tfa98xx(slave_address, length, buf, NULL);
	if (error != Error_Ok)
		return error;
#ifdef TFA98XX_PRINT_DEBUG
	else
		TFA98XX_PRINT("tfa_dsp_msg_write_id_tfa98xx error :%d\n", error);
#endif//

	/* get the result from the DSP (polling) */
	for (tries = TFA98XX_WAITRESULT_NTRIES; tries > 0; tries--) {
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("tries:%d\n", tries);
#endif//
		error = tfa_dsp_msg_status_tfa98xx(slave_address, &rpc_status);
		if (error == Error_Ok && rpc_status == I2C_Req_Done)
			break;
#ifdef TFA98XX_PRINT_DEBUG
		else
			TFA98XX_PRINT("error_status:%d--rpc_status:%d\n", error, rpc_status);
#endif//

		/* If the rpc status is a specific error we want to know it.
		 * If it is busy or not running it should retry
		 */
		if (rpc_status != I2C_Req_Busy && rpc_status != DSP_Not_Running)
		{
#ifdef TFA98XX_PRINT_DEBUG
			TFA98XX_PRINT("breaking------error\n");
#endif//
			break;
		}
	}

	if (rpc_status != I2C_Req_Done) {
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("breaking2------error2\n");
#endif//
		/* DSP RPC call returned an error */
		error = (Tfa9890_Error_t)(rpc_status + Error_RpcBase);
		// pr_debug("DSP msg status: %d (%s)\n", rpc_status, tfa98xx_get_i2c_status_id_string(rpc_status));
	}

	return error;
}
Tfa9890_Error_t tfa98xx_dsp_msg_read(uint8_t slave_address, int length, unsigned char *bytes)
{
	Tfa9890_Error_t error = Error_Ok;
	int burst_size;		/* number of words per burst size */
	int bytes_per_word = 3;
	int num_bytes;
	int offset = 0;
	unsigned short start_offset = 2; /* msg starts @xmem[2] ,[1]=cmd */


	tfa_set_bf_tfa98xx(slave_address, TFA2_BF_DMEM, (uint16_t)DMEM_XMEM);
	error = -tfa_write_reg_tfa98xx(slave_address, TFA2_BF_MADD, start_offset);
	if (error != Error_Ok)
		return error;

	num_bytes = length; /* input param */
	while (num_bytes > 0) {
		burst_size = ROUND_DOWN(TFA_I2C_MAX_SIZE, 3);
		if (num_bytes < burst_size)
			burst_size = num_bytes;
		error = Tfa98xx_ReadData(slave_address, FAM_TFA98XX_CF_MEM, burst_size, bytes + offset);
		if (error != Error_Ok)
			return error;

		num_bytes -= burst_size;
		offset += burst_size;
	}

	return error;
}
Tfa9890_Error_t   tfa98xx_dsp_cmd_id_write_read(uint8_t slave_address,
	unsigned char module_id,
	unsigned char param_id, int num_bytes,
	unsigned char data[])
{
	Tfa9890_Error_t error;
	unsigned char buffer[3];

	buffer[0] = 1;
	buffer[1] = module_id + 128;
	buffer[2] = param_id;


	error = tfa_dsp_msg_tfa98xxx(slave_address, sizeof(unsigned char[3]), (char *)buffer);
	if (error != Error_Ok)
		return error;

	/* read the data from the dsp */
	error = tfa98xx_dsp_msg_read(slave_address, num_bytes, data);
	return error;
}
Tfa9890_Error_t Tfa9890_printCalibration(uint8_t slave_address)
{
	Tfa9890_Error_t error = Error_Ok;
	unsigned char bytes[6] = { 0 };
	int24 data[2];
	float mohm;
	error = tfa98xx_dsp_cmd_id_write_read(slave_address, MODULE_SPEAKERBOOST, SB_PARAM_GET_RE0, 3, bytes);
	assert(error == Error_Ok);


	if (error == Error_Ok) {
		tfa98xx_convert_bytes2data(3, bytes, data);
		mohm = ((float)data[0] * 1024) / TFA2_FW_ReZ_SCALE;
	}
	TFA98XX_PRINT(" Calibrated value :%1.2f mOhms :\n", mohm);

	return error;
}

void tfa2_manual_mtp_cpy(uint8_t slave_address, uint16_t reg_row_to_keep, uint16_t reg_row_to_set, uint8_t row)
{
	uint16_t value;
	int loop = 0;
	Tfa9890_Error_t err = Error_Ok;

	write_register(slave_address, (unsigned char)reg_row_to_keep, &value);
	if (!row)
	{
		write_register(slave_address, 0xA7, value);
		write_register(slave_address, 0xA8, reg_row_to_set);
	}
	else
	{
		write_register(slave_address, 0xA7, reg_row_to_set);
		write_register(slave_address, 0xA8, value);
	}
	write_register(slave_address, 0xA3, 0x10 | row);
}

Tfa9890_Error_t ClearOneTimeCalibration(uint8_t slave_address)
{
	Tfa9890_Error_t err = Error_Ok;
	unsigned short mtp;
	int tries = 0, mtpBusy = 0;
	//Unlock MTP register by writing 0x005A to register 0x0B
	err = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_SSFAIME, 1);
	assert(err == Error_Ok);
	write_register(slave_address, 0x0F, (uint16_t)0x5A6B);
	write_register(slave_address, 0xA1, (uint16_t)0x005A);
	write_register(slave_address, 0x0F, (uint16_t)0);
	//Read MTP register
	read_register(slave_address, 0xF0, &mtp);
	//Set MTPEX=0 and MTPOTC=0 to clear current calibration status
	mtp &= ~(0x03);
	write_register(slave_address, 0xF0, mtp);
	/* err = tfa_set_bf_tfa98xx(slave_address,TFA2_BF_CIMTP,1); */
	/* assert(err == Error_Ok); */
	tfa2_manual_mtp_cpy(slave_address, 0xF1, mtp, 0);
	write_register(slave_address, 0x0F, (uint16_t)0x5A6B);
	write_register(slave_address, 0xA1, (uint16_t)0);
	write_register(slave_address, 0x0F, (uint16_t)0);
	assert(err == Error_Ok);
	do
	{
		tries++;
		TFA98XX_SLEEP(10);
		mtpBusy = tfa_get_bf_tfa98xx(slave_address, TFA2_BF_MTPB);
	} while ((mtpBusy == 0x01) && (tries < TFA98XX_API_WAITRESULT_NTRIES));

	if (tries == TFA98XX_API_WAITRESULT_NTRIES)
	{
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("MTP Busy timedout\n");
#endif//TFA98XX_PRINT_DEBUG
		err = Error_StateTimedOut;
	}
}
void SetOneTimeCalibration(uint8_t slave_address)
{
	Tfa9890_Error_t err = Error_Ok;
	unsigned short mtp;
	int tries = 0, mtpBusy = 0;
	err = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_SSFAIME, 1);
	assert(err == Error_Ok);
	//Unlock MTP register by writing 0x005A to register 0x0B
	write_register(slave_address, 0x0F, (uint16_t)0x5A6B);
	write_register(slave_address, 0xA1, (uint16_t)0x005A);
	write_register(slave_address, 0x0F, (uint16_t)0);
	read_register(slave_address, 0xF0, &mtp);
	//Clear MTPEX
	mtp |= 0x01;
	write_register(slave_address, 0xF0, mtp);
	/* err = tfa_set_bf_tfa98xx(slave_address,TFA2_BF_CIMTP,1); */
	/* assert(err == Error_Ok); no return from manual_mtp_cpy() */
	tfa2_manual_mtp_cpy(slave_address, 0xF1, mtp, 0);
	write_register(slave_address, 0x0F, (uint16_t)0x5A6B);
	write_register(slave_address, 0xA1, (uint16_t)0);
	write_register(slave_address, 0x0F, (uint16_t)0);
	tries = 0;
	do
	{
		tries++;
		TFA98XX_SLEEP(10);
		mtpBusy = tfa_get_bf_tfa98xx(slave_address, TFA2_BF_MTPB);
	} while ((mtpBusy == 0x01) && (tries < TFA98XX_API_WAITRESULT_NTRIES));

	if (tries == TFA98XX_API_WAITRESULT_NTRIES)
	{
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("MTP Busy timedout\n");
#endif//TFA98XX_PRINT_DEBUG
		err = Error_StateTimedOut;
	}

}
Tfa9890_Error_t tfa_dsp_cmd_id_write_tfa98xx(uint8_t slave_address,
	unsigned char module_id,
	unsigned char param_id, int num_bytes,
	const unsigned char data[])
{
	Tfa9890_Error_t error;
	unsigned char *buffer;

	buffer = malloc(3 + num_bytes);
	if (buffer == NULL)
		return Error_Other;

	buffer[0] = 1;
	buffer[1] = module_id + 128;
	buffer[2] = param_id;

	memcpy(&buffer[3], data, num_bytes);

	error = tfa_dsp_msg_tfa98xxx(slave_address, 3 + num_bytes, (char *)buffer);

	free(buffer);

	return error;
}
Tfa9890_Error_t  tfaRunWriteBitfieldTfa98xx(uint8_t slave_address,TfaBitfieldTfa98xx_t bf) {
	Tfa9890_Error_t error;
	uint16_t value;
	union {
		uint16_t field;
		TfaBfEnumTfa98xx_t Enum;
	} bfUni;

	value = bf.value;
	bfUni.field = bf.field;
	error = tfa_set_bf_tfa98xx(slave_address, bfUni.field, value);

	return error;
}
#ifdef TFA98XX_PRINT_DEBUG
void printArray(char *ptr, size_t size)
{
	char *const end = ptr + size;
	while (ptr < end) {
		TFA98XX_PRINT("[%x]-", *ptr++);
	}
}
#endif//TFA98XX_PRINT_DEBUG
/**********************************/
Tfa9890_Error_t  tfa_dsp_msg_id_tfa98xx(uint8_t slave_address, int length, const char *buf, uint8_t cmdid[3])
{
	Tfa9890_Error_t error;
	int tries, rpc_status = I2C_Req_Done;

	/* write the message and notify the DSP */
	error = tfa_dsp_msg_write_id_tfa98xx(slave_address, length, buf, cmdid);
	if (error != Error_Ok)
	{
		TFA98XX_PRINT(" Return error-----error\n");
		return error;
	}
	/* get the result from the DSP (polling) */
	for (tries = 40; tries > 0; tries--) {
		error = tfa_dsp_msg_status_tfa98xx(slave_address, &rpc_status);
		if (error == Error_Ok && rpc_status == I2C_Req_Done)
			break;
	}
	if (rpc_status != I2C_Req_Done) {
		/* DSP RPC call returned an error */
		error = (Tfa9890_Error_t)(rpc_status + Error_RpcBase);
		//pr_debug("DSP msg status: %d (%s)\n", rpc_status, tfa98xx_get_i2c_status_id_string(rpc_status));
	}

	return error;
}
/*****************************/
Tfa9890_Error_t  tfaContWriteVstepMax2_tfa98xx(uint8_t slave_address, TfaVolumeStepMax2FileTfa98xx_t *vp, int vstep_idx, int vstep_msg_idx)
{
	Tfa9890_Error_t err = Error_Ok;
	struct TfaVolumeStepRegisterInfoTfa98xx *regInfo = { 0 };
	struct TfaVolumeStepMessageInfoTfa98xx *msgInfo = { 0 };
	TfaBitfieldTfa98xx_t bitF;
	int msgLength = 0, i, j, size = 0, nrMessages, modified = 0;
	uint8_t cmdid_changed[3];
#ifdef TFA98XX_PRINT_DEBUG
	printArray(vp, 50);
#endif//TFA98XX_PRINT_DEBUG
	if (vstep_idx >= vp->NrOfVsteps) {
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("Volumestep %d is not available \n", vstep_idx);
#endif//
		return Error_Bad_Parameter;
	}
#ifdef TFA98XX_PRINT_DEBUG
	else
		TFA98XX_PRINT("vp->NrOfVsteps:%d---vstep_idx %d--sizeof(TfaHeaderTfa98xx_t):%d\n", vp->NrOfVsteps, vstep_idx, sizeof(TfaHeaderTfa98xx_t));
#endif
	for (i = 0; i <= vstep_idx; i++) {
		regInfo = (struct TfaVolumeStepRegisterInfoTfa98xx*)(vp->vstepsBin + size);
		msgInfo = (struct TfaVolumeStepMessageInfoTfa98xx*)(vp->vstepsBin +
			(regInfo->NrOfRegisters * sizeof(uint32_t) + sizeof(regInfo->NrOfRegisters) + size));

		nrMessages = msgInfo->NrOfMessages;
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("nrMessages:%d\n", nrMessages);
#endif//
		for (j = 0; j < nrMessages; j++) {
			/* location of message j, from vstep i */
			msgInfo = (struct TfaVolumeStepMessageInfoTfa98xx*)(vp->vstepsBin +
				(regInfo->NrOfRegisters * sizeof(uint32_t) + sizeof(regInfo->NrOfRegisters) + size));
			/* message length */
			msgLength = ((msgInfo->MessageLength.b[0] << 16) + (msgInfo->MessageLength.b[1] << 8) + msgInfo->MessageLength.b[2]);
#ifdef TFA98XX_PRINT_DEBUG
			TFA98XX_PRINT("nrMessages:%d--msgLength:%d--MessageType:%d\n", j, msgLength, msgInfo->MessageType);
#endif//
			if (i == vstep_idx) {
				/* If no vstepMsgIndex is passed on, all message needs to be send */
				if ((vstep_msg_idx >= TFA_MAX_VSTEP_MSG_MARKER) || (vstep_msg_idx == j)) {
					/*
					 * The algoparams and mbdrc msg id will be changed to the reset type when SBSL=0
					 * if SBSL=1 the msg will remain unchanged. It's up to the tuning engineer to choose the 'without_reset'
					 * types inside the vstep. In other words: the reset msg is applied during SBSL==0 else it remains unchanged.
					 */
					if (tfa_get_bf_tfa98xx(slave_address, TFA2_BF_SBSL) == 0) {
						if (msgInfo->MessageType == 0) { /* If the messagetype(0) is AlgoParams */
#ifdef TFA98XX_PRINT_DEBUG
							TFA98XX_PRINT("SBSL NULL MessageType NULL\n");
#endif//
							/* Only do this when not set already */
							if (msgInfo->CmdId[2] != SB_PARAM_SET_ALGO_PARAMS) {
#ifdef TFA98XX_PRINT_DEBUG
								TFA98XX_PRINT("CmdId[2] != SB_PARAM_SET_ALGO_PARAMS\n");
#endif//
								cmdid_changed[0] = msgInfo->CmdId[0];
								cmdid_changed[1] = msgInfo->CmdId[1];
								cmdid_changed[2] = SB_PARAM_SET_ALGO_PARAMS;
								modified = 1;
							}
#ifdef TFA98XX_PRINT_DEBUG
							else
								TFA98XX_PRINT("CmdId[2] == SB_PARAM_SET_ALGO_PARAMS\n");
#endif//
						}
						else if (msgInfo->MessageType == 2) { /* If the messagetype(2) is MBDrc */
						 /* Only do this when not set already */
#ifdef TFA98XX_PRINT_DEBUG
							TFA98XX_PRINT("SBSL NULL MessageType 2-2-2\n");
#endif//
							if (msgInfo->CmdId[2] != SB_PARAM_SET_MBDRC) {
#ifdef TFA98XX_PRINT_DEBUG
								TFA98XX_PRINT("msgInfo->CmdId[2] != SB_PARAM_SET_MBDRC\n");
#endif//
								cmdid_changed[0] = msgInfo->CmdId[0];
								cmdid_changed[1] = msgInfo->CmdId[1];
								cmdid_changed[2] = SB_PARAM_SET_MBDRC;
								modified = 1;
							}
#ifdef TFA98XX_PRINT_DEBUG
							else
								TFA98XX_PRINT("msgInfo->CmdId[2] == SB_PARAM_SET_MBDRC\n");
#endif//
						}
					}
#ifdef TFA98XX_PRINT_DEBUG
					else
						TFA98XX_PRINT("SBSL TRUE--TRUE\n");
#endif//
					/* Messagetype(3) is Smartstudio Info! Dont send this! */
					if (msgInfo->MessageType != 3) {
						if (modified == 1) {
#ifdef TFA98XX_PRINT_DEBUG
							if (cmdid_changed[2] == SB_PARAM_SET_ALGO_PARAMS)
								TFA98XX_PRINT("P-ID for SetAlgoParams modified!: ");
							else
								TFA98XX_PRINT("P-ID for SetMBDrc modified!: ");

							TFA98XX_PRINT("Command-ID used: 0x%02x%02x%02x \n",
								cmdid_changed[0], cmdid_changed[1], cmdid_changed[2]);
							/* Send payload to dsp (Remove 1 from the length for cmdid) */
							TFA98XX_PRINT("Send Payload DSP :tfa_dsp_msg_id:((msgLength-1) * 3) %d:\n", (msgLength - 1) * 3);
#endif//
							err = tfa_dsp_msg_id_tfa98xx(slave_address, (msgLength - 1) * 3, (const char *)msgInfo->ParameterData, cmdid_changed);
							if (err != Error_Ok)
								return err;
						}
						else {
							/* Send cmdId + payload to dsp */
#ifdef TFA98XX_PRINT_DEBUG
							TFA98XX_PRINT("Send cmdId + payload to dsp :tfa_dsp_msg_id:(msgLength * 3) %d:\n", msgLength * 3);
#endif//
							err = tfa_dsp_msg_tfa98xxx(slave_address, msgLength * 3, (const char *)msgInfo->CmdId);
							//dsp_msg(devs[0], msgLength * 3,(const char *)msgInfo->CmdId);
							if (err != Error_Ok)
								return err;
						}

						/* Set back to zero every time */
						modified = 0;
					}
				}
			}

			if (msgInfo->MessageType == 3) {
				/* MessageLength is in bytes */
				size += sizeof(msgInfo->MessageType) + sizeof(msgInfo->MessageLength) + msgLength;
			}
			else {
				/* MessageLength is in words (3 bytes) */
				size += sizeof(msgInfo->MessageType) + sizeof(msgInfo->MessageLength) + sizeof(msgInfo->CmdId) + ((msgLength - 1) * 3);
			}
		}
		size += sizeof(regInfo->NrOfRegisters) + (regInfo->NrOfRegisters * sizeof(uint32_t)) + sizeof(msgInfo->NrOfMessages);
	}

	if (regInfo->NrOfRegisters == 0) {
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("No registers in selected vstep (%d)!\n", vstep_idx);
#endif//
		return Error_Bad_Parameter;
	}

	for (i = 0; i < regInfo->NrOfRegisters * 2; i++) {
		/* Byte swap the datasheetname */
		bitF.field = (uint16_t)(regInfo->registerInfo[i] >> 8) | (regInfo->registerInfo[i] << 8);
		i++;
		bitF.value = (uint16_t)regInfo->registerInfo[i] >> 8;
#ifdef TFA98XX_PRINT_DEBUG
		TFA98XX_PRINT("bitF.field:%x--bitF.value:%x\n", bitF.field, bitF.value);
#endif//
		err = tfaRunWriteBitfieldTfa98xx(slave_address, bitF);
		if (err != Error_Ok)
			return err;
	}
	return err;
}
Tfa9890_Error_t  tfa_write_tables_tfa98xx(uint8_t slave_address)
{

	unsigned char buffer[15] = { 0 };
	int size = 15 * sizeof(char);
	Tfa9890_Error_t error = Error_Ok;
	error = tfa_set_bf_tfa98xx(slave_address, TFA2_BF_FRACTDEL, 46);
	assert(error == Error_Ok);

	buffer[0] = (uint8_t)0;
	buffer[1] = (uint8_t)MODULE_FRAMEWORK + 128;
	buffer[2] = (uint8_t)FW_PAR_ID_SET_SENSES_DELAY;
	buffer[5] = 1;	/* Vdelay_P */
	buffer[8] = 0;	/* Idelay_P */
	buffer[11] = 1; /* Vdelay_S */
	buffer[14] = 0; /* Idelay_S */


/* send SetSensesDelay msg */
	return tfa_dsp_msg_tfa98xxx(slave_address, size, (char *)buffer);

}


Tfa9890_Error_t
tfa98xxdsp_get_calibration_impedance(uint8_t slave_address, float *pRe25)
{
	Tfa9890_Error_t  error = Error_Ok;
	int spkr_count = 1, nr_bytes, i;
	unsigned char bytes[6] = { 0 };
	int data[2];
	nr_bytes = spkr_count * 3;
#if 1 
	if (tfa_get_bf_tfa98xx(slave_address, TFA2_BF_MTPOTC))
	{
		*pRe25 = (tfa_get_bf_tfa98xx(slave_address, TFA2_MTP_RE25) * 1000) / 1024;
	}
	else
#endif//
	{
		error = tfa98xx_dsp_cmd_id_write_read(slave_address, MODULE_SPEAKERBOOST, SB_PARAM_GET_RE0, nr_bytes, bytes);
		if (error == Error_Ok) {
			tfa98xx_convert_bytes2data(nr_bytes, bytes, data);
			for (i = 0; i < spkr_count; i++) {
				*pRe25 = (data[i] * 1024) / TFA2_FW_ReZ_SCALE;
			}
		}
		else {
			for (i = 0; i < spkr_count; i++)
				*pRe25 = -1;
		}
	}

	return error;
}
Tfa9890_Error_t tfa98xxRunWaitCalibration(uint8_t slave_address, int *calibrateDone)
{
	Tfa9890_Error_t err = Error_Ok;
	int tries = 0, mtp_busy = 1, tries_mtp_busy = 0;

	*calibrateDone = 0;

	/* in case of calibrate once wait for MTPEX */
	if (tfa_get_bf_tfa98xx(slave_address, TFA2_BF_MTPOTC)) {
		// Check if MTP_busy is clear!
		while (tries_mtp_busy < 50)
		{
			mtp_busy = tfa_get_bf_tfa98xx(slave_address, TFA2_BF_MTPB);
			if (mtp_busy == 1)
				TFA98XX_SLEEP(10); /* wait 10ms to avoid busload */
			else
				break;
			tries_mtp_busy++;
		}

		if (tries_mtp_busy < 50) {
			/* Because of the mTFA98XX_PRINT TFA98XX_API_WAITRESULT_NTRIES is way to long!
				* Setting this to 25 will take it atleast 25*50ms = 1.25 sec
				*/
			while ((*calibrateDone == 0) && (tries < 50)) {
				*calibrateDone = tfa_get_bf_tfa98xx(slave_address, TFA2_BF_MTPEX);
				if (*calibrateDone == 1)
					break;
				TFA98XX_SLEEP(50); /* wait 50ms to avoid busload */
				tries++;
			}

			if (tries >= 50) {
				tries = 3000;
			}
		}
#ifdef TFA98XX_PRINT_DEBUG
		else {
			TFA98XX_PRINT("MTP bussy after %d tries\n", 50);
		}
#endif//
	}

	/* poll xmem for calibrate always
		* calibrateDone = 0 means "calibrating",
		* calibrateDone = -1 (or 0xFFFFFF) means "fails"
		* calibrateDone = 1 means calibration done
		*/
	while ((*calibrateDone != 1) && (tries < 3000)) {
		err = tfa98xx_dsp_read_mem_tfa98xx(slave_address, 516, 1, calibrateDone);
		tries++;
	}

	if (*calibrateDone != 1) {
		TFA98XX_PRINT("Calibration failed! \n");
		err = Error_Bad_Parameter;
	}
	else if (tries == 3000) {
		TFA98XX_PRINT("Calibration has timedout! \n");
		err = Error_StateTimedOut;
	}
	else if (tries_mtp_busy == 1000) {
		TFA98XX_PRINT("Calibrate Failed: MTP_busy stays high! \n");
		err = Error_StateTimedOut;
	}


	return err;
}
#ifdef TFA_ENABLE_LIVEDATA_CAPTURE
#define LSMODEL_MAX_WORDS            150
#define LIVEDATA_ITEMS_LENGTH        26
#define FW_PAR_ID_GET_MEMTRACK       0x8B
int GetscalingFactor(char ItemId)
{
	switch (ItemId) {
	case 0:
	case 1:
		return 16384;

	case 2:
	case 9:
	case 10:
	case 11:
	case 15:
	case 22:
		return 1;
	case 16:
	case 17:
		return 2097152;
	case 18:
	case 19:
		return 524288;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 12:
	case 13:
	case 14:
	case 20:
	case 21:
	case 23:
	case 24:
	case 25:
		return 65536;
	default:
		return 1;


	}
}
void tfa98xxRetrieveLiveData(uint8_t slave_address, float *live_data)
{
	Tfa9890_Error_t err = Error_Ok;
	int ItemId = 0;
	unsigned char *bytes = malloc(sizeof(unsigned char) * ((LSMODEL_MAX_WORDS * 3) + 1));
	int *data = malloc(sizeof(int) * 151); /* 150 + 1*/
	err = tfa98xx_dsp_cmd_id_write_read(slave_address, MODULE_FRAMEWORK, FW_PAR_ID_GET_MEMTRACK, (LIVEDATA_ITEMS_LENGTH * 3) + 3, bytes);
	assert(err == Error_Ok);
	tfa98xx_convert_bytes2data(3 * LIVEDATA_ITEMS_LENGTH, bytes + 3, data);
	if (slave_address == LEFT_AMP)
		TFA98XX_PRINT("tChip_P,T_P,activity_P,AGCenvelope_P,preLIMenvelope_P,LIMenvelope_P,AGCgain_P,preLIMgain_P,LIMgain_P,ampClip_P,damageT_P,damageFres_P,sBiasProtSS_P,sBiasClip_P,sBias_P,fRes_P,X_P,Xout_P,sideChainX_P,sideChainS_P,mbdrcAGCgain_P,mbdrcAGCenvelope_P,tClipCnt_P,Re0_P,ReZ_P,ReT_P,\n");
	else if (slave_address == RIGHT_AMP)
		TFA98XX_PRINT("tChip_S,T_S,activity_S,AGCenvelope_S,preLIMenvelope_S,LIMenvelope_S,AGCgain_S,preLIMgain_S,LIMgain_S,ampClip_S,damageT_S,damageFres_S,sBiasProtSS_S,sBiasClip_S,sBias_S,fRes_S,X_S,Xout_S,sideChainX_S,sideChainS_S,mbdrcAGCgain_S,mbdrcAGCenvelope_S,tClipCnt_S,Re0_S,ReZ_S,ReT_S,\n");
	for (ItemId = 0; ItemId < LIVEDATA_ITEMS_LENGTH; ItemId++)
	{
		//TFA98XX_PRINT("(float)data[ItemId]:%f--GetscalingFactor(ItemId):%d\n,",(float)data[ItemId],GetscalingFactor(ItemId));
		live_data[ItemId] = (float)data[ItemId] / GetscalingFactor(ItemId);
		TFA98XX_PRINT("%f,", live_data[ItemId]);

	}
	TFA98XX_PRINT("/n");
	free(bytes);
	free(data);
}
#endif//
