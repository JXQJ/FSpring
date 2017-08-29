/*
*  BMDLMenuFrame.h
*  Bom MFC Design Library
*
*  Created by Kimbom on 2017. 07. 08...
*  Copyright 2017 Sogang CVIP lab. All rights reserved.
*
*/
#if !defined(BMDL_7E1_07_08_BMDLMENUFRAME_H_INCLUDED)
#define BMDL_7E1_07_08_BMDLMENUFRAME_H_INCLUDED
#pragma once

#include"BMDLFrame.h"
#include"resource.h"
#include<deque>
#include <vector>

class BMDLMenuFrame : public BMDLFrameExpansion {
	/*
	*	@class : BMDLMenuFrame
	*	@comment : 이 클래스는 BMDLFrame에 사용될 확장 클래스 입니다.
	*				 , 커스텀 메뉴를 제공 합니다.
	*/
protected:
	// 이 클래스는 메뉴를 한번에 띄우지 않고 최상위 메뉴를 Caption에 그린뒤 알맞은 위치에 PopupMenu를 띄우는 방식으로 
	//구성 했기에, CString과 CMenu를 각각 가지고 있습니다.
	std::vector<std::pair<CString, CMenu*>> m_menu;
	std::deque<CRect> m_menu_rect;		//메뉴에 대한 영역 입니다.
	int m_menu_hover = -1;				//hover된 메뉴의 인덱스 입니다.
	CMenu* m_bmdl_menu = nullptr;		
	UINT m_position = 0;					//align 입니다. 0일경우 left-align, 그 이외의 경우 right-align
protected:		//style value
	CString m_font_str;					//메뉴에 사용될 폰트입니다.
	COLORREF m_color_text;				//글꼴의 색상입니다.
	COLORREF m_color_hover;				//Hover 된 메뉴의 배경 색 입니다.
	COLORREF m_color_bk;					//메뉴의 배경색 입니다.
public:
	void SetPosition(UINT left_is_zero_right_is_nonzero);
	void SetStyle(CString font, COLORREF bk, COLORREF text, COLORREF hover, COLORREF border);
	void SetMenu(UINT menu_id);
public:			//constructor,destructor
	BMDLMenuFrame(CWnd* wnd);
	virtual ~BMDLMenuFrame();
public:			//messageevent method
	void OnCreate(LPCREATESTRUCT lpCreateStruct)override;
	int OnNcPaint(CDC* pDC, CRect rect)override;
	void OnSize(UINT nType, int cx, int cy)override;
	void OnNcLButtonDown(UINT nHitTest, CPoint point) override;
	void OnNcMouseMove(UINT nHitTest, CPoint point) override;
	void OnNcLButtonUp(UINT nHitTest, CPoint point)override;
	void OnNcLButtonDblClk(UINT nHitTest, CPoint point)override;
	void OnNcMouseLeave() override;
	void OnDestroy() override;
};
#endif