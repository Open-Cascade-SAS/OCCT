#if !defined(AFX_NEWBOXDLG_H__5A881DC0_7A5F_11D4_8D45_00AA00D10994__INCLUDED_)
#define AFX_NEWBOXDLG_H__5A881DC0_7A5F_11D4_8D45_00AA00D10994__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewBoxDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewBoxDlg dialog

class CNewBoxDlg : public CDialog
{
// Construction
public:
	void InitFields(Standard_Real x, Standard_Real y, Standard_Real z, Standard_Real w, Standard_Real l, Standard_Real h, const TCollection_ExtendedString &Name);
	CNewBoxDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewBoxDlg)
	enum { IDD = IDD_NEWBOX };
	double	m_h;
	double	m_l;
	CString	m_Name;
	double	m_w;
	double	m_x;
	double	m_y;
	double	m_z;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewBoxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewBoxDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWBOXDLG_H__5A881DC0_7A5F_11D4_8D45_00AA00D10994__INCLUDED_)
