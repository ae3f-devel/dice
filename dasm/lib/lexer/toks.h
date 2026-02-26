


#ifndef dasm_toks_h
#define dasm_toks_h

#include <ae2f/c90/StdBool.h>
#include <dasm/err.h>
#include <libdice/abi.h>
#include <libdice/type.h>

enum DASM_TOK_STREAM_WHENCE_ {
	DASM_TOK_STREAM_WHENCE_SET,
	DASM_TOK_STREAM_WHENCE_CURSOR,
	DASM_TOK_STREAM_WHENCE_END
};

enum DASM_TOK_TYPE_ {
	DASM_TOK_TYPE_MNEMONIC,
	DASM_TOK_TYPE_IDENT,
	DASM_TOK_TYPE_INT_IMM,
	DASM_TOK_TYPE_CHAR_IMM,
	DASM_TOK_TYPE_STRING_IMM,
	DASM_TOK_TYPE_STAR,
	DASM_TOK_TYPE_COMMA,
	DASM_TOK_TYPE_EOL,
	DASM_TOK_TYPE_EOP
};

struct dasm_tok {
	enum DASM_TOK_TYPE_ m_tok_type;
	libdice_word_t m_lexeme_len;
	const char *m_lexeme;
};

struct dasm_tok_stream {
    struct dasm_tok *m_toks;
    libdice_word_t m_toks_len;
    libdice_word_t m_tok_cnt;
    libdice_word_t m_whence;
};

/**
 * @brief 
 * 
 * @param rdwr_tstream 
 * @param c_offset 
 * @param c_whence 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_tok_stream_seek(struct dasm_tok_stream *rdwr_tstream, 
	const long c_offset, const enum DASM_TOK_STREAM_WHENCE_ c_whence);

DICECALL struct dasm_tok* dasm_tok_stream_peek(struct dasm_tok_stream *rdwr_tstream);

/**
 * @brief 
 * 
 * @param rdwr_tstream 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_tok_stream_advance(struct dasm_tok_stream *rdwr_tstream);

/**
 * @brief 
 * 
 * @param rdwr_tstream 
 * @param rdwr_toks 
 * @param c_toks_len 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_tok_stream_init(struct dasm_tok_stream *rdwr_tstream, 
	struct dasm_tok rdwr_toks[], const libdice_word_t c_toks_len);

/**
 * @brief 
 * 
 * @param rdwr_tstream 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_tok_stream_deinit(struct dasm_tok_stream *rdwr_tstream);

/**
 * @brief 
 * 
 * @param rdwr_tstream 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_tok_stream_append(struct dasm_tok_stream *rdwr_tstream);

/**
 * @brief 
 * 
 * @param rdwr_tstream 
 * @param c_tok_type 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_tok_stream_set_type(struct dasm_tok_stream *rdwr_tstream, 
	const enum DASM_TOK_TYPE_ c_tok_type);

/**
 * @brief 
 * 
 * @param rdwr_tstream 
 * @param rd_lexeme 
 * @param c_lexeme_len 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_tok_stream_set_lexeme(struct dasm_tok_stream *rdwr_tstream, 
	const char *rd_lexeme, const libdice_word_t c_lexeme_len);

/**
 * @brief 
 * 
 * @param rdwr_tstream 
 * @param c_lexeme_len 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_tok_stream_set_lexeme_len(struct dasm_tok_stream *rdwr_tstream, 
	const libdice_word_t c_lexeme_len);

/**
 * @brief 
 * 
 * @param rdwr_tstream 
 * @param c_add_len 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_tok_stream_increase_lexeme_len(struct dasm_tok_stream *rdwr_tstream, 
	const libdice_word_t c_add_len);
	
#endif /* dasm_toks_h */