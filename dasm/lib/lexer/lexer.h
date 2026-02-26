


#ifndef dasm_lexer_h
#define dasm_lexer_h

#include <ae2f/c90/StdBool.h>
#include <dasm/err.h>
#include <libdice/abi.h>
#include <libdice/type.h>
#include "toks.h"

enum DASM_LEXER_STATE_ {
	DASM_LEXER_STATE_IDLE,
	DASM_LEXER_STATE_IDENT,
	DASM_LEXER_STATE_INT_IMM,
	DASM_LEXER_STATE_CHAR_IMM,
	DASM_LEXER_STATE_STRING_IMM,
	DASM_LEXER_STATE_STAR
};

struct dasm_lexer {
	libdice_word_t m_src_len;
	libdice_word_t m_src_cnt;

	struct dasm_tok_stream m_tok_stream;
	const char *m_src;

	enum DASM_LEXER_STATE_ m_state;
};

/**
 * @brief 
 * 
 * @param rdwr_lexer 
 * @param rdwr_toks 
 * @param c_tokens_len 
 * @param rd_src 
 * @param c_src_len 
 * @return bool Returns true on success and false on failure.
 */
DICECALL bool dasm_lexer_init(struct dasm_lexer *rdwr_lexer,
	struct dasm_tok rdwr_toks[], const libdice_word_t c_tokens_len,
	const char rd_src[], const libdice_word_t c_src_len);

/**
 * @brief 
 * 
 * @param rdwr_lexer 
 * @return bool Returns true on success and false on failure.
 */
DICECALL bool dasm_lexer_deinit(struct dasm_lexer *rdwr_lexer);

DICECALL enum DASM_ERR_ dasm_lexer_execute(struct dasm_lexer *rdwr_lexer);

#endif /* dasm_lexer_h */
