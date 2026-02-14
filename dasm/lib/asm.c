#include "pp.h"
#include "tokenizer.h"
#include "parser.h"
#include "assembler.h"

#include <dasm.h>
#include <libdice/opcode.h>
#include <stdlib.h>
#include <stdio.h>

#define TOK_MAX_CNT_PER_LINE 10
#define MAX_TOK_LINE_CNT 20
#define LABEL_MAX_CNT 30
#define PARSED_LINE_MAX_CNT 30


DICEIMPL libdice_word_t dasm_assemble(
		ae2f_LP(c_num_ret) libdice_word_t* ae2f_restrict	rdwr_ret_buf,
		const libdice_word_t					c_ret_buf_len,
		ae2f_LP(str_len) const char* ae2f_restrict		rd_programme,
		const libdice_word_t					c_programme_len,
		struct label_table *rdwr_label_table)
{
	struct dasm_pp_status pp_status = {0,};
	struct dasm_tok_status tok_status = {0,};
	struct dasm_parser_status parser_status = {0,};
	struct dasm_asm_status asm_status = {0,};
	
	enum DASM_PP_ERR_ pp_errcode;
	enum DASM_TOK_ERR_ tok_errcode;
	enum DASM_PARSER_ERR_ parser_errcode;
	enum DASM_ASM_ERR_ asm_errcode;

	char preprocessed_programme[DASM_PROGRAMME_MAX_LEN] = {0,};
	libdice_word_t preprocessed_programme_cnt = 0;


	struct dasm_tok *tok_bufs;
	struct dasm_tok_line tok_lines[MAX_TOK_LINE_CNT] = {0,};

	struct dasm_parsed_line parsed_lines[PARSED_LINE_MAX_CNT] = {0,};
	struct dasm_label_table label_table = {0,};


	tok_bufs = malloc(sizeof(struct dasm_tok) * TOK_MAX_CNT_PER_LINE * MAX_TOK_LINE_CNT);
	if (tok_bufs==NULL) {
		printf("malloc failed\n");
		return DASM_ERR_RET;
	}
	for (libdice_word_t i=0; i<MAX_TOK_LINE_CNT; ++i) {
		tok_lines[i].m_toks = &tok_bufs[i * TOK_MAX_CNT_PER_LINE];
		tok_lines[i].m_toks_len = TOK_MAX_CNT_PER_LINE;
	}

	dasm_init_label_table(&label_table);

	label_table.m_labels = malloc(sizeof(struct dasm_label) * LABEL_MAX_CNT);
	if (!label_table.m_labels) {
		free(tok_bufs);
		printf("malloc failed\n");
		return DASM_ERR_RET;
	}
	label_table.m_labels_len = LABEL_MAX_CNT;
	

	if (c_programme_len > DASM_PROGRAMME_MAX_LEN) {
		free(tok_bufs);
		free(label_table.m_labels);
		return DASM_ERR_RET;
	}

	pp_ret = dasm_preprocess_prog	ramme(preprocessed_programme, DASM_PROGRAMME_MAX_LEN, rd_programme, c_programme_len, &preprocessed_programme_cnt);
	if (pp_ret.m_err != DASM_PP_ERR_OK) {
		free(tok_bufs);
		free(label_table.m_labels);
		printf("[ERROR] pp_ret.m_err = %u\n", pp_ret.m_err);
		return DASM_ERR_RET;
	}

	tok_errcode = dasm_tokenize_programme(tok_lines, DASM_PROGRAMME_MAX_LEN, preprocessed_programme, preprocessed_programme_cnt, &tok_status);
	if (tok_errcode != DASM_TOK_ERR_OK) {
		free(tok_bufs);
		free(label_table.m_labels);
		printf("[ERROR] tok_ret.m_err = %u\n", tok_errcode);
		return DASM_ERR_RET;
	}


	parser_errcode = dasm_parse_programme(parsed_lines, DASM_PROGRAMME_MAX_LEN, tok_lines, tok_status.m_write_tok_line_cnt, &label_table, &parser_status);
	if (parser_errcode != DASM_PARSER_ERR_OK) {
		free(tok_bufs);
		free(label_table.m_labels);
		printf("[ERROR] parser_ret.m_err = %u\n", parser_errcode);
		return DASM_ERR_RET;
	}

	free(tok_bufs);
	free(label_table.m_labels);

	asm_errcode = dasm_assemble_programme(rdwr_ret_buf, c_ret_buf_len, parsed_lines, parser_status.m_write_parsed_line_cnt, &asm_status);
	if (asm_errcode != DASM_ASM_ERR_OK) {
		
		printf("[ERROR] asm_ret.m_err = %u\n", asm_errcode);
		return DASM_ERR_RET;
	}
	

	return asm_status.m_write_word_cnt;
}


