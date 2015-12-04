// ZCueingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Viewer3dApp.h"
#include "Viewer3dDoc.h"
#include "ZCueingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ZCueingDlg dialog


ZCueingDlg::ZCueingDlg(Handle(V3d_View) Current_V3d_View, CViewer3dDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(ZCueingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ZCueingDlg)
	myCurrent_V3d_View = Current_V3d_View;
	myDoc=pDoc;
	m_Cueing = FALSE;
	m_ZCueingDepth = 0.0;
	m_ZCueingWidth = 0.0;
	//}}AFX_DATA_INIT
}


void ZCueingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ZCueingDlg)
	DDX_Control(pDX, IDC_SLIDER_ZCUEINGWIDTH, m_ZCueingWidthSlidCtrl);
	DDX_Control(pDX, IDC_SLIDER_ZCUEINGDEPTH, m_ZCueingDepthSlidCtrl);
	DDX_Check(pDX, IDC_CHECK_CUEINGONOFF, m_Cueing);
	DDX_Text(pDX, IDC_EDIT_ZCUEINGDEPTH, m_ZCueingDepth);
	DDV_MinMaxDouble(pDX, m_ZCueingDepth, -1500., 1500.);
	DDX_Text(pDX, IDC_EDIT_ZCUEINGWIDTH, m_ZCueingWidth);
	DDV_MinMaxDouble(pDX, m_ZCueingWidth, 0., 1500.);
	//}}AFX_DATA_MAP
}

BOOL ZCueingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	Quantity_Length Depth, Width;

// Getting the ZCueing state (ON or OFF) and check (or not) the check button
	if(myCurrent_V3d_View->ZCueing(Depth, Width))
		m_Cueing=TRUE;
	else
		m_Cueing=FALSE;

// Setting the m_ZCueingWidth value at ZCueing Width
	m_ZCueingWidth=Width;
	m_ZCueingWidthSlidCtrl.SetRange(0, 1500, TRUE);
// Setting the m_ZCueingWidthSlidCtrl position at floor(Width) value (because slider position is an integer)
	m_ZCueingWidthSlidCtrl.SetPos( (int) floor(Width));

// Setting the m_ZCueingWidth value at ZCueing Width
	m_ZCueingDepth=Depth;
	m_ZCueingDepthSlidCtrl.SetRange(-1500, 1500, TRUE);
// Setting the m_ZCueingDepthSlidCtrl position at floor(Depth) value (because slider position is an integer)
	m_ZCueingDepthSlidCtrl.SetPos( (int) floor(Depth));

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(ZCueingDlg, CDialog)
	//{{AFX_MSG_MAP(ZCueingDlg)
	ON_BN_CLICKED(IDC_CHECK_CUEINGONOFF, OnCheckCueingonoff)
	ON_EN_CHANGE(IDC_EDIT_ZCUEINGDEPTH, OnChangeEditZcueingdepth)
	ON_EN_CHANGE(IDC_EDIT_ZCUEINGWIDTH, OnChangeEditZcueingwidth)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ZCueingDlg message handlers

void ZCueingDlg::OnCheckCueingonoff() 
{
	Quantity_Length Depth, Width;
	UpdateData(TRUE);

	if(m_Cueing)
	{
		if(!myCurrent_V3d_View->ZCueing(Depth, Width))
// Setting the ZCueing on if it's not yet
			myCurrent_V3d_View->SetZCueingOn();
	}
	else
	{
		if(myCurrent_V3d_View->ZCueing(Depth, Width))
// Setting the ZCueing off if it's not yet
			myCurrent_V3d_View->SetZCueingOff();
	}

	myCurrent_V3d_View->Update();


  TCollection_AsciiString Message("\
if(!myCurrent_V3d_View->ZCueing(Depth, Width));\n\
	myCurrent_V3d_View->SetZCueingOn();\n\
else\n\
	myCurrent_V3d_View->SetZCueingOff();\n\
\n\
myCurrent_V3d_View->Update();\n\
  ");

  // Update The Result Message Dialog
	myDoc->UpdateResultMessageDlg("SetZCueingWidth",Message);
}

void ZCueingDlg::OnChangeEditZcueingdepth() 
{
	UpdateData(TRUE);

// Setting the m_ZCueingDepthSlidCtrl position at floor(m_ZCueingDepth) value (because slider position is an integer)
	m_ZCueingDepthSlidCtrl.SetPos( (int) floor(m_ZCueingDepth));
// Setting the ZCueing depth at m_ZCueingDepth value
	myCurrent_V3d_View->SetZCueingDepth( (const Quantity_Length) m_ZCueingDepth);

	myCurrent_V3d_View->Update();

  TCollection_AsciiString Message("\
myCurrent_V3d_View->SetZCueingDepth( (const Quantity_Length) m_ZCueingDepth);\n\
\n\
myCurrent_V3d_View->Update();\n\
  ");

  // Update The Result Message Dialog
	myDoc->UpdateResultMessageDlg("SetZCueingDepth",Message);
}

void ZCueingDlg::OnChangeEditZcueingwidth() 
{
	UpdateData(TRUE);

	if(m_ZCueingWidth<=0)
	{
		m_ZCueingWidth=1;
		UpdateData(FALSE);
	}

// Setting the m_ZCueingWidthSlidCtrl position at floor(m_ZCueingWidth) value (because slider position is an integer)
	m_ZCueingWidthSlidCtrl.SetPos( (int) floor(m_ZCueingWidth));
// Setting the ZCueing width at m_ZCueingWidth value
	myCurrent_V3d_View->SetZCueingWidth( (const Quantity_Length) m_ZCueingWidth);

	myCurrent_V3d_View->Update();

  TCollection_AsciiString Message("\
myCurrent_V3d_View->SetZCueingWidth( (const Quantity_Length) m_ZCueingWidth);\n\
\n\
myCurrent_V3d_View->Update();\n\
  ");

  // Update The Result Message Dialog
	myDoc->UpdateResultMessageDlg("SetZCueingWidth",Message);
}

void ZCueingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
// Setting the m_ZCueingDepth value at m_ZCueingDepthSlidCtrl position
	m_ZCueingDepth = m_ZCueingDepthSlidCtrl.GetPos();
// Setting the m_ZCueingWidth value at m_ZCueingWidthSlidCtrl position
	m_ZCueingWidth = m_ZCueingWidthSlidCtrl.GetPos();

	if(m_ZCueingWidth<=0)
	{
		m_ZCueingWidth=1;
		m_ZCueingWidthSlidCtrl.SetPos( 1 );
	}

	UpdateData(FALSE);
	
// Setting the ZCueing depth at m_ZCueingDepth value
	myCurrent_V3d_View->SetZCueingDepth( (const Quantity_Length) m_ZCueingDepth);
// Setting the ZCueing width at m_ZCueingWidth value
	myCurrent_V3d_View->SetZCueingWidth( (const Quantity_Length) m_ZCueingWidth);
	myCurrent_V3d_View->Update();

  TCollection_AsciiString Message("\
myCurrent_V3d_View->SetZCueingDepth( (const Quantity_Length) m_ZCueingDepth);\n\
\n\
myCurrent_V3d_View->SetZCueingWidth( (const Quantity_Length) m_ZCueingWidth);\n\
\n\
myCurrent_V3d_View->Update();\n\
  ");

  // Update The Result Message Dialog
	myDoc->UpdateResultMessageDlg("SetZCueingDepth/SetZCueingWidth",Message);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

