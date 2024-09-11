#include "cpu/exec/helper.h"

#define instr jcc_z
#define DATA_BYTE 1
#include "jcc-template.h"
#undef DATA_BYTE


#define DATA_BYTE 4
#include "jcc-template.h"
#undef DATA_BYTE
#undef instr

#define instr jbe
#define DATA_BYTE 1
#include "jcc-template.h"
#undef DATA_BYTE


#define DATA_BYTE 4
#include "jcc-template.h"
#undef DATA_BYTE
#undef instr
