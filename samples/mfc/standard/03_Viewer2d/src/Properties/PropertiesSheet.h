#if !defined(AFX_PropertiesSheet_H__A815F7A6_D51B_11D1_8DDE_0800369C8A03__INCLUDED_)
#define AFX_PropertiesSheet_H__A815F7A6_D51B_11D1_8DDE_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PropertiesSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertiesSheet
#include "ColorPropertyPage.h"
#include "FontPropertyPage.h"
#include "WidthPropertyPage.h"
#include "TypePropertyPage.h"
#include "MarkPropertyPage.h"


class CPropertiesSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropertiesSheet)



private :
    CColorPropertyPage m_pageColorProperty;
    CFontPropertyPage  m_pageFontProperty;
    CWidthPropertyPage m_pageWidthProperty;
    CTypePropertyPage m_pageTypeProperty;
    CMarkPropertyPage m_pageMarkProperty;

// Construction
public:
    CPropertiesSheet(CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

    void SetViewer (Handle(V2d_Viewer) aViewer);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertiesSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertiesSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PropertiesSheet_H__A815F7A6_D51B_11D1_8DDE_0800369C8A03__INCLUDED_)
