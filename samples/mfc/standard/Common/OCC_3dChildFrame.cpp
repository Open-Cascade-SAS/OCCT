// OCC_3dChildFrame.cpp: implementation of the OCC_3dChildFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "OCC_3dChildFrame.h"

#include "res\OCC_Resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE (OCC_3dChildFrame, OCC_BaseChildFrame)

BEGIN_MESSAGE_MAP(OCC_3dChildFrame, OCC_BaseChildFrame)
	//{{AFX_MSG_MAP(OCC_3dChildFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

OCC_3dChildFrame::OCC_3dChildFrame()
{

}

OCC_3dChildFrame::~OCC_3dChildFrame()
{

}

int OCC_3dChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

  // Create toolbar for RayTracing functionality
  if (!m_RTToolBar.Create(this) || !m_RTToolBar.LoadToolBar(IDR_RAY_TRACING))
  {
    TRACE0("Failed to create toolbar\n");
    return -1; // fail to create
  }

  m_RTToolBar.SetBarStyle(m_RTToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
  m_RTToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_RTToolBar);

  return 0;
}