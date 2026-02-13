#include "./pp.h"
#include <string.h>
#include <assert.h>
#include <ae2f/c90/StdBool.h>
#include <stdio.h>

#define PP_LINE_COMMENT_START ';'
#define PP_LINE_COMMENT_END '\n'
#define PP_BLOCK_COMMENT_START '{'
#define PP_BLOCK_COMMENT_END '}'


enum DASM_PP_STATE_ {	
	DASM_PP_STATE_NORMAL,
	DASM_PP_STATE_LINE_COMMENT,
	DASM_PP_STATE_BLOCK_COMMENT
};

static enum DASM_PP_ERR_ dasm_remove_comment_from_line(char rdwr_dst[], const libdice_word_t c_dst_len, 
		const char rd_src[], const libdice_word_t c_src_len, 
		enum DASM_PP_STATE_ *rdwr_state, 
		libdice_word_t *rdwr_write_cnt, libdice_word_t *rdwr_read_cnt)
{
	libdice_word_t write_cnt = 0;
	libdice_word_t read_cnt = 0;

	for (read_cnt=0; read_cnt<c_src_len && write_cnt<c_dst_len;) {
		const char c = rd_src[read_cnt];
		switch (*rdwr_state)
		{
			case DASM_PP_STATE_NORMAL:
				read_cnt++;
				switch (c)
				{
					case PP_LINE_COMMENT_START:
						*rdwr_state =  DASM_PP_STATE_LINE_COMMENT;
						break;
					case PP_BLOCK_COMMENT_START:
						*rdwr_state = DASM_PP_STATE_BLOCK_COMMENT;
						break;
					case '\n':
					case '\0':
						rdwr_dst[write_cnt++] = c;

						*rdwr_read_cnt = read_cnt;
						*rdwr_write_cnt = write_cnt;

						return DASM_PP_ERR_OK;

					default:
						rdwr_dst[write_cnt++] = c;
						break;
				}
				break;
			case DASM_PP_STATE_LINE_COMMENT:
				read_cnt++;
				if (c == PP_LINE_COMMENT_END) {
					*rdwr_state = DASM_PP_STATE_NORMAL;
				}
				break;
			case DASM_PP_STATE_BLOCK_COMMENT:
				read_cnt++;
				if (c == PP_BLOCK_COMMENT_END) {
					*rdwr_state = DASM_PP_STATE_NORMAL;
				} else if (c == '\0') {
					rdwr_dst[write_cnt++] = c;

					*rdwr_read_cnt = read_cnt;
					*rdwr_write_cnt = write_cnt;
					return DASM_PP_ERR_UNCLOSED_BLOCK_COMMENT;
				}
				break;
			default:
				assert(0);
				return DASM_PP_ERR_UNKNOWN;
		}
	}

	*rdwr_read_cnt = read_cnt;
	*rdwr_write_cnt = write_cnt;

	/** Higher Priority than DASM_PP_ERR_NO_TERM */
	if (write_cnt == c_dst_len) {
		return DASM_PP_ERR_MEM_INSUF;
	}
	if (read_cnt == c_src_len) {
		return DASM_PP_ERR_NO_TERM;
	}

	return DASM_PP_ERR_UNKNOWN;
}


/**
 * @brief Remove useless whitespace. Keep line number
 */
static enum DASM_PP_ERR_ dasm_normalize_line(char rdwr_dst[], const libdice_word_t c_dst_len, 
		const char rd_src[], const libdice_word_t c_src_len,
		libdice_word_t *rdwr_write_cnt, libdice_word_t *rdwr_read_cnt)
{
	libdice_word_t write_cnt = 0;
	libdice_word_t read_cnt = 0;

	char	prev_c = 0;
	char	c;

	for (read_cnt=0; read_cnt<c_src_len && write_cnt<c_dst_len; ++read_cnt) {
		c = rd_src[read_cnt];

		switch (c)
		{
			case '\r':
				continue;
			case '\t':
				c = ' ';
				ae2f_fallthrough;
			case ' ':
				if(prev_c == ' ') {
					break;
				}
				ae2f_fallthrough;
			default:
				prev_c = c;
				rdwr_dst[write_cnt++] = c;
				break;

		}

		if (c == '\n' || c == '\0') {
			*rdwr_write_cnt = write_cnt;
			*rdwr_read_cnt = read_cnt;

			return DASM_PP_ERR_OK;
		}
	}

	*rdwr_write_cnt = write_cnt;
	*rdwr_read_cnt = read_cnt;

	/** Higher Priority than DASM_PP_ERR_NO_TERM */
	if (write_cnt == c_dst_len) {
		return DASM_PP_ERR_MEM_INSUF;
	}
	if (read_cnt == c_src_len) {
		return DASM_PP_ERR_NO_TERM;
	}

	return DASM_PP_ERR_UNKNOWN;
}

