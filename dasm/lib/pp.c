#include "pp.h"
#include <dasm.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ae2f/c90/StdBool.h>

#define PP_CHAR_START '\''
#define PP_CHAR_END '\''
#define PP_STRING_START '\"'
#define PP_STRING_END '\"'
#define PP_LINE_COMMENT_START ';'
#define PP_LINE_COMMENT_END '\n'
#define PP_BLOCK_COMMENT_START '{'
#define PP_BLOCK_COMMENT_END '}'


enum LIBDASM_PP_STATE_ {
	LIBDASM_PP_STATE_NORMAL = 0,
	LIBDASM_PP_STATE_CHAR,
	LIBDASM_PP_STATE_STRING,
	LIBDASM_PP_STATE_LINE_COMMENT,
	LIBDASM_PP_STATE_BLOCK_COMMENT
};



static enum LIBDASM_PP_STATE_ libdasm_update_pp_state(enum LIBDASM_PP_STATE_ state, const char c_c)
{

	switch (state)
	{
		case LIBDASM_PP_STATE_NORMAL:
			switch (c_c)
			{
				case PP_CHAR_START:
					state = LIBDASM_PP_STATE_CHAR;
					break;
				case PP_STRING_START:
					state = LIBDASM_PP_STATE_STRING;
					break;
				case PP_LINE_COMMENT_START:
					state = LIBDASM_PP_STATE_LINE_COMMENT;
					break;
				case PP_BLOCK_COMMENT_START:
					state = LIBDASM_PP_STATE_BLOCK_COMMENT;
					break;
				default:
					break;	/* Keep current state */
			}
			break;
	
		case LIBDASM_PP_STATE_CHAR:
			if (c_c == PP_CHAR_END) {
				state = LIBDASM_PP_STATE_NORMAL;
			}
			break;
		case LIBDASM_PP_STATE_STRING:
			if (c_c == PP_STRING_END) {
				state = LIBDASM_PP_STATE_NORMAL;
			}
			break;
		case LIBDASM_PP_STATE_LINE_COMMENT:
			if (c_c == PP_LINE_COMMENT_END) {
				state = LIBDASM_PP_STATE_NORMAL;
			}
			break;
		case LIBDASM_PP_STATE_BLOCK_COMMENT:
			if (c_c == PP_BLOCK_COMMENT_END) {
				state = LIBDASM_PP_STATE_NORMAL;
			}
			break;
		default:
			assert(0);
			break;
	}

	return state;
}


static libdice_word_t libdasm_remove_comment_from_line(char rdwr_dst[], const libdice_word_t c_dst_len, 
						const char rd_src[], const libdice_word_t c_src_len, 
						enum LIBDASM_PP_STATE_ *rdwr_state, libdice_word_t *rdwr_read_cnt)
{
	libdice_word_t write_cnt = 0;
	libdice_word_t read_cnt = 0;

	for (read_cnt=0; read_cnt<c_src_len && write_cnt<c_dst_len; ++read_cnt) {
		const char c = rd_src[read_cnt];
		bool emit = true;

		switch (*rdwr_state)
		{
			case LIBDASM_PP_STATE_LINE_COMMENT:
				if (c != '\n') {
					emit = false;
				}
				break;
			case LIBDASM_PP_STATE_BLOCK_COMMENT:
				emit = false;
				break;
			case LIBDASM_PP_STATE_NORMAL:
				if (c == PP_LINE_COMMENT_START || c == PP_BLOCK_COMMENT_START) {
					emit = false;		
				}
				break;
			case LIBDASM_PP_STATE_STRING:	/*intentionally fallthrough*/
			case LIBDASM_PP_STATE_CHAR:	/*intentionally fallthrough*/
			default:
				break;
		}

		if (c=='\0') {
			break;
		}

		if (emit) {
			rdwr_dst[write_cnt] = c;
			write_cnt++;
		}
		*rdwr_state = libdasm_update_pp_state(*rdwr_state, c);
		
		if (c=='\n') {
			++read_cnt;
			break;
		}

	}

	*rdwr_read_cnt = read_cnt;

	return write_cnt;
}


/**
 * @brief remove useless whitespace. keep line number
 */
