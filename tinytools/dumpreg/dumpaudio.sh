#!/system/bin/sh
RESULT=

AUDIO_IN_DUMP_LIST="0x00 0x01 0x02 0x03 0x44"
AUDIO_OUT_DUMP_LIST="0x00 0x01 0x02 0x03 0x44"

_dump_audioin_register () {
	RESULT=`dumpamlaudioreg --type in --index ${1}`
	echo "${RESULT} || \"${2}\""
}

dump_audio_in_list () {
	echo -e "\n===== AUDIO IN REG LIST  =====\n"
	for offset in ${AUDIO_IN_DUMP_LIST}
	do
		_dump_audioin_register ${offset}
	done
}

_dump_audioout_register () {
	RESULT=`dumpamlaudioreg --type out --index ${1}`
	echo "${RESULT} || \"${2}\""
}

dump_audio_out_list () {
	echo -e "\n===== AUDIO OUT REG LIST  =====\n"
	for offset in ${AUDIO_OUT_DUMP_LIST}
	do
		_dump_audioout_register ${offset} ${offset}
	done
}

list_dump () {
	dump_audio_in_list
	dump_audio_out_list
}

echo -e "\n===== AUDIO IN =====\n"

_dump_audioin_register 0x00 "AUDIN_SPDIF_MODE"
_dump_audioin_register 0x01 "AUDIN_SPDIF_FS_CLK_RLTN"
_dump_audioin_register 0x04 "AUDIN_SPDIF_MISC"
_dump_audioin_register 0x05 "AUDIN_SPDIF_NPCM_PCPD - pc[31:16] pd[15:00]"
_dump_audioin_register 0x10 "AUDIN_I2SIN_CTRL"
_dump_audioin_register 0x11 "AUDIN_SOURCE_SEL"
_dump_audioin_register 0x12 "AUDIN_DECODE_FORMAT"
_dump_audioin_register 0x13 "AUDIN_DECODE_CONTROL_STATUS"
_dump_audioin_register 0x14 "AUDIN_DECODE_CHANNEL_STATUS_A_0"
_dump_audioin_register 0x20 "AUDIN_FIFO0_START"
_dump_audioin_register 0x21 "AUDIN_FIFO0_END"
_dump_audioin_register 0x22 "AUDIN_FIFO0_PTR"
_dump_audioin_register 0x23 "AUDIN_FIFO0_INTR"
_dump_audioin_register 0x24 "AUDIN_FIFO0_RDPTR"
_dump_audioin_register 0x25 "AUDIN_FIFO0_CTRL"
_dump_audioin_register 0x26 "AUDIN_FIFO0_CTRL1"
_dump_audioin_register 0x27 "AUDIN_FIFO0_LVL0"
_dump_audioin_register 0x28 "AUDIN_FIFO0_LVL1"
_dump_audioin_register 0x29 "AUDIN_FIFO0_LVL2"
_dump_audioin_register 0x30 "AUDIN_FIFO0_REQID"
_dump_audioin_register 0x31 "AUDIN_FIFO0_WRAP"
_dump_audioin_register 0x33 "AUDIN_FIFO1_START"
_dump_audioin_register 0x34 "AUDIN_FIFO1_END"
_dump_audioin_register 0x35 "AUDIN_FIFO1_PTR"
_dump_audioin_register 0x36 "AUDIN_FIFO1_INTR"
_dump_audioin_register 0x37 "AUDIN_FIFO1_RDPTR"
_dump_audioin_register 0x38 "AUDIN_FIFO1_CTRL"
_dump_audioin_register 0x39 "AUDIN_FIFO1_CTRL1"
_dump_audioin_register 0x40 "AUDIN_FIFO1_LVL0"
_dump_audioin_register 0x41 "AUDIN_FIFO1_LVL1"
_dump_audioin_register 0x42 "AUDIN_FIFO1_LVL2"
_dump_audioin_register 0x43 "AUDIN_FIFO1_REQID"
_dump_audioin_register 0x44 "AUDIN_FIFO1_WRAP"
_dump_audioin_register 0x51 "AUDIN_INT_CTRL"
_dump_audioin_register 0x80 "AUDOUT_CTRL"

echo -e "\n===== AUDIO OUT =====\n"

_dump_audioout_register 0x04 "AIU_958_MISC"
_dump_audioout_register 0x0a "AIU_958_CTRL"
_dump_audioout_register 0x0c "AIU_I2S_MUTE_SWAP"
