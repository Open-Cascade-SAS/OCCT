// AnimationView3D.cpp : implementation of the CAnimationView3D class
//

#include "stdafx.h"

#include "AnimationView3D.h"

#include "AnimationApp.h"
#include "ShadingDialog.h"
#include "AnimationDoc.h"

#include "Sensitivity.h"

#ifdef _DEBUG
//#define new DEBUG_NEW by CasCade
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
static int rotCount = 0;

// for elastic bean selection
#define ValZWMin 1


/////////////////////////////////////////////////////////////////////////////
// CAnimationView3D

IMPLEMENT_DYNCREATE(CAnimationView3D, CView)

BEGIN_MESSAGE_MAP(CAnimationView3D, CView)
	//{{AFX_MSG_MAP(CAnimationView3D)
	ON_COMMAND(ID_BUTTONAxo, OnBUTTONAxo)
	ON_COMMAND(ID_BUTTONBack, OnBUTTONBack)
	ON_COMMAND(ID_BUTTONBottom, OnBUTTONBottom)
	ON_COMMAND(ID_BUTTONFront, OnBUTTONFront)
	ON_COMMAND(ID_BUTTONHlrOff, OnBUTTONHlrOff)
	ON_COMMAND(ID_BUTTONHlrOn, OnBUTTONHlrOn)
	ON_COMMAND(ID_BUTTONLeft, OnBUTTONLeft)
	ON_COMMAND(ID_BUTTONPan, OnBUTTONPan)
	ON_COMMAND(ID_BUTTONPanGlo, OnBUTTONPanGlo)
	ON_COMMAND(ID_BUTTONReset, OnBUTTONReset)
	ON_COMMAND(ID_BUTTONRight, OnBUTTONRight)
	ON_COMMAND(ID_BUTTONRot, OnBUTTONRot)
	ON_COMMAND(ID_BUTTONTop, OnBUTTONTop)
	ON_COMMAND(ID_BUTTONZoomAll, OnBUTTONZoomAll)
    ON_COMMAND(ID_FILE_EXPORT_IMAGE, OnFileExportImage)
	ON_WM_SIZE()
	ON_COMMAND(ID_BUTTONZoomProg, OnBUTTONZoomProg)
	ON_COMMAND(ID_BUTTONZoomWin, OnBUTTONZoomWin)
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

	ON_COMMAND(ID_Modify_ChangeBackground     , OnChangeBackground)
	ON_WM_TIMER()
	ON_COMMAND(ID_STOP, OnStop)
	ON_COMMAND(ID_RESTART, OnRestart)

	ON_COMMAND(ID_SENSITIVITY, OnSensitivity)
	ON_COMMAND(ID_BUTTONFly, OnBUTTONFly)
	ON_COMMAND(ID_BUTTONTurn, OnBUTTONTurn)
	ON_UPDATE_COMMAND_UI(ID_BUTTONFly, OnUpdateBUTTONFly)
	ON_UPDATE_COMMAND_UI(ID_BUTTONTurn, OnUpdateBUTTONTurn)
	ON_COMMAND(ID_VIEW_DISPLAYSTATUS, OnViewDisplaystatus)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISPLAYSTATUS, OnUpdateViewDisplaystatus)
	//}}AFX_MSG_MAP
// CasCade

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimationView3D construction/destruction

CAnimationView3D::CAnimationView3D()
: myXmin (0),
  myYmin (0),
  myXmax (0),
  myYmax (0),
  myCurZoom (0.0),
  myHlrModeIsOn (Standard_False),
  myCurrentMode  (CurrentAction3d_Nothing),
  m_FlySens  (500.0),
  m_TurnSens (M_PI / 40.0),
  m_Pen (NULL)
{
  // TODO: add construction code here
}

CAnimationView3D::~CAnimationView3D()
{
    myView->Remove();
    if (m_Pen) delete m_Pen;
}

BOOL CAnimationView3D::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs
  cs.lpszClass = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
  return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAnimationView3D drawing

