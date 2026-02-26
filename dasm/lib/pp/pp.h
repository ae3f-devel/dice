/**
 * @file pp.h
 * @author coder0908 (da471283@gmail.com)
 * @brief Remove comments, remove '\r', replace '\t' to ' '
 * @version 0.1
 * @date 2026-02-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */




#ifndef dasm_pp_h
#define dasm_pp_h

#include <ae2f/c90/StdBool.h>
#include <dasm/err.h>
#include <libdice/abi.h>
#include <libdice/type.h>

enum DASM_PP_STATE_ {
	DASM_PP_STATE_NORMAL,
	DASM_PP_STATE_BLOCK_COMMENT,
	DASM_PP_STATE_LINE_COMMENT,
	DASM_PP_STATE_STRING_IMM
};

struct dasm_pp {
	libdice_word_t m_dst_len;
	libdice_word_t m_dst_cnt;
	libdice_word_t m_src_len;
	libdice_word_t m_src_cnt;

	char *m_dst;
	const char *m_src;
	
	enum DASM_PP_STATE_ m_state;
};

/**
 * @brief 
 * 
 * @param rdwr_pp 
 * @param rdwr_dst 
 * @param c_dst_len 
 * @param rd_src 
 * @param c_src_len 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_pp_init(struct dasm_pp *rdwr_pp, 
	char rdwr_dst[], const libdice_word_t c_dst_len, 
	const char rd_src[], const libdice_word_t c_src_len);

/**
 * @brief 
 * 
 * @param rdwr_pp 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_pp_deinit(struct dasm_pp *rdwr_pp);

DICECALL enum DASM_ERR_ dasm_pp_execute(struct dasm_pp *rdwr_pp);

/**
 * @brief 
 * 
 * @param rdwr_pp 
 * @param rdwr_dst_opt 
 * @param c_dst_len 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_pp_reset_dst(struct dasm_pp *rdwr_pp, 
	char rdwr_dst_opt[], const libdice_word_t c_dst_len);

/**
 * @brief 
 * 
 * @param rdwr_pp 
 * @param rd_src_opt 
 * @param c_src_len 
 * @return bool Returns true on success and false on failure. 
 */
DICECALL bool dasm_pp_reset_src(struct dasm_pp *rdwr_pp, 
	const char rd_src_opt[], const libdice_word_t c_src_len);

#endif /* dasm_pp_h */