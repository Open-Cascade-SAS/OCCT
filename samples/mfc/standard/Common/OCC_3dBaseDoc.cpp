// OCC_3dBaseDoc.cpp: implementation of the OCC_3dBaseDoc class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#include "OCC_3dBaseDoc.h"

#include "OCC_3dView.h"
#include "OCC_App.h"
#include <res\OCC_Resource.h>
#include "ImportExport/ImportExport.h"
#include "AISDialogs.h"
#include <AIS_LocalContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopoDS_Shape.hxx>

BEGIN_MESSAGE_MAP(OCC_3dBaseDoc, OCC_BaseDoc)
  //{{AFX_MSG_MAP(OCC_3dBaseDoc)
  ON_COMMAND(ID_FILE_IMPORT_BREP, OnFileImportBrep)
  ON_COMMAND(ID_FILE_EXPORT_BREP, OnFileExportBrep)
  ON_COMMAND(ID_OBJECT_ERASE, OnObjectErase)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_ERASE, OnUpdateObjectErase)
  ON_COMMAND(ID_OBJECT_COLOR, OnObjectColor)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_COLOR, OnUpdateObjectColor)
  ON_COMMAND(ID_OBJECT_SHADING, OnObjectShading)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_SHADING, OnUpdateObjectShading)
  ON_COMMAND(ID_OBJECT_WIREFRAME, OnObjectWireframe)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_WIREFRAME, OnUpdateObjectWireframe)
  ON_COMMAND(ID_OBJECT_TRANSPARENCY, OnObjectTransparency)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_TRANSPARENCY, OnUpdateObjectTransparency)
  ON_COMMAND(ID_OBJECT_MATERIAL, OnObjectMaterial)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_MATERIAL, OnUpdateObjectMaterial)
  ON_COMMAND(ID_OBJECT_DISPLAYALL, OnObjectDisplayall)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_DISPLAYALL, OnUpdateObjectDisplayall)
  ON_COMMAND(ID_OBJECT_REMOVE, OnObjectRemove)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_REMOVE, OnUpdateObjectRemove)

  //}}AFX_MSG_MAP
  ON_COMMAND_EX_RANGE(ID_OBJECT_MATERIAL_BRASS,ID_OBJECT_MATERIAL_DEFAULT, OnObjectMaterialRange)
  ON_UPDATE_COMMAND_UI_RANGE(ID_OBJECT_MATERIAL_BRASS,ID_OBJECT_MATERIAL_DEFAULT, OnUpdateObjectMaterialRange)

  //RayTracing
  ON_COMMAND(ID_OBJECT_RAY_TRACING,OnObjectRayTracing)
  ON_COMMAND(ID_OBJECT_SHADOWS,OnObjectShadows)
  ON_COMMAND(ID_OBJECT_REFLECTIONS,OnObjectReflections)
  ON_COMMAND(ID_OBJECT_ANTI_ALIASING,OnObjectAntiAliasing)

  ON_UPDATE_COMMAND_UI(ID_OBJECT_RAY_TRACING, OnUpdateV3dButtons)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_SHADOWS, OnUpdateV3dButtons)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_REFLECTIONS, OnUpdateV3dButtons)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_ANTI_ALIASING, OnUpdateV3dButtons)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OCC_3dBaseDoc::OCC_3dBaseDoc()
:myPopupMenuNumber(0)
{
  AfxInitRichEdit();

  Handle(Graphic3d_GraphicDriver) aGraphicDriver = ((OCC_App*)AfxGetApp())->GetGraphicDriver();

  myViewer = new V3d_Viewer (aGraphicDriver);
  myViewer->SetDefaultLights();
  myViewer->SetLightOn();
  myAISContext = new AIS_InteractiveContext (myViewer);

  myRayTracingIsOn            = false;
  myRaytracedShadowsIsOn      = true;
  myRaytracedReflectionsIsOn  = false;
  myRaytracedAntialiasingIsOn = false;
}

