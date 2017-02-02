// HLRView.cpp : implementation of the CHLRView3D3D class
//

#include "stdafx.h"
#include "HLRApp.h"

#include "HLRDoc.h"
#include "HLRView3D.h"
#include "..\..\Common\res\OCC_Resource.h"

#ifdef _DEBUG
//#define new DEBUG_NEW by CasCade
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// for elastic bean selection
#define ValZWMin 1

// the key for multi selection :
#define MULTISELECTIONKEY MK_SHIFT

// the key for shortcut ( use to activate dynamic rotation, panning )
#define CASCADESHORTCUTKEY MK_CONTROL

// define in witch case you want to display the popup
#define POPUPONBUTTONDOWN

/////////////////////////////////////////////////////////////////////////////
// CHLRView3D

IMPLEMENT_DYNCREATE(CHLRView3D, OCC_3dView)

BEGIN_MESSAGE_MAP(CHLRView3D, OCC_3dView)
	//{{AFX_MSG_MAP(CHLRView3D)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
// CasCade
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHLRView3D construction/destruction

CHLRView3D::CHLRView3D()
{
}

CHLRView3D::~CHLRView3D()
{
}

void CHLRView3D::OnInitialUpdate() 
{

	// TODO: Add your specialized code here and/or call the base class
	    // TODO: Add your specialized code here and/or call the base class
    myView = GetDocument()->GetViewer()->CreateView();

    // store for restore state after rotation (witch is in Degenerated mode)
    myDegenerateModeIsOn = myView->DegenerateModeIsOn();

	Handle(Graphic3d_WNTGraphicDevice) theGraphicDevice = 
		((CHLRApp*)AfxGetApp())->GetGraphicDevice();
    
    Handle(WNT_Window) aWNTWindow = new WNT_Window(theGraphicDevice,GetSafeHwnd ());
    myView->SetWindow(aWNTWindow);
    if (!aWNTWindow->IsMapped()) aWNTWindow->Map();

	Standard_Integer w=100 , h=100 ;   /* Debug Matrox                         */
	aWNTWindow->Size (w,h) ;           /* Keeps me unsatisfied (rlb).....      */
	                                   /* Resize is not supposed to be done on */
	                                   /* Matrox                               */
	                                   /* I suspect another problem elsewhere  */
	::PostMessage ( GetSafeHwnd () , WM_SIZE , SIZE_RESTORED , w + h*65536 ) ;

    // store the mode ( nothing , dynamic zooming, dynamic ... )
    myCurrentMode = CurAction3d_Nothing;

}

/////////////////////////////////////////////////////////////////////////////
// CHLRView3D diagnostics

#ifdef _DEBUG
void CHLRView3D::AssertValid() const
{
	CView::AssertValid();
}

void CHLRView3D::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CHLRDoc* CHLRView3D::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHLRDoc)));
	return (CHLRDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHLRView3D message handlers


void CHLRView3D::OnLButtonDown(UINT nFlags, CPoint point) 
{
  //  save the current mouse coordinate in min 
  myXmin=point.x;  myYmin=point.y;
  myXmax=point.x;  myYmax=point.y;

  if ( nFlags & CASCADESHORTCUTKEY ) 
	  {
	    // Button MB1 down Control :start zomming 
        // SetCursor(AfxGetApp()->LoadStandardCursor());
	  }
	else // if ( Ctrl )
	  {
        switch (myCurrentMode)
        {
         case CurAction3d_Nothing : // start a drag
           if (nFlags & MULTISELECTIONKEY)
			   MultiDragEvent(myXmax,myYmax,-1);
           else
               DragEvent(myXmax,myYmax,-1);
        break;
         break;
         case CurAction3d_DynamicZooming : // noting
         break;
         case CurAction3d_WindowZooming : // noting
         break;
         case CurAction3d_DynamicPanning :// noting
         break;
         case CurAction3d_GlobalPanning :// noting
        break;
        case  CurAction3d_DynamicRotation :
			if (!myDegenerateModeIsOn)
			  myView->SetDegenerateModeOn();
			myView->StartRotation(point.x,point.y);  
        break;
        default :
           throw Standard_Failure(" incompatible Current Mode ");
        break;
        }
    }
}

void CHLRView3D::OnLButtonUp(UINT nFlags, CPoint point) 
{
   if ( nFlags & CASCADESHORTCUTKEY ) 
	  {
        return;
	  }
	else // if ( Ctrl )
	  {
        switch (myCurrentMode)
        {
         case CurAction3d_Nothing :
         if (point.x == myXmin && point.y == myYmin)
         { // no offset between down and up --> selectEvent
            myXmax=point.x;  
            myYmax=point.y;
            if (nFlags & MULTISELECTIONKEY )
              MultiInputEvent(point.x,point.y);
            else
              InputEvent     (point.x,point.y);
         } else
         {
            myXmax=point.x;            myYmax=point.y;
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False);
		    if (nFlags & MULTISELECTIONKEY)
				MultiDragEvent(point.x,point.y,1);
			else
				DragEvent(point.x,point.y,1);
         }
         break;
         case CurAction3d_DynamicZooming :
             // SetCursor(AfxGetApp()->LoadStandardCursor());         
	       myCurrentMode = CurAction3d_Nothing;
         break;
         case CurAction3d_WindowZooming :
           myXmax=point.x;          myYmax=point.y;
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False,LongDash);
	       if ((abs(myXmin-myXmax)>ValZWMin) || (abs(myYmin-myYmax)>ValZWMin))
					 // Test if the zoom window is greater than a minimale window.
			{
			  // Do the zoom window between Pmin and Pmax
			  myView->WindowFitAll(myXmin,myYmin,myXmax,myYmax);  
			}  
	       myCurrentMode = CurAction3d_Nothing;
         break;
         case CurAction3d_DynamicPanning :
           myCurrentMode = CurAction3d_Nothing;
         break;
         case CurAction3d_GlobalPanning :
	       myView->Place(point.x,point.y,myCurZoom); 
	       myCurrentMode = CurAction3d_Nothing;
        break;
        case  CurAction3d_DynamicRotation :
	       myCurrentMode = CurAction3d_Nothing;
        break;
        default :
           throw Standard_Failure(" incompatible Current Mode ");
        break;
        } //switch (myCurrentMode)
    } //	else // if ( Ctrl )
}

