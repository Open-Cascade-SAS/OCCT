// GeometryView.cpp : implementation of the CGeometryView class
//

#include "stdafx.h"
#include "GeometryApp.h"

#include <GeometryApp.h>
#include "GeometryDoc.h"
#include "GeometryView.h"

#define ValZWMin 1

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeometryView

IMPLEMENT_DYNCREATE(CGeometryView, OCC_3dView)

BEGIN_MESSAGE_MAP(CGeometryView, OCC_3dView)
  //{{AFX_MSG_MAP(CGeometryView)
  ON_COMMAND(ID_FILE_EXPORT_IMAGE, OnFileExportImage)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeometryView construction/destruction

CGeometryView::CGeometryView()
{
}

CGeometryView::~CGeometryView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGeometryView diagnostics

#ifdef _DEBUG
void CGeometryView::AssertValid() const
{
  CView::AssertValid();
}

void CGeometryView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}

CGeometryDoc* CGeometryView::GetDocument() // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGeometryDoc)));
  return (CGeometryDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGeometryView message handlers

void CGeometryView::OnFileExportImage()
{
  GetDocument()->ExportView (myView);
}