OCC_3dBaseDoc::~OCC_3dBaseDoc()
{
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void OCC_3dBaseDoc::DragEvent (const Standard_Integer theMouseX,
                               const Standard_Integer theMouseY,
                               const Standard_Integer theState,
                               const Handle(V3d_View)& theView)
{
  // TheState == -1  button down
  // TheState ==  0  move
  // TheState ==  1  button up

  static Standard_Integer aStartDragX = 0;
  static Standard_Integer aStartDragY = 0;

  switch (theState)
  {
  case -1:
    {
      aStartDragX = theMouseX;
      aStartDragY = theMouseY;
      break;
    }
  case 0:
    {
      myAISContext->UpdateCurrentViewer();
      break;
    }
  case 1:
    {
      myAISContext->Select (aStartDragX, aStartDragY,
                            theMouseX, theMouseY,
                            theView, Standard_True);
      break;
    }
  };
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void OCC_3dBaseDoc::InputEvent (const Standard_Integer theMouseX,
                                const Standard_Integer theMouseY,
                                const Handle(V3d_View)& theView)
{
  myAISContext->MoveTo (theMouseX, theMouseY, theView, Standard_False);
  myAISContext->Select (Standard_True);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void OCC_3dBaseDoc::MoveEvent (const Standard_Integer theMouseX,
                               const Standard_Integer theMouseY,
                               const Handle(V3d_View)& theView)
{
  myAISContext->MoveTo (theMouseX, theMouseY, theView, Standard_True);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void OCC_3dBaseDoc::ShiftMoveEvent (const Standard_Integer theMouseX,
                                    const Standard_Integer theMouseY,
                                    const Handle(V3d_View)& theView)
{
  myAISContext->MoveTo (theMouseX, theMouseY, theView, Standard_True);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void OCC_3dBaseDoc::ShiftDragEvent (const Standard_Integer theMouseX,
                                    const Standard_Integer theMouseY,
                                    const Standard_Integer theState,
                                    const Handle(V3d_View)& theView)
{
  // TheState == -1  button down
  // TheState ==  0  move
  // TheState ==  1  button up

  static Standard_Integer aStartDragX = 0;
  static Standard_Integer aStartDragY = 0;

  if (theState == -1)
  {
    // button down
    aStartDragX = theMouseX;
    aStartDragY = theMouseY;
  }

  if (theState == 0)
  {
    // button up
    myAISContext->ShiftSelect (aStartDragX, aStartDragY,
                               theMouseX, theMouseY,
                               theView, Standard_True);
  }
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void OCC_3dBaseDoc::ShiftInputEvent (const Standard_Integer /*theMouseX*/,
                                     const Standard_Integer /*theMouseY*/,
                                     const Handle(V3d_View)& /*theView*/)
{
  myAISContext->ShiftSelect (Standard_True);
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void  OCC_3dBaseDoc::Popup (const Standard_Integer theMouseX,
                            const Standard_Integer theMouseY,
                            const Handle(V3d_View)& theView)
{
  // Base check which context menu to call
  if (!myPopupMenuNumber)
  {
    myAISContext->InitSelected();
    if (myAISContext->MoreSelected())
      myPopupMenuNumber=1;
  }

  CMenu menu;
  VERIFY(menu.LoadMenu(IDR_Popup3D));
  CMenu* pPopup = menu.GetSubMenu(myPopupMenuNumber);

  ASSERT(pPopup != NULL);
   if (myPopupMenuNumber == 1) // more than 1 object.
  {
    bool OneOrMoreInShading = false;
	for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),1)) OneOrMoreInShading=true;
	if(!OneOrMoreInShading)
   	pPopup->EnableMenuItem(5, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
   }

  POINT winCoord = { theMouseX , theMouseY };
  Handle(WNT_Window) aWNTWindow=
  Handle(WNT_Window)::DownCast(theView->Window());
  ClientToScreen ( (HWND)(aWNTWindow->HWindow()),&winCoord);
  pPopup->TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                          winCoord.x,
                          winCoord.y,
                          AfxGetMainWnd());
}

void OCC_3dBaseDoc::Fit()
{
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	OCC_3dView *pView = (OCC_3dView *) pChild->GetActiveView();
	pView->FitAll();
}

int OCC_3dBaseDoc::OnFileImportBrep_WithInitDir (const wchar_t* )
{
  if (CImportExport::ReadBREP (myAISContext) == 1)
    return 1;
  Fit();
  return 0;
}

void OCC_3dBaseDoc::OnFileImportBrep() 
{   
  OnFileImportBrep_WithInitDir (NULL);
}

void OCC_3dBaseDoc::OnFileExportBrep() 
{   CImportExport::SaveBREP(myAISContext);}

void OCC_3dBaseDoc::OnObjectColor() 
{
	Handle(AIS_InteractiveObject) Current ;
	COLORREF MSColor ;

	myAISContext->InitSelected();
    Current = myAISContext->SelectedInteractive();
	if ( Current->HasColor () ) {
      Quantity_Color CSFColor;
      myAISContext->Color (Current, CSFColor);
      MSColor = RGB (CSFColor.Red()*255.,CSFColor.Green()*255.,CSFColor.Blue()*255.);
	}
	else {
	  MSColor = RGB (255,255,255) ;
	}
	
	CColorDialog dlgColor(MSColor);
	if (dlgColor.DoModal() == IDOK)
	{
	  MSColor = dlgColor.GetColor();
	  Quantity_Color CSFColor (GetRValue(MSColor)/255., GetGValue(MSColor)/255., GetBValue(MSColor)/255., Quantity_TOC_RGB);
	  for (;myAISContext->MoreSelected ();myAISContext->NextSelected ())
      myAISContext->SetColor (myAISContext->SelectedInteractive(), CSFColor, Standard_False);
    myAISContext->UpdateCurrentViewer();
	}
}
void OCC_3dBaseDoc::OnUpdateObjectColor(CCmdUI* pCmdUI) 
{
  bool OneOrMoreIsShadingOrWireframe = false;
  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),0)
		||myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),1))
		OneOrMoreIsShadingOrWireframe=true;
  pCmdUI->Enable (OneOrMoreIsShadingOrWireframe);
}

