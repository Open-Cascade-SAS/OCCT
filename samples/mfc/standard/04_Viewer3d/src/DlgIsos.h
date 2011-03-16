// DlgIsos.h : header file
//

#include "stdafx.h"

#if !defined(AFX_DLGISOS_H__AD159C33_9EDC_11D1_A47D_00C095ECDA78__INCLUDED_)
#define AFX_DLGISOS_H__AD159C33_9EDC_11D1_A47D_00C095ECDA78__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// DlgIsos dialog

class DlgIsos : public CDialog
{
// Construction
public:
	DlgIsos(CWnd* pParent,
			int nisou,
			int nisov);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DlgIsos)
	enum { IDD = IDD_ISOS };
	int		m_isou;
	int		m_isov;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgIsos)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgIsos)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGISOS_H__AD159C33_9EDC_11D1_A47D_00C095ECDA78__INCLUDED_)
