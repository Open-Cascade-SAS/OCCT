#if !defined(AFX_TUNE_H__D7E45B53_AAD2_11D2_9E97_0800362A0F04__INCLUDED_)
#define AFX_TUNE_H__D7E45B53_AAD2_11D2_9E97_0800362A0F04__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Tune.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTune dialog

class CTune : public CDialog
{
// Construction
public:
	CTune(CWnd* pParent = NULL);   // standard constructor
	CView * m_pView;

// Dialog Data
	//{{AFX_DATA(CTune)
	enum { IDD = IDD_TUNE };
	double	m_dAngle;
	double	m_dFocus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTune)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTune)
	afx_msg void OnDeltaposSpinang(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinfoc(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeApperture();
	afx_msg void OnChangeFocdist();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TUNE_H__D7E45B53_AAD2_11D2_9E97_0800362A0F04__INCLUDED_)