static struct dasm_pp_ret dasm_remove_comments(char rdwr_dst[], const libdice_word_t c_dst_len,
		const char rd_src[], const libdice_word_t c_src_len,
		libdice_word_t *rdwr_write_cnt, libdice_word_t *rdwr_read_cnt)
{
	libdice_word_t real_src_len;

	libdice_word_t read_cnt = 0;
	libdice_word_t write_cnt = 0;
	libdice_word_t line_cnt = 0;

	enum DASM_PP_STATE_ state = DASM_PP_STATE_NORMAL;
	struct dasm_pp_ret ret;

	real_src_len = (libdice_word_t)strlen(rd_src) + 1;		/** +1 for nul-terminator*/

	if (real_src_len > c_src_len) {
		real_src_len = c_src_len;
	}

	while (read_cnt < real_src_len && write_cnt < c_dst_len) {
		libdice_word_t tmp_read_cnt = 0;
		libdice_word_t tmp_write_cnt = 0;
		
		ret.err = dasm_remove_comment_from_line(
				rdwr_dst + write_cnt
				, c_dst_len - write_cnt
				, rd_src + read_cnt
				, real_src_len - read_cnt
				, &state
				, &tmp_write_cnt
				, &tmp_read_cnt
				);

		write_cnt += tmp_write_cnt;
		read_cnt += tmp_read_cnt;
		line_cnt++;

		if (ret.err != DASM_PP_ERR_OK) {
			break;	
		}
	}

	*rdwr_write_cnt = write_cnt;
	*rdwr_read_cnt = read_cnt;
	ret.line_cnt = line_cnt;

	return ret;
}

static struct dasm_pp_ret dasm_normalize_lines(char rdwr_dst[], const libdice_word_t c_dst_len,
		const char rd_src[], const libdice_word_t c_src_len,
		libdice_word_t *rdwr_write_cnt, libdice_word_t *rdwr_read_cnt)
{
	libdice_word_t real_src_len;

	libdice_word_t read_cnt = 0;
	libdice_word_t write_cnt = 0;
	libdice_word_t line_cnt = 0;



	struct dasm_pp_ret ret;

	real_src_len = (libdice_word_t)strlen(rd_src) + 1;		/** +1 for nul-terminator*/

	if (real_src_len > c_src_len) {
		real_src_len = c_src_len;
	}

	while (read_cnt < real_src_len && write_cnt < c_dst_len) {

		libdice_word_t tmp_read_cnt = 0;
		libdice_word_t tmp_write_cnt = 0;
		ret.err = dasm_normalize_line(rdwr_dst + write_cnt, c_dst_len - write_cnt
				, rd_src + read_cnt, real_src_len - read_cnt
				, &tmp_write_cnt, &tmp_read_cnt);

		write_cnt += tmp_write_cnt;
		read_cnt += tmp_read_cnt;
		line_cnt++;

		if (ret.err != DASM_PP_ERR_OK) {
			break;	
		}
	}

	*rdwr_write_cnt = write_cnt;
	*rdwr_read_cnt = read_cnt;
	ret.line_cnt = line_cnt;

	return ret;
}

DICEIMPL struct dasm_pp_ret dasm_preprocess_programme(char rdwr_dst[], const libdice_word_t c_dst_len,
		const char rd_src[], const libdice_word_t c_src_len,
		libdice_word_t *rdwr_write_cnt)
{
	char buf[DASM_PROGRAMME_MAX_LEN] = {0,};
	libdice_word_t tmp = 0;
	libdice_word_t buf_cnt = 0;
	struct dasm_pp_ret ret;

	ret = dasm_remove_comments(buf, DASM_PROGRAMME_MAX_LEN, rd_src, c_src_len,
					&buf_cnt, &tmp);
	if (ret.err != DASM_PP_ERR_OK) {
		return ret;
	}
	

	ret = dasm_normalize_lines(rdwr_dst, c_dst_len, buf, buf_cnt,
					rdwr_write_cnt, &tmp);

	printf("\n\n%u\n\n", tmp);
	return ret;
}
