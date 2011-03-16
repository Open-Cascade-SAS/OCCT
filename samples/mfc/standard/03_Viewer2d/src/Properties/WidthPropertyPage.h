#if !defined(AFX_WIDTHPROPERTY_H__0F6C04B3_D5DB_11D1_8DDF_0800369C8A03__INCLUDED_)
#define AFX_WIDTHPROPERTY_H__0F6C04B3_D5DB_11D1_8DDF_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WidthProperty.h : header file
//
#include "Aspect_WidthMap.hxx"
/////////////////////////////////////////////////////////////////////////////
// CWidthPropertyPage dialog

class CWidthPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CWidthPropertyPage)

private :
    Handle(Aspect_WidthMap) myWidthMap;
private :
    Handle(V2d_Viewer) myViewer;
public:
    void SetViewer(Handle(V2d_Viewer) aViewer) {
        myViewer = aViewer;
        myWidthMap = aViewer->WidthMap();
    };

// Construction
public:
	CWidthPropertyPage();
	~CWidthPropertyPage();

// Dialog Data
	//{{AFX_DATA(CWidthPropertyPage)
	enum { IDD = IDD_DIALOG_Width };
	CString	  m_WidthMapSize;
	CTabCtrl  m_TabCtrl;
	CString	  m_EntryType;
	CString	  m_EntryWidth;
	CComboBox m_NewEntryType;
	CEdit	  m_NewEntryWidthControl;
	double	  m_NewEntryWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWidthPropertyPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWidthPropertyPage)
	afx_msg void OnSelchangeDialogWidthTAB(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnWidthMapBUTTONAddNewEntry();
	afx_msg void OnSelchangeWidthMapCOMBONewEntryType();
	afx_msg void OnWidthMapBUTTONUpdateCurrentEntry();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  void UpdateDisplay(int CurrentSelectionIndex);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIDTHPROPERTY_H__0F6C04B3_D5DB_11D1_8DDF_0800369C8A03__INCLUDED_)
