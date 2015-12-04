// ZClippingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Viewer3dApp.h"
#include "Viewer3dDoc.h"
#include "ZClippingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ZClippingDlg dialog

ZClippingDlg::ZClippingDlg(Handle(V3d_View) Current_V3d_View, CViewer3dDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(ZClippingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ZClippingDlg)
	m_ZClippingDepth = 0.0;
	m_ZClippingWidth = 0.0;
	myDoc=pDoc;
	myCurrent_V3d_View = Current_V3d_View;
	//}}AFX_DATA_INIT
}


void ZClippingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ZClippingDlg)
	DDX_Control(pDX, IDC_SLIDER_ZCLIPPINGWIDTH, m_ZClippingWidthSlidCtrl);
	DDX_Control(pDX, IDC_SLIDER_ZCLIPPINGDEPTH, m_ZClippingDepthSlidCtrl);
	DDX_Control(pDX, IDC_COMBO_ZCLIPPINGTYPE, m_ZClippingTypeList);
	DDX_Text(pDX, IDC_EDIT_ZCLIPPINGDEPTH, m_ZClippingDepth);
	DDV_MinMaxDouble(pDX, m_ZClippingDepth, -1500., 1500.);
	DDX_Text(pDX, IDC_EDIT_ZCLIPPINGWIDTH, m_ZClippingWidth);
	DDV_MinMaxDouble(pDX, m_ZClippingWidth, 0., 1500.);
	//}}AFX_DATA_MAP
}

BOOL ZClippingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
// Initializing the ComboBox
	m_ZClippingTypeList.InsertString(-1, L"OFF");
	m_ZClippingTypeList.InsertString(-1, L"BACK");
	m_ZClippingTypeList.InsertString(-1, L"FRONT");
	m_ZClippingTypeList.InsertString(-1, L"SLICE");

// Getting the type of ZClipping and select it in the ComboBox
	Quantity_Length Depth, Width;
	V3d_TypeOfZclipping myTypeOfZclipping=myCurrent_V3d_View->ZClipping(Depth, Width);
	if(myTypeOfZclipping==V3d_OFF)
		m_ZClippingTypeList.SetCurSel(0);
	else if(myTypeOfZclipping==V3d_BACK)
		m_ZClippingTypeList.SetCurSel(1);
	else if(myTypeOfZclipping==V3d_FRONT)
		m_ZClippingTypeList.SetCurSel(2);
	else if(myTypeOfZclipping==V3d_SLICE)
		m_ZClippingTypeList.SetCurSel(3);

// Setting the m_ZClippingWidth value at ZClipping width
	m_ZClippingWidth=Width;
// Setting the m_ZClippingWidthSlidCtrl position at floor(Width) value (because slider position is an integer)
	m_ZClippingWidthSlidCtrl.SetRange(0, 1500, TRUE);
	m_ZClippingWidthSlidCtrl.SetPos( (int) floor(Width));

// Setting the m_ZClippingDepth value at ZClipping depth
	m_ZClippingDepth=Depth;
// Setting the m_ZClippingDepthSlidCtrl position at floor(Depth) value (because slider position is an integer)
	m_ZClippingDepthSlidCtrl.SetRange(-1500, 1500, TRUE);
	m_ZClippingDepthSlidCtrl.SetPos( (int) floor(Depth));

	UpdateData(FALSE);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(ZClippingDlg, CDialog)
	//{{AFX_MSG_MAP(ZClippingDlg)
	ON_EN_CHANGE(IDC_EDIT_ZCLIPPINGDEPTH, OnChangeEditZclippingdepth)
	ON_EN_CHANGE(IDC_EDIT_ZCLIPPINGWIDTH, OnChangeEditZclippingwidth)
	ON_CBN_SELCHANGE(IDC_COMBO_ZCLIPPINGTYPE, OnSelchangeComboZclippingtype)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ZClippingDlg message handlers

