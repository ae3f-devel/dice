

#include <dasm.h>
#include <libdice/opcode.h>
#include "pp.h"
#include "tokenizer.h"
#include "parser.h"
#include "assembler.h"



DICEIMPL libdice_word_t libdasm_assemble(
		ae2f_LP(c_num_ret) libdice_word_t* ae2f_restrict	rdwr_ret_buf,
		const libdice_word_t					c_ret_buf_len,
		ae2f_LP(str_len) const char* ae2f_restrict		rd_programme,
		const libdice_word_t					c_programme_len
		)
{
	(void)rdwr_ret_buf;
	(void)c_ret_buf_len;
	(void)rd_programme;
	(void)c_programme_len;
	char buf[LIBDASM_PROGRAMME_MAX_LEN] = {0,};
	libdice_word_t buf_cnt = 0;
	struct libdasm_token_line token_lines[LIBDASM_PROGRAMME_MAX_LEN] = {0,};
	libdice_word_t token_line_cnt = 0;
	struct libdasm_parsed_line parsed_lines[LIBDASM_PROGRAMME_MAX_LEN] = {0,};
	libdice_word_t parsed_line_cnt = 0;

	if (c_programme_len > LIBDASM_PROGRAMME_MAX_LEN) {
		return LIBDASM_ERR_RET;
	}

	buf_cnt = libdasm_preprocess_programme(buf, LIBDASM_PROGRAMME_MAX_LEN, rd_programme, c_programme_len);
	if (buf_cnt == LIBDASM_ERR_RET) {
		return LIBDASM_ERR_RET;
	}

	token_line_cnt = libdasm_tokenize_programme(token_lines, LIBDASM_PROGRAMME_MAX_LEN, buf, buf_cnt);
	if (token_line_cnt == LIBDASM_ERR_RET) {
		return LIBDASM_ERR_RET;
	}

	parsed_line_cnt = libdasm_parse_programme(parsed_lines, LIBDASM_PROGRAMME_MAX_LEN, token_lines, token_line_cnt);
	if (parsed_line_cnt == LIBDASM_ERR_RET) {
		return LIBDASM_ERR_RET;
	}

	return libdasm_assemble_programme(rdwr_ret_buf, c_ret_buf_len, parsed_lines, parsed_line_cnt);
}


