// Created on: 1997-07-23
// Created by: Henri JEANNIN
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Standard_Stream.hxx>

#include <ViewerTest.hxx>
#include <ViewerTest_CmdParser.hxx>

#include <Draw.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <OSD_Timer.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Axis1Placement.hxx>
#include <gp_Trsf.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <StdSelect_ShapeTypeFilter.hxx>
#include <AIS.hxx>
#include <AIS_ColoredShape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_Axis.hxx>
#include <AIS_Relation.hxx>
#include <AIS_TypeFilter.hxx>
#include <AIS_SignatureFilter.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Aspect_Window.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Image_AlienPixMap.hxx>
#include <OSD_File.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Select3D_SensitiveWire.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <TopTools_MapOfShape.hxx>
#include <ViewerTest_AutoUpdater.hxx>

#include <stdio.h>

#include <Draw_Interpretor.hxx>
#include <TCollection_AsciiString.hxx>
#include <Draw_PluginMacro.hxx>

extern int ViewerMainLoop(Standard_Integer argc, const char** argv);

#include <Quantity_Color.hxx>
#include <Quantity_NameOfColor.hxx>

#include <Graphic3d_NameOfMaterial.hxx>

#define DEFAULT_COLOR              Quantity_NOC_GOLDENROD
#define DEFAULT_FREEBOUNDARY_COLOR Quantity_NOC_GREEN
#define DEFAULT_MATERIAL           Graphic3d_NOM_BRASS

//=======================================================================
//function : GetColorFromName
//purpose  : get the Quantity_NameOfColor from a string
//=======================================================================

Quantity_NameOfColor ViewerTest::GetColorFromName (const Standard_CString theName)
{
  Quantity_NameOfColor aColor = DEFAULT_COLOR;
  Quantity_Color::ColorFromName (theName, aColor);
  return aColor;
}

//=======================================================================
//function : ParseColor
//purpose  :
//=======================================================================

Standard_Integer ViewerTest::ParseColor (Standard_Integer  theArgNb,
                                         const char**      theArgVec,
                                         Quantity_Color&   theColor)
{
  Quantity_NameOfColor aColor = Quantity_NOC_BLACK;
  if (theArgNb >= 1
   && Quantity_Color::ColorFromName (theArgVec[0], aColor))
  {
    theColor = aColor;
    return 1;
  }
  else if (theArgNb >= 3)
  {
    const TCollection_AsciiString anRgbStr[3] =
    {
      theArgVec[0],
      theArgVec[1],
      theArgVec[2]
    };
    if (!anRgbStr[0].IsRealValue()
     || !anRgbStr[1].IsRealValue()
     || !anRgbStr[2].IsRealValue())
    {
      return 0;
    }

    Graphic3d_Vec4d anRgb;
    anRgb.x() = anRgbStr[0].RealValue();
    anRgb.y() = anRgbStr[1].RealValue();
    anRgb.z() = anRgbStr[2].RealValue();
    if (anRgb.x() < 0.0 || anRgb.x() > 1.0
     || anRgb.y() < 0.0 || anRgb.y() > 1.0
     || anRgb.z() < 0.0 || anRgb.z() > 1.0)
    {
      std::cout << "Error: RGB color values should be within range 0..1!\n";
      return 0;
    }

    theColor.SetValues (anRgb.x(), anRgb.y(), anRgb.z(), Quantity_TOC_RGB);
    return 3;
  }

  return 0;
}

//=======================================================================
//function : ParseOnOff
//purpose  :
//=======================================================================
Standard_Boolean ViewerTest::ParseOnOff (Standard_CString  theArg,
                                         Standard_Boolean& theIsOn)
{
  TCollection_AsciiString aFlag(theArg);
  aFlag.LowerCase();
  if (aFlag == "on"
   || aFlag == "1")
  {
    theIsOn = Standard_True;
    return Standard_True;
  }
  else if (aFlag == "off"
        || aFlag == "0")
  {
    theIsOn = Standard_False;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : GetSelectedShapes
//purpose  :
//=======================================================================
void ViewerTest::GetSelectedShapes (TopTools_ListOfShape& theSelectedShapes)
{
  for (GetAISContext()->InitSelected(); GetAISContext()->MoreSelected(); GetAISContext()->NextSelected())
  {
    TopoDS_Shape aShape = GetAISContext()->SelectedShape();
    if (!aShape.IsNull())
    {
      theSelectedShapes.Append (aShape);
    }
  }
}

//=======================================================================
//function : ParseLineType
//purpose  :
//=======================================================================
Standard_Boolean ViewerTest::ParseLineType (Standard_CString   theArg,
                                            Aspect_TypeOfLine& theType)
{
  TCollection_AsciiString aTypeStr (theArg);
  aTypeStr.LowerCase();
  if (aTypeStr == "empty")
  {
    theType = Aspect_TOL_EMPTY;
  }
  else if (aTypeStr == "solid")
  {
    theType = Aspect_TOL_SOLID;
  }
  else if (aTypeStr == "dot")
  {
    theType = Aspect_TOL_DOT;
  }
  else if (aTypeStr == "dash")
  {
    theType = Aspect_TOL_DASH;
  }
  else if (aTypeStr == "dotdash")
  {
    theType = Aspect_TOL_DOTDASH;
  }
  else
  {
    const int aTypeInt = Draw::Atoi (theArg);
    if (aTypeInt < -1 || aTypeInt >= Aspect_TOL_USERDEFINED)
    {
      return Standard_False;
    }
    theType = (Aspect_TypeOfLine )aTypeInt;
  }
  return Standard_True;
}

//=======================================================================
//function : ParseMarkerType
//purpose  :
//=======================================================================
Standard_Boolean ViewerTest::ParseMarkerType (Standard_CString theArg,
                                              Aspect_TypeOfMarker& theType,
                                              Handle(Image_PixMap)& theImage)
{
  theImage.Nullify();
  TCollection_AsciiString aTypeStr (theArg);
  aTypeStr.LowerCase();
  if (aTypeStr == "empty")
  {
    theType = Aspect_TOM_EMPTY;
  }
  else if (aTypeStr == "point"
        || aTypeStr == "dot"
        || aTypeStr == ".")
  {
    theType = Aspect_TOM_POINT;
  }
  else if (aTypeStr == "plus"
        || aTypeStr == "+")
  {
    theType = Aspect_TOM_PLUS;
  }
  else if (aTypeStr == "star"
        || aTypeStr == "*")
  {
    theType = Aspect_TOM_STAR;
  }
  else if (aTypeStr == "cross"
        || aTypeStr == "x")
  {
    theType = Aspect_TOM_X;
  }
  else if (aTypeStr == "circle"
        || aTypeStr == "o")
  {
    theType = Aspect_TOM_O;
  }
  else if (aTypeStr == "pointincircle")
  {
    theType = Aspect_TOM_O_POINT;
  }
  else if (aTypeStr == "plusincircle")
  {
    theType = Aspect_TOM_O_PLUS;
  }
  else if (aTypeStr == "starincircle")
  {
    theType = Aspect_TOM_O_STAR;
  }
  else if (aTypeStr == "crossincircle"
        || aTypeStr == "xcircle")
  {
    theType = Aspect_TOM_O_X;
  }
  else if (aTypeStr == "ring1")
  {
    theType = Aspect_TOM_RING1;
  }
  else if (aTypeStr == "ring2")
  {
    theType = Aspect_TOM_RING2;
  }
  else if (aTypeStr == "ring"
        || aTypeStr == "ring3")
  {
    theType = Aspect_TOM_RING3;
  }
  else if (aTypeStr == "ball")
  {
    theType = Aspect_TOM_BALL;
  }
  else if (aTypeStr.IsIntegerValue())
  {
    const int aTypeInt = aTypeStr.IntegerValue();
    if (aTypeInt < -1 || aTypeInt >= Aspect_TOM_USERDEFINED)
    {
      return Standard_False;
    }
    theType = (Aspect_TypeOfMarker )aTypeInt;
  }
  else
  {
    theType = Aspect_TOM_USERDEFINED;
    Handle(Image_AlienPixMap) anImage = new Image_AlienPixMap();
    if (!anImage->Load (theArg))
    {
      return Standard_False;
    }
    if (anImage->Format() == Image_Format_Gray)
    {
      anImage->SetFormat (Image_Format_Alpha);
    }
    else if (anImage->Format() == Image_Format_GrayF)
    {
      anImage->SetFormat (Image_Format_AlphaF);
    }
    theImage = anImage;
  }
  return Standard_True;
}

//=======================================================================
//function : GetTypeNames
//purpose  :
//=======================================================================
static const char** GetTypeNames()
{
  static const char* names[14] = {"Point","Axis","Trihedron","PlaneTrihedron", "Line","Circle","Plane",
			  "Shape","ConnectedShape","MultiConn.Shape",
			  "ConnectedInter.","MultiConn.",
			  "Constraint","Dimension"};
  static const char** ThePointer = names;
  return ThePointer;
}

//=======================================================================
//function : GetTypeAndSignfromString
//purpose  :
//=======================================================================
void GetTypeAndSignfromString (const char* name,AIS_KindOfInteractive& TheType,Standard_Integer& TheSign)
{
  const char ** thefullnames = GetTypeNames();
  Standard_Integer index(-1);

  for(Standard_Integer i=0;i<=13 && index==-1;i++)
    if(!strcasecmp(name,thefullnames[i]))
      index = i;

  if(index ==-1){
    TheType = AIS_KOI_None;
    TheSign = -1;
    return;
  }

  if(index<=6){
    TheType = AIS_KOI_Datum;
    TheSign = index+1;
  }
  else if (index <=9){
    TheType = AIS_KOI_Shape;
    TheSign = index-7;
  }
  else if(index<=11){
    TheType = AIS_KOI_Object;
    TheSign = index-10;
  }
  else{
    TheType = AIS_KOI_Relation;
    TheSign = index-12;
  }

}



#include <string.h>
#include <Draw_Interpretor.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <DBRep.hxx>


#include <TCollection_AsciiString.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <V3d.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <AIS_DisplayMode.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <AIS_MapOfInteractive.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_EventManager.hxx>

#include <TopoDS_Solid.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS.hxx>
#include <BRep_Tool.hxx>


#include <Draw_Window.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_DisplayMode.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>

//==============================================================================
//  VIEWER OBJECT MANAGEMENT GLOBAL VARIABLES
//==============================================================================
Standard_EXPORT ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS(){
  static ViewerTest_DoubleMapOfInteractiveAndName TheMap;
  return TheMap;
}

//=======================================================================
//function : Display
//purpose  :
//=======================================================================
Standard_Boolean ViewerTest::Display (const TCollection_AsciiString&       theName,
                                      const Handle(AIS_InteractiveObject)& theObject,
                                      const Standard_Boolean               theToUpdate,
                                      const Standard_Boolean               theReplaceIfExists)
{
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "Error: AIS context is not available.\n";
    return Standard_False;
  }

  if (aMap.IsBound2 (theName))
  {
    if (!theReplaceIfExists)
    {
      std::cout << "Error: other interactive object has been already registered with name: " << theName << ".\n"
                << "Please use another name.\n";
      return Standard_False;
    }

    Handle(AIS_InteractiveObject) anOldObj = Handle(AIS_InteractiveObject)::DownCast (aMap.Find2 (theName));
    if (!anOldObj.IsNull())
    {
      aCtx->Remove (anOldObj, theObject.IsNull() && theToUpdate);
    }
    aMap.UnBind2 (theName);
  }

  if (theObject.IsNull())
  {
    // object with specified name has been already unbound
    return Standard_True;
  }

  // unbind AIS object if it was bound with another name
  aMap.UnBind1 (theObject);

  // can be registered without rebinding
  aMap.Bind (theObject, theName);
  aCtx->Display (theObject, theToUpdate);
  return Standard_True;
}

//! Alias for ViewerTest::Display(), compatibility with old code.
Standard_EXPORT Standard_Boolean VDisplayAISObject (const TCollection_AsciiString&       theName,
                                                    const Handle(AIS_InteractiveObject)& theObject,
                                                    Standard_Boolean theReplaceIfExists = Standard_True)
{
  return ViewerTest::Display (theName, theObject, Standard_True, theReplaceIfExists);
}

static TColStd_MapOfInteger theactivatedmodes(8);
static TColStd_ListOfTransient theEventMgrs;

static void VwrTst_InitEventMgr(const Handle(V3d_View)& aView,
                                const Handle(AIS_InteractiveContext)& Ctx)
{
  theEventMgrs.Clear();
  theEventMgrs.Prepend(new ViewerTest_EventManager(aView, Ctx));
}

static Handle(V3d_View)&  a3DView()
{
  static Handle(V3d_View) Viou;
  return Viou;
}


Standard_EXPORT Handle(AIS_InteractiveContext)& TheAISContext(){
  static Handle(AIS_InteractiveContext) aContext;
  return aContext;
}

const Handle(V3d_View)& ViewerTest::CurrentView()
{
  return a3DView();
}
void ViewerTest::CurrentView(const Handle(V3d_View)& V)
{
  a3DView() = V;
}

const Handle(AIS_InteractiveContext)& ViewerTest::GetAISContext()
{
  return TheAISContext();
}

void ViewerTest::SetAISContext (const Handle(AIS_InteractiveContext)& aCtx)
{
  TheAISContext() = aCtx;
  ViewerTest::ResetEventManager();
}

Handle(V3d_Viewer) ViewerTest::GetViewerFromContext()
{
  return !TheAISContext().IsNull() ? TheAISContext()->CurrentViewer() : Handle(V3d_Viewer)();
}

Handle(V3d_Viewer) ViewerTest::GetCollectorFromContext()
{
  return !TheAISContext().IsNull() ? TheAISContext()->CurrentViewer() : Handle(V3d_Viewer)();
}


void ViewerTest::SetEventManager(const Handle(ViewerTest_EventManager)& EM){
  theEventMgrs.Prepend(EM);
}

void ViewerTest::UnsetEventManager()
{
  theEventMgrs.RemoveFirst();
}


void ViewerTest::ResetEventManager()
{
  const Handle(V3d_View) aView = ViewerTest::CurrentView();
  VwrTst_InitEventMgr(aView, ViewerTest::GetAISContext());
}

Handle(ViewerTest_EventManager) ViewerTest::CurrentEventManager()
{
  Handle(ViewerTest_EventManager) EM;
  if(theEventMgrs.IsEmpty()) return EM;
  Handle(Standard_Transient) Tr =  theEventMgrs.First();
  EM = Handle(ViewerTest_EventManager)::DownCast (Tr);
  return EM;
}

//=======================================================================
//function : Get Context and active view
//purpose  :
//=======================================================================
static Standard_Boolean getCtxAndView (Handle(AIS_InteractiveContext)& theCtx,
                                       Handle(V3d_View)&               theView)
{
  theCtx  = ViewerTest::GetAISContext();
  theView = ViewerTest::CurrentView();
  if (theCtx.IsNull()
   || theView.IsNull())
  {
    std::cout << "Error: cannot find an active view!\n";
    return Standard_False;
  }
  return Standard_True;
}

//==============================================================================
//function : GetShapeFromName
//purpose  : Compute an Shape from a draw variable or a file name
//==============================================================================

static TopoDS_Shape GetShapeFromName(const char* name)
{
  TopoDS_Shape S = DBRep::Get(name);

  if ( S.IsNull() ) {
   	BRep_Builder aBuilder;
  	BRepTools::Read( S, name, aBuilder);
  }

  return S;
}

//==============================================================================
//function : GetAISShapeFromName
//purpose  : Compute an AIS_Shape from a draw variable or a file name
//==============================================================================
Handle(AIS_Shape) GetAISShapeFromName(const char* name)
{
  Handle(AIS_Shape) retsh;

  if(GetMapOfAIS().IsBound2(name)){
    const Handle(AIS_InteractiveObject) IO =
      Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(name));
    if (!IO.IsNull()) {
      if(IO->Type()==AIS_KOI_Shape) {
        if(IO->Signature()==0){
          retsh = Handle(AIS_Shape)::DownCast (IO);
        }
        else
          cout << "an Object which is not an AIS_Shape "
            "already has this name!!!"<<endl;
      }
    }
    return retsh;
  }


  TopoDS_Shape S = GetShapeFromName(name);
  if ( !S.IsNull() ) {
    retsh = new AIS_Shape(S);
  }
  return retsh;
}


//==============================================================================
//function : Clear
//purpose  : Remove all the object from the viewer
//==============================================================================
void ViewerTest::Clear()
{
  if (a3DView().IsNull())
  {
    return;
  }

  NCollection_Sequence<Handle(AIS_InteractiveObject)> aListRemoved;
  for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anObjIter (GetMapOfAIS()); anObjIter.More(); anObjIter.Next())
  {
    const Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (anObjIter.Key1());
    if (anObj->GetContext() != TheAISContext())
    {
      continue;
    }

    std::cout << "Remove " << anObjIter.Key2() << std::endl;
    TheAISContext()->Remove (anObj, Standard_False);
    aListRemoved.Append (anObj);
  }

  TheAISContext()->RebuildSelectionStructs();
  TheAISContext()->UpdateCurrentViewer();
  if (aListRemoved.Size() == GetMapOfAIS().Extent())
  {
    GetMapOfAIS().Clear();
  }
  else
  {
    for (NCollection_Sequence<Handle(AIS_InteractiveObject)>::Iterator anObjIter (aListRemoved); anObjIter.More(); anObjIter.Next())
    {
      GetMapOfAIS().UnBind1 (anObjIter.Value());
    }
  }
}

//==============================================================================
//function : StandardModesActivation
//purpose  : Activate a selection mode, vertex, edge, wire ..., in a local
//           Context
//==============================================================================
Standard_DISABLE_DEPRECATION_WARNINGS
void ViewerTest::StandardModeActivation(const Standard_Integer mode )
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(mode==0) {
    if (TheAISContext()->HasOpenedContext())
    {
      aContext->CloseLocalContext();
    }
  } else {

    if(!aContext->HasOpenedContext()) {
      // To unhilight the preselected object
      aContext->UnhilightSelected(Standard_False);
      // Open a local Context in order to be able to select subshape from
      // the selected shape if any or for all if there is no selection
      if (!aContext->FirstSelectedObject().IsNull()){
        aContext->OpenLocalContext(Standard_False);

	for(aContext->InitSelected();aContext->MoreSelected();aContext->NextSelected()){
	  aContext->Load(	aContext->SelectedInteractive(),-1,Standard_True);
	}
      }
      else
      {
        aContext->OpenLocalContext();
      }
    }

    const TopAbs_ShapeEnum aShapeType = AIS_Shape::SelectionType (mode);
    const char* cmode = mode >= 0 && mode <= 8
                      ? TopAbs::ShapeTypeToString (aShapeType)
                      : "???";
    if(theactivatedmodes.Contains(mode))
    { // Desactivate
      aContext->DeactivateStandardMode(AIS_Shape::SelectionType(mode));
      theactivatedmodes.Remove(mode);
      cout<<"Mode "<< cmode <<" OFF"<<endl;
    }
    else
    { // Activate
      aContext->ActivateStandardMode(AIS_Shape::SelectionType(mode));
      theactivatedmodes.Add(mode);
      cout<<"Mode "<< cmode << " ON" << endl;
    }
  }
}
Standard_ENABLE_DEPRECATION_WARNINGS

//==============================================================================
//function : CopyIsoAspect
//purpose  : Returns copy Prs3d_IsoAspect with new number of isolines.
//==============================================================================
static Handle(Prs3d_IsoAspect) CopyIsoAspect
      (const Handle(Prs3d_IsoAspect) &theIsoAspect,
       const Standard_Integer theNbIsos)
{
  Quantity_Color    aColor = theIsoAspect->Aspect()->Color();
  Aspect_TypeOfLine aType  = theIsoAspect->Aspect()->Type();
  Standard_Real     aWidth = theIsoAspect->Aspect()->Width();

  Handle(Prs3d_IsoAspect) aResult =
    new Prs3d_IsoAspect(aColor, aType, aWidth, theNbIsos);

  return aResult;
}

//==============================================================================
//function : visos
//purpose  : Returns or sets the number of U- and V- isos and isIsoOnPlane flag
//Draw arg : [name1 ...] [nbUIsos nbVIsos IsoOnPlane(0|1)]
//==============================================================================
static int visos (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (TheAISContext().IsNull()) {
    di << argv[0] << " Call 'vinit' before!\n";
    return 1;
  }

  if (argc <= 1) {
    di << "Current number of isos : " <<
      TheAISContext()->IsoNumber(AIS_TOI_IsoU) << " " <<
      TheAISContext()->IsoNumber(AIS_TOI_IsoV) << "\n";
    di << "IsoOnPlane mode is " <<
      (TheAISContext()->IsoOnPlane() ? "ON" : "OFF") << "\n";
    di << "IsoOnTriangulation mode is " <<
      (TheAISContext()->IsoOnTriangulation() ? "ON" : "OFF") << "\n";
    return 0;
  }

  Standard_Integer aLastInd = argc - 1;
  Standard_Boolean isChanged = Standard_False;
  Standard_Integer aNbUIsos = 0;
  Standard_Integer aNbVIsos = 0;

  if (aLastInd >= 3) {
    Standard_Boolean isIsoOnPlane = Standard_False;

    if (strcmp(argv[aLastInd], "1") == 0) {
      isIsoOnPlane = Standard_True;
      isChanged    = Standard_True;
    } else if (strcmp(argv[aLastInd], "0") == 0) {
      isIsoOnPlane = Standard_False;
      isChanged    = Standard_True;
    }

    if (isChanged) {
      aNbVIsos = Draw::Atoi(argv[aLastInd - 1]);
      aNbUIsos = Draw::Atoi(argv[aLastInd - 2]);
      aLastInd -= 3;

      di << "New number of isos : " << aNbUIsos << " " << aNbVIsos << "\n";
      di << "New IsoOnPlane mode is " << (isIsoOnPlane ? "ON" : "OFF") << "\n";

      TheAISContext()->IsoOnPlane(isIsoOnPlane);

      if (aLastInd == 0) {
        // If there are no shapes provided set the default numbers.
        TheAISContext()->SetIsoNumber(aNbUIsos, AIS_TOI_IsoU);
        TheAISContext()->SetIsoNumber(aNbVIsos, AIS_TOI_IsoV);
      }
    }
  }

  Standard_Integer i;

  for (i = 1; i <= aLastInd; i++) {
    TCollection_AsciiString name(argv[i]);
    Standard_Boolean IsBound = GetMapOfAIS().IsBound2(name);

    if (IsBound) {
      const Handle(Standard_Transient) anObj = GetMapOfAIS().Find2(name);
      if (anObj->IsKind(STANDARD_TYPE(AIS_InteractiveObject))) {
        const Handle(AIS_InteractiveObject) aShape =
        Handle(AIS_InteractiveObject)::DownCast (anObj);
        Handle(Prs3d_Drawer) CurDrawer = aShape->Attributes();
        Handle(Prs3d_IsoAspect) aUIso = CurDrawer->UIsoAspect();
        Handle(Prs3d_IsoAspect) aVIso = CurDrawer->VIsoAspect();

        if (isChanged) {
          CurDrawer->SetUIsoAspect(CopyIsoAspect(aUIso, aNbUIsos));
          CurDrawer->SetVIsoAspect(CopyIsoAspect(aVIso, aNbVIsos));
          TheAISContext()->SetLocalAttributes
                  (aShape, CurDrawer, Standard_False);
          TheAISContext()->Redisplay (aShape, Standard_False);
        } else {
          di << "Number of isos for " << argv[i] << " : "
             << aUIso->Number() << " " << aVIso->Number() << "\n";
        }
      } else {
        di << argv[i] << ": Not an AIS interactive object!\n";
      }
    } else {
      di << argv[i] << ": Use 'vdisplay' before\n";
    }
  }

  if (isChanged) {
    TheAISContext()->UpdateCurrentViewer();
  }

  return 0;
}

