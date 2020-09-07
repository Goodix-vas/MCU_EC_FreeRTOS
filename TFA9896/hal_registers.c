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

Tfa9890_Error_t classify_i2c_error(TFA_I2C_Error_t i2c_error)
{
	if (i2c_error != TFA_I2C_Ok)
		TFA98XX_PRINT("i2c_error:%d\n", i2c_error);
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
Tfa9890_Error_t read_register(uint8_t slave_address, uint8_t subaddress, unsigned short* value)
{
	TFA_I2C_Error_t i2c_error;
	uint8_t write_data[1]; /* subaddress */
	uint8_t read_buffer[2]; /* 2 data bytes */

	write_data[0] = subaddress;
	//i2c_error = TFA_I2C_WriteRead(slave_address,
	//	sizeof(write_data), write_data, sizeof(read_buffer), read_buffer);

	if (classify_i2c_error(i2c_error) != Error_Ok) {
		return classify_i2c_error(i2c_error);
	}
	else {

		*value = (read_buffer[0] << 8) + read_buffer[1];
		return Error_Ok;
	}
}
int tfa_read_reg(uint8_t slave_address, const uint16_t bf)
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

	//i2c_error = TFA_I2C_WriteRead(slave_address,
	//	sizeof(write_data), write_data, 0, 0);
	return classify_i2c_error(i2c_error);
}
uint16_t tfa_get_bf(uint8_t slave_address, const uint16_t bf)
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
Tfa9890_Error_t tfa_set_bf(uint8_t slave_address, const uint16_t bf, const uint16_t value)
{

	Tfa9890_Error_t err = Error_Ok;
	uint16_t regvalue, msk, oldvalue;
	uint8_t len = (bf & 0x0f) + 1;
	uint8_t pos = (bf >> 4) & 0x0f;
	uint8_t address = (bf >> 8) & 0xff;
	err = read_register(slave_address, address, &regvalue);
	oldvalue = regvalue;
	msk = ((1 << len) - 1) << pos;
	regvalue &= ~msk;
	regvalue |= value << pos;

	if (oldvalue != regvalue)
		err = write_register(slave_address, (bf >> 8) & 0xff, regvalue);

	return err;


}
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
	//i2c_error =
	//	TFA_I2C_WriteRead(slave_address, bytes2write,
	//		write_data, num_bytes, data);
	if (classify_i2c_error(i2c_error) != Error_Ok)
		return classify_i2c_error(i2c_error);
	else
		return Error_Ok;
}
Tfa9890_Error_t Tfa98xx_WriteData(uint8_t slave_address,
	unsigned char subaddress, int num_bytes,
	const unsigned char data[])
{
	TFA_I2C_Error_t i2c_error;
	int bytes2write = num_bytes + 1;        /* slave address, subaddress followed by the data */
	unsigned char write_data[MAX_PARAM_SIZE];
	if (bytes2write > MAX_PARAM_SIZE)
		return Error_Bad_Parameter;
	if (bytes2write > MAX_I2C_LENGTH)
		return   Error_Bad_Parameter;
	write_data[0] = subaddress;
	memcpy(write_data + 1, data, num_bytes);
	//TFA98XX_PRINT("%s\n",__FUNCTION__);
	//i2c_error = TFA_I2C_WriteRead(slave_address,
	//	bytes2write, write_data, 0, 0);
	return classify_i2c_error(i2c_error);
}
/* read the return code for the RPC call */
Tfa9890_Error_t checkRpcStatus(uint8_t slave_address, int* pRpcStatus)
{
	Tfa9890_Error_t error = Error_Ok;
	unsigned short cf_ctrl = 0x0002;        /* the value to sent to the CF_CONTROLS register: cf_req=00000000, cf_int=0, cf_aif=0, cf_dmem=XMEM=01, cf_rst_dsp=0 */
	unsigned short cf_mad = 0x0000; /* memory address to be accessed (0 : Status, 1 : ID, 2 : parameters) */
	unsigned char mem[3];   /* will be filled with the status read from DSP memory */
	if (pRpcStatus == 0)
		return Error_Bad_Parameter;
#ifdef OPTIMIZED_RPC
	{
		/* minimize the number of I2C transactions by making use of the autoincrement in I2C */
		unsigned char buffer[4];
		/* first the data for CF_CONTROLS */
		buffer[0] = (unsigned char)((cf_ctrl >> 8) & 0xFF);
		buffer[1] = (unsigned char)(cf_ctrl & 0xFF);
		/* write the contents of CF_MAD which is the subaddress following CF_CONTROLS */
		buffer[2] = (unsigned char)((cf_mad >> 8) & 0xFF);
		buffer[3] = (unsigned char)(cf_mad & 0xFF);
		error =
			Tfa98xx_WriteData(slave_address, 0x70,
				sizeof(buffer), buffer);
	}
#else                           /* 
								 */
								 /* 1) write DMEM=XMEM to the DSP XMEM */
	if (error == Error_Ok) {
		error =
			write_register(slave_address, 0x70, cf_ctrl);
	}
	if (error == Error_Ok) {
		/* write the address in XMEM where to read */
		write_register(slave_address, 0x71, cf_mad);
	}
#endif                          /* 
								 */
	if (error == Error_Ok) {
		/* read 1 word (24 bit) from XMEM */
		error =
			Tfa98xx_ReadData(slave_address, 0x72,
				3 /*sizeof(mem) */, mem);
	}
	if (error == Error_Ok) {
		*pRpcStatus = (mem[0] << 16) | (mem[1] << 8) | mem[2];
	}
	return error;
}
/* check that num_byte matches the memory type selected */
Tfa9890_Error_t check_size(Tfa98xx_DMEM which_mem, int num_bytes)
{
	Tfa9890_Error_t error = Error_Ok;
	int modulo_size = 1;
	switch (which_mem) {
	case DMEM_PMEM:
		/* 32 bit PMEM */
		modulo_size = 4;
		break;
	case DMEM_XMEM:
	case DMEM_YMEM:
	case DMEM_IOMEM:
		/* 24 bit MEM */
		modulo_size = 3;
		break;
	default:
		error = Error_Bad_Parameter;
	}
	if (error == Error_Ok) {
		if ((num_bytes % modulo_size) != 0) {
			error = Error_Bad_Parameter;
		}
	}
	return error;
}
Tfa9890_Error_t
writeParameter(uint8_t slave_address,
	unsigned char module_id,
	unsigned char param_id,
	int num_bytes, const unsigned char data[])
{
	Tfa9890_Error_t error;
	unsigned short cf_ctrl = 0x0002;        /* the value to be sent to the CF_CONTROLS register: cf_req=00000000, cf_int=0, cf_aif=0, cf_dmem=XMEM=01, cf_rst_dsp=0 */
	unsigned short cf_mad = 0x0001; /* memory address to be accessed (0 : Status, 1 : ID, 2 : parameters) */
	error = check_size(DMEM_XMEM, num_bytes);
	if (error == Error_Ok) {
		if ((num_bytes <= 0) || (num_bytes > MAX_PARAM_SIZE)) {
			error = Error_Bad_Parameter;
		}
	}
#ifdef OPTIMIZED_RPC
	{
		/* minimize the number of I2C transactions by making use of the autoincrement in I2C */
		unsigned char buffer[7];
		/* first the data for CF_CONTROLS */
		buffer[0] = (unsigned char)((cf_ctrl >> 8) & 0xFF);
		buffer[1] = (unsigned char)(cf_ctrl & 0xFF);
		/* write the contents of CF_MAD which is the subaddress following CF_CONTROLS */
		buffer[2] = (unsigned char)((cf_mad >> 8) & 0xFF);
		buffer[3] = (unsigned char)(cf_mad & 0xFF);
		/* write the module and RPC id into CF_MEM, which follows CF_MAD */
		buffer[4] = 0;
		buffer[5] = module_id + 128;
		buffer[6] = param_id;
		//TFA98XX_PRINT("calling Tfa98xx_WriteData\n");
		error =
			Tfa98xx_WriteData(slave_address, 0x70,
				sizeof(buffer), buffer);
	}
#else                           /* 
								 */
	if (error == Error_Ok) {
		error =
			write_register(slave_address, 0x70, cf_ctrl);
	}
	if (error == Error_Ok) {
		write_register(slave_address, 0x71, cf_mad);

	}
	if (error == Error_Ok) {
		unsigned char id[3];
		id[0] = 0;
		id[1] = module_id + 128;
		id[2] = param_id;
		error = Tfa98xx_WriteData(slave_address, 0x72, 3, id);
	}
#endif                          /* OPTIMIZED_RPC */
	if (error == Error_Ok) {
		int offset = 0;
		int chunk_size =
			ROUND_DOWN(TFA_I2C_MAX_SIZE, 3 /* XMEM word size */);
		int remaining_bytes = num_bytes;
		/* due to autoincrement in cf_ctrl, next write will happen at the next address */
		while ((error == Error_Ok) && (remaining_bytes > 0)) {
			if (remaining_bytes < chunk_size) {
				chunk_size = remaining_bytes;
			}
			// else chunk_size remains at initialize value above
			error =
				Tfa98xx_WriteData(slave_address, 0x72,
					chunk_size, data + offset);
			remaining_bytes -= chunk_size;
			offset += chunk_size;
		}
	}
	return error;
}

