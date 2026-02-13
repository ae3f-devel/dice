#include "pp.h"
#include "tokenizer.h"
#include "parser.h"
#include "assembler.h"

#include <dasm.h>
#include <libdice/opcode.h>

#include <stdio.h>


DICEIMPL libdice_word_t dasm_assemble(
		ae2f_LP(c_num_ret) libdice_word_t* ae2f_restrict	rdwr_ret_buf,
		const libdice_word_t					c_ret_buf_len,
		ae2f_LP(str_len) const char* ae2f_restrict		rd_programme,
		const libdice_word_t					c_programme_len
		)
{
	struct dasm_pp_ret pp_ret;
	struct dasm_tok_ret tok_ret;
	struct dasm_parser_ret parser_ret;
	struct dasm_asm_ret asm_ret;

	char tmp_buf[DASM_PROGRAMME_MAX_LEN] = {0,};
	libdice_word_t buf_cnt = 0;

	struct dasm_tok_line tok_lines[DASM_PROGRAMME_MAX_LEN] = {0,};
	libdice_word_t tok_line_cnt = 0;

	struct dasm_parsed_line parsed_lines[DASM_PROGRAMME_MAX_LEN] = {0,};
	libdice_word_t parsed_line_cnt = 0;

	libdice_word_t ret_buf_cnt = 0;

	if (c_programme_len > DASM_PROGRAMME_MAX_LEN) {
		return DASM_ERR_RET;
	}

	pp_ret = dasm_preprocess_programme(tmp_buf, DASM_PROGRAMME_MAX_LEN, rd_programme, c_programme_len, &buf_cnt);
	if (pp_ret.m_err != DASM_PP_ERR_OK) {
		printf("[ERROR] pp_ret.m_err = %u\n", pp_ret.m_err);
		return DASM_ERR_RET;
	}

	printf("%s", tmp_buf);

	tok_ret = dasm_tokenize_programme(tok_lines, DASM_PROGRAMME_MAX_LEN, tmp_buf, buf_cnt, &tok_line_cnt);
	if (tok_ret.err != DASM_TOK_ERR_OK) {
		printf("[ERROR] tok_ret.m_err = %u\n", tok_ret.err);
		return DASM_ERR_RET;
	}

	parser_ret = dasm_parse_programme(parsed_lines, DASM_PROGRAMME_MAX_LEN, tok_lines, tok_line_cnt, &parsed_line_cnt);
	if (parser_ret.m_err != DASM_PARSER_ERR_OK) {
		printf("[ERROR] parser_ret.m_err = %u\n", parser_ret.m_err);
		return DASM_ERR_RET;
	}

	asm_ret = dasm_assemble_programme(rdwr_ret_buf, c_ret_buf_len, parsed_lines, parsed_line_cnt, &ret_buf_cnt);
	if (asm_ret.m_err != DASM_ASM_ERR_OK) {
		printf("[ERROR] asm_ret.m_err = %u\n", asm_ret.m_err);
		return DASM_ERR_RET;
	}

	return ret_buf_cnt;
}


