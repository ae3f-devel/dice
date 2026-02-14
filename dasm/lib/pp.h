#ifndef dasm_pp_h
#define dasm_pp_h

#include <libdice/type.h>
#include <dasm.h>

enum DASM_PP_ERR_ {
	DASM_PP_ERR_OK,
	DASM_PP_ERR_UNKNOWN,
	DASM_PP_ERR_MEM_INSUF,
	DASM_PP_ERR_INVAL_COMMENT,
	/** 
	 * @brief No '\n' and '\0'
	 * NO_TERMINATION
	 * */
	DASM_PP_ERR_NO_TERM,
	DASM_PP_ERR_UNCLOSED_BLOCK_COMMENT
};

struct dasm_pp_status {
	libdice_word_t m_read_line_cnt;
	libdice_word_t m_read_char_cnt;
	libdice_word_t m_write_char_cnt;
};

DICECALL enum DASM_PP_ERR_ dasm_preprocess_programme(char rdwr_dst[], const libdice_word_t c_dst_len,
		const char rd_src[], const libdice_word_t c_src_len,
		struct dasm_pp_status *rdwr_status);

#endif /* dasm_pp_h */
