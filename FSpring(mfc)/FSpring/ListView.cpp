#include "stdafx.h"
#include "ListView.h"


ListView::ListView(CWnd* wnd) :VirtualView(wnd) {

}


ListView::~ListView() {
}
///@@@
int ListView::OnCreate() {



	return TRUE;
}
void ListView::OnDestroy() {

}
void ListView::OnPaint(CDC* pDC) {

	std::vector<CString>& files = VirtualView::Element::m_files;
	CRect rect = this->GetViewRect();
	CBrush brush_bk;
	brush_bk.CreateSolidBrush(VirtualView::Colors::m_color_bk);
	pDC->FillRect(&rect,&brush_bk );
	brush_bk.DeleteObject();
	CFont font;
	int h = bmdl::MeasureFontHeight(VirtualView::Colors::m_font_str, HEIGHT, pDC);
	font.CreatePointFont(h, VirtualView::Colors::m_font_str);
	CFont* old_font = pDC->SelectObject(&font);

	int view_height = GetViewHeight();
	int page_height = GetPageHeight();

	int page_jump = 0;	//띄워야 하는 좌표
	if (page_height > view_height) {
		page_jump = static_cast<int>((page_height - view_height)*m_scroll_pos);
	}
	int page_y = 0;		//페이지가 시작될 좌표.
	int page_idx = 0;	//시작될 줄 넘버
	if (page_jump > 0) {
		div_t div = std::div(page_jump, HEIGHT);
		page_y = -div.rem;
		page_idx = div.quot;
	}
	CPen pen_null;
	pen_null.CreatePen(PS_NULL, 0, RGB(0, 0, 0));

	pDC->SetBkMode(TRANSPARENT);
	while (page_idx < (int)files.size() && page_y < view_height) {
		CString tagfile = files[page_idx];
		int dot = tagfile.ReverseFind(TEXT('.'));
		tagfile = tagfile.Left(dot) + TEXT(".frames");
		CRect rect_is_tag = {
			0
			,page_y
			,HEIGHT
			,page_y + HEIGHT
		};
		//draw checkbox
		CBrush brush_check;
		CPen pen_check;
		int pen_check_weight = HEIGHT / 10;
		pen_check.CreatePen(PS_SOLID, pen_check_weight, VirtualView::Colors::m_color_bk);
		if (::PathFileExistsW(tagfile) == TRUE) {
			brush_check.CreateSolidBrush(VirtualView::Colors::m_color_thumb);
		} else {
			brush_check.CreateSolidBrush(RGB(88,110,117));
		}
		CBrush* old_brush = pDC->SelectObject(&brush_check);
		CPen* old_pen = pDC->SelectObject(&pen_null);
		pDC->RoundRect(&rect_is_tag, CPoint(5, 5));
		pDC->SelectObject(old_pen);
		old_pen = pDC->SelectObject(&pen_check);
		int divw = HEIGHT / 5;
		pDC->MoveTo(rect_is_tag.left+divw, static_cast<int>(rect_is_tag.top+divw*2.5));
		pDC->LineTo(static_cast<int>(rect_is_tag.left+divw*1.5), rect_is_tag.top+divw*4);
		pDC->LineTo(rect_is_tag.right - divw, rect_is_tag.top + divw * 1);
		pDC->SelectObject(old_pen);
		pDC->SelectObject(old_brush);
		brush_check.DeleteObject();
		pen_check.DeleteObject();
		//draw file path
		
		pDC->TextOutW(rect_is_tag.right, page_y, files[page_idx]);
		//pDC->TextOutW(0, page_y, files[page_idx]);


		page_idx++;
		page_y += HEIGHT;
	}
	DrawScroll(pDC);

	pen_null.DeleteObject();

	font.DeleteObject();
}
void ListView::OnSetFocus(CWnd* pOldWnd) {
}
void ListView::OnKillFocus(CWnd* pNewWnd) {
}
void ListView::OnLButtonDown(UINT nFlags, CPoint point) {
	m_is_drag = true;
	m_prev_scroll_pos = m_scroll_pos;
	m_drag_point = point;
}
void ListView::OnLButtonUp(UINT nFlags, CPoint point) {
	if (m_is_drag == true) {
		m_parent->KillTimer(TIMER_SCROLLMOVE);
		m_is_drag = false;
	}
}
void ListView::OnLButtonDblClk(UINT nFlags, CPoint point) {
}
void ListView::OnRButtonUp(UINT nFlags, CPoint point) {
}
BOOL ListView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	if ((GetKeyState(VK_CONTROL) & 0x8000) == 0x8000) {
		//Ctrl키를 누르고 휠을 돌리면 확대 옵션.
		HEIGHT += (zDelta > 0)?1:-1;
		bmdl::SetRange(HEIGHT, 10, 50);
	}else if ((GetKeyState(VK_SHIFT) & 0x8000) == 0x8000) {
		m_scroll_pos += 0.01F*((zDelta > 0) ? -1 : 1);
		bmdl::SetRange(m_scroll_pos, 0.0F, 1.0F);
		
	} else {
		int view_height = GetViewHeight();
		int thumb_height = GetThumbHeight();
		int page_height = GetPageHeight();
		int page_jump = 0;	//띄워야 하는 좌표
		if (page_height > view_height) {
			page_jump = static_cast<int>((page_height - view_height)*m_scroll_pos);
		}
		int page_idx = 0;	//시작될 줄 넘버
		if (page_jump > 0) {
			page_idx = static_cast<int>(std::round((float)page_jump / HEIGHT));
		}
		page_idx += ((zDelta > 0) ? -1 : 1);
		m_scroll_pos = ((float)page_idx*HEIGHT) / (page_height - view_height);
		bmdl::SetRange(m_scroll_pos, 0.0F, 1.0F);
	}
	return TRUE;
}
void ListView::OnMouseMove(UINT nFlags, CPoint point) {

	if (m_is_drag == true) {
		int view_height = GetViewHeight();
		int page_height = GetPageHeight();
		int thumb_height = GetThumbHeight();
		int drag_height = point.y - m_drag_point.y;
		m_scroll_pos = m_prev_scroll_pos + ((float)drag_height / (view_height - thumb_height));
		bmdl::SetRange(m_scroll_pos, 0.0F, 1.0F);
	}
}
void ListView::OnMouseLeave() {

	if (m_is_drag == true) {
		m_parent->SetTimer(TIMER_SCROLLMOVE, 10, nullptr);
	}
}
BOOL ListView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	return TRUE;
}
void ListView::OnSize(UINT nType, int cx, int cy) {
}
void ListView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
}
void ListView::OnTimer(UINT_PTR nIDEvent) {

	if (nIDEvent == TIMER_SCROLLMOVE) {
		CPoint point = this->GetMousePoint();
		CRect rect = this->GetViewRect();
		//두 조건은 합쳐질수없음
		if (rect.PtInRect(point) == TRUE) {
			m_parent->KillTimer(TIMER_SCROLLMOVE);
			return;
		}
		if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == false) {
			m_is_drag = false;
			m_parent->KillTimer(TIMER_SCROLLMOVE);
			return;
		}
		int view_height = GetViewHeight();
		int page_height = GetPageHeight();
		int thumb_height = GetThumbHeight();
		int drag_height = point.y - m_drag_point.y;
		m_scroll_pos = m_prev_scroll_pos + ((float)drag_height / (view_height - thumb_height));
		m_scroll_pos = bmdl::Max(m_scroll_pos, 0.0F);
		m_scroll_pos = bmdl::Min(m_scroll_pos, 1.0F);
	}
}