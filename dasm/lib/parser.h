#ifndef dasm_parser_h
#define dasm_parser_h

#include <dasm.h>
#include <libdice/type.h>
#include <libdice/opcode.h>
#include <ae2f/c90/StdBool.h>
#include "label.h"
#include "./tokenizer.h"

#define DASM_OPERAND_MAX_LEN 64

enum DASM_PARSER_ERR_ {
	DASM_PARSER_ERR_OK,
	DASM_PARSER_ERR_MEM_INSUF,
	DASM_PARSER_ERR_INVAL_LABEL,
	DASM_PARSER_ERR_INVAL_ASCII,
	DASM_PARSER_ERR_INVAL_TOK,
	DASM_PARSER_ERR_NO_TERM,
	DASM_PARSER_ERR_INVAL_OPERATOR,
	DASM_PARSER_ERR_INVAL_STRING,
	DASM_PARSER_ERR_INVAL_OPCODE,
	DASM_PARSER_ERR_INVAL_INSTRUCTION,
	DASM_PARSER_ERR_LABEL_MEM_INSUF,
	DASM_PARSER_ERR_UNKNOWN
};

struct dasm_parser_status {
	libdice_word_t m_read_tok_line_cnt_for_label_table;
	libdice_word_t m_read_tok_line_cnt;
	libdice_word_t m_write_parsed_line_cnt;
};

struct dasm_operand {
	char m_text[DASM_OPERAND_MAX_LEN];
};

struct dasm_parsed_line {
	enum LIBDICE_OPCODE_ m_opcode;
	struct dasm_operand m_operands[LIBDICE_OPERAND_MAX_CNT];
	libdice_word_t m_operand_cnt;
};

DICECALL  void dasm_init_label_table(struct dasm_label_table *rdwr_label_table);

DICECALL enum DASM_PARSER_ERR_ dasm_parse_programme(struct dasm_parsed_line rdwr_parsed_lines[], const libdice_word_t c_parsed_lines_len, 
		const struct dasm_tok_line rd_tok_lines[], const libdice_word_t c_tok_lines_len,
		struct dasm_label_table *rdwr_label_table, struct dasm_parser_status *rdwr_status);

#endif /* dasm_parser_h */
