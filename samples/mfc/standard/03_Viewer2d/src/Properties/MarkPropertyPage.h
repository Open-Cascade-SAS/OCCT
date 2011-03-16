#if !defined(AFX_MarkPropertyPage_H__B6E7AAD3_DB72_11D1_8DE5_0800369C8A03__INCLUDED_)
#define AFX_MarkPropertyPage_H__B6E7AAD3_DB72_11D1_8DE5_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MarkPropertyPage.h : header file
//

#include "V2d_Viewer.hxx"
#include "TColQuantity_Array1OfLength.hxx"
#include "TColQuantity_HArray1OfLength.hxx"
#include "TColStd_Array1OfBoolean.hxx"
/////////////////////////////////////////////////////////////////////////////
// CMarkPropertyPage dialog

class CMarkPropertyPage : public CPropertyPage
{
private :
    Handle(Aspect_MarkMap) myMarkMap;
private :
    Handle(V2d_Viewer) myViewer;
public:
    void SetViewer(Handle(V2d_Viewer) aViewer) {
        myViewer = aViewer;
        myMarkMap = aViewer->MarkMap();
    };

	DECLARE_DYNCREATE(CMarkPropertyPage)

// Construction
public:
	CMarkPropertyPage();
	~CMarkPropertyPage();

// Dialog Data
	//{{AFX_DATA(CMarkPropertyPage)
	enum { IDD = IDD_DIALOG_Mark };
	CTabCtrl	m_TabCtrl;
	CComboBox	m_NewEntryControl;

	CString	m_MarkMapSize;
	CString	m_CurrentEntryStyle;
	CString	m_CurrentEntryXValue;
	CString	m_CurrentEntryYValue;
	CString	m_CurrentEntrySValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMarkPropertyPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMarkPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMarkMapTAB(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
  void UpdateDisplay(int CurrentSelectionIndex);

  TCollection_AsciiString BuildValuesAscii(const TShort_Array1OfShortReal& anArray);
  TCollection_AsciiString BuildValuesAscii(const TColStd_Array1OfBoolean& anArray);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MarkPropertyPage_H__B6E7AAD3_DB72_11D1_8DE5_0800369C8A03__INCLUDED_)
