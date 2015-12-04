// ShadingModelDlg.h : header file
//
#if !defined(AFX_SHADINGMODELDLG_H__4DEFD9F9_61FC_11D3_8D0A_00AA00D10994__INCLUDED_)
#define AFX_SHADINGMODELDLG_H__4DEFD9F9_61FC_11D3_8D0A_00AA00D10994__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "..\res\resource.h"

/////////////////////////////////////////////////////////////////////////////
// CShadingModelDlg dialog

class CShadingModelDlg : public CDialog
{
// Construction
public:
	CShadingModelDlg(Handle(V3d_View) Current_V3d_View, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShadingModelDlg)
	enum { IDD = IDD_SHADINGMODEL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShadingModelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShadingModelDlg)
	afx_msg void OnShadingmodelColor();
	afx_msg void OnShadingmodelFlat();
	afx_msg void OnShadingmodelGouraud();
	afx_msg void OnShadingmodelPhong();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	Handle(V3d_View)	myCurrent_V3d_View;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHADINGMODELDLG_H__4DEFD9F9_61FC_11D3_8D0A_00AA00D10994__INCLUDED_)
