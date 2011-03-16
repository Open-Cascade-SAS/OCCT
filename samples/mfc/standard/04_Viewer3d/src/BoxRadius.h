// BoxRadius.h : header file
//

#if !defined(AFX_BOXRADIUS_H__AD263FF3_54F3_11D1_8C40_00AA00D10994__INCLUDED_)
#define AFX_BOXRADIUS_H__AD263FF3_54F3_11D1_8C40_00AA00D10994__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// BoxRadius dialog

class BoxRadius : public CDialog
{
// Construction
public:
	BoxRadius(CWnd* pParent,double rad);   // standard constructor

// Dialog Data
	//{{AFX_DATA(BoxRadius)
	enum { IDD = IDD_RADIUS };
	CSpinButtonCtrl	m_spinradius;
	double	m_radius;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BoxRadius)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(BoxRadius)
	afx_msg void OnDeltaposSpinRadius(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOXRADIUS_H__AD263FF3_54F3_11D1_8C40_00AA00D10994__INCLUDED_)
