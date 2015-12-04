// TrihedronDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Viewer3dApp.h"
#include "Viewer3dView.h"
#include "TrihedronDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void round(double &value, unsigned char digits) 
{ 
char neg = 1; 
if(value < 0){ 
neg = (-1); 
value *= (-1); 
} 
double inc(1.0); 
while(digits){ inc*=10; --digits; } 
double dg(value*inc*100); 
value = floor(floor(dg)/100); 
if((dg-(value*100)) > 49.9) value+=1; 
value = (value*neg)/inc; 
}


/////////////////////////////////////////////////////////////////////////////
// CTrihedronDlg dialog


//CTrihedronDlg::CTrihedronDlg(CWnd* pParent /*=NULL*/)
	//: CDialog(CTrihedronDlg::IDD, pParent)

CTrihedronDlg::CTrihedronDlg(Handle(V3d_View) Current_V3d_View, CViewer3dDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CTrihedronDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT(CTrihedronDlg)
	m_TrihedronScale = 0.1;
	myDoc=pDoc;
	myCurrent_V3d_View = Current_V3d_View;
	/*m_*/Color = Quantity_NOC_WHITE;
	/*m_*/Position = Aspect_TOTP_LEFT_UPPER;

		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTrihedronDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrihedronDlg)
	DDX_Control(pDX, IDC_COMBOTRIHEDRPOS, m_ComboTrihedronPosList);
	DDX_Control(pDX, IDC_COMBOTRIHEDRCOLOR, m_ComboTrihedronColorList);
	DDX_Text(pDX, IDC_EDITTRIHEDRSCALE, m_TrihedronScale);
	DDV_MinMaxDouble(pDX, m_TrihedronScale, 0., 1.);
	DDX_Control(pDX, IDC_SPINTRIHEDRSCALE, m_SpinTrihedronScale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrihedronDlg, CDialog)
	//{{AFX_MSG_MAP(CTrihedronDlg)
	ON_CBN_SELCHANGE(IDC_COMBOTRIHEDRCOLOR, OnSelchangeCombotrihedrcolor)
	ON_CBN_SELCHANGE(IDC_COMBOTRIHEDRPOS, OnSelchangeCombotrihedrpos)
	ON_EN_CHANGE(IDC_EDITTRIHEDRSCALE, OnChangeEdittrihedrscale)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINTRIHEDRSCALE, OnDeltaposSpintrihedrscale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





/////////////////////////////////////////////////////////////////////////////
// CTrihedronDlg message handlers

void CTrihedronDlg::OnSelchangeCombotrihedrcolor() 
{
	UpdateData(TRUE);
	int a = m_ComboTrihedronColorList.GetCurSel();
	if(	a==0)
		Color=Quantity_NOC_BLACK;
	else if(a==1)
		Color=Quantity_NOC_MATRABLUE;
	else if(a==2)
		Color=Quantity_NOC_MATRAGRAY;
//	else if(a==3)
//		Color=Quantity_NOC_ALICEBLUE;
	else if(a==3)
		Color=Quantity_NOC_ANTIQUEWHITE;
	//else if(a==4)
	//	Color=Quantity_NOC_BISQUE;

	UpdateData(FALSE);

	myCurrent_V3d_View->TriedronDisplay(Position, Color, m_TrihedronScale);
	myCurrent_V3d_View->Update();

}

void CTrihedronDlg::OnSelchangeCombotrihedrpos() 
{
	UpdateData(TRUE);
	int b = m_ComboTrihedronPosList.GetCurSel();

	if(	b==0)
		Position=Aspect_TOTP_CENTER;
	else if(b==1)
		Position=Aspect_TOTP_LEFT_LOWER;
	else if(b==2)
		Position=Aspect_TOTP_LEFT_UPPER;
	else if(b==3)
		Position=Aspect_TOTP_RIGHT_LOWER;
	else if(b==4)
		Position=Aspect_TOTP_RIGHT_UPPER;

	UpdateData(FALSE);

	myCurrent_V3d_View->TriedronDisplay(Position, Color, m_TrihedronScale);
	myCurrent_V3d_View->Update();
	
}


BOOL CTrihedronDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateData(TRUE);

// Initializing the ComboBox : Position
	m_ComboTrihedronPosList.InsertString(-1, L"Center");
  m_ComboTrihedronPosList.InsertString(-1, L"Left Lower");
  m_ComboTrihedronPosList.InsertString(-1, L"Left Upper");
  m_ComboTrihedronPosList.InsertString(-1, L"Right Lower");
  m_ComboTrihedronPosList.InsertString(-1, L"Right Upper");

// Initializing the ComboBox : Color
	/*
	m_ComboTrihedronColorList.InsertString(-1, L"BLACK");
	m_ComboTrihedronColorList.InsertString(-1, L"MATRABLUE");
	m_ComboTrihedronColorList.InsertString(-1, L"MATRAGRAY");
	m_ComboTrihedronColorList.InsertString(-1, L"ALICE BLUE");
	m_ComboTrihedronColorList.InsertString(-1, L"WHITE");
	m_ComboTrihedronColorList.InsertString(-1, L"BISQUE");
	*/
  m_ComboTrihedronColorList.InsertString(-1, L"Black");
  m_ComboTrihedronColorList.InsertString(-1, L"Blue");
  m_ComboTrihedronColorList.InsertString(-1, L"Gray");
  m_ComboTrihedronColorList.InsertString(-1, L"White");
	//m_ComboTrihedronColorList.InsertString(-1, L"Bisque");
	


	UpdateData(FALSE);
	
