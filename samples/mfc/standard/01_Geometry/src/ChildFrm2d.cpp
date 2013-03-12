// ChildFrm2D.cpp : implementation of the CChildFrame2D class/

#include "stdafx.h"
#include "ChildFrm2d.h"
#include "GeometryApp.h"

IMPLEMENT_DYNCREATE(CChildFrame2D, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame2D, CMDIChildWnd)
  //{{AFX_MSG_MAP(CChildFrame2D)
  ON_WM_CREATE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
  ID_SEPARATOR,           // status line indicator
  ID_INDICATOR_CAPS,
  ID_INDICATOR_NUM,
  ID_INDICATOR_SCRL,
};


/////////////////////////////////////////////////////////////////////////////
// CChildFrame2D construction/destruction

CChildFrame2D::CChildFrame2D()
{
  // TODO: add member initialization code here

}

CChildFrame2D::~CChildFrame2D()
{
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame2D diagnostics

#ifdef _DEBUG
void CChildFrame2D::AssertValid() const
{
  CMDIChildWnd::AssertValid();
}

void CChildFrame2D::Dump(CDumpContext& dc) const
{
  CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame2D message handlers

int CChildFrame2D::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

  if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  if (!m_wndToolBar.Create(this) ||
    !m_wndToolBar.LoadToolBar(IDR_2dCHILDFRAME))
  {
    TRACE0("Failed to create toolbar\n");
    return -1;      // fail to create
  }

  // TODO: Remove this if you don't want tool tips or a resizeable toolbar
  m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
    CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

  // TODO: Delete these three lines if you don't want the toolbar to
  //  be dockable
  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);

  return 0;

}


void CChildFrame2D::ActivateFrame(int nCmdShow) 
{
  // TODO: Add your specialized code here and/or call the base class

  CMDIChildWnd::ActivateFrame(nCmdShow);
}
