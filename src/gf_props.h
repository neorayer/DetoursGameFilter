#ifndef GF_PROPS_H_
#define GF_PROPS_H_
#include <Windows.h>

#include <zr_file.h>
#include <zr_logger.h>

#define MAX_USERNAME 20
#define MAX_NICKNAME 32

typedef struct gf_props {
	char game_path[MAX_PATH];
	int v_netmask_c;
	union {
		u_char u_bs[4];
		u_long u_l;
	} v_ipaddr;
	char pipe_host[MAX_PATH];
	u_short pipe_tcpport;
	u_short pipe_udpport;
	u_short udpdig_port_bgn;
	u_short udpdig_port_end;
	u_char has_tcp;
	char username[MAX_USERNAME];
	char nickname[MAX_NICKNAME];
	char res_url[MAX_PATH];
	char sessionkey[MAX_PATH];
	char roomid[MAX_PATH];
	u_short myspeed;
} gf_props_t;

void gf_props_buildpath(char * path);
void gf_props_read(gf_props_t * p_props);
void gf_props_debug(gf_props_t *p_props);
#endif /*TMP_PROPS_H_*/
