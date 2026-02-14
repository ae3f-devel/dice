#define _CRT_SECURE_NO_WARNINGS


#include "parser.h"
#include "tokenizer.h"
#include <string.h>
#include <dasm/keys.h>
#include <assert.h>
#include <stdio.h>



#define PARSER_MAX_LABEL_CNT 1000

struct dasm_label {
	char m_text[DASM_TOK_MAX_LEN];
	libdice_word_t m_address;
};

struct dasm_label_table {
	struct dasm_label m_labels[PARSER_MAX_LABEL_CNT];
	libdice_word_t m_label_cnt;
};

struct dasm_opcode_define {
	const char m_mnemonic[DASM_TOK_MAX_LEN];
	const enum LIBDICE_OPCODE_ m_opcode;
	const libdice_word_t m_operand_cnt;
};

static const struct dasm_opcode_define s_opcode_define_table[LIBDICE_OPCODE_CNT] = {
	{DASM_OPCODE_TXT_NOP,        LIBDICE_OPCODE_NOP,        0},

	{DASM_OPCODE_TXT_IADD,       LIBDICE_OPCODE_IADD,       5},
	{DASM_OPCODE_TXT_ISUB,       LIBDICE_OPCODE_ISUB,       5},
	{DASM_OPCODE_TXT_IMUL,       LIBDICE_OPCODE_IMUL,       5},
	{DASM_OPCODE_TXT_IDIV,       LIBDICE_OPCODE_IDIV,       5},
	{DASM_OPCODE_TXT_IREM,       LIBDICE_OPCODE_IREM,       5},

	{DASM_OPCODE_TXT_UMUL,       LIBDICE_OPCODE_UMUL,       5},
	{DASM_OPCODE_TXT_UDIV,       LIBDICE_OPCODE_UDIV,       5},
	{DASM_OPCODE_TXT_UREM,       LIBDICE_OPCODE_UREM,       5},

	{DASM_OPCODE_TXT_FADD,       LIBDICE_OPCODE_FADD,       5},
	{DASM_OPCODE_TXT_FSUB,       LIBDICE_OPCODE_FSUB,       5},
	{DASM_OPCODE_TXT_FMUL,       LIBDICE_OPCODE_FMUL,       5},
	{DASM_OPCODE_TXT_FDIV,       LIBDICE_OPCODE_FDIV,       5},

	{DASM_OPCODE_TXT_INEG,       LIBDICE_OPCODE_INEG,       3},
	{DASM_OPCODE_TXT_FNEG,       LIBDICE_OPCODE_FNEG,       3},

	{DASM_OPCODE_TXT_JMP,        LIBDICE_OPCODE_JMP,        2},
	{DASM_OPCODE_TXT_JMPA,       LIBDICE_OPCODE_JMPA,       2},
	{DASM_OPCODE_TXT_JMPN,       LIBDICE_OPCODE_JMPN,       2},

	{DASM_OPCODE_TXT_JMPZ,       LIBDICE_OPCODE_JMPZ,       4},
	{DASM_OPCODE_TXT_JMPZA,      LIBDICE_OPCODE_JMPZA,      4},
	{DASM_OPCODE_TXT_JMPZN,      LIBDICE_OPCODE_JMPZN,      4},

	{DASM_OPCODE_TXT_BAND,       LIBDICE_OPCODE_BAND,       5},
	{DASM_OPCODE_TXT_BOR,        LIBDICE_OPCODE_BOR,        5},
	{DASM_OPCODE_TXT_BXOR,       LIBDICE_OPCODE_BXOR,       5},
	{DASM_OPCODE_TXT_BLSHIFT,    LIBDICE_OPCODE_BLSHIFT,    5},
	{DASM_OPCODE_TXT_BRSHIFT,    LIBDICE_OPCODE_BRSHIFT,    5},
	{DASM_OPCODE_TXT_LRSHIFT,    LIBDICE_OPCODE_LRSHIFT,    5},
	{DASM_OPCODE_TXT_BNOT,       LIBDICE_OPCODE_BNOT,       3},

	{DASM_OPCODE_TXT_LAND,       LIBDICE_OPCODE_LAND,       5},
	{DASM_OPCODE_TXT_LOR,        LIBDICE_OPCODE_LOR,        5},
	{DASM_OPCODE_TXT_LNOT,       LIBDICE_OPCODE_LNOT,       3},

	{DASM_OPCODE_TXT_TOBIT,      LIBDICE_OPCODE_TOBIT,      3},

	{DASM_OPCODE_TXT_EQ,         LIBDICE_OPCODE_EQ,         5},
	{DASM_OPCODE_TXT_NEQ,        LIBDICE_OPCODE_NEQ,        5},

	{DASM_OPCODE_TXT_SET,        LIBDICE_OPCODE_SET,        2},
	{DASM_OPCODE_TXT_MSET,       LIBDICE_OPCODE_MSET,       4},
	{DASM_OPCODE_TXT_MOV,        LIBDICE_OPCODE_MOV,        4},

	{DASM_OPCODE_TXT_ITOF,       LIBDICE_OPCODE_ITOF,       3},
	{DASM_OPCODE_TXT_FTOI,       LIBDICE_OPCODE_FTOI,       3},

	{DASM_OPCODE_TXT_IGT,        LIBDICE_OPCODE_IGT,        5},
	{DASM_OPCODE_TXT_FGT,        LIBDICE_OPCODE_FGT,        5},
	{DASM_OPCODE_TXT_ILT,        LIBDICE_OPCODE_ILT,        5},
	{DASM_OPCODE_TXT_FLT,        LIBDICE_OPCODE_FLT,        5},

	{DASM_OPCODE_TXT_PUTS,       LIBDICE_OPCODE_PUTS,       1},
	{DASM_OPCODE_TXT_PUTI,       LIBDICE_OPCODE_PUTI,       2},
	{DASM_OPCODE_TXT_PUTU,       LIBDICE_OPCODE_PUTU,       2},
	{DASM_OPCODE_TXT_PUTC,       LIBDICE_OPCODE_PUTC,       2},
	{DASM_OPCODE_TXT_PUTF,       LIBDICE_OPCODE_PUTF,       2},

	{DASM_OPCODE_TXT_DEF,        LIBDICE_OPCODE_DEF,        1},
	{DASM_OPCODE_TXT_UNDEF,      LIBDICE_OPCODE_UNDEF,      1},

	{DASM_OPCODE_TXT_RAND,       LIBDICE_OPCODE_RAND,	2},


	{DASM_OPCODE_TXT_EOP,        LIBDICE_OPCODE_EOP,        0},
};


