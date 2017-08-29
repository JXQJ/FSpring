#include "stdafx.h"
#include "VirtualView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CString VirtualView::Colors::m_font_str = TEXT("Arial");
COLORREF VirtualView::Colors::m_color_bk = RGB(0, 0, 0);
COLORREF VirtualView::Colors::m_color_text = RGB(0, 0, 0);
COLORREF VirtualView::Colors::m_color_hover = RGB(0, 0, 0);
COLORREF VirtualView::Colors::m_color_border = RGB(0, 0, 0);
COLORREF VirtualView::Colors::m_color_thumb = RGB(0, 0, 0);
std::vector<CString> VirtualView::Element::m_files;
int VirtualView::Element::m_files_idx = -1;

VirtualView::Status::EStatus VirtualView::Status::m_estatus = INIT;
std::atomic_bool VirtualView::Status::m_prepare = false;
std::atomic_int64_t VirtualView::Status::m_progress = 0;
int64_t VirtualView::Status::m_total_frame_count = 0;

VirtualView::VirtualView(CWnd* wnd) {
	this->m_parent = wnd;
}
VirtualView::~VirtualView() {
}
int VirtualView::OnCreate() {
	this->m_cdc = new CClientDC(this->m_parent);
	return 0;
}
void VirtualView::OnDestroy() {
	if (this->m_cdc != nullptr) {
		delete this->m_cdc;
	}
}

void VirtualView::OnPaint(CDC* pDC) {
}
void VirtualView::OnSetFocus(CWnd* pOldWnd) {
}
void VirtualView::OnKillFocus(CWnd* pNewWnd) {
}


void VirtualView::OnLButtonDown(UINT nFlags, CPoint point) {
}
void VirtualView::OnLButtonUp(UINT nFlags, CPoint point) {
}
void VirtualView::OnLButtonDblClk(UINT nFlags, CPoint point) {
}
void VirtualView::OnRButtonUp(UINT nFlags, CPoint point) {
}
BOOL VirtualView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	return TRUE;
}
void VirtualView::OnMouseMove(UINT nFlags, CPoint point) {
}
void VirtualView::OnMouseLeave() {
}
BOOL VirtualView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	return TRUE;
}

void VirtualView::OnSize(UINT nType, int cx, int cy) {
}

void VirtualView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
}

void VirtualView::OnTimer(UINT_PTR nIDEvent) {

}
//void VirtualView::OnContextMenu(CWnd* pWnd, CPoint point) {
//}

//void VirtualView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
//}
//LONG VirtualView::OnComposition(UINT wParam, LONG lParam) {
//	return 1;
//}