void CHLRView3D::OnRButtonDown(UINT nFlags, CPoint point) 
{
   if ( nFlags & CASCADESHORTCUTKEY ) 
	  {
	    if (!myDegenerateModeIsOn)
	      myView->SetDegenerateModeOn();
	    myView->StartRotation(point.x,point.y);  
	  }
	else // if ( CASCADESHORTCUTKEY )
	  {
#ifdef POPUPONBUTTONDOWN
	    GetDocument()->Popup(point.x,point.y, myView);
#endif	
      }	
}

void CHLRView3D::OnMouseMove(UINT nFlags, CPoint point) 
{
    //   ============================  LEFT BUTTON =======================
  if ( nFlags & MK_LBUTTON)
    {
     if ( nFlags & CASCADESHORTCUTKEY ) 
	  {
	    // move with MB1 and Control : on the dynamic zooming  
	    // Do the zoom in function of mouse's coordinates  
	    myView->Zoom(myXmax,myYmax,point.x,point.y); 
	    // save the current mouse coordinate in min 
		myXmax = point.x; 
        myYmax = point.y;	
	  }
	  else // if ( Ctrl )
	  {
        switch (myCurrentMode)
        {
         case CurAction3d_Nothing :
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False);
		   myXmax = point.x; 
           myYmax = point.y;
           if (nFlags & MULTISELECTIONKEY)		
       	     MultiDragEvent(myXmax,myYmax,0);
           else
             DragEvent(myXmax,myYmax,0);
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_True);
          break;
         case CurAction3d_DynamicZooming :
	       myView->Zoom(myXmax,myYmax,point.x,point.y); 
	       // save the current mouse coordinate in min \n";
	       myXmax=point.x;  myYmax=point.y;
         break;
         case CurAction3d_WindowZooming :
		   myXmax = point.x; myYmax = point.y;	
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False,LongDash);
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_True,LongDash);
         break;
         case CurAction3d_DynamicPanning :
		   myView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
		   myXmax = point.x; myYmax = point.y;	
         break;
         case CurAction3d_GlobalPanning : // nothing           
        break;
        case  CurAction3d_DynamicRotation :
          myView->Rotation(point.x,point.y);
	      myView->Redraw();
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
		myView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
		myXmax = point.x; myYmax = point.y;	

	  }
    } else //  if ( nFlags & MK_MBUTTON)
    //   ============================  RIGHT BUTTON =======================
    if ( nFlags & MK_RBUTTON)
    {
     if ( nFlags & CASCADESHORTCUTKEY ) 
	  {
      	 myView->Rotation(point.x,point.y);
	  }
    }else //if ( nFlags & MK_RBUTTON)
    //   ============================  NO BUTTON =======================
    {  // No buttons 
	  myXmax = point.x; myYmax = point.y;	
	  if (nFlags & MULTISELECTIONKEY)
		MultiMoveEvent(point.x,point.y);
	  else
		MoveEvent(point.x,point.y);
   }
}

void CHLRView3D::DragEvent(const Standard_Integer  x        ,
				                  const Standard_Integer  y        ,
				                  const Standard_Integer  TheState )
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
	  GetDocument()->GetAISContext()->Select(theButtonDownX,theButtonDownY,x,y,myView);  
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView3D::InputEvent(const Standard_Integer  x     ,
				                   const Standard_Integer  y     ) 
{
    GetDocument()->GetAISContext()->Select(); 
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView3D::MoveEvent(const Standard_Integer  x       ,
                                  const Standard_Integer  y       ) 
{
      GetDocument()->GetAISContext()->MoveTo(x,y,myView);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView3D::MultiMoveEvent(const Standard_Integer  x       ,
                                  const Standard_Integer  y       ) 
{
      GetDocument()->GetAISContext()->MoveTo(x,y,myView);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView3D::MultiDragEvent(const Standard_Integer  x        ,
									   const Standard_Integer  y        ,
									   const Standard_Integer  TheState ) 
{
    static Standard_Integer theButtonDownX=0;
    static Standard_Integer theButtonDownY=0;

	if (TheState == -1)
    {
      theButtonDownX=x;
      theButtonDownY=y;
    }

	if (TheState == 0)
	  GetDocument()->GetAISContext()->ShiftSelect(theButtonDownX,theButtonDownY,x,y,myView);  
}


//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CHLRView3D::MultiInputEvent(const Standard_Integer  x       ,
									    const Standard_Integer  y       ) 
{
	GetDocument()->GetAISContext()->ShiftSelect(); 
}

