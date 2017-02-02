// HLRView.cpp : implementation of the CHLRView2D class
//

#include "stdafx.h"
#include "HLRView2D.h"
#include "HLRApp.h"
#include "HLRDoc.h"
#include "resource2d\RectangularGrid.h"
#include "resource2d\CircularGrid.h"

#define ValZWMin 1

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// the key for multi selection :
#define MULTISELECTIONKEY MK_SHIFT

// the key for shortcut ( use to activate dynamic rotation, panning )
#define CASCADESHORTCUTKEY MK_CONTROL

// define in witch case you want to display the popup
#define POPUPONBUTTONDOWN

/////////////////////////////////////////////////////////////////////////////
// CHLRView2D

IMPLEMENT_DYNCREATE(CHLRView2D, OCC_2dView)

BEGIN_MESSAGE_MAP(CHLRView2D, OCC_2dView)
  //{{AFX_MSG_MAP(CHLRView2D)
  ON_WM_MOUSEMOVE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHLRView2D construction/destruction

CHLRView2D::CHLRView2D()
{
}

CHLRView2D::~CHLRView2D()
{
}

CHLRDoc* CHLRView2D::GetDocument() // non-debug version is inline
{
  //ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(OCC_2dDoc)));
  return (CHLRDoc*)m_pDocument;
}


void CHLRView2D::OnInitialUpdate()
{
  Handle(WNT_Window) aWNTWindow;
  aWNTWindow = new WNT_Window(GetSafeHwnd(),Quantity_NOC_GRAY);
  myV2dView = GetDocument()->GetViewer2D()->CreateView();
  myV2dView->SetWindow(aWNTWindow);

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

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::DragEvent2D(const Standard_Integer /*x*/,
                             const Standard_Integer /*y*/,
                             const Standard_Integer /*TheState*/)
{
}


//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::InputEvent2D(const Standard_Integer /*x*/,
                              const Standard_Integer /*y*/)
{
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::MoveEvent2D(const Standard_Integer /*x*/,
                             const Standard_Integer /*y*/)
{
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::MultiMoveEvent2D(const Standard_Integer /*x*/,
                                  const Standard_Integer /*y*/)
{
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::MultiDragEvent2D(const Standard_Integer /*x*/,
                                  const Standard_Integer /*y*/,
                                  const Standard_Integer /*TheState*/)
{
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::MultiInputEvent2D(const Standard_Integer /*x*/,
                                   const Standard_Integer /*y*/)
{
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView2D::OnMouseMove(UINT nFlags, CPoint point)
{
  if ( (nFlags & MK_LBUTTON) &! (nFlags & MK_RBUTTON) ) // Left + Right is specific
  {
    if ( nFlags & CASCADESHORTCUTKEY )
    {
      // move with MB1 and CASCADESHORTCUTKEY : on the dynamic zooming
      // Do the zoom in function of mouse's coordinates
      myV2dView->Zoom(myXmax,myYmax,point.x,point.y);
      myXmax = point.x;
      myYmax = point.y;
    }
    else // if ( CASCADESHORTCUTKEY )
    {
      switch (myCurrentMode)
      {
      case CurAction2d_Nothing :
        myXmax = point.x;
        myYmax = point.y;
        DragEvent2D(myXmax,myYmax,0);
        break;
      case CurAction2d_DynamicZooming :
        myV2dView->Zoom(myXmax,myYmax,point.x,point.y);
        myXmax=point.x;
        myYmax=point.y;
        break;
      case CurAction2d_WindowZooming :
        myXmax = point.x;
        myYmax = point.y;
        break;
      case CurAction2d_DynamicPanning :
        myV2dView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
        myXmax = point.x;
        myYmax = point.y;
        break;
      case CurAction2d_GlobalPanning :
        break;
      default :
        throw Standard_Failure(" incompatible Current Mode ");
        break;
      }
    }
  } else if ( nFlags & MK_MBUTTON)
  {
    if ( nFlags & CASCADESHORTCUTKEY )
    {
      myV2dView->Pan (point.x-myXmax,myYmax-point.y);
      myXmax = point.x;
      myYmax = point.y;
    }
  } else if ( (nFlags & MK_RBUTTON) &! (nFlags & MK_LBUTTON) )
  {
  }
  else if ( (nFlags & MK_RBUTTON) && (nFlags & MK_LBUTTON) )
  {
    if ( nFlags & CASCADESHORTCUTKEY ) 
    {
      myV2dView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
      myXmax = point.x; myYmax = point.y;	
    }
  }
  else
  {  // No buttons
    myXmax = point.x;
    myYmax = point.y;
    if (nFlags & MULTISELECTIONKEY)
    {
      MultiMoveEvent2D(point.x,point.y);
    }
    else
    {
      MoveEvent2D(point.x,point.y);
    }
  }
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