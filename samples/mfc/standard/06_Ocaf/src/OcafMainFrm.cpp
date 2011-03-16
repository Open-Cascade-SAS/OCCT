// MainFrm.cpp : implementation of the OcafMainFrame class
//

#include "StdAfx.h"

#include "OcafMainFrm.h"

#include "OcafApp.h"

/////////////////////////////////////////////////////////////////////////////
// OcafMainFrame

IMPLEMENT_DYNAMIC(OcafMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(OcafMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(OcafMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// OcafMainFrame construction/destruction

OcafMainFrame::OcafMainFrame():
	OCC_MainFrame(true)
{
	// TODO: add member initialization code here
	
}

OcafMainFrame::~OcafMainFrame()
{
}

int OcafMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (OCC_MainFrame::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_UndoRedoBar.Create(this) ||
		!m_UndoRedoBar.LoadToolBar(IDR_UNDOREDO) ||
		!m_ActionsBar.Create(this) ||
		!m_ActionsBar.LoadToolBar(IDR_ACTIONS))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_UndoRedoBar.SetBarStyle(m_UndoRedoBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_ActionsBar.SetBarStyle(m_ActionsBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_UndoRedoBar.EnableDocking(CBRS_ALIGN_ANY);
	m_ActionsBar.EnableDocking(CBRS_ALIGN_ANY);

	EnableDocking(CBRS_ALIGN_ANY);
    DockControlBarLeftOf(&m_UndoRedoBar,m_AISToolBar);
    DockControlBarLeftOf(&m_ActionsBar,&m_UndoRedoBar);

//	SetTitle("Ocaf Sample");
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// OcafMainFrame diagnostics

#ifdef _DEBUG
void OcafMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void OcafMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// OcafMainFrame message handlers
