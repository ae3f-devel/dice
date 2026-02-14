#include "./tokenizer.h"
#include <string.h>
#include <ae2f/c90/StdBool.h>
#include <assert.h>


#include <ctype.h>
enum e_tokenizer_state {
	TOKENIZER_STATE_IDLE = 0,
	TOKENIZER_STATE_IDENT,
	TOKENIZER_STATE_NUMBER,
	TOKENIZER_STATE_STRING,
	TOKENIZER_STATE_ASCII,
	TOKENIZER_STATE_OPERATOR
};




static void dasm_init_tok_line(struct dasm_tok_line rdwr_toks[])
{
	rdwr_toks->m_tok_cnt = 0;
	memset(rdwr_toks->m_toks
			, 0, sizeof(struct dasm_tok) * DASM_TOK_MAX_CNT_PER_LINE
	      );
}

static bool dasm_create_new_tok(struct dasm_tok_line rdwr_toks[])
{
	if (rdwr_toks->m_tok_cnt == DASM_TOK_MAX_CNT_PER_LINE) {
		return false;
	}

	memset(rdwr_toks->m_toks[rdwr_toks->m_tok_cnt].m_text, 0, DASM_TOK_MAX_LEN);
	rdwr_toks->m_tok_cnt++;

	return true;
}

/* Append c_c behind last tok's last character. Doesn't create new tok */
static inline bool dasm_insert_tok_char(struct dasm_tok_line rdwr_toks[], const char c_c)
{
	libdice_word_t i = 0;
	char *tok = rdwr_toks->m_toks[rdwr_toks->m_tok_cnt-1].m_text;	/* last tok */

	if (!rdwr_toks->m_tok_cnt) {
		return false;
	}

	for (i=0; i<DASM_TOK_MAX_LEN; ++i) {
		if (tok[i] == '\0') {
			/* Boundary check */
			if (i+1 >= DASM_TOK_MAX_LEN) {
				return false;
			}
			tok[i] = c_c;
			tok[i+1] = '\0';

			return true;
		}
	}

	return false;
}

/* Set last tok's tok type. Doesn't create new tok */
static inline bool dasm_set_tok_type(struct dasm_tok_line rdwr_toks[], const enum DASM_TOK_TYPE_ c_tok_type)
{
	if (!rdwr_toks->m_tok_cnt) {
		return false;
	}

	rdwr_toks->m_toks[rdwr_toks->m_tok_cnt-1].m_tok_type = c_tok_type;
	return true;
}

