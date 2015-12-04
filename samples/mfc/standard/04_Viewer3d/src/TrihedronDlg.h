#if !defined(AFX_TRIHEDRONDLG_H__1917B30F_3102_11D6_BD0D_00A0C982B46F__INCLUDED_)
#define AFX_TRIHEDRONDLG_H__1917B30F_3102_11D6_BD0D_00A0C982B46F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrihedronDlg.h : header file
//

#include "Viewer3dDoc.h"

#include <V3d_View.hxx>
#include <Quantity_NameOfColor.hxx>

#include "..\res\resource.h"
/////////////////////////////////////////////////////////////////////////////
// CTrihedronDlg dialog

class CTrihedronDlg : public CDialog
{
// Construction
public:
	CTrihedronDlg(CWnd* pParent = NULL);   // standard constructor
	CTrihedronDlg(Handle(V3d_View) Current_V3d_View, CViewer3dDoc* pDoc, CWnd* pParent = NULL);   // standard constructor


// Dialog Data
	//{{AFX_DATA(CTrihedronDlg)
	enum { IDD = IDD_TRIHEDRON };
	CComboBox m_ComboTrihedronPosList;
	CComboBox m_ComboTrihedronColorList;
	double	m_TrihedronScale;
	CSpinButtonCtrl	m_SpinTrihedronScale;
	Quantity_NameOfColor m_Color;
	Aspect_TypeOfTriedronPosition m_Position;


		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrihedronDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTrihedronDlg)
	afx_msg void OnSelchangeCombotrihedrcolor();
	afx_msg void OnSelchangeCombotrihedrpos();
	afx_msg void OnChangeEdittrihedrscale();
	afx_msg void OnDeltaposSpintrihedrscale(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	Handle(V3d_View) myCurrent_V3d_View;
	CViewer3dDoc* myDoc;
	Quantity_NameOfColor Color ;
	Aspect_TypeOfTriedronPosition Position;
/*
public:

static	Standard_Boolean Initialisation;
static	Standard_Boolean GetInitialisation() {return Initialisation;};
*/

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIHEDRONDLG_H__1917B30F_3102_11D6_BD0D_00A0C982B46F__INCLUDED_)
