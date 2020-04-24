// OCC_3dView.cpp: implementation of the OCC_3dView class.
//

#include "stdafx.h"

#include "OCC_3dView.h"
#include "OCC_App.h"
#include "OCC_3dBaseDoc.h"
#include <res\OCC_Resource.h>

#include <Graphic3d_Camera.hxx>

#include <OpenGl_GraphicDriver.hxx>

IMPLEMENT_DYNCREATE(OCC_3dView, OCC_BaseView)

BEGIN_MESSAGE_MAP(OCC_3dView, OCC_BaseView)
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
{
  // TODO: add construction code here
}

OCC_3dView::~OCC_3dView()
{
  delete m_pStereoDlg;
}

/////////////////////////////////////////////////////////////////////////////
// OCC_3dView drawing
void OCC_3dView::OnInitialUpdate() 
{
  OCC_BaseView::OnInitialUpdate();
  m_pStereoDlg = new OCC_StereoConfigDlg (this);
  m_pStereoDlg->SetView (myView);
  m_pStereoDlg->Create (IDD_DIALOG_STEREO, this);
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

// See the back View
void OCC_3dView::OnBUTTONBack() 
{
  myView->SetProj(V3d_Ypos);
  myView->Redraw();
} 

// See the front View
void OCC_3dView::OnBUTTONFront() 
{
  myView->SetProj(V3d_Yneg);
  myView->Redraw();
} 

// See the bottom View
void OCC_3dView::OnBUTTONBottom() 
{
  myView->SetProj(V3d_Zneg);
  myView->Redraw();
}

// See the top View
void OCC_3dView::OnBUTTONTop() 
{
  myView->SetProj(V3d_Zpos);
  myView->Redraw();
} 	

// See the left View
void OCC_3dView::OnBUTTONLeft() 
{
  myView->SetProj(V3d_Xneg);
  myView->Redraw();
}

// See the right View
void OCC_3dView::OnBUTTONRight() 
{
  myView->SetProj(V3d_Xpos);
  myView->Redraw();
} 

// See the axonometric View
void OCC_3dView::OnBUTTONAxo() 
{
  myView->SetProj(V3d_XposYnegZpos);
  myView->Redraw();
} 

void OCC_3dView::OnBUTTONHlrOff() 
{
  myView->SetComputedMode (Standard_False);
  myView->Redraw();
}

void OCC_3dView::OnBUTTONHlrOn() 
{
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
  myView->SetComputedMode (Standard_True);
  myView->Redraw();
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void OCC_3dView::OnBUTTONPan() 
{
  setCurrentAction (CurAction3d_DynamicPanning);
}

void OCC_3dView::OnBUTTONPanGlo() 
{
  // save the current zoom value 
  myCurZoom = myView->Scale();
  // Do a Global Zoom 
  myView->FitAll();
  // Set the mode 
  setCurrentAction (CurAction3d_GlobalPanning);
}

void OCC_3dView::OnBUTTONReset() 
{
  myView->Reset();
}

void OCC_3dView::OnBUTTONRot() 
{
  setCurrentAction (CurAction3d_DynamicRotation);
}

void OCC_3dView::OnBUTTONZoomAll() 
{
  myView->FitAll();
  myView->ZFitAll();
}

void OCC_3dView::OnBUTTONZoomProg() 
{
  setCurrentAction (CurAction3d_DynamicZooming);
}

void OCC_3dView::OnBUTTONZoomWin() 
{
  setCurrentAction (CurAction3d_WindowZooming);
}

void OCC_3dView::OnUpdateBUTTONHlrOff(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (!myView->ComputedMode());
  pCmdUI->Enable   (myView->ComputedMode());
}

void OCC_3dView::OnUpdateBUTTONHlrOn(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck (myView->ComputedMode());
  pCmdUI->Enable   (!myView->ComputedMode());
}

void OCC_3dView::OnUpdateBUTTONPanGlo(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (getCurrentAction() == CurAction3d_GlobalPanning);
  pCmdUI->Enable   (getCurrentAction() != CurAction3d_GlobalPanning);	
}

void OCC_3dView::OnUpdateBUTTONPan(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (getCurrentAction() == CurAction3d_DynamicPanning);
  pCmdUI->Enable   (getCurrentAction() != CurAction3d_DynamicPanning );	
}

void OCC_3dView::OnUpdateBUTTONZoomProg(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (getCurrentAction() == CurAction3d_DynamicZooming );
  pCmdUI->Enable   (getCurrentAction() != CurAction3d_DynamicZooming);	
}

void OCC_3dView::OnUpdateBUTTONZoomWin(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (getCurrentAction() == CurAction3d_WindowZooming);
  pCmdUI->Enable   (getCurrentAction() != CurAction3d_WindowZooming);	
}

void OCC_3dView::OnUpdateBUTTONRot(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck (getCurrentAction() == CurAction3d_DynamicRotation);
  pCmdUI->Enable   (getCurrentAction() != CurAction3d_DynamicRotation);	
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
