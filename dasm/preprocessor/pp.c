#include "pp.h"
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


enum e_libdasm_pp_state {
	LIBDASM_PP_STATE_NORMAL = 0,
	LIBDASM_PP_STATE_CHAR,
	LIBDASM_PP_STATE_STRING,
	LIBDASM_PP_STATE_LINE_COMMENT,
	LIBDASM_PP_STATE_BLOCK_COMMENT
};



static void libdasm_update_pp_state(enum e_libdasm_pp_state *state, char c)
{

	switch (*state)
	{
		case LIBDASM_PP_STATE_NORMAL:
		{	
		switch (c)
		{
		case PP_CHAR_START:
		{
			*state = LIBDASM_PP_STATE_CHAR;
			break;
		}
		case PP_STRING_START:
		{
			*state = LIBDASM_PP_STATE_STRING;
			break;
		}
		case PP_LINE_COMMENT_START:
		{
			*state = LIBDASM_PP_STATE_LINE_COMMENT;
			break;
		}
		case PP_BLOCK_COMMENT_START:
		{
			*state = LIBDASM_PP_STATE_BLOCK_COMMENT;
			break;
		}
		default:
		{	
			/* Keep current state */
			break;
		}
		}
		break;
	}
	case LIBDASM_PP_STATE_CHAR:
	{
		if (c == PP_CHAR_END) {
			*state = LIBDASM_PP_STATE_NORMAL;
		}
		break;
	}
	case LIBDASM_PP_STATE_STRING:
	{
		if (c == PP_STRING_END) {
			*state = LIBDASM_PP_STATE_NORMAL;
		}
		break;
	}
	case LIBDASM_PP_STATE_LINE_COMMENT:
	{
		if (c == PP_LINE_COMMENT_END) {
			*state = LIBDASM_PP_STATE_NORMAL;
		}
		break;
	}
	case LIBDASM_PP_STATE_BLOCK_COMMENT:
	{
		if (c == PP_BLOCK_COMMENT_END) {
			*state = LIBDASM_PP_STATE_NORMAL;
		}
		break;
	}
	default:
	{
		assert(0);
		return;
	}
	}
}

static libdice_word_t libdasm_remove_comment(char *dst, const libdice_word_t c_dst_len, const char *c_rd_src_line, enum e_libdasm_pp_state *state, libdice_word_t *read_len)
{
	libdice_word_t dst_cnt = 0;
	char c = 0;
	bool emit = true;
	libdice_word_t i = 0;

	for (i=0; ; i++) {
		c = c_rd_src_line[i];
		emit = true;

		switch (*state)
		{
		case LIBDASM_PP_STATE_LINE_COMMENT:
		{
			if (c != '\n') {
				emit = false;
			}
			break;
		}
		case LIBDASM_PP_STATE_BLOCK_COMMENT:
		{
			emit = false;
			break;
		}
		case LIBDASM_PP_STATE_NORMAL:
		{
			if (c == PP_LINE_COMMENT_START || c == PP_BLOCK_COMMENT_START) {
				emit = false;		
			}
			break;
		}
		case LIBDASM_PP_STATE_STRING:	/*intentionally fallthrough*/
		case LIBDASM_PP_STATE_CHAR:	/*intentionally fallthrough*/
		default:
			break;
		}

		if (emit) {
			assert(dst_cnt+1 < c_dst_len);
			dst[dst_cnt] = c;
			dst_cnt++;
		}
		libdasm_update_pp_state(state, c);
		
		if (c == '\n' || c=='\0') {
			break;
		}

	}

	*read_len = i;

	return dst_cnt;
}



static libdice_word_t libdasm_normalize_line(char *rdwr_dst, const libdice_word_t c_dst_len, const char *c_rd_src_line, libdice_word_t *read_len)
{
	libdice_word_t dst_cnt = 0;
	libdice_word_t i = 0;
	bool was_prev_delimiter = false;
	bool has_non_whitespace = false;	

	char c = 0;
	for (i=0; ;i++) {
		c = c_rd_src_line[i];
		if (c == '\r') {
			continue;
		}
		if (c=='\t') {
			c = ' ';
		}
		if (c==' ') {
			if (was_prev_delimiter) {
				continue;
			} else {
				was_prev_delimiter = true;;
			}
		} else {
			was_prev_delimiter = false;
		}
		
		if (c!=' ' && c!='\n') {
			has_non_whitespace = true;
		}

		assert(dst_cnt+1 < c_dst_len);
		rdwr_dst[dst_cnt] = c;
		dst_cnt++;

		if (c == '\n' || c=='\0') {
			break;
		}
	}
	*read_len = i;

	if (!has_non_whitespace) {
		rdwr_dst[0] = '\n';
		dst_cnt = 1;
	}

	return dst_cnt;
}

static libdice_word_t libdasm_remove_comments(char *rdwr_dst, const libdice_word_t c_dst_len, const char *rd_src)
{
	libdice_word_t tmp_read_len = 0;
	libdice_word_t tmp_write_len = 0;
	libdice_word_t pc = 0;
	libdice_word_t dst_cnt = 0;
	const size_t rd_src_len = strlen(rd_src)+1;
	enum e_libdasm_pp_state state = LIBDASM_PP_STATE_NORMAL;

	while (pc < rd_src_len) {
		tmp_read_len = 0;
		tmp_write_len = 0;

		tmp_write_len = libdasm_remove_comment(rdwr_dst+dst_cnt, c_dst_len-dst_cnt, rd_src+pc, &state, &tmp_read_len);
		assert(dst_cnt + tmp_write_len <= c_dst_len);
		assert(tmp_read_len > 0);
		dst_cnt += tmp_write_len;
		pc += tmp_read_len;
	}

	return dst_cnt;

}

static libdice_word_t libdasm_normalize_lines(char *rdwr_dst, const libdice_word_t c_dst_len, const char *rd_src)
{
	libdice_word_t tmp_read_len = 0;
	libdice_word_t tmp_write_len = 0;
	libdice_word_t pc = 0;
	libdice_word_t dst_cnt = 0;
	const size_t c_src_len = strlen(rd_src)+1;

	while (pc < c_src_len) {
		tmp_read_len = 0;
		tmp_write_len = 0;

		
		tmp_write_len = libdasm_normalize_line(rdwr_dst+dst_cnt, c_dst_len-dst_cnt, rd_src+pc, &tmp_read_len);
		assert(dst_cnt + tmp_write_len <= c_dst_len);
		assert(tmp_read_len > 0);
		dst_cnt += tmp_write_len;
		pc += tmp_read_len;
	}

	return dst_cnt;
}

libdice_word_t libdasm_preprocess_programme(char *dst, const libdice_word_t c_dst_len, const char *rd_src)
{
	char *tmp_buf[2000] = {0,};
	libdice_word_t tmp_write_cnt = 0;

	libdasm_remove_comments(tmp_buf, 2000, rd_src);
	return libdasm_normalize_lines(dst, c_dst_len, tmp_buf);
}