Tfa9890_Error_t executeParam(uint8_t slave_address)
{
	Tfa9890_Error_t error;
	unsigned short cf_ctrl = 0x0002;        /* the value to be sent to the CF_CONTROLS register: cf_req=00000000, cf_int=0, cf_aif=0, cf_dmem=XMEM=01, cf_rst_dsp=0 */
	cf_ctrl |= (1 << 8) | (1 << 4); /* set the cf_req1 and cf_int bit */
	error = write_register(slave_address, 0x70, (uint16_t)cf_ctrl);
	return error;
}
Tfa9890_Error_t waitResult(uint8_t slave_address)
{
	Tfa9890_Error_t error;
	unsigned short cf_status;       /* the contents of the CF_STATUS register */
	int tries = 0;
	do {
		error = read_register(slave_address, 0x73, &cf_status);
		tries++;
	} while ((error == Error_Ok) && ((cf_status & 0x0100) == 0) && (tries < TFA98XX_API_WAITRESULT_NTRIES));     /* don't wait forever, DSP is pretty quick to respond (< 1ms) */
	if (tries >= TFA98XX_API_WAITRESULT_NTRIES) {
		/* something wrong with communication with DSP */
		error = Error_DSP_not_running;
	}
	return error;
}
Tfa9890_Error_t
Tfa98xx_DspSetParam(uint8_t slave_address,
	unsigned char module_id,
	unsigned char param_id, int num_bytes,
	const unsigned char data[])
{

	Tfa9890_Error_t error;
	int rpcStatus = STATUS_OK;
	/* 1) write the id and data to the DSP XMEM */
	error = writeParameter(slave_address, module_id, param_id, num_bytes, data);
	/* 2) wake up the DSP and let it process the data */
	if (error == Error_Ok) {
		error = executeParam(slave_address);
	}
	/* check the result when addressed an IC uniquely */
	if (slave_address != 0x1C) {
		/* 3) wait for the ack */
		if (error == Error_Ok) {
			error = waitResult(slave_address);
		}
		/* 4) check the RPC return value */
		if (error == Error_Ok) {
			error = checkRpcStatus(slave_address, &rpcStatus);
			TFA98XX_PRINT("rpcStatus:%d\n", rpcStatus);
		}
		if (error == Error_Ok) {
			if (rpcStatus != STATUS_OK) {
				/* DSP RPC call returned an error */
				error =
					(Tfa9890_Error_t)(rpcStatus +
						Error_RpcBase);
			}
		}
	}
	return error;
}

#if 1
/*
 * tfa_ functions copied from tfa_dsp.c
 */

void tfa98xx_convert_data2bytes(int num_data, const int data[],
	unsigned char bytes[])
{
	int i;			/* index for data */
	int k;			/* index for bytes */
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
		bytes[k] = (d >> 16) & 0xFF;	/* MSB */
		bytes[k + 1] = (d >> 8) & 0xFF;
		bytes[k + 2] = (d) & 0xFF;	/* LSB */
	}
}
void tfa98xx_convert_bytes2data(int num_bytes, const unsigned char bytes[],
	int data[])
{
	int i;			/* index for data */
	int k;			/* index for bytes */
	int d;
	int num_data = num_bytes / 3;
	for (i = 0, k = 0; i < num_data; ++i, k += 3) {
		d = (bytes[k] << 16) | (bytes[k + 1] << 8) | (bytes[k + 2]);
		if (bytes[k] & 0x80)	/* sign bit was set */
			d = -((1 << 24) - d);

		data[i] = d;
	}
}

#endif//
Tfa9890_Error_t
Tfa98xx_DspBiquad_Disable(uint8_t slave_address, int biquad_index)
{
	int coeff_buffer[6];
	unsigned char bytes[6 * 3];
	Tfa9890_Error_t error = Error_Ok;
	/* set in correct order and format for the DSP */
	coeff_buffer[0] = (int)-8388608;    /* -1.0f */
	coeff_buffer[1] = 0;
	coeff_buffer[2] = 0;
	coeff_buffer[3] = 0;
	coeff_buffer[4] = 0;
	coeff_buffer[5] = 0;
	/* convert float to fixed point and then bytes suitable for transmaission over I2C */
	tfa98xx_convert_data2bytes(6, coeff_buffer, bytes);
	error = Tfa98xx_DspSetParam(slave_address, 2,
		(unsigned char)biquad_index,
		(unsigned char)6 * 3, bytes);

	return error;

}
#ifdef TFA9891_DEVICE
/* load all the parameters for the DRC settings from a file */
Tfa9890_Error_t Tfa98xx_DspWriteDrc(uint8_t slave_address,
	int length,
	const unsigned char* pDrcBytes)
{
	Tfa9890_Error_t error = Error_Ok;
	if (pDrcBytes != 0) {
		/* by design: keep the data opaque and no interpreting/calculation */
		TFA98XX_PRINT("Bytes to be written_DRC_end:%d\n", length);
		error =
			Tfa98xx_DspSetParam(slave_address, 1,
				0x0F, length, pDrcBytes);
		TFA98XX_PRINT("Bytes written_DRC_end:%d\n", length);
	}
	else
		error = Error_Bad_Parameter;

	return error;
}

