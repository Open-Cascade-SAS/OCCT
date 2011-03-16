#if !defined(AFX_TypePropertyPage_H__B6E7AAD3_DB72_11D1_8DE5_0800369C8A03__INCLUDED_)
#define AFX_TypePropertyPage_H__B6E7AAD3_DB72_11D1_8DE5_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TypePropertyPage.h : header file
//

#include "V2d_Viewer.hxx"
#include "TColQuantity_HArray1OfLength.hxx"

/////////////////////////////////////////////////////////////////////////////
// CTypePropertyPage dialog

class CTypePropertyPage : public CPropertyPage
{
private :
    Handle(Aspect_TypeMap) myTypeMap;
private :
    Handle(V2d_Viewer) myViewer;
public:
    void SetViewer(Handle(V2d_Viewer) aViewer) {
        myViewer = aViewer;
        myTypeMap = aViewer->TypeMap();
    };

	DECLARE_DYNCREATE(CTypePropertyPage)

// Construction
public:
	CTypePropertyPage();
	~CTypePropertyPage();

// Dialog Data
	//{{AFX_DATA(CTypePropertyPage)
	enum { IDD = IDD_DIALOG_Type };
	CTabCtrl	m_TabCtrl;
	CComboBox	m_NewEntryControl;

	CString	m_TypeMapSize;
	CString	m_CurrentEntryStyle;
	CString	m_CurrentEntryValue;
	CEdit	m_NewEntryValueControl;
	CString	m_NewEntryValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTypePropertyPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTypePropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnFontMapBUTTONNewEntry();
	afx_msg void OnFontMapBUTTONUpdateCurrent();
	afx_msg void OnChangeTypeMapEDITNewEntryValue();
	afx_msg void OnSelchangeTypeMapTAB(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeTypeMapCOMBONewEntryStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
  void UpdateDisplay(int CurrentSelectionIndex);

  TCollection_AsciiString BuildValuesAscii(const TColQuantity_Array1OfLength& anArray);
  Standard_Boolean ExtractValues(TCollection_AsciiString aMessage,Handle(TColQuantity_HArray1OfLength)& anArray);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TypePropertyPage_H__B6E7AAD3_DB72_11D1_8DE5_0800369C8A03__INCLUDED_)
