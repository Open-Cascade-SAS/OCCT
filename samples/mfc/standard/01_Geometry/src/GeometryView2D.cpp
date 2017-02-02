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

IMPLEMENT_DYNCREATE(CGeometryView2D, CView)
BEGIN_MESSAGE_MAP(CGeometryView2D, OCC_2dView)
  //{{AFX_MSG_MAP(CGeometryView2D)
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MBUTTONDOWN()
  ON_WM_MBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_SIZE()
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

// CNSGView drawing

void CGeometryView2D::OnDraw(CDC* /*pDC*/)
{
  CGeometryDoc* pDoc = GetDocument();
  ASSERT_VALID(pDoc);

  if (!myV2dView.IsNull())
    myV2dView->Update();
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

void CGeometryView2D::OnLButtonDown(UINT nFlags, CPoint point) 
{
  //  save the current mouse coordinate in min 
  myXmin=point.x;  myYmin=point.y;
  myXmax=point.x;  myYmax=point.y;

  if ( nFlags & MK_CONTROL ) 
  {
    // Button MB1 down Control :start zomming 
    // SetCursor(AfxGetApp()->LoadStandardCursor());
  }
  else // if ( Ctrl )
  {
    switch (myCurrentMode)
    {
    case CurAction2d_Nothing : // start a drag
      GetDocument()->DragEvent2D(point.x,point.y,-1,myV2dView);
      break;
    case CurAction2d_DynamicZooming : // noting
      break;
    case CurAction2d_WindowZooming : 
      break;
    case CurAction2d_DynamicPanning :// noting
      break;
    case CurAction2d_GlobalPanning :// noting
      break;
    default :
      throw Standard_Failure(" incompatible Current Mode ");
      break;
    }
  }
}

void CGeometryView2D::OnLButtonUp(UINT nFlags, CPoint point) 
{
  // TODO: Add your message handler code here and/or call default
  if ( nFlags & MK_CONTROL ) 
  {
    return;
  }
  else // if ( Ctrl )
  {
    const Handle(AIS_InteractiveContext)& aContext = GetDocument()->GetISessionContext();
    switch (myCurrentMode)
    {
    case CurAction2d_Nothing :
      if (point.x == myXmin && point.y == myYmin)
      { // no offset between down and up --> selectEvent
        myXmax=point.x;  
        myYmax=point.y;
        if (nFlags & MK_SHIFT )
          GetDocument()->ShiftInputEvent2D(point.x,point.y,myV2dView);
        else
          GetDocument()->InputEvent2D     (point.x,point.y,myV2dView);
      } else
      {
        drawRectangle (myXmin,myYmin,myXmax,myYmax,aContext,Standard_False);
        myXmax=point.x;  
        myYmax=point.y;
        if (nFlags & MK_SHIFT)
          GetDocument()->ShiftDragEvent2D(point.x,point.y,1,myV2dView);
        else
          GetDocument()->DragEvent2D(point.x,point.y,1,myV2dView);
      }
      break;
    case CurAction2d_DynamicZooming :
      // SetCursor(AfxGetApp()->LoadStandardCursor());         
      myCurrentMode = CurAction2d_Nothing;
      break;
    case CurAction2d_WindowZooming :
      myXmax=point.x;         myYmax=point.y;
      drawRectangle (myXmin,myYmin,myXmax,myYmax,aContext,Standard_False);
      if ((abs(myXmin-myXmax)>ValZWMin) || (abs(myYmin-myYmax)>ValZWMin))
        // Test if the zoom window is greater than a minimale window.
      {
        // Do the zoom window between Pmin and Pmax
        myV2dView->WindowFit(myXmin,myYmin,myXmax,myYmax);  
      }  
      myCurrentMode = CurAction2d_Nothing;
      break;
    case CurAction2d_DynamicPanning :
      myCurrentMode = CurAction2d_Nothing;
      break;
    case CurAction2d_GlobalPanning :
      myV2dView->Place(point.x,point.y,myCurZoom); 
      myCurrentMode = CurAction2d_Nothing;
      break;
    default :
      throw Standard_Failure(" incompatible Current Mode ");
      break;
    } //switch (myCurrentMode)
  } //	else // if ( Ctrl )

}

void CGeometryView2D::OnMButtonDown(UINT nFlags, CPoint /*point*/) 
{
  if ( nFlags & MK_CONTROL ) 
  {
    // Button MB2 down Control : panning init  
    // SetCursor(AfxGetApp()->LoadStandardCursor());   
  }
}

void CGeometryView2D::OnMButtonUp(UINT nFlags, CPoint /*point*/) 
{
  if ( nFlags & MK_CONTROL ) 
  {
    // Button MB2 up Control : panning stop 
    // SetCursor(AfxGetApp()->LoadStandardCursor());   
  }
}

void CGeometryView2D::OnRButtonDown(UINT nFlags, CPoint point) 
{
  // TODO: Add your message handler code here and/or call default
  if ( nFlags & MK_CONTROL ) 
  {
    // SetCursor(AfxGetApp()->LoadStandardCursor());   
  }
  else // if ( Ctrl )
  {
    GetDocument()->Popup2D(point.x,point.y,myV2dView);
  }	
}

void CGeometryView2D::OnRButtonUp(UINT /*nFlags*/, CPoint point) 
{
  OCC_2dView::Popup2D(point.x,point.y);
}

void CGeometryView2D::OnMouseMove(UINT nFlags, CPoint point) 
{
  //   ============================  LEFT BUTTON =======================
  if ( nFlags & MK_LBUTTON)
  {
    if ( nFlags & MK_CONTROL ) 
    {
      // move with MB1 and Control : on the dynamic zooming  
      // Do the zoom in function of mouse's coordinates  
      myV2dView->Zoom(myXmax,myYmax,point.x,point.y); 
      // save the current mouse coordinate in min 
      myXmax = point.x; 
      myYmax = point.y;	
    }
    else // if ( Ctrl )
    {
      const Handle(AIS_InteractiveContext)& aContext = GetDocument()->GetISessionContext();
      switch (myCurrentMode)
      {
      case CurAction2d_Nothing :
        myXmax = point.x; 
        myYmax = point.y;	
        GetDocument()->DragEvent2D(myXmax,myYmax,0,myV2dView);
        drawRectangle (myXmin,myYmin,myXmax,myYmax, aContext);
        break;
      case CurAction2d_DynamicZooming :
        myV2dView->Zoom(myXmax,myYmax,point.x,point.y); 
        // save the current mouse coordinate in min \n";
        myXmax=point.x;  myYmax=point.y;
        break;
      case CurAction2d_WindowZooming :
        myXmax = point.x; myYmax = point.y;	
        drawRectangle (myXmin,myYmin,myXmax,myYmax, aContext);
        break;
      case CurAction2d_DynamicPanning :
        myV2dView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
        myXmax = point.x; myYmax = point.y;	
        break;
      case CurAction2d_GlobalPanning : // nothing           
        break;
      default :
        throw Standard_Failure(" incompatible Current Mode ");
        break;
      }//  switch (myCurrentMode)
    }// if ( nFlags & MK_CONTROL )  else 
  } else //   if ( nFlags & MK_LBUTTON) 
    //   ============================  MIDDLE BUTTON =======================
    if ( nFlags & MK_MBUTTON)
    {
      if ( nFlags & MK_CONTROL ) 
      {
        myV2dView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
        myXmax = point.x; myYmax = point.y;	

      }
    } else //  if ( nFlags & MK_MBUTTON)
      //   ============================  RIGHT BUTTON =======================
      if ( nFlags & MK_RBUTTON)
      {
      }else //if ( nFlags & MK_RBUTTON)
        //   ============================  NO BUTTON =======================
      {  // No buttons 
        myXmax = point.x; myYmax = point.y;	
        if (nFlags & MK_SHIFT)
          GetDocument()->ShiftMoveEvent2D(point.x,point.y,myV2dView);
        else
          GetDocument()->MoveEvent2D(point.x,point.y,myV2dView);
      }
}

void CGeometryView2D::OnSize(UINT nType, int cx, int cy)
{
  OCC_2dView::OnSize (nType, cx, cy);
  if (!myV2dView.IsNull())
  {
    myV2dView->MustBeResized(); // added sro
  }
}

void CGeometryView2D::OnInitialUpdate()
{
  Handle(WNT_Window) aWNTWindow;
  aWNTWindow = new WNT_Window(GetSafeHwnd());	  

  myV2dView = GetDocument()->GetViewer2D()->CreateView();
  myV2dView->SetWindow(aWNTWindow);
  myV2dView->SetBackgroundColor(Quantity_NOC_BLACK);

  // initialyse the grids dialogs
  TheRectangularGridDialog.Create(CRectangularGrid::IDD, NULL);
  TheCircularGridDialog.Create(CCircularGrid::IDD, NULL);
  TheRectangularGridDialog.SetViewer (GetDocument()->GetViewer2D());
  TheCircularGridDialog.SetViewer (GetDocument()->GetViewer2D());

  Standard_Integer w=100 , h=100 ;   /* Debug Matrox                         */
  aWNTWindow->Size (w,h) ;           /* Keeps me unsatisfied (rlb).....      */
  /* Resize is not supposed to be done on */
  /* Matrox                               */
  /* I suspect another problem elsewhere  */
  ::PostMessage ( GetSafeHwnd () , WM_SIZE , SIZE_RESTORED , w + h*65536 ) ;
}