#endif//
#ifdef TFA_DEBUG_DATA
void printArray(char* ptr, size_t size)
{
	char* const end = ptr + size;
	while (ptr < end) {
		TFA98XX_PRINT("[%x]-", *ptr++);
	}
}
#endif//
Tfa9890_Error_t Tfa98xx_Write_FilterBank(uint8_t slave_address, TfaFilterM_t* filter)
{
	unsigned char biquad_index;
	Tfa9890_Error_t error = Error_Ok;
	for (biquad_index = 0; biquad_index < 10; biquad_index++) {
		if (filter[biquad_index].enabled) {
			TFA98XX_PRINT("Tfa98xx_Write_FilterBank inside if block\n");
			error = Tfa98xx_DspSetParam(slave_address, 2,
				biquad_index + 1, //start @1
				sizeof(filter[biquad_index].biquad.bytes),
				filter[biquad_index].biquad.bytes);
		}
		else
		{
			TFA98XX_PRINT("Tfa98xx_Write_FilterBank inside else block\n");
			error = Tfa98xx_DspBiquad_Disable(slave_address, biquad_index + 1);
		}

	}
	return error;
}
#if defined (TFA9896_DEVICE)
unsigned char vsfwdelay_table_96[] = {
	0,0,2, /*Index 0 - Current/Volt Fractional Delay for 8KHz  */
	0,0,0, /*Index 1 - Current/Volt Fractional Delay for 11KHz */
	0,0,0, /*Index 2 - Current/Volt Fractional Delay for 12KHz */
	0,0,2, /*Index 3 - Current/Volt Fractional Delay for 16KHz */
	0,0,2, /*Index 4 - Current/Volt Fractional Delay for 22KHz */
	0,0,2, /*Index 5 - Current/Volt Fractional Delay for 24KHz */
	0,0,2, /*Index 6 - Current/Volt Fractional Delay for 32KHz */
	0,0,2, /*Index 7 - Current/Volt Fractional Delay for 44KHz */
	0,0,3 /*Index 8 - Current/Volt Fractional Delay for 48KHz */
};
unsigned char cvfracdelay_table_96[] = {
	0,0,51, /*Index 0 - Current/Volt Fractional Delay for 8KHz  */
	0,0, 0, /*Index 1 - Current/Volt Fractional Delay for 11KHz */
	0,0, 0, /*Index 2 - Current/Volt Fractional Delay for 12KHz */
	0,0,38, /*Index 3 - Current/Volt Fractional Delay for 16KHz */
	0,0,34, /*Index 4 - Current/Volt Fractional Delay for 22KHz */
	0,0,33, /*Index 5 - Current/Volt Fractional Delay for 24KHz */
	0,0,11, /*Index 6 - Current/Volt Fractional Delay for 32KHz */
	0,0,2, /*Index 7 - Current/Volt Fractional Delay for 44KHz */
	0,0,62 /*Index 8 - Current/Volt Fractional Delay for 48KHz */
};
#elif defined (TFA9897_DEVICE)
unsigned char vsfwdelay_table_97[] = {
	0,0,2, /*Index 0 - Current/Volt Fractional Delay for 8KHz  */
	0,0,0, /*Index 1 - Current/Volt Fractional Delay for 11KHz */
	0,0,0, /*Index 2 - Current/Volt Fractional Delay for 12KHz */
	0,0,2, /*Index 3 - Current/Volt Fractional Delay for 16KHz */
	0,0,2, /*Index 4 - Current/Volt Fractional Delay for 22KHz */
	0,0,2, /*Index 5 - Current/Volt Fractional Delay for 24KHz */
	0,0,2, /*Index 6 - Current/Volt Fractional Delay for 32KHz */
	0,0,2, /*Index 7 - Current/Volt Fractional Delay for 44KHz */
	0,0,3 /*Index 8 - Current/Volt Fractional Delay for 48KHz */
};
unsigned char cvfracdelay_table_97[] = {
	0,0,51, /*Index 0 - Current/Volt Fractional Delay for 8KHz  */
	0,0, 0, /*Index 1 - Current/Volt Fractional Delay for 11KHz */
	0,0, 0, /*Index 2 - Current/Volt Fractional Delay for 12KHz */
	0,0,38, /*Index 3 - Current/Volt Fractional Delay for 16KHz */
	0,0,34, /*Index 4 - Current/Volt Fractional Delay for 22KHz */
	0,0,33, /*Index 5 - Current/Volt Fractional Delay for 24KHz */
	0,0,11, /*Index 6 - Current/Volt Fractional Delay for 32KHz */
	0,0,2, /*Index 7 - Current/Volt Fractional Delay for 44KHz */
	0,0,62 /*Index 8 - Current/Volt Fractional Delay for 48KHz */
};
#endif
Tfa9890_Error_t  tfa_dsp_write_cvfracdelay_table96_97(uint8_t slave_address)
{
#if defined (TFA9896_DEVICE)
	TFA98XX_PRINT("Bytes  cvfracdelay_table_96 to be written:%d\n", sizeof(cvfracdelay_table_96));
	return Tfa98xx_DspSetParam(slave_address, MODULE_FRAMEWORK,
		0x06, sizeof(cvfracdelay_table_96), cvfracdelay_table_96);

#elif defined (TFA9897_DEVICE)
	TFA98XX_PRINT("Bytes  cvfracdelay_table_97 to be written:%d\n", sizeof(cvfracdelay_table_97));
	return Tfa98xx_DspSetParam(slave_address, MODULE_FRAMEWORK,
		0x06, sizeof(cvfracdelay_table_97), cvfracdelay_table_97);
#endif//

}
Tfa9890_Error_t tfa_dsp_write_vsfwdelay_table96_97(uint8_t slave_address)
{
#if defined (TFA9896_DEVICE)
	TFA98XX_PRINT("Bytes  vsfwdelay_table_96 to be written:%d\n", sizeof(vsfwdelay_table_96));
	return Tfa98xx_DspSetParam(slave_address, MODULE_FRAMEWORK,
		0x03, sizeof(vsfwdelay_table_96), vsfwdelay_table_96);
#elif defined (TFA9897_DEVICE)
	TFA98XX_PRINT("Bytes  vsfwdelay_table_97 to be written:%d\n", sizeof(vsfwdelay_table_97));
	return Tfa98xx_DspSetParam(slave_address, MODULE_FRAMEWORK,
		0x03, sizeof(vsfwdelay_table_97), vsfwdelay_table_97);
#endif
}
Tfa9890_Error_t tfa_dsp_write_tables96_97(uint8_t slave_address)
{
	Tfa9890_Error_t  error = Error_Ok;

	error = tfa_dsp_write_vsfwdelay_table96_97(slave_address);
	if (error == Error_Ok) {
		error = tfa_dsp_write_cvfracdelay_table96_97(slave_address);
		assert(error == Error_Ok);
	}

	tfa_set_bf(slave_address, TFA1_BF_RST, 1);
	assert(error == Error_Ok);
	tfa_set_bf(slave_address, TFA1_BF_RST, 0);
	assert(error == Error_Ok);
	return error;
}
Tfa9890_Error_t
processPatchFile(uint8_t slave_address, int length,
	const unsigned char* bytes)
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

		TFA98XX_PRINT("Bytes to be written_patch:%d\n", size);
		//i2c_error = TFA_I2C_WriteRead(slave_address, size, bytes + index, 0, 0);

		if (i2c_error != TFA_I2C_Ok) {
			break;
		}
		index += size;
	}
	return classify_i2c_error(i2c_error);
}
Tfa9890_Error_t
Tfa98xx_DspWritePreset(uint8_t slave_address, int length,
	const unsigned char* pPresetBytes)
{
	Tfa9890_Error_t error = Error_Ok;
	if (pPresetBytes != 0) {
		TFA98XX_PRINT("Bytes to be written_preset_end:%d\n", length);
		/* by design: keep the data opaque and no interpreting/calculation */
		error =
			Tfa98xx_DspSetParam(slave_address, MODULE_SPEAKERBOOST,
				SB_PARAM_SET_PRESET, length, pPresetBytes);
		TFA98XX_PRINT("Bytes  written_preset_end:%d\n", length);
	}
	else {
		error = Error_Bad_Parameter;
	}
	return error;
}
Tfa9890_Error_t
Tfa98xx_DspWriteConfig(uint8_t slave_address, int length,
	const unsigned char* pConfigBytes)
{
	Tfa9890_Error_t error = Error_Ok;
	uint8_t bytes[3] = { 0, 0, 0 };
	TFA98XX_PRINT("Bytes to be written_config:%d\n", length);
	error =
		Tfa98xx_DspSetParam(slave_address, MODULE_SPEAKERBOOST, SB_PARAM_SET_CONFIG,
			length, pConfigBytes);
	TFA98XX_PRINT("Bytes to written_config_end:%d\n", length);
#ifdef TFA9891_DEVICE
	error = Tfa98xx_DspSetParam(slave_address, MODULE_SPEAKERBOOST, SB_PARAM_SET_AGCINS,
		sizeof(bytes), bytes);
#endif//				
	return error;
}
#if 1
Tfa9890_Error_t
Tfa98xx_DspWriteSpeakerParameters(uint8_t slave_address,
	int length,
	const unsigned char* pSpeakerBytes)
{
	Tfa9890_Error_t error;
	uint8_t bytes[3] = { 0, 0, 0 };
	if (pSpeakerBytes != 0)
	{
		TFA98XX_PRINT("Bytes to be written_speaker:%d\n", length);
		/* by design: keep the data opaque and no interpreting/calculation */
		error = Tfa98xx_DspSetParam(slave_address, MODULE_SPEAKERBOOST,
			SB_PARAM_SET_LSMODEL, length,
			pSpeakerBytes);

		TFA98XX_PRINT("Bytes to written_speaker_end:%d\n", length);
#ifdef TFA9891_DEVICE
		error = Tfa98xx_DspSetParam(slave_address, MODULE_SPEAKERBOOST, SB_PARAM_SET_AGCINS,
			sizeof(bytes), bytes);
#endif//
	}
	else
	{
		error = Error_Bad_Parameter;
	}

	return error;
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

uint16_t tfa9890_dsp_system_stable(uint8_t slave_address, int* ready)
{
	char error;
	unsigned short status, mtp0;
	int result;
	uint8_t tries;
	/************************/
	result = tfa_read_reg(slave_address, TFA1_BF_AREFS);
	if (result < 0) {
		error = -result;
		goto errorExit;
	}
	status = (unsigned short)result;

	/* if AMPS is set then we were already configured and running
	 *   no need to check further
	 */
	*ready = (tfa_get_bf_value(TFA1_BF_AMPS, status) == 1);
	if (*ready)		/* if  ready go back */
		return 0;	/* will be Tfa98xx_Error_Ok */

	/* check AREFS and CLKS: not ready if either is clear */
	*ready = !((tfa_get_bf_value(TFA1_BF_AREFS, status) == 0)
		|| (tfa_get_bf_value(TFA1_BF_CLKS, status) == 0));
	if (!*ready)		/* if not ready go back */
		return 0;	/* will be Tfa98xx_Error_Ok */

   /* check MTPB
	*   mtpbusy will be active when the subsys copies MTP to I2C
	*   2 times retry avoids catching this short mtpbusy active period
	*/
	for (tries = 2; tries > 0; tries--) {
		result = tfa_get_bf(slave_address, TFA1_BF_MTPB);
		if (result < 0) {
			error = -result;
			goto errorExit;
		}
		status = (unsigned short)result;

		/* check the contents of the STATUS register */
		*ready = (status == 0);
		if (*ready)	/* if ready go on */
			break;

	}
	if (tries == 0)		/* ready will be 0 if retries exausted */
		return 0;
	/* check the contents of  MTP register for non-zero,
	 *  this indicates that the subsys is ready  */

	error = read_register(slave_address, 0x84, &mtp0);
	if (error)
		goto errorExit;

	*ready = (mtp0 != 0);	/* The MTP register written? */

	return error;


errorExit:
	*ready = 0;
	return error;
}
void setOtc(uint8_t slave_address, int otcOn)
{
	unsigned short mtp;
	Tfa9890_Error_t err;
	unsigned short status;
	int mtpChanged = 0;
	TFA98XX_PRINT("Inside setOtc\n");
	err = read_register(slave_address, 0x80, &mtp);
	assert(err == Error_Ok);
	TFA98XX_PRINT("mtp:%d--mtp & 0x01:%d--otcOn:%d\n", mtp, (mtp & 0x01), otcOn);
	/* set reset MTPEX bit if needed */
	if ((mtp & 0x01) != otcOn)
	{
		TFA98XX_PRINT("writing into 0x5A and setting OTC\n");
		/* need to change the OTC bit, set MTPEX=0 in any case */
		err = write_register(slave_address, 0x0B, (uint16_t)0x5A);
		assert(err == Error_Ok);
		err = write_register(slave_address, 0x80, (uint16_t)otcOn);
		assert(err == Error_Ok);
		err = write_register(slave_address, 0x62, 1 << 11);
		assert(err == Error_Ok);
		mtpChanged = 1;

	}
	do
	{
		TFA98XX_SLEEP(10);
		err = read_register(slave_address, 0x00, &status);
		assert(err == Error_Ok);

	} while ((status & 0x100) == 0x100);
	assert((status & 0x100) == 0);
	if (mtpChanged)
	{
		TFA98XX_PRINT("writing 0x01 into 0x70\n");
		err = write_register(slave_address, 0x70, (uint16_t)0x1);
		assert(err == Error_Ok);
	}
}
Tfa9890_Error_t Tfa98xx_SetConfigured(uint8_t slave_address)
{
	Tfa9890_Error_t  error;
	unsigned short value;
	/* read the SystemControl register, modify the bit and write again */
	error = read_register(slave_address, 0x09, &value);
	if (error != Error_Ok) {
		return error;
	}
	value |= 0x20;
	error = write_register(slave_address, 0x09, value);
	return error;
}
Tfa9890_Error_t
Tfa98xx_DspReadMem(uint8_t slave_address,
	unsigned short start_offset, int num_words, int* pValues)
{
	Tfa9890_Error_t error = Error_Ok;
	unsigned short cf_ctrl; /* the value to sent to the CF_CONTROLS register */
	unsigned char bytes[MAX_PARAM_SIZE/*MAX_I2C_LENGTH*/];
	int burst_size;         /* number of words per burst size */
	int bytes_per_word = 3;
	int num_bytes;
	int* p;
	/* first set DMEM and AIF, leaving other bits intact */
	error = read_register(slave_address, 0x70, &cf_ctrl);
	if (error != Error_Ok) {
		return error;
	}
	cf_ctrl &= ~0x000E;     /* clear AIF & DMEM */
	cf_ctrl |= (DMEM_XMEM << 1);    /* set DMEM, leave AIF cleared for autoincrement */
	error = write_register(slave_address, 0x70, cf_ctrl);
	if (error != Error_Ok) {
		return error;
	}
	error = write_register(slave_address, 0x71, start_offset);
	if (error != Error_Ok) {
		return error;
	}
	num_bytes = num_words * bytes_per_word;
	p = pValues;
	for (; num_bytes > 0;) {
		burst_size = ROUND_DOWN(MAX_I2C_LENGTH, bytes_per_word);
		if (num_bytes < burst_size) {
			burst_size = num_bytes;
		}
		assert(burst_size <= sizeof(bytes));
		error =
			Tfa98xx_ReadData(slave_address, 0x72, burst_size, bytes);
		if (error != Error_Ok) {
			return error;
		}
		tfa98xx_convert_bytes2data(burst_size, bytes, p);
		num_bytes -= burst_size;
		p += burst_size / bytes_per_word;
	}
	return Error_Ok;
}
Tfa9890_Error_t
Tfa98xx_DspExecuteRpc(uint8_t slave_address,
	unsigned char module_id,
	unsigned char param_id, int num_inbytes, unsigned char indata[],
	int num_outbytes, unsigned char outdata[])
{
	Tfa9890_Error_t error;
	int rpcStatus = STATUS_OK;
	int i;


	/* 1) write the id and data to the DSP XMEM */
	error = writeParameter(slave_address, module_id, param_id, num_inbytes, indata);

	/* 2) wake up the DSP and let it process the data */
	if (error == Error_Ok) {
		error = executeParam(slave_address);
	}

	/* 3) wait for the ack */
	if (error == Error_Ok) {
		error = waitResult(slave_address);
	}

	/* 4) check the RPC return value */
	if (error == Error_Ok) {
		error = checkRpcStatus(slave_address, &rpcStatus);
	}
	if (error == Error_Ok) {
		if (rpcStatus != STATUS_OK) {
			/* DSP RPC call returned an error */
			error =
				(Tfa9890_Error_t)(rpcStatus +
					Error_RpcBase);
		}
	}

	/* 5) read the resulting data */
	error = write_register(slave_address, 0x71, 2 /*start_offset*/);
	if (error != Error_Ok) {
		return error;
	}
	/* read in chunks, limited by max I2C length */
	for (i = 0; i < num_outbytes; ) {
		int burst_size = ROUND_DOWN(MAX_I2C_LENGTH, 3 /*bytes_per_word*/);
		if ((num_outbytes - i) < burst_size) {
			burst_size = num_outbytes - i;
		}
		error =
			Tfa98xx_ReadData(slave_address, 0x72, burst_size, outdata + i);
		if (error != Error_Ok) {
			return error;
		}
		i += burst_size;
	}

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
#if defined (TFA9896_DEVICE) || defined (TFA9897_DEVICE)
	if ((checkrev != 0xFF) && (checkrev != 0x96) && (checkrev != 0x97))
#elif defined(TFA9890_DEVICE)
	if ((checkrev != 0xFF) && (checkrev != 0x80))
#else
	if ((checkrev != 0xFF) && (checkrev != 0x92)) //default is TFA9891 
#endif
	{
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
				Tfa98xx_DspReadMem(slave_address, checkaddress, 1, &value);
		}
		if (error == Error_Ok) {
			if (value != checkvalue) {
				error = Error_Not_Supported;
			}
		}
	}
	return error;
}
int tfa_write_reg(uint8_t slave_address, const uint16_t bf, const uint16_t reg_value)
{
	Tfa9890_Error_t err;

	/* bitfield enum - 8..15 : address */
	uint8_t address = (bf >> 8) & 0xff;

	err = write_register(slave_address, address, reg_value);
	if (err)
		return -err;

	return 0;
}

