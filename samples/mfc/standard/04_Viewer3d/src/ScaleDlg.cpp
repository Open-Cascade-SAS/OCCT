// ScaleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScaleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScaleDlg dialog


ScaleDlg::ScaleDlg(Handle(V3d_View) Current_V3d_View, CViewer3dView* pView, CWnd* pParent /*=NULL*/)
	: CDialog(ScaleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ScaleDlg)
	myCurrent_V3d_View = Current_V3d_View;
	myView=pView;
	m_ScaleX = 0;
	m_ScaleY = 0;
	m_ScaleZ = 0;
	//}}AFX_DATA_INIT
}


void ScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ScaleDlg)
	DDX_Control(pDX, IDC_SLIDER_SCALEX, m_ScaleXSlidCtrl);
	DDX_Control(pDX, IDC_SLIDER_SCALEY, m_ScaleYSlidCtrl);
	DDX_Control(pDX, IDC_SLIDER_SCALEZ, m_ScaleZSlidCtrl);
	DDX_Text(pDX, IDC_EDIT_SCALEX, m_ScaleX);
	DDV_MinMaxInt(pDX, m_ScaleX, 1, 100);
	DDX_Text(pDX, IDC_EDIT_SCALEY, m_ScaleY);
	DDV_MinMaxInt(pDX, m_ScaleY, 1, 100);
	DDX_Text(pDX, IDC_EDIT_SCALEZ, m_ScaleZ);
	DDV_MinMaxInt(pDX, m_ScaleZ, 1, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ScaleDlg, CDialog)
	//{{AFX_MSG_MAP(ScaleDlg)
	ON_EN_CHANGE(IDC_EDIT_SCALEX, OnChangeEditScaleX)
	ON_EN_CHANGE(IDC_EDIT_SCALEY, OnChangeEditScaleY)
	ON_EN_CHANGE(IDC_EDIT_SCALEZ, OnChangeEditScaleZ)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScaleDlg message handlers
BOOL ScaleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int ScaleX = myView->scaleX;
	int ScaleY = myView->scaleY;
	int ScaleZ = myView->scaleZ;

	m_ScaleX=ScaleX;
	m_ScaleXSlidCtrl.SetRange(0, 100, TRUE);
	m_ScaleXSlidCtrl.SetPos( ScaleX );

	m_ScaleY=ScaleY;
	m_ScaleYSlidCtrl.SetRange(0, 100, TRUE);
	m_ScaleYSlidCtrl.SetPos( ScaleY );

	m_ScaleZ=ScaleZ;
	m_ScaleZSlidCtrl.SetRange(0, 100, TRUE);
	m_ScaleZSlidCtrl.SetPos( ScaleZ );

	UpdateData(FALSE);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ScaleDlg::OnChangeEditScaleX() 
{
	UpdateData(TRUE);

	m_ScaleXSlidCtrl.SetPos( m_ScaleX );
	myView->scaleX = m_ScaleX;

    myCurrent_V3d_View->SetAxialScale( m_ScaleX, m_ScaleY, m_ScaleZ );
	myCurrent_V3d_View->Update();

  TCollection_AsciiString Message("\
myCurrent_V3d_View->SetAxialScale( m_ScaleX, m_ScaleY, m_ScaleZ);\n\
\n\
myCurrent_V3d_View->Update();\n\
  ");

  // Update The Result Message Dialog
	myView->GetDocument()->UpdateResultMessageDlg("SetAxialScale",Message);
}

void ScaleDlg::OnChangeEditScaleY() 
{
	UpdateData(TRUE);

	m_ScaleYSlidCtrl.SetPos( m_ScaleY );
	myView->scaleY = m_ScaleY;

    myCurrent_V3d_View->SetAxialScale( m_ScaleX, m_ScaleY, m_ScaleZ );
	myCurrent_V3d_View->Update();

  TCollection_AsciiString Message("\
myCurrent_V3d_View->SetAxialScale( m_ScaleX, m_ScaleY, m_ScaleZ);\n\
\n\
myCurrent_V3d_View->Update();\n\
  ");

  // Update The Result Message Dialog
	myView->GetDocument()->UpdateResultMessageDlg("SetAxialScale",Message);
}

void ScaleDlg::OnChangeEditScaleZ() 
{
	UpdateData(TRUE);

	m_ScaleZSlidCtrl.SetPos( m_ScaleZ );
	myView->scaleZ = m_ScaleZ;

    myCurrent_V3d_View->SetAxialScale( m_ScaleX, m_ScaleY, m_ScaleZ );
	myCurrent_V3d_View->Update();

  TCollection_AsciiString Message("\
myCurrent_V3d_View->SetAxialScale( m_ScaleX, m_ScaleY, m_ScaleZ);\n\
\n\
myCurrent_V3d_View->Update();\n\
  ");

  // Update The Result Message Dialog
	myView->GetDocument()->UpdateResultMessageDlg("SetAxialScale",Message);
}

void ScaleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_ScaleX = m_ScaleXSlidCtrl.GetPos();
	m_ScaleY = m_ScaleYSlidCtrl.GetPos();
	m_ScaleZ = m_ScaleZSlidCtrl.GetPos();

	if(m_ScaleX<=0)
	{
		m_ScaleX=1;
		m_ScaleXSlidCtrl.SetPos( 1 );
	}

	if(m_ScaleY<=0)
	{
		m_ScaleY=1;
		m_ScaleYSlidCtrl.SetPos( 1 );
	}

	if(m_ScaleZ<=0)
	{
		m_ScaleZ=1;
		m_ScaleZSlidCtrl.SetPos( 1 );
	}

	UpdateData(FALSE);

	myView->scaleX = m_ScaleX;
	myView->scaleY = m_ScaleY;
	myView->scaleZ = m_ScaleZ;

    myCurrent_V3d_View->SetAxialScale( m_ScaleX, m_ScaleY, m_ScaleZ );
	myCurrent_V3d_View->Update();

  TCollection_AsciiString Message("\
myCurrent_V3d_View->SetAxialScale( m_ScaleX, m_ScaleY, m_ScaleZ);\n\
\n\
myCurrent_V3d_View->Update();\n\
  ");

  // Update The Result Message Dialog
	myView->GetDocument()->UpdateResultMessageDlg("SetAxialScale",Message);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

