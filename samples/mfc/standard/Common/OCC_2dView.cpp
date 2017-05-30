// OCC_2dView.cpp: implementation of the OCC_2dView class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "OCC_2dView.h"

#include "OCC_App.h"
#include "OCC_2dDoc.h"
#include "resource2d\RectangularGrid.h"
#include "resource2d\CircularGrid.h"

#include "Quantity_Color.hxx"
#include "Quantity_NameOfColor.hxx"

#define ValZWMin 1

// the key for multi selection :
#define MULTISELECTIONKEY MK_SHIFT

// the key for shortcut ( use to activate dynamic rotation, panning )
#define CASCADESHORTCUTKEY MK_CONTROL

// define in witch case you want to display the popup
#define POPUPONBUTTONDOWN

/////////////////////////////////////////////////////////////////////////////
// OCC_2dView

IMPLEMENT_DYNCREATE(OCC_2dView, CView)

BEGIN_MESSAGE_MAP(OCC_2dView, CView)
	//{{AFX_MSG_MAP(OCC_2dView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	ON_COMMAND(ID_FILE_EXPORT_IMAGE, OnFileExportImage)
    ON_COMMAND(ID_BUTTON2DGridRectLines, OnBUTTONGridRectLines)
	ON_COMMAND(ID_BUTTON2DGridRectPoints, OnBUTTONGridRectPoints)
	ON_COMMAND(ID_BUTTON2DGridCircLines, OnBUTTONGridCircLines)
	ON_COMMAND(ID_BUTTON2DGridCircPoints, OnBUTTONGridCircPoints)
	ON_COMMAND(ID_BUTTON2DGridValues, OnBUTTONGridValues)
    ON_UPDATE_COMMAND_UI(ID_BUTTON2DGridValues, OnUpdateBUTTONGridValues)
	ON_COMMAND(ID_BUTTON2DGridCancel, OnBUTTONGridCancel)
	ON_UPDATE_COMMAND_UI(ID_BUTTON2DGridCancel, OnUpdateBUTTONGridCancel)
    ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_COMMAND(ID_BUTTON2DFitAll, OnBUTTONFitAll)
	ON_COMMAND(ID_BUTTON2DGlobPanning, OnBUTTONGlobPanning)
	ON_COMMAND(ID_BUTTON2DPanning, OnBUTTONPanning)
	ON_COMMAND(ID_BUTTON2DZoomProg, OnBUTTONZoomProg)
	ON_COMMAND(ID_BUTTON2DZoomWin, OnBUTTONZoomWin)
	ON_UPDATE_COMMAND_UI(ID_BUTTON2DGlobPanning, OnUpdateBUTTON2DGlobPanning)
	ON_UPDATE_COMMAND_UI(ID_BUTTON2DPanning, OnUpdateBUTTON2DPanning)
	ON_UPDATE_COMMAND_UI(ID_BUTTON2DZoomProg, OnUpdateBUTTON2DZoomProg)
	ON_UPDATE_COMMAND_UI(ID_BUTTON2DZoomWin, OnUpdateBUTTON2DZoomWin)
	ON_COMMAND(ID_Modify_ChangeBackground ,OnChangeBackground)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OCC_2dView construction/destruction

OCC_2dView::OCC_2dView()
: myCurrentMode (CurAction2d_Nothing)
{
}

OCC_2dView::~OCC_2dView()
{
  myV2dView->Remove();
}

BOOL OCC_2dView::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs
  cs.lpszClass = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
  return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// OCC_2dView drawing

void OCC_2dView::OnDraw(CDC* /*pDC*/)
{
  if (!myV2dView.IsNull())
    myV2dView->Update();
}

