#include <ae2f/cc.h>
#include "./lexer.h"
#include <ctype.h>
#include "dasm/err.h"
#include "./toks.h"
#include <stdlib.h>


DICEIMPL bool dasm_lexer_init(struct dasm_lexer *rdwr_lexer, struct dasm_tok rdwr_toks[],
	const libdice_word_t c_toks_len, const char rd_src[], const libdice_word_t c_src_len)
{
	if (!rdwr_lexer || !rdwr_toks || !rd_src) {
		return false;
	}

	rdwr_lexer->m_src = rd_src;
	rdwr_lexer->m_src_len = c_src_len;
	rdwr_lexer->m_src_cnt = 0;

	rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;

	return dasm_tok_stream_init(&rdwr_lexer->m_tok_stream, rdwr_toks, c_toks_len);
}

DICEIMPL bool dasm_lexer_deinit(struct dasm_lexer *rdwr_lexer)
{
	if (!rdwr_lexer) {
		return false;	/** NRE */
	}

	rdwr_lexer->m_src = NULL;
	rdwr_lexer->m_src_len = 0;
	rdwr_lexer->m_src_cnt = 0;

	dasm_tok_stream_deinit(&rdwr_lexer->m_tok_stream);

	rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;

	return true;
}

static ae2f_inline enum DASM_ERR_ dasm_lexer_execute_line(struct dasm_lexer *rdwr_lexer)
{

	libdice_word_t char_verification_cnt = 0;

	if (!(rdwr_lexer && rdwr_lexer->m_src)) {
		return DASM_ERR_UNKNOWN;	/** NRE */
	}

	while (rdwr_lexer->m_tok_stream.m_tok_cnt < rdwr_lexer->m_tok_stream.m_toks_len
		&& rdwr_lexer->m_src_cnt < rdwr_lexer->m_src_len) {
		const char * const lexeme = &(rdwr_lexer->m_src[rdwr_lexer->m_src_cnt]);
		const char ch = lexeme[0];


		switch (rdwr_lexer->m_state) {
		case DASM_LEXER_STATE_IDLE:

#if 1
			switch(ch) {
				enum DASM_TOK_TYPE_	TOKTYPE_SETTYPE;

				ae2f_unexpected_but_if(0) {
					case '\'':
						char_verification_cnt = 0;
						rdwr_lexer->m_state = DASM_LEXER_STATE_CHAR_IMM;
						TOKTYPE_SETTYPE = DASM_TOK_TYPE_CHAR_IMM;
				}

				ae2f_unexpected_but_if(0) {
					ae2f_unreachable();
					case '\"':
						rdwr_lexer->m_state = DASM_LEXER_STATE_STRING_IMM;
						TOKTYPE_SETTYPE = DASM_TOK_TYPE_STRING_IMM;
				}

				ae2f_unexpected_but_if(0) {
					ae2f_unreachable();
					case '*':
						rdwr_lexer->m_state = DASM_LEXER_STATE_STAR;
						TOKTYPE_SETTYPE = DASM_TOK_TYPE_STAR;
				}
				ae2f_unexpected_but_if(0) {
					ae2f_unreachable();
					case '\n':
						TOKTYPE_SETTYPE = DASM_TOK_TYPE_EOL;
				}

				ae2f_unexpected_but_if(0) {
					ae2f_unreachable();
					case '\0':
						TOKTYPE_SETTYPE = DASM_TOK_TYPE_EOP;
				}

				ae2f_unexpected_but_if(0) {
					ae2f_unreachable();
					case ',':
						TOKTYPE_SETTYPE = DASM_TOK_TYPE_COMMA;
				}

				ae2f_unexpected_but_if(0) {
					ae2f_unreachable();
					case ':':
						TOKTYPE_SETTYPE = DASM_TOK_TYPE_COLON;
				}

				dasm_tok_stream_append(&rdwr_lexer->m_tok_stream);
				dasm_tok_stream_set_lexeme(&rdwr_lexer->m_tok_stream, lexeme, 1);
				dasm_tok_stream_set_type(&rdwr_lexer->m_tok_stream, TOKTYPE_SETTYPE);
				ae2f_fallthrough;
				case ' ':
				rdwr_lexer->m_src_cnt++;

				switch(ch) {
					case '\n': case '\0':
						return DASM_ERR_OK;
					default:break;
				}
				break;

				default:


				if (isalpha(ch) || ch == '_'){

					rdwr_lexer->m_state = DASM_LEXER_STATE_IDENT;

					dasm_tok_stream_append(&rdwr_lexer->m_tok_stream);
					dasm_tok_stream_set_lexeme(&rdwr_lexer->m_tok_stream, lexeme, 1);
					dasm_tok_stream_set_type(&rdwr_lexer->m_tok_stream, DASM_TOK_TYPE_IDENT);

					rdwr_lexer->m_src_cnt++;

				} else if (isdigit(ch)) {

					rdwr_lexer->m_state = DASM_LEXER_STATE_INT_IMM;

					dasm_tok_stream_append(&rdwr_lexer->m_tok_stream);
					dasm_tok_stream_set_lexeme(&rdwr_lexer->m_tok_stream, lexeme, 1);
					dasm_tok_stream_set_type(&rdwr_lexer->m_tok_stream, DASM_TOK_TYPE_INT_IMM);

					rdwr_lexer->m_src_cnt++;

				} else {
					return DASM_ERR_INVAL_PROG;
				}
			}
#endif


			break;

		case DASM_LEXER_STATE_IDENT:

			if (isalnum(ch) || ch == '_') {
				dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
				rdwr_lexer->m_src_cnt++;
			} else {
				rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;
			}
			break;

		case DASM_LEXER_STATE_INT_IMM:

			if (isdigit(ch)) {
				dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
				rdwr_lexer->m_src_cnt++;
			} else {
				rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;
			}
			break;

		case DASM_LEXER_STATE_CHAR_IMM:

			switch (ch) {
				case '\'':
					if (char_verification_cnt != 1) {
						return DASM_ERR_INVAL_CHAR_IMM;
					}
					dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
					rdwr_lexer->m_src_cnt++;
					rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;
					break;

				case '\n':
				case '\0':
					return DASM_ERR_INVAL_CHAR_IMM;
				default:
					char_verification_cnt++;

					dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
					rdwr_lexer->m_src_cnt++;
					break;
			}
			break;

		case DASM_LEXER_STATE_STRING_IMM:

			switch (ch) {
				case '\"':

					dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
					rdwr_lexer->m_src_cnt++;
					rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;
					break;

				case '\n':
				case '\0':
					return DASM_ERR_INVAL_STRING_IMM;
				default:

					dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
					rdwr_lexer->m_src_cnt++;
					break;

			}
			break;

		case DASM_LEXER_STATE_STAR:

			if (ch == '*') {
				dasm_tok_stream_increase_lexeme_len(&rdwr_lexer->m_tok_stream, 1);
				rdwr_lexer->m_src_cnt++;
			} else {
				rdwr_lexer->m_state = DASM_LEXER_STATE_IDLE;
			}
			break;

		default:
			return DASM_ERR_UNKNOWN;
		}
	}

	if (rdwr_lexer->m_tok_stream.m_tok_cnt == rdwr_lexer->m_tok_stream.m_toks_len) {
		return DASM_ERR_MEM_INSUF;
	}

	if (rdwr_lexer->m_src_cnt == rdwr_lexer->m_src_len) {
		return DASM_ERR_NO_TERM;
	}

	return DASM_ERR_UNKNOWN;
}

DICEIMPL enum DASM_ERR_ dasm_lexer_execute(struct dasm_lexer *rdwr_lexer)
{
	enum DASM_ERR_ err = DASM_ERR_OK;

	if (!rdwr_lexer) {
		return DASM_ERR_UNKNOWN;
	}

	while (rdwr_lexer->m_tok_stream.m_tok_cnt < rdwr_lexer->m_tok_stream.m_toks_len
			&& rdwr_lexer->m_src_cnt < rdwr_lexer->m_src_len) {

		err = dasm_lexer_execute_line(rdwr_lexer);
		if (err != DASM_ERR_OK) {
			break;
		}
	}

	return err;
}
