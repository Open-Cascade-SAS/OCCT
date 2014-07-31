// NewCylDlg.cpp : implementation file
//

#include "stdafx.h"

#include "NewCylDlg.h"

//#include "OcafApp.h"

/////////////////////////////////////////////////////////////////////////////
// CNewCylDlg dialog

CNewCylDlg::CNewCylDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewCylDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewCylDlg)
	m_h = 20.0;
	m_Name = _T("Cylinder");
	m_r = 10.0;
	m_x = 0.0;
	m_y = 0.0;
	m_z = 0.0;
	//}}AFX_DATA_INIT
}

void CNewCylDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewCylDlg)
	DDX_Text(pDX, IDC_H, m_h);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_R, m_r);
	DDX_Text(pDX, IDC_X, m_x);
	DDX_Text(pDX, IDC_Y, m_y);
	DDX_Text(pDX, IDC_Z, m_z);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewCylDlg, CDialog)
	//{{AFX_MSG_MAP(CNewCylDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewCylDlg message handlers

void CNewCylDlg::OnOK() 
{
	UpdateData(TRUE);
	if( (m_h<=Precision::Confusion()) || (m_r<=Precision::Confusion()) )
	{
		MessageBox (L"Radius and height of a cylinder should be srictly positives.", L"New box", MB_ICONEXCLAMATION);
		return;
	}
	
	CDialog::OnOK();
}

void CNewCylDlg::InitFields(Standard_Real x, Standard_Real y, Standard_Real z, Standard_Real r, Standard_Real h, const TCollection_ExtendedString &Name)
{
	m_x=x;
	m_y=y;
	m_z=z;
	m_r=r;
	m_h=h;
	TCollection_AsciiString AsciiName(Name);
	m_Name=AsciiName.ToCString();
}
