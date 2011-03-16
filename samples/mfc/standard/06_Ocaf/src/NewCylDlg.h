#if !defined(AFX_NEWCYLDLG_H__FFA9686D_8580_11D4_8D47_00AA00D10994__INCLUDED_)
#define AFX_NEWCYLDLG_H__FFA9686D_8580_11D4_8D47_00AA00D10994__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewCylDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewCylDlg dialog

class CNewCylDlg : public CDialog
{
// Construction
public:
	void InitFields(Standard_Real x, Standard_Real y, Standard_Real z, Standard_Real r, Standard_Real h, const TCollection_ExtendedString &Name);
	CNewCylDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewCylDlg)
	enum { IDD = IDD_NEWCYL };
	double	m_h;
	CString	m_Name;
	double	m_r;
	double	m_x;
	double	m_y;
	double	m_z;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewCylDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewCylDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWCYLDLG_H__FFA9686D_8580_11D4_8D47_00AA00D10994__INCLUDED_)
