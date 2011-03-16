// FileSaveStepDialog.cpp : implementation file
//

#include "stdafx.h"

#include "SaveSTEPDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CFileSaveSTEPDialog dialog


CFileSaveSTEPDialog::CFileSaveSTEPDialog(CWnd* pParent /*=NULL*/)
	: CFileDialog(FALSE,_T("*.STEP"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                _T("STEP Files (*.step)|*.step;|STEP Files (*.stp)|*.stp;||"),
				  pParent)



//dlg.m_ofn.lpstrInitialDir = initdir;

{
	//{{AFX_DATA_INIT(CFileSaveSTEPDialog)
	m_Cc1ModelType = STEPControl_ManifoldSolidBrep;
	//}}AFX_DATA_INIT

	TCHAR tchBuf[80];

CString CASROOTValue = ((GetEnvironmentVariable("CASROOT", tchBuf, 80) > 0) ? tchBuf : NULL); 
CString initdir = (CASROOTValue + "\\..\\data\\step");

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

        if (m_Cc1ModelType==STEPControl_ManifoldSolidBrep) m_DialogType=0;
        if (m_Cc1ModelType==STEPControl_BrepWithVoids) Standard_Failure::Raise("unsupported enumeration terme");
        if (m_Cc1ModelType==STEPControl_FacetedBrep) m_DialogType=1;
        if (m_Cc1ModelType==STEPControl_FacetedBrepAndBrepWithVoids) Standard_Failure::Raise("unsupported enumeration terme");
        if (m_Cc1ModelType==STEPControl_ShellBasedSurfaceModel) m_DialogType=2;
        if (m_Cc1ModelType==STEPControl_GeometricCurveSet) m_DialogType=3;

		}
	//{{AFX_DATA_MAP(CFileSaveSTEPDialog)
	DDX_Control(pDX, IDC_FSaveSTEP_Type, m_SaveTypeCombo);
	DDX_CBIndex(pDX, IDC_FSaveSTEP_Type, m_DialogType );
	//}}AFX_DATA_MAP

    if (pDX->m_bSaveAndValidate)
        {

        if (m_DialogType==0) m_Cc1ModelType=STEPControl_ManifoldSolidBrep;

        if (m_DialogType==1) m_Cc1ModelType=STEPControl_FacetedBrep;

        if (m_DialogType==2) m_Cc1ModelType=STEPControl_ShellBasedSurfaceModel;
        if (m_DialogType==3) m_Cc1ModelType=STEPControl_GeometricCurveSet;

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
	m_SaveTypeCombo.InsertString(-1,"ManifoldSolidBrep");

	m_SaveTypeCombo.InsertString(-1,"FacetedBrep");

	m_SaveTypeCombo.InsertString(-1,"ShellBasedSurfaceModel");
	m_SaveTypeCombo.InsertString(-1,"GeometricCurveSet");
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
