/*
*  BMDLMenuFrame.cpp
*  Bom MFC Design Library
*
*  Created by Kimbom on 2017. 07. 08...
*  Copyright 2017 Sogang CVIP lab. All rights reserved.
*
*/
#include "stdafx.h"
#include "BMDLMenuFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


class BMDLMenu : public CMenu {
	/*
	*	@class : BMDLMenu
	*	@comment : 이 메뉴는 원하는 대로 색상을 바꿀 수 있는 커스텀 메뉴 입니다.
	*/
public:		//constructor,destructor
	BMDLMenu(CWnd* wnd);
	virtual ~BMDLMenu();
private:		//Inner class
	struct MenuObject {
		HICON m_hIcon;
		CString m_strCaption;
		BOOL bFirstMenu;
	};
	std::vector<DWORD_PTR> deleteItem;
	std::vector<DWORD_PTR> deleteMenu;
	//std::vector<MenuObject*> m_objects;
protected:
	CWnd* m_parent_wnd;
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)override;
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)override;
protected:	//static member
	const int MENU_HEIGHT = 16;
	static CString m_font_string;
	static COLORREF m_color_bk;
	static COLORREF m_color_text;
	static COLORREF m_color_hover;
	static COLORREF m_color_border;
public:
	void MakeItemsOwnDraw();
	static void SetStyle(CString font_string, COLORREF color_bk, COLORREF color_text, COLORREF color_hover, COLORREF color_border);
};
///static member
CString BMDLMenu::m_font_string = TEXT("Arial");
COLORREF BMDLMenu::m_color_bk = RGB(255, 255, 255);
COLORREF BMDLMenu::m_color_text = RGB(0, 0, 0);
COLORREF BMDLMenu::m_color_hover = RGB(199, 199, 199);
COLORREF BMDLMenu::m_color_border = RGB(103, 153, 255);
///constructor
BMDLMenu::BMDLMenu(CWnd* wnd) :CMenu() {
	m_parent_wnd = wnd;
}
///destructor
BMDLMenu::~BMDLMenu() {
	for (size_t i = 0; i < deleteItem.size(); i++) {
		delete ((MenuObject*)deleteItem[i]);
	}
	for (size_t i = 0; i < deleteMenu.size(); i++) {
		delete ((BMDLMenu*)deleteMenu[i]);
	}
}
///static metnod
void BMDLMenu::SetStyle(CString font_string, COLORREF color_bk, COLORREF color_text, COLORREF color_hover, COLORREF color_border) {
	BMDLMenu::m_font_string = font_string;
	BMDLMenu::m_color_bk = color_bk;
	BMDLMenu::m_color_text = color_text;
	BMDLMenu::m_color_hover = color_hover;
	BMDLMenu::m_color_border = color_border;
}
///method
void BMDLMenu::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	//CMenu::DrawItem(lpDrawItemStruct);
	CRect rect(lpDrawItemStruct->rcItem);
	COLORREF color_bk = BMDLMenu::m_color_bk;
	if (((MenuObject*)lpDrawItemStruct->itemData)->bFirstMenu) {
		//일반 상태
		color_bk = BMDLMenu::m_color_bk;
	}
	if ((lpDrawItemStruct->itemState & ODS_SELECTED) &&
		(lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) {
		//Hover 상태
		color_bk = BMDLMenu::m_color_hover;
	}
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	::FillRect(pDC->GetSafeHdc(), &rect, CreateSolidBrush(color_bk));
	CFont font;
	int h = bmdl::MeasureFontHeight(m_font_string, MENU_HEIGHT, pDC);
	font.CreatePointFont(h, m_font_string);
	CFont* old_font = pDC->SelectObject(&font);
	pDC->SetBkColor(color_bk);
	pDC->SetTextColor(BMDLMenu::m_color_text);
	pDC->TextOut(rect.left + 5, rect.top, ((MenuObject*)lpDrawItemStruct->itemData)->m_strCaption);
}

void BMDLMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) {
	//CMenu::MeasureItem(lpMeasureItemStruct);
	static CWindowDC dc(m_parent_wnd);
	lpMeasureItemStruct->itemHeight = MENU_HEIGHT;
	CFont font;
	int h = bmdl::MeasureFontHeight(m_font_string, lpMeasureItemStruct->itemHeight, &dc);
	font.CreatePointFont(h, m_font_string);
	CFont* old_font = dc.SelectObject(&font);
	CString str = ((MenuObject*)lpMeasureItemStruct->itemData)->m_strCaption;
	CSize sz;
	::GetTextExtentPoint32W(dc.GetSafeHdc(), str, str.GetLength(), &sz);

	lpMeasureItemStruct->itemWidth = sz.cx + 10 < 100 ? 100 : sz.cx + 10;
}

void BMDLMenu::MakeItemsOwnDraw() {
	//https://stackoverflow.com/questions/30353644/cmenu-border-color-on-mfc-solved
	MENUINFO MenuInfo = { 0 };
	MenuInfo.cbSize = sizeof(MENUINFO);
	GetMenuInfo(&MenuInfo);
	MenuInfo.hbrBack = ::CreateSolidBrush(BMDLMenu::m_color_border);
	MenuInfo.fMask = MIM_BACKGROUND | MIM_STYLE;
	MenuInfo.dwStyle = MIM_APPLYTOSUBMENUS;
	SetMenuInfo(&MenuInfo);

	int iMaxItems = GetMenuItemCount();
	for (int i = 0; i < iMaxItems; i++) {
		CString nameHolder;
		MenuObject* pObject = new MenuObject;
		//m_objects.push_back(pObject);
		deleteItem.push_back((DWORD_PTR)pObject);
		pObject->m_hIcon = NULL;
		pObject->bFirstMenu = TRUE;
		GetMenuString(i, pObject->m_strCaption, MF_BYPOSITION);
		MENUITEMINFO mInfo;
		ZeroMemory(&mInfo, sizeof(MENUITEMINFO));
		//I dont use GetMenuItemID because it doesn't return 0/-1 when it's a Popup (so the MSDN is wrong)
		UINT uID = mInfo.wID;
		uID = GetMenuItemID(i);

		ModifyMenuW(i, MF_BYPOSITION | MF_OWNERDRAW,
					uID, (TCHAR*)pObject);
		if (GetSubMenu(i)) {
			BMDLMenu* pSubMenu = new BMDLMenu(m_parent_wnd);
			deleteMenu.push_back((DWORD_PTR)pSubMenu);
			pSubMenu->Attach(GetSubMenu(i)->GetSafeHmenu());
			pSubMenu->MakeItemsOwnDraw();
		}
	}
}
///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
///@@@@@@@@@@BMDLMenuFrame@@@@@@@@@@
///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void BMDLMenuFrame::SetPosition(UINT left_is_zero_right_is_nonzero) {
	m_position = left_is_zero_right_is_nonzero;
}
void BMDLMenuFrame::SetStyle(CString font, COLORREF bk, COLORREF text, COLORREF hover, COLORREF border) {
	m_font_str = font;
	m_color_bk = bk;
	m_color_text = text;
	m_color_hover = hover;
	BMDLMenu::SetStyle(font, bk, text, hover, border);
}

