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

#include <Standard_Macro.hxx>

#include <iostream>

#include <iomanip>

#include <fstream>

#include <ViewerTest.hxx>
#include <ViewerTest_CmdParser.hxx>
#include <ViewerTest_AutoUpdater.hxx>

#include <Draw.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Map.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <StdSelect_ShapeTypeFilter.hxx>
#include <AIS_ColoredShape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_Axis.hxx>
#include <AIS_SignatureFilter.hxx>
#include <NCollection_List.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Aspect_Window.hxx>
#include <Aspect_XRSession.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_Texture2D.hxx>
#include <Graphic3d_Texture3D.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_MediaTextureSet.hxx>
#include <Image_AlienPixMap.hxx>
#include <Message.hxx>
#include <OSD_File.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <PrsDim_Relation.hxx>
#include <Select3D_SensitiveWire.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <V3d_Viewer.hxx>

#include <cstdio>

#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>

extern int ViewerMainLoop(int argc, const char** argv);

#include <Quantity_Color.hxx>
#include <Quantity_NameOfColor.hxx>

#include <Graphic3d_NameOfMaterial.hxx>

#define DEFAULT_COLOR Quantity_NOC_GOLDENROD
#define DEFAULT_FREEBOUNDARY_COLOR Quantity_NOC_GREEN
#define DEFAULT_MATERIAL Graphic3d_NameOfMaterial_Brass

//=======================================================================
// function : GetColorFromName
// purpose  : get the Quantity_NameOfColor from a string
//=======================================================================

Quantity_NameOfColor ViewerTest::GetColorFromName(const char* const theName)
{
  Quantity_NameOfColor aColor = DEFAULT_COLOR;
  Quantity_Color::ColorFromName(theName, aColor);
  return aColor;
}

//=================================================================================================

int ViewerTest::ParseColor(const int                theArgNb,
                           const char* const* const theArgVec,
                           Quantity_ColorRGBA&      theColor)
{
  return Draw::ParseColor(theArgNb, theArgVec, theColor);
}

//=================================================================================================

int ViewerTest::ParseColor(const int                theArgNb,
                           const char* const* const theArgVec,
                           Quantity_Color&          theColor)
{
  return Draw::ParseColor(theArgNb, theArgVec, theColor);
}

//=================================================================================================

bool ViewerTest::ParseOnOff(const char* theArg, bool& theIsOn)
{
  return Draw::ParseOnOff(theArg, theIsOn);
}

//=================================================================================================

void ViewerTest::GetSelectedShapes(NCollection_List<TopoDS_Shape>& theSelectedShapes)
{
  for (GetAISContext()->InitSelected(); GetAISContext()->MoreSelected();
       GetAISContext()->NextSelected())
  {
    TopoDS_Shape aShape = GetAISContext()->SelectedShape();
    if (!aShape.IsNull())
    {
      theSelectedShapes.Append(aShape);
    }
  }
}

//=================================================================================================

bool ViewerTest::ParseLineType(const char* theArg, Aspect_TypeOfLine& theType, uint16_t& thePattern)
{
  TCollection_AsciiString aTypeStr(theArg);
  aTypeStr.LowerCase();
  if (aTypeStr == "empty" || aTypeStr == "-1")
  {
    theType    = Aspect_TOL_EMPTY;
    thePattern = Graphic3d_Aspects::DefaultLinePatternForType(theType);
  }
  else if (aTypeStr == "solid" || aTypeStr == "0")
  {
    theType    = Aspect_TOL_SOLID;
    thePattern = Graphic3d_Aspects::DefaultLinePatternForType(theType);
  }
  else if (aTypeStr == "dot" || aTypeStr == "2")
  {
    theType    = Aspect_TOL_DOT;
    thePattern = Graphic3d_Aspects::DefaultLinePatternForType(theType);
  }
  else if (aTypeStr == "dash" || aTypeStr == "1")
  {
    theType    = Aspect_TOL_DASH;
    thePattern = Graphic3d_Aspects::DefaultLinePatternForType(theType);
  }
  else if (aTypeStr == "dotdash" || aTypeStr == "3")
  {
    theType    = Aspect_TOL_DOTDASH;
    thePattern = Graphic3d_Aspects::DefaultLinePatternForType(theType);
  }
  else
  {
    if (aTypeStr.StartsWith("0x"))
    {
      aTypeStr = aTypeStr.SubString(3, aTypeStr.Length());
    }

    if (aTypeStr.Length() != 4 || !std::isxdigit(static_cast<unsigned char>(aTypeStr.Value(1)))
        || !std::isxdigit(static_cast<unsigned char>(aTypeStr.Value(2)))
        || !std::isxdigit(static_cast<unsigned char>(aTypeStr.Value(3)))
        || !std::isxdigit(static_cast<unsigned char>(aTypeStr.Value(4))))
    {
      return false;
    }

    std::stringstream aStream;
    aStream << std::setbase(16) << aTypeStr.ToCString();
    if (aStream.fail())
    {
      return false;
    }

    int aNumber = -1;
    aStream >> aNumber;
    if (aStream.fail())
    {
      return false;
    }

    thePattern = (uint16_t)aNumber;
    theType    = Graphic3d_Aspects::DefaultLineTypeForPattern(thePattern);
  }
  return true;
}

//=================================================================================================

bool ViewerTest::ParseMarkerType(const char*                theArg,
                                 Aspect_TypeOfMarker&       theType,
                                 occ::handle<Image_PixMap>& theImage)
{
  theImage.Nullify();
  TCollection_AsciiString aTypeStr(theArg);
  aTypeStr.LowerCase();
  if (aTypeStr == "empty")
  {
    theType = Aspect_TOM_EMPTY;
  }
  else if (aTypeStr == "point" || aTypeStr == "dot" || aTypeStr == ".")
  {
    theType = Aspect_TOM_POINT;
  }
  else if (aTypeStr == "plus" || aTypeStr == "+")
  {
    theType = Aspect_TOM_PLUS;
  }
  else if (aTypeStr == "star" || aTypeStr == "*")
  {
    theType = Aspect_TOM_STAR;
  }
  else if (aTypeStr == "cross" || aTypeStr == "x")
  {
    theType = Aspect_TOM_X;
  }
  else if (aTypeStr == "circle" || aTypeStr == "o")
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
  else if (aTypeStr == "crossincircle" || aTypeStr == "xcircle")
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
  else if (aTypeStr == "ring" || aTypeStr == "ring3")
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
      return false;
    }
    theType = (Aspect_TypeOfMarker)aTypeInt;
  }
  else
  {
    theType                                = Aspect_TOM_USERDEFINED;
    occ::handle<Image_AlienPixMap> anImage = new Image_AlienPixMap();
    if (!anImage->Load(theArg))
    {
      return false;
    }
    if (anImage->Format() == Image_Format_Gray)
    {
      anImage->SetFormat(Image_Format_Alpha);
    }
    else if (anImage->Format() == Image_Format_GrayF)
    {
      anImage->SetFormat(Image_Format_AlphaF);
    }
    theImage = anImage;
  }
  return true;
}

//=================================================================================================

bool ViewerTest::ParseShadingModel(const char* theArg, Graphic3d_TypeOfShadingModel& theModel)
{
  TCollection_AsciiString aTypeStr(theArg);
  aTypeStr.LowerCase();
  if (aTypeStr == "unlit" || aTypeStr == "color" || aTypeStr == "none")
  {
    theModel = Graphic3d_TypeOfShadingModel_Unlit;
  }
  else if (aTypeStr == "flat" || aTypeStr == "facet")
  {
    theModel = Graphic3d_TypeOfShadingModel_PhongFacet;
  }
  else if (aTypeStr == "gouraud" || aTypeStr == "vertex" || aTypeStr == "vert")
  {
    theModel = Graphic3d_TypeOfShadingModel_Gouraud;
  }
  else if (aTypeStr == "phong" || aTypeStr == "fragment" || aTypeStr == "frag"
           || aTypeStr == "pixel")
  {
    theModel = Graphic3d_TypeOfShadingModel_Phong;
  }
  else if (aTypeStr == "pbr")
  {
    theModel = Graphic3d_TypeOfShadingModel_Pbr;
  }
  else if (aTypeStr == "pbr_facet")
  {
    theModel = Graphic3d_TypeOfShadingModel_PbrFacet;
  }
  else if (aTypeStr == "default" || aTypeStr == "def")
  {
    theModel = Graphic3d_TypeOfShadingModel_DEFAULT;
  }
  else if (aTypeStr.IsIntegerValue())
  {
    const int aTypeInt = aTypeStr.IntegerValue();
    if (aTypeInt <= Graphic3d_TypeOfShadingModel_DEFAULT
        || aTypeInt >= Graphic3d_TypeOfShadingModel_NB)
    {
      return false;
    }
    theModel = (Graphic3d_TypeOfShadingModel)aTypeInt;
  }
  else
  {
    return false;
  }
  return true;
}

//=================================================================================================

bool ViewerTest::parseZLayer(const char*         theArg,
                             bool                theToAllowInteger,
                             Graphic3d_ZLayerId& theLayer)
{
  TCollection_AsciiString aName(theArg);
  aName.LowerCase();
  if (aName == "default" || aName == "def")
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
  else if (aName == "overlay" || aName == "toposd")
  {
    theLayer = Graphic3d_ZLayerId_TopOSD;
  }
  else if (aName == "underlay" || aName == "botosd")
  {
    theLayer = Graphic3d_ZLayerId_BotOSD;
  }
  else if (aName == "undefined")
  {
    theLayer = Graphic3d_ZLayerId_UNKNOWN;
  }
  else if (!GetAISContext().IsNull())
  {
    const occ::handle<V3d_Viewer>& aViewer = ViewerTest::GetAISContext()->CurrentViewer();
    NCollection_Sequence<int>      aLayers;
    aViewer->GetAllZLayers(aLayers);
    for (NCollection_Sequence<int>::Iterator aLayeriter(aLayers); aLayeriter.More();
         aLayeriter.Next())
    {
      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings(aLayeriter.Value());
      if (TCollection_AsciiString::IsSameString(aSettings.Name(), aName, false))
      {
        theLayer = aLayeriter.Value();
        return true;
      }
    }

    if (!theToAllowInteger || !aName.IsIntegerValue())
    {
      return false;
    }
    Graphic3d_ZLayerId aLayer = aName.IntegerValue();
    if (aLayer == Graphic3d_ZLayerId_UNKNOWN
        || std::find(aLayers.begin(), aLayers.end(), aLayer) != aLayers.end())
    {
      theLayer = aLayer;
      return true;
    }
    return false;
  }
  return true;
}

//=================================================================================================

static const char** GetTypeNames()
{
  static const char*  names[14]  = {"Point",
                                    "Axis",
                                    "Trihedron",
                                    "PlaneTrihedron",
                                    "Line",
                                    "Circle",
                                    "Plane",
                                    "Shape",
                                    "ConnectedShape",
                                    "MultiConn.Shape",
                                    "ConnectedInter.",
                                    "MultiConn.",
                                    "Constraint",
                                    "Dimension"};
  static const char** ThePointer = names;
  return ThePointer;
}

//=================================================================================================

static void GetTypeAndSignfromString(const char*            theName,
                                     AIS_KindOfInteractive& theType,
                                     int&                   theSign)
{
  const char** aFullNames = GetTypeNames();
  int          anIndex    = -1;
  for (int i = 0; i <= 13 && anIndex == -1; ++i)
  {
    if (strcasecmp(theName, aFullNames[i]) == 0)
    {
      anIndex = i;
    }
  }

  if (anIndex == -1)
  {
    theType = AIS_KindOfInteractive_None;
    theSign = -1;
    return;
  }

  if (anIndex <= 6)
  {
    theType = AIS_KindOfInteractive_Datum;
    theSign = anIndex + 1;
  }
  else if (anIndex <= 9)
  {
    theType = AIS_KindOfInteractive_Shape;
    theSign = anIndex - 7;
  }
  else if (anIndex <= 11)
  {
    theType = AIS_KindOfInteractive_Object;
    theSign = anIndex - 10;
  }
  else
  {
    theType = AIS_KindOfInteractive_Relation;
    theSign = anIndex - 12;
  }
}

#include <cstring>
#include <Draw_Appli.hxx>
#include <DBRep.hxx>

#include <V3d_View.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <AIS_DisplayMode.hxx>
#include <NCollection_DoubleMap.hxx>
#include <ViewerTest_EventManager.hxx>

#include <BRep_Builder.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <BRep_Tool.hxx>

#include <Draw_Window.hxx>
class AIS_InteractiveObject;

//==============================================================================
//  VIEWER OBJECT MANAGEMENT GLOBAL VARIABLES
//==============================================================================
Standard_EXPORT NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>&
                GetMapOfAIS()
{
  static NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString> TheMap;
  return TheMap;
}

//=================================================================================================

bool ViewerTest::Display(const TCollection_AsciiString&            theName,
                         const occ::handle<AIS_InteractiveObject>& theObject,
                         const bool                                theToUpdate,
                         const bool                                theReplaceIfExists)
{
  NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>& aMap =
    GetMapOfAIS();
  occ::handle<AIS_InteractiveContext> aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: AIS context is not available.");
    return false;
  }

  if (aMap.IsBound2(theName))
  {
    if (!theReplaceIfExists)
    {
      Message::SendFail()
        << "Error: other interactive object has been already registered with name: " << theName
        << ".\n"
        << "Please use another name.";
      return false;
    }

    if (occ::handle<AIS_InteractiveObject> anOldObj = aMap.Find2(theName))
    {
      aCtx->Remove(anOldObj, theObject.IsNull() && theToUpdate);
    }
    aMap.UnBind2(theName);
  }

  if (theObject.IsNull())
  {
    // object with specified name has been already unbound
    return true;
  }

  // unbind AIS object if it was bound with another name
  aMap.UnBind1(theObject);

  // can be registered without rebinding
  aMap.Bind(theObject, theName);
  aCtx->Display(theObject, theToUpdate);
  return true;
}

//! Alias for ViewerTest::Display(), compatibility with old code.
Standard_EXPORT bool VDisplayAISObject(const TCollection_AsciiString&            theName,
                                       const occ::handle<AIS_InteractiveObject>& theObject,
                                       bool theReplaceIfExists = true)
{
  return ViewerTest::Display(theName, theObject, true, theReplaceIfExists);
}

static NCollection_List<occ::handle<ViewerTest_EventManager>> theEventMgrs;

static occ::handle<V3d_View>& a3DView()
{
  static occ::handle<V3d_View> Viou;
  return Viou;
}

Standard_EXPORT occ::handle<AIS_InteractiveContext>& TheAISContext()
{
  static occ::handle<AIS_InteractiveContext> aContext;
  return aContext;
}

const occ::handle<V3d_View>& ViewerTest::CurrentView()
{
  return a3DView();
}

void ViewerTest::CurrentView(const occ::handle<V3d_View>& V)
{
  a3DView() = V;
}

const occ::handle<AIS_InteractiveContext>& ViewerTest::GetAISContext()
{
  return TheAISContext();
}

void ViewerTest::SetAISContext(const occ::handle<AIS_InteractiveContext>& aCtx)
{
  TheAISContext() = aCtx;
  ViewerTest::ResetEventManager();
}

occ::handle<V3d_Viewer> ViewerTest::GetViewerFromContext()
{
  return !TheAISContext().IsNull() ? TheAISContext()->CurrentViewer() : occ::handle<V3d_Viewer>();
}

occ::handle<V3d_Viewer> ViewerTest::GetCollectorFromContext()
{
  return !TheAISContext().IsNull() ? TheAISContext()->CurrentViewer() : occ::handle<V3d_Viewer>();
}

void ViewerTest::SetEventManager(const occ::handle<ViewerTest_EventManager>& EM)
{
  theEventMgrs.Prepend(EM);
}

void ViewerTest::UnsetEventManager()
{
  theEventMgrs.RemoveFirst();
}

void ViewerTest::ResetEventManager()
{
  theEventMgrs.Clear();
  theEventMgrs.Prepend(
    new ViewerTest_EventManager(ViewerTest::CurrentView(), ViewerTest::GetAISContext()));
}

occ::handle<ViewerTest_EventManager> ViewerTest::CurrentEventManager()
{
  return !theEventMgrs.IsEmpty() ? theEventMgrs.First() : occ::handle<ViewerTest_EventManager>();
}

//=================================================================================================

static bool getCtxAndView(occ::handle<AIS_InteractiveContext>& theCtx,
                          occ::handle<V3d_View>&               theView)
{
  theCtx  = ViewerTest::GetAISContext();
  theView = ViewerTest::CurrentView();
  if (theCtx.IsNull() || theView.IsNull())
  {
    Message::SendFail("Error: cannot find an active view!");
    return false;
  }
  return true;
}

//==============================================================================
// function : Clear
// purpose  : Remove all the object from the viewer
//==============================================================================
void ViewerTest::Clear()
{
  if (a3DView().IsNull())
  {
    return;
  }

  NCollection_Sequence<occ::handle<AIS_InteractiveObject>> aListRemoved;
  for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>::Iterator
         anObjIter(GetMapOfAIS());
       anObjIter.More();
       anObjIter.Next())
  {
    const occ::handle<AIS_InteractiveObject>& anObj = anObjIter.Key1();
    if (anObj->GetContext() != TheAISContext())
    {
      continue;
    }

    Message::SendInfo() << "Remove " << anObjIter.Key2();
    TheAISContext()->Remove(anObj, false);
    aListRemoved.Append(anObj);
  }

  TheAISContext()->RebuildSelectionStructs();
  TheAISContext()->UpdateCurrentViewer();
  if (aListRemoved.Size() == GetMapOfAIS().Extent())
  {
    GetMapOfAIS().Clear();
  }
  else
  {
    for (NCollection_Sequence<occ::handle<AIS_InteractiveObject>>::Iterator anObjIter(aListRemoved);
         anObjIter.More();
         anObjIter.Next())
    {
      GetMapOfAIS().UnBind1(anObjIter.Value());
    }
  }
}

//==============================================================================
// function : CopyIsoAspect
// purpose  : Returns copy Prs3d_IsoAspect with new number of isolines.
//==============================================================================
static occ::handle<Prs3d_IsoAspect> CopyIsoAspect(const occ::handle<Prs3d_IsoAspect>& theIsoAspect,
                                                  const int                           theNbIsos)
{
  Quantity_Color    aColor = theIsoAspect->Aspect()->Color();
  Aspect_TypeOfLine aType  = theIsoAspect->Aspect()->Type();
  double            aWidth = theIsoAspect->Aspect()->Width();

  occ::handle<Prs3d_IsoAspect> aResult = new Prs3d_IsoAspect(aColor, aType, aWidth, theNbIsos);

  return aResult;
}

//==============================================================================
// function : visos
// purpose  : Returns or sets the number of U- and V- isos and isIsoOnPlane flag
// Draw arg : [name1 ...] [nbUIsos nbVIsos IsoOnPlane(0|1)]
//==============================================================================
static int visos(Draw_Interpretor& di, int argc, const char** argv)
{
  if (TheAISContext().IsNull())
  {
    di << argv[0] << " Call 'vinit' before!\n";
    return 1;
  }

  if (argc <= 1)
  {
    di << "Current number of isos : " << TheAISContext()->IsoNumber(AIS_TOI_IsoU) << " "
       << TheAISContext()->IsoNumber(AIS_TOI_IsoV) << "\n";
    di << "IsoOnPlane mode is " << (TheAISContext()->IsoOnPlane() ? "ON" : "OFF") << "\n";
    di << "IsoOnTriangulation mode is " << (TheAISContext()->IsoOnTriangulation() ? "ON" : "OFF")
       << "\n";
    return 0;
  }

  int  aLastInd  = argc - 1;
  bool isChanged = false;
  int  aNbUIsos  = 0;
  int  aNbVIsos  = 0;

  if (aLastInd >= 3)
  {
    bool isIsoOnPlane = false;

    if (strcmp(argv[aLastInd], "1") == 0)
    {
      isIsoOnPlane = true;
      isChanged    = true;
    }
    else if (strcmp(argv[aLastInd], "0") == 0)
    {
      isIsoOnPlane = false;
      isChanged    = true;
    }

    if (isChanged)
    {
      aNbVIsos = Draw::Atoi(argv[aLastInd - 1]);
      aNbUIsos = Draw::Atoi(argv[aLastInd - 2]);
      aLastInd -= 3;

      di << "New number of isos : " << aNbUIsos << " " << aNbVIsos << "\n";
      di << "New IsoOnPlane mode is " << (isIsoOnPlane ? "ON" : "OFF") << "\n";

      TheAISContext()->IsoOnPlane(isIsoOnPlane);

      if (aLastInd == 0)
      {
        // If there are no shapes provided set the default numbers.
        TheAISContext()->SetIsoNumber(aNbUIsos, AIS_TOI_IsoU);
        TheAISContext()->SetIsoNumber(aNbVIsos, AIS_TOI_IsoV);
      }
    }
  }

  int i;

  for (i = 1; i <= aLastInd; i++)
  {
    TCollection_AsciiString            name(argv[i]);
    occ::handle<AIS_InteractiveObject> aShape;
    GetMapOfAIS().Find2(name, aShape);
    if (aShape.IsNull())
    {
      Message::SendFail() << "Syntax error: object '" << name << "' is not found";
      return 1;
    }

    occ::handle<Prs3d_Drawer>    CurDrawer = aShape->Attributes();
    occ::handle<Prs3d_IsoAspect> aUIso     = CurDrawer->UIsoAspect();
    occ::handle<Prs3d_IsoAspect> aVIso     = CurDrawer->VIsoAspect();
    if (isChanged)
    {
      CurDrawer->SetUIsoAspect(CopyIsoAspect(aUIso, aNbUIsos));
      CurDrawer->SetVIsoAspect(CopyIsoAspect(aVIso, aNbVIsos));
      TheAISContext()->SetLocalAttributes(aShape, CurDrawer, false);
      TheAISContext()->Redisplay(aShape, false);
    }
    else
    {
      di << "Number of isos for " << argv[i] << " : " << aUIso->Number() << " " << aVIso->Number()
         << "\n";
    }
  }

  if (isChanged)
  {
    TheAISContext()->UpdateCurrentViewer();
  }

  return 0;
}

static int VDispSensi(Draw_Interpretor&, int theArgNb, const char**)
{
  if (theArgNb > 1)
  {
    Message::SendFail("Error: wrong syntax!");
    return 1;
  }

  occ::handle<AIS_InteractiveContext> aCtx;
  occ::handle<V3d_View>               aView;
  if (!getCtxAndView(aCtx, aView))
  {
    return 1;
  }

  aCtx->DisplayActiveSensitive(aView);
  return 0;
}

static int VClearSensi(Draw_Interpretor&, int theArgNb, const char**)
{
  if (theArgNb > 1)
  {
    Message::SendFail("Error: wrong syntax!");
    return 1;
  }

  occ::handle<AIS_InteractiveContext> aCtx;
  occ::handle<V3d_View>               aView;
  if (!getCtxAndView(aCtx, aView))
  {
    return 1;
  }
  aCtx->ClearActiveSensitive(aView);
  return 0;
}

