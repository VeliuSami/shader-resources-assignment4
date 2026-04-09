#include "pch.h"
#include "MainFrm.h"

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(&rect);

	if (!m_wndView.Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE, rect, this, AFX_IDW_PANE_FIRST))
		return -1;

	return 0;
}

