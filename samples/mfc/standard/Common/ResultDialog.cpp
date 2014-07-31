// ResultDialog.cpp : implementation file
//

#include <stdafx.h>

#include "ResultDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CResultDialog dialog


CResultDialog::CResultDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CResultDialog::IDD, pParent)
{

}


void CResultDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResultDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResultDialog, CDialog)
	//{{AFX_MSG_MAP(CResultDialog)
	ON_BN_CLICKED(IDC_CopySelectionToClipboard, OnCopySelectionToClipboard)
	ON_BN_CLICKED(IDC_CopyAllToClipboard, OnCopyAllToClipboard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultDialog message handlers

void CResultDialog::Empty()
{
   pEd = (CRichEditCtrl *) GetDlgItem (IDC_RICHEDIT_ResultDialog);
   //pEd->Clear();
   pEd->SetWindowText (L"");
}

void CResultDialog::SetText(const CString & aText)
{
   pEd = (CRichEditCtrl *) GetDlgItem (IDC_RICHEDIT_ResultDialog);
   pEd->SetWindowText(aText);
}

void CResultDialog::GetText(CString & aText)
{
   pEd = (CRichEditCtrl *) GetDlgItem (IDC_RICHEDIT_ResultDialog);
   pEd->GetWindowText(aText);
}

BOOL CResultDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	pEd = (CRichEditCtrl *) GetDlgItem (IDC_RICHEDIT_ResultDialog);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CResultDialog::SetTitle(const CString & aTitle)
{
  SetWindowText(aTitle);
}

void CResultDialog::OnCopySelectionToClipboard() 
{
	// TODO: Add your control notification handler code here
	pEd = (CRichEditCtrl *) GetDlgItem (IDC_RICHEDIT_ResultDialog);
	pEd->Copy( );
}

void CResultDialog::OnCopyAllToClipboard() 
{
	// TODO: Add your control notification handler code here
	pEd = (CRichEditCtrl *) GetDlgItem (IDC_RICHEDIT_ResultDialog);
    CHARRANGE CurrentSel;
    pEd->GetSel( CurrentSel );

	pEd->SetSel(0,-1 );
	pEd->Copy( );
	
    pEd->SetSel( CurrentSel );
}



