#ifndef dasm_tokenizer_h
#define dasm_tokenizer_h

#include <libdice/type.h>
#include <dasm.h>

#define DASM_TOK_MAX_LEN 64	
#define DASM_TOK_MAX_CNT_PER_LINE 12

enum DASM_TOK_TYPE_ {
	/** opcode, jmp *label */
	DASM_TOK_TYPE_IDENT,	
	/* immediate value, address*/
	DASM_TOK_TYPE_NUMBER,	
	/* label: */
	DASM_TOK_TYPE_LABEL,	
	/* "string" */
	DASM_TOK_TYPE_STRING,	
	/* 'A' */
	DASM_TOK_TYPE_ASCII,
	/* asterisk*/
	DASM_TOK_TYPE_OPERATOR,
	/* '\n' */
	DASM_TOK_TYPE_EOL,
	/* '\0' */
	DASM_TOK_TYPE_EOP,
	/**
	 * @brief 
	 * */
	DASM_TOK_TYPE_UNKNOWN
};

enum DASM_TOK_ERR_ {
	DASM_TOK_ERR_OK,
	DASM_TOK_ERR_MEM_INSUF,
	DASM_TOK_ERR_NO_TERM,
	DASM_TOK_ERR_INVAL_CHAR,
	DASM_TOK_ERR_INVAL_ASCII,
	DASM_TOK_ERR_INVAL_STRING,
	DASM_TOK_ERR_UNKNOWN
};

struct dasm_tok {
	char m_text[DASM_TOK_MAX_LEN];
	enum DASM_TOK_TYPE_ m_tok_type;
};

struct dasm_tok_line {
	struct dasm_tok m_toks[DASM_TOK_MAX_CNT_PER_LINE];
	libdice_word_t m_tok_cnt;
};

struct dasm_tok_status {
	/** 
	 * @brief Read line count without error line
	 * */
	libdice_word_t read_line_cnt;
	/** 
	 * @brief Read count without error character 
	 * */
	libdice_word_t read_cnt;
	libdice_word_t tok_line_cnt;
};


DICECALL enum DASM_TOK_TYPE_ dasm_tokenize_programme(struct dasm_tok_line rdwr_tok_lines[], const libdice_word_t c_dst_len,
		const char rd_src[], const libdice_word_t c_src_len, struct dasm_tok_status *status);
DICECALL libdice_word_t dasm_get_tok_line_word_len(const struct dasm_tok_line *rd_tok_line);


#endif /* dasm_tokenizer_h */
