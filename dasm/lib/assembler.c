#include "./assembler.h"
#include <stdlib.h>
#include <assert.h>


static enum DASM_ASM_ERR_ dasm_assemble_line(libdice_word_t rdwr_dst[], const libdice_word_t c_dst_len, const struct dasm_parsed_line *rd_parsed_line, libdice_word_t *rdwr_write_cnt)
{
	libdice_word_t tmp_operand_cnt = 0;
	long number = 0;
	char *end;

	if (c_dst_len < rd_parsed_line->m_operand_cnt + 1) {
		return DASM_ASM_ERR_MEM_INSUF;
	}

	rdwr_dst[0] = (libdice_word_t)rd_parsed_line->m_opcode;

	for (tmp_operand_cnt=0; tmp_operand_cnt<rd_parsed_line->m_operand_cnt; tmp_operand_cnt++) {
		number = strtol(rd_parsed_line->m_operands[tmp_operand_cnt].m_text, &end, 10);
		
		
		if (end == rd_parsed_line->m_operands[tmp_operand_cnt].m_text && *end != '\0') {
			*rdwr_write_cnt = 1 + tmp_operand_cnt;
			return DASM_ASM_ERR_INVAL_ARG;	/* not number */
		}
		rdwr_dst[1 + tmp_operand_cnt] = (libdice_word_t)number;
	}

	/* word cnt */
	*rdwr_write_cnt = 1 + tmp_operand_cnt;
	return DASM_ASM_ERR_OK;
}

DICEIMPL struct dasm_asm_ret dasm_assemble_programme(libdice_word_t rdwr_programme[], const libdice_word_t c_programme_len, 
					const struct dasm_parsed_line rd_parsed_lines[], const libdice_word_t c_parsed_lines_len,
					libdice_word_t *rdwr_write_cnt)
{
	libdice_word_t line_cnt = 0;
	libdice_word_t parsed_line_cnt = 0;
	libdice_word_t programme_word_cnt = 0;
	struct dasm_asm_ret ret;

	for (parsed_line_cnt=0; parsed_line_cnt<c_parsed_lines_len;) {
		libdice_word_t tmp_write_cnt = 0;
		ret.m_err = dasm_assemble_line(&rdwr_programme[programme_word_cnt], c_programme_len-programme_word_cnt, &rd_parsed_lines[parsed_line_cnt], &tmp_write_cnt);
	
		parsed_line_cnt++;
		programme_word_cnt += tmp_write_cnt;
		line_cnt++;

		if (ret.m_err != DASM_ASM_ERR_OK) {
			break;
		}
	}

	*rdwr_write_cnt = programme_word_cnt;
	ret.m_line_cnt = line_cnt;

	return ret;
}