void OCC_2dView::OnInitialUpdate()
{
  CView::OnInitialUpdate();

  Handle(WNT_Window) aWNTWindow = new WNT_Window(GetSafeHwnd(),Quantity_NOC_MATRAGRAY);	  
  myV2dView =((OCC_2dDoc*)GetDocument())->GetViewer2D()->CreateView();
  myV2dView->SetWindow(aWNTWindow);
  // initialize the grids dialogs
  TheRectangularGridDialog.Create(CRectangularGrid::IDD, NULL);
  TheCircularGridDialog.Create(CCircularGrid::IDD, NULL);
  TheRectangularGridDialog.SetViewer (((OCC_2dDoc*)GetDocument())->GetViewer2D());
  TheCircularGridDialog.SetViewer (((OCC_2dDoc*)GetDocument())->GetViewer2D());

  Standard_Integer w=100 , h=100 ;   /* Debug Matrox                         */
  aWNTWindow->Size (w,h) ;           /* Keeps me unsatisfied (rlb).....      */
  /* Resize is not supposed to be done on */
  /* Matrox                               */
  /* I suspect another problem elsewhere  */
  ::PostMessage ( GetSafeHwnd () , WM_SIZE , SIZE_RESTORED , w + h*65536 ) ;

}

void OCC_2dView::OnFileExportImage()
{
  GetDocument()->ExportView (myV2dView);
}

/////////////////////////////////////////////////////////////////////////////
// OCC_2dView diagnostics

#ifdef _DEBUG
void OCC_2dView::AssertValid() const
{
  CView::AssertValid();
}

void OCC_2dView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}

OCC_2dDoc* OCC_2dView::GetDocument() // non-debug version is inline
{
  //ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(OCC_2dDoc)));
  return (OCC_2dDoc*)m_pDocument;
}
#endif //_DEBUG
void OCC_2dView::OnBUTTONGridRectLines() 
{
  Handle(V3d_Viewer) aViewer = myV2dView->Viewer();
  Handle(Graphic3d_AspectMarker3d) aGridAspect = new Graphic3d_AspectMarker3d(Aspect_TOM_RING1,Quantity_NOC_WHITE,2);
  aViewer->SetGridEcho(aGridAspect);
  Standard_Integer aWidth=0, aHeight=0, anOffset=0;
  myV2dView->Window()->Size(aWidth,aHeight);
  aViewer->SetRectangularGridGraphicValues(aWidth,aHeight,anOffset);
  aViewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);
  FitAll();

  if (TheCircularGridDialog.IsWindowVisible())
  {
    TheCircularGridDialog.ShowWindow(SW_HIDE);
    TheRectangularGridDialog.UpdateValues();
    TheRectangularGridDialog.ShowWindow(SW_SHOW);
  }
}

void OCC_2dView::OnBUTTONGridRectPoints() 
{
  Handle(V3d_Viewer) aViewer = myV2dView->Viewer();
  Handle(Graphic3d_AspectMarker3d) aGridAspect = new Graphic3d_AspectMarker3d(Aspect_TOM_RING1,Quantity_NOC_WHITE,2);
  aViewer->SetGridEcho(aGridAspect);
  Standard_Integer aWidth=0, aHeight=0, anOffset=0;
  myV2dView->Window()->Size(aWidth,aHeight);
  aViewer->SetRectangularGridGraphicValues(aWidth,aHeight,anOffset);
  aViewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Points);
  FitAll();

  if (TheCircularGridDialog.IsWindowVisible())
  {
    TheCircularGridDialog.ShowWindow(SW_HIDE);
    TheRectangularGridDialog.UpdateValues();
    TheRectangularGridDialog.ShowWindow(SW_SHOW);
  }
}

void OCC_2dView::OnBUTTONGridCircLines() 
{
  Handle(V3d_Viewer) aViewer = myV2dView->Viewer();
  Handle(Graphic3d_AspectMarker3d) aGridAspect = new Graphic3d_AspectMarker3d(Aspect_TOM_RING1,Quantity_NOC_WHITE,2);
  aViewer->SetGridEcho(aGridAspect);
  Standard_Integer aWidth=0, aHeight=0, anOffset=0;
  myV2dView->Window()->Size(aWidth,aHeight);
  aViewer->SetCircularGridGraphicValues(aWidth>aHeight?aWidth:aHeight,anOffset);
  aViewer->ActivateGrid(Aspect_GT_Circular, Aspect_GDM_Lines);
  FitAll();
 

  if (TheRectangularGridDialog.IsWindowVisible())
  {
    TheRectangularGridDialog.ShowWindow(SW_HIDE);
    TheCircularGridDialog.UpdateValues();
    TheCircularGridDialog.ShowWindow(SW_SHOW);
  }
}