Tfa9890_Error_t
tfa98xx_dsp_system_stable(uint8_t slave_address,
	int* ready)
{
#ifndef TFA9890_DEVICE
	Tfa9890_Error_t  error = Error_Ok;
	unsigned short status;
	int value;

	/* check the contents of the STATUS register */
	value = tfa_read_reg(slave_address, TFA1_BF_AREFS);
	if (value < 0) {
		error = -value;
		*ready = 0;
		//_ASSERT(error);		/* an error here can be fatal */
		TFA98XX_PRINT("FATAL ERROR#######\n");
		return error;
	}
	status = (unsigned short)value;

	/* check AREFS and CLKS: not ready if either is clear */
	*ready = !((tfa_get_bf_value(TFA1_BF_AREFS, status) == 0)
		|| (tfa_get_bf_value(TFA1_BF_CLKS, status) == 0)
		|| (tfa_get_bf_value(TFA1_BF_MTPB, status) == 1));

	return error;
#else
	return tfa9890_dsp_system_stable(slave_address, ready);
#endif//
}

static char* stateFlagsStr(int stateFlags)
{
	static char flags[10];

	flags[0] = (stateFlags & (0x1 << Tfa98xx_SpeakerBoost_ActivityLive)) ? 'A' : 'a';
	flags[1] = (stateFlags & (0x1 << Tfa98xx_SpeakerBoost_S_CtrlLive)) ? 'S' : 's';
	flags[2] = (stateFlags & (0x1 << Tfa98xx_SpeakerBoost_MutedLive)) ? 'M' : 'm';
	flags[3] = (stateFlags & (0x1 << Tfa98xx_SpeakerBoost_X_CtrlLive)) ? 'X' : 'x';
	flags[4] = (stateFlags & (0x1 << Tfa98xx_SpeakerBoost_T_CtrlLive)) ? 'T' : 't';
	flags[5] = (stateFlags & (0x1 << Tfa98xx_SpeakerBoost_NewModelLive)) ? 'L' : 'l';
	flags[6] = (stateFlags & (0x1 << Tfa98xx_SpeakerBoost_VolumeRdyLive)) ? 'V' : 'v';
	flags[7] = (stateFlags & (0x1 << Tfa98xx_SpeakerBoost_DamagedLive)) ? 'D' : 'd';
	flags[8] = (stateFlags & (0x1 << Tfa98xx_SpeakerBoost_SignalClippingLive)) ? 'C' : 'c';

	flags[9] = 0;
	return flags;
}
/*******************/
/* Execute RPC protocol to read something from the DSP */
Tfa9890_Error_t
Tfa98xx_DspGetParam(uint8_t slave_address,
	unsigned char module_id,
	unsigned char param_id, int num_bytes, unsigned char data[])
{
	Tfa9890_Error_t error;
	unsigned short cf_ctrl = 0x0002;        /* the value to be sent to the CF_CONTROLS register: cf_req=00000000, cf_int=0, cf_aif=0, cf_dmem=XMEM=01, cf_rst_dsp=0 */
	unsigned short cf_mad = 0x0001; /* memory address to be accessed (0 : Status, 1 : ID, 2 : parameters) */
	unsigned short cf_status;       /* the contents of the CF_STATUS register */
	int rpcStatus = STATUS_OK;

	if (slave_address == 0x1C) {
		/* cannot read */
		return Error_Bad_Parameter;
	}
	error = check_size(DMEM_XMEM, num_bytes);

	if (error == Error_Ok) {
		if ((num_bytes <= 0) || (num_bytes > MAX_PARAM_SIZE)) {
			error = Error_Bad_Parameter;
		}
	}
#ifdef OPTIMIZED_RPC
	{
		/* minimize the number of I2C transactions by making use of the autoincrement in I2C */
		unsigned char buffer[7];
		/* first the data for CF_CONTROLS */
		buffer[0] = (unsigned char)((cf_ctrl >> 8) & 0xFF);
		buffer[1] = (unsigned char)(cf_ctrl & 0xFF);
		/* write the contents of CF_MAD which is the subaddress following CF_CONTROLS */
		buffer[2] = (unsigned char)((cf_mad >> 8) & 0xFF);
		buffer[3] = (unsigned char)(cf_mad & 0xFF);
		/* write the module and RPC id into CF_MEM, which follows CF_MAD */
		buffer[4] = 0;
		buffer[5] = module_id + 128;
		buffer[6] = param_id;
		error =
			Tfa98xx_WriteData(slave_address, 0x70,
				sizeof(buffer), buffer);
	}
#else                           /* 
								 */
								 /* 1) write the id and data to the DSP XMEM */
	if (error == Error_Ok) {
		error = write_register(slave_address, 0x70, cf_ctrl);
	}
	if (error == Error_Ok) {
		error = write_register(slave_address, 0x71, cf_mad);
	}
	if (error == Error_Ok) {
		unsigned char id[3];
		id[0] = 0;
		id[1] = module_id + 128;
		id[2] = param_id;
		/* only try MEM once, if error, need to resend mad as well */
		error = Tfa98xx_WriteData(slave_address, 0x72, 3, id);
	}
#endif                          /* 
								 */
								 /* 2) wake up the DSP and let it process the data */
	if (error == Error_Ok) {
		cf_ctrl |= (1 << 8) | (1 << 4); /* set the cf_req1 and cf_int bit */

		error = write_register(slave_address, 0x70, (uint16_t)cf_ctrl);
	}
	/* 3) wait for the ack */
	if (error == Error_Ok) {
		int tries = 0;
		do {
			error = read_register(slave_address, 0x73, &cf_status);
			tries++;
		} while ((error == Error_Ok) && ((cf_status & 0x0100) == 0) && (tries < TFA98XX_API_WAITRESULT_NTRIES));     /* don't wait forever, DSP is pretty quick to respond (< 1ms) */
		if (tries >= TFA98XX_API_WAITRESULT_NTRIES) {
			/* something wrong with communication with DSP */
			return Error_DSP_not_running;
		}
	}
	/* 4) check the RPC return value */
	if (error == Error_Ok) {
		error = checkRpcStatus(slave_address, &rpcStatus);
	}
	if (error == Error_Ok) {
		if (rpcStatus != STATUS_OK) {
			/* DSP RPC call returned an error */
			error =
				(Tfa9890_Error_t)(rpcStatus +
					Error_RpcBase);
		}
	}
	/* 5) read the resulting data */
	if (error == Error_Ok) {
		cf_mad = 0x0002;        /* memory address to be accessed (0 : Status, 1 : ID, 2 : parameters) */
		error = write_register(slave_address, 0x71, (uint16_t)cf_mad);
	}
	if (error == Error_Ok) {
		int offset = 0;
		int chunk_size =
			ROUND_DOWN(TFA_I2C_MAX_SIZE, 3 /* XMEM word size */);
		int remaining_bytes = num_bytes;
		/* due to autoincrement in cf_ctrl, next write will happen at the next address */
		while ((error == Error_Ok) && (remaining_bytes > 0)) {
			if (remaining_bytes < 252) {
				chunk_size = remaining_bytes;
			}
			// else chunk_size remains at initialize value above
			error = Tfa98xx_ReadData(slave_address, 0x72, chunk_size, data + offset);
			remaining_bytes -= chunk_size;
			offset += chunk_size;
		}
	}
	return error;
}