static Standard_Integer VDispSensi (Draw_Interpretor& ,
                                    Standard_Integer  theArgNb,
                                    Standard_CString* )
{
  if (theArgNb > 1)
  {
    std::cout << "Error: wrong syntax!\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aCtx;
  Handle(V3d_View)               aView;
  if (!getCtxAndView (aCtx, aView))
  {
    return 1;
  }

  aCtx->DisplayActiveSensitive (aView);
  return 0;

}

static Standard_Integer VClearSensi (Draw_Interpretor& ,
                                     Standard_Integer  theArgNb,
                                     Standard_CString* )
{
  if (theArgNb > 1)
  {
    std::cout << "Error: wrong syntax!\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aCtx;
  Handle(V3d_View)               aView;
  if (!getCtxAndView (aCtx, aView))
  {
    return 1;
  }
  aCtx->ClearActiveSensitive (aView);
  return 0;
}

//==============================================================================
//function : VDir
//purpose  : To list the displayed object with their attributes
//==============================================================================
static int VDir (Draw_Interpretor& theDI,
                 Standard_Integer ,
                 const char** )
{
  if (!a3DView().IsNull())
  {
    return 0;
  }

  for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
       anIter.More(); anIter.Next())
  {
    theDI << "\t" << anIter.Key2().ToCString() << "\n";
  }
  return 0;
}

//! Auxiliary enumeration
enum ViewerTest_StereoPair
{
  ViewerTest_SP_Single,
  ViewerTest_SP_SideBySide,
  ViewerTest_SP_OverUnder
};

//==============================================================================
//function : VDump
//purpose  : To dump the active view snapshot to image file
//==============================================================================
static Standard_Integer VDump (Draw_Interpretor& theDI,
                               Standard_Integer  theArgNb,
                               Standard_CString* theArgVec)
{
  if (theArgNb < 2)
  {
    std::cout << "Error: wrong number of arguments! Image file name should be specified at least.\n";
    return 1;
  }

  Standard_Integer      anArgIter   = 1;
  Standard_CString      aFilePath   = theArgVec[anArgIter++];
  ViewerTest_StereoPair aStereoPair = ViewerTest_SP_Single;
  V3d_ImageDumpOptions  aParams;
  aParams.BufferType    = Graphic3d_BT_RGB;
  aParams.StereoOptions = V3d_SDO_MONO;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-buffer")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }

      TCollection_AsciiString aBufArg (theArgVec[anArgIter]);
      aBufArg.LowerCase();
      if (aBufArg == "rgba")
      {
        aParams.BufferType = Graphic3d_BT_RGBA;
      }
      else if (aBufArg == "rgb")
      {
        aParams.BufferType = Graphic3d_BT_RGB;
      }
      else if (aBufArg == "depth")
      {
        aParams.BufferType = Graphic3d_BT_Depth;
      }
      else
      {
        std::cout << "Error: unknown buffer '" << aBufArg << "'\n";
        return 1;
      }
    }
    else if (anArg == "-stereo")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }

      TCollection_AsciiString aStereoArg (theArgVec[anArgIter]);
      aStereoArg.LowerCase();
      if (aStereoArg == "l"
       || aStereoArg == "left")
      {
        aParams.StereoOptions = V3d_SDO_LEFT_EYE;
      }
      else if (aStereoArg == "r"
            || aStereoArg == "right")
      {
        aParams.StereoOptions = V3d_SDO_RIGHT_EYE;
      }
      else if (aStereoArg == "mono")
      {
        aParams.StereoOptions = V3d_SDO_MONO;
      }
      else if (aStereoArg == "blended"
            || aStereoArg == "blend"
            || aStereoArg == "stereo")
      {
        aParams.StereoOptions = V3d_SDO_BLENDED;
      }
      else if (aStereoArg == "sbs"
            || aStereoArg == "sidebyside")
      {
        aStereoPair = ViewerTest_SP_SideBySide;
      }
      else if (aStereoArg == "ou"
            || aStereoArg == "overunder")
      {
        aStereoPair = ViewerTest_SP_OverUnder;
      }
      else
      {
        std::cout << "Error: unknown stereo format '" << aStereoArg << "'\n";
        return 1;
      }
    }
    else if (anArg == "-rgba"
          || anArg ==  "rgba")
    {
      aParams.BufferType = Graphic3d_BT_RGBA;
    }
    else if (anArg == "-rgb"
          || anArg ==  "rgb")
    {
      aParams.BufferType = Graphic3d_BT_RGB;
    }
    else if (anArg == "-depth"
          || anArg ==  "depth")
    {
      aParams.BufferType = Graphic3d_BT_Depth;
    }
    else if (anArg == "-width"
          || anArg ==  "width"
          || anArg ==  "sizex")
    {
      if (aParams.Width != 0)
      {
        std::cout << "Error: wrong syntax at " << theArgVec[anArgIter] << "\n";
        return 1;
      }
      else if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: integer value is expected right after 'width'\n";
        return 1;
      }
      aParams.Width = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (anArg == "-height"
          || anArg ==  "height"
          || anArg ==  "-sizey")
    {
      if (aParams.Height != 0)
      {
        std::cout << "Error: wrong syntax at " << theArgVec[anArgIter] << "\n";
        return 1;
      }
      else if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: integer value is expected right after 'height'\n";
        return 1;
      }
      aParams.Height = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (anArg == "-tile"
          || anArg == "-tilesize")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: integer value is expected right after 'tileSize'\n";
        return 1;
      }
      aParams.TileSize = Draw::Atoi (theArgVec[anArgIter]);
    }
    else
    {
      std::cout << "Error: unknown argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  if ((aParams.Width <= 0 && aParams.Height >  0)
   || (aParams.Width >  0 && aParams.Height <= 0))
  {
    std::cout << "Error: dimensions " << aParams.Width << "x" << aParams.Height << " are incorrect\n";
    return 1;
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cout << "Error: cannot find an active view!\n";
    return 1;
  }

  if (aParams.Width <= 0 || aParams.Height <= 0)
  {
    aView->Window()->Size (aParams.Width, aParams.Height);
  }

  Image_AlienPixMap aPixMap;
  Image_Format aFormat = Image_Format_UNKNOWN;
  switch (aParams.BufferType)
  {
    case Graphic3d_BT_RGB:                 aFormat = Image_Format_RGB;   break;
    case Graphic3d_BT_RGBA:                aFormat = Image_Format_RGBA;  break;
    case Graphic3d_BT_Depth:               aFormat = Image_Format_GrayF; break;
    case Graphic3d_BT_RGB_RayTraceHdrLeft: aFormat = Image_Format_RGBF;  break;
  }

  switch (aStereoPair)
  {
    case ViewerTest_SP_Single:
    {
      if (!aView->ToPixMap (aPixMap, aParams))
      {
        theDI << "Fail: view dump failed!\n";
        return 0;
      }
      else if (aPixMap.SizeX() != Standard_Size(aParams.Width)
            || aPixMap.SizeY() != Standard_Size(aParams.Height))
      {
        theDI << "Fail: dumped dimensions "    << (Standard_Integer )aPixMap.SizeX() << "x" << (Standard_Integer )aPixMap.SizeY()
              << " are lesser than requested " << aParams.Width << "x" << aParams.Height << "\n";
      }
      break;
    }
    case ViewerTest_SP_SideBySide:
    {
      if (!aPixMap.InitZero (aFormat, aParams.Width * 2, aParams.Height))
      {
        theDI << "Fail: not enough memory for image allocation!\n";
        return 0;
      }

      Image_PixMap aPixMapL, aPixMapR;
      aPixMapL.InitWrapper (aPixMap.Format(), aPixMap.ChangeData(),
                            aParams.Width, aParams.Height, aPixMap.SizeRowBytes());
      aPixMapR.InitWrapper (aPixMap.Format(), aPixMap.ChangeData() + aPixMap.SizePixelBytes() * aParams.Width,
                            aParams.Width, aParams.Height, aPixMap.SizeRowBytes());

      aParams.StereoOptions = V3d_SDO_LEFT_EYE;
      Standard_Boolean isOk = aView->ToPixMap (aPixMapL, aParams);
      aParams.StereoOptions = V3d_SDO_RIGHT_EYE;
      isOk          = isOk && aView->ToPixMap (aPixMapR, aParams);
      if (!isOk)
      {
        theDI << "Fail: view dump failed!\n";
        return 0;
      }
      break;
    }
    case ViewerTest_SP_OverUnder:
    {
      if (!aPixMap.InitZero (aFormat, aParams.Width, aParams.Height * 2))
      {
        theDI << "Fail: not enough memory for image allocation!\n";
        return 0;
      }

      Image_PixMap aPixMapL, aPixMapR;
      aPixMapL.InitWrapper (aPixMap.Format(), aPixMap.ChangeData(),
                            aParams.Width, aParams.Height, aPixMap.SizeRowBytes());
      aPixMapR.InitWrapper (aPixMap.Format(), aPixMap.ChangeData() + aPixMap.SizeRowBytes() * aParams.Height,
                            aParams.Width, aParams.Height, aPixMap.SizeRowBytes());

      aParams.StereoOptions = V3d_SDO_LEFT_EYE;
      Standard_Boolean isOk = aView->ToPixMap (aPixMapL, aParams);
      aParams.StereoOptions = V3d_SDO_RIGHT_EYE;
      isOk          = isOk && aView->ToPixMap (aPixMapR, aParams);
      if (!isOk)
      {
        theDI << "Fail: view dump failed!\n";
        return 0;
      }
      break;
    }
  }

  if (!aPixMap.Save (aFilePath))
  {
    theDI << "Fail: image can not be saved!\n";
  }
  return 0;
}

enum TypeOfDispOperation
{
  TypeOfDispOperation_SetDispMode,
  TypeOfDispOperation_UnsetDispMode
};

//! Displays,Erase...
static void VwrTst_DispErase (const Handle(AIS_InteractiveObject)& thePrs,
			                        const Standard_Integer theMode,
			                        const TypeOfDispOperation theType,
			                        const Standard_Boolean theToUpdate)
{
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  switch (theType)
  {
    case TypeOfDispOperation_SetDispMode:
    {
      if (!thePrs.IsNull())
      {
        aCtx->SetDisplayMode (thePrs, theMode, theToUpdate);
      }
      else
      {
        aCtx->SetDisplayMode ((AIS_DisplayMode )theMode, theToUpdate);
      }
      break;
    }
    case TypeOfDispOperation_UnsetDispMode:
    {
      if (!thePrs.IsNull())
      {
        aCtx->UnsetDisplayMode (thePrs, theToUpdate);
      }
      else
      {
        aCtx->SetDisplayMode (AIS_WireFrame, theToUpdate);
      }
      break;
    }
  }
}

//=======================================================================
//function :
//purpose  :
//=======================================================================
static int VDispMode (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc < 1
   || argc > 3)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  TypeOfDispOperation aType = TCollection_AsciiString (argv[0]) == "vunsetdispmode"
                            ? TypeOfDispOperation_UnsetDispMode
                            : TypeOfDispOperation_SetDispMode;
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aType == TypeOfDispOperation_UnsetDispMode)
  {
    if (argc == 1)
    {
      if (aCtx->NbSelected() == 0)
      {
        VwrTst_DispErase (Handle(AIS_InteractiveObject)(), -1, TypeOfDispOperation_UnsetDispMode, Standard_False);
      }
      else
      {
        for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
        {
          VwrTst_DispErase (aCtx->SelectedInteractive(), -1, TypeOfDispOperation_UnsetDispMode, Standard_False);
        }
      }
      aCtx->UpdateCurrentViewer();
    }
    else
    {
      TCollection_AsciiString aName = argv[1];
      if (GetMapOfAIS().IsBound2 (aName))
      {
        Handle(AIS_InteractiveObject) aPrs = Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2 (aName));
        if (!aPrs.IsNull())
        {
          VwrTst_DispErase (aPrs, -1, TypeOfDispOperation_UnsetDispMode, Standard_True);
        }
      }
    }
  }
  else if (argc == 2)
  {
    Standard_Integer aDispMode = Draw::Atoi (argv[1]);
    if (aCtx->NbSelected() == 0
     && aType == TypeOfDispOperation_SetDispMode)
    {
      VwrTst_DispErase (Handle(AIS_InteractiveObject)(), aDispMode, TypeOfDispOperation_SetDispMode, Standard_True);
    }
    for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
    {
      VwrTst_DispErase (aCtx->SelectedInteractive(), aDispMode, aType, Standard_False);
    }
    aCtx->UpdateCurrentViewer();
  }
  else
  {
    Handle(AIS_InteractiveObject) aPrs;
    TCollection_AsciiString aName (argv[1]);
    if (GetMapOfAIS().IsBound2 (aName))
    {
      aPrs = Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2 (aName));
    }
    if (!aPrs.IsNull())
    {
      VwrTst_DispErase (aPrs, Draw::Atoi(argv[2]), aType, Standard_True);
    }
  }
  return 0;
}


//=======================================================================
//function :
//purpose  :
//=======================================================================
static int VSubInt(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if(argc==1) return 1;
  Standard_Integer On = Draw::Atoi(argv[1]);
  const Handle(AIS_InteractiveContext)& Ctx = ViewerTest::GetAISContext();

  if(argc==2)
  {
    TCollection_AsciiString isOnOff = On == 1 ? "on" : "off";
    di << "Sub intensite is turned " << isOnOff << " for " << Ctx->NbSelected() << "objects\n";
    for (Ctx->InitSelected(); Ctx->MoreSelected(); Ctx->NextSelected())
    {
      if(On==1)
      {
        Ctx->SubIntensityOn (Ctx->SelectedInteractive(), Standard_False);
      }
      else
      {
        Ctx->SubIntensityOff (Ctx->SelectedInteractive(), Standard_False);
      }
    }

    Ctx->UpdateCurrentViewer();
  }
  else {
    Handle(AIS_InteractiveObject) IO;
    TCollection_AsciiString name = argv[2];
    if(GetMapOfAIS().IsBound2(name)){
      IO = Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(name));
      if (!IO.IsNull()) {
        if(On==1)
          Ctx->SubIntensityOn(IO, Standard_True);
        else
          Ctx->SubIntensityOff(IO, Standard_True);
      }
    }
    else return 1;
  }
  return 0;
}

//! Auxiliary class to iterate presentations from different collections.
class ViewTest_PrsIter
{
public:

  //! Create and initialize iterator object.
  ViewTest_PrsIter (const TCollection_AsciiString& theName)
  : mySource (IterSource_All)
  {
    NCollection_Sequence<TCollection_AsciiString> aNames;
    if (!theName.IsEmpty())
    aNames.Append (theName);
    Init (aNames);
  }

  //! Create and initialize iterator object.
  ViewTest_PrsIter (const NCollection_Sequence<TCollection_AsciiString>& theNames)
  : mySource (IterSource_All)
  {
    Init (theNames);
  }

  //! Initialize the iterator.
  void Init (const NCollection_Sequence<TCollection_AsciiString>& theNames)
  {
    Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
    mySeq = theNames;
    mySelIter.Nullify();
    myCurrent.Nullify();
    myCurrentTrs.Nullify();
    if (!mySeq.IsEmpty())
    {
      mySource = IterSource_List;
      mySeqIter = NCollection_Sequence<TCollection_AsciiString>::Iterator (mySeq);
    }
    else if (aCtx->NbSelected() > 0)
    {
      mySource  = IterSource_Selected;
      mySelIter = aCtx;
      mySelIter->InitSelected();
    }
    else
    {
      mySource = IterSource_All;
      myMapIter.Initialize (GetMapOfAIS());
    }
    initCurrent();
  }

  const TCollection_AsciiString& CurrentName() const
  {
    return myCurrentName;
  }

  const Handle(AIS_InteractiveObject)& Current() const
  {
    return myCurrent;
  }

  const Handle(Standard_Transient)& CurrentTrs() const
  {
    return myCurrentTrs;
  }

  //! @return true if iterator points to valid object within collection
  Standard_Boolean More() const
  {
    switch (mySource)
    {
      case IterSource_All:      return myMapIter.More();
      case IterSource_List:     return mySeqIter.More();
      case IterSource_Selected: return mySelIter->MoreSelected();
    }
    return Standard_False;
  }

  //! Go to the next item.
  void Next()
  {
    myCurrentName.Clear();
    myCurrentTrs.Nullify();
    myCurrent.Nullify();
    switch (mySource)
    {
      case IterSource_All:
      {
        myMapIter.Next();
        break;
      }
      case IterSource_List:
      {
        mySeqIter.Next();
        break;
      }
      case IterSource_Selected:
      {
        mySelIter->NextSelected();
        break;
      }
    }
    initCurrent();
  }

private:

  void initCurrent()
  {
    switch (mySource)
    {
      case IterSource_All:
      {
        if (myMapIter.More())
        {
          myCurrentName = myMapIter.Key2();
          myCurrentTrs  = myMapIter.Key1();
          myCurrent     = Handle(AIS_InteractiveObject)::DownCast (myCurrentTrs);
        }
        break;
      }
      case IterSource_List:
      {
        if (mySeqIter.More())
        {
          if (!GetMapOfAIS().IsBound2 (mySeqIter.Value()))
          {
            std::cout << "Error: object " << mySeqIter.Value() << " is not displayed!\n";
            return;
          }
          myCurrentName = mySeqIter.Value();
          myCurrentTrs  = GetMapOfAIS().Find2 (mySeqIter.Value());
          myCurrent     = Handle(AIS_InteractiveObject)::DownCast (myCurrentTrs);
        }
        break;
      }
      case IterSource_Selected:
      {
        if (mySelIter->MoreSelected())
        {
          myCurrentName = GetMapOfAIS().Find1 (mySelIter->SelectedInteractive());
          myCurrent     = mySelIter->SelectedInteractive();
        }
        break;
      }
    }
  }

private:

  enum IterSource
  {
    IterSource_All,
    IterSource_List,
    IterSource_Selected
  };

private:

  Handle(AIS_InteractiveContext) mySelIter;    //!< iterator for current (selected) objects (IterSource_Selected)
  ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName myMapIter; //!< iterator for map of all objects (IterSource_All)
  NCollection_Sequence<TCollection_AsciiString>           mySeq;
  NCollection_Sequence<TCollection_AsciiString>::Iterator mySeqIter;

  TCollection_AsciiString        myCurrentName;//!< current item name
  Handle(Standard_Transient)     myCurrentTrs; //!< current item (as transient object)
  Handle(AIS_InteractiveObject)  myCurrent;    //!< current item

  IterSource                     mySource;     //!< iterated collection

};

//==============================================================================
//function : VInteriorStyle
//purpose  : sets interior style of the a selected or named or displayed shape
//==============================================================================
static int VSetInteriorStyle (Draw_Interpretor& theDI,
                              Standard_Integer  theArgNb,
                              const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aCtx, ViewerTest::CurrentView());
  if (aCtx.IsNull())
  {
    std::cerr << "Error: no active view!\n";
    return 1;
  }

  Standard_Integer anArgIter = 1;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    if (!anUpdateTool.parseRedrawMode (theArgVec[anArgIter]))
    {
      break;
    }
  }
  TCollection_AsciiString aName;
  if (theArgNb - anArgIter == 2)
  {
    aName = theArgVec[anArgIter++];
  }
  else if (theArgNb - anArgIter != 1)
  {
    std::cout << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }
  Aspect_InteriorStyle    anInterStyle = Aspect_IS_SOLID;
  TCollection_AsciiString aStyleArg (theArgVec[anArgIter++]);
  aStyleArg.LowerCase();
  if (aStyleArg == "empty")
  {
    anInterStyle = Aspect_IS_EMPTY;
  }
  else if (aStyleArg == "hollow")
  {
    anInterStyle = Aspect_IS_HOLLOW;
  }
  else if (aStyleArg == "hatch")
  {
    anInterStyle = Aspect_IS_HATCH;
  }
  else if (aStyleArg == "solid")
  {
    anInterStyle = Aspect_IS_SOLID;
  }
  else if (aStyleArg == "hiddenline")
  {
    anInterStyle = Aspect_IS_HIDDENLINE;
  }
  else if (aStyleArg == "point")
  {
    anInterStyle = Aspect_IS_POINT;
  }
  else
  {
    const Standard_Integer anIntStyle = aStyleArg.IntegerValue();
    if (anIntStyle < Aspect_IS_EMPTY
     || anIntStyle > Aspect_IS_POINT)
    {
      std::cout << "Error: style must be within a range [0 (Aspect_IS_EMPTY), "
                << Aspect_IS_POINT << " (Aspect_IS_POINT)]\n";
      return 1;
    }
    anInterStyle = (Aspect_InteriorStyle )anIntStyle;
  }

  if (!aName.IsEmpty()
   && !GetMapOfAIS().IsBound2 (aName))
  {
    std::cout << "Error: object " << aName << " is not displayed!\n";
    return 1;
  }

  for (ViewTest_PrsIter anIter (aName); anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anIO = anIter.Current();
    if (!anIO.IsNull())
    {
      const Handle(Prs3d_Drawer)& aDrawer        = anIO->Attributes();
      Handle(Prs3d_ShadingAspect) aShadingAspect = aDrawer->ShadingAspect();
      Handle(Graphic3d_AspectFillArea3d) aFillAspect = aShadingAspect->Aspect();
      aFillAspect->SetInteriorStyle (anInterStyle);
      if (anInterStyle == Aspect_IS_HATCH
       && aFillAspect->HatchStyle().IsNull())
      {
        aFillAspect->SetHatchStyle (Aspect_HS_VERTICAL);
      }
      aCtx->RecomputePrsOnly (anIO, Standard_False, Standard_True);
    }
  }
  return 0;
}

//! Auxiliary structure for VAspects
struct ViewerTest_AspectsChangeSet
{
  Standard_Integer         ToSetVisibility;
  Standard_Integer         Visibility;

  Standard_Integer         ToSetColor;
  Quantity_Color           Color;

  Standard_Integer         ToSetLineWidth;
  Standard_Real            LineWidth;

  Standard_Integer         ToSetTypeOfLine;
  Aspect_TypeOfLine        TypeOfLine;

  Standard_Integer         ToSetTypeOfMarker;
  Aspect_TypeOfMarker      TypeOfMarker;
  Handle(Image_PixMap)     MarkerImage;

  Standard_Integer         ToSetMarkerSize;
  Standard_Real            MarkerSize;

  Standard_Integer         ToSetTransparency;
  Standard_Real            Transparency;

  Standard_Integer         ToSetMaterial;
  Graphic3d_NameOfMaterial Material;
  TCollection_AsciiString  MatName;

  NCollection_Sequence<TopoDS_Shape> SubShapes;

  Standard_Integer         ToSetShowFreeBoundary;
  Standard_Integer         ToSetFreeBoundaryWidth;
  Standard_Real            FreeBoundaryWidth;
  Standard_Integer         ToSetFreeBoundaryColor;
  Quantity_Color           FreeBoundaryColor;

  Standard_Integer         ToEnableIsoOnTriangulation;

  Standard_Integer         ToSetMaxParamValue;
  Standard_Real            MaxParamValue;

  Standard_Integer         ToSetSensitivity;
  Standard_Integer         SelectionMode;
  Standard_Integer         Sensitivity;

  Standard_Integer         ToSetHatch;
  Standard_Integer         StdHatchStyle;
  TCollection_AsciiString  PathToHatchPattern;

  //! Empty constructor
  ViewerTest_AspectsChangeSet()
  : ToSetVisibility   (0),
    Visibility        (1),
    ToSetColor        (0),
    Color             (DEFAULT_COLOR),
    ToSetLineWidth    (0),
    LineWidth         (1.0),
    ToSetTypeOfLine   (0),
    TypeOfLine        (Aspect_TOL_SOLID),
    ToSetTypeOfMarker (0),
    TypeOfMarker      (Aspect_TOM_PLUS),
    ToSetMarkerSize   (0),
    MarkerSize        (1.0),
    ToSetTransparency (0),
    Transparency      (0.0),
    ToSetMaterial     (0),
    Material          (Graphic3d_NOM_DEFAULT),
    ToSetShowFreeBoundary      (0),
    ToSetFreeBoundaryWidth     (0),
    FreeBoundaryWidth          (1.0),
    ToSetFreeBoundaryColor     (0),
    FreeBoundaryColor          (DEFAULT_FREEBOUNDARY_COLOR),
    ToEnableIsoOnTriangulation (-1),
    ToSetMaxParamValue (0),
    MaxParamValue (500000),
    ToSetSensitivity (0),
    SelectionMode (-1),
    Sensitivity (-1),
    ToSetHatch (0),
    StdHatchStyle (-1)
    {}

  //! @return true if no changes have been requested
  Standard_Boolean IsEmpty() const
  {
    return ToSetVisibility        == 0
        && ToSetLineWidth         == 0
        && ToSetTransparency      == 0
        && ToSetColor             == 0
        && ToSetMaterial          == 0
        && ToSetShowFreeBoundary  == 0
        && ToSetFreeBoundaryColor == 0
        && ToSetFreeBoundaryWidth == 0
        && ToSetMaxParamValue     == 0
        && ToSetSensitivity       == 0
        && ToSetHatch             == 0;
  }

  //! @return true if properties are valid
  Standard_Boolean Validate (const Standard_Boolean theIsSubPart) const
  {
    Standard_Boolean isOk = Standard_True;
    if (Visibility != 0 && Visibility != 1)
    {
      std::cout << "Error: the visibility should be equal to 0 or 1 (0 - invisible; 1 - visible) (specified " << Visibility << ")\n";
      isOk = Standard_False;
    }
    if (LineWidth <= 0.0
     || LineWidth >  10.0)
    {
      std::cout << "Error: the width should be within [1; 10] range (specified " << LineWidth << ")\n";
      isOk = Standard_False;
    }
    if (Transparency < 0.0
     || Transparency > 1.0)
    {
      std::cout << "Error: the transparency should be within [0; 1] range (specified " << Transparency << ")\n";
      isOk = Standard_False;
    }
    if (theIsSubPart
     && ToSetTransparency)
    {
      std::cout << "Error: the transparency can not be defined for sub-part of object!\n";
      isOk = Standard_False;
    }
    if (ToSetMaterial == 1
     && Material == Graphic3d_NOM_DEFAULT)
    {
      std::cout << "Error: unknown material " << MatName << ".\n";
      isOk = Standard_False;
    }
    if (FreeBoundaryWidth <= 0.0
     || FreeBoundaryWidth >  10.0)
    {
      std::cout << "Error: the free boundary width should be within [1; 10] range (specified " << FreeBoundaryWidth << ")\n";
      isOk = Standard_False;
    }
    if (MaxParamValue < 0.0)
    {
      std::cout << "Error: the max parameter value should be greater than zero (specified " << MaxParamValue << ")\n";
      isOk = Standard_False;
    }
    if (Sensitivity <= 0 && ToSetSensitivity)
    {
      std::cout << "Error: sensitivity parameter value should be positive (specified " << Sensitivity << ")\n";
      isOk = Standard_False;
    }
    if (ToSetHatch == 1 && StdHatchStyle < 0 && PathToHatchPattern == "")
    {
      std::cout << "Error: hatch style must be specified\n";
      isOk = Standard_False;
    }
    return isOk;
  }

};

