// ResultDialog.h :  header file
//

#if !defined(AFX_RESULTDIALOG_H__0307BDF3_AF53_11D1_8DAE_0800369C8A03__INCLUDED_)
#define AFX_RESULTDIALOG_H__0307BDF3_AF53_11D1_8DAE_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <stdafx.h>

#include "res\OCC_Resource.h"
#include <Standard_Macro.hxx>

/////////////////////////////////////////////////////////////////////////////
// CResultDialog dialog

class Standard_EXPORT CResultDialog : public CDialog
{

public:
	CResultDialog(CWnd* pParent = NULL);
	void SetTitle(const CString& aTitle);
	void SetText(const CString& aText);
	void GetText(CString& aText);
	void Empty();

// Dialog Data
	//{{AFX_DATA(CResultDialog)
	enum { IDD = IDD_ResultDialog };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResultDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResultDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnCopySelectionToClipboard();
	afx_msg void OnCopyAllToClipboard();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//Attributes
private:
	CRichEditCtrl * pEd;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTDIALOG_H__0307BDF3_AF53_11D1_8DAE_0800369C8A03__INCLUDED_)
