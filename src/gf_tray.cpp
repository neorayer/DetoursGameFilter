#include <windows.h>

#include <zr_logger.h>

#define MYWM_NOTIFYICON (WM_USER+1)

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	zr_log_debug_ec(0,"message=%d\r\n", message);
	
	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case MYWM_NOTIFYICON:
			zr_log_debug("icon!!!!!!!!!!!!!!1");
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;

	return RegisterClassEx(&wcex);
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
		int nCmdShow) {
	//Logger Setting
	zr_log_set_target(ZR_LOG_TG_FILE);
	char logpath[MAX_PATH];
	GetTempPath(sizeof logpath, logpath);
	strcat(logpath, "gf.log");
	zr_log_set_filepath(logpath);

	
	MyRegisterClass(hInstance);
	
	MSG msg;


	HWND hwndTask =:: FindWindow(NULL, "");
	::ShowWindow(hwndTask, SW_SHOW);

	HICON hIcon = (HICON) LoadImage(NULL, "gf.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	NOTIFYICONDATA ni_data;
	ni_data.cbSize = sizeof(NOTIFYICONDATA);
	ni_data.hWnd = hwndTask;
	ni_data.uCallbackMessage = MYWM_NOTIFYICON;
	ni_data.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	ni_data.hIcon = hIcon;
	strcpy(ni_data.szTip, "333333333333333333");

	//	STARTUPINFO startupInfo = { 0 };
			//	startupInfo.cb = sizeof(startupInfo);
			//	PROCESS_INFORMATION processInformation;
			//
			//	BOOL result =:: CreateProcess(
			//			"C:\\Applications\\KO10000开发中期测试\\开始游戏.exe",
			//			NULL,
			//			NULL,
			//			NULL,
			//			FALSE,
			//			NORMAL_PRIORITY_CLASS,
			//			NULL,
			//			NULL,
			//			&startupInfo,
			//			&processInformation
			//	);
			//	if (!result) {
			//		zr_log_error_ec(0, "Create process failed. ");
			//		return 0;
			//	}

			BOOL ni_res = Shell_NotifyIcon(NIM_ADD, &ni_data);

			while (GetMessage(&msg, NULL, 0, 0)) {
				zr_log_debug_ec(0,"message=%d\r\n", msg.message);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			return msg.wParam;
		}