//==============================================================================
//function : VAspects
//purpose  :
//==============================================================================
static Standard_Integer VAspects (Draw_Interpretor& /*theDI*/,
                                  Standard_Integer  theArgNb,
                                  const char**      theArgVec)
{
  TCollection_AsciiString aCmdName (theArgVec[0]);
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aCtx, ViewerTest::CurrentView());
  if (aCtx.IsNull())
  {
    std::cerr << "Error: no active view!\n";
    return 1;
  }

  Standard_Integer anArgIter = 1;
  Standard_Boolean isDefaults = Standard_False;
  NCollection_Sequence<TCollection_AsciiString> aNames;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg = theArgVec[anArgIter];
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else if (!anArg.IsEmpty()
           && anArg.Value (1) != '-')
    {
      aNames.Append (anArg);
    }
    else
    {
      if (anArg == "-defaults")
      {
        isDefaults = Standard_True;
        ++anArgIter;
      }
      break;
    }
  }

  if (!aNames.IsEmpty() && isDefaults)
  {
    std::cout << "Error: wrong syntax. If -defaults is used there should not be any objects' names!\n";
    return 1;
  }

  NCollection_Sequence<ViewerTest_AspectsChangeSet> aChanges;
  aChanges.Append (ViewerTest_AspectsChangeSet());
  ViewerTest_AspectsChangeSet* aChangeSet = &aChanges.ChangeLast();

  // parse syntax of legacy commands
  if (aCmdName == "vsetwidth")
  {
    if (aNames.IsEmpty()
    || !aNames.Last().IsRealValue())
    {
      std::cout << "Error: not enough arguments!\n";
      return 1;
    }
    aChangeSet->ToSetLineWidth = 1;
    aChangeSet->LineWidth = aNames.Last().RealValue();
    aNames.Remove (aNames.Length());
  }
  else if (aCmdName == "vunsetwidth")
  {
    aChangeSet->ToSetLineWidth = -1;
  }
  else if (aCmdName == "vsetcolor")
  {
    if (aNames.IsEmpty())
    {
      std::cout << "Error: not enough arguments!\n";
      return 1;
    }
    aChangeSet->ToSetColor = 1;

    Quantity_NameOfColor aColor = Quantity_NOC_BLACK;
    Standard_Boolean     isOk   = Standard_False;
    if (Quantity_Color::ColorFromName (aNames.Last().ToCString(), aColor))
    {
      aChangeSet->Color = aColor;
      aNames.Remove (aNames.Length());
      isOk = Standard_True;
    }
    else if (aNames.Length() >= 3)
    {
      const TCollection_AsciiString anRgbStr[3] =
      {
        aNames.Value (aNames.Upper() - 2),
        aNames.Value (aNames.Upper() - 1),
        aNames.Value (aNames.Upper() - 0)
      };
      isOk = anRgbStr[0].IsRealValue()
          && anRgbStr[1].IsRealValue()
          && anRgbStr[2].IsRealValue();
      if (isOk)
      {
        Graphic3d_Vec4d anRgb;
        anRgb.x() = anRgbStr[0].RealValue();
        anRgb.y() = anRgbStr[1].RealValue();
        anRgb.z() = anRgbStr[2].RealValue();
        if (anRgb.x() < 0.0 || anRgb.x() > 1.0
         || anRgb.y() < 0.0 || anRgb.y() > 1.0
         || anRgb.z() < 0.0 || anRgb.z() > 1.0)
        {
          std::cout << "Error: RGB color values should be within range 0..1!\n";
          return 1;
        }
        aChangeSet->Color.SetValues (anRgb.x(), anRgb.y(), anRgb.z(), Quantity_TOC_RGB);
        aNames.Remove (aNames.Length());
        aNames.Remove (aNames.Length());
        aNames.Remove (aNames.Length());
      }
    }
    if (!isOk)
    {
      std::cout << "Error: not enough arguments!\n";
      return 1;
    }
  }
  else if (aCmdName == "vunsetcolor")
  {
    aChangeSet->ToSetColor = -1;
  }
  else if (aCmdName == "vsettransparency")
  {
    if (aNames.IsEmpty()
    || !aNames.Last().IsRealValue())
    {
      std::cout << "Error: not enough arguments!\n";
      return 1;
    }
    aChangeSet->ToSetTransparency = 1;
    aChangeSet->Transparency  = aNames.Last().RealValue();
    aNames.Remove (aNames.Length());
  }
  else if (aCmdName == "vunsettransparency")
  {
    aChangeSet->ToSetTransparency = -1;
  }
  else if (aCmdName == "vsetmaterial")
  {
    if (aNames.IsEmpty())
    {
      std::cout << "Error: not enough arguments!\n";
      return 1;
    }
    aChangeSet->ToSetMaterial = 1;
    aChangeSet->MatName  = aNames.Last();
    aChangeSet->Material = Graphic3d_MaterialAspect::MaterialFromName (aChangeSet->MatName.ToCString());
    aNames.Remove (aNames.Length());
  }
  else if (aCmdName == "vunsetmaterial")
  {
    aChangeSet->ToSetMaterial = -1;
  }
  else if (anArgIter >= theArgNb)
  {
    std::cout << "Error: not enough arguments!\n";
    return 1;
  }

  if (!aChangeSet->IsEmpty())
  {
    anArgIter = theArgNb;
  }
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg = theArgVec[anArgIter];
    anArg.LowerCase();
    if (anArg == "-setwidth"
     || anArg == "-setlinewidth")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      aChangeSet->ToSetLineWidth = 1;
      aChangeSet->LineWidth = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (anArg == "-unsetwidth"
          || anArg == "-unsetlinewidth")
    {
      aChangeSet->ToSetLineWidth = -1;
      aChangeSet->LineWidth = 1.0;
    }
    else if (anArg == "-settransp"
          || anArg == "-settransparency")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      aChangeSet->ToSetTransparency = 1;
      aChangeSet->Transparency = Draw::Atof (theArgVec[anArgIter]);
      if (aChangeSet->Transparency >= 0.0
       && aChangeSet->Transparency <= Precision::Confusion())
      {
        aChangeSet->ToSetTransparency = -1;
        aChangeSet->Transparency = 0.0;
      }
    }
    else if (anArg == "-setvis"
          || anArg == "-setvisibility")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }

      aChangeSet->ToSetVisibility = 1;
      aChangeSet->Visibility = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (anArg == "-setalpha")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      aChangeSet->ToSetTransparency = 1;
      aChangeSet->Transparency  = Draw::Atof (theArgVec[anArgIter]);
      if (aChangeSet->Transparency < 0.0
       || aChangeSet->Transparency > 1.0)
      {
        std::cout << "Error: the transparency should be within [0; 1] range (specified " << aChangeSet->Transparency << ")\n";
        return 1;
      }
      aChangeSet->Transparency = 1.0 - aChangeSet->Transparency;
      if (aChangeSet->Transparency >= 0.0
       && aChangeSet->Transparency <= Precision::Confusion())
      {
        aChangeSet->ToSetTransparency = -1;
        aChangeSet->Transparency = 0.0;
      }
    }
    else if (anArg == "-unsettransp"
          || anArg == "-unsettransparency"
          || anArg == "-unsetalpha"
          || anArg == "-opaque")
    {
      aChangeSet->ToSetTransparency = -1;
      aChangeSet->Transparency = 0.0;
    }
    else if (anArg == "-setcolor")
    {
      Standard_Integer aNbComps  = 0;
      Standard_Integer aCompIter = anArgIter + 1;
      for (; aCompIter < theArgNb; ++aCompIter, ++aNbComps)
      {
        if (theArgVec[aCompIter][0] == '-')
        {
          break;
        }
      }
      switch (aNbComps)
      {
        case 1:
        {
          Quantity_NameOfColor aColor = Quantity_NOC_BLACK;
          Standard_CString     aName  = theArgVec[anArgIter + 1];
          if (!Quantity_Color::ColorFromName (aName, aColor))
          {
            std::cout << "Error: unknown color name '" << aName << "'\n";
            return 1;
          }
          aChangeSet->Color = aColor;
          break;
        }
        case 3:
        {
          Graphic3d_Vec3d anRgb;
          anRgb.x() = Draw::Atof (theArgVec[anArgIter + 1]);
          anRgb.y() = Draw::Atof (theArgVec[anArgIter + 2]);
          anRgb.z() = Draw::Atof (theArgVec[anArgIter + 3]);
          if (anRgb.x() < 0.0 || anRgb.x() > 1.0
           || anRgb.y() < 0.0 || anRgb.y() > 1.0
           || anRgb.z() < 0.0 || anRgb.z() > 1.0)
          {
            std::cout << "Error: RGB color values should be within range 0..1!\n";
            return 1;
          }
          aChangeSet->Color.SetValues (anRgb.x(), anRgb.y(), anRgb.z(), Quantity_TOC_RGB);
          break;
        }
        default:
        {
          std::cout << "Error: wrong syntax at " << anArg << "\n";
          return 1;
        }
      }
      aChangeSet->ToSetColor = 1;
      anArgIter += aNbComps;
    }
    else if (anArg == "-setlinetype")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      if (!ViewerTest::ParseLineType (theArgVec[anArgIter], aChangeSet->TypeOfLine))
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }

      aChangeSet->ToSetTypeOfLine = 1;
    }
    else if (anArg == "-unsetlinetype")
    {
      aChangeSet->ToSetTypeOfLine = -1;
    }
    else if (anArg == "-setmarkertype"
          || anArg == "-setpointtype")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      if (!ViewerTest::ParseMarkerType (theArgVec[anArgIter], aChangeSet->TypeOfMarker, aChangeSet->MarkerImage))
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }

      aChangeSet->ToSetTypeOfMarker = 1;
    }
    else if (anArg == "-unsetmarkertype"
          || anArg == "-unsetpointtype")
    {
      aChangeSet->ToSetTypeOfMarker = -1;
    }
    else if (anArg == "-setmarkersize"
          || anArg == "-setpointsize")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      aChangeSet->ToSetMarkerSize = 1;
      aChangeSet->MarkerSize = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (anArg == "-unsetmarkersize"
          || anArg == "-unsetpointsize")
    {
      aChangeSet->ToSetMarkerSize = -1;
      aChangeSet->MarkerSize = 1.0;
    }
    else if (anArg == "-unsetcolor")
    {
      aChangeSet->ToSetColor = -1;
      aChangeSet->Color = DEFAULT_COLOR;
    }
    else if (anArg == "-setmat"
          || anArg == "-setmaterial")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      aChangeSet->ToSetMaterial = 1;
      aChangeSet->MatName  = theArgVec[anArgIter];
      aChangeSet->Material = Graphic3d_MaterialAspect::MaterialFromName (aChangeSet->MatName.ToCString());
    }
    else if (anArg == "-unsetmat"
          || anArg == "-unsetmaterial")
    {
      aChangeSet->ToSetMaterial = -1;
      aChangeSet->Material = Graphic3d_NOM_DEFAULT;
    }
    else if (anArg == "-subshape"
          || anArg == "-subshapes")
    {
      if (isDefaults)
      {
        std::cout << "Error: wrong syntax. -subshapes can not be used together with -defaults call!\n";
        return 1;
      }

      if (aNames.IsEmpty())
      {
        std::cout << "Error: main objects should specified explicitly when -subshapes is used!\n";
        return 1;
      }

      aChanges.Append (ViewerTest_AspectsChangeSet());
      aChangeSet = &aChanges.ChangeLast();

      for (++anArgIter; anArgIter < theArgNb; ++anArgIter)
      {
        Standard_CString aSubShapeName = theArgVec[anArgIter];
        if (*aSubShapeName == '-')
        {
          --anArgIter;
          break;
        }

        TopoDS_Shape aSubShape = DBRep::Get (aSubShapeName);
        if (aSubShape.IsNull())
        {
          std::cerr << "Error: shape " << aSubShapeName << " doesn't found!\n";
          return 1;
        }
        aChangeSet->SubShapes.Append (aSubShape);
      }

      if (aChangeSet->SubShapes.IsEmpty())
      {
        std::cerr << "Error: empty list is specified after -subshapes!\n";
        return 1;
      }
    }
    else if (anArg == "-freeboundary"
          || anArg == "-fb")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      TCollection_AsciiString aValue (theArgVec[anArgIter]);
      aValue.LowerCase();
      if (aValue == "on"
       || aValue == "1")
      {
        aChangeSet->ToSetShowFreeBoundary = 1;
      }
      else if (aValue == "off"
            || aValue == "0")
      {
        aChangeSet->ToSetShowFreeBoundary = -1;
      }
      else
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
    }
    else if (anArg == "-setfreeboundarywidth"
          || anArg == "-setfbwidth")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      aChangeSet->ToSetFreeBoundaryWidth = 1;
      aChangeSet->FreeBoundaryWidth = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (anArg == "-unsetfreeboundarywidth"
          || anArg == "-unsetfbwidth")
    {
      aChangeSet->ToSetFreeBoundaryWidth = -1;
      aChangeSet->FreeBoundaryWidth = 1.0;
    }
    else if (anArg == "-setfreeboundarycolor"
          || anArg == "-setfbcolor")
    {
      Standard_Integer aNbComps  = 0;
      Standard_Integer aCompIter = anArgIter + 1;
      for (; aCompIter < theArgNb; ++aCompIter, ++aNbComps)
      {
        if (theArgVec[aCompIter][0] == '-')
        {
          break;
        }
      }
      switch (aNbComps)
      {
        case 1:
        {
          Quantity_NameOfColor aColor = Quantity_NOC_BLACK;
          Standard_CString     aName  = theArgVec[anArgIter + 1];
          if (!Quantity_Color::ColorFromName (aName, aColor))
          {
            std::cout << "Error: unknown free boundary color name '" << aName << "'\n";
            return 1;
          }
          aChangeSet->FreeBoundaryColor = aColor;
          break;
        }
        case 3:
        {
          Graphic3d_Vec3d anRgb;
          anRgb.x() = Draw::Atof (theArgVec[anArgIter + 1]);
          anRgb.y() = Draw::Atof (theArgVec[anArgIter + 2]);
          anRgb.z() = Draw::Atof (theArgVec[anArgIter + 3]);
          if (anRgb.x() < 0.0 || anRgb.x() > 1.0
           || anRgb.y() < 0.0 || anRgb.y() > 1.0
           || anRgb.z() < 0.0 || anRgb.z() > 1.0)
          {
            std::cout << "Error: free boundary RGB color values should be within range 0..1!\n";
            return 1;
          }
          aChangeSet->FreeBoundaryColor.SetValues (anRgb.x(), anRgb.y(), anRgb.z(), Quantity_TOC_RGB);
          break;
        }
        default:
        {
          std::cout << "Error: wrong syntax at " << anArg << "\n";
          return 1;
        }
      }
      aChangeSet->ToSetFreeBoundaryColor = 1;
      anArgIter += aNbComps;
    }
    else if (anArg == "-unsetfreeboundarycolor"
          || anArg == "-unsetfbcolor")
    {
      aChangeSet->ToSetFreeBoundaryColor = -1;
      aChangeSet->FreeBoundaryColor = DEFAULT_FREEBOUNDARY_COLOR;
    }
    else if (anArg == "-unset")
    {
      aChangeSet->ToSetVisibility = 1;
      aChangeSet->Visibility = 1;
      aChangeSet->ToSetLineWidth = -1;
      aChangeSet->LineWidth = 1.0;
      aChangeSet->ToSetTypeOfLine = -1;
      aChangeSet->TypeOfLine = Aspect_TOL_SOLID;
      aChangeSet->ToSetTypeOfMarker = -1;
      aChangeSet->TypeOfMarker = Aspect_TOM_PLUS;
      aChangeSet->ToSetMarkerSize = -1;
      aChangeSet->MarkerSize = 1.0;
      aChangeSet->ToSetTransparency = -1;
      aChangeSet->Transparency = 0.0;
      aChangeSet->ToSetColor = -1;
      aChangeSet->Color = DEFAULT_COLOR;
      aChangeSet->ToSetMaterial = -1;
      aChangeSet->Material = Graphic3d_NOM_DEFAULT;
      aChangeSet->ToSetShowFreeBoundary = -1;
      aChangeSet->ToSetFreeBoundaryColor = -1;
      aChangeSet->FreeBoundaryColor = DEFAULT_FREEBOUNDARY_COLOR;
      aChangeSet->ToSetFreeBoundaryWidth = -1;
      aChangeSet->FreeBoundaryWidth = 1.0;
      aChangeSet->ToSetHatch = -1;
      aChangeSet->StdHatchStyle = -1;
      aChangeSet->PathToHatchPattern.Clear();
    }
    else if (anArg == "-isoontriangulation"
          || anArg == "-isoontriang")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      TCollection_AsciiString aValue (theArgVec[anArgIter]);
      aValue.LowerCase();
      if (aValue == "on"
        || aValue == "1")
      {
        aChangeSet->ToEnableIsoOnTriangulation = 1;
      }
      else if (aValue == "off"
        || aValue == "0")
      {
        aChangeSet->ToEnableIsoOnTriangulation = 0;
      }
      else
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
    }
    else if (anArg == "-setmaxparamvalue")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      aChangeSet->ToSetMaxParamValue = 1;
      aChangeSet->MaxParamValue = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (anArg == "-setsensitivity")
    {
      if (isDefaults)
      {
        std::cout << "Error: wrong syntax. -setSensitivity can not be used together with -defaults call!\n";
        return 1;
      }

      if (aNames.IsEmpty())
      {
        std::cout << "Error: object and selection mode should specified explicitly when -setSensitivity is used!\n";
        return 1;
      }

      if (anArgIter + 2 >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      aChangeSet->ToSetSensitivity = 1;
      aChangeSet->SelectionMode = Draw::Atoi (theArgVec[++anArgIter]);
      aChangeSet->Sensitivity = Draw::Atoi (theArgVec[++anArgIter]);
    }
    else if (anArg == "-sethatch")
    {
      if (isDefaults)
      {
        std::cout << "Error: wrong syntax. -setHatch can not be used together with -defaults call!\n";
        return 1;
      }

      if (aNames.IsEmpty())
      {
        std::cout << "Error: object should be specified explicitly when -setHatch is used!\n";
        return 1;
      }

      aChangeSet->ToSetHatch = 1;
      TCollection_AsciiString anArgHatch (theArgVec[++anArgIter]);
      if (anArgHatch.Length() <= 2)
      {
        const Standard_Integer anIntStyle = Draw::Atoi (anArgHatch.ToCString());
        if (anIntStyle < 0
         || anIntStyle >= Aspect_HS_NB)
        {
          std::cout << "Error: hatch style is out of range [0, " << (Aspect_HS_NB - 1) << "]!\n";
          return 1;
        }
        aChangeSet->StdHatchStyle = anIntStyle;
      }
      else
      {
        aChangeSet->PathToHatchPattern = anArgHatch;
      }
    }
    else
    {
      std::cout << "Error: wrong syntax at " << anArg << "\n";
      return 1;
    }
  }

  Standard_Boolean isFirst = Standard_True;
  for (NCollection_Sequence<ViewerTest_AspectsChangeSet>::Iterator aChangesIter (aChanges);
       aChangesIter.More(); aChangesIter.Next())
  {
    if (!aChangesIter.Value().Validate (!isFirst))
    {
      return 1;
    }
    isFirst = Standard_False;
  }

  // special case for -defaults parameter.
  // all changed values will be set to DefaultDrawer.
  if (isDefaults)
  {
    const Handle(Prs3d_Drawer)& aDrawer = aCtx->DefaultDrawer();

    if (aChangeSet->ToSetLineWidth != 0)
    {
      aDrawer->LineAspect()->SetWidth (aChangeSet->LineWidth);
      aDrawer->WireAspect()->SetWidth (aChangeSet->LineWidth);
      aDrawer->UnFreeBoundaryAspect()->SetWidth (aChangeSet->LineWidth);
      aDrawer->SeenLineAspect()->SetWidth (aChangeSet->LineWidth);
    }
    if (aChangeSet->ToSetColor != 0)
    {
      aDrawer->ShadingAspect()->SetColor        (aChangeSet->Color);
      aDrawer->LineAspect()->SetColor           (aChangeSet->Color);
      aDrawer->UnFreeBoundaryAspect()->SetColor (aChangeSet->Color);
      aDrawer->SeenLineAspect()->SetColor       (aChangeSet->Color);
      aDrawer->WireAspect()->SetColor           (aChangeSet->Color);
      aDrawer->PointAspect()->SetColor          (aChangeSet->Color);
    }
    if (aChangeSet->ToSetTypeOfLine != 0)
    {
      aDrawer->LineAspect()->SetTypeOfLine           (aChangeSet->TypeOfLine);
      aDrawer->WireAspect()->SetTypeOfLine           (aChangeSet->TypeOfLine);
      aDrawer->FreeBoundaryAspect()->SetTypeOfLine   (aChangeSet->TypeOfLine);
      aDrawer->UnFreeBoundaryAspect()->SetTypeOfLine (aChangeSet->TypeOfLine);
      aDrawer->SeenLineAspect()->SetTypeOfLine       (aChangeSet->TypeOfLine);
    }
    if (aChangeSet->ToSetTypeOfMarker != 0)
    {
      aDrawer->PointAspect()->SetTypeOfMarker (aChangeSet->TypeOfMarker);
      aDrawer->PointAspect()->Aspect()->SetMarkerImage (aChangeSet->MarkerImage.IsNull()
                                                      ? Handle(Graphic3d_MarkerImage)()
                                                      : new Graphic3d_MarkerImage (aChangeSet->MarkerImage));
    }
    if (aChangeSet->ToSetMarkerSize != 0)
    {
      aDrawer->PointAspect()->SetScale (aChangeSet->MarkerSize);
    }
    if (aChangeSet->ToSetTransparency != 0)
    {
      aDrawer->ShadingAspect()->SetTransparency (aChangeSet->Transparency);
    }
    if (aChangeSet->ToSetMaterial != 0)
    {
      aDrawer->ShadingAspect()->SetMaterial (aChangeSet->Material);
    }
    if (aChangeSet->ToSetShowFreeBoundary == 1)
    {
      aDrawer->SetFreeBoundaryDraw (Standard_True);
    }
    else if (aChangeSet->ToSetShowFreeBoundary == -1)
    {
      aDrawer->SetFreeBoundaryDraw (Standard_False);
    }
    if (aChangeSet->ToSetFreeBoundaryWidth != 0)
    {
      aDrawer->FreeBoundaryAspect()->SetWidth (aChangeSet->FreeBoundaryWidth);
    }
    if (aChangeSet->ToSetFreeBoundaryColor != 0)
    {
      aDrawer->FreeBoundaryAspect()->SetColor (aChangeSet->FreeBoundaryColor);
    }
    if (aChangeSet->ToEnableIsoOnTriangulation != -1)
    {
      aDrawer->SetIsoOnTriangulation (aChangeSet->ToEnableIsoOnTriangulation == 1);
    }
    if (aChangeSet->ToSetMaxParamValue != 0)
    {
      aDrawer->SetMaximalParameterValue (aChangeSet->MaxParamValue);
    }

    // redisplay all objects in context
    for (ViewTest_PrsIter aPrsIter (aNames); aPrsIter.More(); aPrsIter.Next())
    {
      Handle(AIS_InteractiveObject)  aPrs = aPrsIter.Current();
      if (!aPrs.IsNull())
      {
        aCtx->Redisplay (aPrs, Standard_False);
      }
    }
    return 0;
  }

  for (ViewTest_PrsIter aPrsIter (aNames); aPrsIter.More(); aPrsIter.Next())
  {
    const TCollection_AsciiString& aName   = aPrsIter.CurrentName();
    Handle(AIS_InteractiveObject)  aPrs    = aPrsIter.Current();
    Handle(Prs3d_Drawer)           aDrawer = aPrs->Attributes();
    Handle(AIS_ColoredShape) aColoredPrs;
    Standard_Boolean toDisplay = Standard_False;
    Standard_Boolean toRedisplay = Standard_False;
    if (aChanges.Length() > 1 || aChangeSet->ToSetVisibility == 1)
    {
      Handle(AIS_Shape) aShapePrs = Handle(AIS_Shape)::DownCast (aPrs);
      if (aShapePrs.IsNull())
      {
        std::cout << "Error: an object " << aName << " is not an AIS_Shape presentation!\n";
        return 1;
      }
      aColoredPrs = Handle(AIS_ColoredShape)::DownCast (aShapePrs);
      if (aColoredPrs.IsNull())
      {
        aColoredPrs = new AIS_ColoredShape (aShapePrs);
        if (aShapePrs->HasDisplayMode())
        {
          aColoredPrs->SetDisplayMode (aShapePrs->DisplayMode());
        }
        aColoredPrs->SetLocalTransformation (aShapePrs->LocalTransformation());
        aCtx->Remove (aShapePrs, Standard_False);
        GetMapOfAIS().UnBind2 (aName);
        GetMapOfAIS().Bind (aColoredPrs, aName);
        toDisplay = Standard_True;
        aShapePrs = aColoredPrs;
        aPrs      = aColoredPrs;
      }
    }

    if (!aPrs.IsNull())
    {
      NCollection_Sequence<ViewerTest_AspectsChangeSet>::Iterator aChangesIter (aChanges);
      aChangeSet = &aChangesIter.ChangeValue();
      if (aChangeSet->ToSetVisibility == 1)
      {
        Handle(AIS_ColoredDrawer) aColDrawer = aColoredPrs->CustomAspects (aColoredPrs->Shape());
        aColDrawer->SetHidden (aChangeSet->Visibility == 0);
      }
      else if (aChangeSet->ToSetMaterial == 1)
      {
        aCtx->SetMaterial (aPrs, aChangeSet->Material, Standard_False);
      }
      else if (aChangeSet->ToSetMaterial == -1)
      {
        aCtx->UnsetMaterial (aPrs, Standard_False);
      }
      if (aChangeSet->ToSetColor == 1)
      {
        aCtx->SetColor (aPrs, aChangeSet->Color, Standard_False);
      }
      else if (aChangeSet->ToSetColor == -1)
      {
        aCtx->UnsetColor (aPrs, Standard_False);
      }
      if (aChangeSet->ToSetTransparency == 1)
      {
        aCtx->SetTransparency (aPrs, aChangeSet->Transparency, Standard_False);
      }
      else if (aChangeSet->ToSetTransparency == -1)
      {
        aCtx->UnsetTransparency (aPrs, Standard_False);
      }
      if (aChangeSet->ToSetLineWidth == 1)
      {
        aCtx->SetWidth (aPrs, aChangeSet->LineWidth, Standard_False);
      }
      else if (aChangeSet->ToSetLineWidth == -1)
      {
        aCtx->UnsetWidth (aPrs, Standard_False);
      }
      else if (aChangeSet->ToEnableIsoOnTriangulation != -1)
      {
        aCtx->IsoOnTriangulation (aChangeSet->ToEnableIsoOnTriangulation == 1, aPrs);
        toRedisplay = Standard_True;
      }
      else if (aChangeSet->ToSetSensitivity != 0)
      {
        aCtx->SetSelectionSensitivity (aPrs, aChangeSet->SelectionMode, aChangeSet->Sensitivity);
      }
      if (!aDrawer.IsNull())
      {
        if (aChangeSet->ToSetShowFreeBoundary == 1)
        {
          aDrawer->SetFreeBoundaryDraw (Standard_True);
          toRedisplay = Standard_True;
        }
        else if (aChangeSet->ToSetShowFreeBoundary == -1)
        {
          aDrawer->SetFreeBoundaryDraw (Standard_False);
          toRedisplay = Standard_True;
        }
        if (aChangeSet->ToSetFreeBoundaryWidth != 0)
        {
          Handle(Prs3d_LineAspect) aBoundaryAspect =
              new Prs3d_LineAspect (Quantity_NOC_RED, Aspect_TOL_SOLID, 1.0);
          *aBoundaryAspect->Aspect() = *aDrawer->FreeBoundaryAspect()->Aspect();
          aBoundaryAspect->SetWidth (aChangeSet->FreeBoundaryWidth);
          aDrawer->SetFreeBoundaryAspect (aBoundaryAspect);
          toRedisplay = Standard_True;
        }
        if (aChangeSet->ToSetFreeBoundaryColor != 0)
        {
          Handle(Prs3d_LineAspect) aBoundaryAspect =
              new Prs3d_LineAspect (Quantity_NOC_RED, Aspect_TOL_SOLID, 1.0);
          *aBoundaryAspect->Aspect() = *aDrawer->FreeBoundaryAspect()->Aspect();
          aBoundaryAspect->SetColor (aChangeSet->FreeBoundaryColor);
          aDrawer->SetFreeBoundaryAspect (aBoundaryAspect);
          toRedisplay = Standard_True;
        }
        if (aChangeSet->ToSetTypeOfLine != 0)
        {
          aDrawer->LineAspect()->SetTypeOfLine           (aChangeSet->TypeOfLine);
          aDrawer->WireAspect()->SetTypeOfLine           (aChangeSet->TypeOfLine);
          aDrawer->FreeBoundaryAspect()->SetTypeOfLine   (aChangeSet->TypeOfLine);
          aDrawer->UnFreeBoundaryAspect()->SetTypeOfLine (aChangeSet->TypeOfLine);
          aDrawer->SeenLineAspect()->SetTypeOfLine       (aChangeSet->TypeOfLine);
          toRedisplay = Standard_True;
        }
        if (aChangeSet->ToSetTypeOfMarker != 0)
        {
          Handle(Prs3d_PointAspect) aMarkerAspect = new Prs3d_PointAspect (Aspect_TOM_PLUS, Quantity_NOC_YELLOW, 1.0);
          *aMarkerAspect->Aspect() = *aDrawer->PointAspect()->Aspect();
          aMarkerAspect->SetTypeOfMarker (aChangeSet->TypeOfMarker);
          aMarkerAspect->Aspect()->SetMarkerImage (aChangeSet->MarkerImage.IsNull()
                                                 ? Handle(Graphic3d_MarkerImage)()
                                                 : new Graphic3d_MarkerImage (aChangeSet->MarkerImage));
          aDrawer->SetPointAspect (aMarkerAspect);
          toRedisplay = Standard_True;
        }
        if (aChangeSet->ToSetMarkerSize != 0)
        {
          Handle(Prs3d_PointAspect) aMarkerAspect = new Prs3d_PointAspect (Aspect_TOM_PLUS, Quantity_NOC_YELLOW, 1.0);
          *aMarkerAspect->Aspect() = *aDrawer->PointAspect()->Aspect();
          aMarkerAspect->SetScale (aChangeSet->MarkerSize);
          aDrawer->SetPointAspect (aMarkerAspect);
          toRedisplay = Standard_True;
        }
        if (aChangeSet->ToSetMaxParamValue != 0)
        {
          aDrawer->SetMaximalParameterValue (aChangeSet->MaxParamValue);
        }
        if (aChangeSet->ToSetHatch != 0)
        {
          if (!aDrawer->HasOwnShadingAspect())
          {
            aDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
            *aDrawer->ShadingAspect()->Aspect() = *aCtx->DefaultDrawer()->ShadingAspect()->Aspect();
          }

          Handle(Graphic3d_AspectFillArea3d) anAsp = aDrawer->ShadingAspect()->Aspect();
          if (aChangeSet->ToSetHatch == -1)
          {
            anAsp->SetInteriorStyle (Aspect_IS_SOLID);
          }
          else
          {
            anAsp->SetInteriorStyle (Aspect_IS_HATCH);
            if (!aChangeSet->PathToHatchPattern.IsEmpty())
            {
              Handle(Image_AlienPixMap) anImage = new Image_AlienPixMap();
              if (anImage->Load (TCollection_AsciiString (aChangeSet->PathToHatchPattern.ToCString())))
              {
                anAsp->SetHatchStyle (new Graphic3d_HatchStyle (anImage));
              }
              else
              {
                std::cout << "Error: cannot load the following image: " << aChangeSet->PathToHatchPattern << std::endl;
                return 1;
              }
            }
            else if (aChangeSet->StdHatchStyle != -1)
            {
              anAsp->SetHatchStyle (new Graphic3d_HatchStyle ((Aspect_HatchStyle)aChangeSet->StdHatchStyle));
            }
          }
          toRedisplay = Standard_True;
        }
      }

      for (aChangesIter.Next(); aChangesIter.More(); aChangesIter.Next())
      {
        aChangeSet = &aChangesIter.ChangeValue();
        for (NCollection_Sequence<TopoDS_Shape>::Iterator aSubShapeIter (aChangeSet->SubShapes);
             aSubShapeIter.More(); aSubShapeIter.Next())
        {
          const TopoDS_Shape& aSubShape = aSubShapeIter.Value();
          if (aChangeSet->ToSetVisibility == 1)
          {
            Handle(AIS_ColoredDrawer) aCurColDrawer = aColoredPrs->CustomAspects (aSubShape);
            aCurColDrawer->SetHidden (aChangeSet->Visibility == 0);
          }
          if (aChangeSet->ToSetColor == 1)
          {
            aColoredPrs->SetCustomColor (aSubShape, aChangeSet->Color);
          }
          if (aChangeSet->ToSetLineWidth == 1)
          {
            aColoredPrs->SetCustomWidth (aSubShape, aChangeSet->LineWidth);
          }
          if (aChangeSet->ToSetColor     == -1
           || aChangeSet->ToSetLineWidth == -1)
          {
            aColoredPrs->UnsetCustomAspects (aSubShape, Standard_True);
          }
          if (aChangeSet->ToSetMaxParamValue != 0)
          {
            Handle(AIS_ColoredDrawer) aCurColDrawer = aColoredPrs->CustomAspects (aSubShape);
            aCurColDrawer->SetMaximalParameterValue (aChangeSet->MaxParamValue);
          }
          if (aChangeSet->ToSetSensitivity != 0)
          {
            aCtx->SetSelectionSensitivity (aPrs, aChangeSet->SelectionMode, aChangeSet->Sensitivity);
          }
        }
      }
      if (toDisplay)
      {
        aCtx->Display (aPrs, Standard_False);
      }
      if (toRedisplay)
      {
        aCtx->Redisplay (aPrs, Standard_False);
      }
      else if (!aColoredPrs.IsNull())
      {
        aCtx->Redisplay (aColoredPrs, Standard_False);
      }
    }
  }
  return 0;
}

