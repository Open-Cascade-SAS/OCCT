// FileSaveIgesDialog.cpp : implementation file
//

#include "stdafx.h"

#include "SaveCSFDBDlg.h"

#include <res\OCC_Resource.h>

/////////////////////////////////////////////////////////////////////////////
// CFileSaveCSFDBDialog dialog


CFileSaveCSFDBDialog::CFileSaveCSFDBDialog(CWnd* pParent /*=NULL*/)
	: CFileDialog(FALSE,_T("*.csfdb"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                _T("CSFDB Files (*.csfdb)|*.csfdb;|CSFDB Files (*.csf)|*.csf;||"),
				  pParent
#if (_MSC_VER < 1500)
               )
#else
               ,0,0)
#endif

{
	//{{AFX_DATA_INIT(CFileSaveCSFDBDialog)
	m_TriangleMode = MgtBRep_WithTriangle;
	//}}AFX_DATA_INIT

CString CASROOTValue;
CASROOTValue.GetEnvironmentVariable (L"CASROOT");
CString initdir = (CASROOTValue + "\\..\\data\\csfdb");

	m_ofn.lpstrInitialDir = initdir;

	m_ofn.Flags |= OFN_ENABLETEMPLATE;
	m_ofn.lpTemplateName = MAKEINTRESOURCE(CFileSaveCSFDBDialog::IDD);
	m_ofn.lpstrTitle = _T("Save as CSFDB File");

}


void CFileSaveCSFDBDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
    int TheTriangleMode = (int)m_TriangleMode;

	//{{AFX_DATA_MAP(CFileSaveCSFDBDialog)
	DDX_Control(pDX, IDC_FSaveCSFDB_Type, m_SaveTypeCombo);
	DDX_CBIndex(pDX, IDC_FSaveCSFDB_Type, TheTriangleMode);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CFileSaveCSFDBDialog, CFileDialog)
	//{{AFX_MSG_MAP(CFileSaveCSFDBDialog)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileSaveCSFDBDialog message handlers

BOOL CFileSaveCSFDBDialog::OnInitDialog() 
{
	
	BOOL bRet =	CFileDialog::OnInitDialog();
  m_SaveTypeCombo.InsertString(-1, L"WithTriangle");
  m_SaveTypeCombo.InsertString(-1, L"WithoutTriangle");
	m_SaveTypeCombo.SetCurSel(m_TriangleMode);

	return bRet;
}

BOOL CFileSaveCSFDBDialog::OnFileNameOK()
{
	ASSERT_VALID(this);
	UpdateData(TRUE);
	// Do not call Default() if you override
	return FALSE;
}
