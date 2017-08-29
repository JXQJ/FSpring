/*
*  BMDLFrame.cpp
*  Bom MFC Design Library
*
*  Created by Kimbom on 2017. 07. 06...
*  Copyright 2017 Sogang CVIP lab. All rights reserved.
*
*/
#include"stdafx.h"
#include "BMDLFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REDRAW_NCAREA this->OnNcPaint()

IMPLEMENT_DYNAMIC(BMDLFrame, CFrameWnd)

//constructor, destructor
BMDLFrame::BMDLFrame() {
	m_color_transparent = RGB(255, 255, 255);
	m_color_bk = RGB(83, 83, 83);
	m_color_border = RGB(0, 216, 255);
}
BMDLFrame::~BMDLFrame() {
}
//static
void BMDLFrame::ButtonEvent_Close(CWnd* wnd) {
	::AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
}
void BMDLFrame::ButtonEvent_MaximizeWindow(CWnd* wnd) {
	BMDLFrame* pmainframe = dynamic_cast<BMDLFrame*>(wnd);
	if (pmainframe->m_is_maximize == false) {
		static CRect rect_window;
		pmainframe->GetWindowRect(rect_window);
#ifdef _M_AMD64
		::SetWindowLongPtr(pmainframe->GetSafeHwnd(), GWLP_USERDATA, (LONG_PTR)&rect_window);
#else
		::SetWindowLongA(pmainframe->GetSafeHwnd(), GWL_USERDATA, (LONG)&rect_window);
#endif
		MONITORINFOEXA monitor;
		monitor.cbSize = sizeof(MONITORINFOEXA);
		CRect rect;
		wnd->GetWindowRect(&rect);
		HMONITOR hMOnitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
		GetMonitorInfoA(hMOnitor, &monitor);

		wnd->MoveWindow(&monitor.rcWork);
	} else {
#ifdef _M_AMD64
		CRect* rect_window = (CRect*)::GetWindowLongPtr(pmainframe->GetSafeHwnd(), GWLP_USERDATA);
#else
		CRect* rect_window = (CRect*)::GetWindowLongA(pmainframe->GetSafeHwnd(), GWL_USERDATA);
#endif
		pmainframe->MoveWindow(*rect_window);
	}
	pmainframe->m_is_maximize = !pmainframe->m_is_maximize;
	pmainframe->ReplaceSystemButtonState(SystemButton::State::Hover, SystemButton::State::Normal);
	pmainframe->ReplaceSystemButtonState(SystemButton::State::Click, SystemButton::State::Normal);
}
void BMDLFrame::ButtonEvent_MinimizeWindow(CWnd* wnd) {
	wnd->SendMessage(WM_SYSCOMMAND, (WPARAM)SC_MINIMIZE);
}
//helper
CPoint BMDLFrame::GetMousePoint() {
	CPoint point;
	CRect rect;
	this->GetWindowRect(&rect);
	::GetCursorPos(&point);
	point.x -= rect.left;
	point.y -= rect.top;
	return point;
}
CRect BMDLFrame::GetWindowCaptionRect() {
	CRect rect_caption = this->GetWindowNomalizedRect();
	rect_caption.top = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYFRAME);
	rect_caption.bottom = rect_caption.top + GetSystemMetrics(SM_CYCAPTION);
	return rect_caption;
}
CRect BMDLFrame::GetWindowNomalizedRect() {
	CRect rect;
	this->GetWindowRect(&rect);
	rect.NormalizeRect();
	rect.OffsetRect(-rect.left, -rect.top);
	return rect;
}

