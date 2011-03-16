#if !defined(AFX_OFFSETDLG_H__9E963234_B2CB_11D8_8CF3_00047571ABCA__INCLUDED_)
#define AFX_OFFSETDLG_H__9E963234_B2CB_11D8_8CF3_00047571ABCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// offsetdlg.h : header file
//

#include "Viewer3dDoc.h"

class CViewer3dDoc;
/////////////////////////////////////////////////////////////////////////////
// COffsetDlg dialog

class COffsetDlg : public CDialog
{
// Construction
public:
	void UpdateValues();
	void SetOffsets(Standard_Real theFactor, Standard_Real theUnits);
	COffsetDlg(	CViewer3dDoc* theDoc, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(COffsetDlg)
	enum { IDD = IDD_SETOFFSETS };
	CSliderCtrl	m_UnitsSlidCtrl;
	CSliderCtrl	m_FactorSlidCtrl;
	Standard_Real		m_Factor;
	Standard_Real		m_Units;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COffsetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COffsetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEditOffsetFactor();
	afx_msg void OnChangeEditOffsetUnits();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CViewer3dDoc* myDoc;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OFFSETDLG_H__9E963234_B2CB_11D8_8CF3_00047571ABCA__INCLUDED_)
