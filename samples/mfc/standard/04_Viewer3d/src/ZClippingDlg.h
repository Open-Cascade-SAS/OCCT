#if !defined(AFX_ZCLIPPINGDLG_H__B8439A01_61FB_11D3_8D0A_00AA00D10994__INCLUDED_)
#define AFX_ZCLIPPINGDLG_H__B8439A01_61FB_11D3_8D0A_00AA00D10994__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ZClippingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ZClippingDlg dialog

class ZClippingDlg : public CDialog
{
// Construction
public:
	ZClippingDlg(Handle(V3d_View) Current_V3d_View, CViewer3dDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ZClippingDlg)
	enum { IDD = IDD_ZCLIPPING };
	CSliderCtrl	m_ZClippingWidthSlidCtrl;
	CSliderCtrl	m_ZClippingDepthSlidCtrl;
	CComboBox	m_ZClippingTypeList;
	double	m_ZClippingDepth;
	double	m_ZClippingWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ZClippingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ZClippingDlg)
	afx_msg void OnChangeEditZclippingdepth();
	afx_msg void OnChangeEditZclippingwidth();
	afx_msg void OnSelchangeComboZclippingtype();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	Handle(V3d_View) myCurrent_V3d_View;
	CViewer3dDoc* myDoc;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZCLIPPINGDLG_H__B8439A01_61FB_11D3_8D0A_00AA00D10994__INCLUDED_)
