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
#include <AIS_ColoredShape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_Axis.hxx>
#include <PrsDim.hxx>
#include <PrsDim_Relation.hxx>
#include <AIS_TypeFilter.hxx>
#include <AIS_SignatureFilter.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Aspect_Window.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_MediaTextureSet.hxx>
#include <Image_AlienPixMap.hxx>
#include <Message.hxx>
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
#include <Aspect_XRSession.hxx>

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
#define DEFAULT_MATERIAL           Graphic3d_NameOfMaterial_Brass

//=======================================================================
// function : GetColorFromName
// purpose  : get the Quantity_NameOfColor from a string
//=======================================================================

Quantity_NameOfColor ViewerTest::GetColorFromName (const Standard_CString theName)
{
  Quantity_NameOfColor aColor = DEFAULT_COLOR;
  Quantity_Color::ColorFromName (theName, aColor);
  return aColor;
}

//=======================================================================
// function : ParseColor
// purpose  :
//=======================================================================
Standard_Integer ViewerTest::ParseColor (const Standard_Integer   theArgNb,
                                         const char* const* const theArgVec,
                                         Quantity_ColorRGBA&      theColor)
{
  return Draw::ParseColor (theArgNb, theArgVec, theColor);
}

//=======================================================================
// function : ParseColor
// purpose  :
//=======================================================================
Standard_Integer ViewerTest::ParseColor (const Standard_Integer   theArgNb,
                                         const char* const* const theArgVec,
                                         Quantity_Color&          theColor)
{
  return Draw::ParseColor (theArgNb, theArgVec, theColor);
}

