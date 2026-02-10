#ifndef	rolladie_h
#define rolladie_h

#include <libdice/type.h>

/**
 * @enum ROLLADIE_RET_
 * operation: from server to client
 * */
enum ROLLADIE_RET_ {
	/**
	 * @brief
	 * whole thing when something changed on server 
	 * reqcode | usercount | maxusercount | linecount | [users] | [lines]
	 *
	 * @details
	 * [lines]
	 * 	> uid32 | time32 | wrdcount_str | ... 
	 * [users]
	 * 	> uid32 | wrdcount_name | ... 
	 * */
	ROLLADIE_RET_SYNC,
	ROLLADIE_RET_QUEUEOUT
};

/**
 * @enum ROLLADIE_REQ_
 * operation: from admin client to server 
 * */
enum ROLLADIE_AREQ_ {
	ROLLADIE_AREQ_EXECAS,

	/**
	 * @brief
	 * configuring a vm as word count
	 * for both: 0 will be ignored.
	 *
	 * reqcode | ram count | lookup count
	 * */
	ROLLADIE_AREQ_VCFG,

	/**
	 * @brief
	 * configuring a name as word count
	 * 0 will be ignored.
	 *
	 * reqcode | name count
	 * */
	ROLLADIE_AREQ_NCFG,

	/**
	 * @brief
	 * configuring a chat name as word count
	 * 0 will be ignored.
	 *
	 * reqcode | chat count
	 * */
	ROLLADIE_AREQ_CCFG,

	/**
	 * @brief
	 * configuring a chat line count on one send
	 * 0 will be ignored.
	 * */
	ROLLADIE_AREQ_LCFG
};

/**
 * @enum ROLLADIE_REQ_
 * operation: from client to server 
 * */
enum ROLLADIE_REQ_ {
	/** 
	 * messaging a plain text 
	 * could cause sync to caller
	 * could cause sync to non-caller
	 *
	 * reqcode | wordcount32 | ...
	 * */
	ROLLADIE_REQ_TXT,

	/**
	 * @brief
	 * clean a whole local memory to zero
	 *
	 * reqcode
	 * */
	ROLLADIE_REQ_VSET,

	/** 
	 * run a vm 
	 * could cause sync
	 *
	 * reqcode | wordcount32 | ...
	 * */
	ROLLADIE_REQ_VRUN,

	/** 
	 * change the nickname 
	 * could cause sync to caller
	 * could cause sync to non-caller
	 *
	 * reqcode | wordcount32 | ...
	 * */
	ROLLADIE_REQ_CHGNAME,

	/** 
	 * @brief
	 * set a chat.
	 * if not present, try making one.
	 *
	 * could cause sync to caller
	 *
	 * reqcode | wordcount32 | ...
	 * */
	ROLLADIE_REQ_CHGCHAT,

	/**
	 * @brief
	 * quit
	 *
	 * reqcode
	 * */
	ROLLADIE_QUIT,

	/**
	 * @brief
	 * join
	 * if try to join with same `wanted_uid32` will not be answered.
	 * ip32 | (ip16ifv6, port16) will be get automatically
	 *
	 * reqcode | wanted_uid32 
	 * */
	ROLLADIE_JOIN
};

#endif
