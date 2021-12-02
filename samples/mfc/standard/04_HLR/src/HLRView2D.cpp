// HLRView.cpp : implementation of the CHLRView2D class
//

#include "stdafx.h"
#include "HLRView2D.h"
#include "HLRApp.h"
#include "HLRDoc.h"
#include "resource2d\RectangularGrid.h"
#include "resource2d\CircularGrid.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHLRView2D

IMPLEMENT_DYNCREATE(CHLRView2D, OCC_2dView)

BEGIN_MESSAGE_MAP(CHLRView2D, OCC_2dView)
  //{{AFX_MSG_MAP(CHLRView2D)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHLRView2D construction/destruction

CHLRView2D::CHLRView2D()
{
  /// TODO
  /// Override MouseMove event to exclude rectangle selection emulation as
  /// no selection is supported in DragEvent2D for this view.
}

CHLRView2D::~CHLRView2D()
{
}

const Handle(AIS_InteractiveContext)& CHLRView2D::GetAISContext() const
{
  return ((CHLRDoc*)m_pDocument)->GetInteractiveContext2D();
}

CHLRDoc* CHLRView2D::GetDocument() // non-debug version is inline
{
  //ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(OCC_2dDoc)));
  return (CHLRDoc*)m_pDocument;
}

#ifdef _DEBUG
void CHLRView2D::AssertValid() const
{
  OCC_2dView::AssertValid();
}

void CHLRView2D::Dump(CDumpContext& dc) const
{
  OCC_2dView::Dump(dc);
}

#endif