//==============================================================================
// function : VDir
// purpose  : To list the displayed object with their attributes
//==============================================================================
static int VDir(Draw_Interpretor& theDI, int theNbArgs, const char** theArgVec)
{
  TCollection_AsciiString aMatch;
  bool                    toFormat = false;
  for (int anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anArgCase == "-list" || anArgCase == "-format")
    {
      toFormat = true;
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
  for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>::Iterator
         anIter(GetMapOfAIS());
       anIter.More();
       anIter.Next())
  {
    if (!aMatch.IsEmpty())
    {
      const TCollection_AsciiString aCheck =
        TCollection_AsciiString("string match '") + aMatch + "' '" + anIter.Key2() + "'";
      if (theDI.Eval(aCheck.ToCString()) == 0 && *theDI.Result() != '1')
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
// function : VDump
// purpose  : To dump the active view snapshot to image file
//==============================================================================
static int VDump(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  occ::handle<V3d_View> aView = ViewerTest::CurrentView();
  if (theArgNb < 2)
  {
    Message::SendFail(
      "Error: wrong number of arguments! Image file name should be specified at least.");
    return 1;
  }
  if (aView.IsNull())
  {
    Message::SendFail() << "Error: cannot find an active view!";
    return 1;
  }

  int                           anArgIter   = 1;
  const char*                   aFilePath   = theArgVec[anArgIter++];
  ViewerTest_StereoPair         aStereoPair = ViewerTest_SP_Single;
  V3d_ImageDumpOptions          aParams;
  occ::handle<Graphic3d_Camera> aCustomCam;
  aParams.BufferType     = Graphic3d_BT_RGB;
  aParams.StereoOptions  = V3d_SDO_MONO;
  aParams.TargetZLayerId = Graphic3d_ZLayerId_BotOSD;
  aParams.IsSingleLayer  = false;
  aParams.LightName      = "";
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-buffer")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aBufArg(theArgVec[anArgIter]);
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
      else if (aBufArg == "shadowmap")
      {
        if (++anArgIter >= theArgNb)
        {
          Message::SendFail() << "Error: missing light name for shadowmap dump";
          return 1;
        }
        aParams.BufferType = Graphic3d_BT_ShadowMap;
        aParams.LightName  = theArgVec[anArgIter];
        bool isLightFound  = false;
        for (NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator aLightIter(
               aView->ActiveLightIterator());
             aLightIter.More();
             aLightIter.Next())
        {
          occ::handle<V3d_Light> aLight = aLightIter.Value();
          if (aLight->Name() == aParams.LightName)
          {
            isLightFound = true;
            break;
          }
        }
        if (!isLightFound)
        {
          Message::SendFail() << "Error: couldn't find light '" << aParams.LightName << "'";
          return 1;
        }
      }
      else
      {
        Message::SendFail() << "Error: unknown buffer '" << aBufArg << "'";
        return 1;
      }
    }
    else if (anArgIter + 1 < theArgNb && anArg == "-xrpose")
    {
      TCollection_AsciiString anXRArg(theArgVec[++anArgIter]);
      anXRArg.LowerCase();
      if (anXRArg == "base")
      {
        aCustomCam = aView->View()->BaseXRCamera();
      }
      else if (anXRArg == "head")
      {
        aCustomCam = aView->View()->PosedXRCamera();
      }
      else if (anXRArg == "handleft" || anXRArg == "handright")
      {
        if (aView->View()->IsActiveXR())
        {
          aCustomCam = new Graphic3d_Camera();
          aView->View()->ComputeXRPosedCameraFromBase(
            *aCustomCam,
            anXRArg == "handleft" ? aView->View()->XRSession()->LeftHandPose()
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

      TCollection_AsciiString aStereoArg(theArgVec[anArgIter]);
      aStereoArg.LowerCase();
      if (aStereoArg == "l" || aStereoArg == "left")
      {
        aParams.StereoOptions = V3d_SDO_LEFT_EYE;
      }
      else if (aStereoArg == "r" || aStereoArg == "right")
      {
        aParams.StereoOptions = V3d_SDO_RIGHT_EYE;
      }
      else if (aStereoArg == "mono")
      {
        aParams.StereoOptions = V3d_SDO_MONO;
      }
      else if (aStereoArg == "blended" || aStereoArg == "blend" || aStereoArg == "stereo")
      {
        aParams.StereoOptions = V3d_SDO_BLENDED;
      }
      else if (aStereoArg == "sbs" || aStereoArg == "sidebyside")
      {
        aStereoPair = ViewerTest_SP_SideBySide;
      }
      else if (aStereoArg == "ou" || aStereoArg == "overunder")
      {
        aStereoPair = ViewerTest_SP_OverUnder;
      }
      else
      {
        Message::SendFail() << "Error: unknown stereo format '" << aStereoArg << "'";
        return 1;
      }
    }
    else if (anArg == "-rgba" || anArg == "rgba")
    {
      aParams.BufferType = Graphic3d_BT_RGBA;
    }
    else if (anArg == "-rgb" || anArg == "rgb")
    {
      aParams.BufferType = Graphic3d_BT_RGB;
    }
    else if (anArg == "-red" || anArg == "red")
    {
      aParams.BufferType = Graphic3d_BT_Red;
    }
    else if (anArg == "-depth" || anArg == "depth")
    {
      aParams.BufferType = Graphic3d_BT_Depth;
    }
    else if (anArg == "-shadowmap" || anArg == "shadowmap")
    {
      aParams.BufferType = Graphic3d_BT_ShadowMap;
    }
    else if (anArg == "-width" || anArg == "width" || anArg == "sizex")
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
      aParams.Width = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (anArg == "-height" || anArg == "height" || anArg == "-sizey")
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
      aParams.Height = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (anArg == "-tile" || anArg == "-tilesize")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: integer value is expected right after 'tileSize'";
        return 1;
      }
      aParams.TileSize = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (anArg == "-grouplayer")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: integer value is expected right after 'grouplayer'";
        return 1;
      }
      Graphic3d_ZLayerId aZLayer = (Graphic3d_ZLayerId)Draw::Atoi(theArgVec[anArgIter]);
      if (!ViewerTest::ParseZLayer(theArgVec[anArgIter], aZLayer)
          || aZLayer == Graphic3d_ZLayerId_UNKNOWN)
      {
        Message::SendFail() << "Error: invalid layer " << aZLayer << ".";
        return 1;
      }
      aParams.TargetZLayerId = aZLayer;
    }
    else if (anArg == "-singlelayer")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: integer value is expected right after 'singlelayer'";
        return 1;
      }
      Graphic3d_ZLayerId aZLayer = (Graphic3d_ZLayerId)Draw::Atoi(theArgVec[anArgIter]);
      if (!ViewerTest::ParseZLayer(theArgVec[anArgIter], aZLayer)
          || aZLayer == Graphic3d_ZLayerId_UNKNOWN)
      {
        Message::SendFail() << "Error: invalid layer " << aZLayer << ".";
        return 1;
      }
      aParams.TargetZLayerId = aZLayer;
      aParams.IsSingleLayer  = true;
    }
    else
    {
      Message::SendFail() << "Error: unknown argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }
  if ((aParams.Width <= 0 && aParams.Height > 0) || (aParams.Width > 0 && aParams.Height <= 0))
  {
    Message::SendFail() << "Error: dimensions " << aParams.Width << "x" << aParams.Height
                        << " are incorrect";
    return 1;
  }

  if (aParams.Width <= 0 || aParams.Height <= 0)
  {
    aView->Window()->Size(aParams.Width, aParams.Height);
  }

  Image_AlienPixMap aPixMap;
  Image_Format      aFormat = Image_Format_UNKNOWN;
  switch (aParams.BufferType)
  {
    case Graphic3d_BT_RGB:
      aFormat = Image_Format_RGB;
      break;
    case Graphic3d_BT_RGBA:
      aFormat = Image_Format_RGBA;
      break;
    case Graphic3d_BT_Depth:
      aFormat = Image_Format_GrayF;
      break;
    case Graphic3d_BT_RGB_RayTraceHdrLeft:
      aFormat = Image_Format_RGBF;
      break;
    case Graphic3d_BT_Red:
      aFormat = Image_Format_Gray;
      break;
    case Graphic3d_BT_ShadowMap:
      aFormat = Image_Format_GrayF;
      break;
  }

  const bool                    wasImmUpdate = aView->SetImmediateUpdate(false);
  occ::handle<Graphic3d_Camera> aCamBack     = aView->Camera();
  if (!aCustomCam.IsNull())
  {
    aView->SetCamera(aCustomCam);
  }
  switch (aStereoPair)
  {
    case ViewerTest_SP_Single: {
      if (!aView->ToPixMap(aPixMap, aParams))
      {
        theDI << "Fail: view dump failed!\n";
        return 0;
      }
      else if (aPixMap.SizeX() != size_t(aParams.Width)
               || aPixMap.SizeY() != size_t(aParams.Height))
      {
        if (aParams.BufferType != Graphic3d_BT_ShadowMap)
        {
          theDI << "Fail: dumped dimensions " << (int)aPixMap.SizeX() << "x" << (int)aPixMap.SizeY()
                << " are lesser than requested " << aParams.Width << "x" << aParams.Height << "\n";
        }
      }
      break;
    }
    case ViewerTest_SP_SideBySide: {
      if (!aPixMap.InitZero(aFormat, aParams.Width * 2, aParams.Height))
      {
        theDI << "Fail: not enough memory for image allocation!\n";
        return 0;
      }

      Image_PixMap aPixMapL, aPixMapR;
      aPixMapL.InitWrapper(aPixMap.Format(),
                           aPixMap.ChangeData(),
                           aParams.Width,
                           aParams.Height,
                           aPixMap.SizeRowBytes());
      aPixMapR.InitWrapper(aPixMap.Format(),
                           aPixMap.ChangeData() + aPixMap.SizePixelBytes() * aParams.Width,
                           aParams.Width,
                           aParams.Height,
                           aPixMap.SizeRowBytes());

      aParams.StereoOptions = V3d_SDO_LEFT_EYE;
      bool isOk             = aView->ToPixMap(aPixMapL, aParams);
      aParams.StereoOptions = V3d_SDO_RIGHT_EYE;
      isOk                  = isOk && aView->ToPixMap(aPixMapR, aParams);
      if (!isOk)
      {
        theDI << "Fail: view dump failed!\n";
        return 0;
      }
      break;
    }
    case ViewerTest_SP_OverUnder: {
      if (!aPixMap.InitZero(aFormat, aParams.Width, aParams.Height * 2))
      {
        theDI << "Fail: not enough memory for image allocation!\n";
        return 0;
      }

      Image_PixMap aPixMapL, aPixMapR;
      aPixMapL.InitWrapper(aPixMap.Format(),
                           aPixMap.ChangeData(),
                           aParams.Width,
                           aParams.Height,
                           aPixMap.SizeRowBytes());
      aPixMapR.InitWrapper(aPixMap.Format(),
                           aPixMap.ChangeData() + aPixMap.SizeRowBytes() * aParams.Height,
                           aParams.Width,
                           aParams.Height,
                           aPixMap.SizeRowBytes());

      aParams.StereoOptions = V3d_SDO_LEFT_EYE;
      bool isOk             = aView->ToPixMap(aPixMapL, aParams);
      aParams.StereoOptions = V3d_SDO_RIGHT_EYE;
      isOk                  = isOk && aView->ToPixMap(aPixMapR, aParams);
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
    aView->SetCamera(aCamBack);
  }
  aView->SetImmediateUpdate(wasImmUpdate);

  if (!aPixMap.Save(aFilePath))
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
static void VwrTst_DispErase(const occ::handle<AIS_InteractiveObject>& thePrs,
                             const int                                 theMode,
                             const TypeOfDispOperation                 theType,
                             const bool                                theToUpdate)
{
  occ::handle<AIS_InteractiveContext> aCtx = ViewerTest::GetAISContext();
  switch (theType)
  {
    case TypeOfDispOperation_SetDispMode: {
      if (!thePrs.IsNull())
      {
        aCtx->SetDisplayMode(thePrs, theMode, theToUpdate);
      }
      else
      {
        aCtx->SetDisplayMode((AIS_DisplayMode)theMode, theToUpdate);
      }
      break;
    }
    case TypeOfDispOperation_UnsetDispMode: {
      if (!thePrs.IsNull())
      {
        aCtx->UnsetDisplayMode(thePrs, theToUpdate);
      }
      else
      {
        aCtx->SetDisplayMode(AIS_WireFrame, theToUpdate);
      }
      break;
    }
  }
}

//=================================================================================================

static int VDispMode(Draw_Interpretor&, int argc, const char** argv)
{
  if (argc < 1 || argc > 3)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  TypeOfDispOperation                 aType = TCollection_AsciiString(argv[0]) == "vunsetdispmode"
                                                ? TypeOfDispOperation_UnsetDispMode
                                                : TypeOfDispOperation_SetDispMode;
  occ::handle<AIS_InteractiveContext> aCtx  = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  if (aType == TypeOfDispOperation_UnsetDispMode)
  {
    if (argc == 1)
    {
      if (aCtx->NbSelected() == 0)
      {
        VwrTst_DispErase(occ::handle<AIS_InteractiveObject>(),
                         -1,
                         TypeOfDispOperation_UnsetDispMode,
                         false);
      }
      else
      {
        for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
        {
          VwrTst_DispErase(aCtx->SelectedInteractive(),
                           -1,
                           TypeOfDispOperation_UnsetDispMode,
                           false);
        }
      }
      aCtx->UpdateCurrentViewer();
    }
    else
    {
      TCollection_AsciiString            aName = argv[1];
      occ::handle<AIS_InteractiveObject> aPrs;
      if (GetMapOfAIS().Find2(aName, aPrs) && !aPrs.IsNull())
      {
        VwrTst_DispErase(aPrs, -1, TypeOfDispOperation_UnsetDispMode, true);
      }
    }
  }
  else if (argc == 2)
  {
    int aDispMode = Draw::Atoi(argv[1]);
    if (aCtx->NbSelected() == 0 && aType == TypeOfDispOperation_SetDispMode)
    {
      VwrTst_DispErase(occ::handle<AIS_InteractiveObject>(),
                       aDispMode,
                       TypeOfDispOperation_SetDispMode,
                       true);
    }
    for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
    {
      VwrTst_DispErase(aCtx->SelectedInteractive(), aDispMode, aType, false);
    }
    aCtx->UpdateCurrentViewer();
  }
  else
  {
    occ::handle<AIS_InteractiveObject> aPrs;
    TCollection_AsciiString            aName(argv[1]);
    if (GetMapOfAIS().Find2(aName, aPrs) && !aPrs.IsNull())
    {
      VwrTst_DispErase(aPrs, Draw::Atoi(argv[2]), aType, true);
    }
  }
  return 0;
}

//=================================================================================================

static int VSubInt(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc == 1)
    return 1;
  int                                        On  = Draw::Atoi(argv[1]);
  const occ::handle<AIS_InteractiveContext>& Ctx = ViewerTest::GetAISContext();
  if (Ctx.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  if (argc == 2)
  {
    TCollection_AsciiString isOnOff = On == 1 ? "on" : "off";
    di << "Sub intensite is turned " << isOnOff << " for " << Ctx->NbSelected() << "objects\n";
    for (Ctx->InitSelected(); Ctx->MoreSelected(); Ctx->NextSelected())
    {
      if (On == 1)
      {
        Ctx->SubIntensityOn(Ctx->SelectedInteractive(), false);
      }
      else
      {
        Ctx->SubIntensityOff(Ctx->SelectedInteractive(), false);
      }
    }

    Ctx->UpdateCurrentViewer();
  }
  else
  {
    occ::handle<AIS_InteractiveObject> IO;
    TCollection_AsciiString            name = argv[2];
    if (GetMapOfAIS().Find2(name, IO) && !IO.IsNull())
    {
      if (On == 1)
        Ctx->SubIntensityOn(IO, true);
      else
        Ctx->SubIntensityOff(IO, true);
    }
    else
      return 1;
  }
  return 0;
}

//! Auxiliary class to iterate presentations from different collections.
class ViewTest_PrsIter
{
public:
  //! Create and initialize iterator object.
  ViewTest_PrsIter(const TCollection_AsciiString& theName)
      : mySource(IterSource_All)
  {
    NCollection_Sequence<TCollection_AsciiString> aNames;
    if (!theName.IsEmpty())
      aNames.Append(theName);
    Init(aNames);
  }

  //! Create and initialize iterator object.
  ViewTest_PrsIter(const NCollection_Sequence<TCollection_AsciiString>& theNames)
      : mySource(IterSource_All)
  {
    Init(theNames);
  }

  //! Initialize the iterator.
  void Init(const NCollection_Sequence<TCollection_AsciiString>& theNames)
  {
    occ::handle<AIS_InteractiveContext> aCtx = ViewerTest::GetAISContext();
    mySeq                                    = theNames;
    mySelIter.Nullify();
    myCurrent.Nullify();
    myCurrentTrs.Nullify();
    if (!mySeq.IsEmpty())
    {
      mySource  = IterSource_List;
      mySeqIter = NCollection_Sequence<TCollection_AsciiString>::Iterator(mySeq);
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
      myMapIter.Initialize(GetMapOfAIS());
    }
    initCurrent();
  }

  const TCollection_AsciiString& CurrentName() const { return myCurrentName; }

  const occ::handle<AIS_InteractiveObject>& Current() const { return myCurrent; }

  const occ::handle<Standard_Transient>& CurrentTrs() const { return myCurrentTrs; }

  //! @return true if iterator points to valid object within collection
  bool More() const
  {
    switch (mySource)
    {
      case IterSource_All:
        return myMapIter.More();
      case IterSource_List:
        return mySeqIter.More();
      case IterSource_Selected:
        return mySelIter->MoreSelected();
    }
    return false;
  }

  //! Go to the next item.
  void Next()
  {
    myCurrentName.Clear();
    myCurrentTrs.Nullify();
    myCurrent.Nullify();
    switch (mySource)
    {
      case IterSource_All: {
        myMapIter.Next();
        break;
      }
      case IterSource_List: {
        mySeqIter.Next();
        break;
      }
      case IterSource_Selected: {
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
      case IterSource_All: {
        if (myMapIter.More())
        {
          myCurrentName = myMapIter.Key2();
          myCurrentTrs  = myMapIter.Key1();
          myCurrent     = occ::down_cast<AIS_InteractiveObject>(myCurrentTrs);
        }
        break;
      }
      case IterSource_List: {
        if (mySeqIter.More())
        {
          if (!GetMapOfAIS().IsBound2(mySeqIter.Value()))
          {
            Message::SendFail() << "Error: object " << mySeqIter.Value() << " is not displayed!";
            return;
          }
          myCurrentName = mySeqIter.Value();
          myCurrentTrs  = GetMapOfAIS().Find2(mySeqIter.Value());
          myCurrent     = occ::down_cast<AIS_InteractiveObject>(myCurrentTrs);
        }
        break;
      }
      case IterSource_Selected: {
        if (mySelIter->MoreSelected())
        {
          myCurrentName = GetMapOfAIS().Find1(mySelIter->SelectedInteractive());
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
  // clang-format off
  occ::handle<AIS_InteractiveContext> mySelIter;    //!< iterator for current (selected) objects (IterSource_Selected)
  NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>::Iterator myMapIter; //!< iterator for map of all objects (IterSource_All)
  // clang-format on
  NCollection_Sequence<TCollection_AsciiString>           mySeq;
  NCollection_Sequence<TCollection_AsciiString>::Iterator mySeqIter;

  TCollection_AsciiString            myCurrentName; //!< current item name
  occ::handle<Standard_Transient>    myCurrentTrs;  //!< current item (as transient object)
  occ::handle<AIS_InteractiveObject> myCurrent;     //!< current item

  IterSource mySource; //!< iterated collection
};

//! Parse interior style name.
static bool parseInteriorStyle(const TCollection_AsciiString& theArg,
                               Aspect_InteriorStyle&          theStyle)
{
  TCollection_AsciiString anArg(theArg);
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
  else if (anArg == "hiddenline" || anArg == "hidden-line" || anArg == "hidden_line")
  {
    theStyle = Aspect_IS_HIDDENLINE;
  }
  else if (anArg == "point")
  {
    theStyle = Aspect_IS_POINT;
  }
  else if (theArg.IsIntegerValue())
  {
    const int anIntStyle = theArg.IntegerValue();
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
  int ToSetVisibility;
  int Visibility;

  int            ToSetColor;
  Quantity_Color Color;
  int            ToSetBackFaceColor;
  Quantity_Color BackFaceColor;

  int    ToSetLineWidth;
  double LineWidth;

  int      ToSetTypeOfLine;
  uint16_t StippleLinePattern;
  uint16_t StippleLineFactor;

  int                       ToSetTypeOfMarker;
  Aspect_TypeOfMarker       TypeOfMarker;
  occ::handle<Image_PixMap> MarkerImage;

  int    ToSetMarkerSize;
  double MarkerSize;

  int    ToSetTransparency;
  double Transparency;

  int                 ToSetAlphaMode;
  Graphic3d_AlphaMode AlphaMode;
  float               AlphaCutoff;

  int                             ToSetFaceCulling;
  Graphic3d_TypeOfBackfacingModel FaceCulling;

  int                      ToSetMaterial;
  Graphic3d_NameOfMaterial Material;
  TCollection_AsciiString  MatName;

  NCollection_Sequence<TopoDS_Shape> SubShapes;

  int            ToSetShowFreeBoundary;
  int            ToSetFreeBoundaryWidth;
  double         FreeBoundaryWidth;
  int            ToSetFreeBoundaryColor;
  Quantity_Color FreeBoundaryColor;

  int ToEnableIsoOnTriangulation;

  int           ToSetFaceBoundaryDraw;
  int           ToSetFaceBoundaryUpperContinuity;
  GeomAbs_Shape FaceBoundaryUpperContinuity;

  int            ToSetFaceBoundaryColor;
  Quantity_Color FaceBoundaryColor;

  int    ToSetFaceBoundaryWidth;
  double FaceBoundaryWidth;

  int               ToSetTypeOfFaceBoundaryLine;
  Aspect_TypeOfLine TypeOfFaceBoundaryLine;

  int    ToSetMaxParamValue;
  double MaxParamValue;

  int ToSetSensitivity;
  int SelectionMode;
  int Sensitivity;

  int                     ToSetHatch;
  int                     StdHatchStyle;
  TCollection_AsciiString PathToHatchPattern;

  int                          ToSetShadingModel;
  Graphic3d_TypeOfShadingModel ShadingModel;
  TCollection_AsciiString      ShadingModelName;

  int                  ToSetInterior;
  Aspect_InteriorStyle InteriorStyle;

  int ToSetDrawSilhouette;

  int ToSetDrawEdges;
  int ToSetQuadEdges;

  int                ToSetEdgeColor;
  Quantity_ColorRGBA EdgeColor;

  int    ToSetEdgeWidth;
  double EdgeWidth;

  int               ToSetTypeOfEdge;
  Aspect_TypeOfLine TypeOfEdge;

  //! Empty constructor
  ViewerTest_AspectsChangeSet()
      : ToSetVisibility(0),
        Visibility(1),
        ToSetColor(0),
        Color(DEFAULT_COLOR),
        ToSetBackFaceColor(0),
        BackFaceColor(DEFAULT_COLOR),
        ToSetLineWidth(0),
        LineWidth(1.0),
        ToSetTypeOfLine(0),
        StippleLinePattern(0xFFFF),
        StippleLineFactor(1),
        ToSetTypeOfMarker(0),
        TypeOfMarker(Aspect_TOM_PLUS),
        ToSetMarkerSize(0),
        MarkerSize(1.0),
        ToSetTransparency(0),
        Transparency(0.0),
        ToSetAlphaMode(0),
        AlphaMode(Graphic3d_AlphaMode_BlendAuto),
        AlphaCutoff(0.5f),
        ToSetFaceCulling(0),
        FaceCulling(Graphic3d_TypeOfBackfacingModel_Auto),
        ToSetMaterial(0),
        Material(Graphic3d_NameOfMaterial_DEFAULT),
        ToSetShowFreeBoundary(0),
        ToSetFreeBoundaryWidth(0),
        FreeBoundaryWidth(1.0),
        ToSetFreeBoundaryColor(0),
        FreeBoundaryColor(DEFAULT_FREEBOUNDARY_COLOR),
        ToEnableIsoOnTriangulation(0),
        //
        ToSetFaceBoundaryDraw(0),
        ToSetFaceBoundaryUpperContinuity(0),
        FaceBoundaryUpperContinuity(GeomAbs_CN),
        ToSetFaceBoundaryColor(0),
        FaceBoundaryColor(Quantity_NOC_BLACK),
        ToSetFaceBoundaryWidth(0),
        FaceBoundaryWidth(1.0f),
        ToSetTypeOfFaceBoundaryLine(0),
        TypeOfFaceBoundaryLine(Aspect_TOL_SOLID),
        //
        ToSetMaxParamValue(0),
        MaxParamValue(500000),
        ToSetSensitivity(0),
        SelectionMode(-1),
        Sensitivity(-1),
        ToSetHatch(0),
        StdHatchStyle(-1),
        ToSetShadingModel(0),
        ShadingModel(Graphic3d_TypeOfShadingModel_DEFAULT),
        ToSetInterior(0),
        InteriorStyle(Aspect_IS_SOLID),
        ToSetDrawSilhouette(0),
        ToSetDrawEdges(0),
        ToSetQuadEdges(0),
        ToSetEdgeColor(0),
        ToSetEdgeWidth(0),
        EdgeWidth(1.0),
        ToSetTypeOfEdge(0),
        TypeOfEdge(Aspect_TOL_SOLID)
  {
  }

  //! @return true if no changes have been requested
  bool IsEmpty() const
  {
    return ToSetVisibility == 0 && ToSetLineWidth == 0 && ToSetTransparency == 0
           && ToSetAlphaMode == 0 && ToSetFaceCulling == 0 && ToSetColor == 0
           && ToSetBackFaceColor == 0 && ToSetMaterial == 0 && ToSetShowFreeBoundary == 0
           && ToSetFreeBoundaryColor == 0 && ToSetFreeBoundaryWidth == 0
           && ToEnableIsoOnTriangulation == 0 && ToSetFaceBoundaryDraw == 0
           && ToSetFaceBoundaryUpperContinuity == 0 && ToSetFaceBoundaryColor == 0
           && ToSetFaceBoundaryWidth == 0 && ToSetTypeOfFaceBoundaryLine == 0
           && ToSetMaxParamValue == 0 && ToSetSensitivity == 0 && ToSetHatch == 0
           && ToSetShadingModel == 0 && ToSetInterior == 0 && ToSetDrawSilhouette == 0
           && ToSetDrawEdges == 0 && ToSetQuadEdges == 0 && ToSetEdgeColor == 0
           && ToSetEdgeWidth == 0 && ToSetTypeOfEdge == 0;
  }

  //! @return true if properties are valid
  bool Validate() const
  {
    bool isOk = true;
    if (Visibility != 0 && Visibility != 1)
    {
      Message::SendFail() << "Error: the visibility should be equal to 0 or 1 (0 - invisible; 1 - "
                             "visible) (specified "
                          << Visibility << ")";
      isOk = false;
    }
    if (LineWidth <= 0.0 || LineWidth > 10.0)
    {
      Message::SendFail() << "Error: the width should be within [1; 10] range (specified "
                          << LineWidth << ")";
      isOk = false;
    }
    if (Transparency < 0.0 || Transparency > 1.0)
    {
      Message::SendFail() << "Error: the transparency should be within [0; 1] range (specified "
                          << Transparency << ")";
      isOk = false;
    }
    if (ToSetAlphaMode == 1 && (AlphaCutoff <= 0.0f || AlphaCutoff >= 1.0f))
    {
      Message::SendFail() << "Error: alpha cutoff value should be within (0; 1) range (specified "
                          << AlphaCutoff << ")";
      isOk = false;
    }
    if (FreeBoundaryWidth <= 0.0 || FreeBoundaryWidth > 10.0)
    {
      Message::SendFail()
        << "Error: the free boundary width should be within [1; 10] range (specified "
        << FreeBoundaryWidth << ")";
      isOk = false;
    }
    if (MaxParamValue < 0.0)
    {
      Message::SendFail()
        << "Error: the max parameter value should be greater than zero (specified " << MaxParamValue
        << ")";
      isOk = false;
    }
    if (Sensitivity < 0 && ToSetSensitivity)
    {
      Message::SendFail() << "Error: sensitivity parameter value should not be negative (specified "
                          << Sensitivity << ")";
      isOk = false;
    }
    if (ToSetHatch == 1 && StdHatchStyle < 0 && PathToHatchPattern == "")
    {
      Message::SendFail("Error: hatch style must be specified");
      isOk = false;
    }
    if (ToSetShadingModel == 1
        && (ShadingModel < Graphic3d_TypeOfShadingModel_DEFAULT
            || ShadingModel > Graphic3d_TypeOfShadingModel_PbrFacet))
    {
      Message::SendFail() << "Error: unknown shading model " << ShadingModelName << ".";
      isOk = false;
    }
    return isOk;
  }

  //! Apply aspects to specified drawer.
  bool Apply(const occ::handle<Prs3d_Drawer>& theDrawer)
  {
    bool                             toRecompute = false;
    const occ::handle<Prs3d_Drawer>& aDefDrawer  = ViewerTest::GetAISContext()->DefaultDrawer();
    if (ToSetShowFreeBoundary != 0)
    {
      theDrawer->SetFreeBoundaryDraw(ToSetShowFreeBoundary == 1);
      toRecompute = true;
    }
    if (ToSetFreeBoundaryWidth != 0)
    {
      if (ToSetFreeBoundaryWidth != -1 || theDrawer->HasOwnFreeBoundaryAspect())
      {
        if (!theDrawer->HasOwnFreeBoundaryAspect())
        {
          occ::handle<Prs3d_LineAspect> aBoundaryAspect =
            new Prs3d_LineAspect(Quantity_NOC_RED, Aspect_TOL_SOLID, 1.0);
          *aBoundaryAspect->Aspect() = *theDrawer->FreeBoundaryAspect()->Aspect();
          theDrawer->SetFreeBoundaryAspect(aBoundaryAspect);
          toRecompute = true;
        }
        theDrawer->FreeBoundaryAspect()->SetWidth(FreeBoundaryWidth);
      }
    }
    if (ToSetFreeBoundaryColor != 0)
    {
      occ::handle<Prs3d_LineAspect> aBoundaryAspect =
        new Prs3d_LineAspect(Quantity_NOC_RED, Aspect_TOL_SOLID, 1.0);
      *aBoundaryAspect->Aspect() = *theDrawer->FreeBoundaryAspect()->Aspect();
      aBoundaryAspect->SetColor(FreeBoundaryColor);
      theDrawer->SetFreeBoundaryAspect(aBoundaryAspect);
      toRecompute = true;
    }
    if (ToSetTypeOfLine != 0)
    {
      if (ToSetTypeOfLine != -1 || theDrawer->HasOwnLineAspect() || theDrawer->HasOwnWireAspect()
          || theDrawer->HasOwnFreeBoundaryAspect() || theDrawer->HasOwnUnFreeBoundaryAspect()
          || theDrawer->HasOwnSeenLineAspect())
      {
        toRecompute = theDrawer->SetOwnLineAspects() || toRecompute;
        theDrawer->LineAspect()->Aspect()->SetLinePattern(StippleLinePattern);
        theDrawer->LineAspect()->Aspect()->SetLineStippleFactor(StippleLineFactor);
        theDrawer->WireAspect()->Aspect()->SetLinePattern(StippleLinePattern);
        theDrawer->WireAspect()->Aspect()->SetLineStippleFactor(StippleLineFactor);
        theDrawer->FreeBoundaryAspect()->Aspect()->SetLinePattern(StippleLinePattern);
        theDrawer->FreeBoundaryAspect()->Aspect()->SetLineStippleFactor(StippleLineFactor);
        theDrawer->UnFreeBoundaryAspect()->Aspect()->SetLinePattern(StippleLinePattern);
        theDrawer->UnFreeBoundaryAspect()->Aspect()->SetLineStippleFactor(StippleLineFactor);
        theDrawer->SeenLineAspect()->Aspect()->SetLinePattern(StippleLinePattern);
        theDrawer->SeenLineAspect()->Aspect()->SetLineStippleFactor(StippleLineFactor);
      }
    }
    if (ToSetTypeOfMarker != 0)
    {
      if (ToSetTypeOfMarker != -1 || theDrawer->HasOwnPointAspect())
      {
        toRecompute = theDrawer->SetupOwnPointAspect(aDefDrawer) || toRecompute;
        theDrawer->PointAspect()->SetTypeOfMarker(TypeOfMarker);
        theDrawer->PointAspect()->Aspect()->SetMarkerImage(
          MarkerImage.IsNull() ? occ::handle<Graphic3d_MarkerImage>()
                               : new Graphic3d_MarkerImage(MarkerImage));
      }
    }
    if (ToSetMarkerSize != 0)
    {
      if (ToSetMarkerSize != -1 || theDrawer->HasOwnPointAspect())
      {
        toRecompute = theDrawer->SetupOwnPointAspect(aDefDrawer) || toRecompute;
        theDrawer->PointAspect()->SetScale(MarkerSize);
        toRecompute = true;
      }
    }
    if (ToSetMaxParamValue != 0)
    {
      if (ToSetMaxParamValue != -1 || theDrawer->HasOwnMaximalParameterValue())
      {
        theDrawer->SetMaximalParameterValue(MaxParamValue);
        toRecompute = true;
      }
    }
    if (ToSetFaceBoundaryDraw != 0)
    {
      if (ToSetFaceBoundaryDraw != -1 || theDrawer->HasOwnFaceBoundaryDraw())
      {
        toRecompute = true;
        theDrawer->SetFaceBoundaryDraw(ToSetFaceBoundaryDraw == 1);
      }
    }
    if (ToSetFaceBoundaryUpperContinuity != 0)
    {
      if (ToSetFaceBoundaryUpperContinuity != -1 || theDrawer->HasOwnFaceBoundaryUpperContinuity())
      {
        toRecompute = true;
        if (ToSetFaceBoundaryUpperContinuity == -1)
        {
          theDrawer->UnsetFaceBoundaryUpperContinuity();
        }
        else
        {
          theDrawer->SetFaceBoundaryUpperContinuity(FaceBoundaryUpperContinuity);
        }
      }
    }
    if (ToSetFaceBoundaryColor != 0)
    {
      if (ToSetFaceBoundaryColor != -1 || theDrawer->HasOwnFaceBoundaryAspect())
      {
        if (ToSetFaceBoundaryColor == -1)
        {
          toRecompute = true;
          theDrawer->SetFaceBoundaryAspect(occ::handle<Prs3d_LineAspect>());
        }
        else
        {
          toRecompute = theDrawer->SetupOwnFaceBoundaryAspect(aDefDrawer) || toRecompute;
          theDrawer->FaceBoundaryAspect()->SetColor(FaceBoundaryColor);
        }
      }
    }
    if (ToSetFaceBoundaryWidth != 0)
    {
      if (ToSetFaceBoundaryWidth != -1 || theDrawer->HasOwnFaceBoundaryAspect())
      {
        toRecompute = theDrawer->SetupOwnFaceBoundaryAspect(aDefDrawer) || toRecompute;
        theDrawer->FaceBoundaryAspect()->SetWidth(FaceBoundaryWidth);
      }
    }
    if (ToSetTypeOfFaceBoundaryLine != 0)
    {
      if (ToSetTypeOfFaceBoundaryLine != -1 || theDrawer->HasOwnFaceBoundaryAspect())
      {
        toRecompute = theDrawer->SetupOwnFaceBoundaryAspect(aDefDrawer) || toRecompute;
        theDrawer->FaceBoundaryAspect()->SetTypeOfLine(TypeOfFaceBoundaryLine);
      }
    }
    if (ToSetShadingModel != 0)
    {
      if (ToSetShadingModel != -1 || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect(aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetShadingModel(ShadingModel);
      }
    }
    if (ToSetBackFaceColor != 0)
    {
      if (ToSetBackFaceColor != -1 || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect(aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->SetColor(BackFaceColor, Aspect_TOFM_BACK_SIDE);
      }
    }
    if (ToSetAlphaMode != 0)
    {
      if (ToSetAlphaMode != -1 || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect(aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetAlphaMode(AlphaMode, AlphaCutoff);
      }
    }
    if (ToSetFaceCulling != 0)
    {
      if (ToSetFaceCulling != -1 || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect(aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetFaceCulling(FaceCulling);
      }
    }
    if (ToSetHatch != 0)
    {
      if (ToSetHatch != -1 || theDrawer->HasOwnShadingAspect())
      {
        theDrawer->SetupOwnShadingAspect(aDefDrawer);
        occ::handle<Graphic3d_AspectFillArea3d> anAsp = theDrawer->ShadingAspect()->Aspect();
        if (ToSetHatch == -1)
        {
          anAsp->SetInteriorStyle(Aspect_IS_SOLID);
        }
        else
        {
          anAsp->SetInteriorStyle(Aspect_IS_HATCH);
          if (!PathToHatchPattern.IsEmpty())
          {
            occ::handle<Image_AlienPixMap> anImage = new Image_AlienPixMap();
            if (anImage->Load(TCollection_AsciiString(PathToHatchPattern.ToCString())))
            {
              anAsp->SetHatchStyle(new Graphic3d_HatchStyle(anImage));
            }
            else
            {
              Message::SendFail() << "Error: cannot load the following image: "
                                  << PathToHatchPattern;
            }
          }
          else if (StdHatchStyle != -1)
          {
            anAsp->SetHatchStyle(new Graphic3d_HatchStyle((Aspect_HatchStyle)StdHatchStyle));
          }
        }
        toRecompute = true;
      }
    }
    if (ToSetInterior != 0)
    {
      if (ToSetInterior != -1 || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect(aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetInteriorStyle(InteriorStyle);
        if (InteriorStyle == Aspect_IS_HATCH
            && theDrawer->ShadingAspect()->Aspect()->HatchStyle().IsNull())
        {
          theDrawer->ShadingAspect()->Aspect()->SetHatchStyle(Aspect_HS_VERTICAL);
        }
      }
    }
    if (ToSetDrawSilhouette != 0)
    {
      if (ToSetDrawSilhouette != -1 || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect(aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetDrawSilhouette(ToSetDrawSilhouette == 1);
      }
    }
    if (ToSetDrawEdges != 0)
    {
      if (ToSetDrawEdges != -1 || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect(aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetDrawEdges(ToSetDrawEdges == 1);
      }
    }
    if (ToSetQuadEdges != 0)
    {
      if (ToSetQuadEdges != -1 || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect(aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetSkipFirstEdge(ToSetQuadEdges == 1);
      }
    }
    if (ToSetEdgeWidth != 0)
    {
      if (ToSetEdgeWidth != -1 || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect(aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetEdgeWidth(EdgeWidth);
      }
    }
    if (ToSetTypeOfEdge != 0)
    {
      if (ToSetTypeOfEdge != -1 || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect(aDefDrawer) || toRecompute;
        theDrawer->ShadingAspect()->Aspect()->SetEdgeLineType(TypeOfEdge);
        if (ToSetInterior == 0)
        {
          theDrawer->ShadingAspect()->Aspect()->SetDrawEdges(ToSetTypeOfEdge == 1
                                                             && TypeOfEdge != Aspect_TOL_EMPTY);
        }
      }
    }
    if (ToSetEdgeColor != 0)
    {
      if (ToSetEdgeColor != -1 || theDrawer->HasOwnShadingAspect())
      {
        toRecompute = theDrawer->SetupOwnShadingAspect(aDefDrawer) || toRecompute;
        if (ToSetEdgeColor == -1)
        {
          theDrawer->ShadingAspect()->Aspect()->SetEdgeColor(
            theDrawer->ShadingAspect()->Aspect()->InteriorColor());
        }
        else
        {
          theDrawer->ShadingAspect()->Aspect()->SetEdgeColor(EdgeColor);
        }
      }
    }
    return toRecompute;
  }
};

//=================================================================================================

static int VAspects(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  TCollection_AsciiString                    aCmdName(theArgVec[0]);
  const occ::handle<AIS_InteractiveContext>& aCtx = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater                     anUpdateTool(aCtx, ViewerTest::CurrentView());
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active view!");
    return 1;
  }

  int                                           anArgIter  = 1;
  bool                                          isDefaults = false;
  NCollection_Sequence<TCollection_AsciiString> aNames;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg = theArgVec[anArgIter];
    if (anUpdateTool.parseRedrawMode(anArg))
    {
      continue;
    }
    else if (!anArg.IsEmpty() && anArg.Value(1) != '-')
    {
      aNames.Append(anArg);
    }
    else
    {
      if (anArg == "-defaults")
      {
        isDefaults = true;
        ++anArgIter;
      }
      break;
    }
  }

  if (!aNames.IsEmpty() && isDefaults)
  {
    Message::SendFail(
      "Error: wrong syntax. If -defaults is used there should not be any objects' names!");
    return 1;
  }

  NCollection_Sequence<ViewerTest_AspectsChangeSet> aChanges;
  aChanges.Append(ViewerTest_AspectsChangeSet());
  ViewerTest_AspectsChangeSet* aChangeSet = &aChanges.ChangeLast();

  // parse syntax of legacy commands
  bool toParseAliasArgs = false;
  bool toDump           = false;
  bool toCompactDump    = false;
  int  aDumpDepth       = -1;
  if (aCmdName == "vsetwidth")
  {
    if (aNames.IsEmpty() || !aNames.Last().IsRealValue(true))
    {
      Message::SendFail("Error: not enough arguments!");
      return 1;
    }
    aChangeSet->ToSetLineWidth = 1;
    aChangeSet->LineWidth      = aNames.Last().RealValue();
    aNames.Remove(aNames.Length());
  }
  else if (aCmdName == "vunsetwidth")
  {
    aChangeSet->ToSetLineWidth = -1;
  }
  else if (aCmdName == "vsetcolor")
  {
    if (aNames.IsEmpty())
    {
      Message::SendFail("Error: not enough arguments!");
      return 1;
    }
    aChangeSet->ToSetColor = 1;

    Quantity_NameOfColor aColor = Quantity_NOC_BLACK;
    bool                 isOk   = false;
    if (Quantity_Color::ColorFromName(aNames.Last().ToCString(), aColor))
    {
      aChangeSet->Color = aColor;
      aNames.Remove(aNames.Length());
      isOk = true;
    }
    else if (Quantity_Color::ColorFromHex(aNames.Last().ToCString(), aChangeSet->Color))
    {
      aNames.Remove(aNames.Length());
      isOk = true;
    }
    else if (aNames.Length() >= 3)
    {
      const char* anArgVec[3] = {
        aNames.Value(aNames.Upper() - 2).ToCString(),
        aNames.Value(aNames.Upper() - 1).ToCString(),
        aNames.Value(aNames.Upper() - 0).ToCString(),
      };

      int aNbParsed = Draw::ParseColor(3, anArgVec, aChangeSet->Color);
      isOk          = aNbParsed == 3;
      aNames.Remove(aNames.Length());
      aNames.Remove(aNames.Length());
      aNames.Remove(aNames.Length());
    }
    if (!isOk)
    {
      Message::SendFail("Error: not enough arguments!");
      return 1;
    }
  }
  else if (aCmdName == "vunsetcolor")
  {
    aChangeSet->ToSetColor = -1;
  }
  else if (aCmdName == "vsettransparency")
  {
    if (aNames.IsEmpty() || !aNames.Last().IsRealValue(true))
    {
      Message::SendFail("Error: not enough arguments!");
      return 1;
    }
    aChangeSet->ToSetTransparency = 1;
    aChangeSet->Transparency      = aNames.Last().RealValue();
    aNames.Remove(aNames.Length());
  }
  else if (aCmdName == "vunsettransparency")
  {
    aChangeSet->ToSetTransparency = -1;
  }
  else if (aCmdName == "vsetmaterial")
  {
    if (aNames.IsEmpty())
    {
      Message::SendFail("Error: not enough arguments!");
      return 1;
    }
    aChangeSet->ToSetMaterial = 1;
    aChangeSet->MatName       = aNames.Last();
    aNames.Remove(aNames.Length());
    if (!Graphic3d_MaterialAspect::MaterialFromName(aChangeSet->MatName.ToCString(),
                                                    aChangeSet->Material))
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
    if (aNames.IsEmpty() || !aNames.Last().IsRealValue(true))
    {
      Message::SendFail("Error: not enough arguments!");
      return 1;
    }
    aChangeSet->ToSetInterior = 1;
    if (!parseInteriorStyle(aNames.Last(), aChangeSet->InteriorStyle))
    {
      Message::SendFail() << "Error: wrong syntax at " << aNames.Last();
      return 1;
    }
    aNames.Remove(aNames.Length());
  }
  else if (aCmdName == "vsetedgetype")
  {
    aChangeSet->ToSetDrawEdges = 1;
    toParseAliasArgs           = true;
  }
  else if (aCmdName == "vunsetedgetype")
  {
    aChangeSet->ToSetDrawEdges  = -1;
    aChangeSet->ToSetEdgeColor  = -1;
    aChangeSet->ToSetTypeOfEdge = -1;
    aChangeSet->TypeOfEdge      = Aspect_TOL_SOLID;
  }
  else if (aCmdName == "vshowfaceboundary")
  {
    aChangeSet->ToSetFaceBoundaryDraw = 1;
    toParseAliasArgs                  = true;
    if (aNames.Size() >= 2 && aNames.Value(2).IsIntegerValue())
    {
      if (aNames.Size() == 7)
      {
        if (ViewerTest::ParseLineType(aNames.Value(7).ToCString(),
                                      aChangeSet->TypeOfFaceBoundaryLine))
        {
          aChangeSet->ToSetTypeOfFaceBoundaryLine = 1;
          aNames.Remove(7);
        }
      }
      if (aNames.Size() == 6 && aNames.Value(6).IsRealValue(true))
      {
        aChangeSet->ToSetFaceBoundaryWidth = 1;
        aChangeSet->FaceBoundaryWidth      = aNames.Value(6).RealValue();
        aNames.Remove(6);
      }
      if (aNames.Size() == 5 && aNames.Value(3).IsIntegerValue() && aNames.Value(4).IsIntegerValue()
          && aNames.Value(5).IsIntegerValue())
      {
        aChangeSet->ToSetFaceBoundaryColor = 1;
        aChangeSet->FaceBoundaryColor      = Quantity_Color(aNames.Value(3).IntegerValue() / 255.0,
                                                       aNames.Value(4).IntegerValue() / 255.0,
                                                       aNames.Value(5).IntegerValue() / 255.0,
                                                       Quantity_TOC_sRGB);
        aNames.Remove(5);
        aNames.Remove(4);
        aNames.Remove(3);
      }
      if (aNames.Size() == 2)
      {
        toParseAliasArgs                  = false;
        aChangeSet->ToSetFaceBoundaryDraw = aNames.Value(2).IntegerValue() == 1 ? 1 : -1;
        aNames.Remove(2);
      }
    }
  }
  else if (anArgIter >= theArgNb)
  {
    Message::SendFail("Error: not enough arguments!");
    return 1;
  }

  if (!aChangeSet->IsEmpty() && !toParseAliasArgs)
  {
    anArgIter = theArgNb;
  }
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg = theArgVec[anArgIter];
    anArg.LowerCase();
    if (anArg == "-setwidth" || anArg == "-width" || anArg == "-setlinewidth"
        || anArg == "-linewidth" || anArg == "-setedgewidth" || anArg == "-setedgeswidth"
        || anArg == "-edgewidth" || anArg == "-edgeswidth" || anArg == "-setfaceboundarywidth"
        || anArg == "-setboundarywidth" || anArg == "-faceboundarywidth"
        || anArg == "-boundarywidth")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }

      const double aWidth = Draw::Atof(theArgVec[anArgIter]);
      if (anArg == "-setedgewidth" || anArg == "-setedgeswidth" || anArg == "-edgewidth"
          || anArg == "-edgeswidth" || aCmdName == "vsetedgetype")
      {
        aChangeSet->ToSetEdgeWidth = 1;
        aChangeSet->EdgeWidth      = aWidth;
      }
      else if (anArg == "-setfaceboundarywidth" || anArg == "-setboundarywidth"
               || anArg == "-faceboundarywidth" || anArg == "-boundarywidth"
               || aCmdName == "vshowfaceboundary")
      {
        aChangeSet->ToSetFaceBoundaryWidth = 1;
        aChangeSet->FaceBoundaryWidth      = aWidth;
      }
      else
      {
        aChangeSet->ToSetLineWidth = 1;
        aChangeSet->LineWidth      = aWidth;
      }
    }
    else if (anArg == "-unsetwidth" || anArg == "-unsetlinewidth" || anArg == "-unsetedgewidth")
    {
      if (anArg == "-unsetedgewidth")
      {
        aChangeSet->ToSetEdgeWidth = -1;
        aChangeSet->EdgeWidth      = 1.0;
      }
      else
      {
        aChangeSet->ToSetLineWidth = -1;
        aChangeSet->LineWidth      = 1.0;
      }
    }
    else if (anArg == "-settransp" || anArg == "-settransparency" || anArg == "-transparency"
             || anArg == "-transp")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetTransparency = 1;
      aChangeSet->Transparency      = Draw::Atof(theArgVec[anArgIter]);
      if (aChangeSet->Transparency >= 0.0 && aChangeSet->Transparency <= Precision::Confusion())
      {
        aChangeSet->ToSetTransparency = -1;
        aChangeSet->Transparency      = 0.0;
      }
    }
    else if (anArg == "-setalphamode" || anArg == "-alphamode")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetAlphaMode = 1;
      aChangeSet->AlphaCutoff    = 0.5f;
      {
        TCollection_AsciiString aParam(theArgVec[anArgIter]);
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
        else if (aParam == "maskblend" || aParam == "blendmask")
        {
          aChangeSet->AlphaMode = Graphic3d_AlphaMode_MaskBlend;
        }
        else if (aParam == "blendauto" || aParam == "auto")
        {
          aChangeSet->AlphaMode = Graphic3d_AlphaMode_BlendAuto;
        }
        else
        {
          Message::SendFail() << "Error: wrong syntax at " << aParam;
          return 1;
        }
      }

      if (anArgIter + 1 < theArgNb && theArgVec[anArgIter + 1][0] != '-')
      {
        TCollection_AsciiString aParam2(theArgVec[anArgIter + 1]);
        if (aParam2.IsRealValue(true))
        {
          aChangeSet->AlphaCutoff = (float)aParam2.RealValue();
          ++anArgIter;
        }
      }
    }
    else if (anArg == "-setfaceculling" || anArg == "-faceculling")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }

      aChangeSet->ToSetFaceCulling = 1;
      {
        TCollection_AsciiString aParam(theArgVec[anArgIter]);
        aParam.LowerCase();
        if (aParam == "auto")
        {
          aChangeSet->FaceCulling = Graphic3d_TypeOfBackfacingModel_Auto;
        }
        else if (aParam == "backculled" || aParam == "backcull" || aParam == "back")
        {
          aChangeSet->FaceCulling = Graphic3d_TypeOfBackfacingModel_BackCulled;
        }
        else if (aParam == "frontculled" || aParam == "frontcull" || aParam == "front")
        {
          aChangeSet->FaceCulling = Graphic3d_TypeOfBackfacingModel_FrontCulled;
        }
        else if (aParam == "doublesided" || aParam == "off")
        {
          aChangeSet->FaceCulling = Graphic3d_TypeOfBackfacingModel_DoubleSided;
        }
        else
        {
          Message::SendFail() << "Error: wrong syntax at '" << aParam << "'";
          return 1;
        }
      }
    }
    else if (anArg == "-setvis" || anArg == "-setvisibility" || anArg == "-visibility")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }

      aChangeSet->ToSetVisibility = 1;
      aChangeSet->Visibility      = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (anArg == "-setalpha" || anArg == "-alpha")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetTransparency = 1;
      aChangeSet->Transparency      = Draw::Atof(theArgVec[anArgIter]);
      if (aChangeSet->Transparency < 0.0 || aChangeSet->Transparency > 1.0)
      {
        Message::SendFail() << "Error: the transparency should be within [0; 1] range (specified "
                            << aChangeSet->Transparency << ")";
        return 1;
      }
      aChangeSet->Transparency = 1.0 - aChangeSet->Transparency;
      if (aChangeSet->Transparency >= 0.0 && aChangeSet->Transparency <= Precision::Confusion())
      {
        aChangeSet->ToSetTransparency = -1;
        aChangeSet->Transparency      = 0.0;
      }
    }
    else if (anArg == "-unsettransp" || anArg == "-unsettransparency" || anArg == "-unsetalpha"
             || anArg == "-opaque")
    {
      aChangeSet->ToSetTransparency = -1;
      aChangeSet->Transparency      = 0.0;
    }
    else if (anArg == "-setcolor" || anArg == "-color" || anArg == "-setbackfacecolor"
             || anArg == "-backfacecolor" || anArg == "-setbackcolor" || anArg == "-backcolor"
             || anArg == "-setfaceboundarycolor" || anArg == "-setboundarycolor"
             || anArg == "-faceboundarycolor" || anArg == "-boundarycolor")
    {
      Quantity_Color aColor;
      int aNbParsed = Draw::ParseColor(theArgNb - anArgIter - 1, theArgVec + anArgIter + 1, aColor);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
      anArgIter += aNbParsed;
      if (aCmdName == "vsetedgetype")
      {
        aChangeSet->ToSetEdgeColor = 1;
        aChangeSet->EdgeColor      = Quantity_ColorRGBA(aColor);
      }
      else if (aCmdName == "vshowfaceboundary" || anArg == "-setfaceboundarycolor"
               || anArg == "-setboundarycolor" || anArg == "-faceboundarycolor"
               || anArg == "-boundarycolor")
      {
        aChangeSet->ToSetFaceBoundaryColor = 1;
        aChangeSet->FaceBoundaryColor      = aColor;
      }
      else if (anArg == "-setbackfacecolor" || anArg == "-backfacecolor" || anArg == "-setbackcolor"
               || anArg == "-backcolor")
      {
        aChangeSet->ToSetBackFaceColor = 1;
        aChangeSet->BackFaceColor      = aColor;
      }
      else
      {
        aChangeSet->ToSetColor = 1;
        aChangeSet->Color      = aColor;
      }
    }
    else if (anArg == "-setlinetype" || anArg == "-linetype" || anArg == "-setedgetype"
             || anArg == "-setedgestype" || anArg == "-edgetype" || anArg == "-edgestype"
             || anArg == "-setfaceboundarystyle" || anArg == "-faceboundarystyle"
             || anArg == "-boundarystyle" || anArg == "-setfaceboundarytype"
             || anArg == "-faceboundarytype" || anArg == "-setboundarytype"
             || anArg == "-boundarytype" || anArg == "-type")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      Aspect_TypeOfLine aLineType    = Aspect_TOL_EMPTY;
      uint16_t          aLinePattern = 0xFFFF;
      if (!ViewerTest::ParseLineType(theArgVec[anArgIter], aLineType, aLinePattern))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }

      if (anArg == "-setedgetype" || anArg == "-setedgestype" || anArg == "-edgetype"
          || anArg == "-edgestype" || aCmdName == "vsetedgetype")
      {
        aChangeSet->TypeOfEdge      = Graphic3d_Aspects::DefaultLineTypeForPattern(aLinePattern);
        aChangeSet->ToSetTypeOfEdge = 1;
      }
      else if (anArg == "-setfaceboundarystyle" || anArg == "-faceboundarystyle"
               || anArg == "-boundarystyle" || anArg == "-setfaceboundarytype"
               || anArg == "-faceboundarytype" || anArg == "-setboundarytype"
               || anArg == "-boundarytype" || aCmdName == "vshowfaceboundary")
      {
        aChangeSet->TypeOfFaceBoundaryLine =
          Graphic3d_Aspects::DefaultLineTypeForPattern(aLinePattern);
        aChangeSet->ToSetTypeOfFaceBoundaryLine = 1;
      }
      else
      {
        aChangeSet->StippleLinePattern = aLinePattern;
        aChangeSet->ToSetTypeOfLine    = 1;
      }
    }
    else if (anArg == "-unsetlinetype" || anArg == "-unsetedgetype" || anArg == "-unsetedgestype")
    {
      if (anArg == "-unsetedgetype" || anArg == "-unsetedgestype")
      {
        aChangeSet->ToSetTypeOfEdge = -1;
      }
      else
      {
        aChangeSet->ToSetTypeOfLine = -1;
      }
    }
    else if (anArg == "-setstipplelinefactor" || anArg == "-setstipplefactor"
             || anArg == "-setlinefactor" || anArg == "-stipplelinefactor"
             || anArg == "-stipplefactor" || anArg == "-linefactor")
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
      aChangeSet->StippleLineFactor = (uint16_t)Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (anArg == "-setmarkertype" || anArg == "-markertype" || anArg == "-setpointtype"
             || anArg == "-pointtype")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      if (!ViewerTest::ParseMarkerType(theArgVec[anArgIter],
                                       aChangeSet->TypeOfMarker,
                                       aChangeSet->MarkerImage))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }

      aChangeSet->ToSetTypeOfMarker = 1;
    }
    else if (anArg == "-unsetmarkertype" || anArg == "-unsetpointtype")
    {
      aChangeSet->ToSetTypeOfMarker = -1;
    }
    else if (anArg == "-setmarkersize" || anArg == "-markersize" || anArg == "-setpointsize"
             || anArg == "-pointsize")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetMarkerSize = 1;
      aChangeSet->MarkerSize      = Draw::Atof(theArgVec[anArgIter]);
    }
    else if (anArg == "-unsetmarkersize" || anArg == "-unsetpointsize")
    {
      aChangeSet->ToSetMarkerSize = -1;
      aChangeSet->MarkerSize      = 1.0;
    }
    else if (anArg == "-unsetcolor")
    {
      aChangeSet->ToSetColor = -1;
      aChangeSet->Color      = DEFAULT_COLOR;
    }
    else if (anArg == "-setmat" || anArg == "-mat" || anArg == "-setmaterial"
             || anArg == "-material")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetMaterial = 1;
      aChangeSet->MatName       = theArgVec[anArgIter];
      if (!Graphic3d_MaterialAspect::MaterialFromName(aChangeSet->MatName.ToCString(),
                                                      aChangeSet->Material))
      {
        Message::SendFail() << "Syntax error: unknown material '" << aChangeSet->MatName << "'.";
        return 1;
      }
    }
    else if (anArg == "-unsetmat" || anArg == "-unsetmaterial")
    {
      aChangeSet->ToSetMaterial = -1;
      aChangeSet->Material      = Graphic3d_NameOfMaterial_DEFAULT;
    }
    else if (anArg == "-subshape" || anArg == "-subshapes")
    {
      if (isDefaults)
      {
        Message::SendFail()
          << "Error: wrong syntax. -subshapes can not be used together with -defaults call!";
        return 1;
      }

      if (aNames.IsEmpty())
      {
        Message::SendFail()
          << "Error: main objects should specified explicitly when -subshapes is used!";
        return 1;
      }

      aChanges.Append(ViewerTest_AspectsChangeSet());
      aChangeSet = &aChanges.ChangeLast();

      for (++anArgIter; anArgIter < theArgNb; ++anArgIter)
      {
        const char* aSubShapeName = theArgVec[anArgIter];
        if (*aSubShapeName == '-')
        {
          --anArgIter;
          break;
        }

        TopoDS_Shape aSubShape = DBRep::Get(aSubShapeName);
        if (aSubShape.IsNull())
        {
          Message::SendFail() << "Error: shape " << aSubShapeName << " doesn't found!";
          return 1;
        }
        aChangeSet->SubShapes.Append(aSubShape);
      }

      if (aChangeSet->SubShapes.IsEmpty())
      {
        Message::SendFail() << "Error: empty list is specified after -subshapes!";
        return 1;
      }
    }
    else if (anArg == "-setfreeboundary" || anArg == "-freeboundary" || anArg == "-setfb"
             || anArg == "-fb")
    {
      bool toEnable = true;
      if (!Draw::ParseOnOff(anArgIter + 1 < theArgNb ? theArgVec[anArgIter + 1] : "", toEnable))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      ++anArgIter;
      aChangeSet->ToSetShowFreeBoundary = toEnable ? 1 : -1;
    }
    else if (anArg == "-setfreeboundarywidth" || anArg == "-freeboundarywidth"
             || anArg == "-setfbwidth" || anArg == "-fbwidth")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetFreeBoundaryWidth = 1;
      aChangeSet->FreeBoundaryWidth      = Draw::Atof(theArgVec[anArgIter]);
    }
    else if (anArg == "-unsetfreeboundarywidth" || anArg == "-unsetfbwidth")
    {
      aChangeSet->ToSetFreeBoundaryWidth = -1;
      aChangeSet->FreeBoundaryWidth      = 1.0;
    }
    else if (anArg == "-setfreeboundarycolor" || anArg == "-freeboundarycolor"
             || anArg == "-setfbcolor" || anArg == "-fbcolor")
    {
      int aNbParsed = Draw::ParseColor(theArgNb - anArgIter - 1,
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
    else if (anArg == "-unsetfreeboundarycolor" || anArg == "-unsetfbcolor")
    {
      aChangeSet->ToSetFreeBoundaryColor = -1;
      aChangeSet->FreeBoundaryColor      = DEFAULT_FREEBOUNDARY_COLOR;
    }
    else if (anArg == "-setisoontriangulation" || anArg == "-isoontriangulation"
             || anArg == "-setisoontriang" || anArg == "-isoontriang")
    {
      bool toEnable = true;
      if (!Draw::ParseOnOff(anArgIter + 1 < theArgNb ? theArgVec[anArgIter + 1] : "", toEnable))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      ++anArgIter;
      aChangeSet->ToEnableIsoOnTriangulation = toEnable ? 1 : -1;
    }
    else if (anArg == "-setfaceboundarydraw" || anArg == "-setdrawfaceboundary"
             || anArg == "-setdrawfaceboundaries" || anArg == "-setshowfaceboundary"
             || anArg == "-setshowfaceboundaries" || anArg == "-setdrawfaceedges"
             || anArg == "-faceboundarydraw" || anArg == "-drawfaceboundary"
             || anArg == "-drawfaceboundaries" || anArg == "-showfaceboundary"
             || anArg == "-showfaceboundaries" || anArg == "-drawfaceedges"
             || anArg == "-faceboundary" || anArg == "-faceboundaries" || anArg == "-faceedges")
    {
      bool toEnable = true;
      if (!Draw::ParseOnOff(anArgIter + 1 < theArgNb ? theArgVec[anArgIter + 1] : "", toEnable))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      ++anArgIter;
      aChangeSet->ToSetFaceBoundaryDraw = toEnable ? 1 : -1;
    }
    else if (anArg == "-unsetfaceboundary" || anArg == "-unsetboundary")
    {
      aChangeSet->ToSetFaceBoundaryDraw  = -1;
      aChangeSet->ToSetFaceBoundaryColor = -1;
    }
    else if (anArg == "-setmostcontinuity" || anArg == "-mostcontinuity")
    {
      TCollection_AsciiString aClassArg(anArgIter + 1 < theArgNb ? theArgVec[anArgIter + 1] : "");
      aClassArg.LowerCase();
      GeomAbs_Shape aClass = GeomAbs_CN;
      if (aClassArg == "c0" || aClassArg == "0")
      {
        aClass = GeomAbs_C0;
      }
      else if (aClassArg == "g1")
      {
        aClass = GeomAbs_G1;
      }
      else if (aClassArg == "c1" || aClassArg == "1")
      {
        aClass = GeomAbs_C1;
      }
      else if (aClassArg == "g2")
      {
        aClass = GeomAbs_G2;
      }
      else if (aClassArg == "c2" || aClassArg == "2")
      {
        aClass = GeomAbs_C2;
      }
      else if (aClassArg == "c3" || aClassArg == "3")
      {
        aClass = GeomAbs_C3;
      }
      else if (aClassArg == "cn" || aClassArg == "n")
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
      aChangeSet->FaceBoundaryUpperContinuity      = aClass;
    }
    else if (anArg == "-setmaxparamvalue" || anArg == "-maxparamvalue")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetMaxParamValue = 1;
      aChangeSet->MaxParamValue      = Draw::Atof(theArgVec[anArgIter]);
    }
    else if (anArg == "-setsensitivity" || anArg == "-sensitivity")
    {
      if (isDefaults)
      {
        Message::SendFail()
          << "Error: wrong syntax. -setSensitivity can not be used together with -defaults call!";
        return 1;
      }

      if (aNames.IsEmpty())
      {
        Message::SendFail() << "Error: object and selection mode should specified explicitly when "
                               "-setSensitivity is used!";
        return 1;
      }

      if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetSensitivity = 1;
      aChangeSet->SelectionMode    = Draw::Atoi(theArgVec[++anArgIter]);
      aChangeSet->Sensitivity      = Draw::Atoi(theArgVec[++anArgIter]);
    }
    else if (anArg == "-sethatch" || anArg == "-hatch")
    {
      if (isDefaults)
      {
        Message::SendFail()
          << "Error: wrong syntax. -setHatch can not be used together with -defaults call!";
        return 1;
      }

      if (aNames.IsEmpty())
      {
        Message::SendFail()
          << "Error: object should be specified explicitly when -setHatch is used!";
        return 1;
      }

      aChangeSet->ToSetHatch = 1;
      TCollection_AsciiString anArgHatch(theArgVec[++anArgIter]);
      if (anArgHatch.Length() <= 2)
      {
        const int anIntStyle = Draw::Atoi(anArgHatch.ToCString());
        if (anIntStyle < 0 || anIntStyle >= Aspect_HS_NB)
        {
          Message::SendFail() << "Error: hatch style is out of range [0, " << (Aspect_HS_NB - 1)
                              << "]!";
          return 1;
        }
        aChangeSet->StdHatchStyle = anIntStyle;
      }
      else
      {
        aChangeSet->PathToHatchPattern = anArgHatch;
      }
    }
    else if (anArg == "-setshadingmodel" || anArg == "-setshading" || anArg == "-shadingmodel"
             || anArg == "-shading")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetShadingModel = 1;
      aChangeSet->ShadingModelName  = theArgVec[anArgIter];
      if (!ViewerTest::ParseShadingModel(theArgVec[anArgIter], aChangeSet->ShadingModel))
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
    }
    else if (anArg == "-unsetshadingmodel")
    {
      aChangeSet->ToSetShadingModel = -1;
      aChangeSet->ShadingModel      = Graphic3d_TypeOfShadingModel_DEFAULT;
    }
    else if (anArg == "-setinterior" || anArg == "-setinteriorstyle" || anArg == "-interior"
             || anArg == "-interiorstyle")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aChangeSet->ToSetInterior = 1;
      if (!parseInteriorStyle(theArgVec[anArgIter], aChangeSet->InteriorStyle))
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
    else if (anArg == "-setdrawoutline" || anArg == "-setdrawsilhouette" || anArg == "-setoutline"
             || anArg == "-setsilhouette" || anArg == "-outline" || anArg == "-outlined"
             || anArg == "-drawsilhouette" || anArg == "-silhouette")
    {
      bool toDrawOutline = true;
      if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toDrawOutline))
      {
        ++anArgIter;
      }
      aChangeSet->ToSetDrawSilhouette = toDrawOutline ? 1 : -1;
    }
    else if (anArg == "-setdrawedges" || anArg == "-setdrawedge" || anArg == "-drawedges"
             || anArg == "-drawedge" || anArg == "-edges")
    {
      bool toDrawEdges = true;
      if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toDrawEdges))
      {
        ++anArgIter;
      }
      aChangeSet->ToSetDrawEdges = toDrawEdges ? 1 : -1;
    }
    else if (anArg == "-setquadedges" || anArg == "-setquads" || anArg == "-quads"
             || anArg == "-skipfirstedge")
    {
      bool isQuadMode = true;
      if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], isQuadMode))
      {
        ++anArgIter;
      }
      aChangeSet->ToSetQuadEdges = isQuadMode ? 1 : -1;
    }
    else if (anArg == "-setedgecolor" || anArg == "-setedgescolor" || anArg == "-edgecolor"
             || anArg == "-edgescolor")
    {
      int aNbParsed = Draw::ParseColor(theArgNb - anArgIter - 1,
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
      aChangeSet->ToSetVisibility    = 1;
      aChangeSet->Visibility         = 1;
      aChangeSet->ToSetLineWidth     = -1;
      aChangeSet->LineWidth          = 1.0;
      aChangeSet->ToSetTypeOfLine    = -1;
      aChangeSet->StippleLinePattern = 0xFFFF;
      aChangeSet->StippleLineFactor  = 1;
      aChangeSet->ToSetTypeOfMarker  = -1;
      aChangeSet->TypeOfMarker       = Aspect_TOM_PLUS;
      aChangeSet->ToSetMarkerSize    = -1;
      aChangeSet->MarkerSize         = 1.0;
      aChangeSet->ToSetTransparency  = -1;
      aChangeSet->Transparency       = 0.0;
      aChangeSet->ToSetAlphaMode     = -1;
      aChangeSet->AlphaMode          = Graphic3d_AlphaMode_BlendAuto;
      aChangeSet->AlphaCutoff        = 0.5f;
      aChangeSet->ToSetFaceCulling   = -1;
      aChangeSet->FaceCulling        = Graphic3d_TypeOfBackfacingModel_Auto;
      aChangeSet->ToSetColor         = -1;
      aChangeSet->Color              = DEFAULT_COLOR;
      // aChangeSet->ToSetBackFaceColor = -1; // should be reset by ToSetColor
      // aChangeSet->BackFaceColor = DEFAULT_COLOR;
      aChangeSet->ToSetMaterial              = -1;
      aChangeSet->Material                   = Graphic3d_NameOfMaterial_DEFAULT;
      aChangeSet->ToSetShowFreeBoundary      = -1;
      aChangeSet->ToSetFreeBoundaryColor     = -1;
      aChangeSet->FreeBoundaryColor          = DEFAULT_FREEBOUNDARY_COLOR;
      aChangeSet->ToSetFreeBoundaryWidth     = -1;
      aChangeSet->FreeBoundaryWidth          = 1.0;
      aChangeSet->ToEnableIsoOnTriangulation = -1;
      //
      aChangeSet->ToSetFaceBoundaryDraw            = -1;
      aChangeSet->ToSetFaceBoundaryUpperContinuity = -1;
      aChangeSet->FaceBoundaryUpperContinuity      = GeomAbs_CN;
      aChangeSet->ToSetFaceBoundaryColor           = -1;
      aChangeSet->FaceBoundaryColor                = Quantity_NOC_BLACK;
      aChangeSet->ToSetFaceBoundaryWidth           = -1;
      aChangeSet->FaceBoundaryWidth                = 1.0f;
      aChangeSet->ToSetTypeOfFaceBoundaryLine      = -1;
      aChangeSet->TypeOfFaceBoundaryLine           = Aspect_TOL_SOLID;
      //
      aChangeSet->ToSetHatch    = -1;
      aChangeSet->StdHatchStyle = -1;
      aChangeSet->PathToHatchPattern.Clear();
      aChangeSet->ToSetShadingModel   = -1;
      aChangeSet->ShadingModel        = Graphic3d_TypeOfShadingModel_DEFAULT;
      aChangeSet->ToSetInterior       = -1;
      aChangeSet->InteriorStyle       = Aspect_IS_SOLID;
      aChangeSet->ToSetDrawSilhouette = -1;
      aChangeSet->ToSetDrawEdges      = -1;
      aChangeSet->ToSetQuadEdges      = -1;
      aChangeSet->ToSetEdgeColor      = -1;
      aChangeSet->EdgeColor           = Quantity_ColorRGBA(DEFAULT_COLOR);
      aChangeSet->ToSetEdgeWidth      = -1;
      aChangeSet->EdgeWidth           = 1.0;
      aChangeSet->ToSetTypeOfEdge     = -1;
      aChangeSet->TypeOfEdge          = Aspect_TOL_SOLID;
    }
    else if (anArg == "-dumpjson")
    {
      toDump = true;
    }
    else if (anArg == "-dumpcompact")
    {
      toCompactDump = false;
      if (++anArgIter >= theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toCompactDump))
        ++anArgIter;
    }
    else if (anArg == "-dumpdepth")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << anArg;
        return 1;
      }
      aDumpDepth = Draw::Atoi(theArgVec[anArgIter]);
    }
    else
    {
      Message::SendFail() << "Error: wrong syntax at " << anArg;
      return 1;
    }
  }

  for (NCollection_Sequence<ViewerTest_AspectsChangeSet>::Iterator aChangesIter(aChanges);
       aChangesIter.More();
       aChangesIter.Next())
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
    const occ::handle<Prs3d_Drawer>& aDrawer = aCtx->DefaultDrawer();
    aChangeSet->Apply(aDrawer);
    if (aChangeSet->ToSetLineWidth != 0)
    {
      aDrawer->LineAspect()->SetWidth(aChangeSet->LineWidth);
      aDrawer->WireAspect()->SetWidth(aChangeSet->LineWidth);
      aDrawer->UnFreeBoundaryAspect()->SetWidth(aChangeSet->LineWidth);
      aDrawer->SeenLineAspect()->SetWidth(aChangeSet->LineWidth);
    }
    if (aChangeSet->ToSetColor != 0)
    {
      aDrawer->ShadingAspect()->SetColor(aChangeSet->Color);
      aDrawer->LineAspect()->SetColor(aChangeSet->Color);
      aDrawer->UnFreeBoundaryAspect()->SetColor(aChangeSet->Color);
      aDrawer->SeenLineAspect()->SetColor(aChangeSet->Color);
      aDrawer->WireAspect()->SetColor(aChangeSet->Color);
      aDrawer->PointAspect()->SetColor(aChangeSet->Color);
    }
    if (aChangeSet->ToSetTransparency != 0)
    {
      aDrawer->ShadingAspect()->SetTransparency(aChangeSet->Transparency);
    }
    if (aChangeSet->ToSetMaterial != 0)
    {
      aDrawer->ShadingAspect()->SetMaterial(aChangeSet->Material);
    }
    if (aChangeSet->ToEnableIsoOnTriangulation != 0)
    {
      aDrawer->SetIsoOnTriangulation(aChangeSet->ToEnableIsoOnTriangulation == 1);
    }

    // redisplay all objects in context
    for (ViewTest_PrsIter aPrsIter(aNames); aPrsIter.More(); aPrsIter.Next())
    {
      const occ::handle<AIS_InteractiveObject>& aPrs = aPrsIter.Current();
      if (!aPrs.IsNull())
      {
        aCtx->Redisplay(aPrs, false);
      }
    }
    if (toDump)
    {
      Standard_SStream aStream;
      aDrawer->DumpJson(aStream, aDumpDepth);

      if (toCompactDump)
        theDI << Standard_Dump::Text(aStream);
      else
        theDI << Standard_Dump::FormatJson(aStream);
    }
    return 0;
  }

  for (ViewTest_PrsIter aPrsIter(aNames); aPrsIter.More(); aPrsIter.Next())
  {
    const TCollection_AsciiString&     aName = aPrsIter.CurrentName();
    occ::handle<AIS_InteractiveObject> aPrs  = aPrsIter.Current();
    if (aPrs.IsNull())
    {
      return 1;
    }

    occ::handle<Prs3d_Drawer>     aDrawer = aPrs->Attributes();
    occ::handle<AIS_ColoredShape> aColoredPrs;
    bool                          toDisplay   = false;
    bool                          toRedisplay = false;
    if (aChanges.Length() > 1 || aChangeSet->ToSetVisibility == 1)
    {
      occ::handle<AIS_Shape> aShapePrs = occ::down_cast<AIS_Shape>(aPrs);
      if (aShapePrs.IsNull())
      {
        Message::SendFail() << "Error: an object " << aName << " is not an AIS_Shape presentation!";
        return 1;
      }
      aColoredPrs = occ::down_cast<AIS_ColoredShape>(aShapePrs);
      if (aColoredPrs.IsNull())
      {
        aColoredPrs = new AIS_ColoredShape(aShapePrs);
        if (aShapePrs->HasDisplayMode())
        {
          aColoredPrs->SetDisplayMode(aShapePrs->DisplayMode());
        }
        aColoredPrs->SetLocalTransformation(aShapePrs->LocalTransformation());
        aCtx->Remove(aShapePrs, false);
        GetMapOfAIS().UnBind2(aName);
        GetMapOfAIS().Bind(aColoredPrs, aName);
        toDisplay = true;
        aShapePrs = aColoredPrs;
        aPrs      = aColoredPrs;
      }
    }

    if (!aPrs.IsNull())
    {
      NCollection_Sequence<ViewerTest_AspectsChangeSet>::Iterator aChangesIter(aChanges);
      aChangeSet = &aChangesIter.ChangeValue();
      if (aChangeSet->ToSetVisibility == 1)
      {
        occ::handle<AIS_ColoredDrawer> aColDrawer =
          aColoredPrs->CustomAspects(aColoredPrs->Shape());
        aColDrawer->SetHidden(aChangeSet->Visibility == 0);
      }
      else if (aChangeSet->ToSetMaterial == 1)
      {
        aCtx->SetMaterial(aPrs, aChangeSet->Material, false);
      }
      else if (aChangeSet->ToSetMaterial == -1)
      {
        aCtx->UnsetMaterial(aPrs, false);
      }
      if (aChangeSet->ToSetColor == 1)
      {
        aCtx->SetColor(aPrs, aChangeSet->Color, false);
      }
      else if (aChangeSet->ToSetColor == -1)
      {
        aCtx->UnsetColor(aPrs, false);
      }
      if (aChangeSet->ToSetTransparency == 1)
      {
        aCtx->SetTransparency(aPrs, aChangeSet->Transparency, false);
      }
      else if (aChangeSet->ToSetTransparency == -1)
      {
        aCtx->UnsetTransparency(aPrs, false);
      }
      if (aChangeSet->ToSetLineWidth == 1)
      {
        aCtx->SetWidth(aPrs, aChangeSet->LineWidth, false);
      }
      else if (aChangeSet->ToSetLineWidth == -1)
      {
        aCtx->UnsetWidth(aPrs, false);
      }
      else if (aChangeSet->ToEnableIsoOnTriangulation != 0)
      {
        aCtx->IsoOnTriangulation(aChangeSet->ToEnableIsoOnTriangulation == 1, aPrs);
        toRedisplay = true;
      }
      else if (aChangeSet->ToSetSensitivity != 0)
      {
        aCtx->SetSelectionSensitivity(aPrs, aChangeSet->SelectionMode, aChangeSet->Sensitivity);
      }
      if (!aDrawer.IsNull())
      {
        toRedisplay = aChangeSet->Apply(aDrawer) || toRedisplay;
      }

      for (aChangesIter.Next(); aChangesIter.More(); aChangesIter.Next())
      {
        aChangeSet = &aChangesIter.ChangeValue();
        for (NCollection_Sequence<TopoDS_Shape>::Iterator aSubShapeIter(aChangeSet->SubShapes);
             aSubShapeIter.More();
             aSubShapeIter.Next())
        {
          const TopoDS_Shape& aSubShape = aSubShapeIter.Value();
          if (!aChangeSet->IsEmpty())
          {
            occ::handle<AIS_ColoredDrawer> aCurColDrawer = aColoredPrs->CustomAspects(aSubShape);
            aChangeSet->Apply(aCurColDrawer);
          }
          if (aChangeSet->ToSetVisibility == 1)
          {
            occ::handle<AIS_ColoredDrawer> aCurColDrawer = aColoredPrs->CustomAspects(aSubShape);
            aCurColDrawer->SetHidden(aChangeSet->Visibility == 0);
          }
          if (aChangeSet->ToSetColor == 1)
          {
            aColoredPrs->SetCustomColor(aSubShape, aChangeSet->Color);
          }
          if (aChangeSet->ToSetTransparency == 1)
          {
            aColoredPrs->SetCustomTransparency(aSubShape, aChangeSet->Transparency);
          }
          if (aChangeSet->ToSetLineWidth == 1)
          {
            aColoredPrs->SetCustomWidth(aSubShape, aChangeSet->LineWidth);
          }
          if (aChangeSet->ToSetColor == -1 || aChangeSet->ToSetLineWidth == -1)
          {
            aColoredPrs->UnsetCustomAspects(aSubShape, true);
          }
          if (aChangeSet->ToSetSensitivity != 0)
          {
            aCtx->SetSelectionSensitivity(aPrs, aChangeSet->SelectionMode, aChangeSet->Sensitivity);
          }
        }
      }
      if (toDisplay)
      {
        aCtx->Display(aPrs, false);
      }
      if (toRedisplay)
      {
        aCtx->Redisplay(aPrs, false);
      }
      else if (!aColoredPrs.IsNull())
      {
        aCtx->Redisplay(aColoredPrs, false);
      }
      else
      {
        aPrs->SynchronizeAspects();
      }

      if (toDump)
      {
        Standard_SStream aStream;
        aDrawer->DumpJson(aStream);

        theDI << aName << ": \n";
        theDI << Standard_Dump::FormatJson(aStream);
        theDI << "\n";
      }
    }
  }
  return 0;
}

