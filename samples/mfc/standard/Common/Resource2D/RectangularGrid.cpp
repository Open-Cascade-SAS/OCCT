// RectangularGrid.cpp : implementation file
//

#include "stdafx.h"

#include "RectangularGrid.h"

/////////////////////////////////////////////////////////////////////////////
// CRectangularGrid dialog


CRectangularGrid::CRectangularGrid(CWnd* pParent /*=NULL*/)
	: CDialog(CRectangularGrid::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRectangularGrid)
	m_XOrigin = 0.0;
	m_YOrigin = 0.0;
	m_XStep   = 0.0;
	m_YStep   = 0.0;
	m_RotationAngle = 0.0;
	//}}AFX_DATA_INIT
}


void CRectangularGrid::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRectangularGrid)
	DDX_Text(pDX, IDC_RectGrid_XOrigin, m_XOrigin);
	DDX_Text(pDX, IDC_RectGrid_Yorigin, m_YOrigin);
	DDX_Text(pDX, IDC_RectGrid_XStep, m_XStep);
	DDX_Text(pDX, IDC_RectGrid_YStep, m_YStep);
	DDX_Text(pDX, IDC_RectGrid_Rotationangle, m_RotationAngle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRectangularGrid, CDialog)
	//{{AFX_MSG_MAP(CRectangularGrid)
	ON_EN_UPDATE(IDC_RectGrid_Rotationangle, OnUpdateRectGridRotationangle)
	ON_EN_UPDATE(IDC_RectGrid_XOrigin, OnUpdateRectGridXOrigin)
	ON_EN_UPDATE(IDC_RectGrid_XStep, OnUpdateRectGridXStep)
	ON_EN_UPDATE(IDC_RectGrid_Yorigin, OnUpdateRectGridYorigin)
	ON_EN_UPDATE(IDC_RectGrid_YStep, OnUpdateRectGridYStep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRectangularGrid message handlers

void CRectangularGrid::UpdateDialogData() 
{
  UpdateData(TRUE);
  ASSERT(!myViewer.IsNull());
  Standard_Real  XOrigin,YOrigin , XStep, YStep;
  Standard_Real RotationAngle;
  XOrigin = m_XOrigin ;
  YOrigin = m_YOrigin ;
  XStep   = m_XStep   ;
  YStep   = m_YStep   ;
  RotationAngle= m_RotationAngle*M_PI/180 ;
  myViewer->SetRectangularGridValues(XOrigin, YOrigin, XStep, YStep, RotationAngle );
}

void CRectangularGrid::OnUpdateRectGridRotationangle() 
{
  UpdateDialogData();
}

void CRectangularGrid::OnUpdateRectGridXOrigin() 
{
  UpdateDialogData();
}

void CRectangularGrid::OnUpdateRectGridXStep() 
{
  UpdateDialogData();
}

void CRectangularGrid::OnUpdateRectGridYorigin() 
{
  UpdateDialogData();
}

void CRectangularGrid::OnUpdateRectGridYStep() 
{
  UpdateDialogData();
}

void CRectangularGrid::OnCancel() 
{
  myViewer->SetRectangularGridValues(SavedXOrigin, SavedYOrigin, 
	                                 SavedXStep, SavedYStep, 
	                                 SavedRotationAngle );
  CDialog::OnCancel();
}

void CRectangularGrid::UpdateValues()
{
  Standard_Real XOrigin, YOrigin, XStep, YStep;
  Standard_Real RotationAngle;
  myViewer->RectangularGridValues(XOrigin, YOrigin, XStep, YStep, RotationAngle );
  m_XOrigin = SavedXOrigin = XOrigin;
  m_YOrigin = SavedYOrigin = YOrigin;
  m_XStep   = SavedXStep   = XStep;
  m_YStep   = SavedYStep   = YStep;
  m_RotationAngle = SavedRotationAngle= RotationAngle;
  UpdateData(FALSE);
}
