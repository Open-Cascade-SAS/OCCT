#if !defined(AFX_FontPropertyPage_H__A815F7A5_D51B_11D1_8DDE_0800369C8A03__INCLUDED_)
#define AFX_FontPropertyPage_H__A815F7A5_D51B_11D1_8DDE_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FontPropertyPage.h : header file
//
#include "Aspect_FontMap.hxx"
/////////////////////////////////////////////////////////////////////////////
// CFontPropertyPage dialog

class CFontPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFontPropertyPage)

private :
    Handle(Aspect_FontMap) myFontMap;
private :
    Handle(V2d_Viewer) myViewer;
public:
    void SetViewer(Handle(V2d_Viewer) aViewer) {
        myViewer = aViewer;
        myFontMap = aViewer->FontMap();
    };

// Construction
public:
	CFontPropertyPage();
	~CFontPropertyPage();

// Dialog Data
	//{{AFX_DATA(CFontPropertyPage)
	enum { IDD = IDD_DIALOG_Font };
	CString	  m_FontMapSize;
	CTabCtrl  m_TabCtrl;
	CString	  m_CurrentEntryValue;
	CString	  m_CurrentEntryStyle;
	CString	  m_CurrentEntrySize;
	CString	  m_CurrentEntrySlant;
	CComboBox m_NewEntry_Type;
	double	m_NewEntrySize;
	double	m_NewEntrySlant;
	CString	m_NewEntryValue;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFontPropertyPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFontPropertyPage)
   	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFontMapTAB(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFontMapBUTTONNewEntryEditFont();
	afx_msg void OnFontMapBUTTONUpdateCurrent();
	afx_msg void OnSelchangeFontMapCOMBONewEntryType();
	afx_msg void OnFontMapBUTTONNewEntry();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
  void UpdateDisplay(int CurrentSelectionIndex);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FontPropertyPage_H__A815F7A5_D51B_11D1_8DDE_0800369C8A03__INCLUDED_)