static void dasm_init_label_table(struct dasm_label_table *rdwr_label_table)
{
	rdwr_label_table->m_label_cnt = 0;
	memset(rdwr_label_table->m_labels, 0, sizeof(struct dasm_label) * PARSER_MAX_LABEL_CNT);
}

static bool dasm_get_label_address(const struct dasm_label_table *rd_label_table, const char c_label[], libdice_word_t *addr)
{
	libdice_word_t i = 0;

	for (i=0; i<rd_label_table->m_label_cnt; i++) {
		if (strcmp(rd_label_table->m_labels[i].m_text, c_label) == 0) {
			*addr = rd_label_table->m_labels[i].m_address;
			return true;
		}
	}

	return false;
}

static bool dasm_insert_label(struct dasm_label_table *rdwr_label_table, const char c_label[], const libdice_word_t c_addr)
{
	if (rdwr_label_table->m_label_cnt == PARSER_MAX_LABEL_CNT) {
		return false;
	}

	strncpy(rdwr_label_table->m_labels[rdwr_label_table->m_label_cnt].m_text, c_label, DASM_TOK_MAX_LEN-1);
	rdwr_label_table->m_labels[rdwr_label_table->m_label_cnt].m_address = c_addr;
	rdwr_label_table->m_label_cnt++;

	return true;	
}

static enum DASM_PARSER_ERR_ dasm_label_programme(struct dasm_label_table *rdwr_label_table, const struct dasm_tok_line rd_tok_lines[], const libdice_word_t c_tok_lines_len)
{
	libdice_word_t tok_lines_idx = 0;
	libdice_word_t tmp = 0;
	libdice_word_t pc = 0;

	dasm_init_label_table(rdwr_label_table);

	for (tok_lines_idx=0; tok_lines_idx<c_tok_lines_len; tok_lines_idx++) {
		const struct dasm_tok_line *rd_tok_line = &(rd_tok_lines[tok_lines_idx]);

		if (rd_tok_line->m_tok_cnt == 0) {
			continue;
		}
		
		if (rd_tok_line->m_toks[0].m_tok_type == DASM_TOK_TYPE_LABEL) {
			/* label must be defined only once */
			if (dasm_get_label_address(rdwr_label_table, rd_tok_line->m_toks[0].m_text, &tmp)) {
				return DASM_PARSER_ERR_INVAL_LABEL;
			}
			dasm_insert_label(rdwr_label_table, rd_tok_line->m_toks[0].m_text, pc);
		}
		pc += dasm_get_tok_line_word_len(rd_tok_line);

		if (rd_tok_line->m_toks[rd_tok_line->m_tok_cnt-1].m_tok_type == DASM_TOK_TYPE_EOP) {
			return DASM_PARSER_ERR_OK;
		}
	}

