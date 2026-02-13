#ifndef dasm_assembler_h
#define dasm_assembler_h

#include "parser.h"
#include <dasm.h>

enum DASM_ASM_ERR_ {
	DASM_ASM_ERR_OK,
	DASM_ASM_ERR_MEM_INSUF,
	DASM_ASM_ERR_INVAL_ARG,
	DASM_ASM_ERR_UNKNOWN
};

struct dasm_asm_ret {
	enum DASM_ASM_ERR_ m_err;
	libdice_word_t m_line_cnt;
};
DICECALL struct dasm_asm_ret dasm_assemble_programme(libdice_word_t rdwr_programme[], const libdice_word_t c_programme_len, 
					const struct dasm_parsed_line rd_parsed_lines[], const libdice_word_t c_parsed_lines_len,
					libdice_word_t *rdwr_write_cnt);




#endif /* dasm_assembler_h */
