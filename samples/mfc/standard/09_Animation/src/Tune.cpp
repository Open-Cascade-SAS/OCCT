// Tune.cpp : implementation file
//

#include "stdafx.h"

#include "Tune.h"

#include "AnimationApp.h"
#include "Animationdoc.h"
#include "AnimationView3D.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTune dialog


CTune::CTune(CWnd* pParent /*=NULL*/)
	: CDialog(CTune::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTune)
	m_dAngle = 0.0;
	m_dFocus = 0.0;
	//}}AFX_DATA_INIT
}


void CTune::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTune)
	DDX_Text(pDX, IDC_APPERTURE, m_dAngle);
	DDV_MinMaxDouble(pDX, m_dAngle, 5., 179.);
	DDX_Text(pDX, IDC_FOCDIST, m_dFocus);
	DDV_MinMaxDouble(pDX, m_dFocus, 1.e-003, 10000000.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTune, CDialog)
	//{{AFX_MSG_MAP(CTune)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINANG, OnDeltaposSpinang)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINFOC, OnDeltaposSpinfoc)
	ON_EN_CHANGE(IDC_APPERTURE, OnChangeApperture)
	ON_EN_CHANGE(IDC_FOCDIST, OnChangeFocdist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTune message handlers

void CTune::OnDeltaposSpinang(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here

	if ( pNMUpDown->iDelta > 0 ) {
		if ( m_dAngle > 2. ) 
     		m_dAngle -= 1. ;
	}
	else {
		if ( m_dAngle < 178. ) 
    		m_dAngle += 1 ;
	}
	UpdateData ( FALSE ) ;
	
    OnChangeApperture() ;

	*pResult = 0;
}

void CTune::OnDeltaposSpinfoc(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here

	if ( pNMUpDown->iDelta > 0 ) {
		m_dFocus  /= 1.1 ;
	}
	else {
		m_dFocus *= 1.1 ;
	}
	UpdateData ( FALSE ) ;
	
    OnChangeFocdist() ;

	*pResult = 0;
}

void CTune::OnChangeApperture() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here

	( (CAnimationView3D *) m_pView) ->SetFocal ( m_dFocus , m_dAngle ) ;
}

void CTune::OnChangeFocdist() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	
	( (CAnimationView3D *) m_pView) ->SetFocal ( m_dFocus , m_dAngle ) ;
}

BOOL CTune::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::OnCommand(wParam, lParam);
}

BOOL CTune::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTune::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}
