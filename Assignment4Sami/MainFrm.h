#pragma once

#include <afxwin.h>

#include "ChildView.h"

class CMainFrame : public CFrameWnd
{
public:
	CMainFrame() = default;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

private:
	CChildView m_wndView;
};

