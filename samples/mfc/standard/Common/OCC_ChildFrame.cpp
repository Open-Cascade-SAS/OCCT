// OCC_BaseChildFrame.cpp: implementation of the OCC_BaseChildFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "OCC_BaseChildFrame.h"

#include <res\OCC_Resource.h>

/////////////////////////////////////////////////////////////////////////////
// OCC_BaseChildFrame

IMPLEMENT_DYNCREATE (OCC_BaseChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(OCC_BaseChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(OCC_BaseChildFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OCC_BaseChildFrame construction/destruction

OCC_BaseChildFrame::OCC_BaseChildFrame()
{
}

OCC_BaseChildFrame::~OCC_BaseChildFrame()
{
}

BOOL OCC_BaseChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIChildWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// OCC_BaseChildFrame diagnostics

#ifdef _DEBUG
void OCC_BaseChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void OCC_BaseChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// OCC_BaseChildFrame message handlers

int OCC_BaseChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_CHILDFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1; // fail to create
	}

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}


void OCC_BaseChildFrame::ActivateFrame(int nCmdShow)
{
	POSITION pos=AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* DocT=AfxGetApp()->GetNextDocTemplate(pos);
	POSITION p=DocT->GetFirstDocPosition();
	DocT->GetNextDoc(p);
	if(p==NULL)
	nCmdShow = SW_SHOWMAXIMIZED;      
	CMDIChildWnd::ActivateFrame(nCmdShow);
}
