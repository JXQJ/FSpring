
// ChildView.cpp : CChildView 클래스의 구현
//

#include "stdafx.h"
#include "FSpring.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SAFETY_CALL(VIEW,METHOD,...)	if(VIEW){(VIEW)->METHOD(__VA_ARGS__);}

// CChildView

CChildView::CChildView() {

	m_view = nullptr;
}

CChildView::~CChildView() {
}
BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) {
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
									   ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);
	return TRUE;
}
void CChildView::OnUpdate(CView* view, LPARAM lParam, CObject* co) {
	this->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
}
BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_NCACTIVATE()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CChildView::OnNcPaint() {
	//사각영역만 칠해야함.
	CWindowDC ncpaint(this);
	CRect rect_window;
	this->GetWindowRect(&rect_window);
	rect_window.NormalizeRect();
	rect_window.OffsetRect(-rect_window.left, -rect_window.top);
	CRect rect_top = rect_window;
	rect_top.bottom = rect_top.top + 3;
	CRect rect_bottom = rect_window;
	rect_bottom.top = rect_bottom.bottom - 3;
	CRect rect_left = rect_window;
	rect_left.right = rect_left.left + 3;
	CRect rect_right = rect_window;
	rect_right.left = rect_right.right - 3;
	//Fixed value
	CBrush brush;
	brush.CreateSolidBrush(VirtualView::Colors::m_color_bk);
	ncpaint.FillRect(rect_top, &brush);
	ncpaint.FillRect(rect_bottom, &brush);
	ncpaint.FillRect(rect_left, &brush);
	ncpaint.FillRect(rect_right, &brush);
	brush.DeleteObject();
}
BOOL CChildView::OnNcActivate(BOOL bActive) {
	this->OnNcPaint();
	return TRUE;
}
///VirtualView
void CChildView::OnPaint() {
	CPaintDC dc(this);
	CRect rect;
	this->GetClientRect(&rect);
	bmdl::DBB dbb(&dc, rect);
	if (VirtualView::Status::m_estatus == VirtualView::Status::EStatus::WAIT) {
		SAFETY_CALL(m_view, OnPaint, dbb.getPDC());
	} else if (VirtualView::Status::m_estatus == VirtualView::Status::EStatus::INIT) {
		CBrush brush_bk;
		brush_bk.CreateSolidBrush(VirtualView::Colors::m_color_bk);
		dbb.getDC().FillRect(&rect, &brush_bk);
		brush_bk.DeleteObject();
		CDC memDC;
		BITMAP bmpinfo;
		memDC.CreateCompatibleDC(dbb.getPDC());
		CBitmap bmp;
		CBitmap* oldbmp = nullptr;
		bmp.LoadBitmapW(IDB_FSPRING);
		bmp.GetBitmap(&bmpinfo);
		oldbmp = memDC.SelectObject(&bmp);
		int newWidth = 0;
		int newHeight = 0;
		if (bmpinfo.bmWidth != rect.Width() || bmpinfo.bmHeight != rect.Height()) {
			newWidth = bmpinfo.bmWidth*(rect.Height() / (double)bmpinfo.bmHeight);
			newHeight = rect.Height();
			if (newWidth > rect.Width()) {
				newWidth = rect.Width();
				newHeight = bmpinfo.bmHeight*(rect.Width() / (double)bmpinfo.bmWidth);
			}
		}
		dbb.getDC().StretchBlt((rect.Width() - newWidth) / 2, (rect.Height() - newHeight) / 2, newWidth, newHeight, &memDC, 0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, SRCCOPY);
		//dbb.getDC().BitBlt(0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight/2, &memDC, 0, 0, SRCCOPY);
	} else if (VirtualView::Status::m_estatus == VirtualView::Status::EStatus::RUN) {
		SAFETY_CALL(m_view, OnPaint, dbb.getPDC());
		CPen null_pen;
		null_pen.CreatePen(PS_NULL, 0, RGB(0, 0, 0));
		dbb.getDC().SelectObject(&null_pen);
		//Draw status window
		CBrush bk_brush;
		bk_brush.CreateSolidBrush(RGB(7, 54, 66));
		CBrush* old_brush = dbb.getDC().SelectObject(&bk_brush);
		int width = rect.Width();
		int height = rect.Height();
		rect.top += height*0.4;
		rect.bottom -= height*0.4;
		rect.left += width*0.2;
		rect.right -= width*0.2;
		dbb.getDC().RoundRect(rect, CPoint(10, 10));
		dbb.getDC().SelectObject(old_brush);
		//Draw Progressbar
		CBrush probk_brush;
		probk_brush.CreateSolidBrush(RGB(88, 110, 117));
		old_brush = dbb.getDC().SelectObject(&probk_brush);
		CRect progress_rect = rect;
		progress_rect.top += rect.Height()*0.1;
		progress_rect.bottom = progress_rect.top + rect.Height()*0.3;
		progress_rect.left = rect.left + rect.Width()*0.05;
		progress_rect.right = rect.right - rect.Width()*0.05;
		dbb.getDC().RoundRect(progress_rect, CPoint(10, 10));
		dbb.getDC().SelectObject(old_brush);
		//Draw progress thumb
		CBrush proth_brush;
		proth_brush.CreateSolidBrush(RGB(38, 139, 210));
		old_brush = dbb.getDC().SelectObject(&proth_brush);
		CRect thumb_rect = progress_rect;
		if (VirtualView::Status::m_progress != 0) {
			thumb_rect.right = thumb_rect.left + progress_rect.Width()*((float)VirtualView::Status::m_progress / VirtualView::Status::m_total_frame_count);
		} else {
			thumb_rect.right = thumb_rect.left;
		}
		dbb.getDC().RoundRect(thumb_rect, CPoint(10, 10));
		dbb.getDC().SelectObject(old_brush);
		//Draw status text
		CRect status_rect = rect;
		status_rect.bottom -= rect.Height()*0.1;
		status_rect.top = status_rect.bottom - rect.Height()*0.3;
		status_rect.left = rect.left + rect.Width()*0.05;
		status_rect.right = rect.right - rect.Width()*0.05;
		CString str;
		str.Format(TEXT("extract frames ... (%lld/%lld)"), VirtualView::Status::m_progress, VirtualView::Status::m_total_frame_count);
		int font_height = bmdl::MeasureFontHeight(TEXT("Arial"), status_rect.Height(), dbb.getPDC());
		printf("%d\n", font_height);
		font_height = bmdl::MeasureFontHeight2(TEXT("Arial"), str, font_height, status_rect.Width(), dbb.getPDC());
		printf("%d\n\n", font_height);
		CFont font;
		font.CreatePointFont(font_height, TEXT("Arial"));
		CFont* old_font = dbb.getDC().SelectObject(&font);
		dbb.getDC().SetTextColor(RGB(253, 246, 227));
		dbb.getDC().SetBkMode(TRANSPARENT);

		dbb.getDC().TextOutW(status_rect.left, status_rect.top, str);
		//dbb.getDC().RoundRect(status_rect, CPoint(10, 10));
		dbb.getDC().SelectObject(old_brush);


		dbb.getDC().SelectObject(old_font);
		font.DeleteObject();
		bk_brush.DeleteObject();
	}
	dbb.Draw();
}
int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CWnd::OnCreate(lpCreateStruct) == -1) {
		return -1;
	}

	return 0;
}
void CChildView::OnSetFocus(CWnd* pOldWnd) {
	CWnd::OnSetFocus(pOldWnd);
	SAFETY_CALL(m_view, OnSetFocus, pOldWnd);
}
void CChildView::OnKillFocus(CWnd* pNewWnd) {
	CWnd::OnKillFocus(pNewWnd);
	SAFETY_CALL(m_view, OnKillFocus, pNewWnd);
}
void CChildView::OnLButtonDown(UINT nFlags, CPoint point) {
	SAFETY_CALL(m_view, OnLButtonDown, nFlags, point);
	CWnd::OnLButtonDown(nFlags, point);
	this->Invalidate();
}
void CChildView::OnLButtonUp(UINT nFlags, CPoint point) {
	SAFETY_CALL(m_view, OnLButtonUp, nFlags, point);
	CWnd::OnLButtonUp(nFlags, point);
	this->Invalidate();
}
void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point) {
	SAFETY_CALL(m_view, OnLButtonDblClk, nFlags, point);
	CWnd::OnLButtonDblClk(nFlags, point);
}
BOOL CChildView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	SAFETY_CALL(m_view, OnSetCursor, pWnd, nHitTest, message);
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
void CChildView::OnSize(UINT nType, int cx, int cy) {
	CWnd::OnSize(nType, cx, cy);
	SAFETY_CALL(m_view, OnSize, nType, cx, cy);
}
void CChildView::OnMouseMove(UINT nFlags, CPoint point) {
	TRACKMOUSEEVENT tme = { 0 };
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = 0;
	TrackMouseEvent(&tme);
	SAFETY_CALL(m_view, OnMouseMove, nFlags, point);
	CWnd::OnMouseMove(nFlags, point);
	this->Invalidate();
}
void CChildView::OnMouseLeave() {
	SAFETY_CALL(m_view, OnMouseLeave);
	CWnd::OnMouseLeave();
	this->Invalidate();
}
BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	SAFETY_CALL(m_view, OnMouseWheel, nFlags, zDelta, pt);
	this->Invalidate();
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);

}
void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	SAFETY_CALL(m_view, OnKeyDown, nChar, nRepCnt, nFlags);
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CChildView::OnDestroy() {
	CWnd::OnDestroy();
	SAFETY_CALL(m_view, OnDestroy);
}
void CChildView::OnTimer(UINT_PTR nIDEvent) {
	SAFETY_CALL(m_view, OnTimer, nIDEvent);
	CWnd::OnTimer(nIDEvent);
	this->Invalidate();
}
void CChildView::OnRButtonUp(UINT nFlags, CPoint point) {
	SAFETY_CALL(m_view, OnRButtonUp, nFlags, point);
	CWnd::OnRButtonUp(nFlags, point);
}


BOOL CChildView::OnEraseBkgnd(CDC* pDC) {
	return TRUE;
}