//==============================================================================
//function : VDonly2
//author   : ege
//purpose  : Display only a selected or named  object
//           if there is no selected or named object s, nothing is done
//==============================================================================
static int VDonly2 (Draw_Interpretor& ,
                    Standard_Integer  theArgNb,
                    const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aCtx, ViewerTest::CurrentView());
  if (aCtx.IsNull())
  {
    std::cerr << "Error: no active view!\n";
    return 1;
  }

  Standard_DISABLE_DEPRECATION_WARNINGS
  if (aCtx->HasOpenedContext())
  {
    aCtx->CloseLocalContext();
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  Standard_Integer anArgIter = 1;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    if (!anUpdateTool.parseRedrawMode (theArgVec[anArgIter]))
    {
      break;
    }
  }

  NCollection_Map<Handle(Standard_Transient)> aDispSet;
  if (anArgIter >= theArgNb)
  {
    // display only selected objects
    if (aCtx->NbSelected() < 1)
    {
      return 0;
    }

    for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
    {
      aDispSet.Add (aCtx->SelectedInteractive());
    }
  }
  else
  {
    // display only specified objects
    for (; anArgIter < theArgNb; ++anArgIter)
    {
      TCollection_AsciiString aName = theArgVec[anArgIter];
      if (GetMapOfAIS().IsBound2 (aName))
      {
        const Handle(AIS_InteractiveObject) aShape = Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (aName));
        if (!aShape.IsNull())
        {
          aCtx->Display (aShape, Standard_False);
          aDispSet.Add (aShape);
        }
      }
    }
  }

  // weed out other objects
  for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS()); anIter.More(); anIter.Next())
  {
    if (aDispSet.Contains (anIter.Key1()))
    {
      continue;
    }

    const Handle(AIS_InteractiveObject) aShape = Handle(AIS_InteractiveObject)::DownCast (anIter.Key1());
    if (aShape.IsNull())
    {
      aCtx->Erase (aShape, Standard_False);
    }
  }
  return 0;
}

//==============================================================================
//function : VRemove
//purpose  : Removes selected or named objects.
//           If there is no selected or named objects,
//           all objects in the viewer can be removed with argument -all.
//           If -context is in arguments, the object is not deleted from the map of
//           objects (deleted only from the current context).
//==============================================================================
int VRemove (Draw_Interpretor& theDI,
             Standard_Integer  theArgNb,
             const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aCtx, ViewerTest::CurrentView());
  if (aCtx.IsNull())
  {
    std::cerr << "Error: no active view!\n";
    return 1;
  }

  Standard_Boolean isContextOnly = Standard_False;
  Standard_Boolean toRemoveAll   = Standard_False;
  Standard_Boolean toPrintInfo   = Standard_True;
  Standard_Boolean toRemoveLocal = Standard_False;

  Standard_Integer anArgIter = 1;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg = theArgVec[anArgIter];
    anArg.LowerCase();
    if (anArg == "-context")
    {
      isContextOnly = Standard_True;
    }
    else if (anArg == "-all")
    {
      toRemoveAll = Standard_True;
    }
    else if (anArg == "-noinfo")
    {
      toPrintInfo = Standard_False;
    }
    else if (anArg == "-local")
    {
      toRemoveLocal = Standard_True;
    }
    else if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else
    {
      break;
    }
  }
  if (toRemoveAll
   && anArgIter < theArgNb)
  {
    std::cerr << "Error: wrong syntax!\n";
    return 1;
  }

  Standard_DISABLE_DEPRECATION_WARNINGS
  if (toRemoveLocal && !aCtx->HasOpenedContext())
  {
    std::cerr << "Error: local selection context is not open.\n";
    return 1;
  }
  else if (!toRemoveLocal && aCtx->HasOpenedContext())
  {
    aCtx->CloseAllContexts (Standard_False);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  NCollection_List<TCollection_AsciiString> anIONameList;
  if (toRemoveAll)
  {
    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
         anIter.More(); anIter.Next())
    {
      anIONameList.Append (anIter.Key2());
    }
  }
  else if (anArgIter < theArgNb) // removed objects names are in argument list
  {
    for (; anArgIter < theArgNb; ++anArgIter)
    {
      TCollection_AsciiString aName = theArgVec[anArgIter];
      if (!GetMapOfAIS().IsBound2 (aName))
      {
        theDI << aName.ToCString() << " was not bound to some object.\n";
        continue;
      }

      const Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (aName));
      if (anIO->GetContext() != aCtx)
      {
        theDI << aName.ToCString() << " was not displayed in current context.\n";
        theDI << "Please activate view with this object displayed and try again.\n";
        continue;
      }

      anIONameList.Append (aName);
      continue;
    }
  }
  else if (aCtx->NbSelected() > 0)
  {
    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
         anIter.More(); anIter.Next())
    {
      const Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (anIter.Key1());
      if (!aCtx->IsSelected (anIO))
      {
        continue;
      }

      anIONameList.Append (anIter.Key2());
      continue;
    }
  }

  // Unbind all removed objects from the map of displayed IO.
  for (NCollection_List<TCollection_AsciiString>::Iterator anIter (anIONameList);
       anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject) anIO  = Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (anIter.Value()));
    aCtx->Remove (anIO, Standard_False);
    if (toPrintInfo)
    {
      theDI << anIter.Value().ToCString() << " was removed\n";
    }
    if (!isContextOnly)
    {
      GetMapOfAIS().UnBind2 (anIter.Value());
    }
  }

  // Close local context if it is empty
  TColStd_MapOfTransient aLocalIO;
  Standard_DISABLE_DEPRECATION_WARNINGS
  if (aCtx->HasOpenedContext()
   && !aCtx->LocalContext()->DisplayedObjects (aLocalIO))
  {
    aCtx->CloseAllContexts (Standard_False);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  return 0;
}

//==============================================================================
//function : VErase
//purpose  : Erase some selected or named objects
//           if there is no selected or named objects, the whole viewer is erased
//==============================================================================
int VErase (Draw_Interpretor& theDI,
            Standard_Integer  theArgNb,
            const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx  = ViewerTest::GetAISContext();
  const Handle(V3d_View)&               aView = ViewerTest::CurrentView();
  ViewerTest_AutoUpdater anUpdateTool (aCtx, aView);
  if (aCtx.IsNull())
  {
    std::cerr << "Error: no active view!\n";
    return 1;
  }

  const Standard_Boolean toEraseAll = TCollection_AsciiString (theArgNb > 0 ? theArgVec[0] : "") == "veraseall";

  Standard_Integer anArgIter = 1;
  Standard_Boolean toEraseLocal  = Standard_False;
  Standard_Boolean toEraseInView = Standard_False;
  TColStd_SequenceOfAsciiString aNamesOfEraseIO;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArgCase (theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArgCase))
    {
      continue;
    }
    else if (anArgCase == "-local")
    {
      toEraseLocal = Standard_True;
    }
    else if (anArgCase == "-view"
          || anArgCase == "-inview")
    {
      toEraseInView = Standard_True;
    }
    else
    {
      aNamesOfEraseIO.Append (theArgVec[anArgIter]);
    }
  }

  if (!aNamesOfEraseIO.IsEmpty() && toEraseAll)
  {
    std::cerr << "Error: wrong syntax, " << theArgVec[0] << " too much arguments.\n";
    return 1;
  }

  Standard_DISABLE_DEPRECATION_WARNINGS
  if (toEraseLocal && !aCtx->HasOpenedContext())
  {
    std::cerr << "Error: local selection context is not open.\n";
    return 1;
  }
  else if (!toEraseLocal && aCtx->HasOpenedContext())
  {
    aCtx->CloseAllContexts (Standard_False);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  if (!aNamesOfEraseIO.IsEmpty())
  {
    // Erase named objects
    for (Standard_Integer anIter = 1; anIter <= aNamesOfEraseIO.Length(); ++anIter)
    {
      TCollection_AsciiString aName = aNamesOfEraseIO.Value (anIter);
      if (!GetMapOfAIS().IsBound2 (aName))
      {
        continue;
      }

      const Handle(Standard_Transient)    anObj = GetMapOfAIS().Find2 (aName);
      const Handle(AIS_InteractiveObject) anIO  = Handle(AIS_InteractiveObject)::DownCast (anObj);
      theDI << aName.ToCString() << " ";
      if (!anIO.IsNull())
      {
        if (toEraseInView)
        {
          aCtx->SetViewAffinity (anIO, aView, Standard_False);
        }
        else
        {
          aCtx->Erase (anIO, Standard_False);
        }
      }
    }
  }
  else if (!toEraseAll && aCtx->NbSelected() > 0)
  {
    // Erase selected objects
    const Standard_Boolean aHasOpenedContext = aCtx->HasOpenedContext();
    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
         anIter.More(); anIter.Next())
    {
      const Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (anIter.Key1());
      if (!anIO.IsNull()
       && aCtx->IsSelected (anIO))
      {
        theDI << anIter.Key2().ToCString() << " ";
        if (toEraseInView)
        {
          aCtx->SetViewAffinity (anIO, aView, Standard_False);
        }
        else if (aHasOpenedContext)
        {
          aCtx->Erase (anIO, Standard_False);
        }
      }
    }

    if (!toEraseInView)
    {
      aCtx->EraseSelected (Standard_False);
    }
  }
  else
  {
    // Erase all objects
    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
         anIter.More(); anIter.Next())
    {
      const Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (anIter.Key1());
      if (!anIO.IsNull())
      {
        if (toEraseInView)
        {
          aCtx->SetViewAffinity (anIO, aView, Standard_False);
        }
        else
        {
          aCtx->Erase (anIO, Standard_False);
        }
      }
    }
  }

  return 0;
}

//==============================================================================
//function : VDisplayAll
//author   : ege
//purpose  : Display all the objects of the Map
//==============================================================================
static int VDisplayAll (Draw_Interpretor& ,
                        Standard_Integer  theArgNb,
                        const char**      theArgVec)

{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aCtx, ViewerTest::CurrentView());
  if (aCtx.IsNull())
  {
    std::cerr << "Error: no active view!\n";
    return 1;
  }

  Standard_Integer anArgIter = 1;
  Standard_Boolean toDisplayLocal = Standard_False;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArgCase (theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anArgCase == "-local")
    {
      toDisplayLocal = Standard_True;
    }
    else if (anUpdateTool.parseRedrawMode (anArgCase))
    {
      continue;
    }
    else
    {
      break;
    }
  }
  if (anArgIter < theArgNb)
  {
    std::cout << theArgVec[0] << "Error: wrong syntax\n";
    return 1;
  }

  Standard_DISABLE_DEPRECATION_WARNINGS
  if (toDisplayLocal && !aCtx->HasOpenedContext())
  {
    std::cerr << "Error: local selection context is not open.\n";
    return 1;
  }
  else if (!toDisplayLocal && aCtx->HasOpenedContext())
  {
    aCtx->CloseLocalContext (Standard_False);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
       anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject) aShape = Handle(AIS_InteractiveObject)::DownCast (anIter.Key1());
    aCtx->Erase (aShape, Standard_False);
  }

  for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
       anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject) aShape = Handle(AIS_InteractiveObject)::DownCast (anIter.Key1());
    aCtx->Display (aShape, Standard_False);
  }
  return 0;
}

//! Auxiliary method to check if presentation exists
inline Standard_Integer checkMode (const Handle(AIS_InteractiveContext)& theCtx,
                                   const Handle(AIS_InteractiveObject)&  theIO,
                                   const Standard_Integer                theMode)
{
  if (theIO.IsNull() || theCtx.IsNull())
  {
    return -1;
  }

  if (theMode != -1)
  {
    if (theCtx->MainPrsMgr()->HasPresentation (theIO, theMode))
    {
      return theMode;
    }
  }
  else if (theCtx->MainPrsMgr()->HasPresentation (theIO, theIO->DisplayMode()))
  {
    return theIO->DisplayMode();
  }
  else if (theCtx->MainPrsMgr()->HasPresentation (theIO, theCtx->DisplayMode()))
  {
    return theCtx->DisplayMode();
  }

  return -1;
}

enum ViewerTest_BndAction
{
  BndAction_Hide,
  BndAction_Show,
  BndAction_Print
};

//! Auxiliary method to print bounding box of presentation
inline void bndPresentation (Draw_Interpretor&                         theDI,
                             const Handle(PrsMgr_PresentationManager)& theMgr,
                             const Handle(AIS_InteractiveObject)&      theObj,
                             const Standard_Integer                    theDispMode,
                             const TCollection_AsciiString&            theName,
                             const ViewerTest_BndAction                theAction,
                             const Handle(Prs3d_Drawer)&               theStyle)
{
  switch (theAction)
  {
    case BndAction_Hide:
    {
      theMgr->Unhighlight (theObj);
      break;
    }
    case BndAction_Show:
    {
      theMgr->Color (theObj, theStyle, theDispMode);
      break;
    }
    case BndAction_Print:
    {
      Bnd_Box aBox;
      for (PrsMgr_Presentations::Iterator aPrsIter (theObj->Presentations()); aPrsIter.More(); aPrsIter.Next())
      {
        if (aPrsIter.Value().Mode() != theDispMode)
          continue;

        aBox = aPrsIter.Value().Presentation()->Presentation()->MinMaxValues();
      }
      gp_Pnt aMin = aBox.CornerMin();
      gp_Pnt aMax = aBox.CornerMax();
      theDI << theName  << "\n"
            << aMin.X() << " " << aMin.Y() << " " << aMin.Z() << " "
            << aMax.X() << " " << aMax.Y() << " " << aMax.Z() << "\n";
      break;
    }
  }
}

//==============================================================================
//function : VBounding
//purpose  :
//==============================================================================
int VBounding (Draw_Interpretor& theDI,
               Standard_Integer  theArgNb,
               const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aCtx, ViewerTest::CurrentView());
  if (aCtx.IsNull())
  {
    std::cout << "Error: no active view!\n";
    return 1;
  }

  ViewerTest_BndAction anAction = BndAction_Show;
  Standard_Integer     aMode    = -1;

  Handle(Prs3d_Drawer) aStyle;

  Standard_Integer anArgIter = 1;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-print")
    {
      anAction = BndAction_Print;
    }
    else if (anArg == "-show")
    {
      anAction = BndAction_Show;
    }
    else if (anArg == "-hide")
    {
      anAction = BndAction_Hide;
    }
    else if (anArg == "-mode")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      aMode = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (!anUpdateTool.parseRedrawMode (anArg))
    {
      break;
    }
  }

  if (anAction == BndAction_Show)
  {
    aStyle = new Prs3d_Drawer();
    aStyle->SetMethod (Aspect_TOHM_BOUNDBOX);
    aStyle->SetColor  (Quantity_NOC_GRAY99);
  }

  Standard_Integer aHighlightedMode = -1;
  if (anArgIter < theArgNb)
  {
    // has a list of names
    for (; anArgIter < theArgNb; ++anArgIter)
    {
      TCollection_AsciiString aName = theArgVec[anArgIter];
      if (!GetMapOfAIS().IsBound2 (aName))
      {
        std::cout << "Error: presentation " << aName << " does not exist\n";
        return 1;
      }

      Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (aName));
      aHighlightedMode = checkMode (aCtx, anIO, aMode);
      if (aHighlightedMode == -1)
      {
        std::cout << "Error: object " << aName << " has no presentation with mode " << aMode << std::endl;
        return 1;
      }
      bndPresentation (theDI, aCtx->MainPrsMgr(), anIO, aHighlightedMode, aName, anAction, aStyle);
    }
  }
  else if (aCtx->NbSelected() > 0)
  {
    // remove all currently selected objects
    for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
    {
      Handle(AIS_InteractiveObject) anIO = aCtx->SelectedInteractive();
      aHighlightedMode = checkMode (aCtx, anIO, aMode);
      if (aHighlightedMode != -1)
      {
        bndPresentation (theDI, aCtx->MainPrsMgr(), anIO, aHighlightedMode,
          GetMapOfAIS().IsBound1 (anIO) ? GetMapOfAIS().Find1 (anIO) : "", anAction, aStyle);
      }
    }
  }
  else
  {
    // all objects
    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
         anIter.More(); anIter.Next())
    {
      Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (anIter.Key1());
      aHighlightedMode = checkMode (aCtx, anIO, aMode);
      if (aHighlightedMode != -1)
      {
        bndPresentation (theDI, aCtx->MainPrsMgr(), anIO, aHighlightedMode, anIter.Key2(), anAction, aStyle);
      }
    }
  }
  return 0;
}