void BMDLMenuFrame::SetMenu(UINT menu_id) {
	if (m_bmdl_menu == nullptr) {
		m_bmdl_menu = new BMDLMenu(m_wnd);
		m_bmdl_menu->LoadMenuW(menu_id);
		((BMDLMenu*)m_bmdl_menu)->MakeItemsOwnDraw();
		int c = m_bmdl_menu->GetMenuItemCount();
		for (int i = 0; i < c; i++) {
			CString str;
			m_bmdl_menu->GetMenuStringW(i, str, MF_BYPOSITION);
			m_bmdl_menu->ModifyMenu(i, MF_BYPOSITION | MF_OWNERDRAW, i, (LPCTSTR)m_bmdl_menu->m_hMenu);
			m_menu.push_back(std::make_pair(str, m_bmdl_menu));
		}
	}
}
///constructor, destructor
BMDLMenuFrame::BMDLMenuFrame(CWnd* wnd) :BMDLFrameExpansion(wnd) {
	m_font_str = TEXT("Arial");
	m_color_text = RGB(255, 255, 255);
	m_color_hover = RGB(255, 199, 199);
	m_color_bk = RGB(255, 255, 255);
}
BMDLMenuFrame::~BMDLMenuFrame() {
}
///Message Method
void BMDLMenuFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
}
int BMDLMenuFrame::OnNcPaint(CDC* pDC, CRect rect) {
	const int margin = 7;
	rect.top += margin;
	rect.bottom -= margin;
	CBrush brush;
	brush.CreateSolidBrush(m_color_bk);
	pDC->FillRect(rect, &brush);
	int h = bmdl::MeasureFontHeight(m_font_str, rect.Height(), pDC);
	CFont font;
	font.CreatePointFont(h, m_font_str);
	CFont* old_font = pDC->SelectObject(&font);
	pDC->SetBkColor(m_color_bk);
	pDC->SetTextColor(m_color_text);
	m_menu_rect.clear();
	int posx = 0;
	int ret = 0;
	if (m_position == 0) {	//set left
		posx = rect.left;
		for (int i = 0; i < (int)m_menu.size(); i++) {
			CString str = m_menu[i].first;
			CSize sz;
			::GetTextExtentPoint32W(pDC->GetSafeHdc(), str, str.GetLength(), &sz);
			m_menu_rect.push_back(CRect(posx, rect.top, posx + sz.cx + margin, rect.bottom));
			if (i == m_menu_hover) {
				CBrush brush;
				brush.CreateSolidBrush(m_color_hover);
				pDC->FillRect(m_menu_rect.back(), &brush);
				brush.DeleteObject();
			}
			pDC->SetBkMode(TRANSPARENT);
			pDC->TextOutW(posx + margin / 2, margin, str, str.GetLength());
			posx += sz.cx + margin;
			if (posx > rect.right) {
				break;
			}
		}
		ret = posx - rect.left;
	} else {
		posx = rect.right;
		for (int i = (int)m_menu.size() - 1; i >= 0; i--) {
			CString str = m_menu[i].first;
			CSize sz;
			::GetTextExtentPoint32W(pDC->GetSafeHdc(), str, str.GetLength(), &sz);
			m_menu_rect.push_front(CRect(posx - sz.cx - margin, rect.top, posx, rect.bottom));
			if (i == m_menu_hover) {
				CBrush brush;
				brush.CreateSolidBrush(m_color_hover);
				pDC->FillRect(m_menu_rect.back(), &brush);
				brush.DeleteObject();
			}
			pDC->SetBkMode(TRANSPARENT);
			pDC->TextOutW(posx - sz.cx - margin, margin, str, str.GetLength());
			posx -= sz.cx + margin;
			if (posx < rect.left) {
				break;
			}
		}
		ret = -(rect.right - posx);
	}
	pDC->SelectObject(old_font);
	brush.DeleteObject();
	font.DeleteObject();
	return ret;
}
void BMDLMenuFrame::OnSize(UINT nType, int cx, int cy) {
}
void BMDLMenuFrame::OnNcMouseMove(UINT nHitTest, CPoint point) {
	m_menu_hover = -1;
	for (int i = 0; i < (int)m_menu_rect.size(); i++) {
		if (m_menu_rect[i].PtInRect(point) == TRUE) {
			m_menu_hover = i;
			break;
		}
	}
}
void BMDLMenuFrame::OnNcLButtonUp(UINT nHitTest, CPoint point) {
}
void BMDLMenuFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point) {
}
void BMDLMenuFrame::OnDestroy() {
	delete m_bmdl_menu;
}
void BMDLMenuFrame::OnNcLButtonDown(UINT nHitTest, CPoint point) {
	CPoint apoint;
	::GetCursorPos(&apoint);
	for (int i = 0; i < (int)m_menu_rect.size(); i++) {
		if (m_menu_rect[i].PtInRect(point) == TRUE) {
			int rVal = (int)m_menu[i].second->GetSubMenu(i)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD
																			, m_menu_rect[i].left + apoint.x - point.x
																			, m_menu_rect[i].bottom + apoint.y - point.y
																			, m_wnd);
			if (rVal != 0) {
				m_wnd->PostMessageW(WM_COMMAND, rVal, 0);
			}
		}
	}
}
void BMDLMenuFrame::OnNcMouseLeave() {
	m_menu_hover = -1;
}