void CAnimationView3D::OnDraw(CDC* /*pDC*/)
{
	CAnimationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here

	  myView->Redraw();

}
void CAnimationView3D::OnInitialUpdate() 
{
  CView::OnInitialUpdate();

  // TODO: Add your specialized code here and/or call the base class
  //	myView = GetDocument()->GetViewer()->CreateView();

  Handle(V3d_Viewer) aViewer;

  aViewer = GetDocument()->GetViewer();
  aViewer->SetDefaultTypeOfView (V3d_PERSPECTIVE);

  myView = aViewer->CreateView();

  // store for restore state after rotation (witch is in Degenerated mode)
  myHlrModeIsOn = myView->ComputedMode();

  Handle(WNT_Window) aWNTWindow = new WNT_Window (GetSafeHwnd());
  myView->SetWindow(aWNTWindow);
  if (!aWNTWindow->IsMapped()) aWNTWindow->Map();

  // store the mode ( nothing , dynamic zooming, dynamic ... )
  myCurrentMode = CurrentAction3d_Nothing;
  CFrameWnd* pParentFrm = GetParentFrame();
  pParentFrm->ActivateFrame(SW_SHOWMAXIMIZED);

  Standard_Integer w=100 , h=100 ;   /* Debug Matrox                         */
  aWNTWindow->Size (w,h) ;           /* Keeps me unsatisfied (rlb).....      */
                                   /* Resize is not supposed to be done on */
                                   /* Matrox                               */
                                   /* I suspect another problem elsewhere  */
  ::PostMessage ( GetSafeHwnd () , WM_SIZE , SIZE_RESTORED , w + h*65536 ) ;

  m_Tune.Create ( IDD_TUNE , NULL ) ;

  RECT dlgrect;
  m_Tune.GetWindowRect(&dlgrect);
  LONG width = dlgrect.right-dlgrect.left;
  LONG height = dlgrect.bottom-dlgrect.top;
  RECT MainWndRect;
  AfxGetApp()->m_pMainWnd->GetWindowRect(&MainWndRect);
  LONG left = MainWndRect.left+3;
  LONG top = MainWndRect.top + 112;
  m_Tune.MoveWindow(left,top,width,height);

  m_Tune.m_pView = this ;

  m_Tune.ShowWindow ( SW_HIDE );

  // store the mode ( nothing , dynamic zooming, dynamic ... )

  myCurrentMode = CurrentAction3d_Nothing;

  ReloadData () ;

}

void CAnimationView3D::DisplayTuneDialog()
{
	m_Tune.Create ( IDD_TUNE , NULL ) ;
	
	RECT dlgrect;
	m_Tune.GetWindowRect(&dlgrect);
	LONG width = dlgrect.right-dlgrect.left;
	LONG height = dlgrect.bottom-dlgrect.top;
	RECT MainWndRect;
	AfxGetApp()->m_pMainWnd->GetWindowRect(&MainWndRect);
	LONG left = MainWndRect.left+3;
	LONG top = MainWndRect.top + 112;
	m_Tune.MoveWindow(left,top,width,height);
	
	m_Tune.m_pView = this ;
}


/////////////////////////////////////////////////////////////////////////////
// CAnimationView3D diagnostics

#ifdef _DEBUG
void CAnimationView3D::AssertValid() const
{
	CView::AssertValid();
}

