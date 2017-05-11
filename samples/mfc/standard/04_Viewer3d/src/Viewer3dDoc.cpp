// Viewer3dDoc.cpp : implementation of the CViewer3dDoc class
//

#include "stdafx.h"
#include "Viewer3dApp.h"
#include "BoxRadius.h"
#include "DlgIsos.h"

#include "Viewer3dDoc.h"
#include "Viewer3dView.h"
#include "OffsetDlg.h"
#include "ResultDialog.h"
#include "User_Cylinder.hxx"

#include "AIS_Trihedron.hxx"
#include "Geom_Axis2Placement.hxx"

/////////////////////////////////////////////////////////////////////////////
// CViewer3dDoc

IMPLEMENT_DYNCREATE(CViewer3dDoc, CDocument)

BEGIN_MESSAGE_MAP(CViewer3dDoc, OCC_3dDoc)
  //{{AFX_MSG_MAP(CViewer3dDoc)
  ON_COMMAND(ID_BOX, OnBox)
  ON_COMMAND(ID_Cylinder, OnCylinder)
  ON_COMMAND(ID_SPHERE, OnSphere)
  ON_COMMAND(ID_ERASEALL, OnRemoveAll)
  ON_COMMAND(ID_NBISOS, OnNbisos)
  ON_COMMAND(ID_FACES, OnFaces)
  ON_COMMAND(ID_EDGES, OnEdges)
  ON_COMMAND(ID_VERTICES, OnVertices)
  ON_COMMAND(ID_NEUTRAL, OnNeutral)
  ON_COMMAND(ID_USERCYLINDER_CHANGEFACECOLOR, OnUsercylinderChangefacecolor)
  ON_COMMAND(ID_FILLET3D, OnFillet3d)
  ON_COMMAND(ID_CIRCLE, OnCircle)
  ON_COMMAND(ID_LINE, OnLine)
  ON_COMMAND(ID_OVERLAPPED_BOX, OnOverlappedBox)
  ON_COMMAND(ID_OVERLAPPED_CYLINDER, OnOverlappedCylinder)
  ON_COMMAND(ID_OVERLAPPED_SPHERE, OnOverlappedSphere)
  ON_COMMAND(ID_POLYGON_OFFSETS, OnPolygonOffsets)
  ON_UPDATE_COMMAND_UI(ID_POLYGON_OFFSETS, OnUpdatePolygonOffsets)
  ON_UPDATE_COMMAND_UI(ID_Cylinder, OnUpdateCylinder)
  ON_UPDATE_COMMAND_UI(ID_SPHERE, OnUpdateSphere)
  ON_UPDATE_COMMAND_UI(ID_BOX, OnUpdateBox)
  ON_UPDATE_COMMAND_UI(ID_OVERLAPPED_CYLINDER, OnUpdateOverlappedCylinder)
  ON_UPDATE_COMMAND_UI(ID_OVERLAPPED_SPHERE, OnUpdateOverlappedSphere)
  ON_UPDATE_COMMAND_UI(ID_OVERLAPPED_BOX, OnUpdateOverlappedBox)
  ON_COMMAND(ID_OBJECT_REMOVE, OnObjectRemove)
  ON_COMMAND(ID_OBJECT_ERASE, OnObjectErase)
  ON_COMMAND(ID_OBJECT_DISPLAYALL, OnObjectDisplayall)
  ON_COMMAND(ID_OBJECT_COLORED_MESH, OnObjectColoredMesh)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_COLORED_MESH, OnUpdateObjectColoredMesh)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_SHADING, OnUpdateObjectShading)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_WIREFRAME, OnUpdateObjectWireframe)
  ON_COMMAND(ID_OPTIONS_TRIHEDRON_DYNAMIC_TRIHEDRON, OnOptionsTrihedronDynamicTrihedron)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_TRIHEDRON_DYNAMIC_TRIHEDRON, OnUpdateOptionsTrihedronDynamicTrihedron)
  ON_UPDATE_COMMAND_UI(ID_OPTIONS_TRIHEDRON_STATIC_TRIHEDRON, OnUpdateOptionsTrihedronStaticTrihedron)
  ON_COMMAND(ID_OBJECT_MATERIAL, OnObjectMaterial)
  ON_COMMAND(ID_TEXTURE_ON, OnTextureOn)
  ON_COMMAND(ID_BUTTONNext, OnBUTTONNext)
  ON_COMMAND(ID_BUTTONStart, OnBUTTONStart)
  ON_COMMAND(ID_BUTTONRepeat, OnBUTTONRepeat)
  ON_COMMAND(ID_BUTTONPrev, OnBUTTONPrev)
  ON_COMMAND(ID_BUTTONEnd, OnBUTTONEnd)
  ON_UPDATE_COMMAND_UI(ID_BUTTONNext, OnUpdateBUTTONNext)
  ON_UPDATE_COMMAND_UI(ID_BUTTONPrev, OnUpdateBUTTONPrev)
  ON_UPDATE_COMMAND_UI(ID_BUTTONStart, OnUpdateBUTTONStart)
  ON_UPDATE_COMMAND_UI(ID_BUTTONRepeat, OnUpdateBUTTONRepeat)
  ON_UPDATE_COMMAND_UI(ID_BUTTONEnd, OnUpdateBUTTONEnd)
  ON_COMMAND(ID_FILE_NEW, OnFileNew)
  ON_COMMAND(ID_DUMP_VIEW, OnDumpView)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewer3dDoc construction/destruction

