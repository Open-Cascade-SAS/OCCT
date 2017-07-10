// OCC_3dView.cpp: implementation of the OCC_3dView class.
//

#include "stdafx.h"

#include "OCC_3dView.h"
#include "OCC_App.h"
#include "OCC_3dBaseDoc.h"
#include <res\OCC_Resource.h>

#include <Graphic3d_ExportFormat.hxx>
#include <Graphic3d_Camera.hxx>

#include <OpenGl_GraphicDriver.hxx>

#define ValZWMin 1

IMPLEMENT_DYNCREATE(OCC_3dView, CView)

BEGIN_MESSAGE_MAP(OCC_3dView, CView)
	//{{AFX_MSG_MAP(OCC_3dView)
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
  ON_COMMAND(ID_BUTTON_STEREOCONFIG, OnStereoConfigButton)
	ON_WM_SIZE()
    ON_COMMAND(ID_FILE_EXPORT_IMAGE, OnFileExportImage)
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
  ON_UPDATE_COMMAND_UI(ID_BUTTON_STEREOCONFIG, OnUpdateStereoConfigButton)
	ON_COMMAND(ID_Modify_ChangeBackground     , OnModifyChangeBackground)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OCC_3dView construction/destruction

OCC_3dView::OCC_3dView()
: myCurrentMode (CurAction3d_Nothing),
  myWidth  (0),
  myHeight (0),
  myHlrModeIsOn (Standard_False)
{
  // TODO: add construction code here
}

OCC_3dView::~OCC_3dView()
{
	if (myView.IsNull())
  {
    myView->Remove();
  }

  delete m_pStereoDlg;
}

BOOL OCC_3dView::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs
  cs.lpszClass = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
  return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// OCC_3dView drawing
void OCC_3dView::OnInitialUpdate() 
{
  CView::OnInitialUpdate();

  myView = GetDocument()->GetViewer()->CreateView();

  // store for restore state after rotation (which is in Degenerated mode)
  myHlrModeIsOn = Standard_False;
  myView->SetComputedMode (myHlrModeIsOn);

  Handle(OpenGl_GraphicDriver) aDriver = 
    Handle(OpenGl_GraphicDriver)::DownCast (((OCC_App*)AfxGetApp())->GetGraphicDriver());

  Handle(WNT_Window) aWNTWindow = new WNT_Window(GetSafeHwnd());

  myView->SetWindow(aWNTWindow);
  myView->Camera()->SetProjectionType (aDriver->Options().contextStereo
    ? Graphic3d_Camera::Projection_Stereo
    : Graphic3d_Camera::Projection_Orthographic);

  if (!aWNTWindow->IsMapped())
  {
    aWNTWindow->Map();
  }

  // store the mode ( nothing , dynamic zooming, dynamic ... )
  myCurrentMode = CurAction3d_Nothing;

  m_pStereoDlg = new OCC_StereoConfigDlg (this);
  m_pStereoDlg->SetView (myView);
  m_pStereoDlg->Create (IDD_DIALOG_STEREO, this);
}

void OCC_3dView::OnDraw(CDC* /*pDC*/)
{
  CRect aRect;
  GetWindowRect(aRect);
  if(myWidth != aRect.Width() || myHeight != aRect.Height()) {
    myWidth = aRect.Width();
    myHeight = aRect.Height();
    ::PostMessage ( GetSafeHwnd() , WM_SIZE , SW_SHOW , myWidth + myHeight*65536 );
  }
  myView->Redraw();
}

/////////////////////////////////////////////////////////////////////////////
// OCC_3dView diagnostics

#ifdef _DEBUG
void OCC_3dView::AssertValid() const
{
	CView::AssertValid();
}

void OCC_3dView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

