// DlgIsos.cpp : implementation file
//

#include "stdafx.h"

#include "DlgIsos.h"

#include "Viewer3dApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgIsos dialog


DlgIsos::DlgIsos(CWnd* pParent,
				 int nisou,
				 int nisov)
	: CDialog(DlgIsos::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgIsos)
	m_isou = nisou;
	m_isov = nisov;
	//}}AFX_DATA_INIT
}


void DlgIsos::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgIsos)
	DDX_Text(pDX, IDC_EDIT_ISOU, m_isou);
	DDX_Text(pDX, IDC_EDIT_ISOV, m_isov);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgIsos, CDialog)
	//{{AFX_MSG_MAP(DlgIsos)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgIsos message handlers
