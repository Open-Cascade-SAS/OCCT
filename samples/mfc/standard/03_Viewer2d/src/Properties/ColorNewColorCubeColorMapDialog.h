// ColorNewColorCubeColorMapDialog.h : header file
//

#if !defined(AFX_COLORNEWCOLORCUBECOLORMAPDIALOG_H__67E9AE93_D9F9_11D1_8DE3_0800369C8A03__INCLUDED_)
#define AFX_COLORNEWCOLORCUBECOLORMAPDIALOG_H__67E9AE93_D9F9_11D1_8DE3_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Aspect_ColorMap.hxx"
/////////////////////////////////////////////////////////////////////////////
// CColorNewColorCubeColorMapDialog dialog

class CColorNewColorCubeColorMapDialog : public CDialog
{
private :
    Handle(Aspect_ColorMap) myColorMap;

// Construction
public:
	CColorNewColorCubeColorMapDialog(CWnd* pParent = NULL);   // standard constructor
    Handle(Aspect_ColorMap) ColorMap() {return myColorMap;};
// Dialog Data
	//{{AFX_DATA(CColorNewColorCubeColorMapDialog)
	enum { IDD = IDD_DIALOG_NewColorCube };
	int		m_base_pixel;
	int		m_redmax;
	int		m_redmult;
	int		m_greenmax;
	int		m_greenmult;
	int		m_bluemax;
	int		m_bluemult;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorNewColorCubeColorMapDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorNewColorCubeColorMapDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORNEWCOLORCUBECOLORMAPDIALOG_H__67E9AE93_D9F9_11D1_8DE3_0800369C8A03__INCLUDED_)