//==============================================================================
//function : VTexture
//purpose  :
//==============================================================================
Standard_Integer VTexture (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "Error: no active view!\n";
    return 1;
  }

  int  toModulate     = -1;
  bool toSetFilter    = false;
  bool toSetAniso     = false;
  bool toSetTrsfAngle = false;
  bool toSetTrsfTrans = false;
  bool toSetTrsfScale = false;
  Standard_ShortReal aTrsfRotAngle = 0.0f;
  Graphic3d_Vec2 aTrsfTrans (0.0f, 0.0f);
  Graphic3d_Vec2 aTrsfScale (1.0f, 1.0f);
  Graphic3d_TypeOfTextureFilter      aFilter       = Graphic3d_TOTF_NEAREST;
  Graphic3d_LevelOfTextureAnisotropy anAnisoFilter = Graphic3d_LOTA_OFF;

  Handle(AIS_Shape) aTexturedIO;
  Handle(Graphic3d_TextureSet) aTextureSetOld;
  NCollection_Vector<Handle(Graphic3d_Texture2Dmanual)> aTextureVecNew;
  bool toSetGenRepeat = false;
  bool toSetGenScale  = false;
  bool toSetGenOrigin = false;
  bool toSetImage     = false;
  bool toComputeUV    = false;

  const TCollection_AsciiString aCommandName (theArgVec[0]);
  bool toSetDefaults = aCommandName == "vtexdefault";

  ViewerTest_AutoUpdater anUpdateTool (aCtx, ViewerTest::CurrentView());
  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    const TCollection_AsciiString aName     = theArgVec[anArgIter];
    TCollection_AsciiString       aNameCase = aName;
    aNameCase.LowerCase();
    if (anUpdateTool.parseRedrawMode (aName))
    {
      continue;
    }
    else if (aTexturedIO.IsNull())
    {
      const ViewerTest_DoubleMapOfInteractiveAndName& aMapOfIO = GetMapOfAIS();
      if (aMapOfIO.IsBound2 (aName))
      {
        aTexturedIO = Handle(AIS_Shape)::DownCast (aMapOfIO.Find2 (aName));
      }
      if (aTexturedIO.IsNull())
      {
        std::cout << "Syntax error: shape " << aName << " does not exists in the viewer.\n";
        return 1;
      }

      if (aTexturedIO->Attributes()->HasOwnShadingAspect())
      {
        aTextureSetOld = aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureSet();
      }
    }
    else if (aNameCase == "-scale"
          || aNameCase == "-setscale"
          || aCommandName == "vtexscale")
    {
      if (aCommandName != "vtexscale")
      {
        ++anArgIter;
      }
      if (anArgIter < theArgsNb)
      {
        TCollection_AsciiString aValU (theArgVec[anArgIter]);
        TCollection_AsciiString aValUCase = aValU;
        aValUCase.LowerCase();
        toSetGenScale = true;
        if (aValUCase == "off")
        {
          aTexturedIO->SetTextureScaleUV (gp_Pnt2d (1.0, 1.0));
          continue;
        }
        else if (anArgIter + 1 < theArgsNb)
        {
          TCollection_AsciiString aValV (theArgVec[anArgIter + 1]);
          if (aValU.IsRealValue()
           && aValV.IsRealValue())
          {
            aTexturedIO->SetTextureScaleUV (gp_Pnt2d (aValU.RealValue(), aValV.RealValue()));
            ++anArgIter;
            continue;
          }
        }
      }
      std::cout << "Syntax error: unexpected argument '" << aName << "'\n";
      return 1;
    }
    else if (aNameCase == "-origin"
          || aNameCase == "-setorigin"
          || aCommandName == "vtexorigin")
    {
      if (aCommandName != "vtexorigin")
      {
        ++anArgIter;
      }
      if (anArgIter < theArgsNb)
      {
        TCollection_AsciiString aValU (theArgVec[anArgIter]);
        TCollection_AsciiString aValUCase = aValU;
        aValUCase.LowerCase();
        toSetGenOrigin = true;
        if (aValUCase == "off")
        {
          aTexturedIO->SetTextureOriginUV (gp_Pnt2d (0.0, 0.0));
          continue;
        }
        else if (anArgIter + 1 < theArgsNb)
        {
          TCollection_AsciiString aValV (theArgVec[anArgIter + 1]);
          if (aValU.IsRealValue()
           && aValV.IsRealValue())
          {
            aTexturedIO->SetTextureOriginUV (gp_Pnt2d (aValU.RealValue(), aValV.RealValue()));
            ++anArgIter;
            continue;
          }
        }
      }
      std::cout << "Syntax error: unexpected argument '" << aName << "'\n";
      return 1;
    }
    else if (aNameCase == "-repeat"
          || aNameCase == "-setrepeat"
          || aCommandName == "vtexrepeat")
    {
      if (aCommandName != "vtexrepeat")
      {
        ++anArgIter;
      }
      if (anArgIter < theArgsNb)
      {
        TCollection_AsciiString aValU (theArgVec[anArgIter]);
        TCollection_AsciiString aValUCase = aValU;
        aValUCase.LowerCase();
        toSetGenRepeat = true;
        if (aValUCase == "off")
        {
          aTexturedIO->SetTextureRepeatUV (gp_Pnt2d (1.0, 1.0));
          continue;
        }
        else if (anArgIter + 1 < theArgsNb)
        {
          TCollection_AsciiString aValV (theArgVec[anArgIter + 1]);
          if (aValU.IsRealValue()
           && aValV.IsRealValue())
          {
            aTexturedIO->SetTextureRepeatUV (gp_Pnt2d (aValU.RealValue(), aValV.RealValue()));
            ++anArgIter;
            continue;
          }
        }
      }
      std::cout << "Syntax error: unexpected argument '" << aName << "'\n";
      return 1;
    }
    else if (aNameCase == "-modulate")
    {
      bool toModulateBool = true;
      if (anArgIter + 1 < theArgsNb
       && ViewerTest::ParseOnOff (theArgVec[anArgIter + 1], toModulateBool))
      {
        ++anArgIter;
      }
      toModulate = toModulateBool ? 1 : 0;
    }
    else if ((aNameCase == "-setfilter"
           || aNameCase == "-filter")
           && anArgIter + 1 < theArgsNb)
    {
      TCollection_AsciiString aValue (theArgVec[anArgIter + 1]);
      aValue.LowerCase();
      ++anArgIter;
      toSetFilter = true;
      if (aValue == "nearest")
      {
        aFilter = Graphic3d_TOTF_NEAREST;
      }
      else if (aValue == "bilinear")
      {
        aFilter = Graphic3d_TOTF_BILINEAR;
      }
      else if (aValue == "trilinear")
      {
        aFilter = Graphic3d_TOTF_TRILINEAR;
      }
      else
      {
        std::cout << "Syntax error: unexpected argument '" << aValue << "'\n";
        return 1;
      }
    }
    else if ((aNameCase == "-setaniso"
           || aNameCase == "-setanisofilter"
           || aNameCase == "-aniso"
           || aNameCase == "-anisofilter")
           && anArgIter + 1 < theArgsNb)
    {
      TCollection_AsciiString aValue (theArgVec[anArgIter + 1]);
      aValue.LowerCase();
      ++anArgIter;
      toSetAniso = true;
      if (aValue == "off")
      {
        anAnisoFilter = Graphic3d_LOTA_OFF;
      }
      else if (aValue == "fast")
      {
        anAnisoFilter = Graphic3d_LOTA_FAST;
      }
      else if (aValue == "middle")
      {
        anAnisoFilter = Graphic3d_LOTA_MIDDLE;
      }
      else if (aValue == "quality"
            || aValue == "high")
      {
        anAnisoFilter =  Graphic3d_LOTA_QUALITY;
      }
      else
      {
        std::cout << "Syntax error: unexpected argument '" << aValue << "'\n";
        return 1;
      }
    }
    else if ((aNameCase == "-rotateangle"
           || aNameCase == "-rotangle"
           || aNameCase == "-rotate"
           || aNameCase == "-angle"
           || aNameCase == "-trsfangle")
           && anArgIter + 1 < theArgsNb)
    {
      aTrsfRotAngle  = Standard_ShortReal (Draw::Atof (theArgVec[anArgIter + 1]));
      toSetTrsfAngle = true;
      ++anArgIter;
    }
    else if ((aNameCase == "-trsftrans"
           || aNameCase == "-trsftranslate"
           || aNameCase == "-translate"
           || aNameCase == "-translation")
           && anArgIter + 2 < theArgsNb)
    {
      aTrsfTrans.x() = Standard_ShortReal (Draw::Atof (theArgVec[anArgIter + 1]));
      aTrsfTrans.y() = Standard_ShortReal (Draw::Atof (theArgVec[anArgIter + 2]));
      toSetTrsfTrans = true;
      anArgIter += 2;
    }
    else if ((aNameCase == "-trsfscale")
           && anArgIter + 2 < theArgsNb)
    {
      aTrsfScale.x() = Standard_ShortReal (Draw::Atof (theArgVec[anArgIter + 1]));
      aTrsfScale.y() = Standard_ShortReal (Draw::Atof (theArgVec[anArgIter + 2]));
      toSetTrsfScale = true;
      anArgIter += 2;
    }
    else if (aNameCase == "-default"
          || aNameCase == "-defaults")
    {
      toSetDefaults = true;
    }
    else if (aCommandName == "vtexture"
          && (aTextureVecNew.IsEmpty()
           || aNameCase.StartsWith ("-tex")))
    {
      Standard_Integer aTexIndex = 0;
      TCollection_AsciiString aTexName = aName;
      if (aNameCase.StartsWith ("-tex"))
      {
        if (anArgIter + 1 >= theArgsNb
         || aNameCase.Length() < 5)
        {
          std::cout << "Syntax error: invalid argument '" << theArgVec[anArgIter] << "'\n";
          return 1;
        }

        TCollection_AsciiString aTexIndexStr = aNameCase.SubString (5, aNameCase.Length());
        if (!aTexIndexStr.IsIntegerValue())
        {
          std::cout << "Syntax error: invalid argument '" << theArgVec[anArgIter] << "'\n";
          return 1;
        }

        aTexIndex = aTexIndexStr.IntegerValue();
        aTexName  = theArgVec[anArgIter + 1];
        ++anArgIter;
      }
      if (aTexIndex >= Graphic3d_TextureUnit_NB
       || aTexIndex >= aCtx->CurrentViewer()->Driver()->InquireLimit (Graphic3d_TypeOfLimit_MaxCombinedTextureUnits))
      {
        std::cout << "Error: too many textures specified\n";
        return 1;
      }

      toSetImage = true;
      if (aTexName.IsIntegerValue())
      {
        const Standard_Integer aValue = aTexName.IntegerValue();
        if (aValue < 0 || aValue >= Graphic3d_Texture2D::NumberOfTextures())
        {
          std::cout << "Syntax error: texture with ID " << aValue << " is undefined!\n";
          return 1;
        }
        aTextureVecNew.SetValue (aTexIndex, new Graphic3d_Texture2Dmanual (Graphic3d_NameOfTexture2D (aValue)));
      }
      else if (aTexName == "?")
      {
        const TCollection_AsciiString aTextureFolder = Graphic3d_TextureRoot::TexturesFolder();

        theDi << "\n Files in current directory : \n\n";
        theDi.Eval ("glob -nocomplain *");

        TCollection_AsciiString aCmnd ("glob -nocomplain ");
        aCmnd += aTextureFolder;
        aCmnd += "/* ";

        theDi << "Files in " << aTextureFolder << " : \n\n";
        theDi.Eval (aCmnd.ToCString());
        return 0;
      }
      else if (aTexName != "off")
      {
        if (!OSD_File (aTexName).Exists())
        {
          std::cout << "Syntax error: non-existing image file has been specified '" << aTexName << "'.\n";
          return 1;
        }
        aTextureVecNew.SetValue (aTexIndex, new Graphic3d_Texture2Dmanual (aTexName));
      }
      else
      {
        aTextureVecNew.SetValue (aTexIndex, Handle(Graphic3d_Texture2Dmanual)());
      }
      aTextureVecNew.ChangeValue (aTexIndex)->GetParams()->SetTextureUnit ((Graphic3d_TextureUnit )aTexIndex);
    }
    else
    {
      std::cout << "Syntax error: invalid argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }

  if (toSetImage)
  {
    // check if new image set is equal to already set one
    Standard_Integer aNbChanged = 0;
    Handle(Graphic3d_TextureSet) aTextureSetNew;
    if (!aTextureVecNew.IsEmpty())
    {
      aNbChanged = aTextureVecNew.Size();
      aTextureSetNew = new Graphic3d_TextureSet (aTextureVecNew.Size());
      for (Standard_Integer aTexIter = 0; aTexIter < aTextureSetNew->Size(); ++aTexIter)
      {
        Handle(Graphic3d_Texture2Dmanual)& aTextureNew = aTextureVecNew.ChangeValue (aTexIter);
        Handle(Graphic3d_TextureRoot) aTextureOld;
        if (!aTextureSetOld.IsNull()
          && aTexIter < aTextureSetOld->Size())
        {
          aTextureOld = aTextureSetOld->Value (aTexIter);
        }

        if (!aTextureOld.IsNull()
         && !aTextureNew.IsNull())
        {
          *aTextureNew->GetParams() = *aTextureOld->GetParams();
          if (Handle(Graphic3d_Texture2Dmanual) anOldManualTex = Handle(Graphic3d_Texture2Dmanual)::DownCast (aTextureOld))
          {
            TCollection_AsciiString aFilePathOld, aFilePathNew;
            aTextureOld->Path().SystemName (aFilePathOld);
            aTextureNew->Path().SystemName (aFilePathNew);
            if (aTextureNew->Name() == anOldManualTex->Name()
             && aFilePathOld == aFilePathNew
             && (!aFilePathNew.IsEmpty() || aTextureNew->Name() != Graphic3d_NOT_2D_UNKNOWN))
            {
              --aNbChanged;
              aTextureNew = anOldManualTex;
            }
          }
        }
        aTextureSetNew->SetValue (aTexIter, aTextureNew);
      }
    }
    if (aNbChanged == 0
     && ((aTextureSetOld.IsNull() && aTextureSetNew.IsNull())
      || (aTextureSetOld->Size() == aTextureSetNew->Size())))
    {
      aTextureSetNew = aTextureSetOld;
    }

    if (!aTexturedIO->Attributes()->HasOwnShadingAspect())
    {
      aTexturedIO->Attributes()->SetShadingAspect (new Prs3d_ShadingAspect());
      *aTexturedIO->Attributes()->ShadingAspect()->Aspect() = *aCtx->DefaultDrawer()->ShadingAspect()->Aspect();
    }

    toComputeUV = !aTextureSetNew.IsNull() && aTextureSetOld.IsNull();
    aTexturedIO->Attributes()->ShadingAspect()->Aspect()->SetTextureMapOn (!aTextureSetNew.IsNull());
    aTexturedIO->Attributes()->ShadingAspect()->Aspect()->SetTextureSet (aTextureSetNew);
    aTextureSetOld.Nullify();
  }

  if (toSetDefaults)
  {
    if (toModulate != -1)
    {
      toModulate = 1;
    }
    if (!toSetFilter)
    {
      toSetFilter = true;
      aFilter     = Graphic3d_TOTF_BILINEAR;
    }
    if (!toSetAniso)
    {
      toSetAniso    = true;
      anAnisoFilter = Graphic3d_LOTA_OFF;
    }
    if (!toSetTrsfAngle)
    {
      toSetTrsfAngle = true;
      aTrsfRotAngle  = 0.0f;
    }
    if (!toSetTrsfTrans)
    {
      toSetTrsfTrans = true;
      aTrsfTrans = Graphic3d_Vec2 (0.0f, 0.0f);
    }
    if (!toSetTrsfScale)
    {
      toSetTrsfScale = true;
      aTrsfScale = Graphic3d_Vec2 (1.0f, 1.0f);
    }
  }

  if (aCommandName == "vtexture"
   && theArgsNb == 2)
  {
    if (!aTextureSetOld.IsNull())
    {
      //toComputeUV = true; // we can keep UV vertex attributes
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->SetTextureMapOff();
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->SetTextureSet (Handle(Graphic3d_TextureSet)());
      aTextureSetOld.Nullify();
    }
  }

  if (aTexturedIO->Attributes()->HasOwnShadingAspect()
  && !aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap().IsNull())
  {
    if (toModulate != -1)
    {
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap()->GetParams()->SetModulate (toModulate == 1);
    }
    if (toSetTrsfAngle)
    {
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap()->GetParams()->SetRotation (aTrsfRotAngle); // takes degrees
    }
    if (toSetTrsfTrans)
    {
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap()->GetParams()->SetTranslation (aTrsfTrans);
    }
    if (toSetTrsfScale)
    {
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap()->GetParams()->SetScale (aTrsfScale);
    }
    if (toSetFilter)
    {
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap()->GetParams()->SetFilter (aFilter);
    }
    if (toSetAniso)
    {
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap()->GetParams()->SetAnisoFilter (anAnisoFilter);
    }
  }

  // set default values if requested
  if (!toSetGenRepeat
   && (aCommandName == "vtexrepeat"
    || toSetDefaults))
  {
    aTexturedIO->SetTextureRepeatUV (gp_Pnt2d (1.0, 1.0));
    toSetGenRepeat = true;
  }
  if (!toSetGenOrigin
   && (aCommandName == "vtexorigin"
    || toSetDefaults))
  {
    aTexturedIO->SetTextureOriginUV (gp_Pnt2d (0.0, 0.0));
    toSetGenOrigin = true;
  }
  if (!toSetGenScale
   && (aCommandName == "vtexscale"
    || toSetDefaults))
  {
    aTexturedIO->SetTextureScaleUV  (gp_Pnt2d (1.0, 1.0));
    toSetGenScale = true;
  }

  if (toSetGenRepeat || toSetGenOrigin || toSetGenScale || toComputeUV)
  {
    aTexturedIO->SetToUpdate (AIS_Shaded);
    if (toSetImage)
    {
      if ((aTexturedIO->HasDisplayMode() && aTexturedIO->DisplayMode() != AIS_Shaded)
       || aCtx->DisplayMode() != AIS_Shaded)
      {
        aCtx->SetDisplayMode (aTexturedIO, AIS_Shaded, false);
      }
    }
  }
  aCtx->Display (aTexturedIO, false);
  aTexturedIO->SynchronizeAspects();
  return 0;
}

//! Auxiliary method to parse transformation persistence flags
inline Standard_Boolean parseTrsfPersFlag (const TCollection_AsciiString& theFlagString,
                                           Graphic3d_TransModeFlags&      theFlags)
{
  if (theFlagString == "zoom")
  {
    theFlags = Graphic3d_TMF_ZoomPers;
  }
  else if (theFlagString == "rotate")
  {
    theFlags = Graphic3d_TMF_RotatePers;
  }
  else if (theFlagString == "zoomrotate")
  {
    theFlags = Graphic3d_TMF_ZoomRotatePers;
  }
  else if (theFlagString == "trihedron"
        || theFlagString == "triedron")
  {
    theFlags = Graphic3d_TMF_TriedronPers;
  }
  else if (theFlagString == "none")
  {
    theFlags = Graphic3d_TMF_None;
  }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

//! Auxiliary method to parse transformation persistence flags
inline Standard_Boolean parseTrsfPersCorner (const TCollection_AsciiString& theString,
                                             Aspect_TypeOfTriedronPosition& theCorner)
{
  TCollection_AsciiString aString (theString);
  aString.LowerCase();
  if (aString == "center")
  {
    theCorner = Aspect_TOTP_CENTER;
  }
  else if (aString == "top"
        || aString == "upper")
  {
    theCorner = Aspect_TOTP_TOP;
  }
  else if (aString == "bottom"
        || aString == "lower")
  {
    theCorner = Aspect_TOTP_BOTTOM;
  }
  else if (aString == "left")
  {
    theCorner = Aspect_TOTP_LEFT;
  }
  else if (aString == "right")
  {
    theCorner = Aspect_TOTP_RIGHT;
  }
  else if (aString == "topleft"
        || aString == "leftupper"
        || aString == "upperleft")
  {
    theCorner = Aspect_TOTP_LEFT_UPPER;
  }
  else if (aString == "bottomleft"
        || aString == "leftlower"
        || aString == "lowerleft")
  {
    theCorner = Aspect_TOTP_LEFT_LOWER;
  }
  else if (aString == "topright"
        || aString == "rightupper"
        || aString == "upperright")
  {
    theCorner = Aspect_TOTP_RIGHT_UPPER;
  }
  else if (aString == "bottomright"
        || aString == "lowerright"
        || aString == "rightlower")
  {
    theCorner = Aspect_TOTP_RIGHT_LOWER;
  }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

//==============================================================================
//function : VDisplay2
//author   : ege
//purpose  : Display an object from its name
//==============================================================================
static int VDisplay2 (Draw_Interpretor& theDI,
                      Standard_Integer  theArgNb,
                      const char**      theArgVec)
{
  if (theArgNb < 2)
  {
    std::cerr << theArgVec[0] << "Error: wrong number of arguments.\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    ViewerTest::ViewerInit();
    aCtx = ViewerTest::GetAISContext();
  }

  // Parse input arguments
  ViewerTest_AutoUpdater anUpdateTool (aCtx, ViewerTest::CurrentView());
  Standard_Integer   isMutable      = -1;
  Graphic3d_ZLayerId aZLayer        = Graphic3d_ZLayerId_UNKNOWN;
  Standard_Boolean   toDisplayLocal = Standard_False;
  Standard_Boolean   toReDisplay    = Standard_False;
  Standard_Integer   isSelectable   = -1;
  Standard_Integer   anObjDispMode  = -2;
  Standard_Integer   anObjHighMode  = -2;
  Standard_Boolean   toSetTrsfPers  = Standard_False;
  Handle(Graphic3d_TransformPers) aTrsfPers;
  TColStd_SequenceOfAsciiString aNamesOfDisplayIO;
  AIS_DisplayStatus aDispStatus = AIS_DS_None;
  Standard_Integer toDisplayInView = Standard_False;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    const TCollection_AsciiString aName     = theArgVec[anArgIter];
    TCollection_AsciiString       aNameCase = aName;
    aNameCase.LowerCase();
    if (anUpdateTool.parseRedrawMode (aName))
    {
      continue;
    }
    else if (aNameCase == "-mutable")
    {
      isMutable = 1;
    }
    else if (aNameCase == "-neutral")
    {
      aDispStatus = AIS_DS_Displayed;
    }
    else if (aNameCase == "-immediate"
          || aNameCase == "-top")
    {
      aZLayer = Graphic3d_ZLayerId_Top;
    }
    else if (aNameCase == "-topmost")
    {
      aZLayer = Graphic3d_ZLayerId_Topmost;
    }
    else if (aNameCase == "-osd"
          || aNameCase == "-toposd"
          || aNameCase == "-overlay")
    {
      aZLayer = Graphic3d_ZLayerId_TopOSD;
    }
    else if (aNameCase == "-botosd"
          || aNameCase == "-underlay")
    {
      aZLayer = Graphic3d_ZLayerId_BotOSD;
    }
    else if (aNameCase == "-select"
          || aNameCase == "-selectable")
    {
      isSelectable = 1;
    }
    else if (aNameCase == "-noselect"
          || aNameCase == "-noselection")
    {
      isSelectable = 0;
    }
    else if (aNameCase == "-dispmode"
          || aNameCase == "-displaymode")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at " << aName << ".\n";
        return 1;
      }

      anObjDispMode = Draw::Atoi (theArgVec [anArgIter]);
    }
    else if (aNameCase == "-highmode"
          || aNameCase == "-highlightmode")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at " << aName << ".\n";
        return 1;
      }

      anObjHighMode = Draw::Atoi (theArgVec [anArgIter]);
    }
    else if (aNameCase == "-3d")
    {
      toSetTrsfPers  = Standard_True;
      aTrsfPers.Nullify();
    }
    else if (aNameCase == "-2d"
          || aNameCase == "-trihedron"
          || aNameCase == "-triedron")
    {
      toSetTrsfPers  = Standard_True;
      aTrsfPers = new Graphic3d_TransformPers (aNameCase == "-2d" ? Graphic3d_TMF_2d : Graphic3d_TMF_TriedronPers, Aspect_TOTP_LEFT_LOWER);

      if (anArgIter + 1 < theArgNb)
      {
        Aspect_TypeOfTriedronPosition aCorner = Aspect_TOTP_CENTER;
        if (parseTrsfPersCorner (theArgVec[anArgIter + 1], aCorner))
        {
          ++anArgIter;
          aTrsfPers->SetCorner2d (aCorner);

          if (anArgIter + 2 < theArgNb)
          {
            TCollection_AsciiString anX (theArgVec[anArgIter + 1]);
            TCollection_AsciiString anY (theArgVec[anArgIter + 2]);
            if (anX.IsIntegerValue()
             && anY.IsIntegerValue())
            {
              anArgIter += 2;
              aTrsfPers->SetOffset2d (Graphic3d_Vec2i (anX.IntegerValue(), anY.IntegerValue()));
            }
          }
        }
      }
    }
    else if (aNameCase == "-trsfpers"
          || aNameCase == "-pers")
    {
      if (++anArgIter >= theArgNb
       || !aTrsfPers.IsNull())
      {
        std::cerr << "Error: wrong syntax at " << aName << ".\n";
        return 1;
      }

      toSetTrsfPers  = Standard_True;
      Graphic3d_TransModeFlags aTrsfPersFlags = Graphic3d_TMF_None;
      TCollection_AsciiString aPersFlags (theArgVec [anArgIter]);
      aPersFlags.LowerCase();
      if (!parseTrsfPersFlag (aPersFlags, aTrsfPersFlags))
      {
        std::cerr << "Error: wrong transform persistence flags " << theArgVec [anArgIter] << ".\n";
        return 1;
      }

      if (aTrsfPersFlags == Graphic3d_TMF_TriedronPers)
      {
        aTrsfPers = new Graphic3d_TransformPers (Graphic3d_TMF_TriedronPers, Aspect_TOTP_LEFT_LOWER);
      }
      else if (aTrsfPersFlags != Graphic3d_TMF_None)
      {
        aTrsfPers = new Graphic3d_TransformPers (aTrsfPersFlags, gp_Pnt());
      }
    }
    else if (aNameCase == "-trsfperspos"
          || aNameCase == "-perspos")
    {
      if (anArgIter + 2 >= theArgNb
       || aTrsfPers.IsNull())
      {
        std::cerr << "Error: wrong syntax at " << aName << ".\n";
        return 1;
      }

      TCollection_AsciiString aX (theArgVec[++anArgIter]);
      TCollection_AsciiString aY (theArgVec[++anArgIter]);
      TCollection_AsciiString aZ = "0";
      if (!aX.IsRealValue()
       || !aY.IsRealValue())
      {
        std::cerr << "Error: wrong syntax at " << aName << ".\n";
        return 1;
      }
      if (anArgIter + 1 < theArgNb)
      {
        TCollection_AsciiString aTemp = theArgVec[anArgIter + 1];
        if (aTemp.IsRealValue())
        {
          aZ = aTemp;
          ++anArgIter;
        }
      }

      const gp_Pnt aPnt (aX.RealValue(), aY.RealValue(), aZ.RealValue());
      if (aTrsfPers->IsZoomOrRotate())
      {
        aTrsfPers->SetAnchorPoint (aPnt);
      }
      else if (aTrsfPers->IsTrihedronOr2d())
      {
        aTrsfPers = Graphic3d_TransformPers::FromDeprecatedParams (aTrsfPers->Mode(), aPnt);
      }
    }
    else if (aNameCase == "-layer")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at " << aName << ".\n";
        return 1;
      }

      TCollection_AsciiString aValue (theArgVec[anArgIter]);
      if (!aValue.IsIntegerValue())
      {
        std::cerr << "Error: wrong syntax at " << aName << ".\n";
        return 1;
      }

      aZLayer = aValue.IntegerValue();
    }
    else if (aNameCase == "-view"
          || aNameCase == "-inview")
    {
      toDisplayInView = Standard_True;
    }
    else if (aNameCase == "-local")
    {
      aDispStatus = AIS_DS_Temporary;
      toDisplayLocal = Standard_True;
    }
    else if (aNameCase == "-redisplay")
    {
      toReDisplay = Standard_True;
    }
    else
    {
      aNamesOfDisplayIO.Append (aName);
    }
  }

  if (aNamesOfDisplayIO.IsEmpty())
  {
    std::cerr << theArgVec[0] << "Error: wrong number of arguments.\n";
    return 1;
  }

  // Prepare context for display
  Standard_DISABLE_DEPRECATION_WARNINGS
  if (toDisplayLocal && !aCtx->HasOpenedContext())
  {
    aCtx->OpenLocalContext (Standard_False);
  }
  else if (!toDisplayLocal && aCtx->HasOpenedContext())
  {
    aCtx->CloseAllContexts (Standard_False);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  // Display interactive objects
  for (Standard_Integer anIter = 1; anIter <= aNamesOfDisplayIO.Length(); ++anIter)
  {
    const TCollection_AsciiString& aName = aNamesOfDisplayIO.Value(anIter);

    if (!GetMapOfAIS().IsBound2 (aName))
    {
      // create the AIS_Shape from a name
      const Handle(AIS_InteractiveObject) aShape = GetAISShapeFromName (aName.ToCString());
      if (!aShape.IsNull())
      {
        if (isMutable != -1)
        {
          aShape->SetMutable (isMutable == 1);
        }
        if (aZLayer != Graphic3d_ZLayerId_UNKNOWN)
        {
          aShape->SetZLayer (aZLayer);
        }
        if (toSetTrsfPers)
        {
          aCtx->SetTransformPersistence (aShape, aTrsfPers);
        }
        if (anObjDispMode != -2)
        {
          aShape->SetDisplayMode (anObjDispMode);
        }
        if (anObjHighMode != -2)
        {
          aShape->SetHilightMode (anObjHighMode);
        }
        if (!toDisplayLocal)
          GetMapOfAIS().Bind (aShape, aName);

        Standard_Integer aDispMode = aShape->HasDisplayMode()
                                   ? aShape->DisplayMode()
                                   : (aShape->AcceptDisplayMode (aCtx->DisplayMode())
                                    ? aCtx->DisplayMode()
                                    : 0);
        Standard_Integer aSelMode = -1;
        if (isSelectable ==  1 || (isSelectable == -1 && aCtx->GetAutoActivateSelection()))
        {
          aSelMode = aShape->GlobalSelectionMode();
        }

        aCtx->Display (aShape, aDispMode, aSelMode,
                       Standard_False, aShape->AcceptShapeDecomposition(),
                       aDispStatus);
        if (toDisplayInView)
        {
          for (V3d_ListOfViewIterator aViewIter (aCtx->CurrentViewer()->DefinedViewIterator()); aViewIter.More(); aViewIter.Next())
          {
            aCtx->SetViewAffinity (aShape, aViewIter.Value(), Standard_False);
          }
          aCtx->SetViewAffinity (aShape, ViewerTest::CurrentView(), Standard_True);
        }
      }
      else
      {
        std::cerr << "Error: object with name '" << aName << "' does not exist!\n";
      }
      continue;
    }

    Handle(AIS_InteractiveObject) aShape = Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (aName));
    if (isMutable != -1)
    {
      aShape->SetMutable (isMutable == 1);
    }
    if (aZLayer != Graphic3d_ZLayerId_UNKNOWN)
    {
      aShape->SetZLayer (aZLayer);
    }
    if (toSetTrsfPers)
    {
      aCtx->SetTransformPersistence (aShape, aTrsfPers);
    }
    if (anObjDispMode != -2)
    {
      aShape->SetDisplayMode (anObjDispMode);
    }
    if (anObjHighMode != -2)
    {
      aShape->SetHilightMode (anObjHighMode);
    }
    Standard_Integer aDispMode = aShape->HasDisplayMode()
                                ? aShape->DisplayMode()
                                : (aShape->AcceptDisplayMode (aCtx->DisplayMode())
                                ? aCtx->DisplayMode()
                                : 0);
    Standard_Integer aSelMode = -1;
    if (isSelectable ==  1 || (isSelectable == -1 && aCtx->GetAutoActivateSelection()))
    {
      aSelMode = aShape->GlobalSelectionMode();
    }

    if (aShape->Type() == AIS_KOI_Datum)
    {
      aCtx->Display (aShape, Standard_False);
    }
    else
    {
      theDI << "Display " << aName.ToCString() << "\n";

      // update the Shape in the AIS_Shape
      TopoDS_Shape      aNewShape = GetShapeFromName (aName.ToCString());
      Handle(AIS_Shape) aShapePrs = Handle(AIS_Shape)::DownCast(aShape);
      if (!aShapePrs.IsNull())
      {
        if (!aShapePrs->Shape().IsEqual (aNewShape))
        {
          toReDisplay = Standard_True;
        }
        aShapePrs->Set (aNewShape);
      }
      if (toReDisplay)
      {
        aCtx->Redisplay (aShape, Standard_False);
      }

      if (aSelMode == -1)
      {
        aCtx->Erase (aShape, Standard_False);
      }
      aCtx->Display (aShape, aDispMode, aSelMode,
                     Standard_False, aShape->AcceptShapeDecomposition(),
                     aDispStatus);
      if (toDisplayInView)
      {
        aCtx->SetViewAffinity (aShape, ViewerTest::CurrentView(), Standard_True);
      }
    }
  }

  return 0;
}

//=======================================================================
//function : VNbDisplayed
//purpose  : Returns number of displayed objects
//=======================================================================
static Standard_Integer VNbDisplayed (Draw_Interpretor& theDi,
                                      Standard_Integer theArgsNb,
                                      const char** theArgVec)
{
  if(theArgsNb != 1)
  {
    theDi << "Usage : " << theArgVec[0] << "\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    std::cout << theArgVec[0] << "AIS context is not available.\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    theDi << "use 'vinit' command before " << theArgVec[0] << "\n";
    return 1;
  }

  AIS_ListOfInteractive aListOfIO;
  aContextAIS->DisplayedObjects(aListOfIO, false);

  theDi << aListOfIO.Extent() << "\n";
  return 0;
}

//===============================================================================================
//function : VUpdate
//purpose  :
//===============================================================================================
static int VUpdate (Draw_Interpretor& /*theDi*/, Standard_Integer theArgsNb, const char** theArgVec)
{
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    std::cout << theArgVec[0] << "AIS context is not available.\n";
    return 1;
  }

  if (theArgsNb < 2)
  {
    std::cout << theArgVec[0] << ": insufficient arguments. Type help for more information.\n";
    return 1;
  }

  const ViewerTest_DoubleMapOfInteractiveAndName& anAISMap = GetMapOfAIS();

  AIS_ListOfInteractive aListOfIO;

  for (int anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    TCollection_AsciiString aName = TCollection_AsciiString (theArgVec[anArgIt]);

    Handle(AIS_InteractiveObject) anAISObj;
    if (anAISMap.IsBound2 (aName))
    {
      anAISObj = Handle(AIS_InteractiveObject)::DownCast (anAISMap.Find2 (aName));
    }

    if (anAISObj.IsNull())
    {
      std::cout << theArgVec[0] << ": no AIS interactive object named \"" << aName << "\".\n";
      return 1;
    }

    aListOfIO.Append (anAISObj);
  }

  AIS_ListIteratorOfListOfInteractive anIOIt (aListOfIO);
  for (; anIOIt.More(); anIOIt.Next())
  {
    aContextAIS->Update (anIOIt.Value(), Standard_False);
  }

  aContextAIS->UpdateCurrentViewer();

  return 0;
}