CViewer3dDoc::CViewer3dDoc()
:OCC_3dDoc()
{
  myCylinder.Nullify();
  mySphere.Nullify();
  myBox.Nullify();
  myOverlappedCylinder.Nullify();
  myOverlappedSphere.Nullify();
  myOverlappedBox.Nullify();
  myOffsetDlg = NULL;
  myStaticTrihedronAxisIsDisplayed = FALSE;
  myState = -1;

  isTextureSampleStarted = FALSE;

  myPresentation = OCCDemo_Presentation::Current;
  myPresentation->SetDocument(this);


  myAISContext->DefaultDrawer()->ShadingAspect()->SetColor(Quantity_NOC_CHARTREUSE1); 
  myAISContext->DefaultDrawer()->ShadingAspect()->SetMaterial(Graphic3d_NOM_SILVER);

  strcpy_s(myDataDir, "Data");
  strcpy_s(myLastPath, ".");

  /*
  Handle(AIS_Trihedron) myTrihedron;
  Handle(Geom_Axis2Placement) myTrihedronAxis=new Geom_Axis2Placement(gp::XOY());
  myTrihedron=new AIS_Trihedron(myTrihedronAxis);
  myAISContext->Display(myTrihedron);
  */
}

CViewer3dDoc::~CViewer3dDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CViewer3dDoc diagnostics

#ifdef _DEBUG
void CViewer3dDoc::AssertValid() const
{
  CDocument::AssertValid();
}

void CViewer3dDoc::Dump(CDumpContext& dc) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewer3dDoc commands

void CViewer3dDoc::UpdateResultMessageDlg (CString theTitle, const TCollection_AsciiString& theMessage)
{
  CString aText (theMessage.ToCString());
  myCResultDialog.SetText (aText);
  myCResultDialog.SetTitle(theTitle);
}

void CViewer3dDoc::UpdateResultMessageDlg(CString theTitle, CString theMessage)
{
  myCResultDialog.SetText (theMessage);
  myCResultDialog.SetTitle(theTitle);
}

void CViewer3dDoc::OnBox()
{
  if(myBox.IsNull())
  {
    BRepPrimAPI_MakeBox B(gp_Pnt(0,-400,-100), 200.,150.,100.);


    myBox = new AIS_Shape(B.Shape());

    myAISContext->SetMaterial (myBox, Graphic3d_NOM_PEWTER, Standard_False);
    myAISContext->SetDisplayMode (myBox, 1, Standard_False);

    myAISContext->Display (myBox, Standard_True);
    TCollection_AsciiString Message("\
BRepPrimAPI_MakeBox Box1(gp_Pnt(0,-400,-100), 200.,150.,100.);\n\
    ");

    UpdateResultMessageDlg("Create Box",Message);
  }
}

void CViewer3dDoc::OnCylinder()
{
  if(myCylinder.IsNull())
  {
    gp_Ax2 CylAx2(gp_Pnt(0,0,-100), gp_Dir(gp_Vec(gp_Pnt(0,0,-100),gp_Pnt(0,0,100))));
    myCylinder = new User_Cylinder(CylAx2, 80.,200.);

    myAISContext->SetDisplayMode (myCylinder, 1, Standard_False);

    myAISContext->Display (myCylinder, Standard_True);

    TCollection_AsciiString Message("\
gp_Ax2 CylAx2(gp_Pnt(0,0,-100), gp_Dir(gp_Vec(gp_Pnt(0,0,-100),gp_Pnt(0,0,100))));\n\
C = new User_Cylinder(CylAx2, 80.,200.);;\n\
    ");

    UpdateResultMessageDlg("Create Cylinder",Message);
  }
}

void CViewer3dDoc::OnSphere()
{
  if(mySphere.IsNull())
  {
    BRepPrimAPI_MakeSphere S(gp_Pnt(0,300,0), 100.);

    mySphere = new AIS_Shape(S.Shape());

    myAISContext->SetMaterial (mySphere, Graphic3d_NOM_BRONZE, Standard_False);
    myAISContext->SetDisplayMode (mySphere, 1, Standard_False);

    myAISContext->Display (mySphere, Standard_False);
    TCollection_AsciiString Message("\
BRepPrimAPI_MakeSphere S(gp_Pnt(0,300,0), 100.);\n\
    ");
    UpdateResultMessageDlg("Create Sphere",Message);
  }
}

void CViewer3dDoc::OnRemoveAll()

