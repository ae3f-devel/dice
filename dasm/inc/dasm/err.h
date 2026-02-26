#ifndef dasm_err_h
#define dasm_err_h

enum DASM_ERR_ {
        DASM_ERR_OK,
        DASM_ERR_UNKNOWN,
        DASM_ERR_NO_TERM,
        DASM_ERR_MEM_INSUF,
        DASM_ERR_INVAL_COMMENT,
	DASM_ERR_INVAL_CHAR_IMM,
	DASM_ERR_INVAL_STRING_IMM,
	/**
	 * @brief Invalid programme. 
	 * 
	 */
	DASM_ERR_INVAL_PROG,
	DASM_ERR_INVAL_IDENT,
	DASM_ERR_INVAL_MNEMONIC
};
 

#endif /* dasm_err_h */