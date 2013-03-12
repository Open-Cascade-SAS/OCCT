// OCC_2dChildFrame.cpp: implementation of the OCC_2dChildFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "OCC_2dChildFrame.h"

#include "res\OCC_Resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE (OCC_2dChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(OCC_2dChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(OCC_2dChildFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


OCC_2dChildFrame::OCC_2dChildFrame()
{

}

OCC_2dChildFrame::~OCC_2dChildFrame()
{

}

int OCC_2dChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_2dCHILDFRAME))
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

BOOL OCC_2dChildFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
  // TODO: Add your specialized code here and/or call the base class

  return CMDIChildWnd::PreCreateWindow(cs);
}
