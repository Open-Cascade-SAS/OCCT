// ThreadDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AnimationApp.h"
#include "ThreadDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThreadDialog dialog


CThreadDialog::CThreadDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CThreadDialog::IDD, pParent)
{
//	EnableAutomation();

	//{{AFX_DATA_INIT(CThreadDialog)
	m_Angle = 4;
	//}}AFX_DATA_INIT
}


void CThreadDialog::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void CThreadDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CThreadDialog)
	DDX_Text(pDX, IDC_Angle, m_Angle);
	DDV_MinMaxUInt(pDX, m_Angle, 1, 60);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CThreadDialog, CDialog)
	//{{AFX_MSG_MAP(CThreadDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CThreadDialog, CDialog)
	//{{AFX_DISPATCH_MAP(CThreadDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IThreadDialog to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {539445D7-1182-11D2-B8C9-0000F87A77C1}
static const IID IID_IThreadDialog =
{ 0x539445d7, 0x1182, 0x11d2, { 0xb8, 0xc9, 0x0, 0x0, 0xf8, 0x7a, 0x77, 0xc1 } };

BEGIN_INTERFACE_MAP(CThreadDialog, CDialog)
	INTERFACE_PART(CThreadDialog, IID_IThreadDialog, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThreadDialog message handlers
