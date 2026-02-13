#ifndef rolladie_svr_h
#define rolladie_svr_h

#include <libdice/type.h>
#include "./uid64.h"
#include <ae2f/Sys/Inet.h>

/** @brief client id */
typedef	libdice_word_t	rolladie_cid_t;

typedef struct {
	rolladie_uid32low_t	m_low;
	rolladie_uid32high_t	m_high;
}	rolladie_u64_t;

typedef libdice_word_t	rolladie_ptr_t[(sizeof(void*) + 3) / 4];

/***
 * @details
 * header | name | ram
 * */
typedef union {
	struct {
		rolladie_u64_t		m_client_id;
		libdice_word_t		m_admin;
		libdice_word_t		m_idx_client;
		libdice_word_t		m_addr_name;
		libdice_word_t		m_addr_ram;
		libdice_word_t		m_cap_ram;
	} m_info;

	libdice_word_t	m_data[1 << 8];
} rolladie_client, *h_rolladie_client_t;


typedef struct {
	libdice_word_t	m_num_client;
	libdice_word_t	m_num_chat;
} rolladie_svr;

#endif
