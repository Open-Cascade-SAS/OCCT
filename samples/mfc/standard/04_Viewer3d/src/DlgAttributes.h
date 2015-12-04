// DlgAttributes.h : header file
//

#if !defined(AFX_DLGATTRIBUTES_H__AF548353_9EF6_11D1_A47D_00C095ECDA78__INCLUDED_)
#define AFX_DLGATTRIBUTES_H__AF548353_9EF6_11D1_A47D_00C095ECDA78__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <AIS_InteractiveContext.hxx>
#include <ResultDialog.h>

/////////////////////////////////////////////////////////////////////////////
// DlgAttributes dialog

class DlgAttributes : public CDialog
{
// Construction
public:
	void Set(Handle(AIS_InteractiveContext)& acontext, CResultDialog& aResultDialog);
	DlgAttributes(CWnd* pParent=NULL);

// Dialog Data
	//{{AFX_DATA(DlgAttributes)
	enum { IDD = IDD_ATTRIBUTES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgAttributes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgAttributes)
	afx_msg void OnColor();
	afx_msg void OnWireframe();
	afx_msg void OnTransparency() ;
	afx_msg void OnShading();
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

	afx_msg void OnUpdateObjectTransparency(CCmdUI* pCmdUI) ;


	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void SetMaterial (Graphic3d_NameOfMaterial Material);

private:
	Handle(AIS_InteractiveContext) myAISContext;
	CResultDialog* myCResultDialog;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGATTRIBUTES_H__AF548353_9EF6_11D1_A47D_00C095ECDA78__INCLUDED_)
