// ColoredMeshDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ColoredMeshDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColoredMeshDlg dialog


CColoredMeshDlg::CColoredMeshDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColoredMeshDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColoredMeshDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CColoredMeshDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColoredMeshDlg)
	DDX_Check(pDX, IDC_RADIO1, m_Rad1OnOff);
	DDX_Check(pDX, IDC_RADIO2, m_Rad2OnOff);
	DDX_Check(pDX, IDC_RADIO3, m_Rad3OnOff);
	DDX_Check(pDX, IDC_CHECK_X1, m_CheckX1OnOff);
	DDX_Check(pDX, IDC_CHECK_XBlue, m_CheckXBlueOnOff);
	DDX_Check(pDX, IDC_CHECK_XGreen, m_CheckXGreenOnOff);
	DDX_Check(pDX, IDC_CHECK_XRed, m_CheckXRedOnOff);
	DDX_Check(pDX, IDC_CHECK_Y1, m_CheckY1OnOff);
	DDX_Check(pDX, IDC_CHECK_YBlue, m_CheckYBlueOnOff);
	DDX_Check(pDX, IDC_CHECK_YGreen, m_CheckYGreenOnOff);
	DDX_Check(pDX, IDC_CHECK_YRed, m_CheckYRedOnOff);
	DDX_Check(pDX, IDC_CHECK_Z1, m_CheckZ1OnOff);
	DDX_Check(pDX, IDC_CHECK_ZBlue, m_CheckZBlueOnOff);
	DDX_Check(pDX, IDC_CHECK_ZGreen, m_CheckZGreenOnOff);
	DDX_Check(pDX, IDC_CHECK_ZRed, m_CheckZRedOnOff);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColoredMeshDlg, CDialog)
	//{{AFX_MSG_MAP(CColoredMeshDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_CHECK_X1, OnCheckX1)
	ON_BN_CLICKED(IDC_CHECK_XBlue, OnCHECKXBlue)
	ON_BN_CLICKED(IDC_CHECK_XGreen, OnCHECKXGreen)
	ON_BN_CLICKED(IDC_CHECK_XRed, OnCHECKXRed)
	ON_BN_CLICKED(IDC_CHECK_Y1, OnCheckY1)
	ON_BN_CLICKED(IDC_CHECK_YBlue, OnCHECKYBlue)
	ON_BN_CLICKED(IDC_CHECK_YGreen, OnCHECKYGreen)
	ON_BN_CLICKED(IDC_CHECK_YRed, OnCHECKYRed)
	ON_BN_CLICKED(IDC_CHECK_Z1, OnCheckZ1)
	ON_BN_CLICKED(IDC_CHECK_ZBlue, OnCHECKZBlue)
	ON_BN_CLICKED(IDC_CHECK_ZGreen, OnCHECKZGreen)
	ON_BN_CLICKED(IDC_CHECK_ZRed, OnCHECKZRed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColoredMeshDlg message handlers
void CColoredMeshDlg::OnCancel() 
{
	OK = Standard_False;	
	CDialog::OnCancel();
}

void CColoredMeshDlg::OnOK() 
{
	OK = Standard_True;
	CDialog::OnOK();
}

BOOL CColoredMeshDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateData (TRUE);
	m_Rad3OnOff = Standard_True;
	m_CheckXRedOnOff = Standard_True;
	m_CheckXGreenOnOff = Standard_False;
	m_CheckXBlueOnOff = Standard_False;
	m_CheckYRedOnOff = Standard_False;
	m_CheckYGreenOnOff = Standard_True;
	m_CheckYBlueOnOff = Standard_False;
	m_CheckZRedOnOff = Standard_False;
	m_CheckZGreenOnOff = Standard_False;
	m_CheckZBlueOnOff = Standard_True;

	Colorization = 2;	

	UpdateData (FALSE);
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CColoredMeshDlg::OnRadio1() 
{
	Colorization = 0;	
}

void CColoredMeshDlg::OnRadio2() 
{
	Colorization = 1;	
}

void CColoredMeshDlg::OnRadio3() 
{
	Colorization = 2;	

}

void CColoredMeshDlg::OnCheckX1()
{
	X1OnOff = m_CheckX1OnOff != 0;
}

void CColoredMeshDlg::OnCHECKXBlue() 
{
	UpdateData (TRUE);
	m_CheckXRedOnOff = Standard_False;
	m_CheckXGreenOnOff = Standard_False;
	m_CheckXBlueOnOff = Standard_True;
	UpdateData (FALSE);
}

void CColoredMeshDlg::OnCHECKXGreen() 
{
	UpdateData (TRUE);
	m_CheckXRedOnOff = Standard_False;
	m_CheckXGreenOnOff = Standard_True;
	m_CheckXBlueOnOff = Standard_False;
	UpdateData (FALSE);
}

void CColoredMeshDlg::OnCHECKXRed() 
{
	UpdateData (TRUE);
	m_CheckXRedOnOff = Standard_True;
	m_CheckXGreenOnOff = Standard_False;
	m_CheckXBlueOnOff = Standard_False;
	UpdateData (FALSE);
}

void CColoredMeshDlg::OnCheckY1() 
{
	Y1OnOff = m_CheckY1OnOff != 0;
}

void CColoredMeshDlg::OnCHECKYBlue() 
{
	UpdateData (TRUE);
	m_CheckYRedOnOff = Standard_False;	
	m_CheckYGreenOnOff = Standard_False;	
	m_CheckYBlueOnOff = Standard_True;	
	UpdateData (FALSE);
}

void CColoredMeshDlg::OnCHECKYGreen() 
{
	UpdateData (TRUE);
	m_CheckYRedOnOff = Standard_False;	
	m_CheckYGreenOnOff = Standard_True;	
	m_CheckYBlueOnOff = Standard_False;	
	UpdateData (FALSE);
}

void CColoredMeshDlg::OnCHECKYRed() 
{
	UpdateData (TRUE);
	m_CheckYRedOnOff = Standard_True;	
	m_CheckYGreenOnOff = Standard_False;	
	m_CheckYBlueOnOff = Standard_False;	
	UpdateData (FALSE);
}

void CColoredMeshDlg::OnCheckZ1() 
{
	Z1OnOff	= m_CheckZ1OnOff != 0;
}

void CColoredMeshDlg::OnCHECKZBlue() 
{
	UpdateData (TRUE);
	m_CheckZRedOnOff = Standard_False;	
	m_CheckZGreenOnOff = Standard_False;	
	m_CheckZBlueOnOff = Standard_True;	
	UpdateData (FALSE);
}

void CColoredMeshDlg::OnCHECKZGreen() 
{
	UpdateData (TRUE);
	m_CheckZRedOnOff = Standard_False;	
	m_CheckZGreenOnOff = Standard_True;	
	m_CheckZBlueOnOff = Standard_False;	
	UpdateData (FALSE);
}

void CColoredMeshDlg::OnCHECKZRed() 
{
	UpdateData (TRUE);
	m_CheckZRedOnOff = Standard_True;	
	m_CheckZGreenOnOff = Standard_False;	
	m_CheckZBlueOnOff = Standard_False;	
	UpdateData (FALSE);
}