{
  AIS_ListOfInteractive aListOfObjects;
  myAISContext->ObjectsInside(aListOfObjects,AIS_KOI_Shape);

  AIS_ListIteratorOfListOfInteractive aListIterator;
  for(aListIterator.Initialize(aListOfObjects);aListIterator.More();aListIterator.Next()){
    myAISContext->Remove (aListIterator.Value(), Standard_False);
  }

  myAISContext->Remove (myCylinder, Standard_True);


  myCylinder.Nullify();
  mySphere.Nullify();
  myBox.Nullify();
  myOverlappedCylinder.Nullify();
  myOverlappedSphere.Nullify();
  myOverlappedBox.Nullify();
  if(myOffsetDlg && myOffsetDlg->IsWindowVisible())
    myOffsetDlg->UpdateValues();
}

void CViewer3dDoc::OnOverlappedBox() 
{
  OnBox();
  if(myOverlappedBox.IsNull()){
    BRepPrimAPI_MakeBox B(gp_Pnt(0,-400,-100), 200.,150.,100.);

    BRepBuilderAPI_NurbsConvert aNurbsConvert(B.Shape());
    TopoDS_Shape aBoxShape2 = aNurbsConvert.Shape();


    myOverlappedBox = new AIS_Shape(aBoxShape2);

    myAISContext->SetMaterial (myOverlappedBox, Graphic3d_NOM_GOLD, Standard_False);
    myAISContext->SetDisplayMode (myOverlappedBox, 1, Standard_False);

    myAISContext->Display (myOverlappedBox, Standard_True);
    TCollection_AsciiString Message("\
BRepPrimAPI_MakeBox Box1(gp_Pnt(0,-400,-100), 200.,150.,100.);\n\
\n\
BRepPrimAPI_MakeBox Box2(gp_Pnt(0,-400,-100), 200.,150.,100.);\n\
BRepBuilderAPI_NurbsConvert aNurbsConvert(Box2.Shape());\n\
    ");
    UpdateResultMessageDlg("Create overlapped boxes",Message);
    if(myOffsetDlg && myOffsetDlg->IsWindowVisible())
      myOffsetDlg->UpdateValues();
  }
}

void CViewer3dDoc::OnOverlappedCylinder()
{
  OnCylinder();

  if(myOverlappedCylinder.IsNull()){
    gp_Ax2 CylAx2(gp_Pnt(0,0,-100), gp_Dir(gp_Vec(gp_Pnt(0,0,-100),gp_Pnt(0,0,100))));
    BRepPrimAPI_MakeCylinder C(CylAx2, 80.,200.);

    BRepBuilderAPI_NurbsConvert aNurbsConvert(C.Shape());
    TopoDS_Shape aCylShape2 = aNurbsConvert.Shape();

    myOverlappedCylinder = new AIS_Shape(aCylShape2);


    myAISContext->SetMaterial (myOverlappedCylinder, Graphic3d_NOM_GOLD, Standard_False);
    myAISContext->SetDisplayMode (myOverlappedCylinder, 1, Standard_False);

    myAISContext->Display (myOverlappedCylinder, Standard_True);

    TCollection_AsciiString Message("\
gp_Ax2 CylAx2(gp_Pnt(0,0,-100), gp_Dir(gp_Vec(gp_Pnt(0,0,-100),gp_Pnt(0,0,100))));\n\
Cylinder1 = new User_Cylinder(CylAx2, 80.,200.);\n\
\n\
BRepPrimAPI_MakeCylinder Cylinder2(CylAx2, 80.,200.);\n\
BRepBuilderAPI_NurbsConvert aNurbsConvert(Cylinder2.Shape());\n\
    ");
    UpdateResultMessageDlg("Create overlapped cylinders",Message);
    if (myOffsetDlg && myOffsetDlg->IsWindowVisible())
    {
      myOffsetDlg->UpdateValues();
    }
  }
}

void CViewer3dDoc::OnOverlappedSphere()
{
  OnSphere();
  if(myOverlappedSphere.IsNull()){
    BRepPrimAPI_MakeSphere S(gp_Pnt(0,300,0), 100.);

    BRepBuilderAPI_NurbsConvert aNurbsConvert(S.Shape());
    TopoDS_Shape aSphereShape2 = aNurbsConvert.Shape();

    myOverlappedSphere = new AIS_Shape(aSphereShape2);

    myAISContext->SetMaterial (myOverlappedSphere, Graphic3d_NOM_GOLD, Standard_False);

    myAISContext->SetDisplayMode (myOverlappedSphere, 1, Standard_False);

    myAISContext->Display (myOverlappedSphere, Standard_True);

    TCollection_AsciiString Message("\
BRepPrimAPI_MakeSphere Sphere1(gp_Pnt(0,300,0), 100.);\n\
\n\
BRepPrimAPI_MakeSphere Sphere2(gp_Pnt(0,300,0), 100.);\n\
BRepBuilderAPI_NurbsConvert aNurbsConvert(Sphere2.Shape());\n\
    ");
    UpdateResultMessageDlg("Create overlapped spheres",Message);
    if (myOffsetDlg && myOffsetDlg->IsWindowVisible())
    {
      myOffsetDlg->UpdateValues();
    }
  }
}

void CViewer3dDoc::OnPolygonOffsets()
{
  myOffsetDlg = new COffsetDlg(this);
  myOffsetDlg->Create(COffsetDlg::IDD,NULL);
  myAISContext->UpdateCurrentViewer();
}