/************************/
#ifdef ENABLE_LIVEDATA_CAPTURE
Tfa9890_Error_t
Tfa98xx_DspGetStateInfo(uint8_t slave_address,
	Tfa98xx_StateInfoLive_t* pInfo) {
	Tfa9890_Error_t error = Error_Ok;
	int24           data[FW_STATE_MAX_SIZE];     /* allocate worst case */
	unsigned char   bytes[FW_STATE_MAX_SIZE * 3];
	int             stateSize = FW_STATE_SIZE; /* contains the actual amount of parameters transferred, depends on IC and ROM code version */
	int             i;


	assert(pInfo != 0);

	/* init to default value to have sane values even when some features aren't supported */
	for (i = 0; i < FW_STATE_MAX_SIZE; i++)
	{
		data[i] = 0;
	}

	error =
		Tfa98xx_DspGetParam(slave_address, MODULE_FRAMEWORK, FW_PARAM_GET_STATE,
			3 * stateSize, bytes);
	if (error != Error_Ok)
	{
		if (error == Error_RpcParamId)
		{
			/* old ROM code, ask SpeakerBoost and only do first portion */
			stateSize = 8;
			error =
				Tfa98xx_DspGetParam(slave_address, MODULE_SPEAKERBOOST, SB_PARAM_GET_STATE,
					3 * stateSize, bytes);
		}
		else
		{
			return error;
		}
	}
	tfa98xx_convert_bytes2data(3 * stateSize, bytes, data);
	pInfo->agcGain = (float)data[0] / (1 << (23 - SPKRBST_AGCGAIN_EXP));     /* /2^23*2^(SPKRBST_AGCGAIN_EXP) */
	pInfo->limGain = (float)data[1] / (1 << (23 - SPKRBST_LIMGAIN_EXP));     /* /2^23*2^(SPKRBST_LIMGAIN_EXP) */
	pInfo->sMax = (float)data[2] / (1 << (23 - SPKRBST_HEADROOM));        /* /2^23*2^(SPKRBST_HEADROOM)    */
	pInfo->T = data[3] / (1 << (23 - SPKRBST_TEMPERATURE_EXP)); /* /2^23*2^(def.SPKRBST_TEMPERATURE_EXP) */
	pInfo->statusFlag = data[4];
	pInfo->X1 = (float)data[5] / (1 << (23 - SPKRBST_HEADROOM));        /* /2^23*2^(SPKRBST_HEADROOM)        */
#if !(defined(TFA9896_DEVICE) || defined(TFA9897_DEVICE))
	pInfo->X2 = (float)data[6] / (1 << (23 - SPKRBST_HEADROOM));        /* /2^23*2^(SPKRBST_HEADROOM)        */
	pInfo->Re = (float)data[7] / (1 << (23 - SPKRBST_TEMPERATURE_EXP)); /* /2^23*2^(SPKRBST_TEMPERATURE_EXP) */
	pInfo->shortOnMips = data[8];
#else
	pInfo->X2 = 0;      /* /2^23*2^(SPKRBST_HEADROOM)        */
	pInfo->Re = (float)data[6] / (1 << (23 - SPKRBST_TEMPERATURE_EXP)); /* /2^23*2^(SPKRBST_TEMPERATURE_EXP) */
	pInfo->shortOnMips = data[7];
#endif

	return error;
}

