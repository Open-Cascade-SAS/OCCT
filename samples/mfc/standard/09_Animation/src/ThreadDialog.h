#if !defined(AFX_THREADDIALOG_H__539445D8_1182_11D2_B8C9_0000F87A77C1__INCLUDED_)
#define AFX_THREADDIALOG_H__539445D8_1182_11D2_B8C9_0000F87A77C1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ThreadDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CThreadDialog dialog

class CThreadDialog : public CDialog
{
// Construction
public:
	CThreadDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CThreadDialog)
	enum { IDD = IDD_ThreadBoxDialog };
	UINT	m_Angle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThreadDialog)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CThreadDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CThreadDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THREADDIALOG_H__539445D8_1182_11D2_B8C9_0000F87A77C1__INCLUDED_)
