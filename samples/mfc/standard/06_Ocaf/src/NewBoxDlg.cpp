// NewBoxDlg.cpp : implementation file
//

#include "stdafx.h"

#include "NewBoxDlg.h"

//#include "OcafApp.h"

/////////////////////////////////////////////////////////////////////////////
// CNewBoxDlg dialog

CNewBoxDlg::CNewBoxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewBoxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewBoxDlg)
	m_h = 10.0;
	m_l = 15.0;
	m_Name = _T("Box");
	m_w = 10.0;
	m_x = 0.0;
	m_y = 0.0;
	m_z = 0.0;
	//}}AFX_DATA_INIT
}

void CNewBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewBoxDlg)
	DDX_Text(pDX, IDC_H, m_h);
	DDX_Text(pDX, IDC_L, m_l);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_W, m_w);
	DDX_Text(pDX, IDC_X, m_x);
	DDX_Text(pDX, IDC_Y, m_y);
	DDX_Text(pDX, IDC_Z, m_z);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewBoxDlg, CDialog)
	//{{AFX_MSG_MAP(CNewBoxDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewBoxDlg message handlers

void CNewBoxDlg::OnOK() 
{
	UpdateData(TRUE);
	if( (m_h<=Precision::Confusion()) || (m_l<=Precision::Confusion()) || (m_w<=Precision::Confusion()) )
	{
		MessageBox (L"Length, height and width of a box should be srictly positives.", L"New box", MB_ICONEXCLAMATION);
		return;
	}
	
	CDialog::OnOK();
}

void CNewBoxDlg::InitFields(Standard_Real x, Standard_Real y, Standard_Real z, Standard_Real w, Standard_Real l, Standard_Real h, const TCollection_ExtendedString &Name)
{
	m_x=x;
	m_y=y;
	m_z=z;
	m_w=w;
	m_l=l;
	m_h=h;
	TCollection_AsciiString AsciiName(Name);
	m_Name=AsciiName.ToCString();
}