void OCC_2dView::OnBUTTONGridCircPoints() 
{
  Handle(V3d_Viewer) aViewer = myV2dView->Viewer();
  Handle(Graphic3d_AspectMarker3d) aGridAspect = new Graphic3d_AspectMarker3d(Aspect_TOM_RING1,Quantity_NOC_WHITE,2);
  aViewer->SetGridEcho(aGridAspect);
  Standard_Integer aWidth=0, aHeight=0, anOffset=0;
  myV2dView->Window()->Size(aWidth,aHeight);
  aViewer->SetCircularGridGraphicValues(aWidth>aHeight?aWidth:aHeight,anOffset);
  aViewer->ActivateGrid(Aspect_GT_Circular, Aspect_GDM_Points);
  FitAll();
  if (TheRectangularGridDialog.IsWindowVisible())
  {
    TheRectangularGridDialog.ShowWindow(SW_HIDE);
    TheCircularGridDialog.UpdateValues();
    TheCircularGridDialog.ShowWindow(SW_SHOW);
  }
}

void OCC_2dView::OnBUTTONGridValues() 
{
  Handle(V3d_Viewer) aViewer = myV2dView->Viewer();
  Aspect_GridType  TheGridtype = aViewer->GridType();

  switch( TheGridtype ) 
  {
  case  Aspect_GT_Rectangular:
    TheRectangularGridDialog.UpdateValues();
    TheRectangularGridDialog.ShowWindow(SW_SHOW);
    break;
  case  Aspect_GT_Circular:
    TheCircularGridDialog.UpdateValues();
    TheCircularGridDialog.ShowWindow(SW_SHOW);
    break;
  default :
    throw Standard_Failure("invalid Aspect_GridType");
  }
}
void OCC_2dView::OnUpdateBUTTONGridValues(CCmdUI* pCmdUI) 
{
  Handle(V3d_Viewer) aViewer = myV2dView->Viewer();
  pCmdUI-> Enable( aViewer->IsActive() );
}

void OCC_2dView::OnBUTTONGridCancel() 
{
  Handle(V3d_Viewer) aViewer = myV2dView->Viewer();
  aViewer->DeactivateGrid();
  TheRectangularGridDialog.ShowWindow(SW_HIDE);
  TheCircularGridDialog.ShowWindow(SW_HIDE);
  aViewer->Update();
}
void OCC_2dView::OnUpdateBUTTONGridCancel(CCmdUI* pCmdUI) 
{
  Handle(V3d_Viewer) aViewer = myV2dView->Viewer();
  pCmdUI-> Enable( aViewer->IsActive() );	
}

void OCC_2dView::OnLButtonDown(UINT nFlags, CPoint point) 
{
  //  save the current mouse coordinate in min 
  myXmin=point.x;  myYmin=point.y;
  myXmax=point.x;  myYmax=point.y;

  if ( nFlags & CASCADESHORTCUTKEY ) 
  {
    // Button MB1 down Control :start zomming 
    // 
  }
  else // if ( MULTISELECTIONKEY )
  {
    switch (myCurrentMode)
    {
    case CurAction2d_Nothing : // start a drag
      DragEvent2D(point.x,point.y,-1);
      break;
    case CurAction2d_DynamicZooming : // nothing
      break;
    case CurAction2d_WindowZooming : // nothing
      break;
    case CurAction2d_DynamicPanning :// nothing
      break;
    case CurAction2d_GlobalPanning :// nothing
      break;
    default :
      throw Standard_Failure(" incompatible Current Mode ");
      break;
    }
  }
}


