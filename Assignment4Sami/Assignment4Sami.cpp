#include "pch.h"

#include "MainFrm.h"

class CAssignment4SamiApp : public CWinApp
{
public:
	virtual BOOL InitInstance() override;
};

CAssignment4SamiApp theApp;

BOOL CAssignment4SamiApp::InitInstance()
{
	CWinApp::InitInstance();

	auto* pFrame = new CMainFrame();
	if (!pFrame->Create(nullptr, L"Assignment4Sami"))
		return FALSE;

	m_pMainWnd = pFrame;
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

