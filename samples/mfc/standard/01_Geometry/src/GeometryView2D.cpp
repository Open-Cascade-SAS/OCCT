// NSGViewBibliotheque.cpp : implementation of the CGeometryView2D class
//

#include "stdafx.h"
#include "GeometryApp.h"
#include "GeometryDoc.h"
#include "GeometryView2D.h"
#include ".\Resource2d\RectangularGrid.h"
#include ".\Resource2d\CircularGrid.h"

/////////////////////////////////////////////////////////////////////////////
// CNSGView

IMPLEMENT_DYNCREATE(CGeometryView2D, OCC_2dView)
BEGIN_MESSAGE_MAP(CGeometryView2D, OCC_2dView)
  //{{AFX_MSG_MAP(CGeometryView2D)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNSGView construction/destruction

CGeometryView2D::CGeometryView2D()
{
}

CGeometryView2D::~CGeometryView2D()
{
}

const Handle(AIS_InteractiveContext)& CGeometryView2D::GetAISContext() const
{
  return ((CGeometryDoc*)m_pDocument)->GetISessionContext();
}

/////////////////////////////////////////////////////////////////////////////
// CGeometryView2D diagnostics

#ifdef _DEBUG
void CGeometryView2D::AssertValid() const
{
  CView::AssertValid();
}

void CGeometryView2D::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}

CGeometryDoc* CGeometryView2D::GetDocument() // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGeometryDoc)));
  return (CGeometryDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNSGView message handlers

//=================================================================
void CGeometryView2D::OnInitialUpdate()
{
  OCC_2dView::OnInitialUpdate();
  myView->SetBackgroundColor (Quantity_NOC_BLACK);
}
