// BoxRadius.cpp : implementation file
//

#include "stdafx.h"
#include "Viewer3dApp.h"
#include "BoxRadius.h"

/////////////////////////////////////////////////////////////////////////////

BoxRadius::BoxRadius(CWnd* pParent,
					 double rad)
	: CDialog(BoxRadius::IDD, pParent)
{
	//{{AFX_DATA_INIT(BoxRadius)
	m_radius = rad;
	//}}AFX_DATA_INIT
}


void BoxRadius::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BoxRadius)
	DDX_Control(pDX, IDC_SPIN_RADIUS, m_spinradius);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_radius);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BoxRadius, CDialog)
	//{{AFX_MSG_MAP(BoxRadius)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RADIUS, OnDeltaposSpinRadius)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BoxRadius message handlers

BOOL BoxRadius::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_spinradius.SetRange(-10000,10000);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void BoxRadius::OnDeltaposSpinRadius(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (pNMUpDown->iDelta == 1)
		pNMUpDown->iDelta = 1;
	else 
		pNMUpDown->iDelta = -1;
	m_radius = m_radius + pNMUpDown->iDelta;
	UpdateData(FALSE);
	*pResult = 0;
}