void CAnimationView3D::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAnimationDoc* CAnimationView3D::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAnimationDoc)));
	return (CAnimationDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAnimationView3D message handlers
void CAnimationView3D::OnFileExportImage()
{
  GetDocument()->ExportView (myView);
}

void CAnimationView3D::OnSize(UINT nType, int cx, int cy)
{
  CView::OnSize (nType, cx, cy);
  m_cx = cx ;
  m_cy = cy ;
  if (!myView.IsNull())
    myView->MustBeResized();
}

void CAnimationView3D::OnBUTTONBack() 
{ myView->SetProj(V3d_Ypos); } // See the back View
void CAnimationView3D::OnBUTTONFront() 
{ myView->SetProj(V3d_Yneg); } // See the front View

void CAnimationView3D::OnBUTTONBottom() 
{ myView->SetProj(V3d_Zneg); } // See the bottom View
void CAnimationView3D::OnBUTTONTop() 
{ myView->SetProj(V3d_Zpos); } // See the top View	

void CAnimationView3D::OnBUTTONLeft() 
{ myView->SetProj(V3d_Xneg); } // See the left View	
void CAnimationView3D::OnBUTTONRight() 
{ myView->SetProj(V3d_Xpos); } // See the right View

void CAnimationView3D::OnBUTTONAxo() 
{ myView->SetProj(V3d_XposYnegZpos); } // See the axonometric View

void CAnimationView3D::OnBUTTONHlrOff() 
{
  myHlrModeIsOn = Standard_False;
  myView->SetComputedMode (myHlrModeIsOn);
  myView->Redraw();
}

void CAnimationView3D::OnBUTTONHlrOn() 
{
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
  myHlrModeIsOn = Standard_True;
  myView->SetComputedMode (myHlrModeIsOn);
  myView->Redraw();
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CAnimationView3D::OnBUTTONPan() 
{  
	myCurrentMode = CurrentAction3d_DynamicPanning; 
}

void CAnimationView3D::OnBUTTONPanGlo() 
{
  // save the current zoom value 
  myCurZoom = myView->Scale();
  // Do a Global Zoom 
  myView->FitAll();
  // Set the mode 
  myCurrentMode = CurrentAction3d_GlobalPanning;
}

void CAnimationView3D::OnBUTTONReset() 
{   myView->Reset(); 
	ReloadData();
}

void CAnimationView3D::OnBUTTONRot() 
{   myCurrentMode = CurrentAction3d_DynamicRotation; }


void CAnimationView3D::OnBUTTONZoomAll() 
{
	SetDimensions();
	myView->FitAll();
	myView->ZFitAll();
}

void CAnimationView3D::OnBUTTONZoomProg() 
{  myCurrentMode = CurrentAction3d_DynamicZooming; }

void CAnimationView3D::OnBUTTONZoomWin() 
{  myCurrentMode = CurrentAction3d_WindowZooming; }

void CAnimationView3D::OnBUTTONFly() 
{  myCurrentMode = CurrentAction3d_Fly; }

void CAnimationView3D::OnBUTTONTurn() 
{  myCurrentMode = CurrentAction3d_Turn; }


void CAnimationView3D::OnLButtonDown(UINT nFlags, CPoint point) 
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
         case CurrentAction3d_Nothing : // start a drag
           if (nFlags & MK_SHIFT)
       	        GetDocument()->ShiftDragEvent(myXmax,myYmax,-1,myView);
           else
                GetDocument()->DragEvent(myXmax,myYmax,-1,myView);
        break;
         break;
         case CurrentAction3d_DynamicZooming : // noting
             // SetCursor(AfxGetApp()->LoadStandardCursor());
         break;
         case CurrentAction3d_WindowZooming : // noting
         break;
         case CurrentAction3d_DynamicPanning :// noting
         break;
         case CurrentAction3d_GlobalPanning :// noting
        break;
        case  CurrentAction3d_DynamicRotation :
          if (myHlrModeIsOn)
          {
            myView->SetComputedMode (Standard_False);
          }
          myView->StartRotation (point.x, point.y);
        break;
		case  CurrentAction3d_Fly :
			KillTimer (1) ;
			SetTimer ( 1 , 100 , NULL ) ;
		break ;
        case  CurrentAction3d_Turn :
			KillTimer (1) ;
			SetTimer ( 1 , 100 , NULL ) ;
		break ;
        default :
           throw Standard_Failure(" incompatible Current Mode ");
        break;
        }
    }
}

