#include "assembler.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>


static libdice_word_t libdasm_assemble_line(libdice_word_t rdwr_dst[], const libdice_word_t c_dst_len, const struct libdasm_parsed_line *rd_parsed_line)
{
	libdice_word_t tmp_operand_cnt = 0;
	long number = 0;
	char *end;

	if (c_dst_len < rd_parsed_line->m_operand_cnt + 1) {
		return LIBDASM_ERR_RET;
	}

	rdwr_dst[0] = rd_parsed_line->m_opcode;

	for (tmp_operand_cnt=0; tmp_operand_cnt<rd_parsed_line->m_operand_cnt; ++tmp_operand_cnt) {
		number = strtol(rd_parsed_line->m_operands[tmp_operand_cnt].m_text, &end, 10);
		
		if (end == rd_parsed_line->m_operands[tmp_operand_cnt].m_text) {
			return LIBDASM_ERR_RET;	/* not number */
		} else if (*end != '\0') {
			return LIBDASM_ERR_RET;	/* trash after number */
		}
		rdwr_dst[1+tmp_operand_cnt] = (libdice_word_t)number;
	}

	/* word cnt */
	return 1+tmp_operand_cnt;
}












libdice_word_t libdasm_assemble_programme(libdice_word_t rdwr_programme[], const libdice_word_t c_programme_len, 
					const struct libdasm_parsed_line rd_parsed_lines[], const libdice_word_t c_parsed_lines_len)
{
	libdice_word_t parsed_line_cnt = 0;
	libdice_word_t tmp = 0;
	libdice_word_t programme_word_cnt = 0;

	for (parsed_line_cnt=0; parsed_line_cnt<c_parsed_lines_len; ++parsed_line_cnt) {
		tmp = libdasm_assemble_line(&rdwr_programme[programme_word_cnt], c_programme_len-programme_word_cnt, &rd_parsed_lines[parsed_line_cnt]);
		if (programme_word_cnt+tmp > c_programme_len) {
			return LIBDASM_ERR_RET;
		}
		programme_word_cnt += tmp;
	}

	return programme_word_cnt;
}