void dump_state_infoLive(Tfa98xx_StateInfoLive_t* pState)
{
	TFA98XX_PRINT("state: flags %s, agcGain %2.1f\tlimGain %2.1f\tsMax %2.1f\tT %d\tX1 %2.1f\tX2 %2.1f\tRe %2.2f\tshortOnMips %d\n",
		stateFlagsStr(pState->statusFlag),
		pState->agcGain,
		pState->limGain,
		pState->sMax,
		pState->T,
		pState->X1,
		pState->X2,
		pState->Re,
		pState->shortOnMips);
}
#endif//ENABLE_LIVEDATA_CAPTURE
Tfa9890_Error_t wait4Calibration(uint8_t slave_address, int* calibrateDone)
{
	Tfa9890_Error_t err;
	int tries = 0;
	unsigned short mtp;
#define WAIT_TRIES 3000

	err = read_register(slave_address, 0x80, &mtp);

	/* in case of calibrate once wait for MTPEX */
	if (mtp & 0x1) {

		while ((*calibrateDone == 0) && (tries < TFA98XX_API_WAITRESULT_NTRIES))
		{   // TODO optimise with wait estimation
			err = read_register(slave_address, 0x80, &mtp);
			*calibrateDone = (mtp & 0x2);    /* check MTP bit1 (MTPEX) */
			tries++;
		}
	}
	else /* poll xmem for calibrate always */
	{
		while ((*calibrateDone == 0) && (tries < WAIT_TRIES))
		{   // TODO optimise with wait estimation
			err = Tfa98xx_DspReadMem(slave_address, 231, 1, calibrateDone);
			tries++;
		}
		if (tries == WAIT_TRIES)
			TFA98XX_PRINT("calibrateDone 231 timedout\n");
	}

}
Tfa9890_Error_t
Tfa98xx_DspGetCalibrationImpedance(uint8_t slave_address, float* pRe25)
{
	Tfa9890_Error_t error = Error_Ok;
	unsigned char bytes[3];
	int24 data[1];
	int calibrateDone;

	assert(pRe25 != 0);
	*pRe25 = 0.0f; /* default 0.0 */
	{
		error = Tfa98xx_DspReadMem(slave_address, 231, 1, &calibrateDone);
		if (error == Error_Ok)
		{
			if (!calibrateDone)
			{
				/* return the default */
				return error;
			}
			error = Tfa98xx_DspGetParam(slave_address, MODULE_SPEAKERBOOST,
				0x85, 3, bytes);
		}
		if (error == Error_Ok)
		{
			tfa98xx_convert_bytes2data(3, bytes, data);
			*pRe25 = ((float)data[0] / (1 << (23 - SPKRBST_TEMPERATURE_EXP)) * 1024) / 1000;        /* /2^23*2^(def.SPKRBST_TEMPERATURE_EXP) */
		}
	}
	return error;
}
void tfa98xx_powerdown(uint8_t slave_address, int powerdown)
{
	tfa_set_bf(slave_address, TFA1_BF_PWDN, (uint16_t)powerdown);
}
Tfa9890_Error_t  Tfa98xx_SetMute(uint8_t slave_address, Tfa9891_Mute_t mute)
{
	Tfa9890_Error_t error;
	unsigned short audioctrl_value;
	unsigned short sysctrl_value;
	error =
		read_register(slave_address, TFA98XX_AUDIO_CTR, &audioctrl_value);
	if (error != Error_Ok)
		return error;
	error =
		read_register(slave_address, TFA98XX_SYS_CTRL, &sysctrl_value);

	if (error != Error_Ok)
		return error;
	switch (mute) {
	case Mute_Off:
		/* previous state can be digital or amplifier mute,
		 * clear the cf_mute and set the enbl_amplifier bits
		 *
		 * To reduce PLOP at power on it is needed to switch the
		 * amplifier on with the DCDC in follower mode (enbl_boost = 0 ?).
		 * This workaround is also needed when toggling the powerdown bit!
		 */
		audioctrl_value &= ~(TFA98XX_AUDIO_CTR_CFSM_MSK);
		sysctrl_value |= TFA98XX_SYS_CTRL_AMPE_MSK;
		sysctrl_value |= TFA98XX_SYS_CTRL_DCA_MSK;
		break;
	case Mute_Digital:
		/* expect the amplifier to run */
		/* set the cf_mute bit */
		audioctrl_value |= TFA98XX_AUDIO_CTR_CFSM_MSK;
		/* set the enbl_amplifier bit */
		sysctrl_value |= (TFA98XX_SYS_CTRL_AMPE_MSK);
		/* clear active mode */
		sysctrl_value &= ~(TFA98XX_SYS_CTRL_DCA_MSK);
		break;
	case Mute_Amplifier:
		/* clear the cf_mute bit */
		audioctrl_value &= ~TFA98XX_AUDIO_CTR_CFSM_MSK;
		/* clear the enbl_amplifier bit and active mode */
		sysctrl_value &= ~(TFA98XX_SYS_CTRL_AMPE_MSK | TFA98XX_SYS_CTRL_DCA_MSK);
		break;
	default:
		error = Error_Bad_Parameter;
	}
	if (error != Error_Ok)
		return error;
	error =
		write_register(slave_address, TFA98XX_AUDIO_CTR, audioctrl_value);
	if (error != Error_Ok)
		return error;
	error =
		write_register(slave_address, TFA98XX_SYS_CTRL, sysctrl_value);

	return error;
}
Tfa9890_Error_t
tfa98xx_dsp_write_mem_word(uint8_t slave_address, unsigned short address, int value, int memtype)
{
	Tfa9890_Error_t error = Error_Ok;
	unsigned char bytes[3];

	tfa_set_bf(slave_address, TFA1_BF_DMEM, (uint16_t)memtype);
	error = -tfa_write_reg(slave_address, TFA1_BF_MADD, address);
	if (error != Error_Ok)
		return error;
	tfa98xx_convert_data2bytes(1, &value, bytes);
	error = Tfa98xx_WriteData(slave_address, 0x720f >> 8, 3, bytes);

	return error;
}