void OCC_3dBaseDoc::OnObjectErase() 
{
  myAISContext->EraseSelected (Standard_False);
  myAISContext->ClearSelected (Standard_True);
}
void OCC_3dBaseDoc::OnUpdateObjectErase(CCmdUI* pCmdUI) 
{
  bool OneOrMoreIsDisplayed = false;
  for (myAISContext->InitSelected(); myAISContext->MoreSelected(); myAISContext->NextSelected())
  {
    if (myAISContext->IsDisplayed (myAISContext->SelectedInteractive()))
      OneOrMoreIsDisplayed = true;
  }
  pCmdUI->Enable (OneOrMoreIsDisplayed);
}

void OCC_3dBaseDoc::OnObjectWireframe() 
{
  for(myAISContext->InitSelected();myAISContext->MoreSelected();myAISContext->NextSelected())
        myAISContext->SetDisplayMode (myAISContext->SelectedInteractive(), 0, Standard_False);
  myAISContext->UpdateCurrentViewer();
}
void OCC_3dBaseDoc::OnUpdateObjectWireframe(CCmdUI* pCmdUI) 
{
  bool OneOrMoreInShading = false;
  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),1)) OneOrMoreInShading=true;
	pCmdUI->Enable (OneOrMoreInShading);	
}

void OCC_3dBaseDoc::OnObjectShading() 
{
  for(myAISContext->InitSelected();myAISContext->MoreSelected();myAISContext->NextSelected())
      myAISContext->SetDisplayMode (myAISContext->SelectedInteractive(), 1, Standard_False);
  myAISContext->UpdateCurrentViewer();
}

void OCC_3dBaseDoc::OnUpdateObjectShading(CCmdUI* pCmdUI) 
{
  bool OneOrMoreInWireframe = false;
  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),0)) OneOrMoreInWireframe=true;
	pCmdUI->Enable (OneOrMoreInWireframe);	
}

void OCC_3dBaseDoc::OnObjectMaterial() 
{
  CDialogMaterial DialBox(myAISContext);
  DialBox.DoModal();
  //CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
  //CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
  //OCC_3dView *pView = (OCC_3dView *) pChild->GetActiveView();
}

void OCC_3dBaseDoc::OnUpdateObjectMaterial(CCmdUI* pCmdUI) 
{
  bool OneOrMoreInShading = false;
  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),1)) OneOrMoreInShading=true;
	pCmdUI->Enable (OneOrMoreInShading);	
}

BOOL OCC_3dBaseDoc::OnObjectMaterialRange(UINT nID) 
{
  // the range ID_OBJECT_MATERIAL_BRASS to ID_OBJECT_MATERIAL_SILVER is
  // continue with the same values as enumeration Type Of Material
  Standard_Real aTransparency;

  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ()){
    aTransparency = myAISContext->SelectedInteractive()->Transparency();
    myAISContext->SetMaterial (myAISContext->SelectedInteractive(),(Graphic3d_NameOfMaterial)(nID-ID_OBJECT_MATERIAL_BRASS), Standard_False);
    myAISContext->SetTransparency (myAISContext->SelectedInteractive(),aTransparency, Standard_False);
  }
  myAISContext->UpdateCurrentViewer();
  return true;

}

void OCC_3dBaseDoc::OnUpdateObjectMaterialRange(CCmdUI* pCmdUI) 
{
  bool OneOrMoreInShading = false;
  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),1)) OneOrMoreInShading=true;
	pCmdUI->Enable (OneOrMoreInShading);
  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->SelectedInteractive()->Material() - (pCmdUI->m_nID - ID_OBJECT_MATERIAL_BRASS) == 0)
		pCmdUI->SetCheck(1);	
}


void OCC_3dBaseDoc::OnObjectTransparency()
{
	CDialogTransparency DialBox(myAISContext);
	DialBox.DoModal();
	//CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	//CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	//OCC_3dView *pView = (OCC_3dView *) pChild->GetActiveView();
//	pView->Redraw();
}

