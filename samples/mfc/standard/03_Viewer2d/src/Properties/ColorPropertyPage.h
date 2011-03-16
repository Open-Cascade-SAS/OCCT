#if !defined(AFX_ColorPropertyPage_H__A815F7A3_D51B_11D1_8DDE_0800369C8A03__INCLUDED_)
#define AFX_ColorPropertyPage_H__A815F7A3_D51B_11D1_8DDE_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ColorPropertyPage.h : header file
//
#include "Aspect_ColorMap.hxx"

/////////////////////////////////////////////////////////////////////////////
// CColorPropertyPage dialog

class CColorPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CColorPropertyPage)

private :
    Handle(Aspect_ColorMap) myColorMap;
private :
    Handle(V2d_Viewer) myViewer;
public:
    void SetViewer(Handle(V2d_Viewer) aViewer) {
        myViewer = aViewer;
        myColorMap = aViewer->ColorMap();
    };


// Construction
public:
	CColorPropertyPage();
    ~CColorPropertyPage();

// Dialog Data
	//{{AFX_DATA(CColorPropertyPage)
	enum { IDD = IDD_DIALOG_Color };
	CComboBox	m_NewEntryColorNameCtrl;
	CTabCtrl	m_TabCtrl;
   	CString m_ColorMapSize ;
	CString m_ColorMapType ;
	CString	m_CurrentEntryRed;
	CString	m_CurrentEntryGreen;
	CString	m_CurrentEntryBlue;
	CString	m_NearsetColorName;
	double	m_NewEntryRed;
	double	m_NewEntryBlue;
	double	m_NewEntryGreen;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CColorPropertyPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CColorPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeColorMapTAB(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeColorMapCOMBONewEntryColorName();
	afx_msg void OnColorMapBUTTONUpdateCurrentEntry();
	afx_msg void OnColorMapBUTTONNewColorCubeColorMap();
	afx_msg void OnColorMapBUTTONNewGenericColorMap();
	afx_msg void OnColorMapBUTTONNewColorRampColorMap();
	afx_msg void OnColorMapBUTTONNewEntryEditColor();
	afx_msg void OnColorMapBUTTONAddNewEntry();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  void UpdateDisplay(int CurrentSelectionIndex);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ColorPropertyPage_H__A815F7A3_D51B_11D1_8DDE_0800369C8A03__INCLUDED_)