void CViewer3dDoc::OnUpdatePolygonOffsets(CCmdUI* pCmdUI) 
{
  Standard_Integer aOffsetMode;
  Standard_ShortReal aFactor;
  Standard_ShortReal aCylUnits = 0;
  Standard_ShortReal aSphereUnits = 0;
  Standard_ShortReal aBoxUnits = 0;

  BOOL IsOverlappedCylinderDisplayed = myAISContext->IsDisplayed(myOverlappedCylinder);
  BOOL IsOverlappedSphereDisplayed = myAISContext->IsDisplayed(myOverlappedSphere);
  BOOL IsOverlappedBoxDisplayed = myAISContext->IsDisplayed(myOverlappedBox);
  BOOL IsAnyOverlappedObjectDisplayed = 
    IsOverlappedCylinderDisplayed || IsOverlappedSphereDisplayed || IsOverlappedBoxDisplayed;

  if(!myOverlappedCylinder.IsNull() && IsOverlappedCylinderDisplayed){
    myOverlappedCylinder->PolygonOffsets(aOffsetMode,aFactor,aCylUnits);
  }

  if(!myOverlappedSphere.IsNull() && IsOverlappedSphereDisplayed){
    myOverlappedSphere->PolygonOffsets(aOffsetMode,aFactor,aSphereUnits);
  }

  if(!myOverlappedBox.IsNull() && IsOverlappedBoxDisplayed){
    myOverlappedBox->PolygonOffsets(aOffsetMode,aFactor,aBoxUnits);
  }

  if(myOffsetDlg && myOffsetDlg->IsWindowVisible())
    pCmdUI->SetCheck(TRUE);
  else
    pCmdUI->SetCheck(FALSE);
  if (IsAnyOverlappedObjectDisplayed)
    if(myOffsetDlg)
      pCmdUI->Enable(!myOffsetDlg->IsWindowVisible());
    else
      pCmdUI->Enable(TRUE);
  else
    pCmdUI->Enable(FALSE);
}

void CViewer3dDoc::OnUpdateCylinder(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(myCylinder.IsNull());
}

void CViewer3dDoc::OnUpdateSphere(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(mySphere.IsNull()); 
}

void CViewer3dDoc::OnUpdateBox(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(myBox.IsNull()); 
}

void CViewer3dDoc::OnUpdateOverlappedCylinder(CCmdUI* pCmdUI)
{
  pCmdUI->Enable   (myOverlappedCylinder.IsNull()
    || myCylinder.IsNull());

}

void CViewer3dDoc::OnUpdateOverlappedSphere(CCmdUI* pCmdUI)
{
  pCmdUI->Enable   (myOverlappedSphere.IsNull()
    || mySphere.IsNull());
}

void CViewer3dDoc::OnUpdateOverlappedBox(CCmdUI* pCmdUI)
{
  pCmdUI->Enable   (myOverlappedBox.IsNull()
    || myBox.IsNull());
}

void CViewer3dDoc::OnObjectRemove()
{
  // A small trick to avoid complier error (C2668).
  const Handle(AIS_InteractiveObject)& aBox = myBox;
  if(myAISContext->IsSelected (aBox))
    myBox.Nullify();

  const Handle(AIS_InteractiveObject)& aCylinder = myCylinder;
  if(myAISContext->IsSelected (aCylinder))
    myCylinder.Nullify();

  const Handle(AIS_InteractiveObject)& aSphere = mySphere;
  if(myAISContext->IsSelected (aSphere))
    mySphere.Nullify();

  const Handle(AIS_InteractiveObject)& anOverlappedBox = myOverlappedBox;
  if(myAISContext->IsSelected (anOverlappedBox))
    myOverlappedBox.Nullify();

  const Handle(AIS_InteractiveObject)& anOverlappedCylinder = myOverlappedCylinder;
  if(myAISContext->IsSelected (anOverlappedCylinder))
    myOverlappedCylinder.Nullify();

  const Handle(AIS_InteractiveObject)& anOverlappedSphere = myOverlappedSphere;
  if(myAISContext->IsSelected (anOverlappedSphere))
    myOverlappedSphere.Nullify();


  for(myAISContext->InitSelected();myAISContext->MoreSelected();myAISContext->InitSelected())
    myAISContext->Remove(myAISContext->SelectedInteractive(),Standard_True);

  if(myOffsetDlg && myOffsetDlg->IsWindowVisible())
    myOffsetDlg->UpdateValues();
}

void CViewer3dDoc::OnObjectErase()

{
  OCC_3dBaseDoc::OnObjectErase();
  if(myOffsetDlg && myOffsetDlg->IsWindowVisible())
    myOffsetDlg->UpdateValues();
}

void CViewer3dDoc::OnObjectDisplayall()

{
  OCC_3dBaseDoc::OnObjectDisplayall();
  if(myOffsetDlg && myOffsetDlg->IsWindowVisible())
    myOffsetDlg->UpdateValues();
}

Handle(User_Cylinder) CViewer3dDoc::GetCylinder()
{
  return myCylinder;
}


Handle(AIS_Shape) CViewer3dDoc::GetSphere()
{
  return mySphere;
}

