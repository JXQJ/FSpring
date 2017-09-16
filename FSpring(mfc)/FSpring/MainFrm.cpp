// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "FSpring.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
BOOL PathFolderExistsA(const char* dir) {
	DWORD ftyp = GetFileAttributesA(dir);
	return (ftyp&FILE_ATTRIBUTE_DIRECTORY);
}
bool DeleteDirectory(std::string dir_path, bool noRecycleBin = true) {
	char *pszFrom = new char[dir_path.length() + 2];
	strcpy_s(pszFrom, dir_path.length() + 2, dir_path.c_str());
	pszFrom[dir_path.length()] = '\0';       // double-null termination
	pszFrom[dir_path.length() + 1] = '\0';   // double-null termination
	SHFILEOPSTRUCTA	FileOp;
	FileOp.hwnd = NULL;
	FileOp.wFunc = FO_DELETE;
	FileOp.pFrom = pszFrom;
	FileOp.pTo = NULL;
	FileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
	if (noRecycleBin == false) {
		FileOp.fFlags |= FOF_ALLOWUNDO;
	}
	FileOp.fAnyOperationsAborted = FALSE;
	FileOp.lpszProgressTitle = NULL;
	FileOp.hNameMappings = NULL;
	int ret = SHFileOperationA(&FileOp);
	delete[] pszFrom;
	return (ret == 0);
}
void GetVideoInfo(std::string video_file, int64_t* pframe_count = nullptr, int64_t* pduration_msec = nullptr, double* pfps = nullptr) {
	bool ret = true;
	av_register_all();
	AVFormatContext* pFormatCtx = nullptr;
	avformat_open_input(&pFormatCtx, video_file.c_str(), NULL, NULL);
	avformat_find_stream_info(pFormatCtx, NULL);
	int videoStream = -1;	//Find video Stream
	for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	}
	int64_t frame_count = pFormatCtx->streams[videoStream]->nb_frames;

	if (frame_count <= 0 || frame_count>INT_MAX) {
		//frame_count is incorrect
		frame_count = 0;
		AVPacket packet;
		while (av_read_frame(pFormatCtx, &packet) >= 0) {
			if (packet.stream_index == videoStream) {
				frame_count++;
			}
			av_packet_unref(&packet);
		}
	}
	int64_t duration_msec = static_cast<int64_t>(pFormatCtx->duration*1000.0 / AV_TIME_BASE);
	double fps = frame_count * 1000.0 / duration_msec;
	if (frame_count) {
		*pframe_count = frame_count;
	}
	if (pduration_msec) {
		*pduration_msec = duration_msec;
	}
	if (pfps) {
		*pfps = fps;
	}
	avformat_close_input(&pFormatCtx);
	avformat_free_context(pFormatCtx);
}
IMPLEMENT_DYNAMIC(CMainFrame, MSpringFrame)

BEGIN_MESSAGE_MAP(CMainFrame, MSpringFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_FILE_ADDVIDEO, &CMainFrame::OnFileOpenvideo)
	ON_COMMAND(ID_EXTRACT_SAVEASJPG, &CMainFrame::OnExtractAsJPG)
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_EXTRACT_SAVEASPNG, &CMainFrame::OnExtractAsPng)
	ON_COMMAND(ID_FILE_CLEARVIDEO, &CMainFrame::OnFileClear)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() {
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame() {
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (MSpringFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 프레임의 클라이언트 영역을 차지하는 뷰를 만듭니다.
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
						  CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL)) {
		TRACE0("뷰 창을 만들지 못했습니다.\n");
		return -1;
	}

	this->AddSysBtn(IDB_FRAME_CLOSE, MSpringFrame::ButtonEvent_Close);
	this->AddSysBtn(IDB_FRAME_MAXIMIZE, MSpringFrame::ButtonEvent_MaximizeWindow);
	this->AddSysBtn(IDB_FRAME_MINIMIZE, MSpringFrame::ButtonEvent_MinimizeWindow);
	this->SetIcon(IDR_MAINFRAME);


	this->SetTitle(TEXT("FSpring"));
	this->SetStyle(g_font_str,g_color_bk,g_color_text, g_color_border);

	m_menu_frame = new MSpringMenuFrame(this);
	m_menu_frame->SetStyle(g_font_str, g_color_bk, g_color_text, g_color_hover, g_color_bk);
	m_menu_frame->SetMenu(IDR_MAINFRAME);
	m_menu_frame->SetPosition(0);
	this->AddExpansionClass(m_menu_frame);

	//m_progress_frame = new ProgressExpansion(this);
	//this->AddExpansionClass(m_progress_frame);

	m_wndView.SetStyle(g_color_bk);
	m_wndView.m_view = new VideoListView(&m_wndView);
	this->SetWindowPos(nullptr, 100, 0, 1200, 600, SWP_NOMOVE);
#ifdef _DEBUG
	if (::AllocConsole() == TRUE) {
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		std::ios::sync_with_stdio();
	}
#endif
	g_status = Status::INIT;
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
	if (!MSpringFrame::PreCreateWindow(cs))
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
	MSpringFrame::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
	MSpringFrame::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/) {
	// 뷰 창으로 포커스를 이동합니다.
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
	// 뷰에서 첫째 크랙이 해당 명령에 나타나도록 합니다.
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 그렇지 않으면 기본 처리합니다.
	return MSpringFrame::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void CMainFrame::OnFileOpenvideo() {
	if (g_status == Status::RUN) {
		this->MessageBox(TEXT("Extractor is already running"), TEXT("warning"), MB_OK);
		return;
	}
	std::vector<CString>& files = g_files;
	const TCHAR* IMAGE_FILTER = TEXT("Video File(*.avi;*.mp4;*.mkv;*.mov)|*.avi;*.mp4;*.mkv;*.mov|AVI File(*.avi)|*.avi|MP4 File(*.mp4)|*.mp4|MKV File(*.mkv)|*.mkv|MOV File(*.mov)|*.mov|");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, IMAGE_FILTER);
	CString strFileList;
	const unsigned int c_cbBuffSize = (1000 * (MAX_PATH + 1)) + 1;
	dlg.GetOFN().lpstrFile = strFileList.GetBuffer(c_cbBuffSize);
	dlg.GetOFN().nMaxFile = c_cbBuffSize;
	if (IDOK == dlg.DoModal()) {
		std::set<CString> overlab_set;
		//make set
		for (auto&e : files) {
			overlab_set.insert(e);
		}
		for (POSITION pos = dlg.GetStartPosition(); pos != NULL;) {
			CString path = dlg.GetNextPathName(pos);
			if (overlab_set.find(path) == overlab_set.end()) {
				files.push_back(path);
			}
		}
		if (files.size() > 0) {
			g_status = Status::WAIT;
		}
		this->Invalidate();
	}
}
void CMainFrame::OnFileClear() {
	if (g_status == Status::RUN) {
		this->MessageBox(TEXT("Extractor is already running"), TEXT("warning"), MB_OK);
		return;
	}
	std::vector<CString>& files = g_files;
	files.clear();
	g_status = Status::INIT;
	this->Invalidate();
}

