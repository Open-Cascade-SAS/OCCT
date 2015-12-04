// AISDialogs.h : header file
//

#if !defined(AFX_AISNBRISOSDIALOG_H__AAD52722_6A63_11D1_8C62_00AA00D10994__INCLUDED_)
#define AFX_AISNBRISOSDIALOG_H__AAD52722_6A63_11D1_8C62_00AA00D10994__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CAISNbrIsosDialog dialog

#include <AIS_InteractiveContext.hxx>
#include "res\OCC_Resource.h"
#include <Standard_Macro.hxx>

/*
class CAISNbrIsosDialog : public CDialog
{
// Construction
public:
	CAISNbrIsosDialog(Handle(AIS_InteractiveContext) CurrentIC,
					  CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAISNbrIsosDialog)
	enum { IDD = IDD_AISNBRISOS };
	int		m_Isosu;
	int		m_Isosv;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAISNbrIsosDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAISNbrIsosDialog)
	afx_msg void OnDeltaposSpinaisisosu(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinaisisosv(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    void UpdateIsos ();

	Handle(AIS_InteractiveContext) myCurrentIC;

};


/////////////////////////////////////////////////////////////////////////////
// CDevCoeffDialog dialog

#include <AIS_InteractiveContext.hxx>

class CDevCoeffDialog : public CDialog
{
// Construction
public:
	CDevCoeffDialog(Handle(AIS_InteractiveContext) CurrentIC,
					CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDevCoeffDialog)
	enum { IDD = IDD_AISDC };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDevCoeffDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDevCoeffDialog)
	afx_msg void OnDcBig();
	afx_msg void OnDcDefault();
	afx_msg void OnDcSmall();
	afx_msg void OnDoubleclickedDcBig();
	afx_msg void OnDoubleclickedDcDefault();
	afx_msg void OnDoubleclickedDcSmall();
	afx_msg void OnDcVbig();
	afx_msg void OnDoubleclickedDcVbig();
	afx_msg void OnDcVsmall();
	afx_msg void OnDoubleclickedDcVsmall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    void RedisplaySelected ();

	Handle(AIS_InteractiveContext) myCurrentIC;
};

*/
class Standard_EXPORT CDialogMaterial : public CDialog
{
// Construction
public:
	CDialogMaterial(Handle(AIS_InteractiveContext) CurrentIC,
		            CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogMaterial)
	enum { IDD = IDD_AISMATERIAL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogMaterial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogMaterial)
   	//afx_msg void OnMaterial(UINT nID);
	afx_msg void OnObjectMaterialAluminium();
	afx_msg void OnObjectMaterialBrass();
	afx_msg void OnObjectMaterialBronze();
	afx_msg void OnObjectMaterialChrome();
	afx_msg void OnObjectMaterialCopper();
	afx_msg void OnObjectMaterialGold();
	afx_msg void OnObjectMaterialJade();
	afx_msg void OnObjectMaterialMetalized();
	afx_msg void OnObjectMaterialNeonGNC();
	afx_msg void OnObjectMaterialNeonPHC();
	afx_msg void OnObjectMaterialObsidian();
	afx_msg void OnObjectMaterialPewter();
	afx_msg void OnObjectMaterialPlastic();
	afx_msg void OnObjectMaterialPlaster();
	afx_msg void OnObjectMaterialSatin();
	afx_msg void OnObjectMaterialShinyPlastic();
	afx_msg void OnObjectMaterialSilver();
	afx_msg void OnObjectMaterialSteel();
	afx_msg void OnObjectMaterialStone();
	afx_msg void OnObjectMaterialDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetMaterial (Graphic3d_NameOfMaterial Material);

	Handle(AIS_InteractiveContext) myCurrentIC;

};

class Standard_EXPORT CDialogTransparency : public CDialog
{
// Construction
public:
	CDialogTransparency(Handle(AIS_InteractiveContext) CurrentIC, 
		                CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogTransparency)
	enum { IDD = IDD_AISTRANSPARENCY };
	int	m_TransValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogTransparency)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogTransparency)
	afx_msg void OnDeltaposSpinaistransp(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditaistransp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	Handle(AIS_InteractiveContext) myCurrentIC;

};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AISNBRISOSDIALOG_H__AAD52722_6A63_11D1_8C62_00AA00D10994__INCLUDED_)