Handle(AIS_Shape) CViewer3dDoc::GetBox()
{
  return myBox;
}

Handle(AIS_Shape) CViewer3dDoc::GetOverlappedCylinder()
{
  return myOverlappedCylinder;
}


Handle(AIS_Shape) CViewer3dDoc::GetOverlappedSphere()
{
  return myOverlappedSphere;
}

Handle(AIS_Shape) CViewer3dDoc::GetOverlappedBox()
{
  return myOverlappedBox;
}

void CViewer3dDoc::DragEvent (const Standard_Integer theX,
                              const Standard_Integer theY,
                              const Standard_Integer theState,
                              const Handle(V3d_View)& theView)
{
  OCC_3dBaseDoc::DragEvent (theX, theY, theState, theView);
  if(myOffsetDlg && myOffsetDlg->IsWindowVisible())
    myOffsetDlg->UpdateValues();
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CViewer3dDoc::InputEvent(const Standard_Integer /*x*/,
                              const Standard_Integer /*y*/,
                              const Handle(V3d_View)& /*aView*/ )
{
  if (myOffsetDlg && myOffsetDlg->IsWindowVisible())
    myOffsetDlg->UpdateValues();

  Quantity_Color CSFColor;
  COLORREF MSColor;
  myAISContext->Select (Standard_True);

  // Change the color of a selected face in a user cylinder
  if (myState == FACE_COLOR)
  {
    myAISContext->InitSelected();
    if (myAISContext->MoreSelected())
    {
      Handle(AIS_InteractiveObject) Current = myAISContext->SelectedInteractive();
      if (Current->HasColor())
      {
        myAISContext->Color (Current, CSFColor);
        MSColor = RGB (CSFColor.Red()*255.0, CSFColor.Green()*255.0, CSFColor.Blue()*255.0);
      }
      else 
      {
        MSColor = RGB (255, 255, 255);
      }

      CColorDialog dlgColor(MSColor);
      if (dlgColor.DoModal() == IDOK)
      {
        MSColor = dlgColor.GetColor();
        CSFColor = Quantity_Color (GetRValue (MSColor)/255.0,
                                   GetGValue (MSColor)/255.0,
                                   GetBValue (MSColor)/255.0,
                                   Quantity_TOC_RGB);

        Handle(AIS_InteractiveObject) aSelectedObject = myAISContext->SelectedInteractive();
        Handle(User_Cylinder)::DownCast (aSelectedObject)->SetColor (CSFColor);

        myAISContext->Redisplay (aSelectedObject, Standard_True);
        myState = -1;
      }
    }

    myCResultDialog.SetTitle("Change face color");
    myCResultDialog.SetText(          "  Handle(AIS_InteractiveObject) aSelectedObject = myAISContext->SelectedInteractive(); \n"
                                      "  Handle(User_Cylinder)::DownCast(aSelectedObject)->SetColor (CSFColor); \n"
                                      "  myAISContext->Redisplay (myAISContext->Current(), Standard_True); \n"
                                      "  \n"
                                      "  \n"
                                      "  NOTE: a User_Cylinder is an object defined by the user. \n"
                                      "  The User_Cylinder class inherit from the AIS_InteractiveObject \n"
                                      "  Cascade class, it's use is the same as an AIS_InteractiveObject. \n"
                                      "  \n");
    SetTitle (L"Change face color");
  }
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CViewer3dDoc::ShiftDragEvent (const Standard_Integer theX,
                                   const Standard_Integer  theY,
                                   const Standard_Integer theState,
                                   const Handle(V3d_View)& theView)
{
  OCC_3dBaseDoc::ShiftDragEvent(theX, theY, theState, theView);
  if(myOffsetDlg && myOffsetDlg->IsWindowVisible())
    myOffsetDlg->UpdateValues();
}


//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void CViewer3dDoc::ShiftInputEvent (const Standard_Integer theX,
                                    const Standard_Integer theY,
                                    const Handle(V3d_View)& theView)
{
  OCC_3dBaseDoc::ShiftInputEvent (theX, theY, theView);
  if(myOffsetDlg && myOffsetDlg->IsWindowVisible())
    myOffsetDlg->UpdateValues();
}

void CViewer3dDoc::OnObjectColoredMesh()
{
  for(myAISContext->InitSelected();myAISContext->MoreSelected();myAISContext->NextSelected())
    if (myAISContext->SelectedInteractive()->IsKind(STANDARD_TYPE(User_Cylinder)))
    {
      myAISContext->ClearPrs(myAISContext->SelectedInteractive(),6,Standard_False);
      myAISContext->RecomputePrsOnly(myAISContext->SelectedInteractive(), Standard_False);
      myAISContext->SetDisplayMode(myAISContext->SelectedInteractive(), 6, Standard_False);
    }

  myAISContext->UpdateCurrentViewer();
}

void CViewer3dDoc::OnUpdateObjectColoredMesh(CCmdUI* pCmdUI)
{
  bool CylinderIsCurrentAndDisplayed = false;
  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if(myAISContext->SelectedInteractive()->IsKind(STANDARD_TYPE(User_Cylinder)))
      CylinderIsCurrentAndDisplayed=true;
  pCmdUI->Enable (CylinderIsCurrentAndDisplayed);
}

void CViewer3dDoc::OnUpdateObjectWireframe(CCmdUI* pCmdUI)
{
  bool OneOrMoreInShadingOrColoredMesh = false;
  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(), 1) || myAISContext->IsDisplayed(myAISContext->SelectedInteractive(), 6))
      OneOrMoreInShadingOrColoredMesh=true;
  pCmdUI->Enable (OneOrMoreInShadingOrColoredMesh);
}


