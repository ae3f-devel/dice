#include <ae2f/Sys/Inet.h>
#include <ae2f/c90/StdInt.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char** argv);

int main(int argc, const char** argv) {
	enum AE2FSYS_INET_	RET;
	ae2fsys_sock_t		SOCK_SVR;
	union 
	{ 
		struct sockaddr_in m_in;
		struct sockaddr m_addr;	
	} SOCKADDR_SVR;
	u16_least	PORT;

	switch(argc)
	{
		case 0:
		case 1:
		case 2:
			puts("[main]\tmust be ./dice-server [ip] [port]");
			return 0;

		default:
		case 3:
			PORT = (u16_least)atoi(argv[2]);
			break;
	}

	/************************************************************/
	/** socket bind & listen */
	/************************************************************/
	ae2fsys_declmkinet(DECLINET);
	ae2fsys_initinet_imp(RET, MAKEWORD(2, 2), DECLINET);

	assert(!RET && "[main]\tae2fsys_initinet_imp failed.");

	SOCK_SVR = socket(AF_INET, SOCK_STREAM, 0);

	unless(ae2fsys_checksock_naive(SOCK_SVR))
	{
		closesocket(SOCK_SVR);
		assert(0 && "[main]\tsocket failed.");
		return -1;
	}

	{
		ae2fsys_sock_blkmode_t	rdwr_mem;
		enum AE2FSYS_SOCK_	res;
		_ae2fsys_set_sock_blkmode_imp(
				rdwr_mem
				, SOCK_SVR
				, 1
				, res);

		if(res) {
			assert(0 && "[main]\t _ae2fsys_set_sock_blkmode_imp failed.");
			closesocket(SOCK_SVR);
			return -1;
		}
	}

	memset(&SOCKADDR_SVR.m_in, 0, sizeof(SOCKADDR_SVR.m_in));
	SOCKADDR_SVR.m_in.sin_addr.s_addr	= INADDR_ANY;
	SOCKADDR_SVR.m_in.sin_port		= htons(PORT);
	SOCKADDR_SVR.m_in.sin_family		= AF_INET;

	if(bind(
				SOCK_SVR
				, &SOCKADDR_SVR.m_addr
				, sizeof(SOCKADDR_SVR.m_in)) < 0)
	{
		closesocket(SOCK_SVR);
		assert(0 && "[main]\tbind failed.");
		return -1;
	}

	if(listen(SOCK_SVR, 5) < 0)
	{
		closesocket(SOCK_SVR);
		assert(0 && "[main]\tlisten failed.");
		return -1;
	}

	puts("[main] has been started gracefully!");

	/************************************************************/
	/** running */
	/************************************************************/
	while((volatile int)1) {

	}

	/************************************************************/
	/** cleanup */
	/************************************************************/
	closesocket(SOCK_SVR);
	ae2fsys_stopinet_imp(RET);
	assert(!RET && "[main]\tae2fsys_stopinet_imp failed.");
	(void)RET;

	return 0;
}
