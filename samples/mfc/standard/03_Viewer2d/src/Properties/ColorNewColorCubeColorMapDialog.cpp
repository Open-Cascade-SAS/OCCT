// ColorNewColorCubeColorMapDialog.cpp : implementation file
//

#include "stdafx.h"

#include "ColorNewColorCubeColorMapDialog.h"

#include "Aspect_ColorCubeColorMap.hxx"

/////////////////////////////////////////////////////////////////////////////
// CColorNewColorCubeColorMapDialog dialog


CColorNewColorCubeColorMapDialog::CColorNewColorCubeColorMapDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CColorNewColorCubeColorMapDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorNewColorCubeColorMapDialog)
	m_base_pixel = 0;
	m_redmax     = 7;
	m_redmult    = 1;
	m_greenmax   = 7;
	m_greenmult  = 8;
	m_bluemax    = 3;
	m_bluemult   = 64;
	//}}AFX_DATA_INIT
}

void CColorNewColorCubeColorMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorNewColorCubeColorMapDialog)
	DDX_Text(pDX, IDC_NewColorCube_EDIT_base_pixel, m_base_pixel);
	DDX_Text(pDX, IDC_NewColorCube_EDIT_redmax, m_redmax);
	DDX_Text(pDX, IDC_NewColorCube_EDIT_redmult, m_redmult);
	DDX_Text(pDX, IDC_NewColorCube_EDIT_greenmax, m_greenmax);
	DDX_Text(pDX, IDC_NewColorCube_EDIT_grennmult, m_greenmult);
	DDX_Text(pDX, IDC_NewColorCube_EDIT_bluemax, m_bluemax);
	DDX_Text(pDX, IDC_NewColorCube_EDIT_bluemult, m_bluemult);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorNewColorCubeColorMapDialog, CDialog)
	//{{AFX_MSG_MAP(CColorNewColorCubeColorMapDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorNewColorCubeColorMapDialog message handlers

void CColorNewColorCubeColorMapDialog::OnOK() 
{
    UpdateData(true);
    myColorMap = 
        new Aspect_ColorCubeColorMap(m_base_pixel,
	                                 m_redmax    ,
	                                 m_redmult   ,
	                                 m_greenmax  ,
	                                 m_greenmult ,
	                                 m_bluemax   ,
	                                 m_bluemult  );
	CDialog::OnOK();
}