void CViewer3dDoc::OnUpdateObjectShading(CCmdUI* pCmdUI)
{
  bool OneOrMoreInWireframeOrColoredMesh = false;
  for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),0) || myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),6))
      OneOrMoreInWireframeOrColoredMesh=true;
  pCmdUI->Enable (OneOrMoreInWireframeOrColoredMesh);
}

void CViewer3dDoc::OnOptionsTrihedronDynamicTrihedron()
{
  if (myAISContext -> IsDisplayed(myTrihedron))
  {
    myAISContext->Remove(myTrihedron, Standard_True);
  }
  else
  {
    Handle(Geom_Axis2Placement) myTrihedronAxis=new Geom_Axis2Placement(gp::XOY());
    myTrihedron=new AIS_Trihedron(myTrihedronAxis);
    myAISContext->SetTrihedronSize(200, Standard_False);
    myAISContext->Display(myTrihedron, Standard_True);
  }
}

void CViewer3dDoc::OnUpdateOptionsTrihedronDynamicTrihedron(CCmdUI* pCmdUI)
{
  if (myAISContext->IsDisplayed(myTrihedron))
    pCmdUI->SetCheck(1);
  else
    pCmdUI->SetCheck(0);
}

void CViewer3dDoc::SetMyStaticTrihedronAxisIsDisplayed(BOOL IsDisplayed)
{
  myStaticTrihedronAxisIsDisplayed = IsDisplayed;
}

void CViewer3dDoc::OnUpdateOptionsTrihedronStaticTrihedron(CCmdUI* pCmdUI) 
{
  if (myStaticTrihedronAxisIsDisplayed)
    pCmdUI->SetCheck(1);
  else
    pCmdUI->SetCheck(0);

}

void  CViewer3dDoc::Popup (const Standard_Integer  x,
                           const Standard_Integer  y ,
                           const Handle(V3d_View)& aView)
{
  myPopupMenuNumber=0;
  // Specified check for context menu number to call
  myAISContext->InitSelected();
  if (myAISContext->MoreSelected())
  {
    if (myAISContext->SelectedInteractive()->IsKind(STANDARD_TYPE(User_Cylinder)))
    {
      myPopupMenuNumber = 2;
      //return;
    }
  }
  OCC_3dBaseDoc::Popup(x,y, aView);
}

//Set faces selection mode
void CViewer3dDoc::OnFaces() 
{
  myAISContext->Activate (AIS_Shape::SelectionMode (TopAbs_FACE));

  myCResultDialog.SetTitle("Standard mode: TopAbs_FACE");
  myCResultDialog.SetText("  myAISContext->OpenLocalContext(); \n"
                                    "  \n"
                                    "  myAISContext->ActivateStandardMode(TopAbs_FACE); \n"
                                    "  \n");
  SetTitle (L"Standard mode: TopAbs_FACE");
}

//Set edges selection mode
void CViewer3dDoc::OnEdges() 
{
  myAISContext->Activate (AIS_Shape::SelectionMode (TopAbs_EDGE));

  myCResultDialog.SetTitle("Standard mode: TopAbs_EDGE");
  myCResultDialog.SetText("  myAISContext->OpenLocalContext(); \n"
                                    "  \n"
                                    "  myAISContext->ActivateStandardMode(TopAbs_EDGE); \n"
                                    "  \n");
  SetTitle (L"Standard mode: TopAbs_EDGE");
}

// Set vertices selection mode
void CViewer3dDoc::OnVertices() 
{
  myAISContext->Activate (AIS_Shape::SelectionMode (TopAbs_VERTEX));

  myCResultDialog.SetTitle("Standard mode: TopAbs_VERTEX");
  myCResultDialog.SetText("  myAISContext->OpenLocalContext(); \n"
                                    "  \n"
                                    "  myAISContext->ActivateStandardMode(TopAbs_VERTEX); \n"
                                    "  \n");
  SetTitle (L"Standard mode: TopAbs_VERTEX");
}

//Neutral selection mode
void CViewer3dDoc::OnNeutral() 
{
  myCResultDialog.SetTitle("Standard mode: Neutral");
  myCResultDialog.SetText("  myAISContext->CloseAllContexts(); \n"
                                    "  \n");
  SetTitle (L"Standard mode: Neutral");
}

// Change the color of faces on a user cylinder
void CViewer3dDoc::OnUsercylinderChangefacecolor() 
{
  myAISContext->Activate(myAISContext->SelectedInteractive(), 4);
  myState = FACE_COLOR;
  // see the following of treatment in inputevent
}

