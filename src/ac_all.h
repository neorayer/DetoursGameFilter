#ifndef AC_ALL_H_
#define AC_ALL_H_


typedef struct wc3gamedll_keynode {
	DWORD pos;
	u_char val;
} wc3gamedll_keynode_t;


#define AC_GAMETYPE_NONE	0
#define AC_GAMETYPE_WC3120e	1
#define AC_GAMETYPE_CS15	2

//ac_hack_check_thread_param
typedef struct ac_hct_param {
	int type;
	HANDLE war3_process_handle;
	char war3_dirpath[MAX_PATH];
}ac_hct_param_t;



void ac_start_hack_check_thread(ac_hct_param_t * p_ac_hct_param) ;

void ac_stop_hack_check_thread();

bool ac_wc3_check_ver_120e(char * wc3_dirpath);

bool ac_wc3_found_aysMH_final(char * wc3_dirpath);

bool ac_war3_found_jianjiaMH(HANDLE hProcess);

bool ac_wc3_check_del_lpk(char * wc3_dirpath);
bool ac_wc3_check_mss32(char * wc3_dirpath);
//void ac_wc3secout_120e_begin(char * war3dir_path);

//void ac_wc3secout_120e_end(char * war3dir_path);

bool ac_wc3secout_120e_dealwith(HANDLE hProcess);

#endif /*AC_ALL_H_*/