//==============================================================================
// function : VDonly2
// author   : ege
// purpose  : Display only a selected or named  object
//           if there is no selected or named object s, nothing is done
//==============================================================================
static int VDonly2(Draw_Interpretor&, int theArgNb, const char** theArgVec)
{
  const occ::handle<AIS_InteractiveContext>& aCtx = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater                     anUpdateTool(aCtx, ViewerTest::CurrentView());
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active view!");
    return 1;
  }

  int anArgIter = 1;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    if (!anUpdateTool.parseRedrawMode(theArgVec[anArgIter]))
    {
      break;
    }
  }

  NCollection_Map<occ::handle<Standard_Transient>> aDispSet;
  if (anArgIter >= theArgNb)
  {
    // display only selected objects
    if (aCtx->NbSelected() < 1)
    {
      return 0;
    }

    for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
    {
      aDispSet.Add(aCtx->SelectedInteractive());
    }
  }
  else
  {
    // display only specified objects
    for (; anArgIter < theArgNb; ++anArgIter)
    {
      TCollection_AsciiString            aName = theArgVec[anArgIter];
      occ::handle<AIS_InteractiveObject> aShape;
      if (GetMapOfAIS().Find2(aName, aShape) && !aShape.IsNull())
      {
        aCtx->Display(aShape, false);
        aDispSet.Add(aShape);
      }
    }
  }

  // weed out other objects
  for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>::Iterator
         anIter(GetMapOfAIS());
       anIter.More();
       anIter.Next())
  {
    if (aDispSet.Contains(anIter.Key1()))
    {
      continue;
    }

    if (const occ::handle<AIS_InteractiveObject>& aShape = anIter.Key1())
    {
      aCtx->Erase(aShape, false);
    }
  }
  return 0;
}

