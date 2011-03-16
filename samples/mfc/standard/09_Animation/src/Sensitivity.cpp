// Sensitivity.cpp : implementation file
//

#include "stdafx.h"

#include "Sensitivity.h"

#include "AnimationApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSensitivity dialog


CSensitivity::CSensitivity(CWnd* pParent /*=NULL*/)
	: CDialog(CSensitivity::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSensitivity)
	m_SensFly = 0.0;
	m_SensTurn = 0.0;
	//}}AFX_DATA_INIT
}


void CSensitivity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSensitivity)
	DDX_Text(pDX, IDC_FLY, m_SensFly);
	DDV_MinMaxDouble(pDX, m_SensFly, 0., 10000000.);
	DDX_Text(pDX, IDC_TURN, m_SensTurn);
	DDV_MinMaxDouble(pDX, m_SensTurn, 0., 10000000.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSensitivity, CDialog)
	//{{AFX_MSG_MAP(CSensitivity)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, OnDeltaposSpin2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSensitivity message handlers

void CSensitivity::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if ( pNMUpDown->iDelta > 0 ) {
		m_SensFly  /= 1.1 ;
	}
	else {
		m_SensFly  *= 1.1 ;
	}
	UpdateData ( FALSE ) ;


	*pResult = 0;
}

void CSensitivity::OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if ( pNMUpDown->iDelta > 0 ) {
		m_SensTurn  /= 1.1 ;
	}
	else {
		m_SensTurn  *= 1.1 ;
	}
	UpdateData ( FALSE ) ;
	
	*pResult = 0;
}