void CAnimationView3D::OnLButtonUp(UINT nFlags, CPoint point) 
{
   if ( nFlags & MK_CONTROL ) 
	  {
        return;
	  }
	else // if ( Ctrl )
	  {
        switch (myCurrentMode)
        {
         case CurrentAction3d_Nothing :
         if (point.x == myXmin && point.y == myYmin)
         { // no offset between down and up --> selectEvent
            myXmax=point.x;  
            myYmax=point.y;
            if (nFlags & MK_SHIFT )
              GetDocument()->ShiftInputEvent(point.x,point.y,myView);
            else
              GetDocument()->InputEvent     (point.x,point.y,myView);
         } else
         {
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False);
            myXmax=point.x;  
            myYmax=point.y;
		    if (nFlags & MK_SHIFT)
				GetDocument()->ShiftDragEvent(point.x,point.y,1,myView);
			else
				GetDocument()->DragEvent(point.x,point.y,1,myView);
         }
         break;
         case CurrentAction3d_DynamicZooming :
             // SetCursor(AfxGetApp()->LoadStandardCursor());         
	       myCurrentMode = CurrentAction3d_Nothing;
         break;
         case CurrentAction3d_WindowZooming :
           myXmax=point.x;            myYmax=point.y;
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False,LongDash);
	       if ((abs(myXmin-myXmax)>ValZWMin) || (abs(myYmin-myYmax)>ValZWMin))
					 // Test if the zoom window is greater than a minimale window.
			{
			  // Do the zoom window between Pmin and Pmax
			  myView->WindowFitAll(myXmin,myYmin,myXmax,myYmax);  
			}  
	       myCurrentMode = CurrentAction3d_Nothing;
         break;
         case CurrentAction3d_DynamicPanning :
           myCurrentMode = CurrentAction3d_Nothing;
         break;
         case CurrentAction3d_GlobalPanning :
	       myView->Place(point.x,point.y,myCurZoom); 
	       myCurrentMode = CurrentAction3d_Nothing;
        break;
        case  CurrentAction3d_DynamicRotation :
	       myCurrentMode = CurrentAction3d_Nothing;
        break;
		case  CurrentAction3d_Fly :
			KillTimer ( 1 ) ;
        case  CurrentAction3d_Turn :
			KillTimer ( 1 ) ;
		break;
        default :
           throw Standard_Failure(" incompatible Current Mode ");
        break;
        } //switch (myCurrentMode)
    } //	else // if ( Ctrl )
}

void CAnimationView3D::OnMButtonDown(UINT nFlags, CPoint /*point*/) 
{
   if ( nFlags & MK_CONTROL ) 
	  {
      	// Button MB2 down Control : panning init  
        // SetCursor(AfxGetApp()->LoadStandardCursor());   
	  }
}

void CAnimationView3D::OnMButtonUp(UINT nFlags, CPoint /*point*/) 
{
   if ( nFlags & MK_CONTROL ) 
	  {
      	// Button MB2 down Control : panning init  
        // SetCursor(AfxGetApp()->LoadStandardCursor());   
	  }
}

void CAnimationView3D::OnRButtonDown(UINT nFlags, CPoint point) 
{
  if ( nFlags & MK_CONTROL )
  {
    // SetCursor(AfxGetApp()->LoadStandardCursor());
    if (myHlrModeIsOn)
    {
      myView->SetComputedMode (Standard_False);
    }
    myView->StartRotation (point.x, point.y);
  }
  else // if ( Ctrl )
  {
    GetDocument()->Popup(point.x,point.y,myView);
  }
}

