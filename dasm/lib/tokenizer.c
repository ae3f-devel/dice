#include "tokenizer.h"
#include <string.h>
#include <stdlib.h>
#include <ae2f/c90/StdBool.h>
#include <assert.h>
#include <dasm.h>

enum e_tokenizer_state {
	TOKENIZER_STATE_IDLE = 0,
	TOKENIZER_STATE_IDENT,
	TOKENIZER_STATE_NUMBER,
	TOKENIZER_STATE_STRING,
	TOKENIZER_STATE_CHAR,
	TOKENIZER_STATE_OPERATOR
};

static bool is_alphabet(const char c_c)
{
	return (c_c>='a' && c_c<='z') || (c_c>='A' && c_c<='Z');
}

static bool is_number(const char c_c)
{
	return c_c>='0' && c_c<='9';
}

static void libdasm_init_token_line(struct libdasm_token_line rdwr_tokens[])
{
	rdwr_tokens->m_token_cnt = 0;
	memset(rdwr_tokens->m_tokens, 0, sizeof(struct libdasm_token) * LIBDASM_TOKEN_MAX_CNT_PER_LINE);
}

static bool libdasm_create_new_token(struct libdasm_token_line rdwr_tokens[])
{
	if (rdwr_tokens->m_token_cnt == LIBDASM_TOKEN_MAX_CNT_PER_LINE) {
		return false;
	}

	memset(rdwr_tokens->m_tokens[rdwr_tokens->m_token_cnt].m_text, 0, LIBDASM_TOKEN_MAX_LEN);
	rdwr_tokens->m_token_cnt++;

	return true;
}

/* Append c_c behind last token's last character. Doesn't create new token */
static inline bool libdasm_insert_token_char(struct libdasm_token_line rdwr_tokens[], const char c_c)
{
	libdice_word_t i = 0;
	char *token = rdwr_tokens->m_tokens[rdwr_tokens->m_token_cnt-1].m_text;	/* last token */

	if (!rdwr_tokens->m_token_cnt) {
		return false;
	}

	for (i=0; i<LIBDASM_TOKEN_MAX_LEN; ++i) {
		if (token[i] == '\0') {
			/* Boundary check */
			if (i+1 >= LIBDASM_TOKEN_MAX_LEN) {
				return false;
			}
			token[i] = c_c;
			token[i+1] = '\0';

			return true;
		}
	}

	return false;
}

/* Set last token's token type. Doesn't create new token */
static inline bool libdasm_set_token_type(struct libdasm_token_line rdwr_tokens[], const enum LIBDASM_TOKEN_TYPE_ c_token_type)
{
	if (!rdwr_tokens->m_token_cnt) {
		return false;
	}

	rdwr_tokens->m_tokens[rdwr_tokens->m_token_cnt-1].m_token_type = c_token_type;
	return true;
}

