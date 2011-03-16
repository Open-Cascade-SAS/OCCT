#if !defined(AFX_OLOREDMESH_H__7CB26B1F_55EC_11D6_BD10_00A0C982B46F__INCLUDED_)
#define AFX_OLOREDMESH_H__7CB26B1F_55EC_11D6_BD10_00A0C982B46F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColoredMeshDlg.h : header file
//
#include <res\OCC_Resource.h>

/////////////////////////////////////////////////////////////////////////////
// CColoredMeshDlg dialog



class CColoredMeshDlg : public CDialog
{
// Construction
public:
	CColoredMeshDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CColoredMeshDlg)
	enum {IDD = IDD_COLORMESH };
	BOOL	OK;

	BOOL	m_Rad1OnOff;
	BOOL	m_Rad2OnOff;
	BOOL	m_Rad3OnOff;
	BOOL	m_CheckX1OnOff;
	BOOL	m_CheckXBlueOnOff;
	BOOL	m_CheckXGreenOnOff;
	BOOL	m_CheckXRedOnOff;
	BOOL	m_CheckY1OnOff;
	BOOL	m_CheckYBlueOnOff;
	BOOL	m_CheckYGreenOnOff;
	BOOL	m_CheckYRedOnOff;
	BOOL	m_CheckZ1OnOff;
	BOOL	m_CheckZBlueOnOff;
	BOOL	m_CheckZGreenOnOff;
	BOOL	m_CheckZRedOnOff;



		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColoredMeshDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColoredMeshDlg)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnCheckX1();
	afx_msg void OnCHECKXBlue();
	afx_msg void OnCHECKXGreen();
	afx_msg void OnCHECKXRed();
	afx_msg void OnCheckY1();
	afx_msg void OnCHECKYBlue();
	afx_msg void OnCHECKYGreen();
	afx_msg void OnCHECKYRed();
	afx_msg void OnCheckZ1();
	afx_msg void OnCHECKZBlue();
	afx_msg void OnCHECKZGreen();
	afx_msg void OnCHECKZRed();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	Standard_Boolean Custom;
int Colorization;
Standard_Boolean	X1OnOff;
Standard_Boolean XBlueOnOff;
Standard_Boolean XGreenOnOff;
Standard_Boolean XRedOnOff;
Standard_Boolean Y1OnOff;
Standard_Boolean YBlueOnOff;
Standard_Boolean YGreenOnOff;
Standard_Boolean YRedOnOff;
Standard_Boolean Z1OnOff;
Standard_Boolean ZBlueOnOff;
Standard_Boolean ZGreenOnOff;
Standard_Boolean ZRedOnOff;

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OLOREDMESH_H__7CB26B1F_55EC_11D6_BD10_00A0C982B46F__INCLUDED_)