void BMDLFrame::ReplaceSystemButtonState(SystemButton::State curr, SystemButton::State val) {
	for (int i = 0; i < (int)this->m_sysbtn.size(); i++) {
		if (this->m_sysbtn[i].m_state == curr) {
			this->m_sysbtn[i].m_state = val;
		}
	}
}
void BMDLFrame::AddSysBtn(UINT bitmap, void(*EventFunc)(CWnd*)) {
	m_sysbtn.push_back(SystemButton(bitmap, EventFunc));
}
void BMDLFrame::SetStyle(COLORREF color_bk, COLORREF color_border, COLORREF color_transparent/*=RGB(255, 255, 255)*/) {
	m_color_bk = color_bk;
	m_color_border = color_border;
	m_color_transparent = color_transparent;
}
void BMDLFrame::SetIcon(UINT id) {
	m_icon.m_icon = ::AfxGetApp()->LoadIconW(id);
}
void BMDLFrame::AddExpansionClass(BMDLFrameExpansion* _class) {
	m_expansion.push_back(_class);
}
void BMDLFrame::SetSysBtnMargin(int margin) {
	m_sysbtn_margin = margin;
}
void BMDLFrame::SetIconMargin(int margin) {
	m_icon_margin = margin;
}
BEGIN_MESSAGE_MAP(BMDLFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_NCPAINT()
	ON_WM_NCACTIVATE()
	ON_WM_SIZE()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCMOUSELEAVE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()
int BMDLFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) {
		return -1;
	}
	this->ModifyStyle(WS_SYSMENU, 0);	//시스템 메뉴(종료,최대화,최소화) 버튼을 제거합니다.
	for (auto&e : m_expansion) {
		e->OnCreate(lpCreateStruct);
	}
	return 0;
}
BOOL BMDLFrame::PreCreateWindow(CREATESTRUCT& cs) {
	if (!CFrameWnd::PreCreateWindow(cs)) {
		return FALSE;
	}
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;	//테두리를 제거합니다.
	cs.lpszClass = AfxRegisterWndClass(0);
	if (cs.hMenu != NULL) {						//메뉴를 제거합니다.
		::DestroyMenu(cs.hMenu);
		cs.hMenu = NULL;
	}
	return TRUE;
}
void BMDLFrame::OnNcPaint() {
	CWindowDC _ncpaint(this);
	CWindowDC* ncpaint = &_ncpaint;
	
	CRect rect_window = this->GetWindowNomalizedRect();
	

	CRect rect_window_top = {
		rect_window.left
		,rect_window.top
		,rect_window.right
		,GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYSIZEFRAME)
	};
	CRect rect_window_bottom = {
		rect_window.left
		,rect_window.bottom - GetSystemMetrics(SM_CYFRAME) - GetSystemMetrics(SM_CYSIZEFRAME)
		,rect_window.right
		,rect_window.bottom
	};
	CRect rect_window_left = {
		rect_window.left
		,rect_window_top.bottom
		,GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXSIZEFRAME)
		,rect_window_bottom.top
	};
	CRect rect_window_right = {
		rect_window.right - GetSystemMetrics(SM_CXFRAME) - GetSystemMetrics(SM_CXSIZEFRAME)
		,rect_window_top.bottom
		,rect_window.right
		,rect_window_bottom.top
	};
	bmdl::DBB* dbb_top=new bmdl::DBB(ncpaint, rect_window_top);
	bmdl::DBB* dbb_bottom = new bmdl::DBB(ncpaint, rect_window_bottom);
	bmdl::DBB* dbb_left = new bmdl::DBB(ncpaint, rect_window_left);
	bmdl::DBB* dbb_right = new bmdl::DBB(ncpaint, rect_window_right);
	CBrush brush;
	brush.CreateSolidBrush(m_color_bk);
	dbb_top->getDC().FillRect(bmdl::DBB::Normalize(rect_window_top), &brush);
	dbb_bottom->getDC().FillRect(bmdl::DBB::Normalize(rect_window_bottom), &brush);
	dbb_left->getDC().FillRect(bmdl::DBB::Normalize(rect_window_left), &brush);
	dbb_right->getDC().FillRect(bmdl::DBB::Normalize(rect_window_right), &brush);
	brush.DeleteObject();
	//여기까지가 프레임 그리기.
	CPen pen, *old_pen;
	int thickness = 1;
	pen.CreatePen(PS_SOLID, thickness, m_color_border);
	old_pen=dbb_top->getDC().SelectObject(&pen);
	dbb_top->getDC().MoveTo(bmdl::DBB::Normalize(rect_window_top).left, bmdl::DBB::Normalize(rect_window_top).bottom);
	dbb_top->getDC().LineTo(bmdl::DBB::Normalize(rect_window_top).left, bmdl::DBB::Normalize(rect_window_top).top);
	dbb_top->getDC().LineTo(bmdl::DBB::Normalize(rect_window_top).right - 1 - thickness, bmdl::DBB::Normalize(rect_window_top).top);
	dbb_top->getDC().LineTo(bmdl::DBB::Normalize(rect_window_top).right - 1 - thickness, bmdl::DBB::Normalize(rect_window_top).bottom);
	dbb_top->getDC().SelectObject(old_pen);

	old_pen = dbb_bottom->getDC().SelectObject(&pen);
	//bottom-bottom
	dbb_bottom->getDC().MoveTo(bmdl::DBB::Normalize(rect_window_bottom).left, bmdl::DBB::Normalize(rect_window_bottom).top);
	dbb_bottom->getDC().LineTo(bmdl::DBB::Normalize(rect_window_bottom).left, bmdl::DBB::Normalize(rect_window_bottom).bottom-1-thickness);
	dbb_bottom->getDC().LineTo(bmdl::DBB::Normalize(rect_window_bottom).right - 1 - thickness, bmdl::DBB::Normalize(rect_window_bottom).bottom - 1 - thickness);
	dbb_bottom->getDC().LineTo(bmdl::DBB::Normalize(rect_window_bottom).right - 1 - thickness, bmdl::DBB::Normalize(rect_window_bottom).top-1);
	dbb_bottom->getDC().SelectObject(old_pen);

	old_pen = dbb_left->getDC().SelectObject(&pen);
	dbb_left->getDC().MoveTo(bmdl::DBB::Normalize(rect_window_left).left, bmdl::DBB::Normalize(rect_window_left).top);
	dbb_left->getDC().LineTo(bmdl::DBB::Normalize(rect_window_left).left, bmdl::DBB::Normalize(rect_window_left).bottom);
	dbb_left->getDC().SelectObject(old_pen);

	old_pen = dbb_right->getDC().SelectObject(&pen);
	dbb_right->getDC().MoveTo(bmdl::DBB::Normalize(rect_window_right).right - 1 - thickness, bmdl::DBB::Normalize(rect_window_right).top);
	dbb_right->getDC().LineTo(bmdl::DBB::Normalize(rect_window_right).right - 1 - thickness, bmdl::DBB::Normalize(rect_window_right).bottom);
	dbb_right->getDC().SelectObject(old_pen);

	pen.DeleteObject();
	//테두리 그리기.
	CSize btn_sz(rect_window_top.Height() - m_sysbtn_margin * 2, rect_window_top.Height() - m_sysbtn_margin * 2);
	CPoint btn_pt(rect_window.right - rect_window_right.Width() - btn_sz.cx,
				  rect_window.top + m_sysbtn_margin);
	CDC cdc;
	cdc.CreateCompatibleDC(ncpaint);
	for (size_t i = 0; i < m_sysbtn.size(); i++) {
		BITMAP bmp;
		CBitmap cbmp, *old_cbmp = nullptr;
		cbmp.LoadBitmap(m_sysbtn[i].m_bitmap_resource);
		cbmp.GetBitmap(&bmp);
		old_cbmp = cdc.SelectObject(&cbmp);
		dbb_top->getDC().TransparentBlt(btn_pt.x, btn_pt.y, btn_sz.cx, btn_sz.cy, &cdc
									   , bmp.bmHeight*static_cast<int>(this->m_sysbtn[i].m_state), 0
									   , bmp.bmHeight, bmp.bmHeight, m_color_transparent);
		m_sysbtn[i].m_rect = CRect(btn_pt.x, btn_pt.y, btn_pt.x + btn_sz.cx, btn_pt.y + btn_sz.cy);
		btn_pt.x -= btn_sz.cx + m_sysbtn_margin;
		cdc.SelectObject(old_cbmp);
	}
	if (m_icon.m_icon != nullptr) {
		m_icon.m_rect.left = m_icon_margin+rect_window_left.Width();
		m_icon.m_rect.top = m_icon_margin;
		m_icon.m_rect.right = rect_window_top.Height() - m_icon_margin + 10;
		m_icon.m_rect.bottom = rect_window_top.Height() - m_icon_margin;
		m_icon.m_rect.NormalizeRect();
		::DrawIconEx(dbb_top->getDC().GetSafeHdc()
					 , m_icon.m_rect.left,m_icon.m_rect.top
					 , m_icon.m_icon, m_icon.m_rect.Width(), m_icon.m_rect.Height()
					 , 0, NULL, DI_NORMAL);
	}
	m_dbb[0]=dbb_top;
	m_dbb[1]=(dbb_bottom);
	m_dbb[2]=(dbb_left);
	m_dbb[3]=(dbb_right);
	
	//아이콘 오른쪽 부터
	int begin_point = m_icon.m_rect.right + 10;
	//시스템 버튼의 왼쪽 까지
	int end_point = m_sysbtn.back().m_rect.left - 10;
	
	CRect iRect = this->GetWindowNomalizedRect();
	iRect.left = begin_point;
	iRect.right = end_point;
	iRect.bottom = iRect.top + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYSIZEFRAME);
	for (auto&e : m_expansion) {
		int last=e->OnNcPaint(m_dbb[0]->getPDC(),iRect);
		if (last >= 0) {
			iRect.left += last + 20;
		} else {
			iRect.right -= std::abs(last) + 20;
		}
	}
	m_blank_caption = iRect;
	//이제 모든 그림을 출력합니다.
	m_dbb[2]->Draw();	//left
	m_dbb[3]->Draw();	//right
	m_dbb[0]->Draw();	//top
	m_dbb[1]->Draw();	//bottom
	for (auto& e : m_dbb) {
		delete e;
		e = nullptr;
	}
}
BOOL BMDLFrame::OnNcActivate(BOOL bActive) {
	REDRAW_NCAREA;
	return TRUE;
}
void BMDLFrame::OnSize(UINT nType, int cx, int cy) {
	CFrameWnd::OnSize(nType, cx, cy);
	CRect rect_window;
	rect_window = this->GetWindowNomalizedRect();
	CRgn rgn;
	//윈도우를 깔끔한 사각형으로 변경합니다.
	rgn.CreateRoundRectRgn(0, 0, rect_window.Width(), rect_window.Height(), 0,0);
	this->SetWindowRgn(static_cast<HRGN>(rgn.GetSafeHandle()), TRUE);
	for (auto&e : m_expansion) {
		e->OnSize(nType, cx, cy);
	}
}
void BMDLFrame::OnNcLButtonDown(UINT nHitTest, CPoint point) {
	CPoint apoint = point;
	point = this->GetMousePoint();
	this->ReplaceSystemButtonState(SystemButton::State::Hover, SystemButton::State::Normal);
	for (int i = 0; i < (int)this->m_sysbtn.size(); i++) {
		if (this->m_sysbtn[i].m_rect.PtInRect(point) == TRUE) {
			this->m_sysbtn[i].m_state = SystemButton::State::Click;
			break;
		}
	}
	for (auto&e : m_expansion) {
		e->OnNcLButtonDown(nHitTest, point);
	}
	REDRAW_NCAREA;
	CFrameWnd::OnNcLButtonDown(nHitTest, apoint);
}
void BMDLFrame::OnNcMouseMove(UINT nHitTest, CPoint point) {
	CPoint apoint = point;
	point = this->GetMousePoint();
	//마우스 추적 활성화
	TRACKMOUSEEVENT tme = { 0 };
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
	tme.dwHoverTime = 0;
	TrackMouseEvent(&tme);
	this->ReplaceSystemButtonState(SystemButton::State::Hover, SystemButton::State::Normal);
	// 버튼 Hover
	bool is_btnclk = false;
	for (int i = 0; i < (int)this->m_sysbtn.size(); i++) {
		if (this->m_sysbtn[i].m_rect.PtInRect(point) == TRUE) {
			this->m_sysbtn[i].m_state = SystemButton::State::Hover;
			is_btnclk = true;
			break;
		}
	}
	CRect rect_caption = this->GetWindowCaptionRect();
	//버튼을 누른게 아니고 최대화 상태에서, 캡션을 잡고, 마우스 왼쪽 버튼으로 끌었을때
	//->크기가 원래대로 돌아가고 창 움직이는상태로.
	if (is_btnclk==false && m_is_maximize == true && rect_caption.PtInRect(point) == TRUE && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
		CPoint abs_point;
		::GetCursorPos(&abs_point);
		CRect rect;
		this->GetWindowRect(rect);
#ifdef _M_AMD64
		CRect* rect_window = (CRect*)::GetWindowLongPtr(this->GetSafeHwnd(), GWLP_USERDATA);
#else
		CRect* rect_window = (CRect*)::GetWindowLongA(this->GetSafeHwnd(), GWL_USERDATA);
#endif
		CRect relative_window = *rect_window;
		int w = relative_window.Width();
		int h = relative_window.Height();
		float xm = (float)point.x / rect.Width();
		float ym = (float)point.y / rect.Height();
		relative_window.left = static_cast<LONG>(abs_point.x - xm*w);
		relative_window.top = static_cast<LONG>(abs_point.y - ym*h);
		relative_window.right = relative_window.left + w;
		relative_window.bottom = relative_window.top + h;
		this->MoveWindow(relative_window);
		m_is_maximize = false;
		::DefWindowProcA(this->GetSafeHwnd(), WM_SYSCOMMAND, SC_MOVE + 1, MAKELPARAM(point.x, point.y));
	}
	for (auto&e : m_expansion) {
		e->OnNcMouseMove(nHitTest, point);
	}
	REDRAW_NCAREA;
	CFrameWnd::OnNcMouseMove(nHitTest, apoint);
}
void BMDLFrame::OnNcLButtonUp(UINT nHitTest, CPoint point) {
	auto it = std::find_if(m_sysbtn.begin(), m_sysbtn.end(), [](SystemButton& btn)->bool {return btn.m_state == SystemButton::State::Click; });
	if (it != m_sysbtn.end()) {
		it->Do(this);
	}
	this->ReplaceSystemButtonState(SystemButton::State::Click, SystemButton::State::Hover);
	for (auto&e : m_expansion) {
		e->OnNcLButtonUp(nHitTest, this->GetMousePoint());
	}
	REDRAW_NCAREA;
	CFrameWnd::OnNcLButtonUp(nHitTest, point);
}
LRESULT BMDLFrame::OnNcHitTest(CPoint point) {
	point = this->GetMousePoint();
	//시스템 버튼 클릭시 아무것도 안함.
	for (int i = 0; i < (int)this->m_sysbtn.size(); i++) {
		if (this->m_sysbtn[i].m_rect.PtInRect(point) == TRUE) {
			return HTBORDER;
		}
	}
	
	//최대화 상태일때 아무것도 안함.
	//캡션바 더블클릭 , 캡션바 잡고끌기 만 허용.
	if (m_is_maximize) {
		//최대화 상태일때 크기 변경을 하지 못하도록 합니다.
		return HTBORDER;
	}
	return CFrameWnd::OnNcHitTest(point);
}
void BMDLFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point) {
	if (m_blank_caption.PtInRect(this->GetMousePoint()) == TRUE) {
		this->ButtonEvent_MaximizeWindow(this);
	}
	for (auto&e : m_expansion) {
		e->OnNcLButtonDblClk(nHitTest, point);
	}
}
void BMDLFrame::OnNcMouseLeave() {
	this->ReplaceSystemButtonState(SystemButton::State::Click,SystemButton::State::Normal);
	this->ReplaceSystemButtonState(SystemButton::State::Hover, SystemButton::State::Normal);
	CFrameWnd::OnNcMouseLeave();
	for (auto&e : m_expansion) {
		e->OnNcMouseLeave();
	}
	REDRAW_NCAREA;
}
BOOL BMDLFrame::OnEraseBkgnd(CDC* pDC) {
	return TRUE;
}
void BMDLFrame::OnDestroy() {
	for (auto&e : m_expansion) {
		e->OnDestroy();
	}
	CFrameWnd::OnDestroy();
}