//==============================================================================
//function : VShading
//purpose  : Sharpen or roughten the quality of the shading
//Draw arg : vshading ShapeName 0.1->0.00001  1 deg-> 30 deg
//==============================================================================
static int VShading(Draw_Interpretor& ,Standard_Integer argc, const char** argv)
{
  Standard_Real    myDevCoef;
  Handle(AIS_InteractiveObject) TheAisIO;

  // Verifications
  const Standard_Boolean HaveToSet = (strcasecmp(argv[0],"vsetshading") == 0);
  if (argc < 3) {
    myDevCoef  = 0.0008;
  } else {
    myDevCoef  =Draw::Atof(argv[2]);
  }

  TCollection_AsciiString name=argv[1];
  if (GetMapOfAIS().IsBound2(name ))
    TheAisIO = Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(name));
  if (TheAisIO.IsNull())
    TheAisIO=GetAISShapeFromName((const char *)name.ToCString());

  if (HaveToSet)
    TheAISContext()->SetDeviationCoefficient(TheAisIO,myDevCoef,Standard_True);
  else
    TheAISContext()->SetDeviationCoefficient(TheAisIO,0.0008,Standard_True);

  TheAISContext()->Redisplay (TheAisIO, Standard_True);
  return 0;
}
//==============================================================================
//function : HaveMode
//use      : VActivatedModes
//==============================================================================
#include <TColStd_ListIteratorOfListOfInteger.hxx>

Standard_Boolean  HaveMode(const Handle(AIS_InteractiveObject)& TheAisIO,const Standard_Integer mode  )
{
  TColStd_ListOfInteger List;
  TheAISContext()->ActivatedModes (TheAisIO,List);
  TColStd_ListIteratorOfListOfInteger it;
  Standard_Boolean Found=Standard_False;
  for (it.Initialize(List); it.More()&&!Found; it.Next() ){
    if (it.Value()==mode ) Found=Standard_True;
  }
  return Found;
}



//==============================================================================
//function : VActivatedMode
//author   : ege
//purpose  : permet d'attribuer a chacune des shapes un mode d'activation
//           (edges,vertex...)qui lui est propre et le mode de selection standard.
//           La fonction s'applique aux shapes selectionnees(current ou selected dans le viewer)
//             Dans le cas ou on veut psser la shape en argument, la fonction n'autorise
//           qu'un nom et qu'un mode.
//Draw arg : vsetam  [ShapeName] mode(0,1,2,3,4,5,6,7)
//==============================================================================
#include <AIS_ListIteratorOfListOfInteractive.hxx>

static int VActivatedMode (Draw_Interpretor& di, Standard_Integer argc, const char** argv)

{
  Standard_Boolean ThereIsName = Standard_False ;

  if(!a3DView().IsNull()){

    const Standard_Boolean HaveToSet = (strcasecmp(argv[0],"vsetam") == 0);
    // verification des arguments
    if (HaveToSet) {
      if (argc<2||argc>3) { di<<" Syntaxe error\n";return 1;}
      ThereIsName = (argc == 3);
    }
    else
    {
      Standard_DISABLE_DEPRECATION_WARNINGS
      // vunsetam
      if (argc>1) {di<<" Syntaxe error\n";return 1;}
      else {
        di<<" R.A.Z de tous les modes de selecion\n";
        di<<" Fermeture du Context local\n";
        if (TheAISContext()->HasOpenedContext())
        {
          TheAISContext()->CloseLocalContext();
        }
      }
      Standard_ENABLE_DEPRECATION_WARNINGS
    }

    // IL n'y a aps de nom de shape passe en argument
    if (HaveToSet && !ThereIsName){
      Standard_Integer aMode=Draw::Atoi(argv [1]);
      const TopAbs_ShapeEnum aShapeType = AIS_Shape::SelectionType (aMode);
      const char* cmode = aMode >= 0 && aMode <= 8
                        ? TopAbs::ShapeTypeToString (aShapeType)
                        : "???";
      if( !TheAISContext()->HasOpenedContext() ) {
        // il n'y a pas de Context local d'ouvert
        // on en ouvre un et on charge toutes les shapes displayees
        // on load tous les objets displayees et on Activate les objets de la liste
        AIS_ListOfInteractive ListOfIO;
        // on sauve dans une AISListOfInteractive tous les objets currents
        if (TheAISContext()->NbSelected()>0 ){
          TheAISContext()->UnhilightSelected(Standard_False);

          for (TheAISContext()->InitSelected(); TheAISContext()->MoreSelected(); TheAISContext()->NextSelected() ){
            ListOfIO.Append(TheAISContext()->SelectedInteractive() );
	  }
	}

  Standard_DISABLE_DEPRECATION_WARNINGS
	TheAISContext()->OpenLocalContext(Standard_False);
  Standard_ENABLE_DEPRECATION_WARNINGS
	ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
          it (GetMapOfAIS());
	while(it.More()){
	  Handle(AIS_InteractiveObject) aIO =
            Handle(AIS_InteractiveObject)::DownCast(it.Key1());
          if (!aIO.IsNull())
            TheAISContext()->Load(aIO,0,Standard_False);
	  it.Next();
	}
	// traitement des objets qui etaient currents dans le Contexte global
	if (!ListOfIO.IsEmpty() ) {
	  // il y avait des objets currents
	  AIS_ListIteratorOfListOfInteractive iter;
	  for (iter.Initialize(ListOfIO); iter.More() ; iter.Next() ) {
	    Handle(AIS_InteractiveObject) aIO=iter.Value();
	    TheAISContext()->Activate(aIO,aMode);
	    di<<" Mode: "<<cmode<<" ON pour "<<GetMapOfAIS().Find1(aIO).ToCString()  <<"\n";
	  }
	}
	else {
	  // On applique le mode a tous les objets displayes
	  ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
            it2 (GetMapOfAIS());
	  while(it2.More()){
            Handle(AIS_InteractiveObject) aIO =
              Handle(AIS_InteractiveObject)::DownCast(it2.Key1());
            if (!aIO.IsNull()) {
              di<<" Mode: "<<cmode<<" ON pour "<<it2.Key2().ToCString() <<"\n";
              TheAISContext()->Activate(aIO,aMode);
            }
	    it2.Next();
	  }
	}

      }

      else {
	// un Context local est deja ouvert
	// Traitement des objets du Context local
	if (TheAISContext()->NbSelected()>0 ){
	  TheAISContext()->UnhilightSelected(Standard_False);
	  // il y a des objets selected,on les parcourt
	  for (TheAISContext()->InitSelected(); TheAISContext()->MoreSelected(); TheAISContext()->NextSelected() ){
	    Handle(AIS_InteractiveObject) aIO=TheAISContext()->SelectedInteractive();


	    if (HaveMode(aIO,aMode) ) {
	      di<<" Mode: "<<cmode<<" OFF pour "<<GetMapOfAIS().Find1(aIO).ToCString() <<"\n";
	      TheAISContext()->Deactivate(aIO,aMode);
	    }
	    else{
	      di<<" Mode: "<<cmode<<" ON pour "<<GetMapOfAIS().Find1(aIO).ToCString() <<"\n";
	      TheAISContext()->Activate(aIO,aMode);
	    }

	  }
	}
	else{
	  // il n'y a pas d'objets selected
	  // tous les objets diplayes sont traites
	  ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
            it (GetMapOfAIS());
	  while(it.More()){
	    Handle(AIS_InteractiveObject) aIO =
              Handle(AIS_InteractiveObject)::DownCast(it.Key1());
            if (!aIO.IsNull()) {
              if (HaveMode(aIO,aMode) ) {
                di<<" Mode: "<<cmode<<" OFF pour "
                  <<GetMapOfAIS().Find1(aIO).ToCString() <<"\n";
                TheAISContext()->Deactivate(aIO,aMode);
              }
              else{
                di<<" Mode: "<<cmode<<" ON pour"
                  <<GetMapOfAIS().Find1(aIO).ToCString() <<"\n";
                TheAISContext()->Activate(aIO,aMode);
              }
            }
	    it.Next();
          }
	}
      }
    }
    else if (HaveToSet && ThereIsName){
      Standard_Integer aMode=Draw::Atoi(argv [2]);
      Handle(AIS_InteractiveObject) aIO =
        Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(argv[1]));

      if (!aIO.IsNull()) {
        const TopAbs_ShapeEnum aShapeType = AIS_Shape::SelectionType (aMode);
        const char* cmode = aMode >= 0 && aMode <= 8
                          ? TopAbs::ShapeTypeToString (aShapeType)
                          : "???";
        if( !TheAISContext()->HasOpenedContext() ) {
          Standard_DISABLE_DEPRECATION_WARNINGS
          TheAISContext()->OpenLocalContext(Standard_False);
          Standard_ENABLE_DEPRECATION_WARNINGS
          // On charge tous les objets de la map
          ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS());
          while(it.More()){
            Handle(AIS_InteractiveObject) aShape=
              Handle(AIS_InteractiveObject)::DownCast(it.Key1());
            if (!aShape.IsNull())
              TheAISContext()->Load(aShape,0,Standard_False);
            it.Next();
          }
          TheAISContext()->Activate(aIO,aMode);
          di<<" Mode: "<<cmode<<" ON pour "<<argv[1]<<"\n";
        }

        else {
          // un Context local est deja ouvert
          if (HaveMode(aIO,aMode) ) {
            di<<" Mode: "<<cmode<<" OFF pour "<<argv[1]<<"\n";
            TheAISContext()->Deactivate(aIO,aMode);
          }
          else{
            di<<" Mode: "<<cmode<<" ON pour "<<argv[1]<<"\n";
            TheAISContext()->Activate(aIO,aMode);
          }
        }
      }
    }
  }
  return 0;
}

//! Auxiliary method to print Interactive Object information
static void objInfo (const NCollection_Map<Handle(AIS_InteractiveObject)>& theDetected,
                     const Handle(Standard_Transient)&                     theObject,
                     Draw_Interpretor&                                     theDI)
{
  const Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (theObject);
  if (anObj.IsNull())
  {
    theDI << theObject->DynamicType()->Name() << " is not AIS presentation\n";
    return;
  }

  theDI << (TheAISContext()->IsDisplayed  (anObj) ? "Displayed"  : "Hidden   ")
        << (TheAISContext()->IsSelected   (anObj) ? " Selected" : "         ")
        << (theDetected.Contains (anObj)          ? " Detected" : "         ")
        << " Type: ";
  if (anObj->Type() == AIS_KOI_Datum)
  {
    // AIS_Datum
    if      (anObj->Signature() == 3) { theDI << " AIS_Trihedron"; }
    else if (anObj->Signature() == 2) { theDI << " AIS_Axis"; }
    else if (anObj->Signature() == 6) { theDI << " AIS_Circle"; }
    else if (anObj->Signature() == 5) { theDI << " AIS_Line"; }
    else if (anObj->Signature() == 7) { theDI << " AIS_Plane"; }
    else if (anObj->Signature() == 1) { theDI << " AIS_Point"; }
    else if (anObj->Signature() == 4) { theDI << " AIS_PlaneTrihedron"; }
  }
  // AIS_Shape
  else if (anObj->Type()      == AIS_KOI_Shape
        && anObj->Signature() == 0)
  {
    theDI << " AIS_Shape";
  }
  else if (anObj->Type() == AIS_KOI_Relation)
  {
    // AIS_Dimention and AIS_Relation
    Handle(AIS_Relation) aRelation = Handle(AIS_Relation)::DownCast (anObj);
    switch (aRelation->KindOfDimension())
    {
      case AIS_KOD_PLANEANGLE:     theDI << " AIS_AngleDimension"; break;
      case AIS_KOD_LENGTH:         theDI << " AIS_Chamf2/3dDimension/AIS_LengthDimension"; break;
      case AIS_KOD_DIAMETER:       theDI << " AIS_DiameterDimension"; break;
      case AIS_KOD_ELLIPSERADIUS:  theDI << " AIS_EllipseRadiusDimension"; break;
      //case AIS_KOD_FILLETRADIUS:   theDI << " AIS_FilletRadiusDimension "; break;
      case AIS_KOD_OFFSET:         theDI << " AIS_OffsetDimension"; break;
      case AIS_KOD_RADIUS:         theDI << " AIS_RadiusDimension"; break;
      default:                     theDI << " UNKNOWN dimension"; break;
    }
  }
  else
  {
    theDI << " UserPrs";
  }
  theDI << " (" << theObject->DynamicType()->Name() << ")";
}

//! Print information about locally selected sub-shapes
template <typename T>
static void printLocalSelectionInfo (const T& theContext, Draw_Interpretor& theDI)
{
  const Standard_Boolean isGlobalCtx = (theContext->DynamicType() == STANDARD_TYPE(AIS_InteractiveContext));
  TCollection_AsciiString aPrevName;
  for (theContext->InitSelected(); theContext->MoreSelected(); theContext->NextSelected())
  {
    const Handle(AIS_Shape) aShapeIO = Handle(AIS_Shape)::DownCast (theContext->SelectedInteractive());
    const Handle(SelectMgr_EntityOwner) anOwner = theContext->SelectedOwner();
    if (aShapeIO.IsNull() || anOwner.IsNull())
      continue;
    if (isGlobalCtx)
    {
      if (anOwner == aShapeIO->GlobalSelOwner())
        continue;
    }
    const TopoDS_Shape      aSubShape = theContext->SelectedShape();
    if (aSubShape.IsNull()
      || aShapeIO.IsNull()
      || !GetMapOfAIS().IsBound1 (aShapeIO))
    {
      continue;
    }

    const TCollection_AsciiString aParentName = GetMapOfAIS().Find1 (aShapeIO);
    TopTools_MapOfShape aFilter;
    Standard_Integer    aNumber = 0;
    const TopoDS_Shape  aShape  = aShapeIO->Shape();
    for (TopExp_Explorer anIter (aShape, aSubShape.ShapeType());
         anIter.More(); anIter.Next())
    {
      if (!aFilter.Add (anIter.Current()))
      {
        continue; // filter duplicates
      }

      ++aNumber;
      if (!anIter.Current().IsSame (aSubShape))
      {
        continue;
      }

      Standard_CString aShapeName = NULL;
      switch (aSubShape.ShapeType())
      {
        case TopAbs_COMPOUND:  aShapeName = " Compound"; break;
        case TopAbs_COMPSOLID: aShapeName = "CompSolid"; break;
        case TopAbs_SOLID:     aShapeName = "    Solid"; break;
        case TopAbs_SHELL:     aShapeName = "    Shell"; break;
        case TopAbs_FACE:      aShapeName = "     Face"; break;
        case TopAbs_WIRE:      aShapeName = "     Wire"; break;
        case TopAbs_EDGE:      aShapeName = "     Edge"; break;
        case TopAbs_VERTEX:    aShapeName = "   Vertex"; break;
        default:
        case TopAbs_SHAPE:     aShapeName = "    Shape"; break;
      }

      if (aParentName != aPrevName)
      {
        theDI << "Locally selected sub-shapes within " << aParentName << ":\n";
        aPrevName = aParentName;
      }
      theDI << "  " << aShapeName << " #" << aNumber << "\n";
      break;
    }
  }
}

//==============================================================================
//function : VState
//purpose  :
//==============================================================================
static Standard_Integer VState (Draw_Interpretor& theDI,
                                Standard_Integer  theArgNb,
                                Standard_CString* theArgVec)
{
  Handle(AIS_InteractiveContext) aCtx = TheAISContext();
  if (aCtx.IsNull())
  {
    std::cerr << "Error: No opened viewer!\n";
    return 1;
  }

  Standard_Boolean toPrintEntities = Standard_False;
  Standard_Boolean toCheckSelected = Standard_False;

  for (Standard_Integer anArgIdx = 1; anArgIdx < theArgNb; ++anArgIdx)
  {
    TCollection_AsciiString anOption (theArgVec[anArgIdx]);
    anOption.LowerCase();
    if (anOption == "-detectedentities"
      || anOption == "-entities")
    {
      toPrintEntities = Standard_True;
    }
    else if (anOption == "-hasselected")
    {
      toCheckSelected = Standard_True;
    }
  }

  if (toCheckSelected)
  {
    aCtx->InitSelected();
    TCollection_AsciiString hasSelected (static_cast<Standard_Integer> (aCtx->HasSelectedShape()));
    theDI << "Check if context has selected shape: " << hasSelected << "\n";

    return 0;
  }

  if (toPrintEntities)
  {
    theDI << "Detected entities:\n";
    Standard_DISABLE_DEPRECATION_WARNINGS
    Handle(StdSelect_ViewerSelector3d) aSelector = aCtx->HasOpenedContext() ? aCtx->LocalSelector() : aCtx->MainSelector();
    Standard_ENABLE_DEPRECATION_WARNINGS
    SelectMgr_SelectingVolumeManager aMgr = aSelector->GetManager();
    for (Standard_Integer aPickIter = 1; aPickIter <= aSelector->NbPicked(); ++aPickIter)
    {
      const SelectMgr_SortCriterion&              aPickData = aSelector->PickedData (aPickIter);
      const Handle(SelectBasics_SensitiveEntity)& anEntity = aSelector->PickedEntity (aPickIter);
      Handle(SelectMgr_EntityOwner) anOwner    = Handle(SelectMgr_EntityOwner)::DownCast (anEntity->OwnerId());
      Handle(AIS_InteractiveObject) anObj      = Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
      TCollection_AsciiString aName = GetMapOfAIS().Find1 (anObj);
      aName.LeftJustify (20, ' ');
      char anInfoStr[512];
      Sprintf (anInfoStr,
               " Depth: %g Distance: %g Point: %g %g %g",
               aPickData.Depth,
               aPickData.MinDist,
               aPickData.Point.X(), aPickData.Point.Y(), aPickData.Point.Z());
      theDI << "  " << aName
            << anInfoStr
            << " (" << anEntity->DynamicType()->Name() << ")"
            << "\n";

      Handle(StdSelect_BRepOwner) aBRepOwner = Handle(StdSelect_BRepOwner)::DownCast (anOwner);
      if (!aBRepOwner.IsNull())
      {
        theDI << "                       Detected Shape: "
              << aBRepOwner->Shape().TShape()->DynamicType()->Name()
              << "\n";
      }

      Handle(Select3D_SensitiveWire) aWire = Handle(Select3D_SensitiveWire)::DownCast (anEntity);
      if (!aWire.IsNull())
      {
        Handle(Select3D_SensitiveEntity) aSen = aWire->GetLastDetected();
        theDI << "                       Detected Child: "
              << aSen->DynamicType()->Name()
              << "\n";
      }

      Handle(Select3D_SensitivePrimitiveArray) aPrimArr = Handle(Select3D_SensitivePrimitiveArray)::DownCast (anEntity);
      if (!aPrimArr.IsNull())
      {
        theDI << "                       Detected Element: "
              << aPrimArr->LastDetectedElement()
              << "\n";
      }
    }
    return 0;
  }

  NCollection_Map<Handle(AIS_InteractiveObject)> aDetected;
  Standard_DISABLE_DEPRECATION_WARNINGS
  for (aCtx->InitDetected(); aCtx->MoreDetected(); aCtx->NextDetected())
  {
    aDetected.Add (aCtx->DetectedCurrentObject());
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  const Standard_Boolean toShowAll = (theArgNb >= 2 && *theArgVec[1] == '*');
  if (theArgNb >= 2
   && !toShowAll)
  {
    for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
    {
      const TCollection_AsciiString anObjName = theArgVec[anArgIter];
      if (!GetMapOfAIS().IsBound2 (anObjName))
      {
        theDI << anObjName << " doesn't exist!\n";
        continue;
      }

      const Handle(Standard_Transient) anObjTrans = GetMapOfAIS().Find2 (anObjName);
      TCollection_AsciiString aName = anObjName;
      aName.LeftJustify (20, ' ');
      theDI << "  " << aName << " ";
      objInfo (aDetected, anObjTrans, theDI);
      theDI << "\n";
    }
    return 0;
  }

  if (!aCtx->HasOpenedContext() && aCtx->NbSelected() > 0 && !toShowAll)
  {
    NCollection_DataMap<Handle(SelectMgr_EntityOwner), TopoDS_Shape> anOwnerShapeMap;
    for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
    {
      const Handle(SelectMgr_EntityOwner) anOwner = aCtx->SelectedOwner();
      const Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
      // handle whole object selection
      if (anOwner == anObj->GlobalSelOwner())
      {
        TCollection_AsciiString aName = GetMapOfAIS().Find1 (anObj);
        aName.LeftJustify (20, ' ');
        theDI << aName << " ";
        objInfo (aDetected, anObj, theDI);
        theDI << "\n";
      }
    }

    // process selected sub-shapes
    printLocalSelectionInfo (aCtx, theDI);

    return 0;
  }

  theDI << "Neutral-point state:\n";
  for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anObjIter (GetMapOfAIS());
       anObjIter.More(); anObjIter.Next())
  {
    Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (anObjIter.Key1());
    if (anObj.IsNull())
    {
      continue;
    }

    TCollection_AsciiString aName = anObjIter.Key2();
    aName.LeftJustify (20, ' ');
    theDI << "  " << aName << " ";
    objInfo (aDetected, anObj, theDI);
    theDI << "\n";
  }
  printLocalSelectionInfo (aCtx, theDI);
  Standard_DISABLE_DEPRECATION_WARNINGS
  if (aCtx->HasOpenedContext())
    printLocalSelectionInfo (aCtx->LocalContext(), theDI);
  Standard_ENABLE_DEPRECATION_WARNINGS
  return 0;
}

