// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "GeometryApp.h"

#include "ChildFrm.h"

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
  //{{AFX_MSG_MAP(CChildFrame)
  ON_WM_CREATE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
  // TODO: add member initialization code here

}

CChildFrame::~CChildFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
  CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
  CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

void CChildFrame::ActivateFrame(int nCmdShow) 
{
  // TODO: Add your specialized code here and/or call the base class

  static BOOL first=true;
  if(first){
    first=false;
    CMDIChildWnd::ActivateFrame(SW_SHOWMAXIMIZED);
    return;
  }
  /*
  POSITION pos=AfxGetApp()->GetFirstDocTemplatePosition();
  CDocTemplate* DocT=AfxGetApp()->GetNextDocTemplate(pos);
  POSITION p=DocT->GetFirstDocPosition();
  DocT->GetNextDoc(p);
  if(p==NULL)
  nCmdShow = SW_SHOWMAXIMIZED;      
  */
  CMDIChildWnd::ActivateFrame(nCmdShow);
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_3dCHILDFRAME))
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

BOOL CChildFrame::DestroyWindow() 
{
  // TODO: Add your specialized code here and/or call the base class

  return CMDIChildWnd::DestroyWindow();
}
