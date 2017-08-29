#pragma once
#include "VirtualView.h"
#include"bmdl.h"
#include<Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

class ListView : public VirtualView {
public:
	ListView(CWnd* wnd);
	virtual ~ListView();
public:		//message method
	int OnCreate()override;
	void OnDestroy()override;
	void OnPaint(CDC* pDC)override;
	void OnSetFocus(CWnd* pOldWnd)override;
	void OnKillFocus(CWnd* pNewWnd)override;
	void OnLButtonDown(UINT nFlags, CPoint point)override;
	void OnLButtonUp(UINT nFlags, CPoint point)override;
	void OnLButtonDblClk(UINT nFlags, CPoint point)override;
	void OnRButtonUp(UINT nFlags, CPoint point)override;
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)override;
	void OnMouseMove(UINT nFlags, CPoint point)override;
	void OnMouseLeave()override;
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)override;
	void OnSize(UINT nType, int cx, int cy)override;
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)override;
	void OnTimer(UINT_PTR nIDEvent)override;
private:
	const UINT_PTR TIMER_SCROLLMOVE = 0x65C187C6;
protected:
	//Drag
	bool m_is_drag = false;
	CPoint m_drag_point;
	float m_prev_scroll_pos;
	
	int m_scrollbar_width = 20;
	int HEIGHT = 20;
	


	float m_scroll_pos = 0.0F;	//0~1;

	int GetViewHeight() {
		CRect rect;
		m_parent->GetClientRect(&rect);
		return rect.Height();
	}
	int GetPageHeight() {
		std::vector<CString>& files = VirtualView::Element::m_files;
		return files.size()*HEIGHT;
	}
	int GetThumbHeight() {
		int view_height=GetViewHeight();
		int page_height = GetPageHeight();
		return static_cast<int>(((float)view_height / page_height)*view_height);
	}
	void DrawScroll(CDC* pDC) {
		
		CRect rect;
		m_parent->GetClientRect(&rect);
		int view_height = GetViewHeight();
		int page_height = GetPageHeight();
		int thumb_height = GetThumbHeight();
		if (page_height > view_height) {
			CRect thumb_rect = rect;
			thumb_rect.top += static_cast<decltype(thumb_rect.top)>((view_height - thumb_height)*m_scroll_pos);
			
			thumb_rect.left = thumb_rect.right - m_scrollbar_width;
			thumb_rect.bottom = thumb_rect.top + thumb_height;



			CBrush brush;
			brush.CreateSolidBrush(VirtualView::Colors::m_color_thumb);
			CPen pen;
			pen.CreatePen(PS_NULL, 1, RGB(0, 0, 0));
			CPen* old_pen = pDC->SelectObject(&pen);
			CBrush* old_brush=pDC->SelectObject(&brush);

			pDC->RoundRect(&thumb_rect, CPoint(5, 5));

			pDC->SelectObject(old_pen);
			pDC->SelectObject(old_brush);
			brush.DeleteObject();
			pen.DeleteObject();
		}
	}
};

