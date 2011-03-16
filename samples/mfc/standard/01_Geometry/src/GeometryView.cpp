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
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_UPDATE_COMMAND_UI(ID_BUTTONHlrOff, OnUpdateBUTTONHlrOff)
	ON_UPDATE_COMMAND_UI(ID_BUTTONHlrOn, OnUpdateBUTTONHlrOn)
	ON_UPDATE_COMMAND_UI(ID_BUTTONPanGlo, OnUpdateBUTTONPanGlo)
	ON_UPDATE_COMMAND_UI(ID_BUTTONPan, OnUpdateBUTTONPan)
	ON_UPDATE_COMMAND_UI(ID_BUTTONZoomProg, OnUpdateBUTTONZoomProg)
	ON_UPDATE_COMMAND_UI(ID_BUTTONZoomWin, OnUpdateBUTTONZoomWin)
	ON_UPDATE_COMMAND_UI(ID_BUTTONRot, OnUpdateBUTTONRot)
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
  CFileDialog dlg(FALSE,_T("*.BMP"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  _T("BMP Files (*.BMP)|*.bmp |GIF Files (*.GIF)|*.gif | XWD Files (*.XWD)|*.xwd||"), 
                  NULL );

  if (dlg.DoModal() == IDOK) 
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    CString filename = dlg.GetPathName();
    Handle(Aspect_Window) anAspectWindow = myView->Window();
    Handle(WNT_Window) aWNTWindow = Handle(WNT_Window)::DownCast(anAspectWindow);
    CString ext = dlg.GetFileExt();
    if (ext == "bmp")     aWNTWindow->SetOutputFormat ( WNT_TOI_BMP );
    if (ext == "gif")     aWNTWindow->SetOutputFormat ( WNT_TOI_GIF );
    if (ext == "xwd")     aWNTWindow->SetOutputFormat ( WNT_TOI_XWD );
    aWNTWindow->Dump ((Standard_CString)(LPCTSTR)filename);
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  }
}

void CGeometryView::OnSize(UINT nType, int cx, int cy) 
{
  if (!myView.IsNull())
   myView->MustBeResized();
}

void CGeometryView::OnLButtonDown(UINT nFlags, CPoint point) 
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
         case CurAction3d_Nothing : // start a drag
           if (nFlags & MK_SHIFT)
        	    ShiftDragEvent(myXmax,myYmax,-1);
           else
                DragEvent(myXmax,myYmax,-1);
        break;
         break;
         case CurAction3d_DynamicZooming : // noting
             // SetCursor(AfxGetApp()->LoadStandardCursor());
         break;
         case CurAction3d_WindowZooming : 
         break;
         case CurAction3d_DynamicPanning :// noting
             // SetCursor(AfxGetApp()->LoadStandardCursor());
         break;
         case CurAction3d_GlobalPanning :// noting
             // SetCursor(AfxGetApp()->LoadStandardCursor());
        break;
        case  CurAction3d_DynamicRotation :
             // SetCursor(AfxGetApp()->LoadStandardCursor());
			if (!myDegenerateModeIsOn)
			  myView->SetDegenerateModeOn();
			myView->StartRotation(point.x,point.y);  
        break;
        default :
           Standard_Failure::Raise(" incompatible Current Mode ");
        break;
        }
    }
}

void CGeometryView::OnLButtonUp(UINT nFlags, CPoint point) 
{
   if ( nFlags & MK_CONTROL ) 
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
            if (nFlags & MK_SHIFT )
              ShiftInputEvent(point.x,point.y);
            else
              InputEvent     (point.x,point.y);
         } else
         {
            myXmax=point.x;    myYmax=point.y;
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False);
		    if (nFlags & MK_SHIFT)
				ShiftDragEvent(point.x,point.y,1);
			else
				DragEvent(point.x,point.y,1);
         }
         break;
         case CurAction3d_DynamicZooming :
             // SetCursor(AfxGetApp()->LoadStandardCursor());         
	       myCurrentMode = CurAction3d_Nothing;
         break;
         case CurAction3d_WindowZooming :
           myXmax=point.x;  myYmax=point.y;
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
           Standard_Failure::Raise(" incompatible Current Mode ");
        break;
        } //switch (myCurrentMode)
    } //	else // if ( Ctrl )
}

void CGeometryView::OnMButtonDown(UINT nFlags, CPoint point) 
{
   if ( nFlags & MK_CONTROL ) 
	  {
      	// Button MB2 down Control : panning init  
        // SetCursor(AfxGetApp()->LoadStandardCursor());   
	  }
}

void CGeometryView::OnMButtonUp(UINT nFlags, CPoint point) 
{
   if ( nFlags & MK_CONTROL ) 
	  {
      	// Button MB2 down Control : panning init  
        // SetCursor(AfxGetApp()->LoadStandardCursor());   
	  }
}

void CGeometryView::OnRButtonDown(UINT nFlags, CPoint point) 
{
   if ( nFlags & MK_CONTROL ) 
	  {
        // SetCursor(AfxGetApp()->LoadStandardCursor());   
	    if (!myDegenerateModeIsOn)
	      myView->SetDegenerateModeOn();
	      myView->StartRotation(point.x,point.y);  
	  }
	else // if ( Ctrl )
	  {
	    Popup(point.x,point.y);
      }	
}

