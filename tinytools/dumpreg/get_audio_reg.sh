#!/bin/bash

dump_one_register () {
	#echo ${1} > /sys/kernel/debug/aml_reg/paddr
	#REG=`cat /sys/kernel/debug/aml_reg/paddr`
	#echo "${REG} - ${2}"
	printf " 0x%04X - 0x%04x\n" ${2} ${1}
}

REG_AUDIOIN=
REG_PDM=
REG_AUDIOOUT=
#
## Audio In registers:
##     Final addr = 0xC110A000 + offset*4
#
dump_one_reg_of_audio_in () {
	PAR1=$((${1}<<2))
	REG_AUDIOIN=$((0xC110A000+${PAR1}))
	dump_one_register ${REG_AUDIOIN} ${2}
}

#
## PDM registers:
##     Final addr = 0xD0042000 + offset*4
#
dump_one_reg_of_pdm () {
	PAR2=$((${1}<<2))
	REG_PDM=$((0xD0042000+${PAR2}))
	dump_one_register ${REG_PDM} ${2}
}

#
## Audio Out registers:
##     Final addr = 0xC1105400 + offset*4
#
dump_one_reg_of_audio_out () {
	PAR3=$((${1}<<2))
	REG_AUDIOOUT=$((0xC1105400+${PAR3}))
	dump_one_register ${REG_AUDIOOUT} ${2}
}

dump_reg_addr_list () {
	printf "\n------ AUDIO IN REG LIST -----\n\n"
	for ((i=0x00; i<=0xc1; i++))
	do
		dump_one_reg_of_audio_in ${i} ${i}
	done

	printf "\n------ AUDIO OUT REG LIST -----\n\n"
	for ((i=0x00; i<=0xab; i++))
	do
		dump_one_reg_of_audio_out ${i} ${i}
	done
}

AUDIO_IN_DUMP_LIST="0x00 0x01 0x02 0x03 0x44"

for offset in ${AUDIO_IN_DUMP_LIST}
do
	dump_one_reg_of_audio_in ${offset} ${offset}
done