//==============================================================================
// function : VRemove
// purpose  : Removes selected or named objects.
//           If there is no selected or named objects,
//           all objects in the viewer can be removed with argument -all.
//           If -context is in arguments, the object is not deleted from the map of
//           objects (deleted only from the current context).
//==============================================================================
int VRemove(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  const occ::handle<AIS_InteractiveContext>& aCtx = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater                     anUpdateTool(aCtx, ViewerTest::CurrentView());
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active view!");
    return 1;
  }

  bool isContextOnly = false;
  bool toRemoveAll   = false;
  bool toPrintInfo   = true;
  bool toFailOnError = true;

  int anArgIter = 1;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg = theArgVec[anArgIter];
    anArg.LowerCase();
    if (anArg == "-context")
    {
      isContextOnly = true;
    }
    else if (anArg == "-all")
    {
      toRemoveAll = true;
    }
    else if (anArg == "-noinfo")
    {
      toPrintInfo = false;
    }
    else if (anArg == "-noerror" || anArg == "-nofail")
    {
      toFailOnError = false;
    }
    else if (anUpdateTool.parseRedrawMode(anArg))
    {
      continue;
    }
    else
    {
      break;
    }
  }
  if (toRemoveAll && anArgIter < theArgNb)
  {
    Message::SendFail("Error: wrong syntax!");
    return 1;
  }

  NCollection_List<TCollection_AsciiString> anIONameList;
  if (toRemoveAll)
  {
    for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>,
                               TCollection_AsciiString>::Iterator anIter(GetMapOfAIS());
         anIter.More();
         anIter.Next())
    {
      anIONameList.Append(anIter.Key2());
    }
  }
  else if (anArgIter < theArgNb) // removed objects names are in argument list
  {
    for (; anArgIter < theArgNb; ++anArgIter)
    {
      const TCollection_AsciiString aName(theArgVec[anArgIter]);
      if (aName.Search("*") != -1)
      {
        for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>,
                                   TCollection_AsciiString>::Iterator aPrsIter(GetMapOfAIS());
             aPrsIter.More();
             aPrsIter.Next())
        {
          if (aPrsIter.Key1()->GetContext() != aCtx)
          {
            continue;
          }
          const TCollection_AsciiString aCheck =
            TCollection_AsciiString("string match '") + aName + "' '" + aPrsIter.Key2() + "'";
          if (theDI.Eval(aCheck.ToCString()) == 0 && *theDI.Result() == '1')
          {
            anIONameList.Append(aPrsIter.Key2());
          }
        }
        theDI.Reset();
        continue;
      }

      occ::handle<AIS_InteractiveObject> anIO;
      if (!GetMapOfAIS().Find2(aName, anIO))
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
          Message::SendFail() << "Syntax error: '" << aName
                              << "' was not displayed in current context.\n"
                              << "Please activate view with this object displayed and try again.";
          return 1;
        }
      }
      else
      {
        anIONameList.Append(aName);
      }
    }
  }
  else if (aCtx->NbSelected() > 0)
  {
    for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>,
                               TCollection_AsciiString>::Iterator anIter(GetMapOfAIS());
         anIter.More();
         anIter.Next())
    {
      if (!aCtx->IsSelected(anIter.Key1()))
      {
        continue;
      }

      anIONameList.Append(anIter.Key2());
      continue;
    }
  }

  // Unbind all removed objects from the map of displayed IO.
  for (NCollection_List<TCollection_AsciiString>::Iterator anIter(anIONameList); anIter.More();
       anIter.Next())
  {
    const occ::handle<AIS_InteractiveObject> anIO = GetMapOfAIS().Find2(anIter.Value());
    aCtx->Remove(anIO, false);
    if (toPrintInfo)
    {
      theDI << anIter.Value() << " ";
    }
    if (!isContextOnly)
    {
      GetMapOfAIS().UnBind2(anIter.Value());
    }
  }
  return 0;
}

