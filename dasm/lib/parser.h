#ifndef dasm_parser_h
#define dasm_parser_h

#include <dasm.h>
#include <libdice/type.h>
#include <libdice/opcode.h>
#include <ae2f/c90/StdBool.h>
#include "./tokenizer.h"

enum DASM_PARSER_ERR_ {
	DASM_PARSER_ERR_OK,
	DASM_PARSER_ERR_MEM_INSUF,
	DASM_PARSER_ERR_INVAL_LABEL,
	DASM_PARSER_ERR_INVAL_ASCII,
	DASM_PARSER_ERR_INVAL_TOK,
	DASM_PARSER_ERR_NO_TERM,
	DASM_PARSER_ERR_INVAL_OPERATOR,
	DASM_PARSER_ERR_INVAL_OPCODE,
	DASM_PARSER_ERR_INVAL_INSTRUCTION,
	DASM_PARSER_ERR_UNKNOWN
};

struct dasm_parser_ret {
	enum DASM_PARSER_ERR_ m_err;
	libdice_word_t m_line_cnt;
};

struct dasm_operand {
	char m_text[DASM_TOK_MAX_LEN];
};

struct dasm_parsed_line {
	enum LIBDICE_OPCODE_ m_opcode;
	struct dasm_operand m_operands[LIBDICE_OPERAND_MAX_CNT];
	libdice_word_t m_operand_cnt;
};
/** 
 * @param rdwr_parsed_line_cnt out
 * */
DICECALL struct dasm_parser_ret dasm_parse_programme(struct dasm_parsed_line rdwr_parsed_lines[], const libdice_word_t c_parsed_lines_len, 
					const struct dasm_tok_line rd_tok_lines[], const libdice_word_t c_tok_lines_len,
					libdice_word_t *rdwr_parsed_line_cnt);

#endif /* dasm_parser_h */