	/* Couldn't find EOP */
	return DASM_PARSER_ERR_NO_TERM;
}

static bool dasm_get_opcode_table_idx(const char rd_mnemonic[], libdice_word_t *rdwr_idx)
{
	libdice_word_t idx = 0;
	for (idx=0; idx<LIBDICE_OPCODE_CNT; ++idx) {
		if (strcmp(s_opcode_define_table[idx].m_mnemonic, rd_mnemonic) == 0) {
			*rdwr_idx = idx;
			return true;;
		}
	}
	return false;
}

static void dasm_init_parsed_line(struct dasm_parsed_line *rdwr_parsed_line)
{
	rdwr_parsed_line->m_opcode = 0;
	memset(rdwr_parsed_line->m_operands, 0, sizeof(struct dasm_operand) * LIBDICE_OPERAND_MAX_CNT);
	rdwr_parsed_line->m_operand_cnt = 0;
}


static enum DASM_PARSER_ERR_ dasm_parse_operand(struct dasm_operand *rdwr_operand, const struct dasm_tok *rd_tok, 
		const struct dasm_label_table *rd_label_table, bool *rdwr_is_operand)
{
	*rdwr_is_operand = false;
	switch (rd_tok->m_tok_type) 
	{
		case DASM_TOK_TYPE_IDENT:
			{
				libdice_word_t label_addr = 0;

				if (dasm_get_label_address(rd_label_table, rd_tok->m_text, &label_addr)) {
					/* This tok is label */
					int tmp = 0;
					tmp = snprintf(rdwr_operand->m_text, DASM_TOK_MAX_LEN, "%u", label_addr);
					if (tmp >= DASM_TOK_MAX_LEN || tmp<0) {
						return DASM_PARSER_ERR_INVAL_LABEL;
					}
					*rdwr_is_operand = true;
					return DASM_PARSER_ERR_OK;
				}
				/* This tok is opcode */
				strncpy(rdwr_operand->m_text, rd_tok->m_text, DASM_TOK_MAX_LEN-1);
				*rdwr_is_operand = true;
				return DASM_PARSER_ERR_OK;
			}
		case DASM_TOK_TYPE_NUMBER:
			*rdwr_is_operand = true;
			strncpy(rdwr_operand->m_text, rd_tok->m_text, DASM_TOK_MAX_LEN-1);
			return DASM_PARSER_ERR_OK;
		case DASM_TOK_TYPE_LABEL:
			/* label must be handled by  dasm_parse_line. This means the label was placed in the middle of the line, not at the beginning*/
			return DASM_PARSER_ERR_INVAL_LABEL;
		case DASM_TOK_TYPE_STRING:
			
				
			return DASM_PARSER_ERR_INVAL_STRING;
			
		case DASM_TOK_TYPE_ASCII:
			{
				int tmp = 0;
				tmp = snprintf(rdwr_operand->m_text, DASM_TOK_MAX_LEN, "%u", (unsigned)(unsigned char)rd_tok->m_text[0]);
				if (tmp >= DASM_TOK_MAX_LEN || tmp<0) {
					return DASM_PARSER_ERR_INVAL_ASCII;
				}
				*rdwr_is_operand = true;
				return DASM_PARSER_ERR_OK;
			}
		case DASM_TOK_TYPE_OPERATOR:
			{
				int tmp = 0;
				if (rd_tok->m_text[0] == '#') {
					if (strlen(rd_tok->m_text) == 1) {
						tmp = snprintf(rdwr_operand->m_text, DASM_TOK_MAX_LEN, "%u", (unsigned)0);
						if (tmp >= DASM_TOK_MAX_LEN || tmp<0) {
							return DASM_PARSER_ERR_INVAL_OPERATOR;
						}
						*rdwr_is_operand = true;
						return DASM_PARSER_ERR_OK;
					} else {
						return DASM_PARSER_ERR_INVAL_OPERATOR;
					}
				}
				tmp = snprintf(rdwr_operand->m_text, DASM_TOK_MAX_LEN, "%u", (unsigned)strlen(rd_tok->m_text));
				if (tmp >= DASM_TOK_MAX_LEN || tmp<0) {
					return DASM_PARSER_ERR_INVAL_OPERATOR;
				}
				*rdwr_is_operand = true;
				return DASM_PARSER_ERR_OK;
			}
		case DASM_TOK_TYPE_EOL:
		case DASM_TOK_TYPE_EOP:
			return DASM_PARSER_ERR_OK;
		case DASM_TOK_TYPE_UNKNOWN:
		default:
			return DASM_PARSER_ERR_INVAL_TOK;
	}
}