void OCC_3dBaseDoc::OnUpdateObjectTransparency(CCmdUI* pCmdUI) 
{
  bool OneOrMoreInShading = false;
  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),1)) OneOrMoreInShading=true;
	pCmdUI->Enable (OneOrMoreInShading);	
}


void OCC_3dBaseDoc::OnObjectDisplayall() 
{
	myAISContext->DisplayAll (Standard_True);
}

void OCC_3dBaseDoc::OnUpdateObjectDisplayall(CCmdUI* pCmdUI) 
{
	
	AIS_ListOfInteractive aList;
	myAISContext->ObjectsInside(aList,AIS_KOI_Shape);
	AIS_ListIteratorOfListOfInteractive aLI;
	Standard_Boolean IS_ANY_OBJECT_ERASED=FALSE;
	for (aLI.Initialize(aList);aLI.More();aLI.Next()){
		if(!myAISContext->IsDisplayed(aLI.Value()))
		IS_ANY_OBJECT_ERASED=TRUE;
	}
	pCmdUI->Enable (IS_ANY_OBJECT_ERASED);

}

void OCC_3dBaseDoc::OnObjectRemove() 
{
	for(myAISContext->InitSelected();myAISContext->MoreSelected();myAISContext->InitSelected())
        myAISContext->Remove(myAISContext->SelectedInteractive(),Standard_True);
}

void OCC_3dBaseDoc::OnUpdateObjectRemove(CCmdUI* pCmdUI) 
{
  bool OneOrMoreIsDisplayed = false;
  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive())) OneOrMoreIsDisplayed=true;
  pCmdUI->Enable (OneOrMoreIsDisplayed);
}

void OCC_3dBaseDoc::SetMaterial(Graphic3d_NameOfMaterial Material) 
{
  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    myAISContext->SetMaterial (myAISContext->SelectedInteractive(),
    (Graphic3d_NameOfMaterial)(Material), Standard_False);
  myAISContext->UpdateCurrentViewer();
}


// RayTracing
void OCC_3dBaseDoc::OnObjectRayTracing()
{
  myRayTracingIsOn = !myRayTracingIsOn;
  if(!myRayTracingIsOn)
  {
    myRaytracedShadowsIsOn = false;
    myRaytracedReflectionsIsOn = false;
    myRaytracedAntialiasingIsOn = false;
  }
  OnObjectRayTracingAction();
}
// Shadows
void OCC_3dBaseDoc::OnObjectShadows()
{
  myRaytracedShadowsIsOn = !myRaytracedShadowsIsOn;
  OnObjectRayTracingAction();
}
// Reflections
void OCC_3dBaseDoc::OnObjectReflections()
{
  myRaytracedReflectionsIsOn = !myRaytracedReflectionsIsOn;
  OnObjectRayTracingAction();
}
// Anti-aliasing
void OCC_3dBaseDoc::OnObjectAntiAliasing()
{
  myRaytracedAntialiasingIsOn = !myRaytracedAntialiasingIsOn;
  OnObjectRayTracingAction();
}
void OCC_3dBaseDoc::OnUpdateV3dButtons (CCmdUI* pCmdUI)
{
  if (pCmdUI->m_nID == ID_OBJECT_RAY_TRACING)
  {
    pCmdUI->SetCheck(myRayTracingIsOn);
  } else {
    pCmdUI->Enable(myRayTracingIsOn);
    if (pCmdUI->m_nID == ID_OBJECT_SHADOWS)
      pCmdUI->SetCheck(myRaytracedShadowsIsOn);
    if (pCmdUI->m_nID == ID_OBJECT_REFLECTIONS)
      pCmdUI->SetCheck(myRaytracedReflectionsIsOn);
    if (pCmdUI->m_nID == ID_OBJECT_ANTI_ALIASING)
      pCmdUI->SetCheck(myRaytracedAntialiasingIsOn);
  }
}
// Common function to change raytracing params and redraw view
void OCC_3dBaseDoc::OnObjectRayTracingAction()
{
  myAISContext->CurrentViewer()->InitActiveViews();
  Handle(V3d_View) aView = myAISContext->CurrentViewer()->ActiveView();
  Graphic3d_RenderingParams& aParams = aView->ChangeRenderingParams();
  if (myRayTracingIsOn)
    aParams.Method = Graphic3d_RM_RAYTRACING;
  else
    aParams.Method = Graphic3d_RM_RASTERIZATION;
  aParams.IsShadowEnabled = myRaytracedShadowsIsOn;
  aParams.IsReflectionEnabled = myRaytracedReflectionsIsOn;
  aParams.IsAntialiasingEnabled = myRaytracedAntialiasingIsOn;
  myAISContext->UpdateCurrentViewer();
}