void CAnimationView3D::OnRButtonUp(UINT /*nFlags*/, CPoint /*point*/) 
{
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    if (myHlrModeIsOn)
    {
      myView->SetComputedMode (myHlrModeIsOn);
      myView->Redraw();
    }
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CAnimationView3D::OnMouseMove(UINT nFlags, CPoint point) 
{
    //   ============================  LEFT BUTTON =======================
  m_curx = point.x ;
  m_cury = point.y ;

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
         case CurrentAction3d_Nothing :
		   myXmax = point.x;            myYmax = point.y;
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False);
           if (nFlags & MK_SHIFT)		
       	     GetDocument()->ShiftDragEvent(myXmax,myYmax,0,myView);
           else
             GetDocument()->DragEvent(myXmax,myYmax,0,myView);
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_True);
          break;
         case CurrentAction3d_DynamicZooming :
	       myView->Zoom(myXmax,myYmax,point.x,point.y); 
	       // save the current mouse coordinate in min \n";
	       myXmax=point.x;  myYmax=point.y;
         break;
         case CurrentAction3d_WindowZooming :
		   myXmax = point.x; myYmax = point.y;	
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_False,LongDash);
            DrawRectangle(myXmin,myYmin,myXmax,myYmax,Standard_True,LongDash);
         break;
         case CurrentAction3d_DynamicPanning :
		   myView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
		   myXmax = point.x; myYmax = point.y;	
         break;
         case CurrentAction3d_GlobalPanning : // nothing           
        break;
        case  CurrentAction3d_DynamicRotation :
          myView->Rotation(point.x,point.y);
	      myView->Redraw();
        break;
		case CurrentAction3d_Fly :
			break ;
		case CurrentAction3d_Turn :
			break ;
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
		myView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
		myXmax = point.x; myYmax = point.y;	

	  }
    } else //  if ( nFlags & MK_MBUTTON)
    //   ============================  RIGHT BUTTON =======================
    if ( nFlags & MK_RBUTTON)
    {
     if ( nFlags & MK_CONTROL ) 
	  {
	     rotCount++;
      	 myView->Rotation(point.x,point.y);
	  }
    }else //if ( nFlags & MK_RBUTTON)
    //   ============================  NO BUTTON =======================
    {  // No buttons 
	  myXmax = point.x; myYmax = point.y;	
	  if (nFlags & MK_SHIFT)
		GetDocument()->ShiftMoveEvent(point.x,point.y,myView);
	  else
		GetDocument()->MoveEvent(point.x,point.y,myView);
   }
}

void CAnimationView3D::OnUpdateBUTTONHlrOff(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (!myHlrModeIsOn);
  pCmdUI->Enable   (myHlrModeIsOn);
}

void CAnimationView3D::OnUpdateBUTTONHlrOn(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (myHlrModeIsOn);
  pCmdUI->Enable   (!myHlrModeIsOn);
}

void CAnimationView3D::OnUpdateBUTTONPanGlo(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_GlobalPanning);
	pCmdUI->Enable   (myCurrentMode != CurrentAction3d_GlobalPanning);	
	
}

void CAnimationView3D::OnUpdateBUTTONPan(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_DynamicPanning);
	pCmdUI->Enable   (myCurrentMode != CurrentAction3d_DynamicPanning );	
}

void CAnimationView3D::OnUpdateBUTTONZoomProg(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_DynamicZooming );
	pCmdUI->Enable   (myCurrentMode != CurrentAction3d_DynamicZooming);	
}

void CAnimationView3D::OnUpdateBUTTONZoomWin(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_WindowZooming);
	pCmdUI->Enable   (myCurrentMode != CurrentAction3d_WindowZooming);	
}

void CAnimationView3D::OnUpdateBUTTONRot(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_DynamicRotation);
	pCmdUI->Enable   (myCurrentMode != CurrentAction3d_DynamicRotation);	
}

void CAnimationView3D::OnUpdateBUTTONFly(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetDocument()->m_bIsGridLoaded);
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_Fly);
}

void CAnimationView3D::OnUpdateBUTTONTurn(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetDocument()->m_bIsGridLoaded);
    pCmdUI->SetCheck (myCurrentMode == CurrentAction3d_Turn);
}

void CAnimationView3D::OnChangeBackground() 
{
	Standard_Real R1;
	Standard_Real G1;
	Standard_Real B1;
    myView->BackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
	COLORREF m_clr ;
	m_clr = RGB(R1*255,G1*255,B1*255);

	CColorDialog dlgColor(m_clr);
	if (dlgColor.DoModal() == IDOK)
	{
		m_clr = dlgColor.GetColor();
		R1 = GetRValue(m_clr)/255.;
		G1 = GetGValue(m_clr)/255.;
		B1 = GetBValue(m_clr)/255.;
        myView->SetBackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
	}
    myView->Redraw();
}

