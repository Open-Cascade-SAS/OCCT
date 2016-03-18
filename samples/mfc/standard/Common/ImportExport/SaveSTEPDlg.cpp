// FileSaveStepDialog.cpp : implementation file
//

#include "stdafx.h"

#include "SaveSTEPDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CFileSaveSTEPDialog dialog


CFileSaveSTEPDialog::CFileSaveSTEPDialog(CWnd* pParent /*=NULL*/)
	: CFileDialog(FALSE,_T("*.STEP"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                _T("STEP Files (*.step)|*.step;|STEP Files (*.stp)|*.stp;||"),
				  pParent
#if (_MSC_VER < 1500)
               )
#else
               ,0,0)
#endif 


//dlg.m_ofn.lpstrInitialDir = initdir;

{
	//{{AFX_DATA_INIT(CFileSaveSTEPDialog)
	m_Cc1ModelType = STEPControl_AsIs;
	//}}AFX_DATA_INIT

CString SHAREPATHValue;
SHAREPATHValue.GetEnvironmentVariable (L"CSF_OCCTDataPath");
CString initdir = (SHAREPATHValue + "\\step");

	m_ofn.lpstrInitialDir = initdir;
	m_ofn.Flags |= OFN_ENABLETEMPLATE;
	m_ofn.lpTemplateName = MAKEINTRESOURCE(CFileSaveSTEPDialog::IDD);
	m_ofn.lpstrTitle = _T("Save as STEP File");
}

void CFileSaveSTEPDialog::DoDataExchange(CDataExchange* pDX)
{
  CFileDialog::DoDataExchange(pDX);
  if (!pDX->m_bSaveAndValidate)
  {
    m_DialogType = m_Cc1ModelType;
  }

  //{{AFX_DATA_MAP(CFileSaveSTEPDialog)
  DDX_Control(pDX, IDC_FSaveSTEP_Type, m_SaveTypeCombo);
  DDX_CBIndex(pDX, IDC_FSaveSTEP_Type, m_DialogType );
  //}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
  {
    m_Cc1ModelType = (STEPControl_StepModelType)m_DialogType;
  }
}

BEGIN_MESSAGE_MAP(CFileSaveSTEPDialog, CFileDialog)
	//{{AFX_MSG_MAP(CFileSaveSTEPDialog)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileSaveSTEPDialog message handlers

BOOL CFileSaveSTEPDialog::OnInitDialog() 
{	
  BOOL bRet =	CFileDialog::OnInitDialog();

  m_SaveTypeCombo.InsertString(-1, L"As Is");
  m_SaveTypeCombo.InsertString(-1, L"Manifold Solid BRep");
  m_SaveTypeCombo.InsertString(-1, L"BRep With Voids");
  m_SaveTypeCombo.InsertString(-1, L"Faceted BRep");
  m_SaveTypeCombo.InsertString(-1, L"Faceted BRep With Voids");
  m_SaveTypeCombo.InsertString(-1, L"Shell Based Surface Model");
  m_SaveTypeCombo.InsertString(-1, L"Geometric Curve Set");
  m_SaveTypeCombo.SetCurSel(m_DialogType);

  return bRet;
}

BOOL CFileSaveSTEPDialog::OnFileNameOK()
{
	ASSERT_VALID(this);
	UpdateData(TRUE);

	// Do not call Default() if you override
	return FALSE;
}
