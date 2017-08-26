#ifndef _DUMP_H_
#define _DUMP_H_

#define AUDIO_IN_MODULE_REG_BASE  (0xC110A000)
#define AUDIO_PDM_MODULE_REG_BASE (0xD0042000)
#define AUDIO_OUT_MODULE_REG_BASE (0xC1105400)

#define REAL_ADDR(base, index) (base+(index<<2))

enum eTypeReg {
	eTypeIn  = 0,
	eTypeOut = 1,
	eTypePdm = 2,
	eTypeMax,
};

#define TYPE_AUDIO_IN  "in"
#define TYPE_AUDIO_OUT "out"
#define TYPE_AUDIO_PDM "pdm"

#define AML_REG_DBG_CTRL "/sys/kernel/debug/aml_reg/paddr"

#endif