OCC_3dDoc* OCC_3dView::GetDocument() // non-debug version is inline
{
//	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(OCC_3dBaseDoc)));
	return (OCC_3dDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// OCC_3dView message handlers
void OCC_3dView::OnFileExportImage()
{
  GetDocument()->ExportView (myView);
}

void OCC_3dView::OnSize(UINT nType, int cx, int cy)
{
  OCC_BaseView::OnSize (nType, cx, cy);
  if (!myView.IsNull())
   myView->MustBeResized();
}

// See the back View
void OCC_3dView::OnBUTTONBack() 
{
  myView->SetProj(V3d_Ypos);
} 

// See the front View
void OCC_3dView::OnBUTTONFront() 
{
  myView->SetProj(V3d_Yneg);
} 

// See the bottom View
void OCC_3dView::OnBUTTONBottom() 
{
  myView->SetProj(V3d_Zneg);
}

// See the top View
void OCC_3dView::OnBUTTONTop() 
{
  myView->SetProj(V3d_Zpos);
} 	

// See the left View
void OCC_3dView::OnBUTTONLeft() 
{
  myView->SetProj(V3d_Xneg);
}

// See the right View
void OCC_3dView::OnBUTTONRight() 
{
  myView->SetProj(V3d_Xpos);
} 

// See the axonometric View
void OCC_3dView::OnBUTTONAxo() 
{
  myView->SetProj(V3d_XposYnegZpos);
} 

void OCC_3dView::OnBUTTONHlrOff() 
{
  myHlrModeIsOn = Standard_False;
  myView->SetComputedMode (myHlrModeIsOn);
  myView->Redraw();
}

void OCC_3dView::OnBUTTONHlrOn() 
{
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
  myHlrModeIsOn = Standard_True;
  myView->SetComputedMode (myHlrModeIsOn);
  myView->Redraw();
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void OCC_3dView::OnBUTTONPan() 
{
  myCurrentMode = CurAction3d_DynamicPanning;
}

void OCC_3dView::OnBUTTONPanGlo() 
{
  // save the current zoom value 
  myCurZoom = myView->Scale();
  // Do a Global Zoom 
  //myView->FitAll();
  // Set the mode 
  myCurrentMode = CurAction3d_GlobalPanning;
}

void OCC_3dView::OnBUTTONReset() 
{
  myView->Reset();
}

void OCC_3dView::OnBUTTONRot() 
{
  myCurrentMode = CurAction3d_DynamicRotation; 
}

void OCC_3dView::OnBUTTONZoomAll() 
{
  myView->FitAll();
  myView->ZFitAll();
}

void OCC_3dView::OnBUTTONZoomProg() 
{  myCurrentMode = CurAction3d_DynamicZooming; }

void OCC_3dView::OnBUTTONZoomWin() 
{  myCurrentMode = CurAction3d_WindowZooming; }

void OCC_3dView::OnLButtonDown(UINT nFlags, CPoint point) 
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
        GetDocument()->ShiftDragEvent(myXmax,myYmax,-1,myView);
      else
        GetDocument()->DragEvent(myXmax,myYmax,-1,myView);
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
      if (myHlrModeIsOn)
      {
        myView->SetComputedMode (Standard_False);
      }

      myView->StartRotation(point.x,point.y);  
      break;
    default :
      throw Standard_Failure(" incompatible Current Mode ");
      break;
    }
  }
}

void OCC_3dView::OnLButtonUp(UINT nFlags, CPoint point) 
{
  if ( nFlags & MK_CONTROL ) 
  {
    return;
  }
  else // if ( Ctrl )
  {
    const Handle(AIS_InteractiveContext)& aContext = GetDocument()->GetAISContext();
    switch (myCurrentMode)
    {
    case CurAction3d_Nothing :
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
        myXmax=point.x;    myYmax=point.y;
        drawRectangle (myXmin, myYmin, myXmax, myYmax, aContext, Standard_False);
        if (nFlags & MK_SHIFT)
          GetDocument()->ShiftDragEvent(point.x,point.y,1,myView);
        else
          GetDocument()->DragEvent(point.x,point.y,1,myView);
      }
      break;
    case CurAction3d_DynamicZooming :
      myCurrentMode = CurAction3d_Nothing;
      break;
    case CurAction3d_WindowZooming :
      myXmax=point.x;        myYmax=point.y;
      drawRectangle (myXmin, myYmin, myXmax, myYmax, aContext, Standard_False);
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
      if (myHlrModeIsOn)
      {
        CWaitCursor aWaitCursor;
        myView->SetComputedMode (myHlrModeIsOn);
        myView->Redraw();
      }
      else
      {
        myView->SetComputedMode (myHlrModeIsOn);
      }
      break;
    default :
      throw Standard_Failure(" incompatible Current Mode ");
      break;
    } //switch (myCurrentMode)
  } //	else // if ( Ctrl )
}

void OCC_3dView::OnMButtonDown(UINT nFlags, CPoint /*point*/) 
{
  if ( nFlags & MK_CONTROL ) 
  {
    // Button MB2 down Control : panning init  
    // SetCursor(AfxGetApp()->LoadStandardCursor());   
  }
}

void OCC_3dView::OnMButtonUp(UINT nFlags, CPoint /*point*/) 
{
  if ( nFlags & MK_CONTROL ) 
  {
    // Button MB2 down Control : panning init  
    // SetCursor(AfxGetApp()->LoadStandardCursor());   
  }
}

void OCC_3dView::OnRButtonDown(UINT nFlags, CPoint point) 
{
  if ( nFlags & MK_CONTROL )
  {
    if (myHlrModeIsOn)
    {
      myView->SetComputedMode (Standard_False);
    }
    myView->StartRotation(point.x,point.y);  
  }
  else // if ( Ctrl )
  {
    GetDocument()->Popup(point.x,point.y,myView);
  }	
}

