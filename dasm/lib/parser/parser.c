#include "./parser.h"
#include <string.h>
#include <dasm/keys.h>

static struct {
	const char *m_str;
	libdice_word_t m_str_len;

} s_mnemonics[DASM_MNEMONIC_CNT] = {
	{ DASM_MNEMONIC_NOP,     sizeof(DASM_MNEMONIC_NOP) - 1},

	{ DASM_MNEMONIC_IADD,    sizeof(DASM_MNEMONIC_IADD) - 1},
	{ DASM_MNEMONIC_ISUB,    sizeof(DASM_MNEMONIC_ISUB) - 1},
	{ DASM_MNEMONIC_IMUL,    sizeof(DASM_MNEMONIC_IMUL) - 1},
	{ DASM_MNEMONIC_IDIV,    sizeof(DASM_MNEMONIC_IDIV) - 1},
	{ DASM_MNEMONIC_IREM,    sizeof(DASM_MNEMONIC_IREM) - 1},
	{ DASM_MNEMONIC_UMUL,    sizeof(DASM_MNEMONIC_UMUL) - 1},
	{ DASM_MNEMONIC_UDIV,    sizeof(DASM_MNEMONIC_UDIV) - 1},
	{ DASM_MNEMONIC_UREM,    sizeof(DASM_MNEMONIC_UREM) - 1},

	{ DASM_MNEMONIC_FADD,    sizeof(DASM_MNEMONIC_FADD) - 1},
	{ DASM_MNEMONIC_FSUB,    sizeof(DASM_MNEMONIC_FSUB) - 1},
	{ DASM_MNEMONIC_FMUL,    sizeof(DASM_MNEMONIC_FMUL) - 1},
	{ DASM_MNEMONIC_FDIV,    sizeof(DASM_MNEMONIC_FDIV) - 1},

	{ DASM_MNEMONIC_INEG,    sizeof(DASM_MNEMONIC_INEG) - 1},
	{ DASM_MNEMONIC_FNEG,    sizeof(DASM_MNEMONIC_FNEG) - 1},

	{ DASM_MNEMONIC_JMP,     sizeof(DASM_MNEMONIC_JMP) - 1},
	{ DASM_MNEMONIC_JMPA,    sizeof(DASM_MNEMONIC_JMPA) - 1},
	{ DASM_MNEMONIC_JMPN,    sizeof(DASM_MNEMONIC_JMPN) - 1},

	{ DASM_MNEMONIC_JMPZ,    sizeof(DASM_MNEMONIC_JMPZ) - 1},
	{ DASM_MNEMONIC_JMPZA,   sizeof(DASM_MNEMONIC_JMPZA) - 1},
	{ DASM_MNEMONIC_JMPZN,   sizeof(DASM_MNEMONIC_JMPZN) - 1},

	{ DASM_MNEMONIC_BAND,    sizeof(DASM_MNEMONIC_BAND) - 1},
	{ DASM_MNEMONIC_BOR,     sizeof(DASM_MNEMONIC_BOR) - 1},
	{ DASM_MNEMONIC_BXOR,    sizeof(DASM_MNEMONIC_BXOR) - 1},
	{ DASM_MNEMONIC_BLSHIFT, sizeof(DASM_MNEMONIC_BLSHIFT) - 1},
	{ DASM_MNEMONIC_BRSHIFT, sizeof(DASM_MNEMONIC_BRSHIFT) - 1},
	{ DASM_MNEMONIC_LRSHIFT, sizeof(DASM_MNEMONIC_LRSHIFT) - 1},
	{ DASM_MNEMONIC_BNOT,    sizeof(DASM_MNEMONIC_BNOT) - 1},

	{ DASM_MNEMONIC_LAND,    sizeof(DASM_MNEMONIC_LAND) - 1},
	{ DASM_MNEMONIC_LOR,     sizeof(DASM_MNEMONIC_LOR) - 1},
	{ DASM_MNEMONIC_LNOT,    sizeof(DASM_MNEMONIC_LNOT) - 1},

	{ DASM_MNEMONIC_TOBIT,   sizeof(DASM_MNEMONIC_TOBIT) - 1},

	{ DASM_MNEMONIC_EQ,      sizeof(DASM_MNEMONIC_EQ) - 1},
	{ DASM_MNEMONIC_NEQ,     sizeof(DASM_MNEMONIC_NEQ) - 1},

	{ DASM_MNEMONIC_SET,     sizeof(DASM_MNEMONIC_SET) - 1},
	{ DASM_MNEMONIC_MSET,    sizeof(DASM_MNEMONIC_MSET) - 1},
	{ DASM_MNEMONIC_MOV,     sizeof(DASM_MNEMONIC_MOV) - 1},

	{ DASM_MNEMONIC_ITOF,    sizeof(DASM_MNEMONIC_ITOF) - 1},
	{ DASM_MNEMONIC_FTOI,    sizeof(DASM_MNEMONIC_FTOI) - 1},

	{ DASM_MNEMONIC_IGT,     sizeof(DASM_MNEMONIC_IGT) - 1},
	{ DASM_MNEMONIC_FGT,     sizeof(DASM_MNEMONIC_FGT) - 1},
	{ DASM_MNEMONIC_ILT,     sizeof(DASM_MNEMONIC_ILT) - 1},
	{ DASM_MNEMONIC_FLT,     sizeof(DASM_MNEMONIC_FLT) - 1},

	{ DASM_MNEMONIC_PUTS,    sizeof(DASM_MNEMONIC_PUTS) - 1},
	{ DASM_MNEMONIC_PUTI,    sizeof(DASM_MNEMONIC_PUTI) - 1},
	{ DASM_MNEMONIC_PUTU,    sizeof(DASM_MNEMONIC_PUTU) - 1},
	{ DASM_MNEMONIC_PUTC,    sizeof(DASM_MNEMONIC_PUTC) - 1},
	{ DASM_MNEMONIC_PUTF,    sizeof(DASM_MNEMONIC_PUTF) - 1},

	{ DASM_MNEMONIC_DEF,     sizeof(DASM_MNEMONIC_DEF) - 1},
	{ DASM_MNEMONIC_UNDEF,   sizeof(DASM_MNEMONIC_UNDEF) - 1},

	{ DASM_MNEMONIC_RAND,    sizeof(DASM_MNEMONIC_RAND) - 1},
	{ DASM_MNEMONIC_TIME,    sizeof(DASM_MNEMONIC_TIME) - 1},

	{ DASM_MNEMONIC_EOP,     sizeof(DASM_MNEMONIC_EOP) - 1},
};

static ae2f_inline bool dasm_lexer_is_tok_mnemonic(const char rd_str[], const libdice_word_t c_str_len)
{
	libdice_word_t i;

	if (!rd_str) {
		return false;
	}

	for (i = 0; i < DASM_MNEMONIC_CNT; ++i) {
		if (s_mnemonics[i].m_str_len == c_str_len
			&& !strncmp(rd_str, s_mnemonics[i].m_str, c_str_len)) {
			return true;
		}
	}

	return false;
}
