// CircularGrid.h : header file
//

#if !defined(AFX_CIRCULARGRID_H__338883C5_510A_11D1_A4A8_00C04FB15CA3__INCLUDED_)
#define AFX_CIRCULARGRID_H__338883C5_510A_11D1_A4A8_00C04FB15CA3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <res\\OCC_Resource.h>
#include <Standard_Macro.hxx>

/////////////////////////////////////////////////////////////////////////////
// CCircularGrid dialog

class Standard_EXPORT CCircularGrid : public CDialog
{
// Construction
public:
	CCircularGrid(CWnd* pParent = NULL);   // standard constructor
    void SetViewer(Handle(V3d_Viewer) aViewer) { myViewer = aViewer; } ;
	void UpdateValues();
// Dialog Data
	//{{AFX_DATA(CCircularGrid)
	enum { IDD = IDD_GrilleCirculaire };
	double	m_RotationAngle;
	double	m_XOrigin;
	double	m_YOrigin;
	double	m_RadiusStep;
	int 	m_DivisionNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCircularGrid)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCircularGrid)
	virtual void OnCancel();
	afx_msg void OnUpdateCircGridDivNumber();
	afx_msg void OnUpdateCircGridRotationAngle();
	afx_msg void OnUpdateCircGridXOrigin();
	afx_msg void OnUpdateCircGridYorigin();
	afx_msg void OnUpdateCirctGridRadiusStep();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
    void UpdateDialogData();

private :
	Handle(V3d_Viewer)  myViewer;

    Standard_Real     SavedXOrigin,SavedYOrigin,SavedRadiusStep;
	Standard_Integer    SavedDivisionNumber;
    Standard_Real SavedRotationAngle;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CIRCULARGRID_H__338883C5_510A_11D1_A4A8_00C04FB15CA3__INCLUDED_)