/*	if (CDemoVisualizationView::slinitialisation())
	{
		AfxMessageBox("debut");

		if(m_Position==Aspect_TOTP_CENTER)
			m_ComboTrihedronPosList.SetCurSel(0);
		else if(m_Position==Aspect_TOTP_LEFT_LOWER)
			m_ComboTrihedronPosList.SetCurSel(1);
		else if(m_Position==Aspect_TOTP_LEFT_UPPER)
			m_ComboTrihedronPosList.SetCurSel(2);
		else if(m_Position==Aspect_TOTP_RIGHT_LOWER)
			m_ComboTrihedronPosList.SetCurSel(3);
		else if(m_Position==Aspect_TOTP_RIGHT_UPPER)
			m_ComboTrihedronPosList.SetCurSel(4);

		if(m_Color==Quantity_NOC_BLACK)
			m_ComboTrihedronColorList.SetCurSel(0);
		else if(m_Color==Quantity_NOC_MATRABLUE)
			m_ComboTrihedronColorList.SetCurSel(1);
		else if(m_Color==Quantity_NOC_MATRAGRAY)
			m_ComboTrihedronColorList.SetCurSel(2);
		else if(m_Color==Quantity_NOC_ALICEBLUE)
			m_ComboTrihedronColorList.SetCurSel(3);
		else if(m_Color==Quantity_NOC_WHITE)
			m_ComboTrihedronColorList.SetCurSel(4);
		else if(m_Color==Quantity_NOC_BISQUE)
			m_ComboTrihedronColorList.SetCurSel(5);
		Position = m_Position;
		Color = m_Color;
		myCurrent_V3d_View->TriedronDisplay(Position, Color, m_TrihedronScale);
		myCurrent_V3d_View->Update();

		Initialisation = Standard_False;

	}
	else
	{*/
		if(Position==Aspect_TOTP_CENTER)
			m_ComboTrihedronPosList.SetCurSel(0);
		else if(Position==Aspect_TOTP_LEFT_LOWER)
			m_ComboTrihedronPosList.SetCurSel(1);
		else if(Position==Aspect_TOTP_LEFT_UPPER)
			m_ComboTrihedronPosList.SetCurSel(2);
		else if(Position==Aspect_TOTP_RIGHT_LOWER)
			m_ComboTrihedronPosList.SetCurSel(3);
		else if(Position==Aspect_TOTP_RIGHT_UPPER)
			m_ComboTrihedronPosList.SetCurSel(4);

		if(Color==Quantity_NOC_BLACK)
			m_ComboTrihedronColorList.SetCurSel(0);
		else if(Color==Quantity_NOC_MATRABLUE)
			m_ComboTrihedronColorList.SetCurSel(1);
		else if(Color==Quantity_NOC_MATRAGRAY)
			m_ComboTrihedronColorList.SetCurSel(2);
//		else if(Color==Quantity_NOC_ALICEBLUE)
//			m_ComboTrihedronColorList.SetCurSel(3);
		else if(Color==Quantity_NOC_WHITE)
			m_ComboTrihedronColorList.SetCurSel(3);
		//else if(Color==Quantity_NOC_BISQUE)
		//	m_ComboTrihedronColorList.SetCurSel(4);	

		myCurrent_V3d_View->TriedronDisplay(Position, Color, m_TrihedronScale);
		myCurrent_V3d_View->Update();
//	}


	


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CTrihedronDlg::OnDeltaposSpintrihedrscale(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	if (pNMUpDown->iDelta >= 1)
		pNMUpDown->iDelta = 1;
	else 
		pNMUpDown->iDelta = -1;
	
	if ((pNMUpDown->iDelta > 0) && (m_TrihedronScale > 0)) 
		m_TrihedronScale = m_TrihedronScale - (pNMUpDown->iDelta)*0.01;
	
	if ((pNMUpDown->iDelta < 0) && (m_TrihedronScale < 1)) 
		m_TrihedronScale = m_TrihedronScale - (pNMUpDown->iDelta)*0.01;


	m_TrihedronScale = m_TrihedronScale*100;
	m_TrihedronScale = floor(m_TrihedronScale );
	m_TrihedronScale  = m_TrihedronScale /100;

//	round(m_TrihedronScale,2);

	if (fabs(m_TrihedronScale) < 0.001)
		m_TrihedronScale =0;
	UpdateData(FALSE);
	
	myCurrent_V3d_View->TriedronDisplay(Position, Color, m_TrihedronScale);
	*pResult = 0;

	myCurrent_V3d_View->Update();

}

void CTrihedronDlg::OnChangeEdittrihedrscale() 
{
	if (UpdateData()){
		myCurrent_V3d_View->TriedronDisplay(Position, Color, m_TrihedronScale);
		myCurrent_V3d_View->Update();
	}
}

void CTrihedronDlg::OnCancel() 
{
	myCurrent_V3d_View->TriedronErase();
	myCurrent_V3d_View->Update();
	myDoc -> SetMyStaticTrihedronAxisIsDisplayed(FALSE);
//	Initialisation = Standard_True;
	CDialog::OnCancel();
}

void CTrihedronDlg::OnOK() 
{
	myDoc -> SetMyStaticTrihedronAxisIsDisplayed(TRUE);
	CDialog::OnOK();
}

