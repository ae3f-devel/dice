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

struct dasm_pp_ret {
	enum DASM_PP_ERR_ m_err;
	libdice_word_t m_line_cnt;
};

DICECALL struct dasm_pp_ret dasm_preprocess_programme(char rdwr_dst[], const libdice_word_t c_dst_len,
		const char rd_src[], const libdice_word_t c_src_len,
		libdice_word_t *rdwr_write_cnt);

#endif /* dasm_pp_h */
