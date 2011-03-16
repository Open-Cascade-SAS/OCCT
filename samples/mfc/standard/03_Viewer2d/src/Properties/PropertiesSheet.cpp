// MapProperties.cpp : implementation file
//

#include "stdafx.h"

#include "PropertiesSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertiesSheet

IMPLEMENT_DYNAMIC(CPropertiesSheet, CPropertySheet)

CPropertiesSheet::CPropertiesSheet(CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet("View Map Properties", pParentWnd, iSelectPage)
{
}

void CPropertiesSheet::SetViewer (Handle(V2d_Viewer) aViewer)
{
  AddPage(& m_pageWidthProperty);
  m_pageWidthProperty.SetViewer(aViewer);

  AddPage(& m_pageColorProperty);
  m_pageColorProperty.SetViewer(aViewer);

  AddPage(& m_pageFontProperty);
  m_pageFontProperty.SetViewer(aViewer);

  AddPage(& m_pageTypeProperty);
  m_pageTypeProperty.SetViewer(aViewer);

  AddPage(& m_pageMarkProperty);
  m_pageMarkProperty.SetViewer(aViewer);

}

CPropertiesSheet::~CPropertiesSheet()
{
}


BEGIN_MESSAGE_MAP(CPropertiesSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CPropertiesSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesSheet message handlers

BOOL CPropertiesSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	return bResult;
}

