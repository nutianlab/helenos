/*
 * Copyright (c) 2011 Jan Vesely
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @addtogroup drvaudiosb16
 * @{
 */
/** @file
 * @brief SB16 main structure combining all functionality
 */
#ifndef DRV_AUDIO_SB16_REGISTERS_H
#define DRV_AUDIO_SB16_REGISTERS_H

typedef struct sb16_regs {
	ioport8_t fm_address_status;
	ioport8_t fm_data;
	ioport8_t afm_address_status;
	ioport8_t afm_data;
	ioport8_t mixer_address;
	ioport8_t mixer_data;
	ioport16_t dsp_reset;
	ioport8_t fm_address_status2;
	ioport8_t fm_data2;
	const ioport8_t dsp_data_read;
	ioport8_t dsp_write; /* Both command and data, bit 7 is write status */
	const ioport8_t dsp_read_status; /* Bit 7 */
	ioport8_t reserved;
	ioport8_t cd_command_data;
	ioport8_t cd_status;
	ioport8_t cd_reset;
	ioport8_t cd_enable;
} sb16_regs_t;

typedef struct mpu_regs {
	ioport8_t data;
#define MPU_CMD_ACK (0xfe)

	ioport8_t status_command;
#define MPU_STATUS_OUTPUT_BUSY (1 << 6)
#define MPU_STATUS_INPUT_BUSY (1 << 7)

#define MPU_CMD_RESET (0xff)
#define MPU_CMD_ENTER_UART (0x3f)
} mpu_regs_t;

#endif
/**
 * @}
 */