/*************************/
#ifdef ENABLE_LIVEDATA_CAPTURE
Tfa9890_Error_t Tfa98xxGetSpeakerModel(uint8_t slave_address,
	int xmodel,
	TFA9891SPKRBST_SpkrModel_t* record)
{
	Tfa9890_Error_t error = Error_Ok;
	unsigned char bytes[3 * 141];
	int24 data[141];
	int i = 0;
	memset(bytes, 0, 423);
	error = Tfa98xx_DspGetParam(slave_address, 1,
		PARAM_GET_LSMODEL /*PARAM_GET_LSMODELW*/, 423, bytes);
	assert(error == Error_Ok);

	tfa98xx_convert_bytes2data(sizeof(bytes), bytes, data);

	for (i = 0; i < 128; i++)
	{
		/*record->pFIR[i] = (double)data[i] / ((1 << 23) * 2);*/
		record->pFIR[i] = (double)data[i] / (1 << 22);
	}

	record->Shift_FIR = data[i++];   ///< Exponent of HX data
	record->leakageFactor = (float)data[i++] / (1 << (23));  ///< Excursion model integration leakage
	record->ReCorrection = (float)data[i++] / (1 << (23));   ///< Correction factor for Re
	record->xInitMargin = (float)data[i++] / (1 << (23 - 2));        ///< (can change) Margin on excursion model during startup
	record->xDamageMargin = (float)data[i++] / (1 << (23 - 2));      ///< Margin on excursion modelwhen damage has been detected
	record->xMargin = (float)data[i++] / (1 << (23 - 2));    ///< Margin on excursion model activated when LookaHead is 0
	record->Bl = (float)data[i++] / (1 << (23 - 2)); ///< Loudspeaker force factor
	record->fRes = data[i++];        ///< (can change) Estimated Speaker Resonance Compensation Filter cutoff frequency
	record->fResInit = data[i++];    ///< Initial Speaker Resonance Compensation Filter cutoff frequency
	record->Qt = (float)data[i++] / (1 << (23 - 6)); ///< Speaker Resonance Compensation Filter Q-factor
	record->xMax = (float)data[i++] / (1 << (23 - 7));       ///< Maximum excursion of the speaker membrane
	record->tMax = (float)data[i++] / (1 << (23 - 9));       ///< Maximum Temperature of the speaker coil
	record->tCoefA = (float)data[i++] / (1 << 23);   ///< (can change) Temperature coefficient
	return error;
}

