// ShadingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AnimationApp.h"
#include "ShadingDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShadingDialog dialog


CShadingDialog::CShadingDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CShadingDialog::IDD, pParent)
{
//	EnableAutomation();

	//{{AFX_DATA_INIT(CShadingDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CShadingDialog::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void CShadingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShadingDialog)
	DDX_Control(pDX, IDC_SLIDER1, m_Slide);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShadingDialog, CDialog)
	//{{AFX_MSG_MAP(CShadingDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CShadingDialog, CDialog)
	//{{AFX_DISPATCH_MAP(CShadingDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IShadingDialog to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {02819A09-10F2-11D2-B8C4-0000F87A77C1}
static const IID IID_IShadingDialog =
{ 0x2819a09, 0x10f2, 0x11d2, { 0xb8, 0xc4, 0x0, 0x0, 0xf8, 0x7a, 0x77, 0xc1 } };

BEGIN_INTERFACE_MAP(CShadingDialog, CDialog)
	INTERFACE_PART(CShadingDialog, IID_IShadingDialog, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShadingDialog message handlers

BOOL CShadingDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

    m_Slide.SetRange(0,100);
    m_Slide.SetTicFreq(100);
    m_Slide.SetPos(myvalue);

	//UpdateData(false);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CShadingDialog::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	// store the position in my variable
    myvalue=  m_Slide.GetPos();
	
	CDialog::OnOK();
}