// Make 3d fillets on solids
// It is necessary to activate the edges selection mode and select edges on an object
// before running this function
void CViewer3dDoc::OnFillet3d() 
{
  myAISContext->InitSelected();
  if (myAISContext->MoreSelected()) 
  {
    Handle(AIS_Shape) aSelInteractive (Handle(AIS_Shape)::DownCast (myAISContext->SelectedInteractive()));
    if (aSelInteractive.IsNull())
    {
      AfxMessageBox (L"It is necessary to activate the edges selection mode\n"
                     L"and select edges on an object before \nrunning this function");
      return;
    }

    BRepFilletAPI_MakeFillet aFillet (aSelInteractive->Shape());

    for (myAISContext->InitSelected(); myAISContext->MoreSelected(); myAISContext->NextSelected())
    {
      const TopoDS_Shape& aSelShape = myAISContext->SelectedShape();
      if (aSelShape.ShapeType() != TopAbs_EDGE)
      {
        AfxMessageBox (L"It is necessary to activate the edges selection mode\n"
                       L"and select edges on an object before \nrunning this function");
        return;
      }
    }

    BoxRadius aDialog (NULL,10.);
    if (aDialog.DoModal() == IDOK)
    {
      for (myAISContext->InitSelected(); myAISContext->MoreSelected(); myAISContext->NextSelected())
      {
        // Selected shape has its own location, and sub-shapes in the solid shape for fillet building
        // don't have own locations. Fillet builder needs to know that input edge is just the same one as
        // some sub-shape in our target solid shape, so no location is to be in input edge for fillet builder.
        TopoDS_Shape aSelShape = myAISContext->SelectedShape().Located (TopLoc_Location());
        aFillet.Add (aDialog.m_radius, TopoDS::Edge (aSelShape) );
      }
    }
    else
    {
      return;
    }

    TopoDS_Shape aNewShape;
    try
    {
      aNewShape = aFillet.Shape();
    }
    catch (Standard_Failure)
    {
      AfxMessageBox (L"Error During Fillet computation");
      return;
    }

    aSelInteractive ->Set (aNewShape);
    myAISContext->Redisplay (aSelInteractive, Standard_True);
  }

  myCResultDialog.SetTitle("Make a fillet");
  myCResultDialog.SetText("  Handle(AIS_Shape) S = Handle(AIS_Shape)::DownCast(myAISContext->Interactive()); \n"
                                    "  \n"
                                    "  BRepAPI_MakeFillet aFillet(S->Shape()); \n"
                                    "  \n"
                                    "  TopoDS_Edge anEdge=TopoDS::Edge(myAISContext->SelectedShape()); \n"
                                    "  \n"
                                    "  aFillet.Add(dlg.m_radius,anEdge); \n"
                                    "  \n"
                                    "  TopoDS_Shape aNewShape = aFillet.Shape(); \n"
                                    "  \n"
                                    "  S->Set(aNewShape); \n"
                                    "  \n"
                                    "  myAISContext->Redisplay(S); \n"
                                    "  \n");
  SetTitle (L"Make a fillet");
}

// Create and display a circle with standard tools
void CViewer3dDoc::OnCircle() 
{
  gp_Ax2 anAx2 (gp_Pnt (0., 0., 0.), gp_Dir(0., 0., -1.));
  Handle(Geom_Circle) aGeomCircle = new Geom_Circle (anAx2, 300);

  // the lines above substitute
  // GC_MakeCircle C(gp_Pnt(-100.,-300.,0.),gp_Pnt(-50.,-200.,0.),gp_Pnt(-10.,-250.,0.));
  // Handle(AIS_Circle) anAISCirc = new AIS_Circle(C.Value());

  Handle(AIS_Circle) anAISCirc = new AIS_Circle(aGeomCircle);
  myAISContext->Display (anAISCirc, Standard_True);

  myCResultDialog.SetTitle("Create a circle");
  myCResultDialog.SetText("  GC_MakeCircle C(gp_Pnt(-100.,-300.,0.),gp_Pnt(-50.,-200.,0.),gp_Pnt(-10.,-250.,0.)); \n"
                                    "  \n"
                                    "  Handle(AIS_Circle) anAISCirc = new AIS_Circle(C.Value()); \n"
                                    "  \n"
                                    "  myAISContext->Display(anAISCirc); \n"
                                    "  \n");
  SetTitle (L"Create a circle");
}

void CViewer3dDoc::OnLine() 
{
  // TODO: Add your command handler code here
  gp_Lin aGpLin (gp_Pnt (0., 0., 0.), gp_Dir(1., 0., 0.));
  Handle(Geom_Line) aGeomLin = new Geom_Line (aGpLin);
  Handle(AIS_Line) anAISLine = new AIS_Line (aGeomLin);
  myAISContext->Display (anAISLine, Standard_True);

  myCResultDialog.SetTitle("Create a line");
  myCResultDialog.SetText("  gp_Lin L(gp_Pnt(0.,0.,0.),gp_Dir(1.,0.,0.)); \n"
                                    "  \n"
                                    "  Handle(Geom_Line) aLine = new Geom_Line(L); \n"
                                    "  \n"
                                    "  Handle(AIS_Line) anAISLine = new AIS_Line(aLine); \n"
                                    "  \n"
                                    "  myAISContext->Display(anAISLine); \n"
                                    "  \n");
  SetTitle (L"Create a line");
}