static enum DASM_TOK_ERR_ dasm_tokenize_line(struct dasm_tok_line rdwr_toks[], 
		const char rd_src[], const libdice_word_t c_src_len,
		libdice_word_t *rdwr_read_cnt)
{
	libdice_word_t read_cnt = 0;
	enum e_tokenizer_state state = TOKENIZER_STATE_IDLE;
	libdice_word_t char_cnt = 0;

	dasm_init_tok_line(rdwr_toks);

	for (read_cnt=0;  read_cnt<c_src_len;) {
		const char c = rd_src[read_cnt];
		switch (state)
		{
			case TOKENIZER_STATE_IDLE:


				if (c==' ') {
					read_cnt++;
				} else if (c=='\n') {
					if (!dasm_create_new_tok(rdwr_toks)) {
						goto memory_insufficient;
					}
					if (!dasm_insert_tok_char(rdwr_toks, c)) {
						goto memory_insufficient;
					}
					dasm_set_tok_type(rdwr_toks, DASM_TOK_TYPE_EOL);
					read_cnt++;

					*rdwr_read_cnt = read_cnt; 
					return DASM_TOK_ERR_OK;
				} else if (c=='*' || c=='#') {	
					if (!dasm_create_new_tok(rdwr_toks)) {
						goto memory_insufficient;
					}
					if (!dasm_insert_tok_char(rdwr_toks, c)) {
						goto memory_insufficient;
					}
					dasm_set_tok_type(rdwr_toks, DASM_TOK_TYPE_OPERATOR);
					state = TOKENIZER_STATE_OPERATOR;
					read_cnt++;
				} else if (isalpha((unsigned char)c) || c=='_') {
					if (!dasm_create_new_tok(rdwr_toks)) {
						goto memory_insufficient;
					}
					if (!dasm_insert_tok_char(rdwr_toks, c)) {
						goto memory_insufficient;
					}
					dasm_set_tok_type(rdwr_toks, DASM_TOK_TYPE_IDENT);
					state = TOKENIZER_STATE_IDENT;
					read_cnt++;
				} else if (c=='\"') {
					if (!dasm_create_new_tok(rdwr_toks)) {
						goto memory_insufficient;
					}
					dasm_set_tok_type(rdwr_toks, DASM_TOK_TYPE_STRING);
					state = TOKENIZER_STATE_STRING;
					read_cnt++;
				} else if (c=='\'') {
					if (!dasm_create_new_tok(rdwr_toks)) {
						goto memory_insufficient;
					}
					dasm_set_tok_type(rdwr_toks, DASM_TOK_TYPE_ASCII);
					state = TOKENIZER_STATE_ASCII;
					char_cnt = 0;
					read_cnt++;
				} else if (c=='\0') {
					if (!dasm_create_new_tok(rdwr_toks)) {
						goto memory_insufficient;
					}
					if (!dasm_insert_tok_char(rdwr_toks, c)) {
						goto memory_insufficient;
					}
					dasm_set_tok_type(rdwr_toks, DASM_TOK_TYPE_EOP);
					read_cnt++;

					*rdwr_read_cnt = read_cnt;
					return DASM_TOK_ERR_OK;
				} else if (isdigit((unsigned char)c) || c=='-') {
					if (!dasm_create_new_tok(rdwr_toks)) {
						goto memory_insufficient;
					}
					if (!dasm_insert_tok_char(rdwr_toks, c)) {
						goto memory_insufficient;
					}
					dasm_set_tok_type(rdwr_toks, DASM_TOK_TYPE_NUMBER);
					state = TOKENIZER_STATE_NUMBER;
					read_cnt++;
				} else {

					*rdwr_read_cnt = read_cnt;
					return DASM_TOK_ERR_INVAL_CHAR;
				}
				break;
			case TOKENIZER_STATE_IDENT:
				if (isalpha((unsigned char)c) || isdigit((unsigned char)c) || c=='_') {
					if (!dasm_insert_tok_char(rdwr_toks, c)) {
						goto memory_insufficient;
					}
					read_cnt++;
				} else if (c==':') {
					dasm_set_tok_type(rdwr_toks, DASM_TOK_TYPE_LABEL);
					state = TOKENIZER_STATE_IDLE;
					read_cnt++;
				} else {
					state = TOKENIZER_STATE_IDLE;
				}
				break;
			case TOKENIZER_STATE_NUMBER:
				if (isdigit((unsigned char)c)) {
					if (!dasm_insert_tok_char(rdwr_toks, c)) {
						goto memory_insufficient;
					}
					read_cnt++;
				} else {
					state = TOKENIZER_STATE_IDLE;
				}
				break;
			case TOKENIZER_STATE_STRING:
				if (c == '\n' || c == '\0') {
					*rdwr_read_cnt = read_cnt;
					return DASM_TOK_ERR_INVAL_STRING;
				}
				/* TODO : escape sequence handling  */
				if (c=='\"') {
					read_cnt++;
					state = TOKENIZER_STATE_IDLE;
				} else {
					if(!dasm_insert_tok_char(rdwr_toks, c)) {
						goto memory_insufficient;
					}
					read_cnt++;
				}
				break;
			case TOKENIZER_STATE_ASCII:
				if (char_cnt==1 && c=='\'') {
					read_cnt++;
					state = TOKENIZER_STATE_IDLE;
				} else if (char_cnt == 0 && isascii((unsigned char)c)) {
					if (!dasm_insert_tok_char(rdwr_toks, c)) {
						goto memory_insufficient;
					}
					read_cnt++;
					char_cnt++;
				} else {
					*rdwr_read_cnt = read_cnt;
					return DASM_TOK_ERR_INVAL_ASCII;		/* not 1 character or Non ascii */
				}
				break;
			case TOKENIZER_STATE_OPERATOR:
				if (c=='*') {
					if (!dasm_insert_tok_char(rdwr_toks, c)) {
						goto memory_insufficient;
					}
					read_cnt++;
				} else {
					state = TOKENIZER_STATE_IDLE;
				}
				break;
			default:
				*rdwr_read_cnt = read_cnt;
				return DASM_TOK_ERR_UNKNOWN;	
		}


	}

	*rdwr_read_cnt = read_cnt;

	if (read_cnt == c_src_len) {
		return DASM_TOK_ERR_NO_TERM;
	}

	return DASM_TOK_ERR_UNKNOWN;

memory_insufficient:
	*rdwr_read_cnt = read_cnt;
	return DASM_TOK_ERR_MEM_INSUF;
}


/**
 * @brief Reports the size occupied by the program in words.
 * */
DICEIMPL libdice_word_t dasm_get_tok_line_word_len(const struct dasm_tok_line *rd_tok_line)
{
	libdice_word_t i = 0;
	libdice_word_t word_len = 0;

	for (i=0; i<rd_tok_line->m_tok_cnt; i++) {
		const enum DASM_TOK_TYPE_ tok_type = rd_tok_line->m_toks[i].m_tok_type;
		if (tok_type == DASM_TOK_TYPE_LABEL || tok_type == DASM_TOK_TYPE_EOL || tok_type == DASM_TOK_TYPE_EOP) {
			continue;
		}
		word_len++;
	}
	return word_len;
}

DICEIMPL enum DASM_TOK_ERR_  dasm_tokenize_programme(struct dasm_tok_line rdwr_tok_lines[], const libdice_word_t c_tok_lines_len,
		const char rd_src[], const libdice_word_t c_src_len, struct dasm_tok_status *rdwr_status)
{
	enum DASM_TOK_ERR_ errcode = DASM_TOK_ERR_OK;
	libdice_word_t real_src_len = 0;

	real_src_len = (libdice_word_t)strlen(rd_src) + 1;

	if (real_src_len > c_src_len) {
		real_src_len = c_src_len;
	}

	while (rdwr_status->tok_line_cnt < c_tok_lines_len && rdwr_status->read_cnt < real_src_len) {
		libdice_word_t tmp_read_cnt = 0;

		errcode = dasm_tokenize_line(&rdwr_tok_lines[rdwr_status->tok_line_cnt], &rd_src[rdwr_status->read_cnt],
				real_src_len-rdwr_status->read_cnt, &tmp_read_cnt);

		if (errcode != DASM_TOK_ERR_OK) {
			break;
		}

		if (rdwr_tok_lines[rdwr_status->tok_line_cnt].m_tok_cnt) {	
			rdwr_status->tok_line_cnt++;
		}


		rdwr_status->read_cnt += tmp_read_cnt;

	}


	return errcode;
}
