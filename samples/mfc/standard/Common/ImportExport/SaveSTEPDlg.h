#if !defined(AFX_FILESAVESTEPDIALOG_H__B7D13C78_AB88_11D1_B97B_444553540000__INCLUDED_)
#define AFX_FILESAVESTEPDIALOG_H__B7D13C78_AB88_11D1_B97B_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FileSaveIgesDialog.h : header file
//

#include "STEPControl_StepModelType.hxx"

#include <res\OCC_Resource.h>

#include <Standard_Macro.hxx>

/////////////////////////////////////////////////////////////////////////////
// CFileSaveSTEPDialog dialog

class Standard_EXPORT CFileSaveSTEPDialog : public CFileDialog
{
// Construction
public:
	CFileSaveSTEPDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileSaveSTEPDialog)
	enum { IDD = IDD_FILESAVESTEP };
	CComboBox	m_SaveTypeCombo;

	STEPControl_StepModelType	m_Cc1ModelType;

	int	m_DialogType;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileSaveSTEPDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnFileNameOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileSaveSTEPDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILESAVESTEPDIALOG_H__B7D13C78_AB88_11D1_B97B_444553540000__INCLUDED_)