//=======================================================================
//function : PickObjects
//purpose  :
//=======================================================================
Standard_Boolean  ViewerTest::PickObjects(Handle(TColStd_HArray1OfTransient)& arr,
					  const AIS_KindOfInteractive TheType,
					  const Standard_Integer TheSignature,
					  const Standard_Integer MaxPick)
{
  Handle(AIS_InteractiveObject) IO;
  Standard_DISABLE_DEPRECATION_WARNINGS
  Standard_Integer curindex = (TheType == AIS_KOI_None) ? 0 : TheAISContext()->OpenLocalContext();
  Standard_ENABLE_DEPRECATION_WARNINGS

  // step 1: prepare the data
  if(curindex !=0){
    Handle(AIS_SignatureFilter) F1 = new AIS_SignatureFilter(TheType,TheSignature);
    TheAISContext()->AddFilter(F1);
  }

  // step 2 : wait for the selection...
  Standard_Integer NbPickGood (0),NbToReach(arr->Length());
  Standard_Integer NbPickFail(0);
  Standard_Integer argccc = 5;
  const char *bufff[] = { "A", "B", "C","D", "E" };
  const char **argvvv = (const char **) bufff;


  while(NbPickGood<NbToReach && NbPickFail <= MaxPick){
    while(ViewerMainLoop(argccc,argvvv)){}
    Standard_Integer NbStored = TheAISContext()->NbSelected();
    if(NbStored != NbPickGood)
      NbPickGood= NbStored;
    else
      NbPickFail++;
    cout<<"NbPicked =  "<<NbPickGood<<" |  Nb Pick Fail :"<<NbPickFail<<endl;
  }

  // step3 get result.

  if (NbPickFail >= NbToReach)
    return Standard_False;

  Standard_Integer i(0);
  for(TheAISContext()->InitSelected();
      TheAISContext()->MoreSelected();
      TheAISContext()->NextSelected()){
    i++;
    Handle(AIS_InteractiveObject) IO2 = TheAISContext()->SelectedInteractive();
    arr->SetValue(i,IO2);
  }

  Standard_DISABLE_DEPRECATION_WARNINGS
  if (curindex > 0)
  {
    TheAISContext()->CloseLocalContext(curindex);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  return Standard_True;
}


//=======================================================================
//function : PickObject
//purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) ViewerTest::PickObject(const AIS_KindOfInteractive TheType,
						     const Standard_Integer TheSignature,
						     const Standard_Integer MaxPick)
{
  Handle(AIS_InteractiveObject) IO;
  Standard_DISABLE_DEPRECATION_WARNINGS
  Standard_Integer curindex = (TheType == AIS_KOI_None) ? 0 : TheAISContext()->OpenLocalContext();
  Standard_ENABLE_DEPRECATION_WARNINGS

  // step 1: prepare the data

  if(curindex !=0){
    Handle(AIS_SignatureFilter) F1 = new AIS_SignatureFilter(TheType,TheSignature);
    TheAISContext()->AddFilter(F1);
  }

  // step 2 : wait for the selection...
  Standard_Boolean IsGood (Standard_False);
  Standard_Integer NbPick(0);
  Standard_Integer argccc = 5;
  const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argvvv = (const char **) bufff;


  while(!IsGood && NbPick<= MaxPick){
    while(ViewerMainLoop(argccc,argvvv)){}
    IsGood = (TheAISContext()->NbSelected()>0) ;
    NbPick++;
    cout<<"Nb Pick :"<<NbPick<<endl;
  }


  // step3 get result.
  if(IsGood){
    TheAISContext()->InitSelected();
    IO = TheAISContext()->SelectedInteractive();
  }

  Standard_DISABLE_DEPRECATION_WARNINGS
  if (curindex != 0)
  {
    TheAISContext()->CloseLocalContext(curindex);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS
  return IO;
}

//=======================================================================
//function : PickShape
//purpose  : First Activate the rightmode + Put Filters to be able to
//           pick objets that are of type <TheType>...
//=======================================================================

TopoDS_Shape ViewerTest::PickShape(const TopAbs_ShapeEnum TheType,
				   const Standard_Integer MaxPick)
{

  // step 1: prepare the data
  Standard_DISABLE_DEPRECATION_WARNINGS
  Standard_Integer curindex = TheAISContext()->OpenLocalContext();
  Standard_ENABLE_DEPRECATION_WARNINGS
  TopoDS_Shape result;

  if(TheType==TopAbs_SHAPE){
    Handle(AIS_TypeFilter) F1 = new AIS_TypeFilter(AIS_KOI_Shape);
    TheAISContext()->AddFilter(F1);
  }
  else{
    Handle(StdSelect_ShapeTypeFilter) TF = new StdSelect_ShapeTypeFilter(TheType);
    TheAISContext()->AddFilter(TF);
    Standard_DISABLE_DEPRECATION_WARNINGS
    TheAISContext()->ActivateStandardMode(TheType);
    Standard_ENABLE_DEPRECATION_WARNINGS
  }


  // step 2 : wait for the selection...
  Standard_Boolean NoShape (Standard_True);
  Standard_Integer NbPick(0);
  Standard_Integer argccc = 5;
  const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argvvv = (const char **) bufff;


  while(NoShape && NbPick<= MaxPick){
    while(ViewerMainLoop(argccc,argvvv)){}
    NoShape = (TheAISContext()->NbSelected()==0) ;
    NbPick++;
    cout<<"Nb Pick :"<<NbPick<<endl;
  }

  // step3 get result.

  if(!NoShape){

    TheAISContext()->InitSelected();
    if(TheAISContext()->HasSelectedShape())
      result = TheAISContext()->SelectedShape();
    else{
      Handle(AIS_InteractiveObject) IO = TheAISContext()->SelectedInteractive();
      result = Handle(AIS_Shape)::DownCast (IO)->Shape();
    }
  }

  Standard_DISABLE_DEPRECATION_WARNINGS
  if (curindex > 0)
  {
    TheAISContext()->CloseLocalContext(curindex);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  return result;
}


//=======================================================================
//function : PickShapes
//purpose  :
//=======================================================================
Standard_Boolean ViewerTest::PickShapes (const TopAbs_ShapeEnum TheType,
					 Handle(TopTools_HArray1OfShape)& thearr,
					 const Standard_Integer MaxPick)
{

  Standard_Integer Taille = thearr->Length();
  if(Taille>1)
    cout<<" WARNING : Pick with Shift+ MB1 for Selection of more than 1 object\n";

  // step 1: prepare the data
  Standard_DISABLE_DEPRECATION_WARNINGS
  Standard_Integer curindex = TheAISContext()->OpenLocalContext();
  Standard_ENABLE_DEPRECATION_WARNINGS
  if(TheType==TopAbs_SHAPE){
    Handle(AIS_TypeFilter) F1 = new AIS_TypeFilter(AIS_KOI_Shape);
    TheAISContext()->AddFilter(F1);
  }
  else{
    Handle(StdSelect_ShapeTypeFilter) TF = new StdSelect_ShapeTypeFilter(TheType);
    TheAISContext()->AddFilter(TF);
    Standard_DISABLE_DEPRECATION_WARNINGS
    TheAISContext()->ActivateStandardMode(TheType);
    Standard_ENABLE_DEPRECATION_WARNINGS
  }

  // step 2 : wait for the selection...
  Standard_Integer NbPickGood (0),NbToReach(thearr->Length());
  Standard_Integer NbPickFail(0);
  Standard_Integer argccc = 5;
  const char *bufff[] = { "A", "B", "C","D", "E" };
  const char **argvvv = (const char **) bufff;


  while(NbPickGood<NbToReach && NbPickFail <= MaxPick){
    while(ViewerMainLoop(argccc,argvvv)){}
    Standard_Integer NbStored = TheAISContext()->NbSelected();
    if (NbStored != NbPickGood)
      NbPickGood= NbStored;
    else
      NbPickFail++;
    cout<<"NbPicked =  "<<NbPickGood<<" |  Nb Pick Fail :"<<NbPickFail<<"\n";
  }

  // step3 get result.

  if (NbPickFail >= NbToReach)
    return Standard_False;

  Standard_Integer i(0);
  for(TheAISContext()->InitSelected();TheAISContext()->MoreSelected();TheAISContext()->NextSelected()){
    i++;
    if(TheAISContext()->HasSelectedShape())
      thearr->SetValue(i,TheAISContext()->SelectedShape());
    else{
      Handle(AIS_InteractiveObject) IO = TheAISContext()->SelectedInteractive();
      thearr->SetValue(i,Handle(AIS_Shape)::DownCast (IO)->Shape());
    }
  }

  Standard_DISABLE_DEPRECATION_WARNINGS
  TheAISContext()->CloseLocalContext(curindex);
  Standard_ENABLE_DEPRECATION_WARNINGS
  return Standard_True;
}


//=======================================================================
//function : VPickShape
//purpose  :
//=======================================================================
static int VPickShape( Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  TopoDS_Shape PickSh;
  TopAbs_ShapeEnum theType = TopAbs_COMPOUND;

  if(argc==1)
    theType = TopAbs_SHAPE;
  else{
    if(!strcasecmp(argv[1],"V" )) theType = TopAbs_VERTEX;
    else if (!strcasecmp(argv[1],"E" )) theType = TopAbs_EDGE;
    else if (!strcasecmp(argv[1],"W" )) theType = TopAbs_WIRE;
    else if (!strcasecmp(argv[1],"F" )) theType = TopAbs_FACE;
    else if(!strcasecmp(argv[1],"SHAPE" )) theType = TopAbs_SHAPE;
    else if (!strcasecmp(argv[1],"SHELL" )) theType = TopAbs_SHELL;
    else if (!strcasecmp(argv[1],"SOLID" )) theType = TopAbs_SOLID;
  }

  static Standard_Integer nbOfSub[8]={0,0,0,0,0,0,0,0};
  static TCollection_AsciiString nameType[8] = {"COMPS","SOL","SHE","F","W","E","V","SHAP"};

  TCollection_AsciiString name;


  Standard_Integer NbToPick = argc>2 ? argc-2 : 1;
  if(NbToPick==1){
    PickSh = ViewerTest::PickShape(theType);

    if(PickSh.IsNull())
      return 1;
    if(argc>2){
      name += argv[2];
    }
    else{

      if(!PickSh.IsNull()){
	nbOfSub[Standard_Integer(theType)]++;
	name += "Picked_";
	name += nameType[Standard_Integer(theType)];
	TCollection_AsciiString indxstring(nbOfSub[Standard_Integer(theType)]);
	name +="_";
	name+=indxstring;
      }
    }
    // si on avait une petite methode pour voir si la shape
    // est deja dans la Double map, ca eviterait de creer....
    DBRep::Set(name.ToCString(),PickSh);

    Handle(AIS_Shape) newsh = new AIS_Shape(PickSh);
    GetMapOfAIS().Bind(newsh, name);
    TheAISContext()->Display (newsh, Standard_True);
    di<<"Nom de la shape pickee : "<<name.ToCString()<<"\n";
  }

  // Plusieurs objets a picker, vite vite vite....
  //
  else{
    Standard_Boolean autonaming = !strcasecmp(argv[2],".");
    Handle(TopTools_HArray1OfShape) arr = new TopTools_HArray1OfShape(1,NbToPick);
    if(ViewerTest::PickShapes(theType,arr)){
      for(Standard_Integer i=1;i<=NbToPick;i++){
	PickSh = arr->Value(i);
	if(!PickSh.IsNull()){
	  if(autonaming){
	    nbOfSub[Standard_Integer(theType)]++;
	    name.Clear();
	    name += "Picked_";
	    name += nameType[Standard_Integer(theType)];
	    TCollection_AsciiString indxstring(nbOfSub[Standard_Integer(theType)]);
	    name +="_";
	    name+=indxstring;
	  }
	}
	else
	  name = argv[1+i];

	DBRep::Set(name.ToCString(),PickSh);
	Handle(AIS_Shape) newsh = new AIS_Shape(PickSh);
	GetMapOfAIS().Bind(newsh, name);
	di<<"display of picke shape #"<<i<<" - nom : "<<name.ToCString()<<"\n";
	TheAISContext()->Display (newsh, Standard_False);

      }
      TheAISContext()->UpdateCurrentViewer();
    }
  }
  return 0;
}

//=======================================================================
//function : VSelFilter
//purpose  :
//=======================================================================
static int VSelFilter(Draw_Interpretor& , Standard_Integer theArgc,
                      const char** theArgv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    std::cout << "Error: AIS context is not available.\n";
    return 1;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgc; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgv[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-clear")
    {
      aContext->RemoveFilters();
    }
    else if (anArg == "-type"
          && anArgIter + 1 < theArgc)
    {
      TCollection_AsciiString aVal (theArgv[++anArgIter]);
      TopAbs_ShapeEnum aShapeType = TopAbs_COMPOUND;
      if (!TopAbs::ShapeTypeFromString (aVal.ToCString(), aShapeType))
      {
        std::cout << "Syntax error: wrong command attribute value '" << aVal << "'\n";
        return 1;
      }

      Handle(SelectMgr_Filter) aFilter;
      if (aShapeType == TopAbs_SHAPE)
      {
        aFilter = new AIS_TypeFilter (AIS_KOI_Shape);
      }
      else
      {
        aFilter = new StdSelect_ShapeTypeFilter (aShapeType);
      }
      aContext->AddFilter (aFilter);
    }
    else
    {
      std::cout << "Syntax error: unknown argument '" << theArgv[anArgIter] << "'\n";
      return 1;
    }
  }
  return 0;
}

//=======================================================================
//function : VPickSelected
//purpose  :
//=======================================================================
static int VPickSelected (Draw_Interpretor& , Standard_Integer theArgNb, const char** theArgs)
{
  static Standard_Integer aCount = 0;
  TCollection_AsciiString aName = "PickedShape_";

  if (theArgNb > 1)
  {
    aName = theArgs[1];
  }
  else
  {
    aName = aName + aCount++ + "_";
  }

  Standard_Integer anIdx = 0;
  for (TheAISContext()->InitSelected(); TheAISContext()->MoreSelected(); TheAISContext()->NextSelected(), ++anIdx)
  {
    TopoDS_Shape aShape;
    if (TheAISContext()->HasSelectedShape())
    {
      aShape = TheAISContext()->SelectedShape();
    }
    else
    {
      Handle(AIS_InteractiveObject) IO = TheAISContext()->SelectedInteractive();
      aShape = Handle(AIS_Shape)::DownCast (IO)->Shape();
    }

    TCollection_AsciiString aCurrentName = aName;
    if (anIdx > 0)
    {
      aCurrentName += anIdx;
    }

    DBRep::Set ((aCurrentName).ToCString(), aShape);

    Handle(AIS_Shape) aNewShape = new AIS_Shape (aShape);
    GetMapOfAIS().Bind (aNewShape, aCurrentName);
    TheAISContext()->Display (aNewShape, Standard_False);
  }

  TheAISContext()->UpdateCurrentViewer();

  return 0;
}

//=======================================================================
//function : list of known objects
//purpose  :
//=======================================================================
static int VIOTypes( Draw_Interpretor& di, Standard_Integer , const char** )
{
  //                             1234567890         12345678901234567         123456789
  TCollection_AsciiString Colum [3]={"Standard Types","Type Of Object","Signature"};
  TCollection_AsciiString BlankLine(64,'_');
  Standard_Integer i ;

  di<<"/n"<<BlankLine.ToCString()<<"\n";

  for( i =0;i<=2;i++)
    Colum[i].Center(20,' ');
  for(i=0;i<=2;i++)
    di<<"|"<<Colum[i].ToCString();
  di<<"|\n";

  di<<BlankLine.ToCString()<<"\n";

  //  TCollection_AsciiString thetypes[5]={"Datum","Shape","Object","Relation","None"};
  const char ** names = GetTypeNames();

  TCollection_AsciiString curstring;
  TCollection_AsciiString curcolum[3];


  // les objets de type Datum..
  curcolum[1]+="Datum";
  for(i =0;i<=6;i++){
    curcolum[0].Clear();
    curcolum[0] += names[i];

    curcolum[2].Clear();
    curcolum[2]+=TCollection_AsciiString(i+1);

    for(Standard_Integer j =0;j<=2;j++){
      curcolum[j].Center(20,' ');
      di<<"|"<<curcolum[j].ToCString();
    }
    di<<"|\n";
  }
  di<<BlankLine.ToCString()<<"\n";

  // les objets de type shape
  curcolum[1].Clear();
  curcolum[1]+="Shape";
  curcolum[1].Center(20,' ');

  for(i=0;i<=2;i++){
    curcolum[0].Clear();
    curcolum[0] += names[7+i];
    curcolum[2].Clear();
    curcolum[2]+=TCollection_AsciiString(i);

    for(Standard_Integer j =0;j<=2;j++){
      curcolum[j].Center(20,' ');
      di<<"|"<<curcolum[j].ToCString();
    }
    di<<"|\n";
  }
  di<<BlankLine.ToCString()<<"\n";
  // les IO de type objet...
  curcolum[1].Clear();
  curcolum[1]+="Object";
  curcolum[1].Center(20,' ');
  for(i=0;i<=1;i++){
    curcolum[0].Clear();
    curcolum[0] += names[10+i];
    curcolum[2].Clear();
    curcolum[2]+=TCollection_AsciiString(i);

    for(Standard_Integer j =0;j<=2;j++){
      curcolum[j].Center(20,' ');
      di<<"|"<<curcolum[j].ToCString();
    }
    di<<"|\n";
  }
  di<<BlankLine.ToCString()<<"\n";
  // les contraintes et dimensions.
  // pour l'instant on separe juste contraintes et dimensions...
  // plus tard, on detaillera toutes les sortes...
  curcolum[1].Clear();
  curcolum[1]+="Relation";
  curcolum[1].Center(20,' ');
  for(i=0;i<=1;i++){
    curcolum[0].Clear();
    curcolum[0] += names[12+i];
    curcolum[2].Clear();
    curcolum[2]+=TCollection_AsciiString(i);

    for(Standard_Integer j =0;j<=2;j++){
      curcolum[j].Center(20,' ');
      di<<"|"<<curcolum[j].ToCString();
    }
    di<<"|\n";
  }
  di<<BlankLine.ToCString()<<"\n";


  return 0;
}


static int VEraseType( Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if(argc!=2) return 1;

  AIS_KindOfInteractive TheType;
  Standard_Integer TheSign(-1);
  GetTypeAndSignfromString(argv[1],TheType,TheSign);


  AIS_ListOfInteractive LIO;

  // en attendant l'amelioration ais pour les dimensions...
  //
  Standard_Integer dimension_status(-1);
  if(TheType==AIS_KOI_Relation){
    dimension_status = TheSign ==1 ? 1 : 0;
    TheSign=-1;
  }

  TheAISContext()->DisplayedObjects(TheType,TheSign,LIO);
  Handle(AIS_InteractiveObject) curio;
  for(AIS_ListIteratorOfListOfInteractive it(LIO);it.More();it.Next()){
    curio  = it.Value();

    if(dimension_status == -1)
      TheAISContext()->Erase(curio,Standard_False);
    else {
      AIS_KindOfDimension KOD = Handle(AIS_Relation)::DownCast (curio)->KindOfDimension();
      if ((dimension_status==0 && KOD == AIS_KOD_NONE)||
	  (dimension_status==1 && KOD != AIS_KOD_NONE))
	TheAISContext()->Erase(curio,Standard_False);
    }
  }
  TheAISContext()->UpdateCurrentViewer();
  return 0;
}
static int VDisplayType(Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if(argc!=2) return 1;

  AIS_KindOfInteractive TheType;
  Standard_Integer TheSign(-1);
  GetTypeAndSignfromString(argv[1],TheType,TheSign);

  // en attendant l'amelioration ais pour les dimensions...
  //
  Standard_Integer dimension_status(-1);
  if(TheType==AIS_KOI_Relation){
    dimension_status = TheSign ==1 ? 1 : 0;
    TheSign=-1;
  }

  AIS_ListOfInteractive LIO;
  TheAISContext()->ObjectsInside(LIO,TheType,TheSign);
  Handle(AIS_InteractiveObject) curio;
  for(AIS_ListIteratorOfListOfInteractive it(LIO);it.More();it.Next()){
    curio  = it.Value();
    if(dimension_status == -1)
      TheAISContext()->Display(curio,Standard_False);
    else {
      AIS_KindOfDimension KOD = Handle(AIS_Relation)::DownCast (curio)->KindOfDimension();
      if ((dimension_status==0 && KOD == AIS_KOD_NONE)||
	  (dimension_status==1 && KOD != AIS_KOD_NONE))
	TheAISContext()->Display(curio,Standard_False);
    }

  }

  TheAISContext()->UpdateCurrentViewer();
  return 0;
}

static Standard_Integer vr(Draw_Interpretor& , Standard_Integer , const char** a)
{
  ifstream s(a[1]);
  BRep_Builder builder;
  TopoDS_Shape shape;
  BRepTools::Read(shape, s, builder);
  DBRep::Set(a[1], shape);
  Handle(AIS_InteractiveContext) Ctx = ViewerTest::GetAISContext();
  Handle(AIS_Shape) ais = new AIS_Shape(shape);
  Ctx->Display (ais, Standard_True);
  return 0;
}

//===============================================================================================
//function : VBsdf
//purpose  :
//===============================================================================================
static int VBsdf (Draw_Interpretor& theDI,
                  Standard_Integer  theArgsNb,
                  const char**      theArgVec)
{
  Handle(V3d_View)   aView   = ViewerTest::CurrentView();
  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  if (aView.IsNull()
   || aViewer.IsNull())
  {
    std::cerr << "No active viewer!\n";
    return 1;
  }

  ViewerTest_CmdParser aCmd;

  aCmd.AddDescription ("Adjusts parameters of material BSDF:");

  aCmd.AddOption ("print|echo|p", "Prints BSDF");

  aCmd.AddOption ("noupdate|update", "Suppresses viewer redraw call");

  aCmd.AddOption ("kc", "Weight of coat specular/glossy BRDF");
  aCmd.AddOption ("kd", "Weight of base diffuse BRDF");
  aCmd.AddOption ("ks", "Weight of base specular/glossy BRDF");
  aCmd.AddOption ("kt", "Weight of base specular/glossy BTDF");
  aCmd.AddOption ("le", "Radiance emitted by surface");

  aCmd.AddOption ("coatFresnel|cf", "Fresnel reflectance of coat layer. Allowed formats: Constant R, Schlick R G B, Dielectric N, Conductor N K");
  aCmd.AddOption ("baseFresnel|bf", "Fresnel reflectance of base layer. Allowed formats: Constant R, Schlick R G B, Dielectric N, Conductor N K");

  aCmd.AddOption ("coatRoughness|cr", "Roughness of coat glossy BRDF");
  aCmd.AddOption ("baseRoughness|br", "Roughness of base glossy BRDF");

  aCmd.AddOption ("absorpCoeff|af", "Absorption coeff of base transmission BTDF");
  aCmd.AddOption ("absorpColor|ac", "Absorption color of base transmission BTDF");

  aCmd.AddOption ("normalize|n", "Normalizes BSDF to ensure energy conservation");

  aCmd.Parse (theArgsNb, theArgVec);

  if (aCmd.HasOption ("help"))
  {
    theDI.PrintHelp (theArgVec[0]);
    return 0;
  }

  // check viewer update mode
  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), ViewerTest::CurrentView());

  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    if (anUpdateTool.parseRedrawMode (theArgVec[anArgIter]))
    {
      break;
    }
  }

  TCollection_AsciiString aName (aCmd.Arg ("", 0).c_str());

  // find object
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  if (!aMap.IsBound2 (aName) )
  {
    std::cerr << "Use 'vdisplay' before\n";
    return 1;
  }

  Handle(AIS_InteractiveObject) anIObj = Handle(AIS_InteractiveObject)::DownCast (aMap.Find2 (aName));
  Graphic3d_MaterialAspect aMaterial = anIObj->Attributes()->ShadingAspect()->Material();
  Graphic3d_BSDF aBSDF = aMaterial.BSDF();

  if (aCmd.HasOption ("print"))
  {
    theDI << "\n"
      << "Kc:               " << aBSDF.Kc.r() << ", " << aBSDF.Kc.g() << ", " << aBSDF.Kc.b() << "\n"
      << "Kd:               " << aBSDF.Kd.r() << ", " << aBSDF.Kd.g() << ", " << aBSDF.Kd.b() << "\n"
      << "Ks:               " << aBSDF.Ks.r() << ", " << aBSDF.Ks.g() << ", " << aBSDF.Ks.b() << "\n"
      << "Kt:               " << aBSDF.Kt.r() << ", " << aBSDF.Kt.g() << ", " << aBSDF.Kt.b() << "\n"
      << "Le:               " << aBSDF.Le.r() << ", " << aBSDF.Le.g() << ", " << aBSDF.Le.b() << "\n";

    for (int aLayerID = 0; aLayerID < 2; ++aLayerID)
    {
      const Graphic3d_Vec4 aFresnel = aLayerID < 1 ? aBSDF.FresnelCoat.Serialize()
                                                   : aBSDF.FresnelBase.Serialize();

      theDI << (aLayerID < 1 ? "Coat Fresnel:     "
                             : "Base Fresnel:     ");

      if (aFresnel.x() >= 0.f)
      {
        theDI << "Schlick " << "R = " << aFresnel.r() << ", "
                            << "G = " << aFresnel.g() << ", "
                            << "B = " << aFresnel.b() << "\n";
      }
      else if (aFresnel.x() >= -1.5f)
      {
        theDI << "Constant " << aFresnel.z() << "\n";
      }
      else if (aFresnel.x() >= -2.5f)
      {
        theDI << "Conductor " << "N = " << aFresnel.y() << ", "
                              << "K = " << aFresnel.z() << "\n";
      }
      else
      {
        theDI << "Dielectric " << "N = " << aFresnel.y() << "\n";
      }
    }

    theDI << "Coat roughness:   " << aBSDF.Kc.w() << "\n"
          << "Base roughness:   " << aBSDF.Ks.w() << "\n"
          << "Absorption coeff: " << aBSDF.Absorption.w() << "\n"
          << "Absorption color: " << aBSDF.Absorption.r() << ", "
                                  << aBSDF.Absorption.g() << ", "
                                  << aBSDF.Absorption.b() << "\n";

    return 0;
  }

  if (aCmd.HasOption ("coatRoughness", 1, Standard_True))
  {
    aBSDF.Kc.w() = aCmd.ArgFloat ("coatRoughness");
  }

  if (aCmd.HasOption ("baseRoughness", 1, Standard_True))
  {
    aBSDF.Ks.w () = aCmd.ArgFloat ("baseRoughness");
  }

  if (aCmd.HasOption ("absorpCoeff", 1, Standard_True))
  {
    aBSDF.Absorption.w() = aCmd.ArgFloat ("absorpCoeff");
  }

  if (aCmd.HasOption ("absorpColor", 3, Standard_True))
  {
    const Graphic3d_Vec3 aRGB = aCmd.ArgVec3f ("absorpColor");

    aBSDF.Absorption.r() = aRGB.r();
    aBSDF.Absorption.g() = aRGB.g();
    aBSDF.Absorption.b() = aRGB.b();
  }

  if (aCmd.HasOption ("kc", 3) || aCmd.HasOption ("kc", 1, Standard_True))
  {
    Graphic3d_Vec3 aKc;

    if (aCmd.HasOption ("kc", 3))
    {
      aKc = aCmd.ArgVec3f ("kc");
    }
    else
    {
      aKc = Graphic3d_Vec3 (aCmd.ArgFloat ("kc"));
    }

    aBSDF.Kc.r() = aKc.r();
    aBSDF.Kc.g() = aKc.g();
    aBSDF.Kc.b() = aKc.b();
  }

  if (aCmd.HasOption ("kd", 3))
  {
    aBSDF.Kd = aCmd.ArgVec3f ("kd");
  }
  else if (aCmd.HasOption ("kd", 1, Standard_True))
  {
    aBSDF.Kd = Graphic3d_Vec3 (aCmd.ArgFloat ("kd"));
  }

  if (aCmd.HasOption ("ks", 3) || aCmd.HasOption ("ks", 1, Standard_True))
  {
    Graphic3d_Vec3 aKs;

    if (aCmd.HasOption ("ks", 3))
    {
      aKs = aCmd.ArgVec3f ("ks");
    }
    else
    {
      aKs = Graphic3d_Vec3 (aCmd.ArgFloat ("ks"));
    }

    aBSDF.Ks.r() = aKs.r();
    aBSDF.Ks.g() = aKs.g();
    aBSDF.Ks.b() = aKs.b();
  }

  if (aCmd.HasOption ("kt", 3))
  {
    aBSDF.Kt = aCmd.ArgVec3f ("kt");
  }
  else if (aCmd.HasOption ("kt", 1, Standard_True))
  {
    aBSDF.Kt = Graphic3d_Vec3 (aCmd.ArgFloat ("kt"));
  }

  if (aCmd.HasOption ("le", 3))
  {
    aBSDF.Le = aCmd.ArgVec3f ("le");
  }
  else if (aCmd.HasOption ("le", 1, Standard_True))
  {
    aBSDF.Le = Graphic3d_Vec3 (aCmd.ArgFloat ("le"));
  }

  const std::string aFresnelErrorMessage =
    "Error! Wrong Fresnel type. Allowed types are: Constant F, Schlick R G B, Dielectric N, Conductor N K\n";

  for (int aLayerID = 0; aLayerID < 2; ++aLayerID)
  {
    const std::string aFresnel = aLayerID < 1 ? "baseFresnel"
                                              : "coatFresnel";

    if (aCmd.HasOption (aFresnel, 4)) // Schlick: type R G B
    {
      std::string aFresnelType = aCmd.Arg (aFresnel, 0);
      std::transform (aFresnelType.begin (), aFresnelType.end (), aFresnelType.begin (), ::LowerCase);

      if (aFresnelType == "schlick")
      {
        Graphic3d_Vec3 aRGB (static_cast<float> (Draw::Atof (aCmd.Arg (aFresnel, 1).c_str())),
                             static_cast<float> (Draw::Atof (aCmd.Arg (aFresnel, 2).c_str())),
                             static_cast<float> (Draw::Atof (aCmd.Arg (aFresnel, 3).c_str())));

        aRGB.r() = std::min (std::max (aRGB.r(), 0.f), 1.f);
        aRGB.g() = std::min (std::max (aRGB.g(), 0.f), 1.f);
        aRGB.b() = std::min (std::max (aRGB.b(), 0.f), 1.f);

        (aLayerID < 1 ? aBSDF.FresnelBase : aBSDF.FresnelCoat) = Graphic3d_Fresnel::CreateSchlick (aRGB);
      }
      else
      {
        theDI << aFresnelErrorMessage.c_str() << "\n";
      }
    }
    else if (aCmd.HasOption (aFresnel, 3)) // Conductor: type N K
    {
      std::string aFresnelType = aCmd.Arg (aFresnel, 0);
      std::transform (aFresnelType.begin (), aFresnelType.end (), aFresnelType.begin (), ::LowerCase);

      if (aFresnelType == "conductor")
      {
        const float aN = static_cast<float> (Draw::Atof (aCmd.Arg (aFresnel, 1).c_str()));
        const float aK = static_cast<float> (Draw::Atof (aCmd.Arg (aFresnel, 2).c_str()));

        (aLayerID < 1 ? aBSDF.FresnelBase : aBSDF.FresnelCoat) = Graphic3d_Fresnel::CreateConductor (aN, aK);
      }
      else
      {
        theDI << aFresnelErrorMessage.c_str() << "\n";
      }
    }
    else if (aCmd.HasOption (aFresnel, 2)) // Dielectric or Constant: type N|C
    {
      std::string aFresnelType = aCmd.Arg (aFresnel, 0);
      std::transform (aFresnelType.begin (), aFresnelType.end (), aFresnelType.begin (), ::LowerCase);

      if (aFresnelType == "constant")
      {
        const float aR = static_cast<float> (Draw::Atof (aCmd.Arg (aFresnel, 1).c_str()));

        (aLayerID < 1 ? aBSDF.FresnelBase : aBSDF.FresnelCoat) = Graphic3d_Fresnel::CreateConstant (aR);
      }
      else if (aFresnelType == "dielectric")
      {
        const float aN = static_cast<float> (Draw::Atof (aCmd.Arg (aFresnel, 1).c_str()));

        (aLayerID < 1 ? aBSDF.FresnelBase : aBSDF.FresnelCoat) = Graphic3d_Fresnel::CreateDielectric (aN);
      }
      else
      {
        theDI << aFresnelErrorMessage.c_str() << "\n";
      }
    }
  }

  if (aCmd.HasOption ("normalize"))
  {
    aBSDF.Normalize();
  }

  aMaterial.SetBSDF (aBSDF);
  anIObj->SetMaterial (aMaterial);

  return 0;
}