//==========================================================================================
//==========================================================================================
//==========================================================================================

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CAnimationView3D::DrawRectangle(const Standard_Integer  MinX    ,
					                    const Standard_Integer  MinY    ,
                                        const Standard_Integer  MaxX ,
					                    const Standard_Integer  MaxY ,
					                    const Standard_Boolean  Draw , 
                                        const LineStyle aLineStyle)
{
    static int m_DrawMode;
    if  (!m_Pen && aLineStyle ==Solid )
        {m_Pen = new CPen(PS_SOLID, 1, RGB(0,0,0)); m_DrawMode = R2_MERGEPENNOT;}
    else if (!m_Pen && aLineStyle ==Dot )
        {m_Pen = new CPen(PS_DOT, 1, RGB(0,0,0));   m_DrawMode = R2_XORPEN;}
    else if (!m_Pen && aLineStyle == ShortDash)
        {m_Pen = new CPen(PS_DASH, 1, RGB(255,0,0));	m_DrawMode = R2_XORPEN;}
    else if (!m_Pen && aLineStyle == LongDash)
        {m_Pen = new CPen(PS_DASH, 1, RGB(0,0,0));	m_DrawMode = R2_NOTXORPEN;}
    else if (aLineStyle == Default) 
        { m_Pen = NULL;	m_DrawMode = R2_MERGEPENNOT;}

    CPen* aOldPen = NULL;
    CClientDC clientDC(this);
    if (m_Pen) aOldPen = clientDC.SelectObject(m_Pen);
    clientDC.SetROP2(m_DrawMode);

    static		Standard_Integer StoredMinX, StoredMaxX, StoredMinY, StoredMaxY;
    static		Standard_Boolean m_IsVisible;

    if ( m_IsVisible && !Draw) // move or up  : erase at the old position 
    {
     clientDC.MoveTo(StoredMinX,StoredMinY); clientDC.LineTo(StoredMinX,StoredMaxY); 
     clientDC.LineTo(StoredMaxX,StoredMaxY); 
	 clientDC.LineTo(StoredMaxX,StoredMinY); clientDC.LineTo(StoredMinX,StoredMinY);
     m_IsVisible = false;
    }

    StoredMinX = Min ( MinX, MaxX );
    StoredMinY = Min ( MinY, MaxY );
    StoredMaxX = Max ( MinX, MaxX );
    StoredMaxY = Max ( MinY, MaxY);

    if (Draw) // move : draw
    {
     clientDC.MoveTo(StoredMinX,StoredMinY); clientDC.LineTo(StoredMinX,StoredMaxY); 
     clientDC.LineTo(StoredMaxX,StoredMaxY); 
	 clientDC.LineTo(StoredMaxX,StoredMinY); clientDC.LineTo(StoredMinX,StoredMinY);
     m_IsVisible = true;
   }

   if (m_Pen) 
       clientDC.SelectObject(aOldPen);
}
void CAnimationView3D::OnStop() 
{
	KillTimer(GetDocument()->myCount);            
}

void CAnimationView3D::OnRestart() 
{
	KillTimer(GetDocument()->myCount);            
	SetTimer(GetDocument()->myCount, 1 , NULL); 
}

/*
void CAnimationView3D::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	GetDocument()->OnMyTimer();
	CView::OnTimer(nIDEvent);
}
*/


/*********************************************************************************
**************  W A L K  T H R O U G H  ******************************************
/********************************************************************************/

void CAnimationView3D::OnTimer(UINT_PTR nIDEvent) 
{
	if ( !GetDocument()->m_bIsGridLoaded )
	{
		// TODO: Add your message handler code here and/or call default
		GetDocument()->OnMyTimer();
		CView::OnTimer(nIDEvent);
	}
	else
	{
		CView::OnTimer(nIDEvent);
		if ( nIDEvent == 1 ) {
		  myView->SetImmediateUpdate ( Standard_False ) ;
		  if ( myCurrentMode == CurrentAction3d_Fly ) {

			 Fly  ( m_curx , m_cury ) ;
			 if ( m_bShift )
			   Roll ( m_curx , m_cury ) ;
			 else
			   Turn ( m_curx , m_cury ) ;

 			myView->SetAt  ( m_Atx  , m_Aty  , m_Atz  ) ;
			myView->SetEye ( m_Eyex , m_Eyey , m_Eyez ) ;

		  }
		  else if ( myCurrentMode == CurrentAction3d_Turn ) {
			   Twist ( m_curx , m_cury ) ;
		  }
		  else
			  KillTimer (1) ;


  		  myView->SetImmediateUpdate ( Standard_True ) ;

		  myView->Update ();
		}

		ReloadData () ;
	}
}