void OCC_2dView::OnLButtonUp(UINT nFlags, CPoint point) 
{
  // TODO: Add your message handler code here and/or call default
  if ( nFlags & CASCADESHORTCUTKEY ) 
  {
    return;
  }
  else // if ( Ctrl )
  {
    const Handle(AIS_InteractiveContext)& aContext = GetDocument()->GetAISContext();
    switch (myCurrentMode)
    {
    case CurAction2d_Nothing :
      if (point.x == myXmin && point.y == myYmin)
      { // no offset between down and up --> selectEvent
        myXmax=point.x;  
        myYmax=point.y;
        if (nFlags & MULTISELECTIONKEY )
          MultiInputEvent2D(point.x,point.y);
        else
          InputEvent2D     (point.x,point.y);
      } else
      {
        drawRectangle (myXmin, myYmin, myXmax, myYmax, aContext, Standard_False);
        myXmax=point.x;  
        myYmax=point.y;
        if (nFlags & MULTISELECTIONKEY)
          MultiDragEvent2D(point.x,point.y,1);
        else
          DragEvent2D(point.x,point.y,1);
      }
      break;
    case CurAction2d_DynamicZooming :
      // 
      myCurrentMode = CurAction2d_Nothing;
      break;
    case CurAction2d_WindowZooming :
      myXmax=point.x;     myYmax=point.y;
      drawRectangle (myXmin, myYmin, myXmax, myYmax, aContext, Standard_False);
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
  } //	else // if ( CASCADESHORTCUTKEY )	
}

void OCC_2dView::OnMButtonDown(UINT nFlags, CPoint /*point*/) 
{
  if ( nFlags & CASCADESHORTCUTKEY ) 
  {
    // Button MB2 down + CASCADESHORTCUTKEY : panning init  
    // 
  }
}

void OCC_2dView::OnMButtonUp(UINT nFlags, CPoint /*point*/) 
{
  if ( nFlags & CASCADESHORTCUTKEY ) 
  {
    // Button MB2 up + CASCADESHORTCUTKEY : panning stop 
  }
}

void OCC_2dView::OnRButtonDown(UINT nFlags, CPoint point) 
{
#ifdef POPUPONBUTTONDOWN
   if ( !(nFlags & CASCADESHORTCUTKEY) ) 
     Popup2D(point.x,point.y);
#endif
}

void OCC_2dView::OnRButtonUp(UINT 
#ifndef POPUPONBUTTONDOWN
                             nFlags
#endif
                             , CPoint 
#ifndef POPUPONBUTTONDOWN
                             point
#endif
                             )
{
#ifndef POPUPONBUTTONDOWN
   if ( !(nFlags & CASCADESHORTCUTKEY) )
    Popup2D(point.x,point.y);
#endif
}

void OCC_2dView::OnMouseMove(UINT nFlags, CPoint point) 
{
  //   ============================  LEFT BUTTON =======================
  if ( (nFlags & MK_LBUTTON) &! (nFlags & MK_RBUTTON) ) // Left + Right is specific
  {
    if ( nFlags & CASCADESHORTCUTKEY ) 
    {
      // move with MB1 and CASCADESHORTCUTKEY : on the dynamic zooming  
      // Do the zoom in function of mouse's coordinates  
      myV2dView->Zoom(myXmax,myYmax,point.x,point.y);
      // save the current mouse coordinate in min 
      myXmax = point.x; 
      myYmax = point.y;	
    }
    else // if ( CASCADESHORTCUTKEY )
    {
      const Handle(AIS_InteractiveContext)& aContext = GetDocument()->GetAISContext();
      switch (myCurrentMode)
      {
      case CurAction2d_Nothing :
        myXmax = point.x;     myYmax = point.y;	
        DragEvent2D(myXmax,myYmax,0);  
        drawRectangle (myXmin, myYmin, myXmax, myYmax, aContext);
        break;
      case CurAction2d_DynamicZooming :
        myV2dView->Zoom(myXmax,myYmax,point.x,point.y);
        // save the current mouse coordinate in min \n";
        myXmax=point.x;  myYmax=point.y;
        break;
      case CurAction2d_WindowZooming :
        myXmax = point.x;
        myYmax = point.y;
        drawRectangle (myXmin, myYmin, myXmax, myYmax, aContext);
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
    }// if ( nFlags & CASCADESHORTCUTKEY )  else 
  } else //   if ( nFlags & MK_LBUTTON) 
    //   ============================  MIDDLE BUTTON =======================
    if ( nFlags & MK_MBUTTON)
    {
      if ( nFlags & CASCADESHORTCUTKEY ) 
      {
        myV2dView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
        myXmax = point.x; myYmax = point.y;	

      }
    } else //  if ( nFlags & MK_MBUTTON)
      //   ============================  RIGHT BUTTON =======================
      if ( (nFlags & MK_RBUTTON) &! (nFlags & MK_LBUTTON) ) // Left + Right is specific
      {
      }else //if ( nFlags & MK_RBUTTON)
        if ( (nFlags & MK_RBUTTON) && (nFlags & MK_LBUTTON) )
        {
          // in case of Left + Right : same as Middle
          if ( nFlags & CASCADESHORTCUTKEY ) 
          {
            myV2dView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
            myXmax = point.x; myYmax = point.y;	
          }
        }else //if ( nFlags & MK_RBUTTON)&& (nFlags & MK_LBUTTON) 
          //   ============================  NO BUTTON =======================
        {  // No buttons 
          myXmax = point.x; myYmax = point.y;	
          if (nFlags & MULTISELECTIONKEY)
            MultiMoveEvent2D(point.x,point.y);
          else
            MoveEvent2D(point.x,point.y);
        }
}


void OCC_2dView::OnSize(UINT nType, int cx, int cy)
{
  OCC_BaseView::OnSize (nType, cx, cy);
  // Take care : This fonction is call before OnInitialUpdate
  if (!myV2dView.IsNull())
    myV2dView->MustBeResized(); 
}

void OCC_2dView::OnBUTTONFitAll() 
{
  myV2dView->FitAll();
}

void OCC_2dView::OnBUTTONGlobPanning() 
{
  //save the current zoom value
  myCurZoom = myV2dView->Scale();  

  // Do a Global Zoom 
  myV2dView->FitAll();

  // Set the mode
  myCurrentMode = CurAction2d_GlobalPanning;
}
void OCC_2dView::OnBUTTONPanning() 
{
  myCurrentMode = CurAction2d_DynamicPanning;
}
void OCC_2dView::OnBUTTONZoomProg() 
{
  myCurrentMode = CurAction2d_DynamicZooming;
}
void OCC_2dView::OnBUTTONZoomWin() 
{
  myCurrentMode = CurAction2d_WindowZooming;
}
void OCC_2dView::OnChangeBackground() 
{
  Standard_Real R1, G1, B1;
  Handle(Aspect_Window) aWindow = myV2dView->Window();
  Aspect_Background ABack = aWindow->Background();
  Quantity_Color aColor = ABack.Color();
  aColor.Values(R1,G1,B1,Quantity_TOC_RGB);
  COLORREF m_clr ;
  m_clr = RGB(R1*255,G1*255,B1*255);

  CColorDialog dlgColor(m_clr);
  if (dlgColor.DoModal() == IDOK)
  {
    m_clr = dlgColor.GetColor();
    R1 = GetRValue(m_clr)/255.;
    G1 = GetGValue(m_clr)/255.;
    B1 = GetBValue(m_clr)/255.;
    aColor.SetValues(R1,G1,B1,Quantity_TOC_RGB);
    myV2dView->SetBackgroundColor(aColor);
    myV2dView->Update();
  }	
}


void OCC_2dView::OnUpdateBUTTON2DGlobPanning(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myCurrentMode == CurAction2d_GlobalPanning);
  pCmdUI->Enable   (myCurrentMode != CurAction2d_GlobalPanning);
}

void OCC_2dView::OnUpdateBUTTON2DPanning(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myCurrentMode == CurAction2d_DynamicPanning);
  pCmdUI->Enable   (myCurrentMode != CurAction2d_DynamicPanning);
}

