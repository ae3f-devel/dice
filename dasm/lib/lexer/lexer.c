#include <ae2f/cc.h>
#include "lexer.h"
#include <ctype.h>
#include "dasm/err.h"
#include "toks.h"
#include <string.h>
#include <dasm/keys.h>
#include <stdlib.h>

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
	libdice_word_t i = 0;

	if (!rd_str) {
		return false;
	}

	for (i=0; i<DASM_MNEMONIC_CNT; ++i) {
		if (s_mnemonics[i].m_str_len == c_str_len
			&& !strncmp(rd_str, s_mnemonics[i].m_str, c_str_len)) {
			return true;
		}
	}

	return false;
}

DICEIMPL bool dasm_lexer_init(struct dasm_lexer *rdwr_lexer,
	struct dasm_tok rdwr_toks[], const libdice_word_t c_toks_len,
	const char rd_src[], const libdice_word_t c_src_len)
{
	if (!rdwr_lexer || !rdwr_toks || !rd_src) {
		return false;
	}

	if (!dasm_tok_stream_init(&(rdwr_lexer->m_tok_stream), rdwr_toks, c_toks_len)) {
		return false;
	}

	rdwr_lexer->m_src = rd_src;
	rdwr_lexer->m_src_len = c_src_len;
	rdwr_lexer->m_src_cnt = 0;

	rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;

	return true;
}

DICEIMPL bool dasm_lexer_deinit(struct dasm_lexer *rdwr_lexer)
{
	if (!rdwr_lexer) {
		return false;
	}

	if(!dasm_tok_stream_deinit(&rdwr_lexer->m_tok_stream)) {
		return false;
	}

	rdwr_lexer->m_src = NULL;
	rdwr_lexer->m_src_len = 0;
	rdwr_lexer->m_src_cnt = 0;

	rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;

	return true;
}