void OCC_3dView::OnRButtonUp(UINT /*nFlags*/, CPoint /*point*/) 
{
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    if (myHlrModeIsOn)
    {
      myView->SetComputedMode (myHlrModeIsOn);
      myView->Redraw();
    }
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void OCC_3dView::OnMouseMove(UINT nFlags, CPoint point) 
{
   //   ============================  LEFT BUTTON =======================
  if ( nFlags & MK_LBUTTON)
  {
    if ( nFlags & MK_CONTROL ) 
    {
      // move with MB1 and Control : on the dynamic zooming  
      // Do the zoom in function of mouse's coordinates  
      myView->Zoom (myXmax,myYmax,point.x,point.y); 
      // save the current mouse coordinate in min 
      myXmax = point.x; 
      myYmax = point.y;	
    }
    else // if ( Ctrl )
    {
      const Handle(AIS_InteractiveContext)& aContext = GetDocument()->GetAISContext();
      switch (myCurrentMode)
      {
      case CurAction3d_Nothing :
        myXmax = point.x;
        myYmax = point.y;

        if (nFlags & MK_SHIFT)
          GetDocument()->ShiftDragEvent(myXmax,myYmax,0,myView);
        else
          GetDocument()->DragEvent(myXmax,myYmax,0,myView);

        drawRectangle (myXmin, myYmin, myXmax, myYmax, aContext);

        break;
      case CurAction3d_DynamicZooming :
        myView->Zoom(myXmax,myYmax,point.x,point.y); 
        // save the current mouse coordinate in min \n";
        myXmax = point.x;
        myYmax = point.y;
        break;
      case CurAction3d_WindowZooming :
        myXmax = point.x;
        myYmax = point.y;
        drawRectangle (myXmin, myYmin, myXmax, myYmax, aContext);
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
    }// if ( nFlags & MK_CONTROL )  else 
  } 
  else if ( nFlags & MK_MBUTTON)
  {
    if ( nFlags & MK_CONTROL ) 
    {
      myView->Pan(point.x-myXmax,myYmax-point.y); // Realize the panning
      myXmax = point.x;
      myYmax = point.y;
    }
  } 
  else if ( nFlags & MK_RBUTTON)
  {
    if ( nFlags & MK_CONTROL ) 
    {
      myView->Rotation(point.x,point.y);
    }
  }
  else 
  {  // No buttons
    myXmax = point.x;
    myYmax = point.y;
    if (nFlags & MK_SHIFT)
      GetDocument()->ShiftMoveEvent(point.x,point.y,myView);
    else
      GetDocument()->MoveEvent(point.x,point.y,myView);
  }
}

void OCC_3dView::OnUpdateBUTTONHlrOff(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (!myHlrModeIsOn);
  pCmdUI->Enable   (myHlrModeIsOn);
}

void OCC_3dView::OnUpdateBUTTONHlrOn(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck (myHlrModeIsOn);
  pCmdUI->Enable   (!myHlrModeIsOn);
}

void OCC_3dView::OnUpdateBUTTONPanGlo(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurAction3d_GlobalPanning);
	pCmdUI->Enable   (myCurrentMode != CurAction3d_GlobalPanning);	
	
}

void OCC_3dView::OnUpdateBUTTONPan(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurAction3d_DynamicPanning);
	pCmdUI->Enable   (myCurrentMode != CurAction3d_DynamicPanning );	
}

void OCC_3dView::OnUpdateBUTTONZoomProg(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurAction3d_DynamicZooming );
	pCmdUI->Enable   (myCurrentMode != CurAction3d_DynamicZooming);	
}

void OCC_3dView::OnUpdateBUTTONZoomWin(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurAction3d_WindowZooming);
	pCmdUI->Enable   (myCurrentMode != CurAction3d_WindowZooming);	
}

void OCC_3dView::OnUpdateBUTTONRot(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck (myCurrentMode == CurAction3d_DynamicRotation);
	pCmdUI->Enable   (myCurrentMode != CurAction3d_DynamicRotation);	
}

void OCC_3dView::OnModifyChangeBackground() 
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

//=============================================================================
// function: OnStereoConfigButton
// purpose: Open stereographic configuration dialog
//=============================================================================
void OCC_3dView::OnStereoConfigButton()
{
  m_pStereoDlg->ShowWindow (SW_SHOW);
}

//=============================================================================
// function: OnUpdateStereoConfigButton
// purpose: Enable / disable state of stereo configuration button
//=============================================================================
void OCC_3dView::OnUpdateStereoConfigButton (CCmdUI* theCmdUI)
{
  // get camera
  Handle(Graphic3d_Camera) aCamera = myView->Camera();

  // check that button is enabled
  Standard_Boolean isEnabled = !aCamera.IsNull() && aCamera->IsStereo();

  // update toggle state
  theCmdUI->Enable (isEnabled);

  if (!isEnabled)
  {
    m_pStereoDlg->ShowWindow (SW_HIDE);
  }
}