//==============================================================================
// function : VErase
// purpose  : Erase some selected or named objects
//           if there is no selected or named objects, the whole viewer is erased
//==============================================================================
int VErase(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  const occ::handle<AIS_InteractiveContext>& aCtx  = ViewerTest::GetAISContext();
  const occ::handle<V3d_View>&               aView = ViewerTest::CurrentView();
  ViewerTest_AutoUpdater                     anUpdateTool(aCtx, aView);
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active view!");
    return 1;
  }

  const bool toEraseAll = TCollection_AsciiString(theArgNb > 0 ? theArgVec[0] : "") == "veraseall";

  int                                           anArgIter     = 1;
  bool                                          toEraseInView = false;
  bool                                          toFailOnError = true;
  NCollection_Sequence<TCollection_AsciiString> aNamesOfEraseIO;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anUpdateTool.parseRedrawMode(anArgCase))
    {
      continue;
    }
    else if (anArgCase == "-view" || anArgCase == "-inview")
    {
      toEraseInView = true;
    }
    else if (anArgCase == "-noerror" || anArgCase == "-nofail")
    {
      toFailOnError = false;
    }
    else
    {
      aNamesOfEraseIO.Append(theArgVec[anArgIter]);
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
    NCollection_IndexedDataMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>
      aPrsList;
    for (NCollection_Sequence<TCollection_AsciiString>::Iterator anIter(aNamesOfEraseIO);
         anIter.More();
         anIter.Next())
    {
      const TCollection_AsciiString& aName = anIter.Value();
      if (aName.Search("*") != -1)
      {
        for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>,
                                   TCollection_AsciiString>::Iterator aPrsIter(GetMapOfAIS());
             aPrsIter.More();
             aPrsIter.Next())
        {
          const TCollection_AsciiString aCheck =
            TCollection_AsciiString("string match '") + aName + "' '" + aPrsIter.Key2() + "'";
          if (theDI.Eval(aCheck.ToCString()) == 0 && *theDI.Result() == '1')
          {
            aPrsList.Add(aPrsIter.Key1(), aPrsIter.Key2());
          }
        }
        theDI.Reset();
      }
      else
      {
        occ::handle<AIS_InteractiveObject> anIO;
        if (!GetMapOfAIS().Find2(aName, anIO))
        {
          if (toFailOnError)
          {
            Message::SendFail() << "Syntax error: '" << aName << "' is not found";
            return 1;
          }
        }
        else
        {
          aPrsList.Add(anIO, aName);
        }
      }
    }

    for (NCollection_IndexedDataMap<occ::handle<AIS_InteractiveObject>,
                                    TCollection_AsciiString>::Iterator anIter(aPrsList);
         anIter.More();
         anIter.Next())
    {
      theDI << anIter.Value() << " ";
      if (toEraseInView)
      {
        aCtx->SetViewAffinity(anIter.Key(), aView, false);
      }
      else
      {
        aCtx->Erase(anIter.Key(), false);
      }
    }
  }
  else if (!toEraseAll && aCtx->NbSelected() > 0)
  {
    // Erase selected objects
    for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>,
                               TCollection_AsciiString>::Iterator anIter(GetMapOfAIS());
         anIter.More();
         anIter.Next())
    {
      const occ::handle<AIS_InteractiveObject>& anIO = anIter.Key1();
      if (!anIO.IsNull() && aCtx->IsSelected(anIO))
      {
        theDI << anIter.Key2() << " ";
        if (toEraseInView)
        {
          aCtx->SetViewAffinity(anIO, aView, false);
        }
      }
    }

    if (!toEraseInView)
    {
      aCtx->EraseSelected(false);
    }
  }
  else
  {
    // Erase all objects
    for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>,
                               TCollection_AsciiString>::Iterator anIter(GetMapOfAIS());
         anIter.More();
         anIter.Next())
    {
      const occ::handle<AIS_InteractiveObject>& anIO = anIter.Key1();
      if (!anIO.IsNull())
      {
        if (toEraseInView)
        {
          aCtx->SetViewAffinity(anIO, aView, false);
        }
        else
        {
          aCtx->Erase(anIO, false);
        }
      }
    }
  }

  return 0;
}

//==============================================================================
// function : VDisplayAll
// purpose  : Display all the objects of the Map
//==============================================================================
static int VDisplayAll(Draw_Interpretor&, int theArgNb, const char** theArgVec)

{
  const occ::handle<AIS_InteractiveContext>& aCtx = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater                     anUpdateTool(aCtx, ViewerTest::CurrentView());
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active view!");
    return 1;
  }

  int anArgIter = 1;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anUpdateTool.parseRedrawMode(anArgCase))
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

  for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>::Iterator
         anIter(GetMapOfAIS());
       anIter.More();
       anIter.Next())
  {
    aCtx->Erase(anIter.Key1(), false);
  }

  for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>::Iterator
         anIter(GetMapOfAIS());
       anIter.More();
       anIter.Next())
  {
    aCtx->Display(anIter.Key1(), false);
  }
  return 0;
}

//! Auxiliary method to check if presentation exists
inline int checkMode(const occ::handle<AIS_InteractiveContext>& theCtx,
                     const occ::handle<AIS_InteractiveObject>&  theIO,
                     const int                                  theMode)
{
  if (theIO.IsNull() || theCtx.IsNull())
  {
    return -1;
  }

  if (theMode != -1)
  {
    if (theCtx->MainPrsMgr()->HasPresentation(theIO, theMode))
    {
      return theMode;
    }
  }
  else if (theCtx->MainPrsMgr()->HasPresentation(theIO, theIO->DisplayMode()))
  {
    return theIO->DisplayMode();
  }
  else if (theCtx->MainPrsMgr()->HasPresentation(theIO, theCtx->DisplayMode()))
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
inline void bndPresentation(Draw_Interpretor&                              theDI,
                            const occ::handle<PrsMgr_PresentationManager>& theMgr,
                            const occ::handle<AIS_InteractiveObject>&      theObj,
                            const int                                      theDispMode,
                            const TCollection_AsciiString&                 theName,
                            const ViewerTest_BndAction                     theAction,
                            const occ::handle<Prs3d_Drawer>&               theStyle)
{
  switch (theAction)
  {
    case BndAction_Hide: {
      theMgr->Unhighlight(theObj);
      break;
    }
    case BndAction_Show: {
      theMgr->Color(theObj, theStyle, theDispMode);
      break;
    }
    case BndAction_Print: {
      Bnd_Box aBox;
      for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(
             theObj->Presentations());
           aPrsIter.More();
           aPrsIter.Next())
      {
        if (aPrsIter.Value()->Mode() != theDispMode)
          continue;

        aBox = aPrsIter.Value()->MinMaxValues();
      }
      gp_Pnt aMin = aBox.CornerMin();
      gp_Pnt aMax = aBox.CornerMax();
      theDI << theName << "\n"
            << aMin.X() << " " << aMin.Y() << " " << aMin.Z() << " " << aMax.X() << " " << aMax.Y()
            << " " << aMax.Z() << "\n";
      break;
    }
  }
}

//=================================================================================================

int VBounding(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  occ::handle<AIS_InteractiveContext> aCtx = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater              anUpdateTool(aCtx, ViewerTest::CurrentView());
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active view!");
    return 1;
  }

  ViewerTest_BndAction anAction = BndAction_Show;
  int                  aMode    = -1;

  occ::handle<Prs3d_Drawer> aStyle;

  int anArgIter = 1;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
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
      aMode = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (!anUpdateTool.parseRedrawMode(anArg))
    {
      break;
    }
  }

  if (anAction == BndAction_Show)
  {
    aStyle = new Prs3d_Drawer();
    aStyle->SetMethod(Aspect_TOHM_BOUNDBOX);
    aStyle->SetColor(Quantity_NOC_GRAY99);
  }

  int aHighlightedMode = -1;
  if (anArgIter < theArgNb)
  {
    // has a list of names
    for (; anArgIter < theArgNb; ++anArgIter)
    {
      TCollection_AsciiString            aName = theArgVec[anArgIter];
      occ::handle<AIS_InteractiveObject> anIO;
      if (!GetMapOfAIS().Find2(aName, anIO))
      {
        Message::SendFail() << "Error: presentation " << aName << " does not exist";
        return 1;
      }

      aHighlightedMode = checkMode(aCtx, anIO, aMode);
      if (aHighlightedMode == -1)
      {
        Message::SendFail() << "Error: object " << aName << " has no presentation with mode "
                            << aMode;
        return 1;
      }
      bndPresentation(theDI, aCtx->MainPrsMgr(), anIO, aHighlightedMode, aName, anAction, aStyle);
    }
  }
  else if (aCtx->NbSelected() > 0)
  {
    // remove all currently selected objects
    for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
    {
      occ::handle<AIS_InteractiveObject> anIO = aCtx->SelectedInteractive();
      aHighlightedMode                        = checkMode(aCtx, anIO, aMode);
      if (aHighlightedMode != -1)
      {
        bndPresentation(theDI,
                        aCtx->MainPrsMgr(),
                        anIO,
                        aHighlightedMode,
                        GetMapOfAIS().IsBound1(anIO) ? GetMapOfAIS().Find1(anIO) : "",
                        anAction,
                        aStyle);
      }
    }
  }
  else
  {
    // all objects
    for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>,
                               TCollection_AsciiString>::Iterator anIter(GetMapOfAIS());
         anIter.More();
         anIter.Next())
    {
      const occ::handle<AIS_InteractiveObject>& anIO = anIter.Key1();
      aHighlightedMode                               = checkMode(aCtx, anIO, aMode);
      if (aHighlightedMode != -1)
      {
        bndPresentation(theDI,
                        aCtx->MainPrsMgr(),
                        anIO,
                        aHighlightedMode,
                        anIter.Key2(),
                        anAction,
                        aStyle);
      }
    }
  }
  return 0;
}

//=================================================================================================

