// CircularGrid.cpp : implementation file
//

#include "stdafx.h"

#include "CircularGrid.h"

/////////////////////////////////////////////////////////////////////////////
// CCircularGrid dialog


CCircularGrid::CCircularGrid(CWnd* pParent /*=NULL*/)
	: CDialog(CCircularGrid::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCircularGrid)
	m_RotationAngle = 0.0;
	m_XOrigin = 0.0;
	m_YOrigin = 0.0;
	m_RadiusStep = 0.0;
	m_DivisionNumber = 0;
	//}}AFX_DATA_INIT
}


void CCircularGrid::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCircularGrid)
	DDX_Text(pDX, IDC_CircGrid_RotationAngle, m_RotationAngle);
	DDX_Text(pDX, IDC_CircGrid_XOrigin, m_XOrigin);
	DDX_Text(pDX, IDC_CircGrid_Yorigin, m_YOrigin);
	DDX_Text(pDX, IDC_CirctGrid_RadiusStep, m_RadiusStep);
	DDX_Text(pDX, IDC_CircGrid_DivNumber, m_DivisionNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCircularGrid, CDialog)
	//{{AFX_MSG_MAP(CCircularGrid)
	ON_EN_UPDATE(IDC_CircGrid_DivNumber, OnUpdateCircGridDivNumber)
	ON_EN_UPDATE(IDC_CircGrid_RotationAngle, OnUpdateCircGridRotationAngle)
	ON_EN_UPDATE(IDC_CircGrid_XOrigin, OnUpdateCircGridXOrigin)
	ON_EN_UPDATE(IDC_CircGrid_Yorigin, OnUpdateCircGridYorigin)
	ON_EN_UPDATE(IDC_CirctGrid_RadiusStep, OnUpdateCirctGridRadiusStep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCircularGrid message handlers

void CCircularGrid::OnCancel() 
{
  myViewer->SetCircularGridValues(SavedXOrigin   , SavedYOrigin, 
	                              SavedRadiusStep, SavedDivisionNumber, 
	                              SavedRotationAngle );
  CDialog::OnCancel();
}

void CCircularGrid::UpdateDialogData() 
{
  UpdateData(TRUE);
  ASSERT(!myViewer.IsNull());
  Standard_Real     XOrigin,YOrigin,RadiusStep;
  Standard_Integer    DivisionNumber;
  Standard_Real RotationAngle;

  XOrigin          = m_XOrigin ;
  YOrigin          = m_YOrigin ;
  RadiusStep       = m_RadiusStep   ;
  DivisionNumber   = m_DivisionNumber   ;
  RotationAngle    = m_RotationAngle*M_PI/180 ;
  myViewer->SetCircularGridValues(XOrigin, YOrigin, RadiusStep, DivisionNumber, RotationAngle );

}


void CCircularGrid::OnUpdateCircGridDivNumber() 
{
  UpdateDialogData();
}

void CCircularGrid::OnUpdateCircGridRotationAngle() 
{
  UpdateDialogData();
}

void CCircularGrid::OnUpdateCircGridXOrigin() 
{
  UpdateDialogData();
}

void CCircularGrid::OnUpdateCircGridYorigin() 
{
  UpdateDialogData();
}

void CCircularGrid::OnUpdateCirctGridRadiusStep() 
{
  UpdateDialogData();
}

void CCircularGrid::UpdateValues()
{
  Standard_Real     XOrigin,YOrigin,RadiusStep;
  Standard_Integer    DivisionNumber;
  Standard_Real RotationAngle;
  myViewer->CircularGridValues(XOrigin, YOrigin, RadiusStep, DivisionNumber, RotationAngle );
  m_XOrigin = SavedXOrigin = XOrigin;
  m_YOrigin = SavedYOrigin = YOrigin;
  m_RadiusStep     = SavedRadiusStep     = RadiusStep;
  m_DivisionNumber = SavedDivisionNumber = DivisionNumber;
  m_RotationAngle  = SavedRotationAngle  = RotationAngle;
  UpdateData(FALSE);
}