void OCC_2dView::OnUpdateBUTTON2DZoomProg(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myCurrentMode == CurAction2d_DynamicZooming);
  pCmdUI->Enable   (myCurrentMode != CurAction2d_DynamicZooming);
}

void OCC_2dView::OnUpdateBUTTON2DZoomWin(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myCurrentMode == CurAction2d_WindowZooming);
  pCmdUI->Enable   (myCurrentMode != CurAction2d_WindowZooming);		
}

// =====================================================================

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void OCC_2dView::DragEvent2D(const Standard_Integer  x,
                            const Standard_Integer  y,
                            const Standard_Integer  TheState)
{
  // TheState == -1  button down
  // TheState ==  0  move
  // TheState ==  1  button up

  static Standard_Integer theButtonDownX=0;
  static Standard_Integer theButtonDownY=0;

  if (TheState == -1)
  {
    theButtonDownX=x;
    theButtonDownY=y;
  }

  if (TheState == 0)
  {
    ((OCC_2dDoc*)GetDocument())->GetInteractiveContext()->MoveTo (x, y, myV2dView, Standard_False);
    ((OCC_2dDoc*)GetDocument())->GetInteractiveContext()->Select (theButtonDownX, theButtonDownY, x, y, myV2dView, Standard_True);
  }

  if (TheState == 1)
  {
    ((OCC_2dDoc*)GetDocument())->GetInteractiveContext()->Select(true);
  }
}