static libdice_word_t libdasm_normalize_line(char rdwr_dst[], const libdice_word_t c_dst_len, 
					const char rd_src[], const libdice_word_t c_src_len,
					libdice_word_t *rdwr_read_cnt)
{
	libdice_word_t write_cnt = 0;
	libdice_word_t read_cnt = 0;
	bool was_prev_delimiter = false;
	bool has_non_whitespace = false;	

	for (read_cnt=0; read_cnt<c_src_len; ++read_cnt) {
		char c = rd_src[read_cnt];
		switch (c) 
		{
			case '\r':
				continue;
			case '\t':
				c = ' ';
				break;
			default:
				break;
		}
	
		if (c==' ') {
			if (was_prev_delimiter) {
				continue;
			} else {
				was_prev_delimiter = true;;
			}
			
		} else {
			was_prev_delimiter = false;
			if (c!='\n') {
				/* c!=' ' && c!='\n' */
				has_non_whitespace = true;	
			}
		}
		
		if (c=='\0') {
			/* break before copy */
			break;
		}

		if (write_cnt+1 >= c_dst_len) {
			return LIBDASM_ERR_RET;
		}

		rdwr_dst[write_cnt] = c;
		write_cnt++;

		if (c == '\n') {
			/* break after copy */
			read_cnt++;
			break;
		}
	}
	*rdwr_read_cnt = read_cnt;

	if (!has_non_whitespace) {
		/* current line that we handling does't have 'non whitespace' character */
		/* Keep '\n' for error message*/
		rdwr_dst[0] = '\n';
		write_cnt = 1;
	}

	return write_cnt;
}

static libdice_word_t libdasm_remove_comments(char rdwr_dst[], const libdice_word_t c_dst_len, const char rd_src[], const libdice_word_t c_src_len)
{
	libdice_word_t real_src_len;
	libdice_word_t read_cnt = 0;
	libdice_word_t write_cnt = 0;
	enum LIBDASM_PP_STATE_ state = LIBDASM_PP_STATE_NORMAL;

	real_src_len = (libdice_word_t)strlen(rd_src);

	while (read_cnt < c_src_len) {
		libdice_word_t tmp_read_cnt = 0;
		libdice_word_t tmp_write_cnt = 0;

		tmp_write_cnt = libdasm_remove_comment_from_line(rdwr_dst+write_cnt, c_dst_len-write_cnt, rd_src+read_cnt, c_src_len-read_cnt, &state, &tmp_read_cnt);
		if (tmp_write_cnt == LIBDASM_ERR_RET) {
			return LIBDASM_ERR_RET;
		}
		if (write_cnt + tmp_write_cnt > c_dst_len) {
			return LIBDASM_ERR_RET;
		}
		
		assert(tmp_read_cnt > 0);
		write_cnt += tmp_write_cnt;
		read_cnt += tmp_read_cnt;

		if (read_cnt >= real_src_len) {
			break;
		}
	}

	if (write_cnt+1 > c_dst_len) {
		return LIBDASM_ERR_RET;
	}
	
	rdwr_dst[write_cnt] = '\0';	/* The count was intentionally not incremented.  */
	write_cnt++;

	return write_cnt;
}

static libdice_word_t libdasm_normalize_lines(char rdwr_dst[], const libdice_word_t c_dst_len, const char rd_src[], const libdice_word_t c_src_len)
{
	libdice_word_t read_cnt = 0;
	libdice_word_t write_cnt = 0;
	libdice_word_t real_src_len;
	real_src_len = (libdice_word_t)strlen(rd_src);

	while (read_cnt < c_src_len) {
		libdice_word_t tmp_read_cnt = 0;
		libdice_word_t tmp_write_cnt = 0;

		tmp_write_cnt = libdasm_normalize_line(rdwr_dst+write_cnt, c_dst_len-write_cnt, rd_src+read_cnt, c_src_len-read_cnt, &tmp_read_cnt);
		if (tmp_write_cnt == LIBDASM_ERR_RET) {
			return LIBDASM_ERR_RET;
		}
		if (write_cnt + tmp_write_cnt > c_dst_len) {
			return LIBDASM_ERR_RET;
		}

		assert(tmp_read_cnt > 0);
		write_cnt += tmp_write_cnt;
		read_cnt += tmp_read_cnt;
	
		if (read_cnt >= real_src_len) {
			break;
		}
	}

	if (write_cnt+1 > c_dst_len) {
		return LIBDASM_ERR_RET;
	}
	
	rdwr_dst[write_cnt] = '\0';	/* The count was intentionally not incremented.  */
	write_cnt++;

	return write_cnt;
}

libdice_word_t libdasm_preprocess_programme(char rdwr_dst[], const libdice_word_t c_dst_len, const char rd_src[], const libdice_word_t c_src_len)
{
	char buf[LIBDASM_PROGRAMME_MAX_LEN] = {0,};
	libdice_word_t buf_cnt = 0;

	buf_cnt = libdasm_remove_comments(buf, LIBDASM_PROGRAMME_MAX_LEN, rd_src, c_src_len);
	if (buf_cnt == LIBDASM_ERR_RET) {
		return LIBDASM_ERR_RET;
	}
	return libdasm_normalize_lines(rdwr_dst, c_dst_len, buf, c_src_len);
}