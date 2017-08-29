#ifndef _M_AMD64
#error This code does not support x86
#endif
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once
#include "ChildView.h"
#include"BMDLFrame.h"
#include"BMDLMenuFrame.h"
#include"VirtualView.h"
#include"ListView.h"
#include<direct.h>
#include<atomic>
#include<SCVL/All>
#include<opencv2/opencv.hpp>
extern "C" {
#include <libavformat/avformat.h>
}

class CMainFrame : public BMDLFrame {

public:
	CMainFrame();
protected:
	DECLARE_DYNAMIC(CMainFrame)

	// 특성입니다.
public:

	// 작업입니다.
public:

	// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	BMDLMenuFrame* m_menu_frame = nullptr;
	CChildView    m_wndView;

	// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnFileOpenvideo();
	afx_msg void OnExtractAsJPG();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnExtractAsPng();
	afx_msg void OnFileClear();
	afx_msg void OnDestroy();
};