int VTexture(Draw_Interpretor& theDi, int theArgsNb, const char** theArgVec)
{
  const occ::handle<AIS_InteractiveContext>& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail() << "Error: no active view!";
    return 1;
  }

  int  toModulate = -1, toSetSRgb = -1;
  bool toSetFilter = false, toSetAniso = false, toSetTrsfAngle = false, toSetTrsfTrans = false,
       toSetTrsfScale                              = false;
  float                              aTrsfRotAngle = 0.0f;
  NCollection_Vec2<float>            aTrsfTrans(0.0f, 0.0f);
  NCollection_Vec2<float>            aTrsfScale(1.0f, 1.0f);
  Graphic3d_TypeOfTextureFilter      aFilter       = Graphic3d_TOTF_NEAREST;
  Graphic3d_LevelOfTextureAnisotropy anAnisoFilter = Graphic3d_LOTA_OFF;

  occ::handle<AIS_InteractiveObject>                    aTexturedIO;
  occ::handle<AIS_Shape>                                aTexturedShape;
  occ::handle<Graphic3d_TextureSet>                     aTextureSetOld;
  NCollection_Vector<occ::handle<Graphic3d_TextureMap>> aTextureVecNew;
  bool toSetGenRepeat = false, toSetGenScale = false, toSetGenOrigin = false, toSetImage = false,
       toComputeUV = false;

  const TCollection_AsciiString aCommandName(theArgVec[0]);
  bool                          toSetDefaults = aCommandName == "vtexdefault";

  ViewerTest_AutoUpdater anUpdateTool(aCtx, ViewerTest::CurrentView());
  for (int anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    const TCollection_AsciiString aName     = theArgVec[anArgIter];
    TCollection_AsciiString       aNameCase = aName;
    aNameCase.LowerCase();
    if (anUpdateTool.parseRedrawMode(aName))
    {
      continue;
    }
    else if (aTexturedIO.IsNull())
    {
      const NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>&
        aMapOfIO = GetMapOfAIS();
      if (aMapOfIO.IsBound2(aName))
      {
        aTexturedIO    = aMapOfIO.Find2(aName);
        aTexturedShape = occ::down_cast<AIS_Shape>(aTexturedIO);
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
             && (aNameCase == "-scale" || aNameCase == "-setscale" || aCommandName == "vtexscale"))
    {
      if (aCommandName != "vtexscale")
      {
        ++anArgIter;
      }
      if (anArgIter < theArgsNb)
      {
        TCollection_AsciiString aValU(theArgVec[anArgIter]);
        TCollection_AsciiString aValUCase = aValU;
        aValUCase.LowerCase();
        toSetGenScale = true;
        if (aValUCase == "off")
        {
          aTexturedShape->SetTextureScaleUV(gp_Pnt2d(1.0, 1.0));
          continue;
        }
        else if (anArgIter + 1 < theArgsNb)
        {
          TCollection_AsciiString aValV(theArgVec[anArgIter + 1]);
          if (aValU.IsRealValue(true) && aValV.IsRealValue(true))
          {
            aTexturedShape->SetTextureScaleUV(gp_Pnt2d(aValU.RealValue(), aValV.RealValue()));
            ++anArgIter;
            continue;
          }
        }
      }
      Message::SendFail() << "Syntax error: unexpected argument '" << aName << "'";
      return 1;
    }
    else if (!aTexturedShape.IsNull()
             && (aNameCase == "-origin" || aNameCase == "-setorigin"
                 || aCommandName == "vtexorigin"))
    {
      if (aCommandName != "vtexorigin")
      {
        ++anArgIter;
      }
      if (anArgIter < theArgsNb)
      {
        TCollection_AsciiString aValU(theArgVec[anArgIter]);
        TCollection_AsciiString aValUCase = aValU;
        aValUCase.LowerCase();
        toSetGenOrigin = true;
        if (aValUCase == "off")
        {
          aTexturedShape->SetTextureOriginUV(gp_Pnt2d(0.0, 0.0));
          continue;
        }
        else if (anArgIter + 1 < theArgsNb)
        {
          TCollection_AsciiString aValV(theArgVec[anArgIter + 1]);
          if (aValU.IsRealValue(true) && aValV.IsRealValue(true))
          {
            aTexturedShape->SetTextureOriginUV(gp_Pnt2d(aValU.RealValue(), aValV.RealValue()));
            ++anArgIter;
            continue;
          }
        }
      }
      Message::SendFail() << "Syntax error: unexpected argument '" << aName << "'";
      return 1;
    }
    else if (!aTexturedShape.IsNull()
             && (aNameCase == "-repeat" || aNameCase == "-setrepeat"
                 || aCommandName == "vtexrepeat"))
    {
      if (aCommandName != "vtexrepeat")
      {
        ++anArgIter;
      }
      if (anArgIter < theArgsNb)
      {
        TCollection_AsciiString aValU(theArgVec[anArgIter]);
        TCollection_AsciiString aValUCase = aValU;
        aValUCase.LowerCase();
        toSetGenRepeat = true;
        if (aValUCase == "off")
        {
          aTexturedShape->SetTextureRepeatUV(gp_Pnt2d(1.0, 1.0));
          continue;
        }
        else if (anArgIter + 1 < theArgsNb)
        {
          TCollection_AsciiString aValV(theArgVec[anArgIter + 1]);
          if (aValU.IsRealValue(true) && aValV.IsRealValue(true))
          {
            aTexturedShape->SetTextureRepeatUV(gp_Pnt2d(aValU.RealValue(), aValV.RealValue()));
            ++anArgIter;
            continue;
          }
        }
      }
      Message::SendFail() << "Syntax error: unexpected argument '" << aName << "'";
      return 1;
    }
    else if (aNameCase == "-modulate" || aNameCase == "-nomodulate")
    {
      toModulate = Draw::ParseOnOffNoIterator(theArgsNb, theArgVec, anArgIter) ? 1 : 0;
    }
    else if (aNameCase == "-srgb" || aNameCase == "-nosrgb")
    {
      toSetSRgb = Draw::ParseOnOffNoIterator(theArgsNb, theArgVec, anArgIter) ? 1 : 0;
    }
    else if ((aNameCase == "-setfilter" || aNameCase == "-filter") && anArgIter + 1 < theArgsNb)
    {
      TCollection_AsciiString aValue(theArgVec[anArgIter + 1]);
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
    else if ((aNameCase == "-setaniso" || aNameCase == "-setanisofilter" || aNameCase == "-aniso"
              || aNameCase == "-anisofilter")
             && anArgIter + 1 < theArgsNb)
    {
      TCollection_AsciiString aValue(theArgVec[anArgIter + 1]);
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
      else if (aValue == "quality" || aValue == "high")
      {
        anAnisoFilter = Graphic3d_LOTA_QUALITY;
      }
      else
      {
        Message::SendFail() << "Syntax error: unexpected argument '" << aValue << "'";
        return 1;
      }
    }
    else if ((aNameCase == "-rotateangle" || aNameCase == "-rotangle" || aNameCase == "-rotate"
              || aNameCase == "-angle" || aNameCase == "-trsfangle")
             && anArgIter + 1 < theArgsNb)
    {
      aTrsfRotAngle  = float(Draw::Atof(theArgVec[anArgIter + 1]));
      toSetTrsfAngle = true;
      ++anArgIter;
    }
    else if ((aNameCase == "-trsftrans" || aNameCase == "-trsftranslate"
              || aNameCase == "-translate" || aNameCase == "-translation")
             && anArgIter + 2 < theArgsNb)
    {
      aTrsfTrans.x() = float(Draw::Atof(theArgVec[anArgIter + 1]));
      aTrsfTrans.y() = float(Draw::Atof(theArgVec[anArgIter + 2]));
      toSetTrsfTrans = true;
      anArgIter += 2;
    }
    else if ((aNameCase == "-trsfscale") && anArgIter + 2 < theArgsNb)
    {
      aTrsfScale.x() = float(Draw::Atof(theArgVec[anArgIter + 1]));
      aTrsfScale.y() = float(Draw::Atof(theArgVec[anArgIter + 2]));
      toSetTrsfScale = true;
      anArgIter += 2;
    }
    else if (aNameCase == "-default" || aNameCase == "-defaults")
    {
      toSetDefaults = true;
    }
    else if ((aNameCase == "-video") && anArgIter + 1 < theArgsNb)
    {
      const TCollection_AsciiString          anInput(theArgVec[++anArgIter]);
      occ::handle<Graphic3d_MediaTextureSet> aMedia =
        occ::down_cast<Graphic3d_MediaTextureSet>(aTextureSetOld);
      if (aMedia.IsNull())
      {
        aMedia = new Graphic3d_MediaTextureSet();
      }
      if (aMedia->Input() != anInput)
      {
        aMedia->OpenInput(anInput, false);
      }
      else
      {
        if (aMedia->SwapFrames()
            && !aCtx->CurrentViewer()->ZLayerSettings(aTexturedIO->ZLayer()).IsImmediate())
        {
          ViewerTest::CurrentView()->Invalidate();
        }
      }
      if (aTexturedIO->Attributes()->SetupOwnShadingAspect(aCtx->DefaultDrawer())
          && aTexturedShape.IsNull())
      {
        aTexturedIO->SetToUpdate();
      }

      toComputeUV = aTextureSetOld.IsNull();
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->SetTextureMapOn(true);
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->SetTextureSet(aMedia);
      aTextureSetOld.Nullify();
    }
    else if (aCommandName == "vtexture" && aTextureVecNew.IsEmpty() && aNameCase == "-3d")
    {
      NCollection_Sequence<TCollection_AsciiString> aSlicesSeq;
      for (; anArgIter + 1 < theArgsNb; ++anArgIter)
      {
        TCollection_AsciiString aSlicePath(theArgVec[anArgIter + 1]);
        if (aSlicePath.StartsWith("-"))
        {
          break;
        }

        aSlicesSeq.Append(aSlicePath);
      }

      if (aSlicesSeq.Size() < 2)
      {
        Message::SendFail() << "Syntax error at '" << aNameCase << "'";
        return 1;
      }
      NCollection_Array1<TCollection_AsciiString> aSlices;
      aSlices.Resize(0, aSlicesSeq.Size() - 1, false);
      int aSliceIndex = 0;
      for (const TCollection_AsciiString& aSliceIter : aSlicesSeq)
      {
        aSlices[aSliceIndex++] = aSliceIter;
      }

      toSetImage = true;
      aTextureVecNew.SetValue(0, new Graphic3d_Texture3D(aSlices));
    }
    else if (aCommandName == "vtexture"
             && (aTextureVecNew.IsEmpty() || aNameCase.StartsWith("-tex")))
    {
      int                     aTexIndex = 0;
      TCollection_AsciiString aTexName  = aName;
      if (aNameCase.StartsWith("-tex"))
      {
        if (anArgIter + 1 >= theArgsNb || aNameCase.Length() < 5)
        {
          Message::SendFail() << "Syntax error: invalid argument '" << theArgVec[anArgIter] << "'";
          return 1;
        }

        TCollection_AsciiString aTexIndexStr = aNameCase.SubString(5, aNameCase.Length());
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
          || aTexIndex >= aCtx->CurrentViewer()->Driver()->InquireLimit(
               Graphic3d_TypeOfLimit_MaxCombinedTextureUnits))
      {
        Message::SendFail("Error: too many textures specified");
        return 1;
      }

      toSetImage = true;
      if (aTexName.IsIntegerValue())
      {
        const int aValue = aTexName.IntegerValue();
        if (aValue < 0 || aValue >= Graphic3d_Texture2D::NumberOfTextures())
        {
          Message::SendFail() << "Syntax error: texture with ID " << aValue << " is undefined!";
          return 1;
        }
        aTextureVecNew.SetValue(aTexIndex,
                                new Graphic3d_Texture2D(Graphic3d_NameOfTexture2D(aValue)));
      }
      else if (aTexName == "?")
      {
        const TCollection_AsciiString aTextureFolder = Graphic3d_TextureRoot::TexturesFolder();

        theDi << "\n Files in current directory : \n\n";
        theDi.Eval("glob -nocomplain *");

        TCollection_AsciiString aCmnd("glob -nocomplain ");
        aCmnd += aTextureFolder;
        aCmnd += "/* ";

        theDi << "Files in " << aTextureFolder << " : \n\n";
        theDi.Eval(aCmnd.ToCString());
        return 0;
      }
      else if (aTexName != "off")
      {
        if (!OSD_File(aTexName).Exists())
        {
          Message::SendFail() << "Syntax error: non-existing image file has been specified '"
                              << aTexName << "'.";
          return 1;
        }
        aTextureVecNew.SetValue(aTexIndex, new Graphic3d_Texture2D(aTexName));
      }
      else
      {
        aTextureVecNew.SetValue(aTexIndex, occ::handle<Graphic3d_TextureMap>());
      }

      if (aTextureVecNew.Value(aTexIndex))
      {
        aTextureVecNew.ChangeValue(aTexIndex)->GetParams()->SetTextureUnit(
          (Graphic3d_TextureUnit)aTexIndex);
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
    int                               aNbChanged = 0;
    occ::handle<Graphic3d_TextureSet> aTextureSetNew;
    if (!aTextureVecNew.IsEmpty())
    {
      aNbChanged     = aTextureVecNew.Size();
      aTextureSetNew = new Graphic3d_TextureSet(aTextureVecNew.Size());
      for (int aTexIter = 0; aTexIter < aTextureSetNew->Size(); ++aTexIter)
      {
        occ::handle<Graphic3d_TextureMap>& aTextureNew = aTextureVecNew.ChangeValue(aTexIter);
        occ::handle<Graphic3d_TextureRoot> aTextureOld;
        if (!aTextureSetOld.IsNull() && aTexIter < aTextureSetOld->Size())
        {
          aTextureOld = aTextureSetOld->Value(aTexIter);
        }

        if (!aTextureOld.IsNull() && !aTextureNew.IsNull())
        {
          *aTextureNew->GetParams() = *aTextureOld->GetParams();

          occ::handle<Graphic3d_Texture2D> aTex2dNew =
            occ::down_cast<Graphic3d_Texture2D>(aTextureNew);
          occ::handle<Graphic3d_Texture2D> aTex2dOld =
            occ::down_cast<Graphic3d_Texture2D>(aTextureOld);
          if (!aTex2dOld.IsNull() && !aTex2dNew.IsNull())
          {
            TCollection_AsciiString aFilePathOld, aFilePathNew;
            aTextureOld->Path().SystemName(aFilePathOld);
            aTextureNew->Path().SystemName(aFilePathNew);
            if (aTex2dNew->Name() == aTex2dOld->Name() && aFilePathOld == aFilePathNew
                && (!aFilePathNew.IsEmpty() || aTex2dNew->Name() != Graphic3d_NOT_2D_UNKNOWN))
            {
              --aNbChanged;
              aTextureNew = aTex2dOld;
            }
          }
        }
        aTextureSetNew->SetValue(aTexIter, aTextureNew);
      }
    }
    if (aNbChanged == 0
        && ((aTextureSetOld.IsNull() && aTextureSetNew.IsNull())
            || (aTextureSetOld->Size() == aTextureSetNew->Size())))
    {
      aTextureSetNew = aTextureSetOld;
    }

    if (aTexturedIO->Attributes()->SetupOwnShadingAspect(aCtx->DefaultDrawer())
        && aTexturedShape.IsNull())
    {
      aTexturedIO->SetToUpdate();
    }

    toComputeUV = !aTextureSetNew.IsNull() && aTextureSetOld.IsNull();
    aTexturedIO->Attributes()->ShadingAspect()->Aspect()->SetTextureMapOn(!aTextureSetNew.IsNull());
    aTexturedIO->Attributes()->ShadingAspect()->Aspect()->SetTextureSet(aTextureSetNew);
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
      aTrsfTrans     = NCollection_Vec2<float>(0.0f, 0.0f);
    }
    if (!toSetTrsfScale)
    {
      toSetTrsfScale = true;
      aTrsfScale     = NCollection_Vec2<float>(1.0f, 1.0f);
    }
  }

  if (aCommandName == "vtexture" && theArgsNb == 2)
  {
    if (!aTextureSetOld.IsNull())
    {
      // toComputeUV = true; // we can keep UV vertex attributes
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->SetTextureMapOff();
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->SetTextureSet(
        occ::handle<Graphic3d_TextureSet>());
      aTextureSetOld.Nullify();
    }
  }

  if (aTexturedIO->Attributes()->HasOwnShadingAspect()
      && !aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap().IsNull())
  {
    if (toModulate != -1)
    {
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap()->GetParams()->SetModulate(
        toModulate == 1);
    }
    if (toSetSRgb != -1)
    {
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap()->SetColorMap(toSetSRgb
                                                                                      == 1);
    }
    if (toSetTrsfAngle)
    {
      // clang-format off
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap()->GetParams()->SetRotation (aTrsfRotAngle); // takes degrees
      // clang-format on
    }
    if (toSetTrsfTrans)
    {
      aTexturedIO->Attributes()
        ->ShadingAspect()
        ->Aspect()
        ->TextureMap()
        ->GetParams()
        ->SetTranslation(aTrsfTrans);
    }
    if (toSetTrsfScale)
    {
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap()->GetParams()->SetScale(
        aTrsfScale);
    }
    if (toSetFilter)
    {
      aTexturedIO->Attributes()->ShadingAspect()->Aspect()->TextureMap()->GetParams()->SetFilter(
        aFilter);
    }
    if (toSetAniso)
    {
      aTexturedIO->Attributes()
        ->ShadingAspect()
        ->Aspect()
        ->TextureMap()
        ->GetParams()
        ->SetAnisoFilter(anAnisoFilter);
    }
  }

  // set default values if requested
  if (!toSetGenRepeat && (aCommandName == "vtexrepeat" || toSetDefaults))
  {
    if (!aTexturedShape.IsNull())
    {
      aTexturedShape->SetTextureRepeatUV(gp_Pnt2d(1.0, 1.0));
    }
    toSetGenRepeat = true;
  }
  if (!toSetGenOrigin && (aCommandName == "vtexorigin" || toSetDefaults))
  {
    if (!aTexturedShape.IsNull())
    {
      aTexturedShape->SetTextureOriginUV(gp_Pnt2d(0.0, 0.0));
    }
    toSetGenOrigin = true;
  }
  if (!toSetGenScale && (aCommandName == "vtexscale" || toSetDefaults))
  {
    if (!aTexturedShape.IsNull())
    {
      aTexturedShape->SetTextureScaleUV(gp_Pnt2d(1.0, 1.0));
    }
    toSetGenScale = true;
  }

  if (toSetGenRepeat || toSetGenOrigin || toSetGenScale || toComputeUV)
  {
    if (!aTexturedShape.IsNull())
    {
      aTexturedShape->SetToUpdate(AIS_Shaded);
      if (toSetImage)
      {
        if ((aTexturedIO->HasDisplayMode() && aTexturedIO->DisplayMode() != AIS_Shaded)
            || aCtx->DisplayMode() != AIS_Shaded)
        {
          aCtx->SetDisplayMode(aTexturedIO, AIS_Shaded, false);
        }
      }
    }
  }
  aCtx->Display(aTexturedIO, false);
  aTexturedIO->SynchronizeAspects();
  return 0;
}

//! Auxiliary method to parse transformation persistence flags
inline bool parseTrsfPersFlag(const TCollection_AsciiString& theFlagString,
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
  else if (theFlagString == "trihedron" || theFlagString == "triedron")
  {
    theFlags = Graphic3d_TMF_TriedronPers;
  }
  else if (theFlagString == "axial")
  {
    theFlags = Graphic3d_TMF_AxialScalePers;
  }
  else if (theFlagString == "zoomaxial")
  {
    theFlags = Graphic3d_TMF_AxialZoomPers;
  }
  else if (theFlagString == "none")
  {
    theFlags = Graphic3d_TMF_None;
  }
  else
  {
    return false;
  }

  return true;
}

//=================================================================================================

bool ViewerTest::ParseCorner(const char* theArg, Aspect_TypeOfTriedronPosition& theCorner)
{
  TCollection_AsciiString aString(theArg);
  aString.LowerCase();
  if (aString == "center")
  {
    theCorner = Aspect_TOTP_CENTER;
  }
  else if (aString == "top" || aString == "upper")
  {
    theCorner = Aspect_TOTP_TOP;
  }
  else if (aString == "bottom" || aString == "lower")
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
  else if (aString == "topleft" || aString == "leftupper" || aString == "upperleft"
           || aString == "left_upper" || aString == "upper_left")
  {
    theCorner = Aspect_TOTP_LEFT_UPPER;
  }
  else if (aString == "bottomleft" || aString == "leftlower" || aString == "lowerleft"
           || aString == "left_lower" || aString == "lower_left")
  {
    theCorner = Aspect_TOTP_LEFT_LOWER;
  }
  else if (aString == "topright" || aString == "rightupper" || aString == "upperright"
           || aString == "right_upper" || aString == "upper_right")
  {
    theCorner = Aspect_TOTP_RIGHT_UPPER;
  }
  else if (aString == "bottomright" || aString == "lowerright" || aString == "rightlower"
           || aString == "right_lower" || aString == "lower_right")
  {
    theCorner = Aspect_TOTP_RIGHT_LOWER;
  }
  else
  {
    return false;
  }

  return true;
}

//==============================================================================
// function : VDisplay2
// author   : ege
// purpose  : Display an object from its name
//==============================================================================
static int VDisplay2(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb < 2)
  {
    Message::SendFail("Syntax error: wrong number of arguments.");
    return 1;
  }
  if (theArgNb == 2 && TCollection_AsciiString(theArgVec[1]) == "*")
  {
    // alias
    return VDisplayAll(theDI, 1, theArgVec);
  }

  occ::handle<AIS_InteractiveContext> aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    ViewerTest::ViewerInit();
    aCtx = ViewerTest::GetAISContext();
  }

  // Parse input arguments
  ViewerTest_AutoUpdater                        anUpdateTool(aCtx, ViewerTest::CurrentView());
  int                                           isMutable     = -1;
  Graphic3d_ZLayerId                            aZLayer       = Graphic3d_ZLayerId_UNKNOWN;
  bool                                          toReDisplay   = false;
  int                                           isSelectable  = -1;
  int                                           anObjDispMode = -2;
  int                                           anObjHighMode = -2;
  bool                                          toSetTrsfPers = false;
  bool                                          toEcho        = true;
  int                                           isAutoTriang  = -1;
  occ::handle<Graphic3d_TransformPers>          aTrsfPers;
  NCollection_Sequence<TCollection_AsciiString> aNamesOfDisplayIO;
  AIS_DisplayStatus                             aDispStatus     = AIS_DS_None;
  int                                           toDisplayInView = false;
  for (int anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    const TCollection_AsciiString aName     = theArgVec[anArgIter];
    TCollection_AsciiString       aNameCase = aName;
    aNameCase.LowerCase();
    if (anUpdateTool.parseRedrawMode(aName))
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
    else if (aNameCase == "-immediate" || aNameCase == "-top")
    {
      aZLayer = Graphic3d_ZLayerId_Top;
    }
    else if (aNameCase == "-topmost")
    {
      aZLayer = Graphic3d_ZLayerId_Topmost;
    }
    else if (aNameCase == "-osd" || aNameCase == "-toposd" || aNameCase == "-overlay")
    {
      aZLayer = Graphic3d_ZLayerId_TopOSD;
    }
    else if (aNameCase == "-botosd" || aNameCase == "-underlay")
    {
      aZLayer = Graphic3d_ZLayerId_BotOSD;
    }
    else if (aNameCase == "-select" || aNameCase == "-selectable")
    {
      isSelectable = 1;
    }
    else if (aNameCase == "-noselect" || aNameCase == "-noselection")
    {
      isSelectable = 0;
    }
    else if (aNameCase == "-dispmode" || aNameCase == "-displaymode")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
        return 1;
      }

      anObjDispMode = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (aNameCase == "-himode" || aNameCase == "-highmode" || aNameCase == "-highlightmode")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
        return 1;
      }

      anObjHighMode = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (aNameCase == "-3d")
    {
      toSetTrsfPers = true;
      aTrsfPers.Nullify();
    }
    else if (aNameCase == "-2d" || aNameCase == "-trihedron" || aNameCase == "-triedron")
    {
      toSetTrsfPers = true;
      aTrsfPers     = new Graphic3d_TransformPers(aNameCase == "-2d" ? Graphic3d_TMF_2d
                                                                 : Graphic3d_TMF_TriedronPers,
                                              Aspect_TOTP_LEFT_LOWER);

      if (anArgIter + 1 < theArgNb)
      {
        Aspect_TypeOfTriedronPosition aCorner = Aspect_TOTP_CENTER;
        if (ViewerTest::ParseCorner(theArgVec[anArgIter + 1], aCorner))
        {
          ++anArgIter;
          aTrsfPers->SetCorner2d(aCorner);

          if (anArgIter + 2 < theArgNb)
          {
            TCollection_AsciiString anX(theArgVec[anArgIter + 1]);
            TCollection_AsciiString anY(theArgVec[anArgIter + 2]);
            if (anX.IsIntegerValue() && anY.IsIntegerValue())
            {
              anArgIter += 2;
              aTrsfPers->SetOffset2d(NCollection_Vec2<int>(anX.IntegerValue(), anY.IntegerValue()));
            }
          }
        }
      }
    }
    else if (aNameCase == "-trsfpers" || aNameCase == "-pers")
    {
      if (++anArgIter >= theArgNb || !aTrsfPers.IsNull())
      {
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
        return 1;
      }

      toSetTrsfPers                           = true;
      Graphic3d_TransModeFlags aTrsfPersFlags = Graphic3d_TMF_None;
      TCollection_AsciiString  aPersFlags(theArgVec[anArgIter]);
      aPersFlags.LowerCase();
      if (!parseTrsfPersFlag(aPersFlags, aTrsfPersFlags))
      {
        Message::SendFail() << "Error: wrong transform persistence flags " << theArgVec[anArgIter]
                            << ".";
        return 1;
      }

      if (aTrsfPersFlags == Graphic3d_TMF_TriedronPers)
      {
        aTrsfPers = new Graphic3d_TransformPers(Graphic3d_TMF_TriedronPers, Aspect_TOTP_LEFT_LOWER);
      }
      else if (aTrsfPersFlags != Graphic3d_TMF_None)
      {
        aTrsfPers = new Graphic3d_TransformPers(aTrsfPersFlags, gp_Pnt());
      }
    }
    else if (aNameCase == "-trsfperspos" || aNameCase == "-perspos")
    {
      if (anArgIter + 2 >= theArgNb || aTrsfPers.IsNull())
      {
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
        return 1;
      }

      TCollection_AsciiString aX(theArgVec[++anArgIter]);
      TCollection_AsciiString aY(theArgVec[++anArgIter]);
      TCollection_AsciiString aZ = "0";
      if (!aX.IsRealValue(true) || !aY.IsRealValue(true))
      {
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
        return 1;
      }
      if (anArgIter + 1 < theArgNb)
      {
        TCollection_AsciiString aTemp = theArgVec[anArgIter + 1];
        if (aTemp.IsRealValue(true))
        {
          aZ = aTemp;
          ++anArgIter;
        }
      }

      if (aTrsfPers->IsZoomOrRotate())
      {
        aTrsfPers->SetAnchorPoint(gp_Pnt(aX.RealValue(), aY.RealValue(), aZ.RealValue()));
      }
      else if (aTrsfPers->IsTrihedronOr2d())
      {
        int aCorner = Aspect_TOTP_CENTER;
        if (aX.RealValue() > 0.0)
        {
          aCorner |= Aspect_TOTP_RIGHT;
        }
        else if (aX.RealValue() < 0.0)
        {
          aCorner |= Aspect_TOTP_LEFT;
        }
        if (aY.RealValue() > 0.0)
        {
          aCorner |= Aspect_TOTP_TOP;
        }
        else if (aY.RealValue() < 0.0)
        {
          aCorner |= Aspect_TOTP_BOTTOM;
        }
        aTrsfPers = new Graphic3d_TransformPers(aTrsfPers->Mode(),
                                                Aspect_TypeOfTriedronPosition(aCorner),
                                                NCollection_Vec2<int>(aZ.IntegerValue()));
      }
    }
    else if (aNameCase == "-trsfPersOrtho")
    {
      if (aTrsfPers.IsNull())
      {
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
        return 1;
      }

      toSetTrsfPers = true;
      if (aTrsfPers->IsZoomOrRotate())
      {
        aTrsfPers = new Graphic3d_TransformPers(aTrsfPers->Mode() | Graphic3d_TMF_OrthoPers,
                                                aTrsfPers->AnchorPoint());
      }
      else if (aTrsfPers->IsTrihedronOr2d())
      {
        aTrsfPers = new Graphic3d_TransformPers(aTrsfPers->Mode() | Graphic3d_TMF_OrthoPers,
                                                aTrsfPers->Corner2d(),
                                                aTrsfPers->Offset2d());
      }
    }
    else if (aNameCase == "-layer" || aNameCase == "-zlayer")
    {
      ++anArgIter;
      if (anArgIter >= theArgNb || !ViewerTest::ParseZLayer(theArgVec[anArgIter], aZLayer)
          || aZLayer == Graphic3d_ZLayerId_UNKNOWN)
      {
        Message::SendFail() << "Error: wrong syntax at " << aName << ".";
        return 1;
      }
    }
    else if (aNameCase == "-view" || aNameCase == "-inview")
    {
      toDisplayInView = true;
    }
    else if (aNameCase == "-redisplay")
    {
      toReDisplay = true;
    }
    else if (aNameCase == "-autotr" || aNameCase == "-autotrian" || aNameCase == "-autotriang"
             || aNameCase == "-autotriangulation" || aNameCase == "-noautotr"
             || aNameCase == "-noautotrian" || aNameCase == "-noautotriang"
             || aNameCase == "-noautotriangulation")
    {
      isAutoTriang = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter) ? 1 : 0;
    }
    else if (aNameCase == "-erased" || aNameCase == "-load")
    {
      aDispStatus = AIS_DS_Erased;
    }
    else if (aNameCase == "-noecho")
    {
      toEcho = false;
    }
    else
    {
      aNamesOfDisplayIO.Append(aName);
    }
  }

  if (aNamesOfDisplayIO.IsEmpty())
  {
    Message::SendFail("Syntax error: wrong number of arguments.");
    return 1;
  }

  // Display interactive objects
  for (int anIter = 1; anIter <= aNamesOfDisplayIO.Length(); ++anIter)
  {
    const TCollection_AsciiString&     aName = aNamesOfDisplayIO.Value(anIter);
    occ::handle<AIS_InteractiveObject> aShape;
    if (!GetMapOfAIS().Find2(aName, aShape))
    {
      // create the AIS_Shape from a name
      TopoDS_Shape aDrawShape = DBRep::GetExisting(aName);
      if (!aDrawShape.IsNull())
      {
        aShape = new AIS_Shape(aDrawShape);
        if (isMutable != -1)
        {
          aShape->SetMutable(isMutable == 1);
        }
        if (aZLayer != Graphic3d_ZLayerId_UNKNOWN)
        {
          aShape->SetZLayer(aZLayer);
        }
        if (isAutoTriang != -1)
        {
          aShape->Attributes()->SetAutoTriangulation(isAutoTriang == 1);
        }
        if (toSetTrsfPers)
        {
          aCtx->SetTransformPersistence(aShape, aTrsfPers);
        }
        if (anObjDispMode != -2)
        {
          if (anObjDispMode == -1)
          {
            aShape->UnsetDisplayMode();
          }
          if (!aShape->AcceptDisplayMode(anObjDispMode))
          {
            Message::SendFail() << "Syntax error: " << aShape->DynamicType()->Name() << " rejects "
                                << anObjDispMode << " display mode";
            return 1;
          }
          else
          {
            aShape->SetDisplayMode(anObjDispMode);
          }
        }
        if (anObjHighMode != -2)
        {
          if (anObjHighMode != -1 && !aShape->AcceptDisplayMode(anObjHighMode))
          {
            Message::SendFail() << "Syntax error: " << aShape->DynamicType()->Name() << " rejects "
                                << anObjHighMode << " display mode";
            return 1;
          }
          aShape->SetHilightMode(anObjHighMode);
        }

        GetMapOfAIS().Bind(aShape, aName);
        int aDispMode =
          aShape->HasDisplayMode()
            ? aShape->DisplayMode()
            : (aShape->AcceptDisplayMode(aCtx->DisplayMode()) ? aCtx->DisplayMode() : 0);
        int aSelMode = -1;
        if (isSelectable == 1 || (isSelectable == -1 && aCtx->GetAutoActivateSelection()))
        {
          aSelMode = aShape->GlobalSelectionMode();
        }

        aCtx->Display(aShape, aDispMode, aSelMode, false, aDispStatus);
        if (toDisplayInView)
        {
          for (NCollection_List<occ::handle<V3d_View>>::Iterator aViewIter(
                 aCtx->CurrentViewer()->DefinedViewIterator());
               aViewIter.More();
               aViewIter.Next())
          {
            aCtx->SetViewAffinity(aShape, aViewIter.Value(), false);
          }
          aCtx->SetViewAffinity(aShape, ViewerTest::CurrentView(), true);
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
      aShape->SetMutable(isMutable == 1);
    }
    if (aZLayer != Graphic3d_ZLayerId_UNKNOWN)
    {
      aShape->SetZLayer(aZLayer);
    }
    if (isAutoTriang != -1)
    {
      aShape->Attributes()->SetAutoTriangulation(isAutoTriang == 1);
    }
    if (toSetTrsfPers)
    {
      aCtx->SetTransformPersistence(aShape, aTrsfPers);
    }
    if (anObjDispMode != -2)
    {
      aShape->SetDisplayMode(anObjDispMode);
    }
    if (anObjHighMode != -2)
    {
      aShape->SetHilightMode(anObjHighMode);
    }
    int aDispMode = aShape->HasDisplayMode()
                      ? aShape->DisplayMode()
                      : (aShape->AcceptDisplayMode(aCtx->DisplayMode()) ? aCtx->DisplayMode() : 0);
    int aSelMode  = -1;
    if (isSelectable == 1 || (isSelectable == -1 && aCtx->GetAutoActivateSelection()))
    {
      aSelMode = aShape->GlobalSelectionMode();
    }

    if (aShape->Type() == AIS_KindOfInteractive_Datum)
    {
      aCtx->Display(aShape, false);
    }
    else
    {
      if (toEcho)
      {
        theDI << "Display " << aName << "\n";
      }

      // update the Shape in the AIS_Shape
      TopoDS_Shape           aNewShape = DBRep::GetExisting(aName);
      occ::handle<AIS_Shape> aShapePrs = occ::down_cast<AIS_Shape>(aShape);
      if (!aShapePrs.IsNull())
      {
        if (!aShapePrs->Shape().IsEqual(aNewShape))
        {
          toReDisplay = true;
        }
        aShapePrs->Set(aNewShape);
      }
      if (toReDisplay)
      {
        aCtx->Redisplay(aShape, false);
      }

      if (aSelMode == -1)
      {
        aCtx->Erase(aShape, false);
      }
      aCtx->Display(aShape, aDispMode, aSelMode, false, aDispStatus);
      if (toDisplayInView)
      {
        aCtx->SetViewAffinity(aShape, ViewerTest::CurrentView(), true);
      }
    }
  }

  return 0;
}

//=======================================================================
// function : VNbDisplayed
// purpose  : Returns number of displayed objects
//=======================================================================
static int VNbDisplayed(Draw_Interpretor& theDi, int theArgsNb, const char** theArgVec)
{
  if (theArgsNb != 1)
  {
    theDi << "Usage : " << theArgVec[0] << "\n";
    return 1;
  }

  occ::handle<AIS_InteractiveContext> aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    Message::SendFail("Syntax error: AIS context is not available.");
    return 1;
  }

  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    theDi << "use 'vinit' command before " << theArgVec[0] << "\n";
    return 1;
  }

  NCollection_List<occ::handle<AIS_InteractiveObject>> aListOfIO;
  aContextAIS->DisplayedObjects(aListOfIO);

  theDi << aListOfIO.Extent() << "\n";
  return 0;
}

//=================================================================================================

static int VUpdate(Draw_Interpretor& /*theDi*/, int theArgsNb, const char** theArgVec)
{
  occ::handle<AIS_InteractiveContext> aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    Message::SendFail("Syntax error: AIS context is not available.");
    return 1;
  }

  if (theArgsNb < 2)
  {
    Message::SendFail("Syntax error: insufficient arguments. Type help for more information.");
    return 1;
  }

  NCollection_List<occ::handle<AIS_InteractiveObject>> aListOfIO;
  for (int anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    TCollection_AsciiString aName = TCollection_AsciiString(theArgVec[anArgIt]);

    occ::handle<AIS_InteractiveObject> anAISObj;
    GetMapOfAIS().Find2(aName, anAISObj);
    if (anAISObj.IsNull())
    {
      Message::SendFail() << theArgVec[0] << ": no AIS interactive object named \"" << aName
                          << "\".";
      return 1;
    }

    aListOfIO.Append(anAISObj);
  }

  NCollection_List<occ::handle<AIS_InteractiveObject>>::Iterator anIOIt(aListOfIO);
  for (; anIOIt.More(); anIOIt.Next())
  {
    aContextAIS->Update(anIOIt.Value(), false);
  }

  aContextAIS->UpdateCurrentViewer();

  return 0;
}

//==============================================================================
// function : VShading
// purpose  : Sharpen or roughten the quality of the shading
// Draw arg : vshading ShapeName 0.1->0.00001  1 deg-> 30 deg
//==============================================================================
static int VShading(Draw_Interpretor&, int argc, const char** argv)
{
  double                             myDevCoef;
  occ::handle<AIS_InteractiveObject> TheAisIO;

  // Verifications
  const bool HaveToSet = (strcasecmp(argv[0], "vsetshading") == 0);
  if (argc < 3)
  {
    myDevCoef = 0.0008;
  }
  else
  {
    myDevCoef = Draw::Atof(argv[2]);
  }

  TCollection_AsciiString name = argv[1];
  GetMapOfAIS().Find2(name, TheAisIO);
  if (TheAisIO.IsNull())
  {
    TopoDS_Shape aDrawShape = DBRep::GetExisting(name);
    if (!aDrawShape.IsNull())
    {
      TheAisIO = new AIS_Shape(aDrawShape);
    }
  }

  if (HaveToSet)
    TheAISContext()->SetDeviationCoefficient(TheAisIO, myDevCoef, true);
  else
    TheAISContext()->SetDeviationCoefficient(TheAisIO, 0.0008, true);

  TheAISContext()->Redisplay(TheAisIO, true);
  return 0;
}

