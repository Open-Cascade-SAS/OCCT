// OCC_MainFrame.cpp: implementation of the OCC_MainFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "OCC_MainFrame.h"

#include <res\OCC_Resource.h>

/////////////////////////////////////////////////////////////////////////////
// OCC_MainFrame

IMPLEMENT_DYNAMIC(OCC_MainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(OCC_MainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(OCC_MainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// OCC_MainFrame construction/destruction

OCC_MainFrame::OCC_MainFrame(BOOL withAISToolBar /*=false*/)
{
  m_withAISToolBar=withAISToolBar;
  if(withAISToolBar){
    m_AISToolBar = new CToolBar;
  }
  else
    m_AISToolBar = NULL;
}

OCC_MainFrame::~OCC_MainFrame()
{
  if (m_AISToolBar) delete m_AISToolBar;
}

int OCC_MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  if (!m_wndToolBar.Create(this) ||
    !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
  {
    TRACE0("Failed to create toolbar\n");
    return -1;      // fail to create
  }

  if (m_withAISToolBar){
    if (!m_AISToolBar->Create(this) ||
      !m_AISToolBar->LoadToolBar(IDR_TB_AIS))
    {
      TRACE0("Failed to create toolbar\n");
      return -1;      // fail to create
    }
  }

  if (!m_wndStatusBar.Create(this) ||
    !m_wndStatusBar.SetIndicators(indicators,
    sizeof(indicators)/sizeof(UINT)))
  {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }

  // TODO: Remove this if you don't want tool tips or a resizeable toolbar
  m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
    CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

  // TODO: Delete these three lines if you don't want the toolbar to
  //  be dockable
  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar,AFX_IDW_DOCKBAR_TOP);
  if (m_withAISToolBar){
    m_AISToolBar->SetBarStyle(m_AISToolBar->GetBarStyle() |
      CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    m_AISToolBar->EnableDocking(CBRS_ALIGN_ANY);
    DockControlBarLeftOf(m_AISToolBar,&m_wndToolBar);
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// OCC_MainFrame diagnostics

#ifdef _DEBUG
void OCC_MainFrame::AssertValid() const
{
  CMDIFrameWnd::AssertValid();
}

void OCC_MainFrame::Dump(CDumpContext& dc) const
{
  CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

void OCC_MainFrame::SetStatusMessage(const CString & message)
{
  m_wndStatusBar.SetWindowText(message);
}


BOOL OCC_MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs
  return CMDIFrameWnd::PreCreateWindow(cs);
}

void OCC_MainFrame::DockControlBarLeftOf(CToolBar* Bar,CToolBar* LeftOf)
{
  CRect rect;
  DWORD dw;
  UINT n;

  // get MFC to adjust the dimensions of all docked ToolBars
  // so that GetWindowRect will be accurate
  RecalcLayout();
  LeftOf->GetWindowRect(&rect);
  rect.OffsetRect(1,0);
  dw=LeftOf->GetBarStyle();
  n = 0;
  n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
  n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
  n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
  n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;

  // When we take the default parameters on rect, DockControlBar will dock
  // each Toolbar on a seperate line.  By calculating a rectangle, we in effect
  // are simulating a Toolbar being dragged to that location and docked.
  DockControlBar(Bar,n,&rect);
}
