#pragma once
#include"stdafx.h"
#include<atomic>
__declspec(selectany) CString g_font_str= TEXT("Arial");
__declspec(selectany) COLORREF g_color_bk = RGB(253, 246, 227);
__declspec(selectany) COLORREF g_color_thumb = RGB(38, 139, 210);
__declspec(selectany) COLORREF g_color_hover = RGB(38, 139, 210);
__declspec(selectany) COLORREF g_color_text = RGB(7, 54, 66);
__declspec(selectany) COLORREF g_color_border = RGB(108, 113, 196);

__declspec(selectany) std::vector<CString> g_files;
__declspec(selectany) std::vector<std::pair<int,int>> g_files_progress;

__declspec(selectany) __int64 g_total_frame_count;
__declspec(selectany) enum Status {
	RUN, WAIT, INIT
}g_status;

///Thread safe
__declspec(selectany) std::atomic_bool g_prepare;
__declspec(selectany) std::atomic_int64_t g_progress;