static libdice_word_t libdasm_tokenize_line(struct libdasm_token_line rdwr_tokens[], const char rd_src[], const libdice_word_t c_src_len)
{
	libdice_word_t src_cnt = 0;
	enum e_tokenizer_state state = TOKENIZER_STATE_IDLE;
	libdice_word_t char_cnt = 0;

	libdasm_init_token_line(rdwr_tokens);
	
	for (src_cnt=0; src_cnt < c_src_len;) {
		const char c = rd_src[src_cnt];
		switch (state)
		{
			case TOKENIZER_STATE_IDLE:
				if (c==' ') {
					src_cnt++;
				} else if (c=='\n') {
					libdasm_create_new_token(rdwr_tokens);
					libdasm_insert_token_char(rdwr_tokens, c);
					libdasm_set_token_type(rdwr_tokens, LIBDASM_TOKEN_TYPE_EOL);
					src_cnt++;
					return src_cnt;
				} else if (c=='*') {
					libdasm_create_new_token(rdwr_tokens);
					libdasm_insert_token_char(rdwr_tokens, c);
					libdasm_set_token_type(rdwr_tokens, LIBDASM_TOKEN_TYPE_OPERATOR);
					state = TOKENIZER_STATE_OPERATOR;
					src_cnt++;
				} else if (is_alphabet(c) || c=='_') {
					libdasm_create_new_token(rdwr_tokens);
					libdasm_insert_token_char(rdwr_tokens, c);
					libdasm_set_token_type(rdwr_tokens, LIBDASM_TOKEN_TYPE_IDENT);
					state = TOKENIZER_STATE_IDENT;
					src_cnt++;
				} else if (c=='\"') {
					libdasm_create_new_token(rdwr_tokens);
					libdasm_insert_token_char(rdwr_tokens, c);
					libdasm_set_token_type(rdwr_tokens, LIBDASM_TOKEN_TYPE_STRING);
					state = TOKENIZER_STATE_STRING;
					src_cnt++;
				} else if (c=='\'') {
					/* TODO : Don't copy single quote */
					libdasm_create_new_token(rdwr_tokens);
					libdasm_insert_token_char(rdwr_tokens, c);
					libdasm_set_token_type(rdwr_tokens, LIBDASM_TOKEN_TYPE_CHAR);
					state = TOKENIZER_STATE_CHAR;
					char_cnt = 0;
					src_cnt++;
				} else if (c=='\0') {
					libdasm_create_new_token(rdwr_tokens);
					libdasm_insert_token_char(rdwr_tokens, c);
					libdasm_set_token_type(rdwr_tokens, LIBDASM_TOKEN_TYPE_EOP);
					/* The count was intentionally not incremented. */
					return src_cnt;
				} else if (is_number(c) || c=='-') {
					libdasm_create_new_token(rdwr_tokens);
					libdasm_insert_token_char(rdwr_tokens, c);
					libdasm_set_token_type(rdwr_tokens, LIBDASM_TOKEN_TYPE_NUMBER);
					state = TOKENIZER_STATE_NUMBER;
					src_cnt++;
				} else {
					assert(0);
				}
				break;
			case TOKENIZER_STATE_IDENT:
				if (is_alphabet(c) || is_number(c) || c=='_') {
					libdasm_insert_token_char(rdwr_tokens, c);
					src_cnt++;
				} else if (c==':') {
					libdasm_set_token_type(rdwr_tokens, LIBDASM_TOKEN_TYPE_LABEL);
					state = TOKENIZER_STATE_IDLE;
					src_cnt++;
				} else {
					state = TOKENIZER_STATE_IDLE;
				}
				break;
			case TOKENIZER_STATE_NUMBER:
				if (is_number(c)) {
					libdasm_insert_token_char(rdwr_tokens, c);
					src_cnt++;
				} else {
					state = TOKENIZER_STATE_IDLE;
				}
				break;
			case TOKENIZER_STATE_STRING:
				assert(c!='\n' && c!='\0');
				/* TODO : escape sequence handling  */
				if (c=='\"') {
					libdasm_insert_token_char(rdwr_tokens, c);
					src_cnt++;
					state = TOKENIZER_STATE_IDLE;
				} else {
					libdasm_insert_token_char(rdwr_tokens, c);
					src_cnt++;
				}
				break;
			case TOKENIZER_STATE_CHAR:
				/* TODO : Don't copy single quote */
				if (char_cnt==1 && c=='\'') {
					libdasm_insert_token_char(rdwr_tokens, c);
					src_cnt++;
					state = TOKENIZER_STATE_IDLE;
				} else if (char_cnt == 0 && (unsigned char)c < 0x80) {
					libdasm_insert_token_char(rdwr_tokens, c);
					src_cnt++;
					char_cnt++;
				} else {
					return LIBDASM_ERR_RET;		/* not 1 character or Non ascii */
				}
				break;
			case TOKENIZER_STATE_OPERATOR:
				if (c=='*') {
					libdasm_insert_token_char(rdwr_tokens, c);
					src_cnt++;
				} else {
					state = TOKENIZER_STATE_IDLE;
				}
				break;
			default:
				assert(0);
				break;
		}
		
		
	}
	return src_cnt;
}


/**
 * @brief Reports the size occupied by the program in words.
 * * */
libdice_word_t libdasm_get_token_line_word_len(const struct libdasm_token_line *rd_token_line)
{
	libdice_word_t i = 0;
	libdice_word_t word_len = 0;
	
	for (i=0; i<rd_token_line->m_token_cnt; i++) {
		const enum LIBDASM_TOKEN_TYPE_ token_type = rd_token_line->m_tokens[i].m_token_type;
		if (token_type == LIBDASM_TOKEN_TYPE_LABEL || token_type == LIBDASM_TOKEN_TYPE_EOL || token_type == LIBDASM_TOKEN_TYPE_EOP) {
			continue;
		}
		word_len++;
	}
	return word_len;
}

libdice_word_t libdasm_tokenize_programme(struct libdasm_token_line rdwr_token_lines[], const libdice_word_t c_token_lines_len, const char rd_src[], const libdice_word_t c_src_len)
{
	libdice_word_t src_cnt = 0;
	libdice_word_t token_line_cnt = 0;

	while (src_cnt < c_src_len) {
		libdice_word_t tmp_read_cnt = 0;
		if (token_line_cnt + 1 > c_token_lines_len) {
			return LIBDASM_ERR_RET;
		}
		tmp_read_cnt = libdasm_tokenize_line(&rdwr_token_lines[token_line_cnt], rd_src+src_cnt, c_src_len-src_cnt);
		if (tmp_read_cnt == LIBDASM_ERR_RET) {
			return LIBDASM_ERR_RET;
		}
		src_cnt += tmp_read_cnt;
		token_line_cnt++;
	} 

	return token_line_cnt;
}