void ZClippingDlg::OnChangeEditZclippingdepth() 
{
	UpdateData(TRUE);

// Setting the m_ZClippingDepthSlidCtrl position at floor(m_ZClippingDepth) value (because slider position is an integer)
	m_ZClippingDepthSlidCtrl.SetPos( (int) floor(m_ZClippingDepth));
// Setting the ZClipping depth at m_ZClippingDepth value
	myCurrent_V3d_View->SetZClippingDepth( (const Quantity_Length) m_ZClippingDepth);

	myCurrent_V3d_View->Update();

  TCollection_AsciiString Message("\
myCurrent_V3d_View->SetZClippingDepth( (const Quantity_Length) m_ZClippingDepth);\n\
\n\
myCurrent_V3d_View->Update();\n\
  ");

  // Update The Result Message Dialog
	myDoc->UpdateResultMessageDlg("SetZClippingDepth",Message);
}

void ZClippingDlg::OnChangeEditZclippingwidth() 
{
	UpdateData(TRUE);

// ZClipping width must be >0
	if(m_ZClippingWidth<=0)
	{
		m_ZClippingWidth=1;
		UpdateData(FALSE);
	}

// Setting the m_ZClippingWidthSlidCtrl position at floor(m_ZClippingWidth) value (because slider position is an integer)
	m_ZClippingWidthSlidCtrl.SetPos( (int) floor(m_ZClippingWidth));
// Setting the ZClipping width at m_ZClippingWidth value
	myCurrent_V3d_View->SetZClippingWidth( (const Quantity_Length) m_ZClippingWidth);

	myCurrent_V3d_View->Update();

  TCollection_AsciiString Message("\
myCurrent_V3d_View->SetZClippingWidth( (const Quantity_Length) m_ZClippingWidth);\n\
\n\
myCurrent_V3d_View->Update();\n\
  ");

  // Update The Result Message Dialog
	myDoc->UpdateResultMessageDlg("SetZClippingDepth",Message);
}

void ZClippingDlg::OnSelchangeComboZclippingtype() 
{
// Setting the ZClipping type as selected in the ComboBox
	myCurrent_V3d_View->SetZClippingType(V3d_TypeOfZclipping(m_ZClippingTypeList.GetCurSel()));
	myCurrent_V3d_View->Update();

  TCollection_AsciiString Message("\
myCurrent_V3d_View->SetZClippingType(V3d_TypeOfZclipping myTypeOfZclipping);\n\
\n\
myCurrent_V3d_View->Update();\n\
  ");

  // Update The Result Message Dialog
	myDoc->UpdateResultMessageDlg("SetZClippingType",Message);
}

void ZClippingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
// Setting the m_ZClippingDepth value at m_ZClippingDepthSlidCtrl position
	m_ZClippingDepth = m_ZClippingDepthSlidCtrl.GetPos();
// Setting the m_ZClippingWidth value at m_ZClippingWidthSlidCtrl position
	m_ZClippingWidth = m_ZClippingWidthSlidCtrl.GetPos();

	if(m_ZClippingWidth<=0)
	{
		m_ZClippingWidth=1;
		m_ZClippingWidthSlidCtrl.SetPos( 1 );
	}

	UpdateData(FALSE);

// Setting the ZClipping depth at m_ZClippingDepth value
	myCurrent_V3d_View->SetZClippingDepth( (const Quantity_Length) m_ZClippingDepth);
// Setting the ZClipping width at m_ZClippingWidth value
	myCurrent_V3d_View->SetZClippingWidth( (const Quantity_Length) m_ZClippingWidth);
	myCurrent_V3d_View->Update();

  TCollection_AsciiString Message("\
myCurrent_V3d_View->SetZClippingWidth( (const Quantity_Length) m_ZClippingWidth);\n\
\n\
myCurrent_V3d_View->SetZClippingDepth( (const Quantity_Length) m_ZClippingDepth);\n\
\n\
myCurrent_V3d_View->Update();\n\
  ");

  // Update The Result Message Dialog
	myDoc->UpdateResultMessageDlg("SetZClippingDepth/SetZClippingWidth",Message);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