/********************************/
void dump_speaker_model(TFA9891SPKRBST_SpkrModel_t* pState, uint8_t slave_address)
{
	TFA98XX_PRINT("FRes:%d--SPKS:%d\n", pState->fRes, tfa_get_bf(slave_address, TFA1_BF_SPKS));
}
#endif //ENABLE_LIVEDATA_CAPTURE
Tfa9890_Error_t Tfa9890_printCalibration(uint8_t slave_address)
{
	Tfa9890_Error_t error = Error_Ok;
	unsigned char bytes[6] = { 0 };
	int24 data[2];
	float mohm;
	error = Tfa98xx_DspGetParam(slave_address, MODULE_SPEAKERBOOST, SB_PARAM_GET_RE0,
		3, bytes);
	assert(error == Error_Ok);


	if (error == Error_Ok) {
		tfa98xx_convert_bytes2data(3, bytes, data);
		mohm = ((float)data[0] * 1024) / TFA1_FW_ReZ_SCALE;
	}
	TFA98XX_PRINT(" Calibrated value :%1.2f mOhms :\n", (mohm * 1024) / 1000);
	return error;
}
#ifdef SUPPORT_EXCURSION_FILTER
Tfa9890_Error_t tfaRunWriteFilter(uint8_t slave_address, TfaContBiquadM_t* bq) {
	Tfa9890_Error_t error = Error_Ok;
	Tfa98xx_DMEM dmem;
	uint16_t address;
	uint8_t data[3 * 3 + sizeof(bq->aa.bytes)];
	int i, channel = 0, runs = 1, cnt = 0;
	int8_t saved_index = bq->aa.index; /* This is used to set back the index */
	unsigned char* ptr = (unsigned char*)&(bq->aa.cutOffFreq);
	for (i = 0; i < sizeof(float); i++)
		TFA98XX_PRINT("%2.2x", ptr[i]);
	TFA98XX_PRINT("\n");

	/*****************/
#if 0 
	char myString[4];
	uint32_t num;
	float f;
	sTFA98XX_PRINT(myString, "%x", (float)(bq->aa.cutOffFreq));
	sscanf(myString, "%x", &num);  // assuming you checked input
	f = *((float*)&num);
	TFA98XX_PRINT("the hexadecimal 0x%08x becomes %.3f as a float\n", num, f);
#endif//
	TFA9XX_PRINT("sizeof(bq):%d\n", sizeof(*bq));
	/*********************/
		//	TFA98XX_PRINT("sizeof(bqOrig):%d\n",sizeof(TfaContBiquadM2_t));
		//TFA98XX_PRINT("bq->aa.index:%x\n",bq->aa.index);
		/* Channel=1 is primary, Channel=2 is secondary*/
	if (bq->aa.index > 100) {
		bq->aa.index -= 100;
		channel = 2;
	}
	else if (bq->aa.index > 50) {
		bq->aa.index -= 50;
		channel = 1;
	}
	else
		TFA98XX_PRINT("bq->aa.index:%x--bq->aa.type:%d--bq->aa.cutOffFreq:%.0f--bq->aa.rippleDb:%.1f--bq->aa.rolloff:%.1f \n", bq->aa.index, bq->aa.type, bq->aa.cutOffFreq, bq->aa.rippleDb, bq->aa.rolloff);

	if (channel == 2)
	{
		TFA98XX_PRINT("channel0:%d\n", channel);
		TFA98XX_PRINT("filter[%d,S]", bq->aa.index);
	}
	else if (channel == 1)
	{
		TFA98XX_PRINT("channel1:%d\n", channel);
		TFA98XX_PRINT("filter[%d,P]", bq->aa.index);
	}
	else
	{
		TFA98XX_PRINT("channel2:%d\n", channel);
		TFA98XX_PRINT("filter[%d]", bq->aa.index);
	}

	for (i = 0; i < runs; i++) {
		if (runs == 2)
			channel++;

		/* get the target address for the filter on this device */
		TFA98XX_PRINT("calling tfa98xx_filter_mem\n");
		dmem = tfa98xx_filter_mem(slave_address, bq->aa.index, &address, channel);
		if (dmem < 0)
			return Error_Bad_Parameter;

		/* send a DSP memory message that targets the devices specific memory for the filter
		 * msg params: which_mem, start_offset, num_words
		 */

		memset(data, 0, 3 * 3);
		data[2] = dmem; /* output[0] = which_mem */
		data[4] = address >> 8; /* output[1] = start_offset */
		data[5] = address & 0xff;
		data[8] = sizeof(bq->aa.bytes) / 3; /*output[2] = num_words */
		memcpy(&data[9], bq->aa.bytes, sizeof(bq->aa.bytes)); /* payload */
		TFA98XX_PRINT("data[2]:%d--data[4]:%d--data[5]:%d--data[8]:%d--\n", data[2], data[4], data[5], data[8]);
		for (cnt = 0; cnt < sizeof(bq->aa.bytes); cnt++)
			TFA98XX_PRINT("bq->aa.bytes[%d]:%d\n", cnt, bq->aa.bytes[cnt]);
		TFA98XX_PRINT("calling tfa_dsp_cmd_id_write filter\n");
		error = Tfa98xx_DspSetParam(slave_address, MODULE_FRAMEWORK, 4,
			sizeof(data), data);


	}

#if 0 //def TFA_DEBUG			
	if (bq->aa.index == 13) {
		TFA98XX_PRINT("=%d,%.0f,%.2f \n",
			bq->in.type, bq->in.cutOffFreq, bq->in.leakage);
	}
	else
#endif//			
		if (bq->aa.index >= 10 && bq->aa.index <= 12) {
			TFA98XX_PRINT("=%d,%.0f,%.1f,%.1f \n", bq->aa.type,
				bq->aa.cutOffFreq, bq->aa.rippleDb, bq->aa.rolloff);
		}
		else {
			TFA98XX_PRINT("= unsupported filter index \n");
		}


	/* Because we can load the same filters multiple times
	 * For example: When we switch profile we re-write in operating mode.
	 * We then need to remember the index (primary, secondary or both)
	 */
	bq->aa.index = saved_index;

	return error;
}
Tfa98xx_DMEM tfa98xx_filter_mem(uint8_t slave_address, int filter_index, unsigned short* address, int channel)
{
	Tfa98xx_DMEM  dmem = -1;
	int idx;
	unsigned short bq_table[7][4] = {
		/* index: 10, 11, 12, 13 */
				{346,351,356,288}, //87 BRA_MAX_MRA4-2_7.00
				{346,351,356,288}, //90 BRA_MAX_MRA6_9.02
				{467,472,477,409}, //95 BRA_MAX_MRA7_10.02
				{406,411,416,348}, //97 BRA_MAX_MRA9_12.01
				{467,472,477,409}, //91 BRA_MAX_MRAA_13.02
				{8832, 8837, 8842, 8847}, //88 part1
				{8853, 8858, 8863, 8868}  //88 part2
				/* Since the 88 is stereo we have 2 parts.
				 * Every index has 5 values except index 13 this one has 6 values
				 */
	};

	if ((10 <= filter_index) && (filter_index <= 13)) {
		dmem = DMEM_YMEM; /* for all devices */
		idx = filter_index - 10;
		*address = bq_table[4][idx];
	}
	return dmem;
}
#endif//SUPPORT_EXCURSION_FILTER
Tfa9890_Error_t
Tfa9890_SetOVPBypass(uint8_t slave_address)
{
	Tfa9890_Error_t error;
	unsigned short value, xor;
	/* read the SystemControl register, modify the bit and write again */
	error = write_register(slave_address, 0x40, 0x5A6B);
	error = read_register(slave_address, 0x8B, &value);
	xor = value ^ 0x005A;
	error = write_register(slave_address, 0x60, xor);
	error = read_register(slave_address, 0x45, &value);
	TFA98XX_PRINT("before 0x45 regsiter is:%d\n", value);
	value |= (0x01 << 9);
	error = write_register(slave_address, 0x45, value);
	error = read_register(slave_address, 0x45, &value);
	TFA98XX_PRINT("after 0x45 regsiter is:%d\n", value);
	return error;
}
Tfa9890_Error_t SetOneTimeCalibration(uint8_t slave_address)
{
	unsigned short mtp;
	unsigned short value, xor;
	Tfa9890_Error_t error = Error_Ok;
	int tries = 0, mtpBusy = 0;
	//Read MTP register
	TFA98XX_PRINT("totest\n");
	//Set MTPOTC=1
	mtp |= 0x01;
	write_register(slave_address, 0x40, (uint16_t)0x5A6B);
	write_register(slave_address, 0x0B, (uint16_t)0x005A);
	write_register(slave_address, 0x40, (uint16_t)0);
	//Update contents of MTP register
	write_register(slave_address, 0x80, mtp);
	tfa_set_bf(slave_address, TFA1_BF_CIMTP, 0x01);
	write_register(slave_address, 0x40, (uint16_t)0x5A6B);
	write_register(slave_address, 0x0B, (uint16_t)0);
	write_register(slave_address, 0x40, (uint16_t)0);
	tries = 0;
	do
	{
		tries++;
		TFA98XX_SLEEP(10);
		mtpBusy = tfa_get_bf(slave_address, TFA1_BF_MTPB);
	} while ((mtpBusy == 0x01) && (tries < TFA98XX_API_WAITRESULT_NTRIES));

	if (tries == TFA98XX_API_WAITRESULT_NTRIES)
	{
		TFA98XX_PRINT("MTP Busy timedout\n");
		error = Error_StateTimedOut;
	}

	return error;

}
Tfa9890_Error_t ClearOneTimeCalibration(uint8_t slave_address)
{
	Tfa9890_Error_t err = Error_Ok;
	unsigned short mtp;
	int tries = 0, mtpBusy = 0;

	//Read MTP register
	read_register(slave_address, 0x80, &mtp);

	//Set MTPEX=0 and MTPOTC=0 to clear current calibration status
	mtp &= ~(0x03);
	write_register(slave_address, 0x40, (uint16_t)0x5A6B);
	write_register(slave_address, 0x0B, (uint16_t)0x005A);
	write_register(slave_address, 0x40, (uint16_t)0);
	//Update contents of MTP register
	write_register(slave_address, 0x80, mtp);
	tfa_set_bf(slave_address, TFA1_BF_CIMTP, 0x01);
	write_register(slave_address, 0x40, (uint16_t)0x5A6B);
	write_register(slave_address, 0x0B, (uint16_t)0);
	write_register(slave_address, 0x40, (uint16_t)0);
	tries = 0;
	do
	{
		tries++;
		TFA98XX_SLEEP(10);
		mtpBusy = tfa_get_bf(slave_address, TFA1_BF_MTPB);
	} while ((mtpBusy == 0x01) && (tries < TFA98XX_API_WAITRESULT_NTRIES));

	if (tries == TFA98XX_API_WAITRESULT_NTRIES)
	{
		TFA98XX_PRINT("MTP Busy timedout\n");
		err = Error_StateTimedOut;
	}

	return err;
}
Tfa9890_Error_t SetFactoryValues(uint8_t slave_address)
{
	unsigned short mtp = 0, ron, regRead, xor;
	Tfa9890_Error_t err = Error_Ok;
	int tries = 0, mtpBusy = 0;
	int i;
	/*********Hardcoding MTPEX and MTPOTC***************************/
	mtp |= 0x03;
	/*********Hardcoding Ron  in MTP to 8 ohm***************************/
	ron = 0x1F40;
	read_register(slave_address, 0x8B, &regRead);
	xor = regRead ^ 0x005A;
	write_register(slave_address, 0x40, (uint16_t)0x5A6B);
	write_register(slave_address, 0x0B, (uint16_t)0x005A);
	write_register(slave_address, 0x60, (uint16_t)xor);
	write_register(slave_address, 0x40, (uint16_t)0);
	write_register(slave_address, 0x32, &regRead);
	/*Update contents of MTP register*/
	write_register(slave_address, 0x83, ron);
	write_register(slave_address, 0x80, mtp);
	write_register(slave_address, TFA1_BF_CIMTP, 0x01);

	write_register(slave_address, 0x40, (uint16_t)0x5A6B);
	write_register(slave_address, 0x0B, (uint16_t)0);
	write_register(slave_address, 0x40, (uint16_t)0);

	tries = 0;
	do
	{
		tries++;
		for (i = 0; i < 0xfff; i++)
			;
		mtpBusy = tfa_get_bf(slave_address, TFA1_BF_MTPB);
	} while ((mtpBusy == 0x01) && (tries < TFA98XX_API_WAITRESULT_NTRIES));

	if (tries == TFA98XX_API_WAITRESULT_NTRIES)
	{
		TFA98XX_PRINT("MTP Busy timedout\n");
		err = Error_StateTimedOut;
	}

	return err;
}