static ae2f_inline enum DASM_ERR_ dasm_lexer_execute_line(struct dasm_lexer *rdwr_lexer)
{
	char ch;
	const char *lexeme = NULL;

	libdice_word_t char_verification_cnt = 0;

	if (!rdwr_lexer || !rdwr_lexer->m_src) {
		return DASM_ERR_UNKNOWN;
	}

	while (rdwr_lexer->m_tok_stream.m_tok_cnt < rdwr_lexer->m_tok_stream.m_toks_len
		&& rdwr_lexer->m_src_cnt < rdwr_lexer->m_src_len) {

		lexeme = &(rdwr_lexer->m_src[rdwr_lexer->m_src_cnt]);
		ch = lexeme[0];

		switch (rdwr_lexer->m_state) {
		case DASM_LEXER_STATE_IDLE:

			if (ch == ' ') {
				rdwr_lexer->m_src_cnt++;
			} else if (ch == '\'') {

				char_verification_cnt = 0;

				rdwr_lexer->m_state = DASM_LEXER_STATE_CHAR_IMM;

				dasm_tok_stream_append(&rdwr_lexer->m_tok_stream);
				dasm_tok_stream_set_lexeme(&rdwr_lexer->m_tok_stream, lexeme, 1);
				dasm_tok_stream_set_type(&rdwr_lexer->m_tok_stream, DASM_TOK_TYPE_CHAR_IMM);
				
				rdwr_lexer->m_src_cnt++;

			} else if (ch == '\"') {

				rdwr_lexer->m_state = DASM_LEXER_STATE_STRING_IMM;

				dasm_tok_stream_append(&rdwr_lexer->m_tok_stream);
				dasm_tok_stream_set_lexeme(&rdwr_lexer->m_tok_stream, lexeme, 1);
				dasm_tok_stream_set_type(&rdwr_lexer->m_tok_stream, DASM_TOK_TYPE_STRING_IMM);

				rdwr_lexer->m_src_cnt++;

			} else if (ch == '*') {

				rdwr_lexer->m_state = DASM_LEXER_STATE_STAR;

				dasm_tok_stream_append(&rdwr_lexer->m_tok_stream);
				dasm_tok_stream_set_lexeme(&rdwr_lexer->m_tok_stream, lexeme, 1);
				dasm_tok_stream_set_type(&rdwr_lexer->m_tok_stream, DASM_TOK_TYPE_STAR);

				rdwr_lexer->m_src_cnt++;

			} else if (ch == '\n') {

				dasm_tok_stream_append(&rdwr_lexer->m_tok_stream);
				dasm_tok_stream_set_lexeme(&rdwr_lexer->m_tok_stream, lexeme, 1);
				dasm_tok_stream_set_type(&rdwr_lexer->m_tok_stream, DASM_TOK_TYPE_EOL);

				rdwr_lexer->m_src_cnt++;

				return DASM_ERR_OK;

			} else if (ch == '\0') {

				dasm_tok_stream_append(&rdwr_lexer->m_tok_stream);
				dasm_tok_stream_set_lexeme(&rdwr_lexer->m_tok_stream, lexeme, 1);
				dasm_tok_stream_set_type(&rdwr_lexer->m_tok_stream, DASM_TOK_TYPE_EOP);

				rdwr_lexer->m_src_cnt++;

				return DASM_ERR_OK;

			} else if (isalpha(ch) || ch == '_'){

				rdwr_lexer->m_state = DASM_LEXER_STATE_IDENT;

				dasm_tok_stream_append(&rdwr_lexer->m_tok_stream);
				dasm_tok_stream_set_lexeme(&rdwr_lexer->m_tok_stream, lexeme, 1);
				dasm_tok_stream_set_type(&rdwr_lexer->m_tok_stream, DASM_TOK_TYPE_IDENT);

				rdwr_lexer->m_src_cnt++;

			} else if (isdigit(ch)) {

				rdwr_lexer->m_state = DASM_LEXER_STATE_INT_IMM;

				dasm_tok_stream_append(&rdwr_lexer->m_tok_stream);
				dasm_tok_stream_set_lexeme(&rdwr_lexer->m_tok_stream, lexeme, 1);
				dasm_tok_stream_set_type(&rdwr_lexer->m_tok_stream, DASM_TOK_TYPE_INT_IMM);

				rdwr_lexer->m_src_cnt++;

			} else if (ch == ',') {

				dasm_tok_stream_append(&rdwr_lexer->m_tok_stream);
				dasm_tok_stream_set_lexeme(&rdwr_lexer->m_tok_stream, lexeme, 1);
				dasm_tok_stream_set_type(&rdwr_lexer->m_tok_stream, DASM_TOK_TYPE_COMMA);

				rdwr_lexer->m_src_cnt++;

			} else {
				return DASM_ERR_UNKNOWN;
			}
			break;

		case DASM_LEXER_STATE_IDENT:

			if (isalnum(ch) || ch == '_') {
				dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
				rdwr_lexer->m_src_cnt++;
			} else {
				struct dasm_tok *tok = NULL;
				if (!dasm_tok_stream_seek(&rdwr_lexer->m_tok_stream, 0, DASM_TOK_STREAM_WHENCE_END)) {
					return DASM_ERR_MEM_INSUF;
				}
				tok = dasm_tok_stream_peek(&rdwr_lexer->m_tok_stream);
				if (dasm_lexer_is_tok_mnemonic(tok->m_lexeme, tok->m_lexeme_len)) {
					dasm_tok_stream_set_type(&rdwr_lexer->m_tok_stream, DASM_TOK_TYPE_MNEMONIC);
				}
				rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;
			}
			break;

		case DASM_LEXER_STATE_INT_IMM:

			if (isdigit(ch)) {
				dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
				rdwr_lexer->m_src_cnt++;
			} else {
				rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;
			}
			break;

		case DASM_LEXER_STATE_CHAR_IMM:

			switch (ch) {
			case '\'':
				if (char_verification_cnt != 1) {
					return DASM_ERR_INVAL_CHAR_IMM;
				}
				dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
				rdwr_lexer->m_src_cnt++;
				rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;
				break;

			case '\n':
			case '\0':
				return DASM_ERR_INVAL_CHAR_IMM;
			default:
				char_verification_cnt++;

				dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
				rdwr_lexer->m_src_cnt++;
				break;
			}
			break;

		case DASM_LEXER_STATE_STRING_IMM:

			switch (ch) {
			case '\"':

				dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
				rdwr_lexer->m_src_cnt++;
				rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;
				break;

			case '\n':
			case '\0':
				return DASM_ERR_INVAL_STRING_IMM;
			default:

				dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
				rdwr_lexer->m_src_cnt++;
				break;

			}
			break;

		case DASM_LEXER_STATE_STAR:

			if (ch == '*') {
				dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
				rdwr_lexer->m_src_cnt++;
			} else {
				rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;
			}
			break;

		default:
			return DASM_ERR_UNKNOWN;
		}
	}

	if (rdwr_lexer->m_tok_stream.m_tok_cnt == rdwr_lexer->m_tok_stream.m_toks_len) {
		return DASM_ERR_MEM_INSUF;
	}

	if (rdwr_lexer->m_src_cnt == rdwr_lexer->m_src_len) {
		return DASM_ERR_NO_TERM;
	}

	return DASM_ERR_UNKNOWN;
}

DICEIMPL enum DASM_ERR_ dasm_lexer_execute(struct dasm_lexer *rdwr_lexer)
{
	enum DASM_ERR_ err = DASM_ERR_OK;

	if (!rdwr_lexer) {
		return DASM_ERR_UNKNOWN;
	}

	while (rdwr_lexer->m_tok_stream.m_tok_cnt < rdwr_lexer->m_tok_stream.m_toks_len
		&& rdwr_lexer->m_src_cnt < rdwr_lexer->m_src_len) {

		err = dasm_lexer_execute_line(rdwr_lexer);
		if (err != DASM_ERR_OK) {
			break;
		}
	}

	return err;
}