void CAnimationView3D::OnSensitivity() 
{
	CSensitivity dial ;

	dial.m_SensFly   = m_FlySens  ;
	dial.m_SensTurn  = m_TurnSens ;
	if ( dial.DoModal () ) {
		m_FlySens  = dial.m_SensFly   ;
        m_TurnSens = dial.m_SensTurn  ;
	}
}

void CAnimationView3D::Fly (int /*x*/ , int y)
{
	double v [3] ;
	double l ;
	double sens ;
	int    i     ;

	sens = (double) myYmin - (double) y ;
	sens /= (double) m_cy ;
	sens *= m_FlySens ;

	v [0] = m_Atx - m_Eyex ;
	v [1] = m_Aty - m_Eyey ;
	v [2] = m_Atz - m_Eyez ;
	l = sqrt ( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] ) ;
	if ( l > 1.e-3 ) {
		for ( i=0 ; i<3 ; i++ )
          v [i] = v [i] / l * sens ;

		m_Atx += v [0] ;
		m_Aty += v [1] ;
		m_Atz += v [2] ;

		m_Eyex += v [0] ;
		m_Eyey += v [1] ;
		m_Eyez += v [2] ;

	}
}

/* Rotation */

void CAnimationView3D::Turn (int x , int /*y*/)
{
	gp_Vec z (0.,0.,1.) ;

	double v [3] ;
	double sens ;
	double aX , aY , aZ ;

	sens = (double) x - (double) myXmin ;
	sens /= (double) m_cx ;
	sens *= m_TurnSens ;

	v [0] = m_Atx - m_Eyex ;
	v [1] = m_Aty - m_Eyey ;
	v [2] = m_Atz - m_Eyez ;

	gp_Pnt eye ( m_Eyex , m_Eyey , m_Eyez ) ;

	gp_Vec reg (v[0],v[1],v[2] );

	gp_Vec vert = reg ^ z ;
	gp_Vec haut = vert ^ reg ;

	gp_Dir dh (haut) ;
	gp_Ax1 rot (eye,dh);

	reg.Rotate (rot,sens) ;

	reg.Coord ( aX , aY , aZ ) ;

	m_Atx = m_Eyex + aX ;
	m_Aty = m_Eyey + aY ;
	m_Atz = m_Eyez + aZ ;
}

void CAnimationView3D::Roll (int x , int /*y*/)
{
	gp_Vec z (0.,0.,1.) ;

	double v [3] ;
	double sens ;
	double aX , aY , aZ ;

	sens = (double) x - (double) myXmin ;
	sens /= (double) m_cx ;
	sens *= m_TurnSens ;

	v [0] = m_Atx - m_Eyex ;
	v [1] = m_Aty - m_Eyey ;
	v [2] = m_Atz - m_Eyez ;

	gp_Pnt eye ( m_Eyex , m_Eyey , m_Eyez ) ;

	gp_Vec reg (v[0],v[1],v[2] );

	gp_Vec vert = reg ^ z ;

	gp_Dir dh (vert) ;
	gp_Ax1 rot (eye,dh);

	reg.Rotate (rot,sens) ;

	reg.Coord ( aX , aY , aZ ) ;

	m_Atx = m_Eyex + aX ;
	m_Aty = m_Eyey + aY ;
	m_Atz = m_Eyez + aZ ;
}

void CAnimationView3D::Twist (int x , int /*y*/)
{
	double sens ;
	double a ;
	
	a = myView->Twist () ;

	sens = (double) x - (double) myXmin ;
	sens /= (double) m_cx ;
	sens *= m_TurnSens ;

	a += sens ;

	myView->SetTwist (a) ;
}

