// FileSaveIgesDialog.h : header file
//

#if !defined(AFX_FILESAVECSFDBDIALOG_H__B7D13C78_AB88_11D1_B97B_444553540000__INCLUDED_)
#define AFX_FILESAVECSFDBDIALOG_H__B7D13C78_AB88_11D1_B97B_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MgtBRep_TriangleMode.hxx"
#include <res/OCC_Resource.h>

#include <Standard_Macro.hxx>

/////////////////////////////////////////////////////////////////////////////
// CFileSaveCSFDBDialog dialog

class Standard_EXPORT CFileSaveCSFDBDialog : public CFileDialog
{
// Construction
public:
	CFileSaveCSFDBDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileSaveCSFDBDialog)
	enum { IDD = IDD_FILESAVECSFDB };
	CComboBox	m_SaveTypeCombo;
	MgtBRep_TriangleMode	m_TriangleMode;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileSaveCSFDBDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnFileNameOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileSaveCSFDBDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILESAVECSFDBDIALOG_H__B7D13C78_AB88_11D1_B97B_444553540000__INCLUDED_)
