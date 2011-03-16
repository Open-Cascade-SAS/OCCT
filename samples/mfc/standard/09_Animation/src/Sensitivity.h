#if !defined(AFX_SENSITIVITY_H__F3897393_7D55_11D2_8E5F_0800369C8A03__INCLUDED_)
#define AFX_SENSITIVITY_H__F3897393_7D55_11D2_8E5F_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Sensitivity.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSensitivity dialog

class CSensitivity : public CDialog
{
// Construction
public:
	CSensitivity(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSensitivity)
	enum { IDD = IDD_SENS };
	double	m_SensFly;
	double	m_SensTurn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSensitivity)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSensitivity)
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENSITIVITY_H__F3897393_7D55_11D2_8E5F_0800369C8A03__INCLUDED_)