//=============================================================================
// function: SetFocal
// purpose:
//=============================================================================
void CAnimationView3D::SetFocal (double theFocus, double theAngle)
{

  Handle(Graphic3d_Camera) aCamera = myView->Camera();

  gp_Pnt anAt  = aCamera->Center();
  gp_Pnt anEye = aCamera->Eye();

  gp_Vec aLook (anAt, anEye);

  if (aCamera->Distance() > 1.e-3)
  {
    aLook = aLook / aCamera->Distance() * theFocus;

    m_Focus = theFocus;

    anAt.SetX (aLook.X() + anEye.X());
    anAt.SetY (aLook.Y() + anEye.Y());
    anAt.SetZ (aLook.Z() + anEye.Z());

    m_dAngle = theAngle;

    aCamera->SetCenter (anAt);
    aCamera->SetFOVy (theAngle);

    myView->Update();
  }
}

void CAnimationView3D::ReloadData()
{
	myView->At  ( m_Atx  , m_Aty  , m_Atz  ) ;
	myView->Eye ( m_Eyex , m_Eyey , m_Eyez ) ;
  double dTwist = myView->Twist() * 180. / M_PI;

  CString aMsg;
  aMsg.Format (L"%lf", m_Atx);
	m_Tune.GetDlgItem (IDC_XAT)->SetWindowText (aMsg);
  aMsg.Format (L"%lf", m_Aty);
	m_Tune.GetDlgItem (IDC_YAT)->SetWindowText (aMsg);
	aMsg.Format (L"%lf", m_Atz);
	m_Tune.GetDlgItem (IDC_ZAT)->SetWindowText (aMsg);

  aMsg.Format (L"%lf", m_Eyex);
	m_Tune.GetDlgItem (IDC_XEYE)->SetWindowText (aMsg);
	aMsg.Format (L"%lf", m_Eyey);
	m_Tune.GetDlgItem (IDC_YEYE)->SetWindowText (aMsg);
	aMsg.Format (L"%lf", m_Eyez);
	m_Tune.GetDlgItem (IDC_ZEYE)->SetWindowText (aMsg);

  aMsg.Format (L"%lf", dTwist);
	m_Tune.GetDlgItem (IDC_TWIST)->SetWindowText (aMsg);

	double dx,dy,dz ;
	dx = m_Atx - m_Eyex ;
	dy = m_Aty - m_Eyey ;
	dz = m_Atz - m_Eyez ;

  m_Focus = sqrt (dx * dx + dy * dy + dz * dz);

  m_dAngle = myView->Camera()->FOVy();

	m_Tune.m_dAngle = m_dAngle ;
	m_Tune.m_dFocus = m_Focus  ;
	m_Tune.UpdateData ( FALSE ) ;
}

void CAnimationView3D::SetDimensions()
{

  CAnimationDoc* pDoc = GetDocument();

  myView->SetImmediateUpdate ( Standard_False ) ;

  m_Atx  = ( pDoc->m_Xmin + pDoc->m_Xmax ) / 2. ;
  m_Aty  = ( pDoc->m_Ymin + pDoc->m_Ymax ) / 2. ;
  m_Atz  = ( pDoc->m_Zmin + pDoc->m_Zmax ) / 2. ;
  m_Eyex = pDoc->m_Xmax ;
  m_Eyey = pDoc->m_Ymax ;
  m_Eyez = pDoc->m_Zmax ;

  myView->SetAt    ( m_Atx  , m_Aty  , m_Atz  ) ;
  myView->SetEye   ( m_Eyex , m_Eyey , m_Eyez ) ;
  myView->SetTwist (0.) ;

  myView->SetImmediateUpdate ( Standard_False ) ;
  myView->FitAll();
  myView->SetImmediateUpdate ( Standard_False ) ;
  myView->ZFitAll();

  myView->SetImmediateUpdate ( Standard_True ) ;

  ReloadData () ;
  myView->Update ();
}

void CAnimationView3D::OnViewDisplaystatus() 
{
	// TODO: Add your command handler code here

	if ( m_Tune.IsWindowVisible () ) {

	}
	else {
		m_Tune.ShowWindow ( SW_SHOWNORMAL ) ;
	}
}

void CAnimationView3D::OnUpdateViewDisplaystatus(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if ( m_Tune.IsWindowVisible () ) {
		pCmdUI->SetCheck ( 1 ) ;
	}
	else {
		pCmdUI->SetCheck ( 0 ) ;
	}
}