void CGeometryView::OnRButtonUp(UINT nFlags, CPoint point) 
{
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    if (!myDegenerateModeIsOn)
    {  
      myView->SetDegenerateModeOff();
      myDegenerateModeIsOn = Standard_False;
    } else
    {
      myView->SetDegenerateModeOn();
      myDegenerateModeIsOn = Standard_True;
    }
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CGeometryView::OnMouseMove(UINT nFlags, CPoint point) 
{
    //   ============================  LEFT BUTTON =======================
  if ( nFlags & MK_LBUTTON)
    {
     if ( nFlags & MK_CONTROL ) 
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
           if (nFlags & MK_SHIFT)		
       	     ShiftDragEvent(myXmax,myYmax,0);
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
           Standard_Failure::Raise(" incompatible Current Mode ");
        break;
        }//  switch (myCurrentMode)
      }// if ( nFlags & MK_CONTROL )  else 
    } else //   if ( nFlags & MK_LBUTTON) 
    //   ============================  MIDDLE BUTTON =======================
    if ( nFlags & MK_MBUTTON)
    {
     if ( nFlags & MK_CONTROL ) 
	  {
		myView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
		myXmax = point.x; myYmax = point.y;	

	  }
    } else //  if ( nFlags & MK_MBUTTON)
    //   ============================  RIGHT BUTTON =======================
    if ( nFlags & MK_RBUTTON)
    {
     if ( nFlags & MK_CONTROL ) 
	  {
      	 myView->Rotation(point.x,point.y);
	  }
    }else //if ( nFlags & MK_RBUTTON)
    //   ============================  NO BUTTON =======================
    {  // No buttons 
	  myXmax = point.x; myYmax = point.y;	
	  if (nFlags & MK_SHIFT)
		ShiftMoveEvent(point.x,point.y);
	  else
		MoveEvent(point.x,point.y);
   }
}

void CGeometryView::OnUpdateBUTTONHlrOff(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myDegenerateModeIsOn);
	pCmdUI->Enable   (!myDegenerateModeIsOn);	
}

void CGeometryView::OnUpdateBUTTONHlrOn(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (!myDegenerateModeIsOn);
	pCmdUI->Enable   (myDegenerateModeIsOn);	
}

void CGeometryView::OnUpdateBUTTONPanGlo(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurAction3d_GlobalPanning);
	pCmdUI->Enable   (myCurrentMode != CurAction3d_GlobalPanning);	
	
}

void CGeometryView::OnUpdateBUTTONPan(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurAction3d_DynamicPanning);
	pCmdUI->Enable   (myCurrentMode != CurAction3d_DynamicPanning );	
}

void CGeometryView::OnUpdateBUTTONZoomProg(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurAction3d_DynamicZooming );
	pCmdUI->Enable   (myCurrentMode != CurAction3d_DynamicZooming);	
}

void CGeometryView::OnUpdateBUTTONZoomWin(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurAction3d_WindowZooming);
	pCmdUI->Enable   (myCurrentMode != CurAction3d_WindowZooming);	
}

void CGeometryView::OnUpdateBUTTONRot(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurAction3d_DynamicRotation);
	pCmdUI->Enable   (myCurrentMode != CurAction3d_DynamicRotation);	
}

//==========================================================================================
//==========================================================================================
//==========================================================================================

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryView::DragEvent(const Standard_Integer  x        ,
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
void CGeometryView::InputEvent(const Standard_Integer  x     ,
				                   const Standard_Integer  y     ) 
{
    GetDocument()->GetAISContext()->Select(); 
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryView::MoveEvent(const Standard_Integer  x       ,
                                  const Standard_Integer  y       ) 
{
      GetDocument()->GetAISContext()->MoveTo(x,y,myView);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryView::ShiftMoveEvent(const Standard_Integer  x       ,
                                  const Standard_Integer  y       ) 
{
      GetDocument()->GetAISContext()->MoveTo(x,y,myView);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CGeometryView::ShiftDragEvent(const Standard_Integer  x        ,
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
void CGeometryView::ShiftInputEvent(const Standard_Integer  x       ,
									    const Standard_Integer  y       ) 
{
	GetDocument()->GetAISContext()->ShiftSelect(); 
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void  CGeometryView::Popup(const Standard_Integer  x,
							   const Standard_Integer  y ) 
{
  Standard_Integer PopupMenuNumber=0;
  GetDocument()->GetAISContext()->InitCurrent();
  if (GetDocument()->GetAISContext()->MoreCurrent())
    PopupMenuNumber=1;

  CMenu menu;
  VERIFY(menu.LoadMenu(IDR_Popup3D));
  CMenu* pPopup = menu.GetSubMenu(PopupMenuNumber);
  ASSERT(pPopup != NULL);
 
  POINT winCoord = { x , y };
  Handle(WNT_Window) aWNTWindow=
  Handle(WNT_Window)::DownCast(myView->Window());
  ClientToScreen ( &winCoord);
  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON , winCoord.x, winCoord.y , 
                         AfxGetMainWnd());
}


