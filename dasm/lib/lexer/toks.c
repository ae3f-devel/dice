#include "toks.h"
#include <stddef.h>
#include <stdlib.h>

DICEIMPL bool dasm_tok_stream_seek(struct dasm_tok_stream *rdwr_tstream, 
	const ptrdiff_t c_offset, const enum DASM_TOK_STREAM_WHENCE_ c_whence)
{
	ptrdiff_t base;
	ptrdiff_t cnt;
	ptrdiff_t new_whence;

	
	if (!rdwr_tstream || !rdwr_tstream->m_tok_cnt) {
		return false;
	}
	
	cnt = (ptrdiff_t)rdwr_tstream->m_tok_cnt;

	switch (c_whence) {
	case DASM_TOK_STREAM_WHENCE_SET:
		base = 0;
		break;

	case DASM_TOK_STREAM_WHENCE_CURSOR:
		base = (ptrdiff_t)rdwr_tstream->m_whence;
		break;

	case DASM_TOK_STREAM_WHENCE_END:
		base = cnt -1;
		break;

	default:
		return false;
	}

	new_whence = base + c_offset;
	if (new_whence < 0 || new_whence > cnt) {
		return false;
	}

	rdwr_tstream->m_whence = (libdice_word_t)new_whence;
	
	return true;
}

DICEIMPL struct dasm_tok* dasm_tok_stream_peek(struct dasm_tok_stream *rdwr_tstream)
{
	if (!rdwr_tstream || !rdwr_tstream->m_tok_cnt) {
		return NULL;
	}

	return &(rdwr_tstream->m_toks[rdwr_tstream->m_whence]);
}

DICEIMPL bool dasm_tok_stream_advance(struct dasm_tok_stream *rdwr_tstream)
{
	if (!rdwr_tstream || (rdwr_tstream->m_whence == rdwr_tstream->m_tok_cnt - 1)) {
		return false;
	}

	rdwr_tstream->m_whence++;

	return true;
}

static bool dasm_tok_init(struct dasm_tok *rdwr_tok)
{
	if (!rdwr_tok) {
		return false;
	}

	rdwr_tok->m_lexeme = NULL;
	rdwr_tok->m_lexeme_len = 0;
	rdwr_tok->m_tok_type = DASM_TOK_TYPE_EOP;

	return true;
}

static bool dasm_tok_deinit(struct dasm_tok *rdwr_tok)
{
	if (!rdwr_tok) {
		return false;
	}

	rdwr_tok->m_lexeme = NULL;
	rdwr_tok->m_lexeme_len = 0;
	rdwr_tok->m_tok_type = DASM_TOK_TYPE_EOP;

	return true;
}

DICEIMPL bool dasm_tok_stream_init(struct dasm_tok_stream *rdwr_tstream, 
	struct dasm_tok rdwr_toks[], const libdice_word_t c_toks_len)
{
	libdice_word_t i = 0;
	libdice_word_t j = 0;

	if (!rdwr_tstream || !rdwr_toks) {
		return false;
	}

	rdwr_tstream->m_toks = rdwr_toks;
	rdwr_tstream->m_toks_len = c_toks_len;
	rdwr_tstream->m_tok_cnt = 0;
	rdwr_tstream->m_whence = 0;

	for (i=0; i<c_toks_len; ++i) {
		if (!dasm_tok_init(&rdwr_tstream->m_toks[i])) {
			for (j=0; j<=i; ++j) {
				/* It would be false anyway */
				(void)dasm_tok_deinit(&rdwr_toks[j]);
			}
			return false;
		}
	}

	return true;
}

DICEIMPL bool dasm_tok_stream_deinit(struct dasm_tok_stream *rdwr_tstream)
{
	libdice_word_t i = 0;

	if (!rdwr_tstream) {
		return false;
	}

	for (i=0; i<rdwr_tstream->m_tok_cnt; ++i) {
		if (!dasm_tok_deinit(&rdwr_tstream->m_toks[i])) {
			return false;
		}
	}

	return true;
}

DICEIMPL bool dasm_tok_stream_append(struct dasm_tok_stream *rdwr_tstream)
{
	if (!rdwr_tstream) {
		return false;
	}	
	
	if (rdwr_tstream->m_tok_cnt == rdwr_tstream->m_toks_len) {
		return false;
	}

	rdwr_tstream->m_tok_cnt++;

	return true;
}

DICEIMPL bool dasm_tok_stream_set_type(struct dasm_tok_stream *rdwr_tstream, 
	const enum DASM_TOK_TYPE_ c_tok_type)
{
	struct dasm_tok *tok = NULL;

	if (!rdwr_tstream) {
		return false;
	}

	if (!dasm_tok_stream_seek(rdwr_tstream, 0, DASM_TOK_STREAM_WHENCE_END)) {
		return false;
	}

	tok = dasm_tok_stream_peek(rdwr_tstream);
	if (!tok) {
		return false;
	}

	tok->m_tok_type = c_tok_type;
	
	return true;
}

DICEIMPL bool dasm_tok_stream_set_lexeme(struct dasm_tok_stream *rdwr_tstream, 
	const char *rd_lexeme, const libdice_word_t c_lexeme_len)
{
	struct dasm_tok *tok = NULL;

	if (!rdwr_tstream || !rd_lexeme) {
		return false;
	}

	if (!dasm_tok_stream_seek(rdwr_tstream, 0, DASM_TOK_STREAM_WHENCE_END)) {
		return false;
	}

	tok = dasm_tok_stream_peek(rdwr_tstream);
	if (!tok) {
		return false;
	}

	tok->m_lexeme = rd_lexeme;
	tok->m_lexeme_len = c_lexeme_len;

	return true;
}

DICEIMPL bool dasm_tok_stream_set_lexeme_len(struct dasm_tok_stream *rdwr_tstream, 
	const libdice_word_t c_lexeme_len)
{
	struct dasm_tok *tok = NULL;

	if (!rdwr_tstream) {
		return false;
	}

	if (!dasm_tok_stream_seek(rdwr_tstream, 0, DASM_TOK_STREAM_WHENCE_END)) {
		return false;
	}

	tok = dasm_tok_stream_peek(rdwr_tstream);
	if (!tok) {
		return false;
	}

	tok->m_lexeme_len = c_lexeme_len;

	return true;
}

DICEIMPL bool dasm_tok_stream_increase_lexeme_len(struct dasm_tok_stream *rdwr_tstream, 
	const libdice_word_t c_add_len)
{
	struct dasm_tok *tok = NULL;

	if (!rdwr_tstream) {
		return false;
	}
	
	if (!dasm_tok_stream_seek(rdwr_tstream, 0, DASM_TOK_STREAM_WHENCE_END)) {
		return false;
	}

	tok = dasm_tok_stream_peek(rdwr_tstream);
	if (!tok) {
		return false;
	}

	tok->m_lexeme_len += c_add_len;

	return true;
}
