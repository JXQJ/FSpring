/*
*  BMDLFrame.h
*  Bom MFC Design Library
*
*  Created by Kimbom on 2017. 07. 06...
*  Copyright 2017 Sogang CVIP lab. All rights reserved.
*
*/
#if !defined(BMDL_7E1_07_06_BMDLFRAME_H_INCLUDED)
#define BMDL_7E1_07_06_BMDLFRAME_H_INCLUDED
#pragma once
#include<afxwin.h>
#include<vector>
#include<iostream>
#include<algorithm>
#include<string>
#include<array>
#include<map>
#include"bmdl.h"

class BMDLFrameExpansion {
	/*
	*	@class : BMDLFrameExpansion
	*	@comment : 이 클래스는 BMDLFrame에 확장 기능을 부여하기 위해 사용됩니다.
	*				 , 이 클래스를 상속 받아 각 메세지 콜백 함수에 대한 처리기를 만드십시오.
	*	@warning : OnNcPaint의 경우 반환값과 매개변수가 원래의 OnNcPaint와 다릅니다 .유의 하십시오.
	*/
protected:
	CWnd* m_wnd;
public:
	BMDLFrameExpansion(CWnd* wnd) :m_wnd(wnd){}
	virtual void OnCreate(LPCREATESTRUCT lpCreateStruct) = 0;
	/*
	*	@method : OnNcPaint
	*	@param
	*		* pDC : 윈도우 캡션에 그림을 그릴수 있는 Device Context 입니다.
	*		* rect : 이 함수가 그릴수 있는(그려야 하는) 영역을 나타냅니다.
	*	@return : 주어진 영역에서 사용한 만큼의 x너비를 반환해야 합니다. 양수의 경우 left-align, 음수의 경우 right-align 입니다.
	*/
	virtual int OnNcPaint(CDC* pDC,CRect rect) = 0;
	virtual void OnSize(UINT nType, int cx, int cy) = 0;
	//아래 마우스 이벤트 함수들의 매개변수 point 는 윈도우에 대한 상대 좌표가 입력 됩니다.
	virtual void OnNcLButtonDown(UINT nHitTest, CPoint point) = 0;	
	virtual void OnNcMouseMove(UINT nHitTest, CPoint point) = 0;
	virtual void OnNcLButtonUp(UINT nHitTest, CPoint point) = 0;
	virtual void OnNcLButtonDblClk(UINT nHitTest, CPoint point) = 0;
	virtual void OnNcMouseLeave() = 0;
	virtual void OnDestroy() = 0;
};
class BMDLFrame : public CFrameWnd {
protected:
	DECLARE_DYNAMIC(BMDLFrame)
public://constructor,destructor
	BMDLFrame();
	virtual ~BMDLFrame();
private://inner class
	class SystemButton {
		/*
		*	@class : SystemButton
		*	@comment : 시스템 버튼을 나타내는 클래스 입니다.
		*/
	public:
		enum State {Normal,Hover,Click};
		CRect m_rect;					//시스템 버튼이 차지하는 영역입니다.
		State m_state;					//시스템 버튼의 상태입니다.
		UINT m_bitmap_resource = 0;	//시스템 버튼에 사용되는 리소스의 ID 입니다.
	private:
		void(*Event)(CWnd* frame) = nullptr;	//시스템 버튼을 클릭했을때 실행되는 함수입니다.
	public:
		void Do(CWnd* param = nullptr) {
			if (Event != nullptr) {
				Event(param);
			}
		}
		SystemButton(UINT r, void(*EventFunction)(CWnd*) = nullptr) {
			m_bitmap_resource = r;
			m_state = State::Normal;
			Event = EventFunction;
		}
	};
	struct AppIcon {
		/*
		*	@class : AppIcon
		*	@comment : Application의 아이콘을 나타냅니다.
		*/
		HICON m_icon = nullptr;
		CRect m_rect;
	};
	std::vector<SystemButton> m_sysbtn;		//시스템 버튼
	void ReplaceSystemButtonState(SystemButton::State curr, SystemButton::State val);
protected:
	AppIcon m_icon;								//앱 아이콘
protected:	
	std::array<bmdl::DBB*,4> m_dbb;			//상,하,좌,우 를 각각 그리는 Device Context 입니다.
	bool m_is_maximize = false;				//현재 창이 최대화 상태인지 나타냅니다.
	std::vector<BMDLFrameExpansion*> m_expansion;	//확장 기능 클래스 입니다.
	CRect m_blank_caption;						//OnNcDblClk 함수에서 사용합니다.
protected:	//coord value
	int m_sysbtn_margin = 4;					//시스템 버튼 사이의 간격입니다.
	int m_icon_margin = 4;						//아이콘의 margin 입니다.
protected:	//style value
	COLORREF m_color_transparent;			//시스템버튼의 투명색을 지정합니다.
	COLORREF m_color_bk;						//프레임의 배경색입니다.
	COLORREF m_color_border;					//경계색의 색상입니다.
public:		//static method
	static void ButtonEvent_Close(CWnd* wnd);
	static void ButtonEvent_MaximizeWindow(CWnd* wnd);
	static void ButtonEvent_MinimizeWindow(CWnd* wnd);
protected:		//Mouse, Window-size method
	CPoint GetMousePoint();						//현재 마우스의 상대적 위치를 가져옵니다.
	CRect GetWindowCaptionRect();				//윈도우 캡션의 상대적 영역을 가져옵니다.
	CRect GetWindowNomalizedRect();			//윈도우의 상대적 영역을 가져옵니다.
public:	
	void SetSysBtnMargin(int margin);			//시스템버튼의 마진을 지정합니다.
	void SetIconMargin(int margin);			//아이콘의 마진을 지정합니다.
	void SetIcon(UINT id);						//아이콘을 지정합니다.
	//스타일을 설정합니다.
	void SetStyle(COLORREF color_bk, COLORREF color_border, COLORREF color_transparent = RGB(255, 255, 255));
	void AddSysBtn(UINT bitmap, void(*EventFunc)(CWnd*));	//시스템버튼을 추가합니다.
	void AddExpansionClass(BMDLFrameExpansion* _class);		//확장 클래스를 추가합니다.
public:	//messageevent method
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseLeave();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
};

#endif