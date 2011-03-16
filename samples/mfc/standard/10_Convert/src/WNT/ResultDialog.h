#if !defined(AFX_RESULTDIALOG_H__6597303D_7F5B_11D5_BA4D_0060B0EE18EA__INCLUDED_)
#define AFX_RESULTDIALOG_H__6597303D_7F5B_11D5_BA4D_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResultDialog.h : header file
//

#include "resource.h"
#include <res\\OCC_Resource.h>

/////////////////////////////////////////////////////////////////////////////
// CResultDialog dialog

class CResultDialog : public CDialog
{
// Construction
public:
	void Initialize();
	CResultDialog(CWnd* pParent = NULL);   // standard constructor
	void SetTitle(LPCSTR aTitle);
	void SetText(LPCSTR aText);
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
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
	//{{AFX_MSG(CResultDialog)
	afx_msg void OnCopySelectionToClipboard();
	afx_msg void OnCopyAllToClipboard();
	virtual void OnCancel();

  // OnSize is a message handler of WM_SIZE messge, 
  // it is reimplemented in order to reposition the buttons
  // (keep them always in the bottom part of the dialog)
  // and resize the rich edit controls as user resized THIS dialog.
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
  int bw, bh; // button width and height

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTDIALOG_H__6597303D_7F5B_11D5_BA4D_0060B0EE18EA__INCLUDED_)