//==============================================================================
//function : VLoadSelection
//purpose  : Adds given objects to map of AIS and loads selection primitives for them
//==============================================================================
static Standard_Integer VLoadSelection (Draw_Interpretor& /*theDi*/,
                                        Standard_Integer theArgNb,
                                        const char** theArgVec)
{
  if (theArgNb < 2)
  {
    std::cerr << theArgVec[0] << "Error: wrong number of arguments.\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    ViewerTest::ViewerInit();
    aCtx = ViewerTest::GetAISContext();
  }

  // Parse input arguments
  TColStd_SequenceOfAsciiString aNamesOfIO;
  Standard_Boolean isLocal = Standard_False;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    const TCollection_AsciiString aName     = theArgVec[anArgIter];
    TCollection_AsciiString       aNameCase = aName;
    aNameCase.LowerCase();
    if (aNameCase == "-local")
    {
      isLocal = Standard_True;
    }
    else
    {
      aNamesOfIO.Append (aName);
    }
  }

  if (aNamesOfIO.IsEmpty())
  {
    std::cerr << theArgVec[0] << "Error: wrong number of arguments.\n";
    return 1;
  }

  // Prepare context
  Standard_DISABLE_DEPRECATION_WARNINGS
  if (isLocal && !aCtx->HasOpenedContext())
  {
    aCtx->OpenLocalContext (Standard_False);
  }
  else if (!isLocal && aCtx->HasOpenedContext())
  {
    aCtx->CloseAllContexts (Standard_False);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  // Load selection of interactive objects
  for (Standard_Integer anIter = 1; anIter <= aNamesOfIO.Length(); ++anIter)
  {
    const TCollection_AsciiString& aName = aNamesOfIO.Value (anIter);

    Handle(AIS_InteractiveObject) aShape;
    if (GetMapOfAIS().IsBound2 (aName))
      aShape = Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (aName));
    else
      aShape = GetAISShapeFromName (aName.ToCString());

    if (!aShape.IsNull())
    {
      if (!GetMapOfAIS().IsBound2 (aName))
      {
        GetMapOfAIS().Bind (aShape, aName);
      }

      aCtx->Load (aShape, -1, Standard_False);
      aCtx->Activate (aShape, aShape->GlobalSelectionMode(), Standard_True);
    }
  }

  return 0;
}

//==============================================================================
//function : ViewerTest::Commands
//purpose  : Add all the viewer command in the Draw_Interpretor
//==============================================================================

void ViewerTest::Commands(Draw_Interpretor& theCommands)
{
  ViewerTest::ViewerCommands(theCommands);
  ViewerTest::RelationCommands(theCommands);
  ViewerTest::ObjectCommands(theCommands);
  ViewerTest::FilletCommands(theCommands);
  ViewerTest::OpenGlCommands(theCommands);

  const char *group = "AIS_Display";

  // display
  theCommands.Add("visos",
      "visos [name1 ...] [nbUIsos nbVIsos IsoOnPlane(0|1)]\n"
      "\tIf last 3 optional parameters are not set prints numbers of U-, V- isolines and IsoOnPlane.\n",
      __FILE__, visos, group);

  theCommands.Add("vdisplay",
              "vdisplay [-noupdate|-update] [-local] [-mutable] [-neutral]"
      "\n\t\t:          [-trsfPers {zoom|rotate|zoomRotate|none}=none]"
      "\n\t\t:                            [-trsfPersPos X Y [Z]] [-3d]"
      "\n\t\t:          [-2d|-trihedron [{top|bottom|left|right|topLeft"
      "\n\t\t:                           |topRight|bottomLeft|bottomRight}"
      "\n\t\t:                                         [offsetX offsetY]]]"
      "\n\t\t:          [-dispMode mode] [-highMode mode]"
      "\n\t\t:          [-layer index] [-top|-topmost|-overlay|-underlay]"
      "\n\t\t:          [-redisplay]"
      "\n\t\t:          name1 [name2] ... [name n]"
      "\n\t\t: Displays named objects."
      "\n\t\t: Option -local enables displaying of objects in local"
      "\n\t\t: selection context. Local selection context will be opened"
      "\n\t\t: if there is not any."
      "\n\t\t:  -noupdate    Suppresses viewer redraw call."
      "\n\t\t:  -mutable     Enables optimizations for mutable objects."
      "\n\t\t:  -neutral     Draws objects in main viewer."
      "\n\t\t:  -layer       Sets z-layer for objects."
      "\n\t\t:               Alternatively -overlay|-underlay|-top|-topmost"
      "\n\t\t:               options can be used for the default z-layers."
      "\n\t\t:  -top         Draws object on top of main presentations"
      "\n\t\t:               but below topmost."
      "\n\t\t:  -topmost     Draws in overlay for 3D presentations."
      "\n\t\t:               with independent Depth."
      "\n\t\t:  -overlay     Draws objects in overlay for 2D presentations."
      "\n\t\t:               (On-Screen-Display)"
      "\n\t\t:  -underlay    Draws objects in underlay for 2D presentations."
      "\n\t\t:               (On-Screen-Display)"
      "\n\t\t:  -selectable|-noselect Controls selection of objects."
      "\n\t\t:  -trsfPers    Sets a transform persistence flags."
      "\n\t\t:  -trsfPersPos Sets an anchor point for transform persistence."
      "\n\t\t:  -2d          Displays object in screen coordinates."
      "\n\t\t:               (DY looks up)"
      "\n\t\t:  -dispmode    Sets display mode for objects."
      "\n\t\t:  -highmode    Sets hilight mode for objects."
      "\n\t\t:  -redisplay   Recomputes presentation of objects.",
      __FILE__, VDisplay2, group);

  theCommands.Add ("vnbdisplayed",
      "vnbdisplayed"
      "\n\t\t: Returns number of displayed objects",
      __FILE__, VNbDisplayed, group);

  theCommands.Add ("vupdate",
      "vupdate name1 [name2] ... [name n]"
      "\n\t\t: Updates named objects in interactive context",
      __FILE__, VUpdate, group);

  theCommands.Add("verase",
      "verase [-noupdate|-update] [-local] [name1] ...  [name n]"
      "\n\t\t: Erases selected or named objects."
      "\n\t\t: If there are no selected or named objects the whole viewer is erased."
      "\n\t\t: Option -local enables erasing of selected or named objects without"
      "\n\t\t: closing local selection context.",
      __FILE__, VErase, group);

  theCommands.Add("vremove",
      "vremove [-noupdate|-update] [-context] [-all] [-noinfo] [name1] ...  [name n]"
      "or vremove [-context] -all to remove all objects"
      "\n\t\t: Removes selected or named objects."
      "\n\t\t  If -context is in arguments, the objects are not deleted"
      "\n\t\t  from the map of objects and names."
      "\n\t\t: Option -local enables removing of selected or named objects without"
      "\n\t\t: closing local selection context. Empty local selection context will be"
      "\n\t\t: closed."
      "\n\t\t: Option -noupdate suppresses viewer redraw call."
      "\n\t\t: Option -noinfo suppresses displaying the list of removed objects.",
      __FILE__, VRemove, group);

  theCommands.Add("vdonly",
		  "vdonly [-noupdate|-update] [name1] ...  [name n]"
      "\n\t\t: Displays only selected or named objects",
		  __FILE__,VDonly2,group);

  theCommands.Add("vdisplayall",
      "vidsplayall [-local]"
      "\n\t\t: Displays all erased interactive objects (see vdir and vstate)."
      "\n\t\t: Option -local enables displaying of the objects in local"
      "\n\t\t: selection context.",
      __FILE__, VDisplayAll, group);

  theCommands.Add("veraseall",
      "veraseall [-local]"
      "\n\t\t: Erases all objects displayed in the viewer."
      "\n\t\t: Option -local enables erasing of the objects in local"
      "\n\t\t: selection context.",
      __FILE__, VErase, group);

  theCommands.Add("verasetype",
      "verasetype <Type>"
      "\n\t\t: Erase all the displayed objects of one given kind (see vtypes)",
      __FILE__, VEraseType, group);
  theCommands.Add("vbounding",
              "vbounding [-noupdate|-update] [-mode] name1 [name2 [...]]"
      "\n\t\t:           [-print] [-hide]"
      "\n\t\t: Temporarily display bounding box of specified Interactive"
      "\n\t\t: Objects, or print it to console if -print is specified."
      "\n\t\t: Already displayed box might be hidden by -hide option.",
		  __FILE__,VBounding,group);

  theCommands.Add("vdisplaytype",
		  "vdisplaytype        : vdisplaytype <Type> <Signature> \n\t display all the objects of one given kind (see vtypes) which are stored the AISContext ",
		  __FILE__,VDisplayType,group);

  theCommands.Add("vsetdispmode",
		  "vsetdispmode [name] mode(1,2,..)"
      "\n\t\t: Sets display mode for all, selected or named objects.",
		  __FILE__,VDispMode,group);

  theCommands.Add("vunsetdispmode",
		  "vunsetdispmode [name]"
      "\n\t\t: Unsets custom display mode for selected or named objects.",
		  __FILE__,VDispMode,group);

  theCommands.Add("vdir",
		  "Lists all objects displayed in 3D viewer",
		  __FILE__,VDir,group);

#ifdef HAVE_FREEIMAGE
  #define DUMP_FORMATS "{png|bmp|jpg|gif}"
#else
  #define DUMP_FORMATS "{ppm}"
#endif
  theCommands.Add("vdump",
              "vdump <filename>." DUMP_FORMATS " [-width Width -height Height]"
      "\n\t\t:       [-buffer rgb|rgba|depth=rgb]"
      "\n\t\t:       [-stereo mono|left|right|blend|sideBySide|overUnder=mono]"
      "\n\t\t:       [-tileSize Size=0]"
      "\n\t\t: Dumps content of the active view into image file",
		  __FILE__,VDump,group);

  theCommands.Add("vsub",      "vsub 0/1 (off/on) [obj]        : Subintensity(on/off) of selected objects",
		  __FILE__,VSubInt,group);

  theCommands.Add("vaspects",
              "vaspects [-noupdate|-update] [name1 [name2 [...]] | -defaults]"
      "\n\t\t:          [-setVisibility 0|1]"
      "\n\t\t:          [-setColor ColorName] [-setcolor R G B] [-unsetColor]"
      "\n\t\t:          [-setMaterial MatName] [-unsetMaterial]"
      "\n\t\t:          [-setTransparency Transp] [-unsetTransparency]"
      "\n\t\t:          [-setWidth LineWidth] [-unsetWidth]"
      "\n\t\t:          [-setLineType {solid|dash|dot|dotDash}] [-unsetLineType]"
      "\n\t\t:          [-setMarkerType {.|+|x|O|xcircle|pointcircle|ring1|ring2|ring3|ball|ImagePath}]"
      "\n\t\t:          [-unsetMarkerType]"
      "\n\t\t:          [-setMarkerSize Scale] [-unsetMarkerSize]"
      "\n\t\t:          [-freeBoundary {off/on | 0/1}]"
      "\n\t\t:          [-setFreeBoundaryWidth Width] [-unsetFreeBoundaryWidth]"
      "\n\t\t:          [-setFreeBoundaryColor {ColorName | R G B}] [-unsetFreeBoundaryColor]"
      "\n\t\t:          [-subshapes subname1 [subname2 [...]]]"
      "\n\t\t:          [-isoontriangulation 0|1]"
      "\n\t\t:          [-setMaxParamValue {value}]"
      "\n\t\t:          [-setSensitivity {selection_mode} {value}]"
      "\n\t\t:          [-setHatch HatchStyle]"
      "\n\t\t: Manage presentation properties of all, selected or named objects."
      "\n\t\t: When -subshapes is specified than following properties will be"
      "\n\t\t: assigned to specified sub-shapes."
      "\n\t\t: When -defaults is specified than presentation properties will be"
      "\n\t\t: assigned to all objects that have not their own specified properties"
      "\n\t\t: and to all objects to be displayed in the future."
      "\n\t\t: If -defaults is used there should not be any objects' names and -subshapes specifier.",
		  __FILE__,VAspects,group);

  theCommands.Add("vsetcolor",
      "vsetcolor [-noupdate|-update] [name] ColorName"
      "\n\t\t: Sets color for all, selected or named objects."
      "\n\t\t: Alias for vaspects -setcolor [name] ColorName.",
		  __FILE__,VAspects,group);

  theCommands.Add("vunsetcolor",
		  "vunsetcolor [-noupdate|-update] [name]"
      "\n\t\t: Resets color for all, selected or named objects."
      "\n\t\t: Alias for vaspects -unsetcolor [name].",
		  __FILE__,VAspects,group);

  theCommands.Add("vsettransparency",
		  "vsettransparency [-noupdate|-update] [name] Coefficient"
      "\n\t\t: Sets transparency for all, selected or named objects."
      "\n\t\t: The Coefficient may be between 0.0 (opaque) and 1.0 (fully transparent)."
      "\n\t\t: Alias for vaspects -settransp [name] Coefficient.",
		  __FILE__,VAspects,group);

  theCommands.Add("vunsettransparency",
		  "vunsettransparency [-noupdate|-update] [name]"
      "\n\t\t: Resets transparency for all, selected or named objects."
      "\n\t\t: Alias for vaspects -unsettransp [name].",
		  __FILE__,VAspects,group);

  theCommands.Add("vsetmaterial",
		  "vsetmaterial [-noupdate|-update] [name] MaterialName"
      "\n\t\t: Alias for vaspects -setmaterial [name] MaterialName.",
		  __FILE__,VAspects,group);

  theCommands.Add("vunsetmaterial",
		  "vunsetmaterial [-noupdate|-update] [name]"
      "\n\t\t: Alias for vaspects -unsetmaterial [name].",
		  __FILE__,VAspects,group);

  theCommands.Add("vsetwidth",
		  "vsetwidth [-noupdate|-update] [name] width(0->10)"
      "\n\t\t: Alias for vaspects -setwidth [name] width.",
		  __FILE__,VAspects,group);

  theCommands.Add("vunsetwidth",
		  "vunsetwidth [-noupdate|-update] [name]"
      "\n\t\t: Alias for vaspects -unsetwidth [name] width.",
		  __FILE__,VAspects,group);

  theCommands.Add("vsetinteriorstyle",
		  "vsetinteriorstyle [-noupdate|-update] [name] style"
      "\n\t\t: Where style is: 0 = EMPTY, 1 = HOLLOW, 2 = HATCH, 3 = SOLID, 4 = HIDDENLINE.",
		  __FILE__,VSetInteriorStyle,group);

  theCommands.Add("vsensdis",
      "vsensdis : Display active entities (sensitive entities of one of the standard types corresponding to active selection modes)."
      "\n\t\t: Standard entity types are those defined in Select3D package:"
      "\n\t\t: - sensitive box"
      "\n\t\t: - sensitive face"
      "\n\t\t: - sensitive curve"
      "\n\t\t: - sensitive segment"
      "\n\t\t: - sensitive circle"
      "\n\t\t: - sensitive point"
      "\n\t\t: - sensitive triangulation"
      "\n\t\t: - sensitive triangle"
      "\n\t\t: Custom(application - defined) sensitive entity types are not processed by this command.",
      __FILE__,VDispSensi,group);

  theCommands.Add("vsensera",
      "vsensera : erase active entities",
      __FILE__,VClearSensi,group);

  theCommands.Add("vsetshading",
      "vsetshading  : vsetshading name Quality(default=0.0008) "
      "\n\t\t: Sets deflection coefficient that defines the quality of the shape representation in the shading mode.",
      __FILE__,VShading,group);

  theCommands.Add("vunsetshading",
      "vunsetshading :vunsetshading name "
      "\n\t\t: Sets default deflection coefficient (0.0008) that defines the quality of the shape representation in the shading mode.",
      __FILE__,VShading,group);

  theCommands.Add ("vtexture",
                   "vtexture [-noupdate|-update] name [ImageFile|IdOfTexture|off]"
                   "\n\t\t:          [-tex0 Image0] [-tex1 Image1] [...]"
                   "\n\t\t:          [-origin {u v|off}] [-scale {u v|off}] [-repeat {u v|off}]"
                   "\n\t\t:          [-trsfTrans du dv] [-trsfScale su sv] [-trsfAngle Angle]"
                   "\n\t\t:          [-modulate {on|off}]"
                   "\n\t\t:          [-setFilter {nearest|bilinear|trilinear}]"
                   "\n\t\t:          [-setAnisoFilter {off|low|middle|quality}]"
                   "\n\t\t:          [-default]"
                   "\n\t\t: The texture can be specified by filepath"
                   "\n\t\t: or as ID (0<=IdOfTexture<=20) specifying one of the predefined textures."
                   "\n\t\t: The options are:"
                   "\n\t\t:   -scale     Setup texture scaling for generating coordinates; (1, 1) by default"
                   "\n\t\t:   -origin    Setup texture origin  for generating coordinates; (0, 0) by default"
                   "\n\t\t:   -repeat    Setup texture repeat  for generating coordinates; (1, 1) by default"
                   "\n\t\t:   -modulate  Enable or disable texture color modulation"
                   "\n\t\t:   -trsfAngle Setup dynamic texture coordinates transformation - rotation angle"
                   "\n\t\t:   -trsfTrans Setup dynamic texture coordinates transformation - translation vector"
                   "\n\t\t:   -trsfScale Setup dynamic texture coordinates transformation - scale vector"
                   "\n\t\t:   -setFilter Setup texture filter"
                   "\n\t\t:   -setAnisoFilter Setup anisotropic filter for texture with mip-levels"
                   "\n\t\t:   -default   Sets texture mapping default parameters",
                    __FILE__, VTexture, group);

  theCommands.Add("vtexscale",
                  "vtexscale name ScaleU ScaleV"
                  "\n\t\t: Alias for vtexture name -setScale ScaleU ScaleV.",
		  __FILE__,VTexture,group);

  theCommands.Add("vtexorigin",
                  "vtexorigin name OriginU OriginV"
                  "\n\t\t: Alias for vtexture name -setOrigin OriginU OriginV.",
		  __FILE__,VTexture,group);

  theCommands.Add("vtexrepeat",
                  "vtexrepeat name RepeatU RepeatV"
                  "\n\t\t: Alias for vtexture name -setRepeat RepeatU RepeatV.",
		  VTexture,group);

  theCommands.Add("vtexdefault",
                  "vtexdefault name"
                  "\n\t\t: Alias for vtexture name -default.",
		  VTexture,group);

  theCommands.Add("vsetam",
      "vsetam [shapename] mode"
      "\n\t\t: Activates selection mode for all selected or named shapes."
      "\n\t\t: Mod can be:"
      "\n\t\t:   0 - for shape itself" 
      "\n\t\t:   1 - vertices"
      "\n\t\t:   2 - edges"
      "\n\t\t:   3 - wires"
      "\n\t\t:   4 - faces"
      "\n\t\t:   5 - shells"
      "\n\t\t:   6 - solids"
      "\n\t\t:   7 - compounds"
      __FILE__,VActivatedMode,group);

  theCommands.Add("vunsetam",
      "vunsetam : Deactivates all selection modes for all shapes.",
      __FILE__,VActivatedMode,group);

  theCommands.Add("vstate",
      "vstate [-entities] [-hasSelected] [name1] ... [nameN]"
      "\n\t\t: Reports show/hidden state for selected or named objects"
      "\n\t\t:   -entities - print low-level information about detected entities"
      "\n\t\t:   -hasSelected - prints 1 if context has selected shape and 0 otherwise",
		  __FILE__,VState,group);

  theCommands.Add("vpickshapes",
		  "vpickshape subtype(VERTEX,EDGE,WIRE,FACE,SHELL,SOLID) [name1 or .] [name2 or .] [name n or .]",
		  __FILE__,VPickShape,group);

  theCommands.Add("vtypes",
		  "vtypes : list of known types and signatures in AIS - To be Used in vpickobject command for selection with filters",
		  VIOTypes,group);

  theCommands.Add("vr",
      "vr filename"
      "\n\t\t: Reads shape from BREP-format file and displays it in the viewer. ",
		  __FILE__,vr, group);

  theCommands.Add("vselfilter",
                  "vselfilter [-type {VERTEX|EDGE|WIRE|FACE|SHAPE|SHELL|SOLID}] [-clear]"
    "\nSets selection shape type filter in context or remove all filters."
    "\n    : Option -type set type of selection filter. Filters are applyed with Or combination."
    "\n    : Option -clear remove all filters in context",
		  __FILE__,VSelFilter,group);

  theCommands.Add("vpickselected", "vpickselected [name]: extract selected shape.",
    __FILE__, VPickSelected, group);

  theCommands.Add ("vloadselection",
    "vloadselection [-context] [name1] ... [nameN] : allows to load selection"
    "\n\t\t: primitives for the shapes with names given without displaying them."
    "\n\t\t:   -local - open local context before selection computation",
    __FILE__, VLoadSelection, group);

  theCommands.Add("vbsdf", "vbsdf [name] [options]"
    "\nAdjusts parameters of material BSDF:"
    "\n    -help : Shows this message"
    "\n    -print : Print BSDF"
    "\n    -kd : Weight of the Lambertian BRDF"
    "\n    -kr : Weight of the reflection BRDF"
    "\n    -kt : Weight of the transmission BTDF"
    "\n    -ks : Weight of the glossy Blinn BRDF"
    "\n    -le : Self-emitted radiance"
    "\n    -fresnel : Fresnel coefficients; Allowed fresnel formats are: Constant x,"
    "\n               Schlick x y z, Dielectric x, Conductor x y"
    "\n    -roughness : Roughness of material (Blinn's exponent)"
    "\n    -absorpcoeff : Absorption coefficient (only for transparent material)"
    "\n    -absorpcolor : Absorption color (only for transparent material)"
    "\n    -normalize : Normalize BSDF coefficients",
    __FILE__, VBsdf, group);

}

//=====================================================================
//========================= for testing Draft and Rib =================
//=====================================================================
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <DBRep.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pln.hxx>
#include <AIS_KindOfSurface.hxx>
#include <BRepOffsetAPI_DraftAngle.hxx>
#include <Precision.hxx>
#include <BRepAlgo.hxx>
#include <OSD_Environment.hxx>
#include <DrawTrSurf.hxx>
//#include <DbgTools.hxx>
//#include <FeatAlgo_MakeLinearForm.hxx>




//=======================================================================
//function : IsValid
//purpose  :
//=======================================================================
static Standard_Boolean IsValid(const TopTools_ListOfShape& theArgs,
				const TopoDS_Shape& theResult,
				const Standard_Boolean closedSolid,
				const Standard_Boolean GeomCtrl)
{
  OSD_Environment check ("DONT_SWITCH_IS_VALID") ;
  TCollection_AsciiString checkValid = check.Value();
  Standard_Boolean ToCheck = Standard_True;
  if (!checkValid.IsEmpty()) {
#ifdef OCCT_DEBUG
    cout <<"DONT_SWITCH_IS_VALID positionnee a :"<<checkValid.ToCString()<<"\n";
#endif
    if ( checkValid=="true" || checkValid=="TRUE" ) {
      ToCheck= Standard_False;
    }
  } else {
#ifdef OCCT_DEBUG
    cout <<"DONT_SWITCH_IS_VALID non positionne\n";
#endif
  }
  Standard_Boolean IsValid = Standard_True;
  if (ToCheck)
    IsValid = BRepAlgo::IsValid(theArgs,theResult,closedSolid,GeomCtrl) ;
  return IsValid;

}

//===============================================================================
// TDraft : test draft, uses AIS Viewer
// Solid Face Plane Angle  Reverse
//===============================================================================
static Standard_Integer TDraft(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) return 1;
// argv[1] - TopoDS_Shape Solid
// argv[2] - TopoDS_Shape Face
// argv[3] - TopoDS_Shape Plane
// argv[4] - Standard_Real Angle
// argv[5] - Standard_Integer Reverse

//  Sprintf(prefix, argv[1]);
  Standard_Real anAngle = 0;
  Standard_Boolean Rev = Standard_False;
  Standard_Integer rev = 0;
  TopoDS_Shape Solid  = GetShapeFromName(argv[1]);
  TopoDS_Shape face   = GetShapeFromName(argv[2]);
  TopoDS_Face Face    = TopoDS::Face(face);
  TopoDS_Shape Plane  = GetShapeFromName(argv[3]);
  if (Plane.IsNull ()) {
    di << "TEST : Plane is NULL\n";
    return 1;
  }
  anAngle = Draw::Atof(argv[4]);
  anAngle = 2*M_PI * anAngle / 360.0;
  gp_Pln aPln;
  Handle( Geom_Surface )aSurf;
  AIS_KindOfSurface aSurfType;
  Standard_Real Offset;
  gp_Dir aDir;
  if(argc > 4) { // == 5
    rev = Draw::Atoi(argv[5]);
    Rev = (rev)? Standard_True : Standard_False;
  }

  TopoDS_Face face2 = TopoDS::Face(Plane);
  if(!AIS::GetPlaneFromFace(face2, aPln, aSurf, aSurfType, Offset))
    {
      di << "TEST : Can't find plane\n";
      return 1;
    }

  aDir = aPln.Axis().Direction();
  if (!aPln.Direct())
    aDir.Reverse();
  if (Plane.Orientation() == TopAbs_REVERSED)
    aDir.Reverse();
  di << "TEST : gp::Resolution() = " << gp::Resolution() << "\n";

  BRepOffsetAPI_DraftAngle Draft (Solid);

  if(Abs(anAngle)< Precision::Angular()) {
    di << "TEST : NULL angle\n";
    return 1;}

  if(Rev) anAngle = - anAngle;
  Draft.Add (Face, aDir, anAngle, aPln);
  Draft.Build ();
  if (!Draft.IsDone())  {
    di << "TEST : Draft Not DONE \n";
    return 1;
  }
  TopTools_ListOfShape Larg;
  Larg.Append(Solid);
  if (!IsValid(Larg,Draft.Shape(),Standard_True,Standard_False)) {
    di << "TEST : DesignAlgo returns Not valid\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) Ctx = ViewerTest::GetAISContext();
  Handle(AIS_Shape) ais = new AIS_Shape(Draft.Shape());

  if ( !ais.IsNull() ) {
    ais->SetColor(DEFAULT_COLOR);
    ais->SetMaterial(DEFAULT_MATERIAL);
    // Display the AIS_Shape without redraw
    Ctx->Display(ais, Standard_False);

    const char *Name = "draft1";
    Standard_Boolean IsBound = GetMapOfAIS().IsBound2(Name);
    if (IsBound) {
      Handle(AIS_InteractiveObject) an_object =
	Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(Name));
      if (!an_object.IsNull()) {
        Ctx->Remove(an_object,
                    Standard_True) ;
        GetMapOfAIS().UnBind2(Name) ;
      }
    }
    GetMapOfAIS().Bind(ais, Name);
//  DBRep::Set("draft", ais->Shape());
  }
  Ctx->Display(ais, Standard_True);
  return 0;
}

//==============================================================================
//function : splitParameter
//purpose  : Split parameter string to parameter name and parameter value
//==============================================================================
Standard_Boolean ViewerTest::SplitParameter (const TCollection_AsciiString& theString,
                                             TCollection_AsciiString&       theName,
                                             TCollection_AsciiString&       theValue)
{
  Standard_Integer aParamNameEnd = theString.FirstLocationInSet ("=", 1, theString.Length());

  if (aParamNameEnd == 0)
  {
    return Standard_False;
  }

  TCollection_AsciiString aString (theString);
  if (aParamNameEnd != 0)
  {
    theValue = aString.Split (aParamNameEnd);
    aString.Split (aString.Length() - 1);
    theName = aString;
  }

  return Standard_True;
}

//============================================================================
//  MyCommands
//============================================================================
void ViewerTest::MyCommands( Draw_Interpretor& theCommands)
{

  DrawTrSurf::BasicCommands(theCommands);
  const char* group = "Check Features Operations commands";

  theCommands.Add("Draft","Draft    Solid Face Plane Angle Reverse",
		  __FILE__,
		  &TDraft,group); //Draft_Modification
}

//==============================================================================
// ViewerTest::Factory
//==============================================================================
void ViewerTest::Factory(Draw_Interpretor& theDI)
{
  // definition of Viewer Command
  ViewerTest::Commands(theDI);

#ifdef OCCT_DEBUG
      theDI << "Draw Plugin : OCC V2d & V3d commands are loaded\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(ViewerTest)
