#undef	__ae2f_MACRO_GENERATED
#define	__ae2f_MACRO_GENERATED	1
/** @file uid64.def.c */

#ifndef	rolladie_uid64_auto_h
#define	rolladie_uid64_auto_h

#include <rolladie/uid64.core.h>
#undef __ae2f_MACRO_GENERATED
#define __ae2f_MACRO_GENERATED 1
#include <ae2f/Sys/Inet.h>
#undef __ae2f_MACRO_GENERATED
#define __ae2f_MACRO_GENERATED 1
#include <ae2f/Macro.h>
#undef __ae2f_MACRO_GENERATED
#define __ae2f_MACRO_GENERATED 1

#define _rolladie_uid64_uid32v2( \
	/** tparam */ \
		 \
 \
	/** param */ \
		/*    , const rolladie_uid64_t */ c_uid64, \
		/*      u32_least */ r_uid0, \
		/*      u32_least    */ r_uid1 \
) \
{ \
	(r_uid0) = (u32_least)((c_uid64) & 0xFFFFFFFF); \
	(r_uid1) = (u32_least)((c_uid64) >> 32) & 0xFFFFFFFF; \
}

#define _rolladie_mkuid64_ipv4( \
	/** tparam */ \
		 \
 \
	/** param */ \
		/*    , rolladie_uid64_t */ ret, \
		/*     const  u32_least */ c_ip, \
		/*      const  u16_least    */ c_port \
)  \
{ \
	(ret) = ((u64_fast)(c_port) << 32) | (u64_fast)(c_ip); \
}

#ifndef _rolladie_mkuid64_ipv4
#define	_rolladie_mkuid64_ipv4	rolladie_mkuid64_ipv4
#endif

#define _rolladie_mkuid64_sockaddr_in( \
	/** tparam */ \
		 \
 \
	/** param */ \
		/*    , rolladie_uid64_t */ ret, \
		/*     const struct sockaddr_in    */ c_addr \
) \
{ \
	_rolladie_mkuid64_ipv4(ret \
			, c_addr.sin_addr.s_addr \
			, c_addr.sin_port); \
}

#endif

#undef	__ae2f_MACRO_GENERATED

#define	__ae2f_MACRO_GENERATED	0