static enum DASM_PARSER_ERR_ dasm_parse_line(struct dasm_parsed_line *rdwr_parsed_line, const struct dasm_tok_line *rd_tok_line, 
		const struct dasm_label_table *rd_label_table)
{
	libdice_word_t tok_line_idx = 0;
	libdice_word_t opcode_table_idx = 0;
	const struct dasm_tok *rd_tok = NULL;
	libdice_word_t tmp_operand_cnt = 0;
	enum DASM_PARSER_ERR_ errcode = DASM_PARSER_ERR_OK;
	bool is_operand = false;

	dasm_init_parsed_line(rdwr_parsed_line);

	if (rd_tok_line->m_tok_cnt == 0 || rd_tok_line->m_tok_cnt >= DASM_TOK_MAX_CNT_PER_LINE) {
		return DASM_PARSER_ERR_NO_TERM;
	}

	if (rd_tok_line->m_tok_cnt==1
			&& (rd_tok_line->m_toks[0].m_tok_type==DASM_TOK_TYPE_EOL 
				|| rd_tok_line->m_toks[0].m_tok_type==DASM_TOK_TYPE_EOP)) {
		return DASM_PARSER_ERR_OK;
	}

	/* Skip label tok */
	if (rd_tok_line->m_toks[0].m_tok_type == DASM_TOK_TYPE_LABEL) {
		tok_line_idx++;
	}

	rd_tok = &(rd_tok_line->m_toks[tok_line_idx]);
	tok_line_idx++;

	/* Check whether the line contains only a label */
	if (rd_tok->m_tok_type==DASM_TOK_TYPE_EOL || rd_tok->m_tok_type==DASM_TOK_TYPE_EOP) {
		return DASM_PARSER_ERR_OK;
	}

	/* opcode must be found in opcode_table */
	if (!dasm_get_opcode_table_idx(rd_tok->m_text, &opcode_table_idx)) {
		return DASM_PARSER_ERR_INVAL_OPCODE;
	}
	rdwr_parsed_line->m_opcode = s_opcode_define_table[opcode_table_idx].m_opcode;
	rdwr_parsed_line->m_operand_cnt = s_opcode_define_table[opcode_table_idx].m_operand_cnt;

	tmp_operand_cnt = 0;
	for (;tok_line_idx<rd_tok_line->m_tok_cnt && tmp_operand_cnt<LIBDICE_OPERAND_MAX_CNT; tok_line_idx++) {

		rd_tok = &(rd_tok_line->m_toks[tok_line_idx]);

	

		errcode = dasm_parse_operand(&(rdwr_parsed_line->m_operands[tmp_operand_cnt]), rd_tok, rd_label_table, &is_operand);	
		if (is_operand) {
			tmp_operand_cnt++;
		}

		if (errcode != DASM_PARSER_ERR_OK) {
			return errcode;
		}
	}
	if (tmp_operand_cnt != rdwr_parsed_line->m_operand_cnt) {
		return DASM_PARSER_ERR_INVAL_INSTRUCTION;
	}	
	return errcode; 
} 


DICEIMPL struct dasm_parser_ret dasm_parse_programme(struct dasm_parsed_line rdwr_parsed_lines[], const libdice_word_t c_parsed_lines_len, 
		const struct dasm_tok_line rd_tok_lines[], const libdice_word_t c_tok_lines_len,
		libdice_word_t *rdwr_parsed_line_cnt)
{
	libdice_word_t line_cnt = 0;
	libdice_word_t parsed_line_cnt = 0;
	libdice_word_t tok_line_cnt = 0;
	struct dasm_label_table label_table;
	struct dasm_parser_ret ret;

	dasm_init_label_table(&label_table);
	ret.m_err = dasm_label_programme(&label_table, rd_tok_lines, c_tok_lines_len);
	if (ret.m_err != DASM_PARSER_ERR_OK) {
		*rdwr_parsed_line_cnt = 0;
		return ret;
	}

	for (tok_line_cnt=0; parsed_line_cnt<c_parsed_lines_len && tok_line_cnt<c_tok_lines_len; ++tok_line_cnt) {
		ret.m_err = dasm_parse_line(&rdwr_parsed_lines[parsed_line_cnt], &rd_tok_lines[tok_line_cnt], &label_table);
		parsed_line_cnt++;
		line_cnt++;
		if (ret.m_err != DASM_PARSER_ERR_OK) {
			break;
		}
	}
	
	*rdwr_parsed_line_cnt = parsed_line_cnt;
	ret.m_line_cnt = line_cnt;

	return ret;
}