void CViewer3dDoc::OnNbisos() 
{
  int aNumU = myAISContext->DefaultDrawer()->UIsoAspect()->Number();
  int aNumV = myAISContext->DefaultDrawer()->VIsoAspect()->Number();

  DlgIsos aDlg (NULL, aNumU, aNumV);

  if (aDlg.DoModal() == IDOK)
  {
    myAISContext->DefaultDrawer()->UIsoAspect()->SetNumber (aDlg.m_isou);
    myAISContext->DefaultDrawer()->VIsoAspect()->SetNumber (aDlg.m_isov);

    myCResultDialog.SetTitle("Iso Aspect");
    myCResultDialog.SetText("  myAISContext->DefaultDrawer()->UIsoAspect()->SetNumber(dlg.m_isou); \n"
                                      "  \n"
                                      "  myAISContext->DefaultDrawer()->VIsoAspect()->SetNumber(dlg.m_isov); \n"
                                      "  \n");
    SetTitle (L"Iso Aspect");
  }
}

BOOL CViewer3dDoc::OnNewDocument()
{
  if (!CDocument::OnNewDocument())
    return FALSE;

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)
  SetTitle(myPresentation->GetName());

  myAISContext->EraseAll (Standard_False);
  myAISContext->SetDisplayMode(AIS_Shaded, Standard_True);

  POSITION pos = GetFirstViewPosition();
  while (pos != NULL)
  {
    CViewer3dView* pView = (CViewer3dView*) GetNextView(pos);
    pView->Reset();
  }  

  return TRUE;
}

void CViewer3dDoc::OnFileNew()
{
  OnNewDocument();
}

void CViewer3dDoc::InitViewButtons()
{
  POSITION pos = GetFirstViewPosition();
  while (pos != NULL)
  {
    CViewer3dView* pView = (CViewer3dView*) GetNextView(pos);
    pView->InitButtons();
  }
}

void CViewer3dDoc::OnTextureOn()
{
	isTextureSampleStarted = TRUE;
	Start();
}

void CViewer3dDoc::DoSample()
{
  InitViewButtons();

  HCURSOR hOldCursor = ::GetCursor();
  HCURSOR hNewCursor = AfxGetApp()->LoadStandardCursor(IDC_APPSTARTING);

  SetCursor(hNewCursor);
  {
    try
    {
      myPresentation->DoSample();
    }
    catch (Standard_Failure const& anException)
    {
      Standard_SStream aSStream;
      aSStream << "An exception was caught: " << anException << ends;
      CString aMsg = aSStream.str().c_str();
      AfxMessageBox (aMsg);
    }
  }
  SetCursor(hOldCursor);
}

void CViewer3dDoc::OnBUTTONStart() 
{
  myAISContext->EraseAll (Standard_True);
  myPresentation->FirstSample();
  DoSample();
}

void CViewer3dDoc::OnBUTTONEnd()
{
  myAISContext->EraseAll (Standard_True);
  myPresentation->LastSample();
  DoSample();
}

void CViewer3dDoc::OnBUTTONRepeat() 
{
  DoSample();
}

void CViewer3dDoc::OnBUTTONNext() 
{
  if (!myPresentation->AtLastSample())
  {
    myPresentation->NextSample();
    DoSample();
  }
}

void CViewer3dDoc::OnBUTTONPrev() 
{
  if (!myPresentation->AtFirstSample())
  {
    myPresentation->PrevSample();
    DoSample();
  }
}

void CViewer3dDoc::OnUpdateBUTTONNext(CCmdUI* pCmdUI) 
{
	if ( isTextureSampleStarted )
		pCmdUI->Enable (!myPresentation->AtLastSample());
	else
		pCmdUI->Enable (FALSE);
}

void CViewer3dDoc::OnUpdateBUTTONPrev(CCmdUI* pCmdUI) 
{
	if ( isTextureSampleStarted )
		pCmdUI->Enable (!myPresentation->AtFirstSample());
	else
		pCmdUI->Enable (FALSE);
}

void CViewer3dDoc::OnUpdateBUTTONStart(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (isTextureSampleStarted);
}

void CViewer3dDoc::OnUpdateBUTTONRepeat(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (isTextureSampleStarted);
}

void CViewer3dDoc::OnUpdateBUTTONEnd(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (isTextureSampleStarted);
}

void CViewer3dDoc::OnDumpView()
{
  for (POSITION aPos = GetFirstViewPosition(); aPos != NULL;)
  {
    CViewer3dView* pView = (CViewer3dView* )GetNextView (aPos);
    pView->UpdateWindow();
  }

  myViewer->InitActiveViews();
  Handle(V3d_View) aView = myViewer->ActiveView();
  ExportView (aView);
}

void CViewer3dDoc::Start()
{
  myPresentation->Init();
  OnBUTTONStart();
}

void CViewer3dDoc::Fit()
{
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	CViewer3dView *pView = (CViewer3dView *) pChild->GetActiveView();
	pView->FitAll();
}
