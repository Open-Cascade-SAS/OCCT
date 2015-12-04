#if !defined(AFX_ScaleDlg_H__1796AA04_63B7_4E46_B00A_D2DAC975CAF1__INCLUDED_)
#define AFX_ScaleDlg_H__1796AA04_63B7_4E46_B00A_D2DAC975CAF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScaleDlg.h : header file
//

#include "..\res\resource.h"
#include "Viewer3dView.h"

/////////////////////////////////////////////////////////////////////////////
// ScaleDlg dialog

class ScaleDlg : public CDialog
{
// Construction
public:
	ScaleDlg(Handle(V3d_View) Current_V3d_View, CViewer3dView* pView, CWnd* pParent = NULL); 

// Dialog Data
	//{{AFX_DATA(ScaleDlg)
	enum { IDD = IDD_SCALE };
	CSliderCtrl	m_ScaleXSlidCtrl;
	CSliderCtrl	m_ScaleYSlidCtrl;
	CSliderCtrl	m_ScaleZSlidCtrl;
	int	m_ScaleX;
	int	m_ScaleY;
	int	m_ScaleZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScaleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ScaleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditScaleX();
	afx_msg void OnChangeEditScaleY();
	afx_msg void OnChangeEditScaleZ();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	Handle(V3d_View) myCurrent_V3d_View;
	CViewer3dView* myView;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ScaleDlg_H__1796AA04_63B7_4E46_B00A_D2DAC975CAF1__INCLUDED_)