UINT Run(LPVOID param) {
	try {
		g_progress.store(0);
		g_files_progress.assign(g_files.size(), std::pair<int, int>(0, 0));
		g_status = Status::RUN;
		void** params = (void**)param;
		CMainFrame* wnd = dynamic_cast<CMainFrame*>((CWnd*)params[0]);
		char* extension = (char*)params[1];
		//Prepare
		g_total_frame_count = 0;
		g_prepare.store(false);
		std::vector<CString>& files = g_files;
		wnd->m_wndView.Invalidate();
		for (int i = 0; i<files.size(); i++) {
			CString cfile = files[i];
			int64_t frame_count;
			std::string file = mspring::String::ToString(cfile);
			GetVideoInfo(file, &frame_count);
			g_files_progress[i].second = frame_count;
			g_total_frame_count += frame_count;
		}
		g_prepare.store(true);
		//Run
		wnd->m_wndView.Invalidate();
		for (int i = 0; i < files.size(); i++) {
			CString cfile = files[i];
			std::string file = CT2CA((LPCWSTR)cfile);
			std::string folder = file.substr(0, file.find_last_of(".")) + ".frames";
			DeleteDirectory(folder, true);
		}
#pragma omp parallel for schedule(dynamic)
		for (int i=0;i<files.size();i++) {
			CString cfile = files[i];
			std::string file = CT2CA((LPCWSTR)cfile);
			std::string folder = file.substr(0, file.find_last_of(".")) + ".frames";
			DeleteDirectory(folder,true);
			do {
				//_mkdir(folder.c_str());
				CreateDirectoryA(folder.c_str(), 0);
				
				Sleep(1);
			} while (PathFolderExistsA(folder.c_str())==FALSE);
			cv::VideoCapture vc(file);
			if (vc.isOpened() == false) {
				continue;
			}
			cv::Mat frame;
			int n = 0;
			std::ostringstream oss;
			while (vc.read(frame)==true) {
				oss.str("");
				oss.width(8);
				oss.fill('0');
				oss << n;
				n++;
				std::string dst = folder + "\\" + oss.str() + "." + extension;
				cv::imwrite(dst, frame);	//imwrite has unknown error in windows7

				//IplImage*iplimage = &IplImage(frame);
				//cvSaveImage(dst.c_str(), iplimage);
				g_files_progress[i].first++;
				g_progress++;
				wnd->m_wndView.Invalidate();
				//wnd->SendMessage(WM_NCPAINT);
			}

		}
		g_status = Status::WAIT;
		wnd->m_wndView.Invalidate();
		wnd->MessageBox(TEXT("Complete!!"), TEXT("message"), MB_OK);
	} catch (std::exception& e) {
		::MessageBoxA(nullptr, e.what(), "exception", MB_OK);
	} catch (...) {
		::MessageBoxA(nullptr, "unknown exception", "exception", MB_OK);
	}
	return 0;
}
void CMainFrame::OnExtractAsJPG() {
	if (g_status == Status::INIT) {
		this->MessageBox(TEXT("No videos"), TEXT("warning"), MB_OK);
		return;
	}
	if (g_status == Status::RUN) {
		this->MessageBox(TEXT("Extractor is already running"), TEXT("warning"), MB_OK);
		return;
	}
	void* params[2] = { this,"jpg" };
#ifdef _DEBUG
	Run(params);
#else
	::AfxBeginThread(Run, (void*)params);
#endif
}
void CMainFrame::OnExtractAsPng() {
	if (g_status == Status::RUN) {
		this->MessageBox(TEXT("Extractor is already running"), TEXT("warning"), MB_OK);
		return;
	}
	if (g_status == Status::INIT) {
		this->MessageBox(TEXT("No videos"), TEXT("warning"), MB_OK);
		return;
	}
	void* params[2] = { this,"png" };
#ifdef _DEBUG
	Run(params);
#else
	::AfxBeginThread(Run, (void*)params);
#endif
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI) {
	lpMMI->ptMinTrackSize.x = 600;
	lpMMI->ptMinTrackSize.y = 300;
	MSpringFrame::OnGetMinMaxInfo(lpMMI);
}


void CMainFrame::OnDestroy() {
	MSpringFrame::OnDestroy();
#ifdef _DEBUG
	FreeConsole();
#endif
}