//! Auxiliary method to print Interactive Object information
static void objInfo(const NCollection_Map<occ::handle<AIS_InteractiveObject>>& theDetected,
                    const occ::handle<AIS_InteractiveObject>&                  theObj,
                    Draw_Interpretor&                                          theDI)
{
  if (theObj.IsNull())
  {
    theDI << "NULL presentation\n";
    return;
  }

  theDI << (TheAISContext()->IsDisplayed(theObj) ? "Displayed" : "Hidden   ")
        << (TheAISContext()->IsSelected(theObj) ? " Selected" : "         ")
        << (theDetected.Contains(theObj) ? " Detected" : "         ") << " Type: ";
  if (theObj->Type() == AIS_KindOfInteractive_Datum)
  {
    // AIS_Datum
    if (theObj->Signature() == 3)
    {
      theDI << " AIS_Trihedron";
    }
    else if (theObj->Signature() == 2)
    {
      theDI << " AIS_Axis";
    }
    else if (theObj->Signature() == 6)
    {
      theDI << " AIS_Circle";
    }
    else if (theObj->Signature() == 5)
    {
      theDI << " AIS_Line";
    }
    else if (theObj->Signature() == 7)
    {
      theDI << " AIS_Plane";
    }
    else if (theObj->Signature() == 1)
    {
      theDI << " AIS_Point";
    }
    else if (theObj->Signature() == 4)
    {
      theDI << " AIS_PlaneTrihedron";
    }
  }
  // AIS_Shape
  else if (theObj->Type() == AIS_KindOfInteractive_Shape && theObj->Signature() == 0)
  {
    theDI << " AIS_Shape";
  }
  else if (theObj->Type() == AIS_KindOfInteractive_Relation)
  {
    // PrsDim_Dimension and PrsDim_Relation
    occ::handle<PrsDim_Relation> aRelation = occ::down_cast<PrsDim_Relation>(theObj);
    switch (aRelation->KindOfDimension())
    {
      case PrsDim_KOD_PLANEANGLE:
        theDI << " PrsDim_AngleDimension";
        break;
      case PrsDim_KOD_LENGTH:
        theDI << " PrsDim_Chamf2/3dDimension/PrsDim_LengthDimension";
        break;
      case PrsDim_KOD_DIAMETER:
        theDI << " PrsDim_DiameterDimension";
        break;
      case PrsDim_KOD_ELLIPSERADIUS:
        theDI << " PrsDim_EllipseRadiusDimension";
        break;
      // case PrsDim_KOD_FILLETRADIUS:   theDI << " PrsDim_FilletRadiusDimension "; break;
      case PrsDim_KOD_OFFSET:
        theDI << " PrsDim_OffsetDimension";
        break;
      case PrsDim_KOD_RADIUS:
        theDI << " PrsDim_RadiusDimension";
        break;
      default:
        theDI << " UNKNOWN dimension";
        break;
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
static void printLocalSelectionInfo(const T& theContext, Draw_Interpretor& theDI)
{
  const bool isGlobalCtx = (theContext->DynamicType() == STANDARD_TYPE(AIS_InteractiveContext));
  TCollection_AsciiString aPrevName;
  for (theContext->InitSelected(); theContext->MoreSelected(); theContext->NextSelected())
  {
    const occ::handle<AIS_Shape> aShapeIO =
      occ::down_cast<AIS_Shape>(theContext->SelectedInteractive());
    const occ::handle<SelectMgr_EntityOwner> anOwner = theContext->SelectedOwner();
    if (aShapeIO.IsNull() || anOwner.IsNull())
      continue;
    if (isGlobalCtx)
    {
      if (anOwner == aShapeIO->GlobalSelOwner())
        continue;
    }
    const TopoDS_Shape aSubShape = theContext->SelectedShape();
    if (aSubShape.IsNull() || aShapeIO.IsNull() || !GetMapOfAIS().IsBound1(aShapeIO))
    {
      continue;
    }

    const TCollection_AsciiString aParentName = GetMapOfAIS().Find1(aShapeIO);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aFilter;
    int                                                    aNumber = 0;
    const TopoDS_Shape                                     aShape  = aShapeIO->Shape();
    for (TopExp_Explorer anIter(aShape, aSubShape.ShapeType()); anIter.More(); anIter.Next())
    {
      if (!aFilter.Add(anIter.Current()))
      {
        continue; // filter duplicates
      }

      ++aNumber;
      if (!anIter.Current().IsSame(aSubShape))
      {
        continue;
      }

      const char* aShapeName = nullptr;
      switch (aSubShape.ShapeType())
      {
        case TopAbs_COMPOUND:
          aShapeName = " Compound";
          break;
        case TopAbs_COMPSOLID:
          aShapeName = "CompSolid";
          break;
        case TopAbs_SOLID:
          aShapeName = "    Solid";
          break;
        case TopAbs_SHELL:
          aShapeName = "    Shell";
          break;
        case TopAbs_FACE:
          aShapeName = "     Face";
          break;
        case TopAbs_WIRE:
          aShapeName = "     Wire";
          break;
        case TopAbs_EDGE:
          aShapeName = "     Edge";
          break;
        case TopAbs_VERTEX:
          aShapeName = "   Vertex";
          break;
        default:
        case TopAbs_SHAPE:
          aShapeName = "    Shape";
          break;
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

//=================================================================================================

static int VState(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  occ::handle<AIS_InteractiveContext> aCtx = TheAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: No opened viewer!");
    return 1;
  }

  bool toPrintEntities = false;
  bool toCheckSelected = false;

  for (int anArgIdx = 1; anArgIdx < theArgNb; ++anArgIdx)
  {
    TCollection_AsciiString anOption(theArgVec[anArgIdx]);
    anOption.LowerCase();
    if (anOption == "-detectedentities" || anOption == "-entities")
    {
      toPrintEntities = true;
    }
    else if (anOption == "-hasselected")
    {
      toCheckSelected = true;
    }
  }

  if (toCheckSelected)
  {
    aCtx->InitSelected();
    TCollection_AsciiString hasSelected(static_cast<int>(aCtx->HasSelectedShape()));
    theDI << "Check if context has selected shape: " << hasSelected << "\n";

    return 0;
  }

  if (toPrintEntities)
  {
    theDI << "Detected entities:\n";
    occ::handle<StdSelect_ViewerSelector3d> aSelector = aCtx->MainSelector();

    SelectMgr_SelectingVolumeManager aMgr = aSelector->GetManager();
    for (int aPickIter = 1; aPickIter <= aSelector->NbPicked(); ++aPickIter)
    {
      const SelectMgr_SortCriterion&               aPickData = aSelector->PickedData(aPickIter);
      const occ::handle<Select3D_SensitiveEntity>& anEntity  = aSelector->PickedEntity(aPickIter);
      const occ::handle<SelectMgr_EntityOwner>&    anOwner   = anEntity->OwnerId();
      occ::handle<AIS_InteractiveObject>           anObj =
        occ::down_cast<AIS_InteractiveObject>(anOwner->Selectable());

      TCollection_AsciiString aName;
      GetMapOfAIS().Find1(anObj, aName);
      aName.LeftJustify(20, ' ');
      char anInfoStr[512];
      if (aPickData.Normal.SquareModulus() > ShortRealEpsilon())
      {
        Sprintf(anInfoStr,
                " Depth: %g Distance: %g Point: %g %g %g Normal: %g %g %g",
                aPickData.Depth,
                aPickData.MinDist,
                aPickData.Point.X(),
                aPickData.Point.Y(),
                aPickData.Point.Z(),
                aPickData.Normal.x(),
                aPickData.Normal.y(),
                aPickData.Normal.z());
      }
      else
      {
        Sprintf(anInfoStr,
                " Depth: %g Distance: %g Point: %g %g %g",
                aPickData.Depth,
                aPickData.MinDist,
                aPickData.Point.X(),
                aPickData.Point.Y(),
                aPickData.Point.Z());
      }
      theDI << "  " << aName << anInfoStr << " (" << anEntity->DynamicType()->Name() << ")"
            << "\n";

      if (occ::handle<StdSelect_BRepOwner> aBRepOwner =
            occ::down_cast<StdSelect_BRepOwner>(anOwner))
      {
        theDI << "                       Detected Shape: "
              << aBRepOwner->Shape().TShape()->DynamicType()->Name() << "\n";
      }

      if (occ::handle<Select3D_SensitiveWire> aWire =
            occ::down_cast<Select3D_SensitiveWire>(anEntity))
      {
        occ::handle<Select3D_SensitiveEntity> aSen = aWire->GetLastDetected();
        theDI << "                       Detected Child: " << aSen->DynamicType()->Name() << "\n";
      }
      else if (occ::handle<Select3D_SensitivePrimitiveArray> aPrimArr =
                 occ::down_cast<Select3D_SensitivePrimitiveArray>(anEntity))
      {
        theDI << "                       Detected Element: " << aPrimArr->LastDetectedElement()
              << "\n";
      }
      else if (occ::handle<Select3D_SensitiveTriangulation> aTriSens =
                 occ::down_cast<Select3D_SensitiveTriangulation>(anEntity))
      {
        theDI << "                       Detected Triangle: "
              << aTriSens->LastDetectedTriangleIndex() << "\n";
      }
    }
    return 0;
  }

  NCollection_Map<occ::handle<AIS_InteractiveObject>> aDetected;
  for (aCtx->InitDetected(); aCtx->MoreDetected(); aCtx->NextDetected())
  {
    aDetected.Add(
      occ::down_cast<AIS_InteractiveObject>(aCtx->DetectedCurrentOwner()->Selectable()));
  }

  const bool toShowAll = (theArgNb >= 2 && *theArgVec[1] == '*');
  if (theArgNb >= 2 && !toShowAll)
  {
    for (int anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
    {
      const TCollection_AsciiString      anObjName = theArgVec[anArgIter];
      occ::handle<AIS_InteractiveObject> anObj;
      if (!GetMapOfAIS().Find2(anObjName, anObj))
      {
        theDI << anObjName << " doesn't exist!\n";
        continue;
      }

      TCollection_AsciiString aName = anObjName;
      aName.LeftJustify(20, ' ');
      theDI << "  " << aName << " ";
      objInfo(aDetected, anObj, theDI);
      theDI << "\n";
    }
    return 0;
  }

  if (aCtx->NbSelected() > 0 && !toShowAll)
  {
    NCollection_DataMap<occ::handle<SelectMgr_EntityOwner>, TopoDS_Shape> anOwnerShapeMap;
    for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
    {
      const occ::handle<SelectMgr_EntityOwner> anOwner = aCtx->SelectedOwner();
      const occ::handle<AIS_InteractiveObject> anObj =
        occ::down_cast<AIS_InteractiveObject>(anOwner->Selectable());
      // handle whole object selection
      if (anOwner == anObj->GlobalSelOwner())
      {
        TCollection_AsciiString aName;
        GetMapOfAIS().Find1(anObj, aName);
        aName.LeftJustify(20, ' ');
        theDI << aName << " ";
        objInfo(aDetected, anObj, theDI);
        theDI << "\n";
      }
    }

    // process selected sub-shapes
    printLocalSelectionInfo(aCtx, theDI);

    return 0;
  }

  theDI << "Neutral-point state:\n";
  for (NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>::Iterator
         anObjIter(GetMapOfAIS());
       anObjIter.More();
       anObjIter.Next())
  {
    if (anObjIter.Key1().IsNull())
    {
      continue;
    }

    TCollection_AsciiString aName = anObjIter.Key2();
    aName.LeftJustify(20, ' ');
    theDI << "  " << aName << " ";
    objInfo(aDetected, anObjIter.Key1(), theDI);
    theDI << "\n";
  }
  printLocalSelectionInfo(aCtx, theDI);
  return 0;
}

//=======================================================================
// function : PickShape
// purpose  : First Activate the rightmode + Put Filters to be able to
//           pick objets that are of type <TheType>...
//=======================================================================

TopoDS_Shape ViewerTest::PickShape(const TopAbs_ShapeEnum theShapeType, const int theMaxPick)
{
  occ::handle<NCollection_HArray1<TopoDS_Shape>> aResArray =
    new NCollection_HArray1<TopoDS_Shape>(1, 1);
  PickShapes(theShapeType, aResArray, theMaxPick);
  return aResArray->First();
}

//=================================================================================================

bool ViewerTest::PickShapes(const TopAbs_ShapeEnum                          theShapeType,
                            occ::handle<NCollection_HArray1<TopoDS_Shape>>& theResArray,
                            const int                                       theMaxPick)
{
  const int aNbToReach = theResArray->Length();
  if (aNbToReach > 1)
  {
    Message::SendWarning("WARNING : Pick with Shift+ MB1 for Selection of more than 1 object");
  }

  // step 1: prepare the data
  occ::handle<AIS_InteractiveContext> aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return false;
  }

  aCtx->RemoveFilters();
  NCollection_List<occ::handle<AIS_InteractiveObject>> aDispObjects;
  aCtx->DisplayedObjects(aDispObjects);
  if (theShapeType == TopAbs_SHAPE)
  {
    aCtx->AddFilter(new AIS_TypeFilter(AIS_KindOfInteractive_Shape));
  }
  else
  {
    aCtx->AddFilter(new StdSelect_ShapeTypeFilter(theShapeType));
  }

  const int aSelMode = AIS_Shape::SelectionMode(theShapeType);
  for (NCollection_List<occ::handle<AIS_InteractiveObject>>::Iterator anObjIter(aDispObjects);
       anObjIter.More();
       anObjIter.Next())
  {
    if (occ::handle<AIS_Shape> aShapePrs = occ::down_cast<AIS_Shape>(anObjIter.Value()))
    {
      aCtx->SetSelectionModeActive(aShapePrs, aSelMode, true, AIS_SelectionModesConcurrency_Single);
    }
  }

  // step 2 : wait for the selection...
  int          aNbPickGood = 0, aNbPickFail = 0;
  int          argccc  = 5;
  const char*  bufff[] = {"A", "B", "C", "D", "E"};
  const char** argvvv  = (const char**)bufff;
  for (; aNbPickGood < aNbToReach && aNbPickFail <= theMaxPick;)
  {
    while (ViewerMainLoop(argccc, argvvv))
    {
    }
    int aNbStored = aCtx->NbSelected();
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
    return false;
  }

  int anIndex = theResArray->Lower();
  for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected(), ++anIndex)
  {
    if (aCtx->HasSelectedShape())
    {
      theResArray->SetValue(anIndex, aCtx->SelectedShape());
    }
    else
    {
      occ::handle<AIS_InteractiveObject> IO = aCtx->SelectedInteractive();
      theResArray->SetValue(anIndex, occ::down_cast<AIS_Shape>(IO)->Shape());
    }
  }

  aCtx->RemoveFilters();
  if (theShapeType != TopAbs_SHAPE)
  {
    for (NCollection_List<occ::handle<AIS_InteractiveObject>>::Iterator anObjIter(aDispObjects);
         anObjIter.More();
         anObjIter.Next())
    {
      if (occ::handle<AIS_Shape> aShapePrs = occ::down_cast<AIS_Shape>(anObjIter.Value()))
      {
        aCtx->SetSelectionModeActive(aShapePrs,
                                     aSelMode,
                                     true,
                                     AIS_SelectionModesConcurrency_Single);
      }
    }
  }
  return true;
}

//=================================================================================================

static int VPickShape(Draw_Interpretor& di, int argc, const char** argv)
{
  TopAbs_ShapeEnum aShapeType = TopAbs_SHAPE;
  if (argc != 1)
  {
    TCollection_AsciiString aShapeArg(argv[1]);
    aShapeArg.LowerCase();
    aShapeType = TopAbs_COMPOUND;
    if (aShapeArg == "v" || aShapeArg == "vertex")
      aShapeType = TopAbs_VERTEX;
    else if (aShapeArg == "e" || aShapeArg == "edge")
      aShapeType = TopAbs_EDGE;
    else if (aShapeArg == "w" || aShapeArg == "wire")
      aShapeType = TopAbs_WIRE;
    else if (aShapeArg == "f" || aShapeArg == "face")
      aShapeType = TopAbs_FACE;
    else if (aShapeArg == "shape")
      aShapeType = TopAbs_SHAPE;
    else if (aShapeArg == "shell")
      aShapeType = TopAbs_SHELL;
    else if (aShapeArg == "solid")
      aShapeType = TopAbs_SOLID;
    else
    {
      Message::SendFail() << "Syntax error at '" << argv[1] << "'";
      return 1;
    }
  }

  static int                           THE_NB_SHAPES_OF_TYPE[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  static const TCollection_AsciiString THE_NAME_TYPE[8] =
    {"COMPS", "SOL", "SHE", "F", "W", "E", "V", "SHAP"};

  const int aNbToPick = argc > 2 ? argc - 2 : 1;
  if (aNbToPick == 1)
  {
    TopoDS_Shape aPickedShape = ViewerTest::PickShape(aShapeType);
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
      const int aShapeIndex = ++THE_NB_SHAPES_OF_TYPE[int(aShapeType)];
      aName =
        TCollection_AsciiString("Picked_") + THE_NAME_TYPE[int(aShapeType)] + "_" + aShapeIndex;
    }

    DBRep::Set(aName.ToCString(), aPickedShape);
    occ::handle<AIS_Shape> aShapePrs = new AIS_Shape(aPickedShape);
    ViewerTest::Display(aName, aShapePrs, false, true);
    di << "Name of picked shape: " << aName << "\n";
  }
  else
  {
    TCollection_AsciiString aName(argv[2]);
    aName.LowerCase();
    const bool                                     isAutoNaming = aName == ".";
    occ::handle<NCollection_HArray1<TopoDS_Shape>> aPickedArray =
      new NCollection_HArray1<TopoDS_Shape>(1, aNbToPick);
    if (ViewerTest::PickShapes(aShapeType, aPickedArray))
    {
      for (int aPickedIter = aPickedArray->Lower(); aPickedIter <= aPickedArray->Upper();
           ++aPickedIter)
      {
        TopoDS_Shape aPickedShape = aPickedArray->Value(aPickedIter);
        aName.Clear();
        if (!aPickedShape.IsNull() && isAutoNaming)
        {
          const int aShapeIndex = ++THE_NB_SHAPES_OF_TYPE[int(aShapeType)];
          aName =
            TCollection_AsciiString("Picked_") + THE_NAME_TYPE[int(aShapeType)] + "_" + aShapeIndex;
        }
        else
        {
          aName = argv[1 + aPickedIter];
        }

        DBRep::Set(aName.ToCString(), aPickedShape);
        occ::handle<AIS_Shape> aShapePrs = new AIS_Shape(aPickedShape);
        di << "Display of picked shape #" << aPickedIter << " - name: " << aName << "\n";
        ViewerTest::Display(aName, aShapePrs, false, true);
      }
    }
  }
  TheAISContext()->UpdateCurrentViewer();
  return 0;
}

//=================================================================================================

static int VSelFilter(Draw_Interpretor&, int theArgc, const char** theArgv)
{
  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail("Error: AIS context is not available.");
    return 1;
  }

  for (int anArgIter = 1; anArgIter < theArgc; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgv[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-clear")
    {
      aContext->RemoveFilters();
    }
    else if (anArg == "-contextfilter" && anArgIter + 1 < theArgc)
    {
      TCollection_AsciiString aVal(theArgv[++anArgIter]);
      aVal.LowerCase();
      if (aVal == "and")
      {
        aContext->SetFilterType(SelectMgr_FilterType_AND);
      }
      else if (aVal == "or")
      {
        aContext->SetFilterType(SelectMgr_FilterType_OR);
      }
      else
      {
        Message::SendFail() << "Syntax error: wrong command attribute value '" << aVal << "'";
        return 1;
      }
    }
    else if (anArg == "-type" && anArgIter + 1 < theArgc)
    {
      TCollection_AsciiString aVal(theArgv[++anArgIter]);
      TopAbs_ShapeEnum        aShapeType = TopAbs_COMPOUND;
      if (!TopAbs::ShapeTypeFromString(aVal.ToCString(), aShapeType))
      {
        Message::SendFail() << "Syntax error: wrong command attribute value '" << aVal << "'";
        return 1;
      }

      occ::handle<SelectMgr_Filter> aFilter;
      if (aShapeType == TopAbs_SHAPE)
      {
        aFilter = new AIS_TypeFilter(AIS_KindOfInteractive_Shape);
      }
      else
      {
        aFilter = new StdSelect_ShapeTypeFilter(aShapeType);
      }
      aContext->AddFilter(aFilter);
    }
    else if (anArg == "-secondtype" && anArgIter + 1 < theArgc)
    {
      TCollection_AsciiString aVal(theArgv[++anArgIter]);
      TopAbs_ShapeEnum        aShapeType = TopAbs_COMPOUND;
      if (!TopAbs::ShapeTypeFromString(aVal.ToCString(), aShapeType))
      {
        Message::SendFail() << "Syntax error: wrong command attribute value '" << aVal << "'";
        return 1;
      }

      occ::handle<SelectMgr_Filter> aFilter;
      if (aShapeType == TopAbs_SHAPE)
      {
        aFilter = new AIS_TypeFilter(AIS_KindOfInteractive_Shape);
      }
      else
      {
        aFilter = new StdSelect_ShapeTypeFilter(aShapeType);
      }
      aContext->AddFilter(aFilter);
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << theArgv[anArgIter] << "'";
      return 1;
    }
  }
  return 0;
}

//=================================================================================================

static int VPickSelected(Draw_Interpretor&, int theArgNb, const char** theArgs)
{
  static int              aCount = 0;
  TCollection_AsciiString aName  = "PickedShape_";

  if (theArgNb > 1)
  {
    aName = theArgs[1];
  }
  else
  {
    aName = aName + aCount++ + "_";
  }

  int anIdx = 0;
  for (TheAISContext()->InitSelected(); TheAISContext()->MoreSelected();
       TheAISContext()->NextSelected(), ++anIdx)
  {
    TopoDS_Shape aShape;
    if (TheAISContext()->HasSelectedShape())
    {
      aShape = TheAISContext()->SelectedShape();
    }
    else
    {
      occ::handle<AIS_InteractiveObject> IO = TheAISContext()->SelectedInteractive();
      aShape                                = occ::down_cast<AIS_Shape>(IO)->Shape();
    }

    TCollection_AsciiString aCurrentName = aName;
    if (anIdx > 0)
    {
      aCurrentName += anIdx;
    }

    DBRep::Set((aCurrentName).ToCString(), aShape);

    occ::handle<AIS_Shape> aNewShape = new AIS_Shape(aShape);
    GetMapOfAIS().Bind(aNewShape, aCurrentName);
    TheAISContext()->Display(aNewShape, false);
  }

  TheAISContext()->UpdateCurrentViewer();

  return 0;
}

//=================================================================================================

static int VIOTypes(Draw_Interpretor& di, int, const char**)
{
  //                             1234567890         12345678901234567         123456789
  TCollection_AsciiString Colum[3] = {"Standard Types", "Type Of Object", "Signature"};
  TCollection_AsciiString BlankLine(64, '_');
  int                     i;

  di << "/n" << BlankLine.ToCString() << "\n";

  for (i = 0; i <= 2; i++)
    Colum[i].Center(20, ' ');
  for (i = 0; i <= 2; i++)
    di << "|" << Colum[i].ToCString();
  di << "|\n";

  di << BlankLine.ToCString() << "\n";

  //  TCollection_AsciiString thetypes[5]={"Datum","Shape","Object","Relation","None"};
  const char** names = GetTypeNames();

  TCollection_AsciiString curstring;
  TCollection_AsciiString curcolum[3];

  // les objets de type Datum..
  curcolum[1] += "Datum";
  for (i = 0; i <= 6; i++)
  {
    curcolum[0].Clear();
    curcolum[0] += names[i];

    curcolum[2].Clear();
    curcolum[2] += TCollection_AsciiString(i + 1);

    for (int j = 0; j <= 2; j++)
    {
      curcolum[j].Center(20, ' ');
      di << "|" << curcolum[j].ToCString();
    }
    di << "|\n";
  }
  di << BlankLine.ToCString() << "\n";

  // les objets de type shape
  curcolum[1].Clear();
  curcolum[1] += "Shape";
  curcolum[1].Center(20, ' ');

  for (i = 0; i <= 2; i++)
  {
    curcolum[0].Clear();
    curcolum[0] += names[7 + i];
    curcolum[2].Clear();
    curcolum[2] += TCollection_AsciiString(i);

    for (int j = 0; j <= 2; j++)
    {
      curcolum[j].Center(20, ' ');
      di << "|" << curcolum[j].ToCString();
    }
    di << "|\n";
  }
  di << BlankLine.ToCString() << "\n";
  // les IO de type objet...
  curcolum[1].Clear();
  curcolum[1] += "Object";
  curcolum[1].Center(20, ' ');
  for (i = 0; i <= 1; i++)
  {
    curcolum[0].Clear();
    curcolum[0] += names[10 + i];
    curcolum[2].Clear();
    curcolum[2] += TCollection_AsciiString(i);

    for (int j = 0; j <= 2; j++)
    {
      curcolum[j].Center(20, ' ');
      di << "|" << curcolum[j].ToCString();
    }
    di << "|\n";
  }
  di << BlankLine.ToCString() << "\n";
  // les contraintes et dimensions.
  // pour l'instant on separe juste contraintes et dimensions...
  // plus tard, on detaillera toutes les sortes...
  curcolum[1].Clear();
  curcolum[1] += "Relation";
  curcolum[1].Center(20, ' ');
  for (i = 0; i <= 1; i++)
  {
    curcolum[0].Clear();
    curcolum[0] += names[12 + i];
    curcolum[2].Clear();
    curcolum[2] += TCollection_AsciiString(i);

    for (int j = 0; j <= 2; j++)
    {
      curcolum[j].Center(20, ' ');
      di << "|" << curcolum[j].ToCString();
    }
    di << "|\n";
  }
  di << BlankLine.ToCString() << "\n";

  return 0;
}

static int VEraseType(Draw_Interpretor&, int argc, const char** argv)
{
  if (argc != 2)
    return 1;

  AIS_KindOfInteractive TheType;
  int                   TheSign(-1);
  GetTypeAndSignfromString(argv[1], TheType, TheSign);

  NCollection_List<occ::handle<AIS_InteractiveObject>> LIO;

  // en attendant l'amelioration ais pour les dimensions...
  //
  int dimension_status(-1);
  if (TheType == AIS_KindOfInteractive_Relation)
  {
    dimension_status = TheSign == 1 ? 1 : 0;
    TheSign          = -1;
  }

  TheAISContext()->DisplayedObjects(TheType, TheSign, LIO);
  occ::handle<AIS_InteractiveObject> curio;
  for (NCollection_List<occ::handle<AIS_InteractiveObject>>::Iterator it(LIO); it.More(); it.Next())
  {
    curio = it.Value();

    if (dimension_status == -1)
      TheAISContext()->Erase(curio, false);
    else
    {
      PrsDim_KindOfDimension KOD = occ::down_cast<PrsDim_Relation>(curio)->KindOfDimension();
      if ((dimension_status == 0 && KOD == PrsDim_KOD_NONE)
          || (dimension_status == 1 && KOD != PrsDim_KOD_NONE))
        TheAISContext()->Erase(curio, false);
    }
  }
  TheAISContext()->UpdateCurrentViewer();
  return 0;
}

static int VDisplayType(Draw_Interpretor&, int argc, const char** argv)
{
  if (argc != 2)
    return 1;

  AIS_KindOfInteractive TheType;
  int                   TheSign(-1);
  GetTypeAndSignfromString(argv[1], TheType, TheSign);

  // en attendant l'amelioration ais pour les dimensions...
  //
  int dimension_status(-1);
  if (TheType == AIS_KindOfInteractive_Relation)
  {
    dimension_status = TheSign == 1 ? 1 : 0;
    TheSign          = -1;
  }

  NCollection_List<occ::handle<AIS_InteractiveObject>> LIO;
  TheAISContext()->ObjectsInside(LIO, TheType, TheSign);
  occ::handle<AIS_InteractiveObject> curio;
  for (NCollection_List<occ::handle<AIS_InteractiveObject>>::Iterator it(LIO); it.More(); it.Next())
  {
    curio = it.Value();
    if (dimension_status == -1)
      TheAISContext()->Display(curio, false);
    else
    {
      PrsDim_KindOfDimension KOD = occ::down_cast<PrsDim_Relation>(curio)->KindOfDimension();
      if ((dimension_status == 0 && KOD == PrsDim_KOD_NONE)
          || (dimension_status == 1 && KOD != PrsDim_KOD_NONE))
        TheAISContext()->Display(curio, false);
    }
  }

  TheAISContext()->UpdateCurrentViewer();
  return 0;
}

//=================================================================================================

static int VBsdf(Draw_Interpretor& theDI, int theArgsNb, const char** theArgVec)
{
  occ::handle<V3d_View>   aView   = ViewerTest::CurrentView();
  occ::handle<V3d_Viewer> aViewer = ViewerTest::GetViewerFromContext();
  if (aView.IsNull() || aViewer.IsNull())
  {
    Message::SendFail("Error: No active viewer!");
    return 1;
  }

  ViewerTest_CmdParser aCmd;

  aCmd.SetDescription("Adjusts parameters of material BSDF:");

  aCmd.AddOption("print|echo|p", "Prints BSDF");

  aCmd.AddOption("noupdate|update", "Suppresses viewer redraw call");

  aCmd.AddOption("kc", "Weight of coat specular/glossy BRDF");
  aCmd.AddOption("kd", "Weight of base diffuse BRDF");
  aCmd.AddOption("ks", "Weight of base specular/glossy BRDF");
  aCmd.AddOption("kt", "Weight of base specular/glossy BTDF");
  aCmd.AddOption("le", "Radiance emitted by surface");

  aCmd.AddOption("coatFresnel|cf",
                 "Fresnel reflectance of coat layer. Allowed formats: Constant R, Schlick R G B, "
                 "Dielectric N, Conductor N K");
  aCmd.AddOption("baseFresnel|bf",
                 "Fresnel reflectance of base layer. Allowed formats: Constant R, Schlick R G B, "
                 "Dielectric N, Conductor N K");

  aCmd.AddOption("coatRoughness|cr", "Roughness of coat glossy BRDF");
  aCmd.AddOption("baseRoughness|br", "Roughness of base glossy BRDF");

  aCmd.AddOption("absorpCoeff|af", "Absorption coeff of base transmission BTDF");
  aCmd.AddOption("absorpColor|ac", "Absorption color of base transmission BTDF");

  aCmd.AddOption("normalize|n", "Normalizes BSDF to ensure energy conservation");

  aCmd.Parse(theArgsNb, theArgVec);

  if (aCmd.HasOption("help"))
  {
    theDI.PrintHelp(theArgVec[0]);
    return 0;
  }

  // check viewer update mode
  ViewerTest_AutoUpdater anUpdateTool(ViewerTest::GetAISContext(), ViewerTest::CurrentView());
  for (int anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    if (anUpdateTool.parseRedrawMode(theArgVec[anArgIter]))
    {
      break;
    }
  }

  // find object
  TCollection_AsciiString aName(
    aCmd.Arg(ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY, 0).c_str());
  occ::handle<AIS_InteractiveObject> anIObj;
  if (!GetMapOfAIS().Find2(aName, anIObj))
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Graphic3d_MaterialAspect aMaterial = anIObj->Attributes()->ShadingAspect()->Material();
  Graphic3d_BSDF           aBSDF     = aMaterial.BSDF();

  if (aCmd.HasOption("print"))
  {
    theDI << "\n"
          << "Kc:               " << aBSDF.Kc.r() << ", " << aBSDF.Kc.g() << ", " << aBSDF.Kc.b()
          << "\n"
          << "Kd:               " << aBSDF.Kd.r() << ", " << aBSDF.Kd.g() << ", " << aBSDF.Kd.b()
          << "\n"
          << "Ks:               " << aBSDF.Ks.r() << ", " << aBSDF.Ks.g() << ", " << aBSDF.Ks.b()
          << "\n"
          << "Kt:               " << aBSDF.Kt.r() << ", " << aBSDF.Kt.g() << ", " << aBSDF.Kt.b()
          << "\n"
          << "Le:               " << aBSDF.Le.r() << ", " << aBSDF.Le.g() << ", " << aBSDF.Le.b()
          << "\n";

    for (int aLayerID = 0; aLayerID < 2; ++aLayerID)
    {
      const NCollection_Vec4<float> aFresnel =
        aLayerID < 1 ? aBSDF.FresnelCoat.Serialize() : aBSDF.FresnelBase.Serialize();

      theDI << (aLayerID < 1 ? "Coat Fresnel:     " : "Base Fresnel:     ");

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
          << "Absorption color: " << aBSDF.Absorption.r() << ", " << aBSDF.Absorption.g() << ", "
          << aBSDF.Absorption.b() << "\n";

    return 0;
  }

  if (aCmd.HasOption("coatRoughness", 1, true))
  {
    aBSDF.Kc.w() = aCmd.ArgFloat("coatRoughness");
  }

  if (aCmd.HasOption("baseRoughness", 1, true))
  {
    aBSDF.Ks.w() = aCmd.ArgFloat("baseRoughness");
  }

  if (aCmd.HasOption("absorpCoeff", 1, true))
  {
    aBSDF.Absorption.w() = aCmd.ArgFloat("absorpCoeff");
  }

  if (aCmd.HasOption("absorpColor", 3, true))
  {
    const NCollection_Vec3<float> aRGB = aCmd.ArgVec3f("absorpColor");

    aBSDF.Absorption.r() = aRGB.r();
    aBSDF.Absorption.g() = aRGB.g();
    aBSDF.Absorption.b() = aRGB.b();
  }

  if (aCmd.HasOption("kc", 3) || aCmd.HasOption("kc", 1, true))
  {
    NCollection_Vec3<float> aKc;

    if (aCmd.HasOption("kc", 3))
    {
      aKc = aCmd.ArgVec3f("kc");
    }
    else
    {
      aKc = NCollection_Vec3<float>(aCmd.ArgFloat("kc"));
    }

    aBSDF.Kc.r() = aKc.r();
    aBSDF.Kc.g() = aKc.g();
    aBSDF.Kc.b() = aKc.b();
  }

  if (aCmd.HasOption("kd", 3))
  {
    aBSDF.Kd = aCmd.ArgVec3f("kd");
  }
  else if (aCmd.HasOption("kd", 1, true))
  {
    aBSDF.Kd = NCollection_Vec3<float>(aCmd.ArgFloat("kd"));
  }

  if (aCmd.HasOption("ks", 3) || aCmd.HasOption("ks", 1, true))
  {
    NCollection_Vec3<float> aKs;

    if (aCmd.HasOption("ks", 3))
    {
      aKs = aCmd.ArgVec3f("ks");
    }
    else
    {
      aKs = NCollection_Vec3<float>(aCmd.ArgFloat("ks"));
    }

    aBSDF.Ks.r() = aKs.r();
    aBSDF.Ks.g() = aKs.g();
    aBSDF.Ks.b() = aKs.b();
  }

  if (aCmd.HasOption("kt", 3))
  {
    aBSDF.Kt = aCmd.ArgVec3f("kt");
  }
  else if (aCmd.HasOption("kt", 1, true))
  {
    aBSDF.Kt = NCollection_Vec3<float>(aCmd.ArgFloat("kt"));
  }

  if (aCmd.HasOption("le", 3))
  {
    aBSDF.Le = aCmd.ArgVec3f("le");
  }
  else if (aCmd.HasOption("le", 1, true))
  {
    aBSDF.Le = NCollection_Vec3<float>(aCmd.ArgFloat("le"));
  }

  const std::string aFresnelErrorMessage = "Error! Wrong Fresnel type. Allowed types are: Constant "
                                           "F, Schlick R G B, Dielectric N, Conductor N K\n";

  for (int aLayerID = 0; aLayerID < 2; ++aLayerID)
  {
    const std::string aFresnel = aLayerID < 1 ? "baseFresnel" : "coatFresnel";

    if (aCmd.HasOption(aFresnel, 4)) // Schlick: type R G B
    {
      std::string aFresnelType = aCmd.Arg(aFresnel, 0);
      std::transform(aFresnelType.begin(), aFresnelType.end(), aFresnelType.begin(), ::LowerCase);

      if (aFresnelType == "schlick")
      {
        NCollection_Vec3<float> aRGB(static_cast<float>(Draw::Atof(aCmd.Arg(aFresnel, 1).c_str())),
                                     static_cast<float>(Draw::Atof(aCmd.Arg(aFresnel, 2).c_str())),
                                     static_cast<float>(Draw::Atof(aCmd.Arg(aFresnel, 3).c_str())));

        aRGB.r() = std::min(std::max(aRGB.r(), 0.f), 1.f);
        aRGB.g() = std::min(std::max(aRGB.g(), 0.f), 1.f);
        aRGB.b() = std::min(std::max(aRGB.b(), 0.f), 1.f);

        (aLayerID < 1 ? aBSDF.FresnelBase : aBSDF.FresnelCoat) =
          Graphic3d_Fresnel::CreateSchlick(aRGB);
      }
      else
      {
        theDI << aFresnelErrorMessage.c_str() << "\n";
      }
    }
    else if (aCmd.HasOption(aFresnel, 3)) // Conductor: type N K
    {
      std::string aFresnelType = aCmd.Arg(aFresnel, 0);
      std::transform(aFresnelType.begin(), aFresnelType.end(), aFresnelType.begin(), ::LowerCase);

      if (aFresnelType == "conductor")
      {
        const float aN = static_cast<float>(Draw::Atof(aCmd.Arg(aFresnel, 1).c_str()));
        const float aK = static_cast<float>(Draw::Atof(aCmd.Arg(aFresnel, 2).c_str()));

        (aLayerID < 1 ? aBSDF.FresnelBase : aBSDF.FresnelCoat) =
          Graphic3d_Fresnel::CreateConductor(aN, aK);
      }
      else
      {
        theDI << aFresnelErrorMessage.c_str() << "\n";
      }
    }
    else if (aCmd.HasOption(aFresnel, 2)) // Dielectric or Constant: type N|C
    {
      std::string aFresnelType = aCmd.Arg(aFresnel, 0);
      std::transform(aFresnelType.begin(), aFresnelType.end(), aFresnelType.begin(), ::LowerCase);

      if (aFresnelType == "constant")
      {
        const float aR = static_cast<float>(Draw::Atof(aCmd.Arg(aFresnel, 1).c_str()));

        (aLayerID < 1 ? aBSDF.FresnelBase : aBSDF.FresnelCoat) =
          Graphic3d_Fresnel::CreateConstant(aR);
      }
      else if (aFresnelType == "dielectric")
      {
        const float aN = static_cast<float>(Draw::Atof(aCmd.Arg(aFresnel, 1).c_str()));

        (aLayerID < 1 ? aBSDF.FresnelBase : aBSDF.FresnelCoat) =
          Graphic3d_Fresnel::CreateDielectric(aN);
      }
      else
      {
        theDI << aFresnelErrorMessage.c_str() << "\n";
      }
    }
  }

  if (aCmd.HasOption("normalize"))
  {
    aBSDF.Normalize();
  }

  aMaterial.SetBSDF(aBSDF);
  anIObj->SetMaterial(aMaterial);

  return 0;
}

//==============================================================================
// function : VLoadSelection
// purpose  : Adds given objects to map of AIS and loads selection primitives for them
//==============================================================================
static int VLoadSelection(Draw_Interpretor& /*theDi*/, int theArgNb, const char** theArgVec)
{
  if (theArgNb < 2)
  {
    Message::SendFail("Syntax error: wrong number of arguments.");
    return 1;
  }

  occ::handle<AIS_InteractiveContext> aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    ViewerTest::ViewerInit();
    aCtx = ViewerTest::GetAISContext();
  }

  // Parse input arguments
  for (int anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    const TCollection_AsciiString      aName = theArgVec[anArgIter];
    occ::handle<AIS_InteractiveObject> aShape;
    if (!GetMapOfAIS().Find2(aName, aShape))
    {
      TopoDS_Shape aDrawShape = DBRep::GetExisting(aName);
      if (!aDrawShape.IsNull())
      {
        aShape = new AIS_Shape(aDrawShape);
        GetMapOfAIS().Bind(aShape, aName);
      }
    }
    if (aShape.IsNull())
    {
      Message::SendFail() << "Syntax error: presentation '" << aName << "' not found";
      return 1;
    }

    aCtx->Load(aShape, -1);
    aCtx->Activate(aShape, aShape->GlobalSelectionMode(), true);
  }
  return 0;
}

//==============================================================================
// function : ViewerTest::Commands
// purpose  : Add all the viewer command in the Draw_Interpretor
//==============================================================================

void ViewerTest::Commands(Draw_Interpretor& theCommands)
{
  ViewerTest::ViewerCommands(theCommands);
  ViewerTest::RelationCommands(theCommands);
  ViewerTest::ObjectCommands(theCommands);
  ViewerTest::FilletCommands(theCommands);
  ViewerTest::OpenGlCommands(theCommands);

  const char* aGroup    = "AIS Viewer";
  const char* aFileName = __FILE__;
  auto        addCmd =
    [&](const char* theName, Draw_Interpretor::CommandFunction theFunc, const char* theHelp) {
      theCommands.Add(theName, theHelp, aFileName, theFunc, aGroup);
    };

  // display
  addCmd("visos", visos, /* [visos] */ R"(
visos [name1 ...] [nbUIsos nbVIsos IsoOnPlane(0|1)]
If last 3 optional parameters are not set prints numbers of U-, V- isolines and IsoOnPlane.
)" /* [visos] */);

  addCmd("vdisplay", VDisplay2, /* [vdisplay] */ R"(
vdisplay [-noupdate|-update] [-mutable] [-neutral]
         [-trsfPers {zoom|rotate|zoomRotate|trihedron|axial|zoomaxial|none}=none]
            [-trsfPersPos X Y [Z]] [-3d]
            [-2d|-trihedron [{top|bottom|left|right|topLeft
                            |topRight|bottomLeft|bottomRight}
              [offsetX offsetY]]]
            [-trsfPersOrtho]
         [-dispMode mode] [-highMode mode]
         [-layer index] [-top|-topmost|-overlay|-underlay]
         [-redisplay] [-erased]
         [-noecho] [-autoTriangulation {0|1}]
         name1 [name2] ... [name n]
Displays named objects.
 -noupdate      Suppresses viewer redraw call.
 -mutable       Enables optimizations for mutable objects.
 -neutral       Draws objects in main viewer.
 -erased        Loads the object into context, but does not display it.
 -layer         Sets z-layer for objects.
                Alternatively -overlay|-underlay|-top|-topmost
                options can be used for the default z-layers.
 -top           Draws object on top of main presentations
                but below topmost.
 -topmost       Draws in overlay for 3D presentations.
                with independent Depth.
 -overlay       Draws objects in overlay for 2D presentations.
                (On-Screen-Display)
 -underlay      Draws objects in underlay for 2D presentations.
                (On-Screen-Display)
 -selectable|-noselect Controls selection of objects.
 -trsfPers      Sets a transform persistence flags.
 -trsfPersPos   Sets an anchor point for transform persistence.
 -2d            Displays object in screen coordinates.
                (DY looks up)
 -trsfPersOrtho Set orthographic transform persistence.
                (Objects shown with orthographic projection)
 -dispmode      Sets display mode for objects.
 -highmode      Sets hilight mode for objects.
 -redisplay     Recomputes presentation of objects.
 -noecho        Avoid printing of command results.
 -autoTriang    Enable/disable auto-triangulation for displayed shape.
)" /* [vdisplay] */);

  addCmd("vnbdisplayed", VNbDisplayed, /* [vnbdisplayed] */ R"(
vnbdisplayed : Returns number of displayed objects
)" /* [vnbdisplayed] */);

  addCmd("vupdate", VUpdate, /* [vupdate] */ R"(
vupdate name1 [name2] ... [name n]
Updates named objects in interactive context
)" /* [vupdate] */);

  addCmd("verase", VErase, /* [verase] */ R"(
verase [-noupdate|-update] [name1] ...  [name n] [-noerror]
Erases selected or named objects.
If there are no selected or named objects the whole viewer is erased.
Option -noerror prevents exception on non-existing objects.
)" /* [verase] */);

  addCmd("vremove", VRemove, /* [vremove] */ R"(
vremove [-noupdate|-update] [-context] [-all] [-noinfo] [name1] ...  [name n] [-noerror]
or vremove [-context] -all to remove all objects
Removes selected or named objects.
 -context  do not delete object from the map of objects and names;
 -noupdate suppresses viewer redraw call;
 -noinfo   suppresses displaying the list of removed objects;
 -noerror  prevents exception on non-existing objects.
)" /* [vremove] */);

  addCmd("vdonly", VDonly2, /* [vdonly] */ R"(
vdonly [-noupdate|-update] [name1] ...  [name n]
Displays only selected or named objects.
)" /* [vdonly] */);

  addCmd("vdisplayall", VDisplayAll, /* [vdisplayall] */ R"(
vdisplayall : Displays all erased interactive objects (see vdir and vstate).
)" /* [vdisplayall] */);

  addCmd("veraseall", VErase, /* [veraseall] */ R"(
veraseall : Erases all objects displayed in the viewer.
)" /* [veraseall] */);

  addCmd("verasetype", VEraseType, /* [verasetype] */ R"(
verasetype <Type>
Erase all the displayed objects of one given kind (see vtypes).
The following types are possible:
  Point, Axis, Trihedron, PlaneTrihedron, Line, Circle, Plane, Shape,
  ConnectedShape, MultiConn.Shape, ConnectedInter., MultiConn., Constraint and Dimension.
)" /* [verasetype] */);

  addCmd("vbounding", VBounding, /* [vbounding] */ R"(
vbounding [-noupdate|-update] [-mode] name1 [name2 [...]]
          [-print] [-hide]
Temporarily display bounding box of specified Interactive Objects,
or print it to console if -print is specified.
Already displayed box might be hidden by -hide option.
)" /* [vbounding] */);

  addCmd("vdisplaytype", VDisplayType, /* [vdisplaytype] */ R"(
vdisplaytype <Type> <Signature>
Display all the objects of one given kind (see vtypes) which are stored the interactive context.
The following types are possible:
  Point, Axis, Trihedron, PlaneTrihedron, Line, Circle, Plane, Shape,
  ConnectedShape, MultiConn.Shape, ConnectedInter., MultiConn., Constraint and Dimension.
)" /* [vdisplaytype] */);

  addCmd("vsetdispmode", VDispMode, /* [vsetdispmode] */ R"(
vsetdispmode [name] mode(1,2,..)
Sets display mode for all, selected or named objects.
In case of a shape presentation, 0 defines WireFrame, and 1 defines Shading display modes.
)" /* [vsetdispmode] */);

  addCmd("vunsetdispmode", VDispMode, /* [vunsetdispmode] */ R"(
vunsetdispmode [name]
Unsets custom display mode for selected or named objects.
)" /* [vunsetdispmode] */);

  addCmd("vdir", VDir, /* [vdir] */ R"(
vdir [mask] [-list]
Lists all objects displayed in 3D viewer
  mask - name filter like prefix*
 -list - format list with new-line per name; OFF by default
)" /* [vdir] */);

  addCmd("vdump", VDump, /* [vdump] */ R"(
vdump <filename>.png [-width Width -height Height]
      [-buffer rgb|rgba|depth=rgb]
      [-stereo mono|left|right|blend|sideBySide|overUnder=mono]
      [-xrPose base|head|handLeft|handRight=base]
      [-tileSize Size=0]
      [-grouplayer zlayerId]
      [-singlelayer zlayerId]
      [-buffer shadowmap lightname]
Dumps content of the active view into image file.
)" /* [vdump] */);

  addCmd("vsub", VSubInt, /* [vsub] */ R"(
vsub 0/1 (off/on) [obj] : Subintensity(on/off) of selected objects
)" /* [vsub] */);

  addCmd("vaspects", VAspects, /* [vaspects] */ R"(
vaspects [-noupdate|-update] [name1 [name2 [...]] | -defaults] [-subshapes subname1 [subname2 [...]]]
         [-visibility {0|1}]
         [-color {ColorName | R G B}] [-unsetColor]
         [-backfaceColor Color] [-faceCulling {auto|back|front|doublesided}]
         [-material MatName] [-unsetMaterial]
         [-transparency Transp] [-unsetTransparency]
         [-width LineWidth] [-unsetWidth]
         [-lineType {solid|dash|dot|dotDash|0xHexPattern} [-stippleFactor factor]]
           [-unsetLineType]
         [-markerType {.|+|x|O|xcircle|pointcircle|ring1|ring2|ring3|ball|ImagePath}]
           [-unsetMarkerType]
         [-markerSize Scale] [-unsetMarkerSize]
         [-freeBoundary {0|1}]
           [-freeBoundaryWidth Width] [-unsetFreeBoundaryWidth]
           [-freeBoundaryColor {ColorName | R G B}] [-unsetFreeBoundaryColor]
         [-isoOnTriangulation 0|1]
         [-maxParamValue {value}]
         [-sensitivity {selection_mode} {value}]
         [-shadingModel {unlit|flat|gouraud|phong|pbr|pbr_facet}]
           [-unsetShadingModel]
         [-interior {solid|hatch|hidenline|point}] [-setHatch HatchStyle]
           [-unsetInterior]
         [-faceBoundaryDraw {0|1}] [-mostContinuity {c0|g1|c1|g2|c2|c3|cn}]
         [-faceBoundaryWidth LineWidth] [-faceBoundaryColor R G B] [-faceBoundaryType LineType]
         [-drawEdges {0|1}] [-edgeType LineType] [-edgeColor R G B] [-quadEdges {0|1}]
         [-drawSilhouette {0|1}]
         [-alphaMode {opaque|mask|blend|maskblend|blendauto} [alphaCutOff=0.5]]
         [-dumpJson] [-dumpCompact {0|1}] [-dumpDepth depth]
Manage presentation properties of all, selected or named objects.
When -subshapes is specified than following properties will be assigned to specified sub-shapes.
When -defaults is specified than presentation properties will be
assigned to all objects that have not their own specified properties
and to all objects to be displayed in the future.
If -defaults is used there should not be any objects' names nor -subshapes specifier.
See also vlistcolors and vlistmaterials to list named colors and materials
accepted by arguments -material and -color
)" /* [vaspects] */);

  addCmd("vsetcolor", VAspects, /* [vsetcolor] */ R"(
vsetcolor [-noupdate|-update] [name] ColorName
Sets color for all, selected or named objects.
Alias for vaspects -setcolor [name] ColorName.
)" /* [vsetcolor] */);

  addCmd("vunsetcolor", VAspects, /* [vunsetcolor] */ R"(
vunsetcolor [-noupdate|-update] [name]
Resets color for all, selected or named objects.
Alias for vaspects -unsetcolor [name].
)" /* [vunsetcolor] */);

  addCmd("vsettransparency", VAspects, /* [vsettransparency] */ R"(
vsettransparency [-noupdate|-update] [name] Coefficient
Sets transparency for all, selected or named objects.
The Coefficient may be between 0.0 (opaque) and 1.0 (fully transparent).
Alias for vaspects -settransp [name] Coefficient.
)" /* [vsettransparency] */);

  addCmd("vunsettransparency", VAspects, /* [vunsettransparency] */ R"(
vunsettransparency [-noupdate|-update] [name]
Resets transparency for all, selected or named objects.
Alias for vaspects -unsettransp [name].
)" /* [vunsettransparency] */);

  addCmd("vsetmaterial", VAspects, /* [vsetmaterial] */ R"(
vsetmaterial [-noupdate|-update] [name] MaterialName
n\t\t: Alias for vaspects -setmaterial [name] MaterialName.
)" /* [vsetmaterial] */);

  addCmd("vunsetmaterial", VAspects, /* [vunsetmaterial] */ R"(
vunsetmaterial [-noupdate|-update] [name]
Alias for vaspects -unsetmaterial [name].
)" /* [vunsetmaterial] */);

  addCmd("vsetwidth", VAspects, /* [vsetwidth] */ R"(
vsetwidth [-noupdate|-update] [name] width(0->10)
Alias for vaspects -setwidth [name] width.
)" /* [vsetwidth] */);

  addCmd("vunsetwidth", VAspects, /* [vunsetwidth] */ R"(
vunsetwidth [-noupdate|-update] [name]
Alias for vaspects -unsetwidth [name].
)" /* [vunsetwidth] */);

  addCmd("vsetinteriorstyle", VAspects, /* [vsetinteriorstyle] */ R"(
vsetinteriorstyle [-noupdate|-update] [name] Style
Alias for vaspects -setInterior [name] Style.
)" /* [vsetinteriorstyle] */);

  addCmd("vsetedgetype", VAspects, /* [vsetedgetype] */ R"(
vsetedgetype [name] [-type {solid, dash, dot}] [-color R G B] [-width value]
Alias for vaspects [name] -setEdgeType Type.
)" /* [vsetedgetype] */);

  addCmd("vunsetedgetype", VAspects, /* [vunsetedgetype] */ R"(
vunsetedgetype [name] : Alias for vaspects [name] -unsetEdgeType.
)" /* [vunsetedgetype] */);

  addCmd("vshowfaceboundary", VAspects, /* [vshowfaceboundary] */ R"(
vshowfaceboundary [name]: Alias for vaspects [name] -setFaceBoundaryDraw on.
)" /* [vshowfaceboundary] */);

  addCmd("vsensdis", VDispSensi, /* [vsensdis] */ R"(
vsensdis : Display active entities
(sensitive entities of one of the standard types corresponding to active selection modes).
Standard entity types are those defined in Select3D package:
 - sensitive box, face, curve, segment, circle, point, triangulation, triangle.
Custom (application-defined) sensitive entity types are not processed by this command.
)" /* [vsensdis] */);

  addCmd("vsensera", VClearSensi, /* [vsensera] */ R"(
vsensera : erase active entities
)" /* [vsensera] */);

  addCmd("vsetshading", VShading, /* [vsetshading] */ R"(
vsetshading name Quality(default=0.0008)
Sets deflection coefficient that defines the quality of the shape representation in the shading mode.
)" /* [vsetshading] */);

  addCmd("vunsetshading", VShading, /* [vunsetshading] */ R"(
vunsetshading name
Sets default deflection coefficient (0.0008) that defines the quality of the shape representation in the shading mode.
)" /* [vunsetshading] */);

  addCmd("vtexture", VTexture, /* [vtexture] */ R"(
vtexture [-noupdate|-update] name [ImageFile|IdOfTexture|off]
         [-tex0 Image0] [-tex1 Image1] [...]
         [-3d Image0 Image1 ... ImageN]
         [-origin {u v|off}] [-scale {u v|off}] [-repeat {u v|off}]
         [-trsfTrans du dv] [-trsfScale su sv] [-trsfAngle Angle]
         [-modulate {on|off}] [-srgb {on|off}]=on
         [-setFilter {nearest|bilinear|trilinear}]
         [-setAnisoFilter {off|low|middle|quality}]
         [-default]
The texture can be specified by filepath
or as ID (0<=IdOfTexture<=20) specifying one of the predefined textures.
The options are:
 -scale     Setup texture scaling for generating coordinates; (1, 1) by default
 -origin    Setup texture origin  for generating coordinates; (0, 0) by default
 -repeat    Setup texture repeat  for generating coordinates; (1, 1) by default
 -modulate  Enable or disable texture color modulation
 -srgb      Prefer sRGB texture format when applicable; TRUE by default
 -trsfAngle Setup dynamic texture coordinates transformation - rotation angle
 -trsfTrans Setup dynamic texture coordinates transformation - translation vector
 -trsfScale Setup dynamic texture coordinates transformation - scale vector
 -setFilter Setup texture filter
 -setAnisoFilter Setup anisotropic filter for texture with mip-levels
 -default   Sets texture mapping default parameters
 -3d        Load 3D texture from the list of 2D image files
)" /* [vtexture] */);

  addCmd("vtexscale", VTexture, /* [vtexscale] */ R"(
vtexscale name ScaleU ScaleV
Alias for vtexture name -setScale ScaleU ScaleV.
)" /* [vtexscale] */);

  addCmd("vtexorigin", VTexture, /* [vtexorigin] */ R"(
vtexorigin name OriginU OriginV
Alias for vtexture name -setOrigin OriginU OriginV.
)" /* [vtexorigin] */);

  addCmd("vtexrepeat", VTexture, /* [vtexrepeat] */ R"(
vtexrepeat name RepeatU RepeatV
Alias for vtexture name -setRepeat RepeatU RepeatV.
)" /* [vtexrepeat] */);

  addCmd("vtexdefault", VTexture, /* [vtexdefault] */ R"(
vtexdefault name : Alias for vtexture name -default.
)" /* [vtexdefault] */);

  addCmd("vstate", VState, /* [vstate] */ R"(
vstate [-entities] [-hasSelected] [name1] ... [nameN]
Reports show/hidden state for selected or named objects.
 -entities    prints low-level information about detected entities;
 -hasSelected prints 1 if context has selected shape and 0 otherwise.
)" /* [vstate] */);

  addCmd("vpickshapes", VPickShape, /* [vpickshapes] */ R"(
vpickshape subtype(VERTEX,EDGE,WIRE,FACE,SHELL,SOLID) [name1 or .] [name2 or .] [name n or .]
Hold Ctrl and pick object by clicking Left mouse button.
Hold also Shift for multiple selection.
)" /* [vpickshapes] */);

  addCmd("vtypes", VIOTypes, /* [vtypes] */ R"(
vtypes : list of known types and signatures in AIS.
To be Used in vpickobject command for selection with filters.
)" /* [vtypes] */);

  addCmd("vselfilter", VSelFilter, /* [vselfilter] */ R"(
vselfilter [-contextfilter {AND|OR}]
           [-type {VERTEX|EDGE|WIRE|FACE|SHAPE|SHELL|SOLID}]
           [-secondtype {VERTEX|EDGE|WIRE|FACE|SHAPE|SHELL|SOLID}]
           [-clear]
Sets selection shape type filter in context or remove all filters.
 -contextfilter to define a selection filter for two or more types of entity,
                use value AND (OR by default).
 -type  set type of selection filter; filters are applied with Or combination.
 -clear remove all filters in context.
)" /* [vselfilter] */);

  addCmd("vpickselected", VPickSelected, /* [vpickselected] */ R"(
vpickselected [name]: extract selected shape.
)" /* [vpickselected] */);

  addCmd("vloadselection", VLoadSelection, /* [vloadselection] */ R"(
vloadselection [-context] [name1] ... [nameN]
Allows to load selection primitives for the shapes with names given without displaying them.
)" /* [vloadselection] */);

  addCmd("vbsdf", VBsdf, /* [vbsdf] */ R"(
vbsdf [name] [options]
nAdjusts parameters of material BSDF:
 -help    shows this message
 -print   print BSDF
 -kd      weight of the Lambertian BRDF
 -kr      weight of the reflection BRDF
 -kt      weight of the transmission BTDF
 -ks      weight of the glossy Blinn BRDF
 -le      self-emitted radiance
 -fresnel Fresnel coefficients; Allowed fresnel formats are: Constant x,
          Schlick x y z, Dielectric x, Conductor x y
 -roughness   roughness of material (Blinn's exponent)
 -absorpcoeff absorption coefficient (only for transparent material)
 -absorpcolor absorption color (only for transparent material)
 -normalize   normalize BSDF coefficients
)" /* [vbsdf] */);
}

//==============================================================================
// function : splitParameter
// purpose  : Split parameter string to parameter name and parameter value
//==============================================================================
bool ViewerTest::SplitParameter(const TCollection_AsciiString& theString,
                                TCollection_AsciiString&       theName,
                                TCollection_AsciiString&       theValue)
{
  int aParamNameEnd = theString.FirstLocationInSet("=", 1, theString.Length());

  if (aParamNameEnd == 0)
  {
    return false;
  }

  TCollection_AsciiString aString(theString);
  if (aParamNameEnd != 0)
  {
    theValue = aString.Split(aParamNameEnd);
    aString.Split(aString.Length() - 1);
    theName = aString;
  }

  return true;
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
