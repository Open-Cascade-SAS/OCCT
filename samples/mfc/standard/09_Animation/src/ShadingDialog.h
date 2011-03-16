#if !defined(AFX_SHADINGDIALOG_H__02819A0A_10F2_11D2_B8C4_0000F87A77C1__INCLUDED_)
#define AFX_SHADINGDIALOG_H__02819A0A_10F2_11D2_B8C4_0000F87A77C1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ShadingDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShadingDialog dialog

class CShadingDialog : public CDialog
{
// Construction
public:
	CShadingDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShadingDialog)
	enum { IDD = IDD_ShadingBoxDialog };
	CSliderCtrl	m_Slide;
	int myvalue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShadingDialog)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShadingDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CShadingDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHADINGDIALOG_H__02819A0A_10F2_11D2_B8C4_0000F87A77C1__INCLUDED_)
