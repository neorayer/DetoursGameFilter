#include <stdio.h>

#include <zr_logger.h>

#include "gf_props.h"

void gf_props_buildpath(char * path) {
	GetTempPath(MAX_PATH, path);
	strcat(path, "gf_props.ini");
}

void gf_props_read(gf_props_t * p_props) {
	char path[MAX_PATH];
	gf_props_buildpath(path);
	//	zr_log_info_ec(0, "Read gf_props file %s", path);
	if (ZR_FILE_SUCC != zr_fexist(path)) {
				zr_log_error_ec(0, "File isn't exist. path=%s", path);
//		return;
	}

	
	char * sec_name = "gf_props";
	p_props->v_netmask_c = GetPrivateProfileInt(sec_name, "v_netmask_c", 3, path);
	p_props->pipe_tcpport = GetPrivateProfileInt(sec_name, "pipe_tcpport", 8000, path);
	p_props->pipe_udpport = GetPrivateProfileInt(sec_name, "pipe_udpport", 8000, path);
	p_props->myspeed = GetPrivateProfileInt(sec_name, "myspeed", 1000, path);
	
	char buf[128];
	GetPrivateProfileString(sec_name, "v_ipaddr", "170.3.2.1", buf, sizeof buf, path);
	p_props->v_ipaddr.u_l = inet_addr(buf);
	
	GetPrivateProfileString(sec_name, "game_path", "", p_props->game_path, sizeof p_props->game_path, path);
	GetPrivateProfileString(sec_name, "pipe_host", "10.1.1.196", p_props->pipe_host, sizeof p_props->pipe_host, path);
	GetPrivateProfileString(sec_name, "username", "", p_props->username, MAX_USERNAME, path);
	GetPrivateProfileString(sec_name, "nickname", "", p_props->nickname, MAX_NICKNAME, path);
	GetPrivateProfileString(sec_name, "res_url", "", p_props->res_url, MAX_PATH, path);
	GetPrivateProfileString(sec_name, "sessionkey", "", p_props->sessionkey, MAX_PATH , path);
	GetPrivateProfileString(sec_name, "roomid", "", p_props->roomid, MAX_PATH, path);

	p_props->udpdig_port_bgn = GetPrivateProfileInt(sec_name, "udpdig_port_bgn", 27015, path);
	p_props->udpdig_port_end = GetPrivateProfileInt(sec_name, "udpdig_port_end", 27024, path);
	p_props->has_tcp = GetPrivateProfileInt(sec_name, "has_tcp", 0, path);

}


void gf_props_debug(gf_props_t * props) {
	zr_log_debug_ec(0, "game_path=%s", props->game_path);
	zr_log_debug_ec(0, "v_ipaddr=%d.%d.%d.%d", 
			props->v_ipaddr.u_bs[0], //
			props->v_ipaddr.u_bs[1], //
			props->v_ipaddr.u_bs[2], //
			props->v_ipaddr.u_bs[3]);
	zr_log_debug_ec(0, "v_netmask_c=%d", props->v_netmask_c);
	zr_log_debug_ec(0, "pipe_host=%s", props->pipe_host);
	zr_log_debug_ec(0, "pipe_tcpport=%d", props->pipe_tcpport);
	zr_log_debug_ec(0, "pipe_udpport=%d", props->pipe_udpport);
	zr_log_debug_ec(0, "udpdig_port_bgn=%d", props->udpdig_port_bgn);
	zr_log_debug_ec(0, "udpdig_port_end=%d", props->udpdig_port_end);
	zr_log_debug_ec(0, "has_tcp=%d", props->has_tcp);
	zr_log_debug_ec(0, "username=%s", props->username);
	zr_log_debug_ec(0, "nickname=%s", props->nickname);
	zr_log_debug_ec(0, "myspeed=%d", props->myspeed);
	zr_log_debug_ec(0, "res_url=%s", props->res_url);
	zr_log_debug_ec(0, "sessionkey=%s", props->sessionkey);
	zr_log_debug_ec(0, "roomid=%s", props->roomid);
}


