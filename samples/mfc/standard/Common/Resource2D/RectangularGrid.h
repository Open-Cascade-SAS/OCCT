// RectangularGrid.h : header file
//

#if !defined(AFX_RECTANGULARGRID_H__8FB85997_50FE_11D1_A4A8_00C04FB15CA3__INCLUDED_)
#define AFX_RECTANGULARGRID_H__8FB85997_50FE_11D1_A4A8_00C04FB15CA3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <res\\OCC_Resource.h>
#include <Standard_Macro.hxx>

/////////////////////////////////////////////////////////////////////////////
// CRectangularGrid dialog

class Standard_EXPORT CRectangularGrid : public CDialog
{
// Construction
public:
	CRectangularGrid(CWnd* pParent = NULL);   // standard constructor
    void SetViewer(Handle(V3d_Viewer) aViewer) { myViewer = aViewer; } ;
	void UpdateValues();

// Dialog Data
	//{{AFX_DATA(CRectangularGrid)
	enum { IDD = IDD_GrilleRectangulaire };
	double	m_XOrigin;
	double	m_YOrigin;
	double	m_XStep;
	double	m_YStep;
	double	m_RotationAngle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRectangularGrid)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRectangularGrid)
	afx_msg void OnUpdateRectGridRotationangle();
	afx_msg void OnUpdateRectGridXOrigin();
	afx_msg void OnUpdateRectGridXStep();
	afx_msg void OnUpdateRectGridYorigin();
	afx_msg void OnUpdateRectGridYStep();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
    void UpdateDialogData();

private :
  Handle(V3d_Viewer) myViewer;

  Standard_Real SavedXOrigin, SavedYOrigin, SavedXStep, SavedYStep;
  Standard_Real SavedRotationAngle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECTANGULARGRID_H__8FB85997_50FE_11D1_A4A8_00C04FB15CA3__INCLUDED_)
