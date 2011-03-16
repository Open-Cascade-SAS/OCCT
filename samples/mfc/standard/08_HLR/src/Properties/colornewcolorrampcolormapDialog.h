// ColorNewColorRampColorMapDialog.h : header file
//

#if !defined(AFX_ColorNewColorRampColorMapDialog_H__A45F6973_DB41_11D1_8DE5_0800369C8A03__INCLUDED_)
#define AFX_ColorNewColorRampColorMapDialog_H__A45F6973_DB41_11D1_8DE5_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <resource.h>

/////////////////////////////////////////////////////////////////////////////
// CColorNewColorRampColorMapDialog dialog

class CColorNewColorRampColorMapDialog : public CDialog
{
private :
    Handle(Aspect_ColorMap) myColorMap;

// Construction
public:
	CColorNewColorRampColorMapDialog(CWnd* pParent = NULL);   // standard constructor
    Handle(Aspect_ColorMap) ColorMap() {return myColorMap;};

// Dialog Data
	//{{AFX_DATA(CColorNewColorRampColorMapDialog)
	enum { IDD = IDD_DIALOG_NewColorRamp };
	CComboBox	m_ColorList;
	int		m_base_pixel;
	int		m_dimension;
	double	m_Red;
	double	m_Green;
	double	m_Blue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorNewColorRampColorMapDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorNewColorRampColorMapDialog)
	virtual void OnOK();
	afx_msg void OnColorMapRampBUTTONEditColor();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeColorMapRampEDITColorRed();
	afx_msg void OnChangeColorMapRampEDITColorGreen();
	afx_msg void OnChangeColorMapRampEDITColorBlue();
	afx_msg void OnSelchangeColorMapCOMBONewEntryColorName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ColorNewColorRampColorMapDialog_H__A45F6973_DB41_11D1_8DE5_0800369C8A03__INCLUDED_)
