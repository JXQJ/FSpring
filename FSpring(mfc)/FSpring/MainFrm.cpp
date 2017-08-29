
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "FSpring.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
void GetVideoInfo(std::string video_file, int64_t* pframe_count = nullptr, int64_t* pduration_msec = nullptr, double* pfps = nullptr) {
	bool ret = true;
	av_register_all();
	AVFormatContext* pFormatCtx = nullptr;
	avformat_open_input(&pFormatCtx, video_file.c_str(), NULL, NULL);
	avformat_find_stream_info(pFormatCtx, NULL);
	int videoStream = -1;	//Find video Stream
	for (int i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
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
			av_free_packet(&packet);
		}
	}
	int64_t duration_msec = pFormatCtx->duration*1000.0 / AV_TIME_BASE;
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
IMPLEMENT_DYNAMIC(CMainFrame, BMDLFrame)

BEGIN_MESSAGE_MAP(CMainFrame, BMDLFrame)
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
	if (BMDLFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 프레임의 클라이언트 영역을 차지하는 뷰를 만듭니다.
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
						  CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL)) {
		TRACE0("뷰 창을 만들지 못했습니다.\n");
		return -1;
	}

	this->AddSysBtn(IDB_FRAME_CLOSE, BMDLFrame::ButtonEvent_Close);
	this->AddSysBtn(IDB_FRAME_MAXIMIZE, BMDLFrame::ButtonEvent_MaximizeWindow);
	this->AddSysBtn(IDB_FRAME_MINIMIZE, BMDLFrame::ButtonEvent_MinimizeWindow);
	this->SetIcon(IDR_MAINFRAME);
	CString font = TEXT("Arial");
	COLORREF color_bk = RGB(253, 246, 227);
	COLORREF color_hover = RGB(38, 139, 210);
	COLORREF color_text = RGB(7, 54, 66);
	COLORREF color_border = RGB(108, 113, 196);


	this->SetStyle(color_bk, color_border);
	VirtualView::Colors::m_color_bk = color_bk;
	VirtualView::Colors::m_color_hover = color_hover;
	VirtualView::Colors::m_color_text = color_text;
	VirtualView::Colors::m_color_border = color_border;
	VirtualView::Colors::m_color_thumb = RGB(38, 139, 210);

	m_menu_frame = new BMDLMenuFrame(this);
	m_menu_frame->SetStyle(font, color_bk, color_text, color_hover, color_bk);
	m_menu_frame->SetMenu(IDR_MAINFRAME);
	m_menu_frame->SetPosition(0);
	this->AddExpansionClass(m_menu_frame);


	m_wndView.m_view = new ListView(&m_wndView);
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
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
	if (!BMDLFrame::PreCreateWindow(cs))
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
	BMDLFrame::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
	BMDLFrame::Dump(dc);
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
	return BMDLFrame::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void CMainFrame::OnFileOpenvideo() {
	if (VirtualView::Status::m_estatus == VirtualView::Status::EStatus::RUN) {
		this->MessageBox(TEXT("Extractor is already running"), TEXT("warning"), MB_OK);
		return;
	}
	std::vector<CString>& files = VirtualView::Element::m_files;
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
			VirtualView::Status::m_estatus = VirtualView::Status::EStatus::WAIT;
		}
		this->Invalidate();
	}
}
void CMainFrame::OnFileClear() {
	if (VirtualView::Status::m_estatus == VirtualView::Status::EStatus::RUN) {
		this->MessageBox(TEXT("Extractor is already running"), TEXT("warning"), MB_OK);
		return;
	}
	std::vector<CString>& files = VirtualView::Element::m_files;
	files.clear();
	VirtualView::Status::m_estatus = VirtualView::Status::EStatus::INIT;
	this->Invalidate();
}

UINT Run(LPVOID param) {
	try {
		VirtualView::Status::m_estatus = VirtualView::Status::EStatus::RUN;
		void** params = (void**)param;
		CMainFrame* wnd = dynamic_cast<CMainFrame*>((CWnd*)params[0]);
		char* extension = (char*)params[1];
		//Prepare
		VirtualView::Status::m_total_frame_count = 0;
		VirtualView::Status::m_prepare.store(false);
		std::vector<CString>& files = VirtualView::Element::m_files;
		wnd->m_wndView.Invalidate();
		for (auto&cfile : files) {
			int64_t frame_count;
			std::string file = CT2CA((LPCWSTR)cfile);
			GetVideoInfo(file, &frame_count);
			VirtualView::Status::m_total_frame_count += frame_count;
		}
		VirtualView::Status::m_prepare.store(true);
		//Run
		wnd->m_wndView.Invalidate();
		VirtualView::Status::m_progress.store(0);
		for (auto&cfile : files) {
			std::string file = CT2CA((LPCWSTR)cfile);
			std::string folder = file.substr(0, file.find_last_of(".")) + ".frames";
			scvl::File::DeleteDirectory(folder);
			Sleep(100);
			_mkdir(folder.c_str());
			//CreateDirectoryA(folder.c_str(), nullptr);
			cv::VideoCapture vc(file);
			if (vc.isOpened() == false) {
				continue;
			}
			cv::Mat frame;
			int n = 0;
			std::ostringstream oss;
			while (true) {
				vc >> frame;
				if (frame.empty() == true) {
					break;
				}
				oss.str("");
				oss.width(8);
				oss.fill('0');
				oss << n;
				n++;
				std::string dst = folder + "\\" + oss.str() + "." + extension;
				//cv::imwrite(dst, frame);	//imwrite has unknown error in windows7

				IplImage*iplimage = &IplImage(frame);
				cvSaveImage(dst.c_str(), iplimage);
				

				VirtualView::Status::m_progress++;
				wnd->m_wndView.Invalidate();
			}

		}
		VirtualView::Status::m_estatus = VirtualView::Status::EStatus::WAIT;
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
	if (VirtualView::Status::m_estatus == VirtualView::Status::EStatus::INIT) {
		this->MessageBox(TEXT("No videos"), TEXT("warning"), MB_OK);
		return;
	}
	if (VirtualView::Status::m_estatus == VirtualView::Status::EStatus::RUN) {
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
	if (VirtualView::Status::m_estatus == VirtualView::Status::EStatus::RUN) {
		this->MessageBox(TEXT("Extractor is already running"), TEXT("warning"), MB_OK);
		return;
	}
	if (VirtualView::Status::m_estatus == VirtualView::Status::EStatus::INIT) {
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	lpMMI->ptMinTrackSize.x = 600;
	lpMMI->ptMinTrackSize.y = 300;
	BMDLFrame::OnGetMinMaxInfo(lpMMI);
}








void CMainFrame::OnDestroy() {
	BMDLFrame::OnDestroy();
#ifdef _DEBUG
	FreeConsole();
#endif
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
