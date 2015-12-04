#if !defined(AFX_ZCUEINGDLG_H__B8439A02_61FB_11D3_8D0A_00AA00D10994__INCLUDED_)
#define AFX_ZCUEINGDLG_H__B8439A02_61FB_11D3_8D0A_00AA00D10994__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ZCueingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ZCueingDlg dialog

class ZCueingDlg : public CDialog
{
// Construction
public:
	ZCueingDlg(Handle(V3d_View) Current_V3d_View, CViewer3dDoc* pDoc, CWnd* pParent = NULL); 
// Dialog Data
	//{{AFX_DATA(ZCueingDlg)
	enum { IDD = IDD_ZCUEING };
	CSliderCtrl	m_ZCueingWidthSlidCtrl;
	CSliderCtrl	m_ZCueingDepthSlidCtrl;
	BOOL	m_Cueing;
	double	m_ZCueingDepth;
	double	m_ZCueingWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ZCueingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ZCueingDlg)
	afx_msg void OnCheckCueingonoff();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditZcueingdepth();
	afx_msg void OnChangeEditZcueingwidth();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	Handle(V3d_View) myCurrent_V3d_View;
	CViewer3dDoc* myDoc;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZCUEINGDLG_H__B8439A02_61FB_11D3_8D0A_00AA00D10994__INCLUDED_)