//=======================================================================
//function : ParseOnOff
//purpose  :
//=======================================================================
Standard_Boolean ViewerTest::ParseOnOff (Standard_CString  theArg,
                                         Standard_Boolean& theIsOn)
{
  return Draw::ParseOnOff (theArg, theIsOn);
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
Standard_Boolean ViewerTest::ParseLineType (Standard_CString theArg,
                                            Aspect_TypeOfLine& theType,
                                            uint16_t& thePattern)
{
  TCollection_AsciiString aTypeStr (theArg);
  aTypeStr.LowerCase();
  if (aTypeStr == "empty"
   || aTypeStr == "-1")
  {
    theType = Aspect_TOL_EMPTY;
    thePattern = Graphic3d_Aspects::DefaultLinePatternForType (theType);
  }
  else if (aTypeStr == "solid"
        || aTypeStr == "0")
  {
    theType = Aspect_TOL_SOLID;
    thePattern = Graphic3d_Aspects::DefaultLinePatternForType (theType);
  }
  else if (aTypeStr == "dot"
        || aTypeStr == "2")
  {
    theType = Aspect_TOL_DOT;
    thePattern = Graphic3d_Aspects::DefaultLinePatternForType (theType);
  }
  else if (aTypeStr == "dash"
        || aTypeStr == "1")
  {
    theType = Aspect_TOL_DASH;
    thePattern = Graphic3d_Aspects::DefaultLinePatternForType (theType);
  }
  else if (aTypeStr == "dotdash"
        || aTypeStr == "3")
  {
    theType = Aspect_TOL_DOTDASH;
    thePattern = Graphic3d_Aspects::DefaultLinePatternForType (theType);
  }
  else
  {
    if (aTypeStr.StartsWith ("0x"))
    {
      aTypeStr = aTypeStr.SubString (3, aTypeStr.Length());
    }

    if (aTypeStr.Length() != 4
    || !std::isxdigit (static_cast<unsigned char> (aTypeStr.Value (1)))
    || !std::isxdigit (static_cast<unsigned char> (aTypeStr.Value (2)))
    || !std::isxdigit (static_cast<unsigned char> (aTypeStr.Value (3)))
    || !std::isxdigit (static_cast<unsigned char> (aTypeStr.Value (4))))
    {
      return Standard_False;
    }

    std::stringstream aStream;
    aStream << std::setbase (16) << aTypeStr.ToCString();
    if (aStream.fail())
    {
      return Standard_False;
    }

    Standard_Integer aNumber = -1;
    aStream >> aNumber;
    if (aStream.fail())
    {
      return Standard_False;
    }

    thePattern = (uint16_t )aNumber;
    theType = Graphic3d_Aspects::DefaultLineTypeForPattern (thePattern);
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
//function : ParseShadingModel
//purpose  :
//=======================================================================
Standard_Boolean ViewerTest::ParseShadingModel (Standard_CString              theArg,
                                                Graphic3d_TypeOfShadingModel& theModel)
{
  TCollection_AsciiString aTypeStr (theArg);
  aTypeStr.LowerCase();
  if (aTypeStr == "unlit"
   || aTypeStr == "color"
   || aTypeStr == "none")
  {
    theModel = Graphic3d_TOSM_UNLIT;
  }
  else if (aTypeStr == "flat"
        || aTypeStr == "facet")
  {
    theModel = Graphic3d_TOSM_FACET;
  }
  else if (aTypeStr == "gouraud"
        || aTypeStr == "vertex"
        || aTypeStr == "vert")
  {
    theModel = Graphic3d_TOSM_VERTEX;
  }
  else if (aTypeStr == "phong"
        || aTypeStr == "fragment"
        || aTypeStr == "frag"
        || aTypeStr == "pixel")
  {
    theModel = Graphic3d_TOSM_FRAGMENT;
  }
  else if (aTypeStr == "pbr")
  {
    theModel = Graphic3d_TOSM_PBR;
  }
  else if (aTypeStr == "pbr_facet")
  {
    theModel = Graphic3d_TOSM_PBR_FACET;
  }
  else if (aTypeStr == "default"
        || aTypeStr == "def")
  {
    theModel = Graphic3d_TOSM_DEFAULT;
  }
  else if (aTypeStr.IsIntegerValue())
  {
    const int aTypeInt = aTypeStr.IntegerValue();
    if (aTypeInt <= Graphic3d_TOSM_DEFAULT || aTypeInt >= Graphic3d_TypeOfShadingModel_NB)
    {
      return Standard_False;
    }
    theModel = (Graphic3d_TypeOfShadingModel)aTypeInt;
  }
  else
  {
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : parseZLayer
//purpose  :
//=======================================================================
Standard_Boolean ViewerTest::parseZLayer (Standard_CString theArg,
                                          Standard_Boolean theToAllowInteger,
                                          Graphic3d_ZLayerId& theLayer)
{
  TCollection_AsciiString aName (theArg);
  aName.LowerCase();
  if (aName == "default"
   || aName == "def")
  {
    theLayer = Graphic3d_ZLayerId_Default;
  }
  else if (aName == "top")
  {
    theLayer = Graphic3d_ZLayerId_Top;
  }
  else if (aName == "topmost")
  {
    theLayer = Graphic3d_ZLayerId_Topmost;
  }
  else if (aName == "overlay"
        || aName == "toposd")
  {
    theLayer = Graphic3d_ZLayerId_TopOSD;
  }
  else if (aName == "underlay"
        || aName == "botosd")
  {
    theLayer = Graphic3d_ZLayerId_BotOSD;
  }
  else if (aName == "undefined")
  {
    theLayer = Graphic3d_ZLayerId_UNKNOWN;
  }
  else if (!GetAISContext().IsNull())
  {
    const Handle(V3d_Viewer)& aViewer = ViewerTest::GetAISContext()->CurrentViewer();
    TColStd_SequenceOfInteger aLayers;
    aViewer->GetAllZLayers (aLayers);
    for (TColStd_SequenceOfInteger::Iterator aLayeriter (aLayers); aLayeriter.More(); aLayeriter.Next())
    {
      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (aLayeriter.Value());
      if (TCollection_AsciiString::IsSameString (aSettings.Name(), aName, Standard_False))
      {
        theLayer = aLayeriter.Value();
        return true;
      }
    }

    if (!theToAllowInteger
     || !aName.IsIntegerValue())
    {
      return false;
    }
    Graphic3d_ZLayerId aLayer = aName.IntegerValue();
    if (aLayer == Graphic3d_ZLayerId_UNKNOWN
     || std::find (aLayers.begin(), aLayers.end(), aLayer) != aLayers.end())
    {
      theLayer = aLayer;
      return true;
    }
    return false;
  }
  return true;
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
    Message::SendFail ("Error: AIS context is not available.");
    return Standard_False;
  }

  if (aMap.IsBound2 (theName))
  {
    if (!theReplaceIfExists)
    {
      Message::SendFail() << "Error: other interactive object has been already registered with name: " << theName << ".\n"
                          << "Please use another name.";
      return Standard_False;
    }

    if (Handle(AIS_InteractiveObject) anOldObj = aMap.Find2 (theName))
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

static NCollection_List<Handle(ViewerTest_EventManager)> theEventMgrs;

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
  theEventMgrs.Clear();
  theEventMgrs.Prepend (new ViewerTest_EventManager (ViewerTest::CurrentView(), ViewerTest::GetAISContext()));
}

Handle(ViewerTest_EventManager) ViewerTest::CurrentEventManager()
{
  return !theEventMgrs.IsEmpty()
        ? theEventMgrs.First()
        : Handle(ViewerTest_EventManager)();
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
    Message::SendFail ("Error: cannot find an active view!");
    return Standard_False;
  }
  return Standard_True;
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
    const Handle(AIS_InteractiveObject) anObj = anObjIter.Key1();
    if (anObj->GetContext() != TheAISContext())
    {
      continue;
    }

    Message::SendInfo() << "Remove " << anObjIter.Key2();
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

  for (i = 1; i <= aLastInd; i++)
  {
    TCollection_AsciiString name(argv[i]);
    Handle(AIS_InteractiveObject) aShape;
    GetMapOfAIS().Find2(name, aShape);
    if (aShape.IsNull())
    {
      Message::SendFail() << "Syntax error: object '" << name << "' is not found";
      return 1;
    }

    Handle(Prs3d_Drawer) CurDrawer = aShape->Attributes();
    Handle(Prs3d_IsoAspect) aUIso = CurDrawer->UIsoAspect();
    Handle(Prs3d_IsoAspect) aVIso = CurDrawer->VIsoAspect();
    if (isChanged)
    {
      CurDrawer->SetUIsoAspect(CopyIsoAspect(aUIso, aNbUIsos));
      CurDrawer->SetVIsoAspect(CopyIsoAspect(aVIso, aNbVIsos));
      TheAISContext()->SetLocalAttributes (aShape, CurDrawer, Standard_False);
      TheAISContext()->Redisplay (aShape, Standard_False);
    }
    else
    {
      di << "Number of isos for " << argv[i] << " : "
          << aUIso->Number() << " " << aVIso->Number() << "\n";
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
    Message::SendFail ("Error: wrong syntax!");
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
    Message::SendFail ("Error: wrong syntax!");
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
                 Standard_Integer theNbArgs,
                 const char** theArgVec)
{
  TCollection_AsciiString aMatch;
  Standard_Boolean toFormat = Standard_False;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgCase (theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anArgCase == "-list"
     || anArgCase == "-format")
    {
      toFormat = Standard_True;
    }
    else if (aMatch.IsEmpty())
    {
      aMatch = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  TCollection_AsciiString aRes;
  for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS()); anIter.More(); anIter.Next())
  {
    if (!aMatch.IsEmpty())
    {
      const TCollection_AsciiString aCheck = TCollection_AsciiString ("string match '") + aMatch + "' '" + anIter.Key2() + "'";
      if (theDI.Eval (aCheck.ToCString()) == 0
      && *theDI.Result() != '1')
      {
        continue;
      }
    }

    if (toFormat)
    {
      aRes += TCollection_AsciiString("\t") + anIter.Key2() + "\n";
    }
    else
    {
      aRes += anIter.Key2() + " ";
    }
  }
  theDI.Reset();
  theDI << aRes;
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
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (theArgNb < 2)
  {
    Message::SendFail ("Error: wrong number of arguments! Image file name should be specified at least.");
    return 1;
  }
  if (aView.IsNull())
  {
    Message::SendFail() << "Error: cannot find an active view!";
    return 1;
  }

  Standard_Integer      anArgIter   = 1;
  Standard_CString      aFilePath   = theArgVec[anArgIter++];
  ViewerTest_StereoPair aStereoPair = ViewerTest_SP_Single;
  V3d_ImageDumpOptions  aParams;
  Handle(Graphic3d_Camera) aCustomCam;
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
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
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
      else if (aBufArg == "red")
      {
        aParams.BufferType = Graphic3d_BT_Red;
      }
      else if (aBufArg == "depth")
      {
        aParams.BufferType = Graphic3d_BT_Depth;
      }
      else
      {
        Message::SendFail() << "Error: unknown buffer '" << aBufArg << "'";
        return 1;
      }
    }
    else if (anArgIter + 1 < theArgNb
          && anArg == "-xrpose")
    {
      TCollection_AsciiString anXRArg (theArgVec[++anArgIter]);
      anXRArg.LowerCase();
      if (anXRArg == "base")
      {
        aCustomCam = aView->View()->BaseXRCamera();
      }
      else if (anXRArg == "head")
      {
        aCustomCam = aView->View()->PosedXRCamera();
      }
      else if (anXRArg == "handleft"
            || anXRArg == "handright")
      {
        if (aView->View()->IsActiveXR())
        {
          aCustomCam = new Graphic3d_Camera();
          aView->View()->ComputeXRPosedCameraFromBase (*aCustomCam, anXRArg == "handleft"
                                                     ? aView->View()->XRSession()->LeftHandPose()
                                                     : aView->View()->XRSession()->RightHandPose());
        }
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown XR pose '" << anXRArg << "'";
        return 1;
      }
      if (aCustomCam.IsNull())
      {
        Message::SendFail() << "Error: undefined XR pose";
        return 0;
      }
    }
    else if (anArg == "-stereo")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
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
        Message::SendFail() << "Error: unknown stereo format '" << aStereoArg << "'";
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
    else if (anArg == "-red"
          || anArg ==  "red")
    {
      aParams.BufferType = Graphic3d_BT_Red;
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
        Message::SendFail() << "Error: wrong syntax at " << theArgVec[anArgIter];
        return 1;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: integer value is expected right after 'width'";
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
        Message::SendFail() << "Error: wrong syntax at " << theArgVec[anArgIter];
        return 1;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: integer value is expected right after 'height'";
        return 1;
      }
      aParams.Height = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (anArg == "-tile"
          || anArg == "-tilesize")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: integer value is expected right after 'tileSize'";
        return 1;
      }
      aParams.TileSize = Draw::Atoi (theArgVec[anArgIter]);
    }
    else
    {
      Message::SendFail() << "Error: unknown argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }
  if ((aParams.Width <= 0 && aParams.Height >  0)
   || (aParams.Width >  0 && aParams.Height <= 0))
  {
    Message::SendFail() << "Error: dimensions " << aParams.Width << "x" << aParams.Height << " are incorrect";
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
    case Graphic3d_BT_Red:                 aFormat = Image_Format_Gray;  break;
  }

  const bool wasImmUpdate = aView->SetImmediateUpdate (false);
  Handle(Graphic3d_Camera) aCamBack = aView->Camera();
  if (!aCustomCam.IsNull())
  {
    aView->SetCamera (aCustomCam);
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
  if (!aCustomCam.IsNull())
  {
    aView->SetCamera (aCamBack);
  }
  aView->SetImmediateUpdate (wasImmUpdate);

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
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  TypeOfDispOperation aType = TCollection_AsciiString (argv[0]) == "vunsetdispmode"
                            ? TypeOfDispOperation_UnsetDispMode
                            : TypeOfDispOperation_SetDispMode;
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

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
      Handle(AIS_InteractiveObject) aPrs;
      if (GetMapOfAIS().Find2 (aName, aPrs)
      && !aPrs.IsNull())
      {
        VwrTst_DispErase (aPrs, -1, TypeOfDispOperation_UnsetDispMode, Standard_True);
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
    if (GetMapOfAIS().Find2 (aName, aPrs)
     && !aPrs.IsNull())
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
  if (Ctx.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

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
    if (GetMapOfAIS().Find2 (name, IO)
    && !IO.IsNull())
    {
      if(On==1)
        Ctx->SubIntensityOn(IO, Standard_True);
      else
        Ctx->SubIntensityOff(IO, Standard_True);
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
            Message::SendFail() << "Error: object " << mySeqIter.Value() << " is not displayed!";
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

//! Parse interior style name.
static bool parseInteriorStyle (const TCollection_AsciiString& theArg,
                                Aspect_InteriorStyle& theStyle)
{
  TCollection_AsciiString anArg (theArg);
  anArg.LowerCase();
  if (anArg == "empty")
  {
    theStyle = Aspect_IS_EMPTY;
  }
  else if (anArg == "hollow")
  {
    theStyle = Aspect_IS_HOLLOW;
  }
  else if (anArg == "solid")
  {
    theStyle = Aspect_IS_SOLID;
  }
  else if (anArg == "hatch")
  {
    theStyle = Aspect_IS_HATCH;
  }
  else if (anArg == "hiddenline"
        || anArg == "hidden-line"
        || anArg == "hidden_line")
  {
    theStyle = Aspect_IS_HIDDENLINE;
  }
  else if (anArg == "point")
  {
    theStyle = Aspect_IS_POINT;
  }
  else if (theArg.IsIntegerValue())
  {
    const Standard_Integer anIntStyle = theArg.IntegerValue();
    if (anIntStyle < Aspect_IS_EMPTY || anIntStyle > Aspect_IS_POINT)
    {
      return false;
    }
    theStyle = (Aspect_InteriorStyle)anIntStyle;
  }
  else
  {
    return false;
  }
  return true;
}

//! Auxiliary structure for VAspects
struct ViewerTest_AspectsChangeSet
{
  Standard_Integer             ToSetVisibility;
  Standard_Integer             Visibility;

  Standard_Integer             ToSetColor;
  Quantity_Color               Color;
  Standard_Integer             ToSetBackFaceColor;
  Quantity_Color               BackFaceColor;

  Standard_Integer             ToSetLineWidth;
  Standard_Real                LineWidth;

  Standard_Integer             ToSetTypeOfLine;
  uint16_t                     StippleLinePattern;
  uint16_t                     StippleLineFactor;

  Standard_Integer             ToSetTypeOfMarker;
  Aspect_TypeOfMarker          TypeOfMarker;
  Handle(Image_PixMap)         MarkerImage;

  Standard_Integer             ToSetMarkerSize;
  Standard_Real                MarkerSize;

  Standard_Integer             ToSetTransparency;
  Standard_Real                Transparency;

  Standard_Integer             ToSetAlphaMode;
  Graphic3d_AlphaMode          AlphaMode;
  Standard_ShortReal           AlphaCutoff;

  Standard_Integer             ToSetMaterial;
  Graphic3d_NameOfMaterial     Material;
  TCollection_AsciiString      MatName;

  NCollection_Sequence<TopoDS_Shape> SubShapes;

  Standard_Integer             ToSetShowFreeBoundary;
  Standard_Integer             ToSetFreeBoundaryWidth;
  Standard_Real                FreeBoundaryWidth;
  Standard_Integer             ToSetFreeBoundaryColor;
  Quantity_Color               FreeBoundaryColor;

  Standard_Integer             ToEnableIsoOnTriangulation;

  Standard_Integer             ToSetFaceBoundaryDraw;
  Standard_Integer             ToSetFaceBoundaryUpperContinuity;
  GeomAbs_Shape                FaceBoundaryUpperContinuity;

  Standard_Integer             ToSetFaceBoundaryColor;
  Quantity_Color               FaceBoundaryColor;

  Standard_Integer             ToSetFaceBoundaryWidth;
  Standard_Real                FaceBoundaryWidth;

  Standard_Integer             ToSetTypeOfFaceBoundaryLine;
  Aspect_TypeOfLine            TypeOfFaceBoundaryLine;

  Standard_Integer             ToSetMaxParamValue;
  Standard_Real                MaxParamValue;

  Standard_Integer             ToSetSensitivity;
  Standard_Integer             SelectionMode;
  Standard_Integer             Sensitivity;

  Standard_Integer             ToSetHatch;
  Standard_Integer             StdHatchStyle;
  TCollection_AsciiString      PathToHatchPattern;

  Standard_Integer             ToSetShadingModel;
  Graphic3d_TypeOfShadingModel ShadingModel;
  TCollection_AsciiString      ShadingModelName;

  Standard_Integer             ToSetInterior;
  Aspect_InteriorStyle         InteriorStyle;

  Standard_Integer             ToSetDrawSilhouette;

  Standard_Integer             ToSetDrawEdges;
  Standard_Integer             ToSetQuadEdges;

  Standard_Integer             ToSetEdgeColor;
  Quantity_ColorRGBA           EdgeColor;

  Standard_Integer             ToSetEdgeWidth;
  Standard_Real                EdgeWidth;

  Standard_Integer             ToSetTypeOfEdge;
  Aspect_TypeOfLine            TypeOfEdge;

  //! Empty constructor
  ViewerTest_AspectsChangeSet()
  : ToSetVisibility   (0),
    Visibility        (1),
    ToSetColor        (0),
    Color             (DEFAULT_COLOR),
    ToSetBackFaceColor(0),
    BackFaceColor     (DEFAULT_COLOR),
    ToSetLineWidth    (0),
    LineWidth         (1.0),
    ToSetTypeOfLine   (0),
    StippleLinePattern(0xFFFF),
    StippleLineFactor (1),
    ToSetTypeOfMarker (0),
    TypeOfMarker      (Aspect_TOM_PLUS),
    ToSetMarkerSize   (0),
    MarkerSize        (1.0),
    ToSetTransparency (0),
    Transparency      (0.0),
    ToSetAlphaMode    (0),
    AlphaMode         (Graphic3d_AlphaMode_BlendAuto),
    AlphaCutoff       (0.5f),
    ToSetMaterial     (0),
    Material          (Graphic3d_NameOfMaterial_DEFAULT),
    ToSetShowFreeBoundary      (0),
    ToSetFreeBoundaryWidth     (0),
    FreeBoundaryWidth          (1.0),
    ToSetFreeBoundaryColor     (0),
    FreeBoundaryColor          (DEFAULT_FREEBOUNDARY_COLOR),
    ToEnableIsoOnTriangulation (0),
    //
    ToSetFaceBoundaryDraw      (0),
    ToSetFaceBoundaryUpperContinuity (0),
    FaceBoundaryUpperContinuity(GeomAbs_CN),
    ToSetFaceBoundaryColor     (0),
    FaceBoundaryColor          (Quantity_NOC_BLACK),
    ToSetFaceBoundaryWidth     (0),
    FaceBoundaryWidth          (1.0f),
    ToSetTypeOfFaceBoundaryLine(0),
    TypeOfFaceBoundaryLine     (Aspect_TOL_SOLID),
    //
    ToSetMaxParamValue         (0),
    MaxParamValue              (500000),
    ToSetSensitivity           (0),
    SelectionMode              (-1),
    Sensitivity                (-1),
    ToSetHatch                 (0),
    StdHatchStyle              (-1),
    ToSetShadingModel          (0),
    ShadingModel               (Graphic3d_TOSM_DEFAULT),
    ToSetInterior              (0),
    InteriorStyle              (Aspect_IS_SOLID),
    ToSetDrawSilhouette (0),
    ToSetDrawEdges    (0),
    ToSetQuadEdges    (0),
    ToSetEdgeColor    (0),
    ToSetEdgeWidth    (0),
    EdgeWidth         (1.0),
    ToSetTypeOfEdge   (0),
    TypeOfEdge        (Aspect_TOL_SOLID)
    {}

  //! @return true if no changes have been requested
  Standard_Boolean IsEmpty() const
  {
    return ToSetVisibility        == 0
        && ToSetLineWidth         == 0
        && ToSetTransparency      == 0
        && ToSetAlphaMode         == 0
        && ToSetColor             == 0
        && ToSetBackFaceColor     == 0
        && ToSetMaterial          == 0
        && ToSetShowFreeBoundary  == 0
        && ToSetFreeBoundaryColor == 0
        && ToSetFreeBoundaryWidth == 0
        && ToEnableIsoOnTriangulation == 0
        && ToSetFaceBoundaryDraw == 0
        && ToSetFaceBoundaryUpperContinuity == 0
        && ToSetFaceBoundaryColor == 0
        && ToSetFaceBoundaryWidth == 0
        && ToSetTypeOfFaceBoundaryLine == 0
        && ToSetMaxParamValue     == 0
        && ToSetSensitivity       == 0
        && ToSetHatch             == 0
        && ToSetShadingModel      == 0
        && ToSetInterior          == 0
        && ToSetDrawSilhouette    == 0
        && ToSetDrawEdges         == 0
        && ToSetQuadEdges         == 0
        && ToSetEdgeColor         == 0
        && ToSetEdgeWidth         == 0
        && ToSetTypeOfEdge        == 0;
  }

  //! @return true if properties are valid
  Standard_Boolean Validate() const
  {
    Standard_Boolean isOk = Standard_True;
    if (Visibility != 0 && Visibility != 1)
    {
      Message::SendFail() << "Error: the visibility should be equal to 0 or 1 (0 - invisible; 1 - visible) (specified " << Visibility << ")";
      isOk = Standard_False;
    }
    if (LineWidth <= 0.0
     || LineWidth >  10.0)
    {
      Message::SendFail() << "Error: the width should be within [1; 10] range (specified " << LineWidth << ")";
      isOk = Standard_False;
    }
    if (Transparency < 0.0
     || Transparency > 1.0)
    {
      Message::SendFail() << "Error: the transparency should be within [0; 1] range (specified " << Transparency << ")";
      isOk = Standard_False;
    }
    if (ToSetAlphaMode == 1
     && (AlphaCutoff <= 0.0f || AlphaCutoff >= 1.0f))
    {
      Message::SendFail() << "Error: alpha cutoff value should be within (0; 1) range (specified " << AlphaCutoff << ")";
      isOk = Standard_False;
    }
    if (FreeBoundaryWidth <= 0.0
     || FreeBoundaryWidth >  10.0)
    {
      Message::SendFail() << "Error: the free boundary width should be within [1; 10] range (specified " << FreeBoundaryWidth << ")";
      isOk = Standard_False;
    }
    if (MaxParamValue < 0.0)
    {
      Message::SendFail() << "Error: the max parameter value should be greater than zero (specified " << MaxParamValue << ")";
      isOk = Standard_False;
    }
    if (Sensitivity <= 0 && ToSetSensitivity)
    {
      Message::SendFail() << "Error: sensitivity parameter value should be positive (specified " << Sensitivity << ")";
      isOk = Standard_False;
    }
    if (ToSetHatch == 1 && StdHatchStyle < 0 && PathToHatchPattern == "")
    {
      Message::SendFail ("Error: hatch style must be specified");
      isOk = Standard_False;
    }
    if (ToSetShadingModel == 1
    && (ShadingModel < Graphic3d_TOSM_DEFAULT || ShadingModel > Graphic3d_TOSM_PBR_FACET))
    {
      Message::SendFail() << "Error: unknown shading model " << ShadingModelName << ".";
      isOk = Standard_False;
    }
    return isOk;
  }

  //! Apply aspects to specified drawer.
  bool Apply (const Handle(Prs3d_Drawer)& theDrawer)
  {
    bool toRecompute = false;
    const Handle(Prs3d_Drawer)& aDefDrawer = ViewerTest::GetAISContext()->DefaultDrawer();
    if (ToSetShowFreeBoundary != 0)
    {
      theDrawer->SetFreeBoundaryDraw (ToSetShowFreeBoundary == 1);
      toRecompute = true;
    }
    if (ToSetFreeBoundaryWidth != 0)
    {
      if (ToSetFreeBoundaryWidth != -1
       || theDrawer->HasOwnFreeBoundaryAspect())
      {
        if (!theDrawer->HasOwnFreeBoundaryAspect())
        {
          Handle(Prs3d_LineAspect) aBoundaryAspect = new Prs3d_LineAspect (Quantity_NOC_RED, Aspect_TOL_SOLID, 1.0);
          *aBoundaryAspect->Aspect() = *theDrawer->FreeBoundaryAspect()->Aspect();
          theDrawer->SetFreeBoundaryAspect (aBoundaryAspect);
          toRecompute = true;
        }
        theDrawer->FreeBoundaryAspect()->SetWidth (FreeBoundaryWidth);
      }
    }
    if (ToSetFreeBoundaryColor != 0)
    {
      Handle(Prs3d_LineAspect) aBoundaryAspect = new Prs3d_LineAspect (Quantity_NOC_RED, Aspect_TOL_SOLID, 1.0);
      *aBoundaryAspect->Aspect() = *theDrawer->FreeBoundaryAspect()->Aspect();
      aBoundaryAspect->SetColor (FreeBoundaryColor);
      theDrawer->SetFreeBoundaryAspect (aBoundaryAspect);
      toRecompute = true;
    }
    if (ToSetTypeOfLine != 0)
    {
      if (ToSetTypeOfLine != -1
       || theDrawer->HasOwnLineAspect()
       || theDrawer->HasOwnWireAspect()
       || theDrawer->HasOwnFreeBoundaryAspect()
       || theDrawer->HasOwnUnFreeBoundaryAspect()
       || theDrawer->HasOwnSeenLineAspect())
      {
        toRecompute = theDrawer->SetOwnLineAspects() || toRecompute;
        theDrawer->LineAspect()->Aspect()->SetLinePattern (StippleLinePattern);
        theDrawer->LineAspect()->Aspect()->SetLineStippleFactor (StippleLineFactor);
        theDrawer->WireAspect()->Aspect()->SetLinePattern (StippleLinePattern);
        theDrawer->WireAspect()->Aspect()->SetLineStippleFactor (StippleLineFactor);
        theDrawer->FreeBoundaryAspect()->Aspect()->SetLinePattern (StippleLinePattern);
        theDrawer->FreeBoundaryAspect()->Aspect()->SetLineStippleFactor (StippleLineFactor);
        theDrawer->UnFreeBoundaryAspect()->Aspect()->SetLinePattern (StippleLinePattern);
        theDrawer->UnFreeBoundaryAspect()->Aspect()->SetLineStippleFactor (StippleLineFactor);
        theDrawer->SeenLineAspect()->Aspect()->SetLinePattern (StippleLinePattern);
        theDrawer->SeenLineAspect()->Aspect()->SetLineStippleFactor (StippleLineFactor);
      }
    }
    if (ToSetTypeOfMarker != 0)
    {
      if (ToSetTypeOfMarker != -1
       || theDrawer->HasOwnPointAspect())
      {
        toRecompute = theDrawer->SetupOwnPointAspect (aDefDrawer) || toRecompute;
        theDrawer->PointAspect()->SetTypeOfMarker (TypeOfMarker);
        theDrawer->PointAspect()->Aspect()->SetMarkerImage (MarkerImage.IsNull() ? Handle(Graphic3d_MarkerImage)() : new Graphic3d_MarkerImage (MarkerImage));
      }
    }
    if (ToSetMarkerSize != 0)
    {
      if (ToSetMarkerSize != -1
       || theDrawer->HasOwnPointAspect())
      {
        toRecompute = theDrawer->SetupOwnPointAspect (aDefDrawer) || toRecompute;
        theDrawer->PointAspect()->SetScale (MarkerSize);
        toRecompute = true;
      }
    }
    if (ToSetMaxParamValue != 0)
    {
      if (ToSetMaxParamValue != -1
       || theDrawer->HasOwnMaximalParameterValue())
      {
        theDrawer->SetMaximalParameterValue (MaxParamValue);
        toRecompute = true;
      }
    }
    if (ToSetFaceBoundaryDraw != 0)
    {
      if (ToSetFaceBoundaryDraw != -1
       || theDrawer->HasOwnFaceBoundaryDraw())
      {
        toRecompute = true;
        theDrawer->SetFaceBoundaryDraw (ToSetFaceBoundaryDraw == 1);
      }
    }
    if (ToSetFaceBoundaryUpperContinuity != 0)
    {
      if (ToSetFaceBoundaryUpperContinuity != -1
       || theDrawer->HasOwnFaceBoundaryUpperContinuity())
      {
        toRecompute = true;
        if (ToSetFaceBoundaryUpperContinuity == -1)
        {
          theDrawer->UnsetFaceBoundaryUpperContinuity();
        }
        else
        {
          theDrawer->SetFaceBoundaryUpperContinuity (FaceBoundaryUpperContinuity);
        }
      }
    }
    if (ToSetFaceBoundaryColor != 0)
    {
      if (ToSetFaceBoundaryColor != -1
       || theDrawer->HasOwnFaceBoundaryAspect())
      {
        if (ToSetFaceBoundaryColor == -1)
        {
          toRecompute = true;
          theDrawer->SetFaceBoundaryAspect (Handle(Prs3d_LineAspect)());
        }
        else
        {
          toRecompute = theDrawer->SetupOwnFaceBoundaryAspect (aDefDrawer) || toRecompute;
          theDrawer->FaceBoundaryAspect()->SetColor (FaceBoundaryColor);
        }
      }
    }
    if (ToSetFaceBoundaryWidth != 0)
    {
      if (ToSetFaceBoundaryWidth != -1
       || theDrawer->HasOwnFaceBoundaryAspect())
      {
        toRecompute = theDrawer->SetupOwnFaceBoundaryAspect (aDefDrawer) || toRecompute;
        theDrawer->FaceBoundaryAspect()->SetWidth (FaceBoundaryWidth);
      }
    }
    if (ToSetTypeOfFaceBoundaryLine != 0)
    {
      if (ToSetTypeOfFaceBoundaryLine != -1
       || theDrawer->HasOwnFaceBoundaryAspect())
      {
        toRecompute = theDrawer->SetupOwnFaceBoundaryAspect (aDefDrawer) || toRecompute;
        theDrawer->FaceBoundaryAspect()->SetTypeOfLine (TypeOfFaceBoundaryLine);
      }
    }
    if (ToSetShadingModel != 0)
    {
      if (ToSetShadingModel != -1
       || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect (aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetShadingModel (ShadingModel);
      }
    }
    if (ToSetBackFaceColor != 0)
    {
      if (ToSetBackFaceColor != -1
       || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect (aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->SetColor (BackFaceColor, Aspect_TOFM_BACK_SIDE);
      }
    }
    if (ToSetAlphaMode != 0)
    {
      if (ToSetAlphaMode != -1
       || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect (aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetAlphaMode (AlphaMode, AlphaCutoff);
      }
    }
    if (ToSetHatch != 0)
    {
      if (ToSetHatch != -1
      ||  theDrawer->HasOwnShadingAspect())
      {
        theDrawer->SetupOwnShadingAspect (aDefDrawer);
        Handle(Graphic3d_AspectFillArea3d) anAsp = theDrawer->ShadingAspect()->Aspect();
        if (ToSetHatch == -1)
        {
          anAsp->SetInteriorStyle (Aspect_IS_SOLID);
        }
        else
        {
          anAsp->SetInteriorStyle (Aspect_IS_HATCH);
          if (!PathToHatchPattern.IsEmpty())
          {
            Handle(Image_AlienPixMap) anImage = new Image_AlienPixMap();
            if (anImage->Load (TCollection_AsciiString (PathToHatchPattern.ToCString())))
            {
              anAsp->SetHatchStyle (new Graphic3d_HatchStyle (anImage));
            }
            else
            {
              Message::SendFail() << "Error: cannot load the following image: " << PathToHatchPattern;
            }
          }
          else if (StdHatchStyle != -1)
          {
            anAsp->SetHatchStyle (new Graphic3d_HatchStyle ((Aspect_HatchStyle)StdHatchStyle));
          }
        }
        toRecompute = true;
      }
    }
    if (ToSetInterior != 0)
    {
      if (ToSetInterior != -1
       || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect (aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetInteriorStyle (InteriorStyle);
        if (InteriorStyle == Aspect_IS_HATCH
         && theDrawer->ShadingAspect()->Aspect()->HatchStyle().IsNull())
        {
          theDrawer->ShadingAspect()->Aspect()->SetHatchStyle (Aspect_HS_VERTICAL);
        }
      }
    }
    if (ToSetDrawSilhouette != 0)
    {
      if (ToSetDrawSilhouette != -1
       || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect (aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetDrawSilhouette (ToSetDrawSilhouette == 1);
      }
    }
    if (ToSetDrawEdges != 0)
    {
      if (ToSetDrawEdges != -1
       || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect (aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetDrawEdges (ToSetDrawEdges == 1);
      }
    }
    if (ToSetQuadEdges != 0)
    {
      if (ToSetQuadEdges != -1
          || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect (aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetSkipFirstEdge (ToSetQuadEdges == 1);
      }
    }
    if (ToSetEdgeWidth != 0)
    {
      if (ToSetEdgeWidth != -1
       || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect (aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetEdgeWidth (EdgeWidth);
      }
    }
    if (ToSetTypeOfEdge != 0)
    {
      if (ToSetTypeOfEdge != -1
       || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect (aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetEdgeLineType (TypeOfEdge);
        if (ToSetInterior == 0)
        {
          theDrawer->ShadingAspect()->Aspect()->SetDrawEdges (ToSetTypeOfEdge == 1
                                                           && TypeOfEdge != Aspect_TOL_EMPTY);
        }
      }
    }
    if (ToSetEdgeColor != 0)
    {
      if (ToSetEdgeColor != -1
       || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect (aDefDrawer) || toRecompute;
        if (ToSetEdgeColor == -1)
        {
          theDrawer->ShadingAspect()->Aspect()->SetEdgeColor (theDrawer->ShadingAspect()->Aspect()->InteriorColor());
        }
        else
        {
          theDrawer->ShadingAspect()->Aspect()->SetEdgeColor (EdgeColor);
        }
      }
    }
    return toRecompute;
  }
};

//==============================================================================
//function : VAspects
//purpose  :
//==============================================================================
static Standard_Integer VAspects (Draw_Interpretor& theDI,
                                  Standard_Integer  theArgNb,
                                  const char**      theArgVec)
{
  TCollection_AsciiString aCmdName (theArgVec[0]);
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aCtx, ViewerTest::CurrentView());
  if (aCtx.IsNull())
  {
    Message::SendFail ("Error: no active view!");
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
    Message::SendFail ("Error: wrong syntax. If -defaults is used there should not be any objects' names!");
    return 1;
  }

  NCollection_Sequence<ViewerTest_AspectsChangeSet> aChanges;
  aChanges.Append (ViewerTest_AspectsChangeSet());
  ViewerTest_AspectsChangeSet* aChangeSet = &aChanges.ChangeLast();

  // parse syntax of legacy commands
  bool toParseAliasArgs = false;
  Standard_Boolean toDump = 0;
  Standard_Boolean toCompactDump = 0;
  Standard_Integer aDumpDepth = -1;
  if (aCmdName == "vsetwidth")
  {
    if (aNames.IsEmpty()
    || !aNames.Last().IsRealValue())
    {
      Message::SendFail ("Error: not enough arguments!");
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
      Message::SendFail ("Error: not enough arguments!");
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
    else if (Quantity_Color::ColorFromHex (aNames.Last().ToCString(), aChangeSet->Color))
    {
      aNames.Remove (aNames.Length());
      isOk = Standard_True;
    }
    else if (aNames.Length() >= 3)
    {
      const char* anArgVec[3] =
      {
        aNames.Value (aNames.Upper() - 2).ToCString(),
        aNames.Value (aNames.Upper() - 1).ToCString(),
        aNames.Value (aNames.Upper() - 0).ToCString(),
      };

      Standard_Integer aNbParsed = Draw::ParseColor (3, anArgVec, aChangeSet->Color);
      isOk = aNbParsed == 3;
      aNames.Remove (aNames.Length());
      aNames.Remove (aNames.Length());
      aNames.Remove (aNames.Length());
    }
    if (!isOk)
    {
      Message::SendFail ("Error: not enough arguments!");
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
      Message::SendFail ("Error: not enough arguments!");
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
      Message::SendFail ("Error: not enough arguments!");
      return 1;
    }
    aChangeSet->ToSetMaterial = 1;
    aChangeSet->MatName = aNames.Last();
    aNames.Remove (aNames.Length());
    if (!Graphic3d_MaterialAspect::MaterialFromName (aChangeSet->MatName.ToCString(), aChangeSet->Material))
    {
      Message::SendFail() << "Syntax error: unknown material '" << aChangeSet->MatName << "'.";
      return 1;
    }
  }
  else if (aCmdName == "vunsetmaterial")
  {
    aChangeSet->ToSetMaterial = -1;
  }
  else if (aCmdName == "vsetinteriorstyle")
  {
    if (aNames.IsEmpty()
    || !aNames.Last().IsRealValue())
    {
      Message::SendFail ("Error: not enough arguments!");
      return 1;
    }
    aChangeSet->ToSetInterior = 1;
    if (!parseInteriorStyle (aNames.Last(), aChangeSet->InteriorStyle))
    {
      Message::SendFail() << "Error: wrong syntax at " << aNames.Last();
      return 1;
    }
    aNames.Remove (aNames.Length());
  }
  else if (aCmdName == "vsetedgetype")
  {
    aChangeSet->ToSetDrawEdges = 1;
    toParseAliasArgs = true;
  }
  else if (aCmdName == "vunsetedgetype")
  {
    aChangeSet->ToSetDrawEdges  = -1;
    aChangeSet->ToSetEdgeColor  = -1;
    aChangeSet->ToSetTypeOfEdge = -1;
    aChangeSet->TypeOfEdge = Aspect_TOL_SOLID;
  }
  else if (aCmdName == "vshowfaceboundary")
  {
    aChangeSet->ToSetFaceBoundaryDraw = 1;
    toParseAliasArgs = true;
    if (aNames.Size() >= 2
     && aNames.Value (2).IsIntegerValue())
    {
      if (aNames.Size() == 7)
      {
        if (ViewerTest::ParseLineType (aNames.Value (7).ToCString(), aChangeSet->TypeOfFaceBoundaryLine))
        {
          aChangeSet->ToSetTypeOfFaceBoundaryLine = 1;
          aNames.Remove (7);
        }
      }
      if (aNames.Size() == 6
       && aNames.Value (6).IsRealValue())
      {
        aChangeSet->ToSetFaceBoundaryWidth = 1;
        aChangeSet->FaceBoundaryWidth = aNames.Value (6).RealValue();
        aNames.Remove (6);
      }
      if (aNames.Size() == 5
       && aNames.Value (3).IsIntegerValue()
       && aNames.Value (4).IsIntegerValue()
       && aNames.Value (5).IsIntegerValue())
      {
        aChangeSet->ToSetFaceBoundaryColor = 1;
        aChangeSet->FaceBoundaryColor = Quantity_Color (aNames.Value (3).IntegerValue() / 255.0,
                                                        aNames.Value (4).IntegerValue() / 255.0,
                                                        aNames.Value (5).IntegerValue() / 255.0,
                                                        Quantity_TOC_sRGB);
        aNames.Remove (5);
        aNames.Remove (4);
        aNames.Remove (3);
      }
      if (aNames.Size() == 2)
      {
        toParseAliasArgs = false;
        aChangeSet->ToSetFaceBoundaryDraw = aNames.Value (2).IntegerValue() == 1 ? 1 : -1;
        aNames.Remove (2);
      }
    }
  }
  else if (anArgIter >= theArgNb)
  {
    Message::SendFail ("Error: not enough arguments!");
    return 1;
  }

  if (!aChangeSet->IsEmpty()
   && !toParseAliasArgs)
  {
    anArgIter = theArgNb;
  }
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg = theArgVec[anArgIter];
    anArg.LowerCase();
    if (anArg == "-setwidth"
     || anArg == "-width"
     || anArg == "-setlinewidth"
     || anArg == "-linewidth"
     || anArg == "-setedgewidth"
     || anArg == "-setedgeswidth"
     || anArg == "-edgewidth"
     || anArg == "-edgeswidth"
     || anArg == "-setfaceboundarywidth"
     || anArg == "-setboundarywidth"
     || anArg == "-faceboundarywidth"
     || anArg == "-boundarywidth")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }

      const Standard_Real aWidth = Draw::Atof (theArgVec[anArgIter]);
      if (anArg == "-setedgewidth"
       || anArg == "-setedgeswidth"
       || anArg == "-edgewidth"
       || anArg == "-edgeswidth"
       || aCmdName == "vsetedgetype")
      {
        aChangeSet->ToSetEdgeWidth = 1;
        aChangeSet->EdgeWidth = aWidth;
      }
      else if (anArg == "-setfaceboundarywidth"
            || anArg == "-setboundarywidth"
            || anArg == "-faceboundarywidth"
            || anArg == "-boundarywidth"
            || aCmdName == "vshowfaceboundary")
      {
        aChangeSet->ToSetFaceBoundaryWidth = 1;
        aChangeSet->FaceBoundaryWidth = aWidth;
      }
      else
      {
        aChangeSet->ToSetLineWidth = 1;
        aChangeSet->LineWidth = aWidth;
      }
    }
    else if (anArg == "-unsetwidth"
          || anArg == "-unsetlinewidth"
          || anArg == "-unsetedgewidth")
    {
      if (anArg == "-unsetedgewidth")
      {
        aChangeSet->ToSetEdgeWidth = -1;
        aChangeSet->EdgeWidth = 1.0;
      }
      else
      {
        aChangeSet->ToSetLineWidth = -1;
        aChangeSet->LineWidth = 1.0;
      }
    }
    else if (anArg == "-settransp"
          || anArg == "-settransparency"
          || anArg == "-transparency"
          || anArg == "-transp")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
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
    else if (anArg == "-setalphamode"
          || anArg == "-alphamode")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetAlphaMode = 1;
      aChangeSet->AlphaCutoff = 0.5f;
      {
        TCollection_AsciiString aParam (theArgVec[anArgIter]);
        aParam.LowerCase();
        if (aParam == "opaque")
        {
          aChangeSet->AlphaMode = Graphic3d_AlphaMode_Opaque;
        }
        else if (aParam == "mask")
        {
          aChangeSet->AlphaMode = Graphic3d_AlphaMode_Mask;
        }
        else if (aParam == "blend")
        {
          aChangeSet->AlphaMode = Graphic3d_AlphaMode_Blend;
        }
        else if (aParam == "blendauto"
              || aParam == "auto")
        {
          aChangeSet->AlphaMode = Graphic3d_AlphaMode_BlendAuto;
        }
        else
        {
          Message::SendFail() << "Error: wrong syntax at " << aParam;
          return 1;
        }
      }

      if (anArgIter + 1 < theArgNb
       && theArgVec[anArgIter + 1][0] != '-')
      {
        TCollection_AsciiString aParam2 (theArgVec[anArgIter + 1]);
        if (aParam2.IsRealValue())
        {
          aChangeSet->AlphaCutoff = (float )aParam2.RealValue();
          ++anArgIter;
        }
      }
    }
    else if (anArg == "-setvis"
          || anArg == "-setvisibility"
          || anArg == "-visibility")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }

      aChangeSet->ToSetVisibility = 1;
      aChangeSet->Visibility = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (anArg == "-setalpha"
          || anArg == "-alpha")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetTransparency = 1;
      aChangeSet->Transparency  = Draw::Atof (theArgVec[anArgIter]);
      if (aChangeSet->Transparency < 0.0
       || aChangeSet->Transparency > 1.0)
      {
        Message::SendFail() << "Error: the transparency should be within [0; 1] range (specified " << aChangeSet->Transparency << ")";
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
    else if (anArg == "-setcolor"
          || anArg == "-color"
          || anArg == "-setbackfacecolor"
          || anArg == "-backfacecolor"
          || anArg == "-setbackcolor"
          || anArg == "-backcolor"
          || anArg == "-setfaceboundarycolor"
          || anArg == "-setboundarycolor"
          || anArg == "-faceboundarycolor"
          || anArg == "-boundarycolor")
    {
      Quantity_Color aColor;
      Standard_Integer aNbParsed = Draw::ParseColor (theArgNb  - anArgIter - 1,
                                                     theArgVec + anArgIter + 1,
                                                     aColor);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
      anArgIter += aNbParsed;
      if (aCmdName == "vsetedgetype")
      {
        aChangeSet->ToSetEdgeColor = 1;
        aChangeSet->EdgeColor = Quantity_ColorRGBA (aColor);
      }
      else if (aCmdName == "vshowfaceboundary"
            || anArg == "-setfaceboundarycolor"
            || anArg == "-setboundarycolor"
            || anArg == "-faceboundarycolor"
            || anArg == "-boundarycolor")
      {
        aChangeSet->ToSetFaceBoundaryColor = 1;
        aChangeSet->FaceBoundaryColor = aColor;
      }
      else if (anArg == "-setbackfacecolor"
            || anArg == "-backfacecolor"
            || anArg == "-setbackcolor"
            || anArg == "-backcolor")
      {
        aChangeSet->ToSetBackFaceColor = 1;
        aChangeSet->BackFaceColor = aColor;
      }
      else
      {
        aChangeSet->ToSetColor = 1;
        aChangeSet->Color = aColor;
      }
    }
    else if (anArg == "-setlinetype"
          || anArg == "-linetype"
          || anArg == "-setedgetype"
          || anArg == "-setedgestype"
          || anArg == "-edgetype"
          || anArg == "-edgestype"
          || anArg == "-setfaceboundarystyle"
          || anArg == "-faceboundarystyle"
          || anArg == "-boundarystyle"
          || anArg == "-setfaceboundarytype"
          || anArg == "-faceboundarytype"
          || anArg == "-setboundarytype"
          || anArg == "-boundarytype"
          || anArg == "-type")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      Aspect_TypeOfLine aLineType = Aspect_TOL_EMPTY;
      uint16_t aLinePattern = 0xFFFF;
      if (!ViewerTest::ParseLineType (theArgVec[anArgIter], aLineType, aLinePattern))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }

      if (anArg == "-setedgetype"
       || anArg == "-setedgestype"
       || anArg == "-edgetype"
       || anArg == "-edgestype"
       || aCmdName == "vsetedgetype")
      {
        aChangeSet->TypeOfEdge = Graphic3d_Aspects::DefaultLineTypeForPattern (aLinePattern);
        aChangeSet->ToSetTypeOfEdge = 1;
      }
      else if (anArg == "-setfaceboundarystyle"
            || anArg == "-faceboundarystyle"
            || anArg == "-boundarystyle"
            || anArg == "-setfaceboundarytype"
            || anArg == "-faceboundarytype"
            || anArg == "-setboundarytype"
            || anArg == "-boundarytype"
            || aCmdName == "vshowfaceboundary")
      {
        aChangeSet->TypeOfFaceBoundaryLine = Graphic3d_Aspects::DefaultLineTypeForPattern (aLinePattern);
        aChangeSet->ToSetTypeOfFaceBoundaryLine = 1;
      }
      else
      {
        aChangeSet->StippleLinePattern = aLinePattern;
        aChangeSet->ToSetTypeOfLine = 1;
      }
    }
    else if (anArg == "-unsetlinetype"
          || anArg == "-unsetedgetype"
          || anArg == "-unsetedgestype")
    {
      if (anArg == "-unsetedgetype"
       || anArg == "-unsetedgestype")
      {
        aChangeSet->ToSetTypeOfEdge = -1;
      }
      else
      {
        aChangeSet->ToSetTypeOfLine = -1;
      }
    }
    else if (anArg == "-setstipplelinefactor"
          || anArg == "-setstipplefactor"
          || anArg == "-setlinefactor"
          || anArg == "-stipplelinefactor"
          || anArg == "-stipplefactor"
          || anArg == "-linefactor")
    {
      if (aChangeSet->ToSetTypeOfLine == -1)
      {
        Message::SendFail() << "Error: -setStippleLineFactor requires -setLineType";
        return 1;
      }
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->StippleLineFactor = (uint16_t )Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (anArg == "-setmarkertype"
          || anArg == "-markertype"
          || anArg == "-setpointtype"
          || anArg == "-pointtype")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      if (!ViewerTest::ParseMarkerType (theArgVec[anArgIter], aChangeSet->TypeOfMarker, aChangeSet->MarkerImage))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
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
          || anArg == "-markersize"
          || anArg == "-setpointsize"
          || anArg == "-pointsize")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
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
          || anArg == "-mat"
          || anArg == "-setmaterial"
          || anArg == "-material")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetMaterial = 1;
      aChangeSet->MatName = theArgVec[anArgIter];
      if (!Graphic3d_MaterialAspect::MaterialFromName (aChangeSet->MatName.ToCString(), aChangeSet->Material))
      {
        Message::SendFail() << "Syntax error: unknown material '" << aChangeSet->MatName << "'.";
        return 1;
      }
    }
    else if (anArg == "-unsetmat"
          || anArg == "-unsetmaterial")
    {
      aChangeSet->ToSetMaterial = -1;
      aChangeSet->Material = Graphic3d_NameOfMaterial_DEFAULT;
    }
    else if (anArg == "-subshape"
          || anArg == "-subshapes")
    {
      if (isDefaults)
      {
        Message::SendFail() << "Error: wrong syntax. -subshapes can not be used together with -defaults call!";
        return 1;
      }

      if (aNames.IsEmpty())
      {
        Message::SendFail() << "Error: main objects should specified explicitly when -subshapes is used!";
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
          Message::SendFail() << "Error: shape " << aSubShapeName << " doesn't found!";
          return 1;
        }
        aChangeSet->SubShapes.Append (aSubShape);
      }

      if (aChangeSet->SubShapes.IsEmpty())
      {
        Message::SendFail() << "Error: empty list is specified after -subshapes!";
        return 1;
      }
    }
    else if (anArg == "-setfreeboundary"
          || anArg == "-freeboundary"
          || anArg == "-setfb"
          || anArg == "-fb")
    {
      bool toEnable = true;
      if (!Draw::ParseOnOff (anArgIter + 1 < theArgNb ? theArgVec[anArgIter + 1] : "", toEnable))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      ++anArgIter;
      aChangeSet->ToSetShowFreeBoundary = toEnable ? 1 : -1;
    }
    else if (anArg == "-setfreeboundarywidth"
          || anArg == "-freeboundarywidth"
          || anArg == "-setfbwidth"
          || anArg == "-fbwidth")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
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
          || anArg == "-freeboundarycolor"
          || anArg == "-setfbcolor"
          || anArg == "-fbcolor")
    {
      Standard_Integer aNbParsed = Draw::ParseColor (theArgNb  - anArgIter - 1,
                                                     theArgVec + anArgIter + 1,
                                                     aChangeSet->FreeBoundaryColor);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
      anArgIter += aNbParsed;
      aChangeSet->ToSetFreeBoundaryColor = 1;
    }
    else if (anArg == "-unsetfreeboundarycolor"
          || anArg == "-unsetfbcolor")
    {
      aChangeSet->ToSetFreeBoundaryColor = -1;
      aChangeSet->FreeBoundaryColor = DEFAULT_FREEBOUNDARY_COLOR;
    }
    else if (anArg == "-setisoontriangulation"
          || anArg == "-isoontriangulation"
          || anArg == "-setisoontriang"
          || anArg == "-isoontriang")
    {
      bool toEnable = true;
      if (!Draw::ParseOnOff (anArgIter + 1 < theArgNb ? theArgVec[anArgIter + 1] : "", toEnable))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      ++anArgIter;
      aChangeSet->ToEnableIsoOnTriangulation = toEnable ? 1 : -1;
    }
    else if (anArg == "-setfaceboundarydraw"
          || anArg == "-setdrawfaceboundary"
          || anArg == "-setdrawfaceboundaries"
          || anArg == "-setshowfaceboundary"
          || anArg == "-setshowfaceboundaries"
          || anArg == "-setdrawfaceedges"
          || anArg == "-faceboundarydraw"
          || anArg == "-drawfaceboundary"
          || anArg == "-drawfaceboundaries"
          || anArg == "-showfaceboundary"
          || anArg == "-showfaceboundaries"
          || anArg == "-drawfaceedges"
          || anArg == "-faceboundary"
          || anArg == "-faceboundaries"
          || anArg == "-faceedges")
    {
      bool toEnable = true;
      if (!Draw::ParseOnOff (anArgIter + 1 < theArgNb ? theArgVec[anArgIter + 1] : "", toEnable))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      ++anArgIter;
      aChangeSet->ToSetFaceBoundaryDraw = toEnable ? 1 : -1;
    }
    else if (anArg == "-unsetfaceboundary"
          || anArg == "-unsetboundary")
    {
      aChangeSet->ToSetFaceBoundaryDraw  = -1;
      aChangeSet->ToSetFaceBoundaryColor = -1;
    }
    else if (anArg == "-setmostcontinuity"
          || anArg == "-mostcontinuity")
    {
      TCollection_AsciiString aClassArg (anArgIter + 1 < theArgNb ? theArgVec[anArgIter + 1] : "");
      aClassArg.LowerCase();
      GeomAbs_Shape aClass = GeomAbs_CN;
      if (aClassArg == "c0"
       || aClassArg == "0")
      {
        aClass = GeomAbs_C0;
      }
      else if (aClassArg == "g1")
      {
        aClass = GeomAbs_G1;
      }
      else if (aClassArg == "c1"
            || aClassArg == "1")
      {
        aClass = GeomAbs_C1;
      }
      else if (aClassArg == "g2")
      {
        aClass = GeomAbs_G2;
      }
      else if (aClassArg == "c2"
            || aClassArg == "2")
      {
        aClass = GeomAbs_C2;
      }
      else if (aClassArg == "c3"
            || aClassArg == "3")
      {
        aClass = GeomAbs_C3;
      }
      else if (aClassArg == "cn"
            || aClassArg == "n")
      {
        aClass = GeomAbs_CN;
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }

      ++anArgIter;
      aChangeSet->ToSetFaceBoundaryUpperContinuity = 1;
      aChangeSet->FaceBoundaryUpperContinuity = aClass;
    }
    else if (anArg == "-setmaxparamvalue"
          || anArg == "-maxparamvalue")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetMaxParamValue = 1;
      aChangeSet->MaxParamValue = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (anArg == "-setsensitivity"
          || anArg == "-sensitivity")
    {
      if (isDefaults)
      {
        Message::SendFail() << "Error: wrong syntax. -setSensitivity can not be used together with -defaults call!";
        return 1;
      }

      if (aNames.IsEmpty())
      {
        Message::SendFail() << "Error: object and selection mode should specified explicitly when -setSensitivity is used!";
        return 1;
      }

      if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetSensitivity = 1;
      aChangeSet->SelectionMode = Draw::Atoi (theArgVec[++anArgIter]);
      aChangeSet->Sensitivity = Draw::Atoi (theArgVec[++anArgIter]);
    }
    else if (anArg == "-sethatch"
          || anArg == "-hatch")
    {
      if (isDefaults)
      {
        Message::SendFail() << "Error: wrong syntax. -setHatch can not be used together with -defaults call!";
        return 1;
      }

      if (aNames.IsEmpty())
      {
        Message::SendFail() << "Error: object should be specified explicitly when -setHatch is used!";
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
          Message::SendFail() << "Error: hatch style is out of range [0, " << (Aspect_HS_NB - 1) << "]!";
          return 1;
        }
        aChangeSet->StdHatchStyle = anIntStyle;
      }
      else
      {
        aChangeSet->PathToHatchPattern = anArgHatch;
      }
    }
    else if (anArg == "-setshadingmodel"
          || anArg == "-setshading"
          || anArg == "-shadingmodel"
          || anArg == "-shading")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetShadingModel = 1;
      aChangeSet->ShadingModelName  = theArgVec[anArgIter];
      if (!ViewerTest::ParseShadingModel (theArgVec[anArgIter], aChangeSet->ShadingModel))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
    }
    else if (anArg == "-unsetshadingmodel")
    {
      aChangeSet->ToSetShadingModel = -1;
      aChangeSet->ShadingModel = Graphic3d_TOSM_DEFAULT;
    }
    else if (anArg == "-setinterior"
          || anArg == "-setinteriorstyle"
          || anArg == "-interior"
          || anArg == "-interiorstyle")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetInterior = 1;
      if (!parseInteriorStyle (theArgVec[anArgIter], aChangeSet->InteriorStyle))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
    }
    else if (anArg == "-unsetinterior")
    {
      aChangeSet->ToSetInterior = -1;
      aChangeSet->InteriorStyle = Aspect_IS_SOLID;
    }
    else if (anArg == "-setdrawoutline"
          || anArg == "-setdrawsilhouette"
          || anArg == "-setoutline"
          || anArg == "-setsilhouette"
          || anArg == "-outline"
          || anArg == "-outlined"
          || anArg == "-silhouette")
    {
      bool toDrawOutline = true;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toDrawOutline))
      {
        ++anArgIter;
      }
      aChangeSet->ToSetDrawSilhouette = toDrawOutline ? 1 : -1;
    }
    else if (anArg == "-setdrawedges"
          || anArg == "-setdrawedge"
          || anArg == "-drawedges"
          || anArg == "-drawedge"
          || anArg == "-edges")
    {
      bool toDrawEdges = true;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toDrawEdges))
      {
        ++anArgIter;
      }
      aChangeSet->ToSetDrawEdges = toDrawEdges ? 1 : -1;
    }
    else if (anArg == "-setquadedges"
          || anArg == "-setquads"
          || anArg == "-quads"
          || anArg == "-skipfirstedge")
    {
      bool isQuadMode = true;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], isQuadMode))
      {
        ++anArgIter;
      }
      aChangeSet->ToSetQuadEdges = isQuadMode ? 1 : -1;
    }
    else if (anArg == "-setedgecolor"
          || anArg == "-setedgescolor"
          || anArg == "-edgecolor"
          || anArg == "-edgescolor")
    {
      Standard_Integer aNbParsed = Draw::ParseColor (theArgNb  - anArgIter - 1,
                                                     theArgVec + anArgIter + 1,
                                                     aChangeSet->EdgeColor);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
      anArgIter += aNbParsed;
      aChangeSet->ToSetEdgeColor = 1;
    }
    else if (anArg == "-unset")
    {
      aChangeSet->ToSetVisibility = 1;
      aChangeSet->Visibility = 1;
      aChangeSet->ToSetLineWidth = -1;
      aChangeSet->LineWidth = 1.0;
      aChangeSet->ToSetTypeOfLine = -1;
      aChangeSet->StippleLinePattern = 0xFFFF;
      aChangeSet->StippleLineFactor = 1;
      aChangeSet->ToSetTypeOfMarker = -1;
      aChangeSet->TypeOfMarker = Aspect_TOM_PLUS;
      aChangeSet->ToSetMarkerSize = -1;
      aChangeSet->MarkerSize = 1.0;
      aChangeSet->ToSetTransparency = -1;
      aChangeSet->Transparency = 0.0;
      aChangeSet->ToSetAlphaMode = -1;
      aChangeSet->AlphaMode = Graphic3d_AlphaMode_BlendAuto;
      aChangeSet->AlphaCutoff = 0.5f;
      aChangeSet->ToSetColor = -1;
      aChangeSet->Color = DEFAULT_COLOR;
      //aChangeSet->ToSetBackFaceColor = -1; // should be reset by ToSetColor
      //aChangeSet->BackFaceColor = DEFAULT_COLOR;
      aChangeSet->ToSetMaterial = -1;
      aChangeSet->Material = Graphic3d_NameOfMaterial_DEFAULT;
      aChangeSet->ToSetShowFreeBoundary = -1;
      aChangeSet->ToSetFreeBoundaryColor = -1;
      aChangeSet->FreeBoundaryColor = DEFAULT_FREEBOUNDARY_COLOR;
      aChangeSet->ToSetFreeBoundaryWidth = -1;
      aChangeSet->FreeBoundaryWidth = 1.0;
      aChangeSet->ToEnableIsoOnTriangulation = -1;
      //
      aChangeSet->ToSetFaceBoundaryDraw = -1;
      aChangeSet->ToSetFaceBoundaryUpperContinuity = -1;
      aChangeSet->FaceBoundaryUpperContinuity = GeomAbs_CN;
      aChangeSet->ToSetFaceBoundaryColor = -1;
      aChangeSet->FaceBoundaryColor = Quantity_NOC_BLACK;
      aChangeSet->ToSetFaceBoundaryWidth = -1;
      aChangeSet->FaceBoundaryWidth = 1.0f;
      aChangeSet->ToSetTypeOfFaceBoundaryLine = -1;
      aChangeSet->TypeOfFaceBoundaryLine = Aspect_TOL_SOLID;
      //
      aChangeSet->ToSetHatch = -1;
      aChangeSet->StdHatchStyle = -1;
      aChangeSet->PathToHatchPattern.Clear();
      aChangeSet->ToSetShadingModel = -1;
      aChangeSet->ShadingModel = Graphic3d_TOSM_DEFAULT;
      aChangeSet->ToSetInterior = -1;
      aChangeSet->InteriorStyle = Aspect_IS_SOLID;
      aChangeSet->ToSetDrawSilhouette = -1;
      aChangeSet->ToSetDrawEdges = -1;
      aChangeSet->ToSetQuadEdges = -1;
      aChangeSet->ToSetEdgeColor = -1;
      aChangeSet->EdgeColor = Quantity_ColorRGBA (DEFAULT_COLOR);
      aChangeSet->ToSetEdgeWidth = -1;
      aChangeSet->EdgeWidth = 1.0;
      aChangeSet->ToSetTypeOfEdge = -1;
      aChangeSet->TypeOfEdge = Aspect_TOL_SOLID;
    }
    else if (anArg == "-dumpjson")
    {
      toDump = Standard_True;
    }
    else if (anArg == "-dumpcompact")
    {
      toCompactDump = Standard_False;
      if (++anArgIter >= theArgNb && Draw::ParseOnOff (theArgVec[anArgIter + 1], toCompactDump))
        ++anArgIter;
    }
    else if (anArg == "-dumpdepth")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aDumpDepth = Draw::Atoi (theArgVec[anArgIter]);
    }
    else
    {
      Message::SendFail() << "Error: wrong syntax at " << anArg;
      return 1;
    }
  }

  for (NCollection_Sequence<ViewerTest_AspectsChangeSet>::Iterator aChangesIter (aChanges);
       aChangesIter.More(); aChangesIter.Next())
  {
    if (!aChangesIter.Value().Validate())
    {
      return 1;
    }
  }

  // special case for -defaults parameter.
  // all changed values will be set to DefaultDrawer.
  if (isDefaults)
  {
    const Handle(Prs3d_Drawer)& aDrawer = aCtx->DefaultDrawer();
    aChangeSet->Apply (aDrawer);
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
    if (aChangeSet->ToSetTransparency != 0)
    {
      aDrawer->ShadingAspect()->SetTransparency (aChangeSet->Transparency);
    }
    if (aChangeSet->ToSetMaterial != 0)
    {
      aDrawer->ShadingAspect()->SetMaterial (aChangeSet->Material);
    }
    if (aChangeSet->ToEnableIsoOnTriangulation != 0)
    {
      aDrawer->SetIsoOnTriangulation (aChangeSet->ToEnableIsoOnTriangulation == 1);
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
    if (toDump)
    {
      Standard_SStream aStream;
      aDrawer->DumpJson (aStream, aDumpDepth);

      if (toCompactDump)
        theDI << Standard_Dump::Text (aStream);
      else
        theDI << Standard_Dump::FormatJson (aStream);
    }
    return 0;
  }

  for (ViewTest_PrsIter aPrsIter (aNames); aPrsIter.More(); aPrsIter.Next())
  {
    const TCollection_AsciiString& aName = aPrsIter.CurrentName();
    Handle(AIS_InteractiveObject)  aPrs  = aPrsIter.Current();
    if (aPrs.IsNull())
    {
      return 1;
    }

    Handle(Prs3d_Drawer)           aDrawer = aPrs->Attributes();
    Handle(AIS_ColoredShape) aColoredPrs;
    Standard_Boolean toDisplay = Standard_False;
    Standard_Boolean toRedisplay = Standard_False;
    if (aChanges.Length() > 1 || aChangeSet->ToSetVisibility == 1)
    {
      Handle(AIS_Shape) aShapePrs = Handle(AIS_Shape)::DownCast (aPrs);
      if (aShapePrs.IsNull())
      {
        Message::SendFail() << "Error: an object " << aName << " is not an AIS_Shape presentation!";
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
      else if (aChangeSet->ToEnableIsoOnTriangulation != 0)
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
        toRedisplay = aChangeSet->Apply (aDrawer) || toRedisplay;
      }

      for (aChangesIter.Next(); aChangesIter.More(); aChangesIter.Next())
      {
        aChangeSet = &aChangesIter.ChangeValue();
        for (NCollection_Sequence<TopoDS_Shape>::Iterator aSubShapeIter (aChangeSet->SubShapes);
             aSubShapeIter.More(); aSubShapeIter.Next())
        {
          const TopoDS_Shape& aSubShape = aSubShapeIter.Value();
          if (!aChangeSet->IsEmpty())
          {
            Handle(AIS_ColoredDrawer) aCurColDrawer = aColoredPrs->CustomAspects (aSubShape);
            aChangeSet->Apply (aCurColDrawer);
          }
          if (aChangeSet->ToSetVisibility == 1)
          {
            Handle(AIS_ColoredDrawer) aCurColDrawer = aColoredPrs->CustomAspects (aSubShape);
            aCurColDrawer->SetHidden (aChangeSet->Visibility == 0);
          }
          if (aChangeSet->ToSetColor == 1)
          {
            aColoredPrs->SetCustomColor (aSubShape, aChangeSet->Color);
          }
          if (aChangeSet->ToSetTransparency == 1)
          {
            aColoredPrs->SetCustomTransparency (aSubShape, aChangeSet->Transparency);
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
      else
      {
        aPrs->SynchronizeAspects();
      }

      if (toDump)
      {
        Standard_SStream aStream;
        aDrawer->DumpJson (aStream);

        theDI << aName << ": \n";
        theDI << Standard_Dump::FormatJson (aStream);
        theDI << "\n";
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
    Message::SendFail ("Error: no active view!");
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
      Handle(AIS_InteractiveObject) aShape;
      if (GetMapOfAIS().Find2 (aName, aShape)
      && !aShape.IsNull())
      {
        aCtx->Display (aShape, Standard_False);
        aDispSet.Add (aShape);
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

    if (Handle(AIS_InteractiveObject) aShape = anIter.Key1())
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
    Message::SendFail ("Error: no active view!");
    return 1;
  }

  Standard_Boolean isContextOnly = Standard_False;
  Standard_Boolean toRemoveAll   = Standard_False;
  Standard_Boolean toPrintInfo   = Standard_True;
  Standard_Boolean toFailOnError = Standard_True;

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
    else if (anArg == "-noerror"
          || anArg == "-nofail")
    {
      toFailOnError = Standard_False;
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
    Message::SendFail ("Error: wrong syntax!");
    return 1;
  }

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
      const TCollection_AsciiString aName (theArgVec[anArgIter]);
      if (aName.Search ("*") != -1)
      {
        for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName aPrsIter (GetMapOfAIS()); aPrsIter.More(); aPrsIter.Next())
        {
          if (aPrsIter.Key1()->GetContext() != aCtx)
          {
            continue;
          }
          const TCollection_AsciiString aCheck = TCollection_AsciiString ("string match '") + aName + "' '" + aPrsIter.Key2() + "'";
          if (theDI.Eval (aCheck.ToCString()) == 0
          && *theDI.Result() == '1')
          {
            anIONameList.Append (aPrsIter.Key2());
          }
        }
        theDI.Reset();
        continue;
      }

      Handle(AIS_InteractiveObject) anIO;
      if (!GetMapOfAIS().Find2 (aName, anIO))
      {
        if (toFailOnError)
        {
          Message::SendFail() << "Syntax error: '" << aName << "' was not bound to some object.";
          return 1;
        }
      }
      else if (anIO->GetContext() != aCtx)
      {
        if (toFailOnError)
        {
          Message::SendFail() << "Syntax error: '" << aName << "' was not displayed in current context.\n"
                              << "Please activate view with this object displayed and try again.";
          return 1;
        }
      }
      else
      {
        anIONameList.Append (aName);
      }
    }
  }
  else if (aCtx->NbSelected() > 0)
  {
    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
         anIter.More(); anIter.Next())
    {
      if (!aCtx->IsSelected (anIter.Key1()))
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
    const Handle(AIS_InteractiveObject) anIO = GetMapOfAIS().Find2 (anIter.Value());
    aCtx->Remove (anIO, Standard_False);
    if (toPrintInfo)
    {
      theDI << anIter.Value() << " ";
    }
    if (!isContextOnly)
    {
      GetMapOfAIS().UnBind2 (anIter.Value());
    }
  }
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
    Message::SendFail ("Error: no active view!");
    return 1;
  }

  const Standard_Boolean toEraseAll = TCollection_AsciiString (theArgNb > 0 ? theArgVec[0] : "") == "veraseall";

  Standard_Integer anArgIter = 1;
  Standard_Boolean toEraseInView = Standard_False;
  Standard_Boolean toFailOnError = Standard_True;
  TColStd_SequenceOfAsciiString aNamesOfEraseIO;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArgCase (theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArgCase))
    {
      continue;
    }
    else if (anArgCase == "-view"
          || anArgCase == "-inview")
    {
      toEraseInView = Standard_True;
    }
    else if (anArgCase == "-noerror"
          || anArgCase == "-nofail")
    {
      toFailOnError = Standard_False;
    }
    else
    {
      aNamesOfEraseIO.Append (theArgVec[anArgIter]);
    }
  }

  if (!aNamesOfEraseIO.IsEmpty() && toEraseAll)
  {
    Message::SendFail() << "Error: wrong syntax, " << theArgVec[0] << " too much arguments.";
    return 1;
  }

  if (!aNamesOfEraseIO.IsEmpty())
  {
    // Erase named objects
    NCollection_IndexedDataMap<Handle(AIS_InteractiveObject), TCollection_AsciiString> aPrsList;
    for (TColStd_SequenceOfAsciiString::Iterator anIter (aNamesOfEraseIO); anIter.More(); anIter.Next())
    {
      const TCollection_AsciiString& aName = anIter.Value();
      if (aName.Search ("*") != -1)
      {
        for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName aPrsIter (GetMapOfAIS()); aPrsIter.More(); aPrsIter.Next())
        {
          const TCollection_AsciiString aCheck = TCollection_AsciiString ("string match '") + aName + "' '" + aPrsIter.Key2() + "'";
          if (theDI.Eval (aCheck.ToCString()) == 0
          && *theDI.Result() == '1')
          {
            aPrsList.Add (aPrsIter.Key1(), aPrsIter.Key2());
          }
        }
        theDI.Reset();
      }
      else
      {
        Handle(AIS_InteractiveObject) anIO;
        if (!GetMapOfAIS().Find2 (aName, anIO))
        {
          if (toFailOnError)
          {
            Message::SendFail() << "Syntax error: '" << aName << "' is not found";
            return 1;
          }
        }
        else
        {
          aPrsList.Add (anIO, aName);
        }
      }
    }

    for (NCollection_IndexedDataMap<Handle(AIS_InteractiveObject), TCollection_AsciiString>::Iterator anIter (aPrsList); anIter.More(); anIter.Next())
    {
      theDI << anIter.Value() << " ";
      if (toEraseInView)
      {
        aCtx->SetViewAffinity (anIter.Key(), aView, Standard_False);
      }
      else
      {
        aCtx->Erase (anIter.Key(), Standard_False);
      }
    }
  }
  else if (!toEraseAll && aCtx->NbSelected() > 0)
  {
    // Erase selected objects
    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
         anIter.More(); anIter.Next())
    {
      const Handle(AIS_InteractiveObject) anIO = anIter.Key1();
      if (!anIO.IsNull()
       && aCtx->IsSelected (anIO))
      {
        theDI << anIter.Key2() << " ";
        if (toEraseInView)
        {
          aCtx->SetViewAffinity (anIO, aView, Standard_False);
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
      Handle(AIS_InteractiveObject) anIO = anIter.Key1();
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
    Message::SendFail ("Error: no active view!");
    return 1;
  }

  Standard_Integer anArgIter = 1;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArgCase (theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArgCase))
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
    Message::SendFail() << theArgVec[0] << "Error: wrong syntax";
    return 1;
  }

  for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
       anIter.More(); anIter.Next())
  {
    aCtx->Erase (anIter.Key1(), Standard_False);
  }

  for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
       anIter.More(); anIter.Next())
  {
    aCtx->Display (anIter.Key1(), Standard_False);
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
        if (aPrsIter.Value()->Mode() != theDispMode)
          continue;

        aBox = aPrsIter.Value()->MinMaxValues();
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
    Message::SendFail ("Error: no active view!");
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
        Message::SendFail() << "Error: wrong syntax at " << anArg;
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
      Handle(AIS_InteractiveObject) anIO;
      if (!GetMapOfAIS().Find2 (aName, anIO))
      {
        Message::SendFail() << "Error: presentation " << aName << " does not exist";
        return 1;
      }

      aHighlightedMode = checkMode (aCtx, anIO, aMode);
      if (aHighlightedMode == -1)
      {
        Message::SendFail() << "Error: object " << aName << " has no presentation with mode " << aMode;
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
      Handle(AIS_InteractiveObject) anIO = anIter.Key1();
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
    Message::SendFail() << "Error: no active view!";
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

  Handle(AIS_InteractiveObject) aTexturedIO;
  Handle(AIS_Shape) aTexturedShape;
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
        aTexturedIO = aMapOfIO.Find2 (aName);
        aTexturedShape = Handle(AIS_Shape)::DownCast (aTexturedIO);
      }
      if (aTexturedIO.IsNull())
      {
        Message::SendFail() << "Syntax error: shape " << aName << " does not exists in the viewer.";
        return 1;
      }

      if (aTexturedIO->Attributes()->HasOwnShadingAspect())
      {
        aTextureSetOld = aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureSet();
      }
    }
    else if (!aTexturedShape.IsNull()
          && (aNameCase == "-scale"
           || aNameCase == "-setscale"
           || aCommandName == "vtexscale"))
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
          aTexturedShape->SetTextureScaleUV (gp_Pnt2d (1.0, 1.0));
          continue;
        }
        else if (anArgIter + 1 < theArgsNb)
        {
          TCollection_AsciiString aValV (theArgVec[anArgIter + 1]);
          if (aValU.IsRealValue()
           && aValV.IsRealValue())
          {
            aTexturedShape->SetTextureScaleUV (gp_Pnt2d (aValU.RealValue(), aValV.RealValue()));
            ++anArgIter;
            continue;
          }
        }
      }
      Message::SendFail() << "Syntax error: unexpected argument '" << aName << "'";
      return 1;
    }
    else if (!aTexturedShape.IsNull()
          && (aNameCase == "-origin"
           || aNameCase == "-setorigin"
           || aCommandName == "vtexorigin"))
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
          aTexturedShape->SetTextureOriginUV (gp_Pnt2d (0.0, 0.0));
          continue;
        }
        else if (anArgIter + 1 < theArgsNb)
        {
          TCollection_AsciiString aValV (theArgVec[anArgIter + 1]);
          if (aValU.IsRealValue()
           && aValV.IsRealValue())
          {
            aTexturedShape->SetTextureOriginUV (gp_Pnt2d (aValU.RealValue(), aValV.RealValue()));
            ++anArgIter;
            continue;
          }
        }
      }
      Message::SendFail() << "Syntax error: unexpected argument '" << aName << "'";
      return 1;
    }
    else if (!aTexturedShape.IsNull()
          && (aNameCase == "-repeat"
           || aNameCase == "-setrepeat"
           || aCommandName == "vtexrepeat"))
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
          aTexturedShape->SetTextureRepeatUV (gp_Pnt2d (1.0, 1.0));
          continue;
        }
        else if (anArgIter + 1 < theArgsNb)
        {
          TCollection_AsciiString aValV (theArgVec[anArgIter + 1]);
          if (aValU.IsRealValue()
           && aValV.IsRealValue())
          {
            aTexturedShape->SetTextureRepeatUV (gp_Pnt2d (aValU.RealValue(), aValV.RealValue()));
            ++anArgIter;
            continue;
          }
        }
      }
      Message::SendFail() << "Syntax error: unexpected argument '" << aName << "'";
      return 1;
    }
    else if (aNameCase == "-modulate")
    {
      bool toModulateBool = true;
      if (anArgIter + 1 < theArgsNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toModulateBool))
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
        Message::SendFail() << "Syntax error: unexpected argument '" << aValue << "'";
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
        Message::SendFail() << "Syntax error: unexpected argument '" << aValue << "'";
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
    else if ((aNameCase == "-video")
           && anArgIter + 1 < theArgsNb)
    {
      const TCollection_AsciiString anInput (theArgVec[++anArgIter]);
      Handle(Graphic3d_MediaTextureSet) aMedia = Handle(Graphic3d_MediaTextureSet)::DownCast (aTextureSetOld);
      if (aMedia.IsNull())
      {
        aMedia = new Graphic3d_MediaTextureSet();
      }
      if (aMedia->Input() != anInput)
      {
        aMedia->OpenInput (anInput, false);
      }
      else
      {
        if (aMedia->SwapFrames()
        && !aCtx->CurrentViewer()->ZLayerSettings (aTexturedIO->ZLayer()).IsImmediate())
        {
          ViewerTest::CurrentView()->Invalidate();
        }
      }
      if (aTexturedIO->Attributes()->SetupOwnShadingAspect (aCtx->DefaultDrawer())
       && aTexturedShape.IsNull())
      {
        aTexturedIO->SetToUpdate();
      }

      toComputeUV = aTextureSetOld.IsNull();
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->SetTextureMapOn (true);
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->SetTextureSet (aMedia);
      aTextureSetOld.Nullify();
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
          Message::SendFail() << "Syntax error: invalid argument '" << theArgVec[anArgIter] << "'";
          return 1;
        }

        TCollection_AsciiString aTexIndexStr = aNameCase.SubString (5, aNameCase.Length());
        if (!aTexIndexStr.IsIntegerValue())
        {
          Message::SendFail() << "Syntax error: invalid argument '" << theArgVec[anArgIter] << "'";
          return 1;
        }

        aTexIndex = aTexIndexStr.IntegerValue();
        aTexName  = theArgVec[anArgIter + 1];
        ++anArgIter;
      }
      if (aTexIndex >= Graphic3d_TextureUnit_NB
       || aTexIndex >= aCtx->CurrentViewer()->Driver()->InquireLimit (Graphic3d_TypeOfLimit_MaxCombinedTextureUnits))
      {
        Message::SendFail ("Error: too many textures specified");
        return 1;
      }

      toSetImage = true;
      if (aTexName.IsIntegerValue())
      {
        const Standard_Integer aValue = aTexName.IntegerValue();
        if (aValue < 0 || aValue >= Graphic3d_Texture2D::NumberOfTextures())
        {
          Message::SendFail() << "Syntax error: texture with ID " << aValue << " is undefined!";
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
          Message::SendFail() << "Syntax error: non-existing image file has been specified '" << aTexName << "'.";
          return 1;
        }
        aTextureVecNew.SetValue (aTexIndex, new Graphic3d_Texture2Dmanual (aTexName));
      }
      else
      {
        aTextureVecNew.SetValue (aTexIndex, Handle(Graphic3d_Texture2Dmanual)());
      }

      if (aTextureVecNew.Value (aTexIndex))
      {
        aTextureVecNew.ChangeValue(aTexIndex)->GetParams()->SetTextureUnit((Graphic3d_TextureUnit)aTexIndex);
      }
    }
    else
    {
      Message::SendFail() << "Syntax error: invalid argument '" << theArgVec[anArgIter] << "'";
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

    if (aTexturedIO->Attributes()->SetupOwnShadingAspect (aCtx->DefaultDrawer())
     && aTexturedShape.IsNull())
    {
      aTexturedIO->SetToUpdate();
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
    if (!aTexturedShape.IsNull())
    {
      aTexturedShape->SetTextureRepeatUV (gp_Pnt2d (1.0, 1.0));
    }
    toSetGenRepeat = true;
  }
  if (!toSetGenOrigin
   && (aCommandName == "vtexorigin"
    || toSetDefaults))
  {
    if (!aTexturedShape.IsNull())
    {
      aTexturedShape->SetTextureOriginUV (gp_Pnt2d (0.0, 0.0));
    }
    toSetGenOrigin = true;
  }
  if (!toSetGenScale
   && (aCommandName == "vtexscale"
    || toSetDefaults))
  {
    if (!aTexturedShape.IsNull())
    {
      aTexturedShape->SetTextureScaleUV  (gp_Pnt2d (1.0, 1.0));
    }
    toSetGenScale = true;
  }

  if (toSetGenRepeat || toSetGenOrigin || toSetGenScale || toComputeUV)
  {
    if (!aTexturedShape.IsNull())
    {
      aTexturedShape->SetToUpdate (AIS_Shaded);
      if (toSetImage)
      {
        if ((aTexturedIO->HasDisplayMode() && aTexturedIO->DisplayMode() != AIS_Shaded)
         || aCtx->DisplayMode() != AIS_Shaded)
        {
          aCtx->SetDisplayMode (aTexturedIO, AIS_Shaded, false);
        }
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
    Message::SendFail ("Syntax error: wrong number of arguments.");
    return 1;
  }
  if (theArgNb == 2
   && TCollection_AsciiString (theArgVec[1]) == "*")
  {
    // alias
    return VDisplayAll (theDI, 1, theArgVec);
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
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
        return 1;
      }

      anObjDispMode = Draw::Atoi (theArgVec [anArgIter]);
    }
    else if (aNameCase == "-himode"
          || aNameCase == "-highmode"
          || aNameCase == "-highlightmode")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
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
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
        return 1;
      }

      toSetTrsfPers  = Standard_True;
      Graphic3d_TransModeFlags aTrsfPersFlags = Graphic3d_TMF_None;
      TCollection_AsciiString aPersFlags (theArgVec [anArgIter]);
      aPersFlags.LowerCase();
      if (!parseTrsfPersFlag (aPersFlags, aTrsfPersFlags))
      {
        Message::SendFail() << "Error: wrong transform persistence flags " << theArgVec [anArgIter] << ".";
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
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
        return 1;
      }

      TCollection_AsciiString aX (theArgVec[++anArgIter]);
      TCollection_AsciiString aY (theArgVec[++anArgIter]);
      TCollection_AsciiString aZ = "0";
      if (!aX.IsRealValue()
       || !aY.IsRealValue())
      {
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
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
    else if (aNameCase == "-layer"
          || aNameCase == "-zlayer")
    {
      ++anArgIter;
      if (anArgIter >= theArgNb
      || !ViewerTest::ParseZLayer (theArgVec[anArgIter], aZLayer)
      ||  aZLayer == Graphic3d_ZLayerId_UNKNOWN)
      {
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
        return 1;
      }
    }
    else if (aNameCase == "-view"
          || aNameCase == "-inview")
    {
      toDisplayInView = Standard_True;
    }
    else if (aNameCase == "-redisplay")
    {
      toReDisplay = Standard_True;
    }
    else if (aNameCase == "-erased"
          || aNameCase == "-load")
    {
      aDispStatus = AIS_DS_Erased;
    }
    else
    {
      aNamesOfDisplayIO.Append (aName);
    }
  }

  if (aNamesOfDisplayIO.IsEmpty())
  {
    Message::SendFail ("Syntax error: wrong number of arguments.");
    return 1;
  }

  // Display interactive objects
  for (Standard_Integer anIter = 1; anIter <= aNamesOfDisplayIO.Length(); ++anIter)
  {
    const TCollection_AsciiString& aName = aNamesOfDisplayIO.Value (anIter);
    Handle(AIS_InteractiveObject) aShape;
    if (!GetMapOfAIS().Find2 (aName, aShape))
    {
      // create the AIS_Shape from a name
      TopoDS_Shape aDrawShape = DBRep::GetExisting (aName);
      if (!aDrawShape.IsNull())
      {
        aShape = new AIS_Shape (aDrawShape);
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
          if (anObjDispMode == -1)
          {
            aShape->UnsetDisplayMode();
          }
          if (!aShape->AcceptDisplayMode (anObjDispMode))
          {
            Message::SendFail() << "Syntax error: " << aShape->DynamicType()->Name() << " rejects " << anObjDispMode << " display mode";
            return 1;
          }
          else
          {
            aShape->SetDisplayMode (anObjDispMode);
          }
        }
        if (anObjHighMode != -2)
        {
          if (anObjHighMode != -1
          && !aShape->AcceptDisplayMode (anObjHighMode))
          {
            Message::SendFail() << "Syntax error: " << aShape->DynamicType()->Name() << " rejects " << anObjHighMode << " display mode";
            return 1;
          }
          aShape->SetHilightMode (anObjHighMode);
        }

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

        aCtx->Display (aShape, aDispMode, aSelMode, Standard_False, aDispStatus);
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
        Message::SendFail() << "Error: object with name '" << aName << "' does not exist!";
      }
      continue;
    }

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
      theDI << "Display " << aName << "\n";

      // update the Shape in the AIS_Shape
      TopoDS_Shape      aNewShape = DBRep::GetExisting (aName);
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
      aCtx->Display (aShape, aDispMode, aSelMode, Standard_False, aDispStatus);
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
    Message::SendFail ("Syntax error: AIS context is not available.");
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    theDi << "use 'vinit' command before " << theArgVec[0] << "\n";
    return 1;
  }

  AIS_ListOfInteractive aListOfIO;
  aContextAIS->DisplayedObjects (aListOfIO);

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
    Message::SendFail ("Syntax error: AIS context is not available.");
    return 1;
  }

  if (theArgsNb < 2)
  {
    Message::SendFail ("Syntax error: insufficient arguments. Type help for more information.");
    return 1;
  }

  AIS_ListOfInteractive aListOfIO;
  for (int anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    TCollection_AsciiString aName = TCollection_AsciiString (theArgVec[anArgIt]);

    Handle(AIS_InteractiveObject) anAISObj;
    GetMapOfAIS().Find2 (aName, anAISObj);
    if (anAISObj.IsNull())
    {
      Message::SendFail() << theArgVec[0] << ": no AIS interactive object named \"" << aName << "\".";
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
  GetMapOfAIS().Find2(name, TheAisIO);
  if (TheAisIO.IsNull())
  {
    TopoDS_Shape aDrawShape = DBRep::GetExisting (name);
    if (!aDrawShape.IsNull())
    {
      TheAisIO = new AIS_Shape (aDrawShape);
    }
  }

  if (HaveToSet)
    TheAISContext()->SetDeviationCoefficient(TheAisIO,myDevCoef,Standard_True);
  else
    TheAISContext()->SetDeviationCoefficient(TheAisIO,0.0008,Standard_True);

  TheAISContext()->Redisplay (TheAisIO, Standard_True);
  return 0;
}

//! Auxiliary method to print Interactive Object information
static void objInfo (const NCollection_Map<Handle(AIS_InteractiveObject)>& theDetected,
                     const Handle(AIS_InteractiveObject)&                  theObj,
                     Draw_Interpretor&                                     theDI)
{
  if (theObj.IsNull())
  {
    theDI << "NULL presentation\n";
    return;
  }

  theDI << (TheAISContext()->IsDisplayed (theObj) ? "Displayed"  : "Hidden   ")
        << (TheAISContext()->IsSelected  (theObj) ? " Selected" : "         ")
        << (theDetected.Contains (theObj)         ? " Detected" : "         ")
        << " Type: ";
  if (theObj->Type() == AIS_KOI_Datum)
  {
    // AIS_Datum
    if      (theObj->Signature() == 3) { theDI << " AIS_Trihedron"; }
    else if (theObj->Signature() == 2) { theDI << " AIS_Axis"; }
    else if (theObj->Signature() == 6) { theDI << " AIS_Circle"; }
    else if (theObj->Signature() == 5) { theDI << " AIS_Line"; }
    else if (theObj->Signature() == 7) { theDI << " AIS_Plane"; }
    else if (theObj->Signature() == 1) { theDI << " AIS_Point"; }
    else if (theObj->Signature() == 4) { theDI << " AIS_PlaneTrihedron"; }
  }
  // AIS_Shape
  else if (theObj->Type()      == AIS_KOI_Shape
        && theObj->Signature() == 0)
  {
    theDI << " AIS_Shape";
  }
  else if (theObj->Type() == AIS_KOI_Relation)
  {
    // PrsDim_Dimention and AIS_Relation
    Handle(PrsDim_Relation) aRelation = Handle(PrsDim_Relation)::DownCast (theObj);
    switch (aRelation->KindOfDimension())
    {
      case PrsDim_KOD_PLANEANGLE:     theDI << " PrsDim_AngleDimension"; break;
      case PrsDim_KOD_LENGTH:         theDI << " PrsDim_Chamf2/3dDimension/PrsDim_LengthDimension"; break;
      case PrsDim_KOD_DIAMETER:       theDI << " PrsDim_DiameterDimension"; break;
      case PrsDim_KOD_ELLIPSERADIUS:  theDI << " PrsDim_EllipseRadiusDimension"; break;
      //case PrsDim_KOD_FILLETRADIUS:   theDI << " PrsDim_FilletRadiusDimension "; break;
      case PrsDim_KOD_OFFSET:         theDI << " PrsDim_OffsetDimension"; break;
      case PrsDim_KOD_RADIUS:         theDI << " PrsDim_RadiusDimension"; break;
      default:                     theDI << " UNKNOWN dimension"; break;
    }
  }
  else
  {
    theDI << " UserPrs";
  }
  theDI << " (" << theObj->DynamicType()->Name() << ")";
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
    Message::SendFail ("Error: No opened viewer!");
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
    Handle(StdSelect_ViewerSelector3d) aSelector = aCtx->MainSelector();

    SelectMgr_SelectingVolumeManager aMgr = aSelector->GetManager();
    for (Standard_Integer aPickIter = 1; aPickIter <= aSelector->NbPicked(); ++aPickIter)
    {
      const SelectMgr_SortCriterion&         aPickData = aSelector->PickedData (aPickIter);
      const Handle(Select3D_SensitiveEntity)& anEntity = aSelector->PickedEntity (aPickIter);
      const Handle(SelectMgr_EntityOwner)& anOwner = anEntity->OwnerId();
      Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());

      TCollection_AsciiString aName;
      GetMapOfAIS().Find1 (anObj, aName);
      aName.LeftJustify (20, ' ');
      char anInfoStr[512];
      if (aPickData.Normal.SquareModulus() > ShortRealEpsilon())
      {
        Sprintf (anInfoStr,
                 " Depth: %g Distance: %g Point: %g %g %g Normal: %g %g %g",
                 aPickData.Depth,
                 aPickData.MinDist,
                 aPickData.Point.X(), aPickData.Point.Y(), aPickData.Point.Z(),
                 aPickData.Normal.x(), aPickData.Normal.y(), aPickData.Normal.z());
      }
      else
      {
        Sprintf (anInfoStr,
                 " Depth: %g Distance: %g Point: %g %g %g",
                 aPickData.Depth,
                 aPickData.MinDist,
                 aPickData.Point.X(), aPickData.Point.Y(), aPickData.Point.Z());
      }
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
  for (aCtx->InitDetected(); aCtx->MoreDetected(); aCtx->NextDetected())
  {
    aDetected.Add (Handle(AIS_InteractiveObject)::DownCast (aCtx->DetectedCurrentOwner()->Selectable()));
  }

  const Standard_Boolean toShowAll = (theArgNb >= 2 && *theArgVec[1] == '*');
  if (theArgNb >= 2
   && !toShowAll)
  {
    for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
    {
      const TCollection_AsciiString anObjName = theArgVec[anArgIter];
      Handle(AIS_InteractiveObject) anObj;
      if (!GetMapOfAIS().Find2 (anObjName, anObj))
      {
        theDI << anObjName << " doesn't exist!\n";
        continue;
      }

      TCollection_AsciiString aName = anObjName;
      aName.LeftJustify (20, ' ');
      theDI << "  " << aName << " ";
      objInfo (aDetected, anObj, theDI);
      theDI << "\n";
    }
    return 0;
  }

  if (aCtx->NbSelected() > 0 && !toShowAll)
  {
    NCollection_DataMap<Handle(SelectMgr_EntityOwner), TopoDS_Shape> anOwnerShapeMap;
    for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
    {
      const Handle(SelectMgr_EntityOwner) anOwner = aCtx->SelectedOwner();
      const Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
      // handle whole object selection
      if (anOwner == anObj->GlobalSelOwner())
      {
        TCollection_AsciiString aName;
        GetMapOfAIS().Find1 (anObj, aName);
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
    if (anObjIter.Key1().IsNull())
    {
      continue;
    }

    TCollection_AsciiString aName = anObjIter.Key2();
    aName.LeftJustify (20, ' ');
    theDI << "  " << aName << " ";
    objInfo (aDetected, anObjIter.Key1(), theDI);
    theDI << "\n";
  }
  printLocalSelectionInfo (aCtx, theDI);
  return 0;
}

//=======================================================================
//function : PickShape
//purpose  : First Activate the rightmode + Put Filters to be able to
//           pick objets that are of type <TheType>...
//=======================================================================

TopoDS_Shape ViewerTest::PickShape (const TopAbs_ShapeEnum theShapeType,
                                    const Standard_Integer theMaxPick)
{
  Handle(TopTools_HArray1OfShape) aResArray = new TopTools_HArray1OfShape (1, 1);
  PickShapes (theShapeType, aResArray, theMaxPick);
  return aResArray->First();
}

//=======================================================================
//function : PickShapes
//purpose  :
//=======================================================================
Standard_Boolean ViewerTest::PickShapes (const TopAbs_ShapeEnum theShapeType,
                                         Handle(TopTools_HArray1OfShape)& theResArray,
                                         const Standard_Integer theMaxPick)
{
  const Standard_Integer aNbToReach = theResArray->Length();
  if (aNbToReach > 1)
  {
    Message::SendWarning ("WARNING : Pick with Shift+ MB1 for Selection of more than 1 object");
  }

  // step 1: prepare the data
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return Standard_False;
  }

  aCtx->RemoveFilters();
  AIS_ListOfInteractive aDispObjects;
  aCtx->DisplayedObjects (aDispObjects);
  if (theShapeType == TopAbs_SHAPE)
  {
    aCtx->AddFilter (new AIS_TypeFilter (AIS_KOI_Shape));
  }
  else
  {
    aCtx->AddFilter (new StdSelect_ShapeTypeFilter (theShapeType));
  }

  const Standard_Integer aSelMode = AIS_Shape::SelectionMode (theShapeType);
  for (AIS_ListOfInteractive::Iterator anObjIter (aDispObjects); anObjIter.More(); anObjIter.Next())
  {
    if (Handle(AIS_Shape) aShapePrs = Handle(AIS_Shape)::DownCast (anObjIter.Value()))
    {
      aCtx->SetSelectionModeActive (aShapePrs, aSelMode, true, AIS_SelectionModesConcurrency_Single);
    }
  }

  // step 2 : wait for the selection...
  Standard_Integer aNbPickGood = 0, aNbPickFail = 0;
  Standard_Integer argccc = 5;
  const char *bufff[] = { "A", "B", "C", "D", "E" };
  const char **argvvv = (const char** )bufff;
  for (; aNbPickGood < aNbToReach && aNbPickFail <= theMaxPick; )
  {
    while (ViewerMainLoop (argccc, argvvv)) {}
    Standard_Integer aNbStored = aCtx->NbSelected();
    if (aNbStored != aNbPickGood)
    {
      aNbPickGood = aNbStored;
    }
    else
    {
      ++aNbPickFail;
    }
    Message::SendInfo() << "NbPicked =  " << aNbPickGood << " |  Nb Pick Fail :" << aNbPickFail;
  }

  // step3 get result.
  if (aNbPickFail >= aNbToReach)
  {
    return Standard_False;
  }

  Standard_Integer anIndex = theResArray->Lower();
  for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected(), ++anIndex)
  {
    if (aCtx->HasSelectedShape())
    {
      theResArray->SetValue (anIndex, aCtx->SelectedShape());
    }
    else
    {
      Handle(AIS_InteractiveObject) IO = aCtx->SelectedInteractive();
      theResArray->SetValue (anIndex, Handle(AIS_Shape)::DownCast (IO)->Shape());
    }
  }

  aCtx->RemoveFilters();
  if (theShapeType != TopAbs_SHAPE)
  {
    for (AIS_ListOfInteractive::Iterator anObjIter (aDispObjects); anObjIter.More(); anObjIter.Next())
    {
      if (Handle(AIS_Shape) aShapePrs = Handle(AIS_Shape)::DownCast (anObjIter.Value()))
      {
        aCtx->SetSelectionModeActive (aShapePrs, aSelMode, true, AIS_SelectionModesConcurrency_Single);
      }
    }
  }
  return Standard_True;
}

//=======================================================================
//function : VPickShape
//purpose  :
//=======================================================================
static int VPickShape( Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  TopAbs_ShapeEnum aShapeType = TopAbs_SHAPE;
  if (argc != 1)
  {
    TCollection_AsciiString aShapeArg (argv[1]);
    aShapeArg.LowerCase();
    aShapeType = TopAbs_COMPOUND;
    if      (aShapeArg == "v"
          || aShapeArg == "vertex") aShapeType = TopAbs_VERTEX;
    else if (aShapeArg == "e"
          || aShapeArg == "edge")   aShapeType = TopAbs_EDGE;
    else if (aShapeArg == "w"
          || aShapeArg == "wire")   aShapeType = TopAbs_WIRE;
    else if (aShapeArg == "f"
          || aShapeArg == "face")   aShapeType = TopAbs_FACE;
    else if (aShapeArg == "shape")  aShapeType = TopAbs_SHAPE;
    else if (aShapeArg == "shell")  aShapeType = TopAbs_SHELL;
    else if (aShapeArg == "solid")  aShapeType = TopAbs_SOLID;
    else
    {
      Message::SendFail() << "Syntax error at '" << argv[1] << "'";
      return 1;
    }
  }

  static Standard_Integer THE_NB_SHAPES_OF_TYPE[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  static const TCollection_AsciiString THE_NAME_TYPE[8] = {"COMPS","SOL","SHE","F","W","E","V","SHAP"};

  const Standard_Integer aNbToPick = argc > 2 ? argc - 2 : 1;
  if (aNbToPick == 1)
  {
    TopoDS_Shape aPickedShape = ViewerTest::PickShape (aShapeType);
    if (aPickedShape.IsNull())
    {
      return 1;
    }

    TCollection_AsciiString aName;
    if (argc > 2)
    {
      aName = argv[2];
    }
    else
    {
      const int aShapeIndex = ++THE_NB_SHAPES_OF_TYPE[Standard_Integer(aShapeType)];
      aName = TCollection_AsciiString ("Picked_") + THE_NAME_TYPE[Standard_Integer(aShapeType)] + "_" + aShapeIndex;
    }

    DBRep::Set (aName.ToCString(), aPickedShape);
    Handle(AIS_Shape) aShapePrs = new AIS_Shape (aPickedShape);
    ViewerTest::Display (aName, aShapePrs, false, true);
    di << "Name of picked shape: " << aName <<"\n";
  }
  else
  {
    TCollection_AsciiString aName (argv[2]);
    aName.LowerCase();
    const Standard_Boolean isAutoNaming = aName == ".";
    Handle(TopTools_HArray1OfShape) aPickedArray = new TopTools_HArray1OfShape (1, aNbToPick);
    if (ViewerTest::PickShapes (aShapeType, aPickedArray))
    {
      for (Standard_Integer aPickedIter = aPickedArray->Lower(); aPickedIter <= aPickedArray->Upper(); ++aPickedIter)
      {
        TopoDS_Shape aPickedShape = aPickedArray->Value (aPickedIter);
        aName.Clear();
        if (!aPickedShape.IsNull()
         && isAutoNaming)
        {
          const int aShapeIndex = ++THE_NB_SHAPES_OF_TYPE[Standard_Integer(aShapeType)];
          aName = TCollection_AsciiString ("Picked_") + THE_NAME_TYPE[Standard_Integer(aShapeType)] + "_" + aShapeIndex;
        }
        else
        {
          aName = argv[1 + aPickedIter];
        }

        DBRep::Set (aName.ToCString(), aPickedShape);
        Handle(AIS_Shape) aShapePrs = new AIS_Shape (aPickedShape);
        di << "Display of picked shape #" << aPickedIter << " - name: " << aName <<"\n";
        ViewerTest::Display (aName, aShapePrs, false, true);
      }
    }
  }
  TheAISContext()->UpdateCurrentViewer();
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
    Message::SendFail ("Error: AIS context is not available.");
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
    else if (anArg == "-contextfilter" && anArgIter + 1 < theArgc)
    {
      TCollection_AsciiString aVal (theArgv[++anArgIter]);
      aVal.LowerCase();
      if (aVal == "and")
      {
        aContext->SetFilterType (SelectMgr_FilterType_AND);
      }
      else if (aVal == "or")
      {
        aContext->SetFilterType (SelectMgr_FilterType_OR);
      }
      else
      {
        Message::SendFail() << "Syntax error: wrong command attribute value '" << aVal << "'";
        return 1;
      }
    }
    else if (anArg == "-type"
          && anArgIter + 1 < theArgc)
    {
      TCollection_AsciiString aVal (theArgv[++anArgIter]);
      TopAbs_ShapeEnum aShapeType = TopAbs_COMPOUND;
      if (!TopAbs::ShapeTypeFromString (aVal.ToCString(), aShapeType))
      {
        Message::SendFail() << "Syntax error: wrong command attribute value '" << aVal << "'";
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
    else if (anArg == "-secondtype"
          && anArgIter + 1 < theArgc)
    {
      TCollection_AsciiString aVal (theArgv[++anArgIter]);
      TopAbs_ShapeEnum aShapeType = TopAbs_COMPOUND;
      if (!TopAbs::ShapeTypeFromString (aVal.ToCString(), aShapeType))
      {
        Message::SendFail() << "Syntax error: wrong command attribute value '" << aVal << "'";
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
      Message::SendFail() << "Syntax error: unknown argument '" << theArgv[anArgIter] << "'";
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
      PrsDim_KindOfDimension KOD = Handle(PrsDim_Relation)::DownCast (curio)->KindOfDimension();
      if ((dimension_status==0 && KOD == PrsDim_KOD_NONE)||
	  (dimension_status==1 && KOD != PrsDim_KOD_NONE))
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
      PrsDim_KindOfDimension KOD = Handle(PrsDim_Relation)::DownCast (curio)->KindOfDimension();
      if ((dimension_status==0 && KOD == PrsDim_KOD_NONE)||
	  (dimension_status==1 && KOD != PrsDim_KOD_NONE))
	TheAISContext()->Display(curio,Standard_False);
    }

  }

  TheAISContext()->UpdateCurrentViewer();
  return 0;
}

static Standard_Integer vr(Draw_Interpretor& , Standard_Integer , const char** a)
{
  std::ifstream s(a[1]);
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
    Message::SendFail ("Error: No active viewer!");
    return 1;
  }

  ViewerTest_CmdParser aCmd;

  aCmd.SetDescription ("Adjusts parameters of material BSDF:");

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

  // find object
  TCollection_AsciiString aName (aCmd.Arg (ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY, 0).c_str());
  Handle(AIS_InteractiveObject) anIObj;
  if (!GetMapOfAIS().Find2 (aName, anIObj))
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

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
    Message::SendFail ("Syntax error: wrong number of arguments.");
    return 1;
  }

  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    ViewerTest::ViewerInit();
    aCtx = ViewerTest::GetAISContext();
  }

  // Parse input arguments
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    const TCollection_AsciiString aName = theArgVec[anArgIter];
    Handle(AIS_InteractiveObject) aShape;
    if (!GetMapOfAIS().Find2 (aName, aShape))
    {
      TopoDS_Shape aDrawShape = DBRep::GetExisting (aName);
      if (!aDrawShape.IsNull())
      {
        aShape = new AIS_Shape (aDrawShape);
        GetMapOfAIS().Bind (aShape, aName);
      }
    }
    if (aShape.IsNull())
    {
      Message::SendFail() << "Syntax error: presentation '" << aName << "' not found";
      return 1;
    }

    aCtx->Load (aShape, -1);
    aCtx->Activate (aShape, aShape->GlobalSelectionMode(), Standard_True);
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
      "\n\t\t:          [-redisplay] [-erased]"
      "\n\t\t:          name1 [name2] ... [name n]"
      "\n\t\t: Displays named objects."
      "\n\t\t: Option -local enables displaying of objects in local"
      "\n\t\t: selection context. Local selection context will be opened"
      "\n\t\t: if there is not any."
      "\n\t\t:  -noupdate    Suppresses viewer redraw call."
      "\n\t\t:  -mutable     Enables optimizations for mutable objects."
      "\n\t\t:  -neutral     Draws objects in main viewer."
      "\n\t\t:  -erased      Loads the object into context, but does not display it."
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
      "verase [-noupdate|-update] [-local] [name1] ...  [name n] [-noerror]"
      "\n\t\t: Erases selected or named objects."
      "\n\t\t: If there are no selected or named objects the whole viewer is erased."
      "\n\t\t: Option -local enables erasing of selected or named objects without"
      "\n\t\t: closing local selection context."
      "\n\t\t: Option -noerror prevents exception on non-existing objects.",
      __FILE__, VErase, group);

  theCommands.Add("vremove",
      "vremove [-noupdate|-update] [-context] [-all] [-noinfo] [name1] ...  [name n] [-noerror]"
      "or vremove [-context] -all to remove all objects"
      "\n\t\t: Removes selected or named objects."
      "\n\t\t  If -context is in arguments, the objects are not deleted"
      "\n\t\t  from the map of objects and names."
      "\n\t\t: Option -local enables removing of selected or named objects without"
      "\n\t\t: closing local selection context. Empty local selection context will be"
      "\n\t\t: closed."
      "\n\t\t: Option -noupdate suppresses viewer redraw call."
      "\n\t\t: Option -noinfo suppresses displaying the list of removed objects."
      "\n\t\t: Option -noerror prevents exception on non-existing objects.",
      __FILE__, VRemove, group);

  theCommands.Add("vdonly",
		  "vdonly [-noupdate|-update] [name1] ...  [name n]"
      "\n\t\t: Displays only selected or named objects",
		  __FILE__,VDonly2,group);

  theCommands.Add("vdisplayall",
      "vdisplayall"
      "\n\t\t: Displays all erased interactive objects (see vdir and vstate).",
      __FILE__, VDisplayAll, group);

  theCommands.Add("veraseall",
      "veraseall"
      "\n\t\t: Erases all objects displayed in the viewer.",
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
              "vdir [mask] [-list]"
      "\n\t\t: Lists all objects displayed in 3D viewer"
      "\n\t\t:    mask - name filter like prefix*"
      "\n\t\t:   -list - format list with new-line per name; OFF by default",
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
      "\n\t\t:       [-xrPose base|head|handLeft|handRight=base]"
      "\n\t\t:       [-tileSize Size=0]"
      "\n\t\t: Dumps content of the active view into image file",
		  __FILE__,VDump,group);

  theCommands.Add("vsub",      "vsub 0/1 (off/on) [obj]        : Subintensity(on/off) of selected objects",
		  __FILE__,VSubInt,group);

  theCommands.Add("vaspects",
              "vaspects [-noupdate|-update] [name1 [name2 [...]] | -defaults] [-subshapes subname1 [subname2 [...]]]"
      "\n\t\t:          [-visibility {0|1}]"
      "\n\t\t:          [-color {ColorName | R G B}] [-unsetColor]"
      "\n\t\t:          [-backfaceColor Color]"
      "\n\t\t:          [-material MatName] [-unsetMaterial]"
      "\n\t\t:          [-transparency Transp] [-unsetTransparency]"
      "\n\t\t:          [-width LineWidth] [-unsetWidth]"
      "\n\t\t:          [-lineType {solid|dash|dot|dotDash|0xHexPattern} [-stippleFactor factor]]"
      "\n\t\t:          [-unsetLineType]"
      "\n\t\t:          [-markerType {.|+|x|O|xcircle|pointcircle|ring1|ring2|ring3|ball|ImagePath}]"
      "\n\t\t:          [-unsetMarkerType]"
      "\n\t\t:          [-markerSize Scale] [-unsetMarkerSize]"
      "\n\t\t:          [-freeBoundary {0|1}]"
      "\n\t\t:          [-freeBoundaryWidth Width] [-unsetFreeBoundaryWidth]"
      "\n\t\t:          [-freeBoundaryColor {ColorName | R G B}] [-unsetFreeBoundaryColor]"
      "\n\t\t:          [-isoOnTriangulation 0|1]"
      "\n\t\t:          [-maxParamValue {value}]"
      "\n\t\t:          [-sensitivity {selection_mode} {value}]"
      "\n\t\t:          [-shadingModel {unlit|flat|gouraud|phong|pbr|pbr_facet}]"
      "\n\t\t:          [-unsetShadingModel]"
      "\n\t\t:          [-interior {solid|hatch|hidenline|point}] [-setHatch HatchStyle]"
      "\n\t\t:          [-unsetInterior]"
      "\n\t\t:          [-faceBoundaryDraw {0|1}] [-mostContinuity {c0|g1|c1|g2|c2|c3|cn}]"
      "\n\t\t:          [-faceBoundaryWidth LineWidth] [-faceBoundaryColor R G B] [-faceBoundaryType LineType]"
      "\n\t\t:          [-drawEdges {0|1}] [-edgeType LineType] [-edgeColor R G B] [-quadEdges {0|1}]"
      "\n\t\t:          [-drawSilhouette {0|1}]"
      "\n\t\t:          [-alphaMode {opaque|mask|blend|blendauto} [alphaCutOff=0.5]]"
      "\n\t\t:          [-dumpJson]"
      "\n\t\t:          [-dumpCompact {0|1}]"
      "\n\t\t:          [-dumpDepth depth]"
      "\n\t\t: Manage presentation properties of all, selected or named objects."
      "\n\t\t: When -subshapes is specified than following properties will be assigned to specified sub-shapes."
      "\n\t\t: When -defaults is specified than presentation properties will be"
      "\n\t\t: assigned to all objects that have not their own specified properties"
      "\n\t\t: and to all objects to be displayed in the future."
      "\n\t\t: If -defaults is used there should not be any objects' names nor -subshapes specifier."
      "\n\t\t: See also vlistcolors and vlistmaterials to list named colors and materials"
      "\n\t\t: accepted by arguments -material and -color",
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
      "\n\t\t: Alias for vaspects -unsetwidth [name].",
		  __FILE__,VAspects,group);

  theCommands.Add("vsetinteriorstyle",
    "vsetinteriorstyle [-noupdate|-update] [name] Style"
    "\n\t\t: Alias for vaspects -setInterior [name] Style.",
		  __FILE__,VAspects,group);

  theCommands.Add ("vsetedgetype",
    "vsetedgetype [name] [-type {solid, dash, dot}] [-color R G B] [-width value]"
    "\n\t\t: Alias for vaspects [name] -setEdgeType Type.",
      __FILE__, VAspects, group);

  theCommands.Add ("vunsetedgetype",
    "vunsetedgetype [name]"
    "\n\t\t: Alias for vaspects [name] -unsetEdgeType.",
      __FILE__, VAspects, group);

  theCommands.Add ("vshowfaceboundary",
    "vshowfaceboundary [name]"
    "\n\t\t: Alias for vaspects [name] -setFaceBoundaryDraw on",
      __FILE__, VAspects, group);

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

  theCommands.Add("vstate",
      "vstate [-entities] [-hasSelected] [name1] ... [nameN]"
      "\n\t\t: Reports show/hidden state for selected or named objects"
      "\n\t\t:   -entities - print low-level information about detected entities"
      "\n\t\t:   -hasSelected - prints 1 if context has selected shape and 0 otherwise",
		  __FILE__,VState,group);

  theCommands.Add("vpickshapes",
                  "vpickshape subtype(VERTEX,EDGE,WIRE,FACE,SHELL,SOLID) [name1 or .] [name2 or .] [name n or .]"
                  "\n\t\t: Hold Ctrl and pick object by clicking Left mouse button."
                  "\n\t\t: Hold also Shift for multiple selection.",
                  __FILE__, VPickShape, group);

  theCommands.Add("vtypes",
		  "vtypes : list of known types and signatures in AIS - To be Used in vpickobject command for selection with filters",
		  VIOTypes,group);

  theCommands.Add("vr",
      "vr filename"
      "\n\t\t: Reads shape from BREP-format file and displays it in the viewer. ",
		  __FILE__,vr, group);

  theCommands.Add("vselfilter",
    "vselfilter [-contextfilter {AND|OR}]"
    "\n         [-type {VERTEX|EDGE|WIRE|FACE|SHAPE|SHELL|SOLID}]"
    "\n         [-secondtype {VERTEX|EDGE|WIRE|FACE|SHAPE|SHELL|SOLID}]"
    "\n         [-clear]"
    "\nSets selection shape type filter in context or remove all filters."
    "\n    : Option -contextfilter : To define a selection filter for two or more types of entity,"
    "\n                              use value AND (OR by default)."
    "\n    : Option -type set type of selection filter. Filters are applyed with Or combination."
    "\n    : Option -clear remove all filters in context",
		  __FILE__,VSelFilter,group);

  theCommands.Add("vpickselected", "vpickselected [name]: extract selected shape.",
    __FILE__, VPickSelected, group);

  theCommands.Add ("vloadselection",
    "vloadselection [-context] [name1] ... [nameN] : allows to load selection"
    "\n\t\t: primitives for the shapes with names given without displaying them.",
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
#include <BRepOffsetAPI_DraftAngle.hxx>
#include <Precision.hxx>
#include <BRepAlgo.hxx>
#include <OSD_Environment.hxx>
#include <DrawTrSurf.hxx>

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
    std::cout <<"DONT_SWITCH_IS_VALID positionnee a :"<<checkValid.ToCString()<<"\n";
#endif
    if ( checkValid=="true" || checkValid=="TRUE" ) {
      ToCheck= Standard_False;
    }
  } else {
#ifdef OCCT_DEBUG
    std::cout <<"DONT_SWITCH_IS_VALID non positionne\n";
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
  TopoDS_Shape Solid  = DBRep::Get (argv[1]);
  TopoDS_Shape face   = DBRep::Get (argv[2]);
  TopoDS_Face Face    = TopoDS::Face(face);
  TopoDS_Shape Plane  = DBRep::Get (argv[3]);
  if (Plane.IsNull ()) {
    di << "TEST : Plane is NULL\n";
    return 1;
  }
  anAngle = Draw::Atof(argv[4]);
  anAngle = 2*M_PI * anAngle / 360.0;
  gp_Pln aPln;
  Handle( Geom_Surface )aSurf;
  PrsDim_KindOfSurface aSurfType;
  Standard_Real Offset;
  gp_Dir aDir;
  if(argc > 4) { // == 5
    rev = Draw::Atoi(argv[5]);
    Rev = (rev)? Standard_True : Standard_False;
  }

  TopoDS_Face face2 = TopoDS::Face(Plane);
  if(!PrsDim::GetPlaneFromFace(face2, aPln, aSurf, aSurfType, Offset))
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
    Handle(AIS_InteractiveObject) an_object;
    if (GetMapOfAIS().Find2(Name, an_object))
    {
      if (!an_object.IsNull())
      {
        Ctx->Remove (an_object, Standard_True);
      }
      GetMapOfAIS().UnBind2 (Name);
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