//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void OCC_2dView::InputEvent2D(const Standard_Integer /*x*/,
                              const Standard_Integer /*y*/)
{
  ((OCC_2dDoc*)GetDocument())->GetInteractiveContext()->Select(true);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void OCC_2dView::MoveEvent2D(const Standard_Integer  x,
                            const Standard_Integer  y) 
{
  if(myV2dView->Viewer()->Grid()->IsActive())
  {
    Standard_Real aGridX=0,aGridY=0,aGridZ=0;
    myV2dView->ConvertToGrid(x,y,aGridX,aGridY,aGridZ);
    //View is not updated automatically in ConvertToGrid
    myV2dView->Update();
  }
  ((OCC_2dDoc*)GetDocument())->GetInteractiveContext()->MoveTo (x, y, myV2dView, Standard_True);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void OCC_2dView::MultiMoveEvent2D(const Standard_Integer  x,
                                  const Standard_Integer  y) 
{
// MultiMoveEvent2D means we move the mouse in a multi selection mode
((OCC_2dDoc*)GetDocument())->GetInteractiveContext()->MoveTo (x, y, myV2dView, Standard_True);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void OCC_2dView::MultiDragEvent2D(const Standard_Integer  x        ,
                                  const Standard_Integer  y        ,
                                  const Standard_Integer  TheState) 
{
  static Standard_Integer theButtonDownX=0;
  static Standard_Integer theButtonDownY=0;

  if (TheState == -1)
  {
    theButtonDownX=x;
    theButtonDownY=y;
  }

  if (TheState == 0)
  {
    ((OCC_2dDoc*)GetDocument())->GetInteractiveContext()->MoveTo (x, y, myV2dView, Standard_False);
    ((OCC_2dDoc*)GetDocument())->GetInteractiveContext()->ShiftSelect (theButtonDownX, theButtonDownY, x, y, myV2dView, Standard_True);
  }

  if (TheState == 1)
  {
    ((OCC_2dDoc*)GetDocument())->GetInteractiveContext()->ShiftSelect(true);
  }
}


//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void OCC_2dView::MultiInputEvent2D(const Standard_Integer /*x*/,
                                   const Standard_Integer /*y*/)
{
  ((OCC_2dDoc*)GetDocument())->GetInteractiveContext()->ShiftSelect(true);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void  OCC_2dView::Popup2D(const Standard_Integer  x,
                         const Standard_Integer  y ) 
{
  CMenu menu;
  CMenu* pPopup ;

  // load the 'normal' popup 	 
  VERIFY(menu.LoadMenu(IDR_Popup2D));
  // activate the sub menu '0'
  pPopup= menu.GetSubMenu(0);
  ASSERT(pPopup != NULL);

  // display the popup
  POINT winCoord = { x , y };
  ClientToScreen ( &winCoord);
  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON , winCoord.x, winCoord.y , AfxGetMainWnd());
}

void OCC_2dView::FitAll()
{
  myV2dView->FitAll();
}
