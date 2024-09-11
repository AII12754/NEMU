#include "cpu/exec/helper.h"

#define instr je
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

#define instr jne
#define DATA_BYTE 1
#include "jcc-template.h"
#undef DATA_BYTE


#define DATA_BYTE 4
#include "jcc-template.h"
#undef DATA_BYTE
#undef instr
