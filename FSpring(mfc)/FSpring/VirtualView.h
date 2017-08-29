#pragma once
#include<vector>
#include<string>
#include<afxwin.h>
#include<atomic>
class VirtualView {
public:
	class Colors {
	public:
		static CString m_font_str;
		static COLORREF m_color_bk;
		static COLORREF m_color_text;
		static COLORREF m_color_hover;
		static COLORREF m_color_border;
		static COLORREF m_color_thumb;
	};
	class Element {
	public:
		static std::vector<CString> m_files;
		static int m_files_idx;
	};
	class Status {
	public: static enum EStatus {
			INIT, WAIT, RUN
		}m_estatus;
		static int64_t m_total_frame_count;
		static std::atomic_bool m_prepare;
		static std::atomic_int64_t m_progress;
	};
	CPoint GetMousePoint() {
		CPoint point;
		CRect rect;
		m_parent->GetWindowRect(&rect);
		::GetCursorPos(&point);
		point.x -= rect.left;
		point.y -= rect.top;
		return point;
	}
public:
	VirtualView(CWnd* wnd);
	virtual ~VirtualView();
public:
	CWnd* m_parent;
	CClientDC* m_cdc;
	CRect GetViewRect() {
		CRect rect;
		m_parent->GetClientRect(&rect);
		return rect;
	}
	virtual int OnCreate();
	virtual void OnDestroy();

	virtual void OnPaint(CDC* pDC);

	virtual void OnSetFocus(CWnd* pOldWnd);
	virtual void OnKillFocus(CWnd* pNewWnd);
	
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnMouseLeave();

	virtual BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual void OnSize(UINT nType, int cx, int cy);
	
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	virtual void OnTimer(UINT_PTR nIDEvent);
	//virtual void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//virtual void OnContextMenu(CWnd* pWnd, CPoint point);
	//virtual LONG OnComposition(UINT wParam, LONG lParam);
};

