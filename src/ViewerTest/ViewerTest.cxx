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

// Modified by  Eric Gouthiere [sep-oct 98] -> add commands for display...
// Modified by  Robert Coublanc [nov 16-17-18 1998]
//             -split ViewerTest.cxx into 3 files : ViewerTest.cxx,
//                                                  ViewerTest_ObjectCommands.cxx
//                                                  ViewerTest_RelationCommands.cxx
//             -add Functions and commands for interactive selection of shapes and objects
//              in AIS Viewers. (PickShape(s), PickObject(s),

#include <Standard_Stream.hxx>

#include <ViewerTest.hxx>
#include <ViewerTest_CmdParser.hxx>

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
#include <Graphic3d_TextureRoot.hxx>
#include <Image_AlienPixMap.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Select3D_SensitiveWire.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <TopTools_MapOfShape.hxx>
#include <ViewerTest_AutoUpdater.hxx>

#include <stdio.h>

#include <Draw_Interpretor.hxx>
#include <TCollection_AsciiString.hxx>
#include <Draw_PluginMacro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(4:4190)
#pragma warning (disable:4996)
#endif

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
#include <AIS_TexturedShape.hxx>
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
#include <BRepOffset.hxx>

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
      aCtx->Remove (anOldObj, Standard_True);
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
  if ( !a3DView().IsNull() ) {
    if (TheAISContext()->HasOpenedContext())
      TheAISContext()->CloseLocalContext();
    ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it(GetMapOfAIS());
    while ( it.More() ) {
      cout << "Remove " << it.Key2() << endl;
      const Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (it.Key1());
      TheAISContext()->Remove(anObj,Standard_False);
      it.Next();
    }
    TheAISContext()->RebuildSelectionStructs();
    TheAISContext()->UpdateCurrentViewer();
    GetMapOfAIS().Clear();
  }
}

//==============================================================================
//function : StandardModesActivation
//purpose  : Activate a selection mode, vertex, edge, wire ..., in a local
//           Context
//==============================================================================
void ViewerTest::StandardModeActivation(const Standard_Integer mode )
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(mode==0) {
    if (TheAISContext()->HasOpenedContext())
      aContext->CloseLocalContext();
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
	aContext->OpenLocalContext();
    }

    const char *cmode="???";

    switch (mode) {
    case 0: cmode = "Shape"; break;
    case 1: cmode = "Vertex"; break;
    case 2: cmode = "Edge"; break;
    case 3: cmode = "Wire"; break;
    case 4: cmode = "Face"; break;
    case 5: cmode = "Shell"; break;
    case 6: cmode = "Solid"; break;
    case 7: cmode = "Compsolid"; break;
    case 8: cmode = "Compound"; break;
    }

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

//==============================================================================
//function : CopyIsoAspect
//purpose  : Returns copy Prs3d_IsoAspect with new number of isolines.
//==============================================================================
static Handle(Prs3d_IsoAspect) CopyIsoAspect
      (const Handle(Prs3d_IsoAspect) &theIsoAspect,
       const Standard_Integer theNbIsos)
{
  Quantity_Color    aColor;
  Aspect_TypeOfLine aType;
  Standard_Real     aWidth;

  theIsoAspect->Aspect()->Values(aColor, aType, aWidth);

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
          TheAISContext()->Redisplay(aShape);
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

//==============================================================================
//function : VSelPrecision
//purpose  : To set the selection tolerance value
//Draw arg : Selection tolerance value (real value determining the width and
//           height of selecting frustum bases). Without arguments the function
//           just prints current tolerance.
//==============================================================================
static int VSelPrecision(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if( argc > 2 )
  {
    di << "Wrong parameters! Must be: " << argv[0] << " [-unset] [tolerance]\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if( aContext.IsNull() )
    return 1;

  if( argc == 1 )
  {
    Standard_Real aPixelTolerance = aContext->PixelTolerance();
    di << "Pixel tolerance : " << aPixelTolerance << "\n";
  }
  else if (argc == 2)
  {
    TCollection_AsciiString anArg = TCollection_AsciiString (argv[1]);
    anArg.LowerCase();
    if (anArg == "-unset")
    {
      aContext->SetPixelTolerance (-1);
    }
    else
    {
      aContext->SetPixelTolerance (anArg.IntegerValue());
    }
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
  Graphic3d_BufferType  aBufferType = Graphic3d_BT_RGB;
  V3d_StereoDumpOptions aStereoOpts = V3d_SDO_MONO;
  ViewerTest_StereoPair aStereoPair = ViewerTest_SP_Single;
  Standard_Integer      aWidth      = 0;
  Standard_Integer      aHeight     = 0;
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
        aBufferType = Graphic3d_BT_RGBA;
      }
      else if (aBufArg == "rgb")
      {
        aBufferType = Graphic3d_BT_RGB;
      }
      else if (aBufArg == "depth")
      {
        aBufferType = Graphic3d_BT_Depth;
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
        aStereoOpts = V3d_SDO_LEFT_EYE;
      }
      else if (aStereoArg == "r"
            || aStereoArg == "right")
      {
        aStereoOpts = V3d_SDO_RIGHT_EYE;
      }
      else if (aStereoArg == "mono")
      {
        aStereoOpts = V3d_SDO_MONO;
      }
      else if (aStereoArg == "blended"
            || aStereoArg == "blend"
            || aStereoArg == "stereo")
      {
        aStereoOpts = V3d_SDO_BLENDED;
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
      aBufferType = Graphic3d_BT_RGBA;
    }
    else if (anArg == "-rgb"
          || anArg ==  "rgb")
    {
      aBufferType = Graphic3d_BT_RGB;
    }
    else if (anArg == "-depth"
          || anArg ==  "depth")
    {
      aBufferType = Graphic3d_BT_Depth;
    }

    else if (anArg == "-width"
          || anArg ==  "width"
          || anArg ==  "sizex")
    {
      if (aWidth != 0)
      {
        std::cout << "Error: wrong syntax at " << theArgVec[anArgIter] << "\n";
        return 1;
      }
      else if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: integer value is expected right after 'width'\n";
        return 1;
      }
      aWidth = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (anArg == "-height"
          || anArg ==  "height"
          || anArg ==  "-sizey")
    {
      if (aHeight != 0)
      {
        std::cout << "Error: wrong syntax at " << theArgVec[anArgIter] << "\n";
        return 1;
      }
      else if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: integer value is expected right after 'height'\n";
        return 1;
      }
      aHeight = Draw::Atoi (theArgVec[anArgIter]);
    }
    else
    {
      std::cout << "Error: unknown argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  if ((aWidth <= 0 && aHeight >  0)
   || (aWidth >  0 && aHeight <= 0))
  {
    std::cout << "Error: dimensions " << aWidth << "x" << aHeight << " are incorrect\n";
    return 1;
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cout << "Error: cannot find an active view!\n";
    return 1;
  }

  if (aWidth <= 0 || aHeight <= 0)
  {
    aView->Window()->Size (aWidth, aHeight);
  }

  Image_AlienPixMap aPixMap;

  bool isBigEndian = Image_PixMap::IsBigEndianHost();
  Image_PixMap::ImgFormat aFormat = Image_PixMap::ImgUNKNOWN;
  switch (aBufferType)
  {
    case Graphic3d_BT_RGB:   aFormat = isBigEndian ? Image_PixMap::ImgRGB  : Image_PixMap::ImgBGR;  break;
    case Graphic3d_BT_RGBA:  aFormat = isBigEndian ? Image_PixMap::ImgRGBA : Image_PixMap::ImgBGRA; break;
    case Graphic3d_BT_Depth: aFormat = Image_PixMap::ImgGrayF; break;
  }

  switch (aStereoPair)
  {
    case ViewerTest_SP_Single:
    {
      if (!aView->ToPixMap (aPixMap, aWidth, aHeight, aBufferType, Standard_True, aStereoOpts))
      {
        theDI << "Fail: view dump failed!\n";
        return 0;
      }
      else if (aPixMap.SizeX() != Standard_Size(aWidth)
            || aPixMap.SizeY() != Standard_Size(aHeight))
      {
        theDI << "Fail: dumped dimensions "    << (Standard_Integer )aPixMap.SizeX() << "x" << (Standard_Integer )aPixMap.SizeY()
              << " are lesser than requested " << aWidth << "x" << aHeight << "\n";
      }
      break;
    }
    case ViewerTest_SP_SideBySide:
    {
      if (!aPixMap.InitZero (aFormat, aWidth * 2, aHeight))
      {
        theDI << "Fail: not enough memory for image allocation!\n";
        return 0;
      }

      Image_PixMap aPixMapL, aPixMapR;
      aPixMapL.InitWrapper (aPixMap.Format(), aPixMap.ChangeData(),
                            aWidth, aHeight, aPixMap.SizeRowBytes());
      aPixMapR.InitWrapper (aPixMap.Format(), aPixMap.ChangeData() + aPixMap.SizePixelBytes() * aWidth,
                            aWidth, aHeight, aPixMap.SizeRowBytes());
      if (!aView->ToPixMap (aPixMapL, aWidth, aHeight, aBufferType, Standard_True, V3d_SDO_LEFT_EYE)
       || !aView->ToPixMap (aPixMapR, aWidth, aHeight, aBufferType, Standard_True, V3d_SDO_RIGHT_EYE)
       )
      {
        theDI << "Fail: view dump failed!\n";
        return 0;
      }
      break;
    }
    case ViewerTest_SP_OverUnder:
    {
      if (!aPixMap.InitZero (aFormat, aWidth, aHeight * 2))
      {
        theDI << "Fail: not enough memory for image allocation!\n";
        return 0;
      }

      Image_PixMap aPixMapL, aPixMapR;
      aPixMapL.InitWrapper (aFormat, aPixMap.ChangeData(),
                            aWidth, aHeight, aPixMap.SizeRowBytes());
      aPixMapR.InitWrapper (aFormat, aPixMap.ChangeData() + aPixMap.SizeRowBytes() * aHeight,
                            aWidth, aHeight, aPixMap.SizeRowBytes());
      if (!aView->ToPixMap (aPixMapL, aWidth, aHeight, aBufferType, Standard_True, V3d_SDO_LEFT_EYE)
       || !aView->ToPixMap (aPixMapR, aWidth, aHeight, aBufferType, Standard_True, V3d_SDO_RIGHT_EYE))
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

//==============================================================================
//function : Displays,Erase...
//purpose  :
//Draw arg :
//==============================================================================
static int VwrTst_DispErase(const Handle(AIS_InteractiveObject)& IO,
			    const Standard_Integer Mode,
			    const Standard_Integer TypeOfOperation,
			    const Standard_Boolean Upd)
{
  Handle(AIS_InteractiveContext) Ctx = ViewerTest::GetAISContext();

  switch(TypeOfOperation){
  case 1:
    Ctx->Display(IO,Mode,Upd);
    break;
  case 2:{
    Ctx->Erase(IO,Upd);
    break;
  }
  case 3:{
    if(IO.IsNull())
      Ctx->SetDisplayMode((AIS_DisplayMode)Mode,Upd);
    else
      Ctx->SetDisplayMode(IO,Mode,Upd);
    break;
  }
  case 4:{
    if(IO.IsNull())
      Ctx->SetDisplayMode(0,Upd);
    else
      Ctx->UnsetDisplayMode(IO,Upd);
    break;
  }
  }
  return 0;
}

//=======================================================================
//function :
//purpose  :
//=======================================================================
static int VDispMode (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{

  TCollection_AsciiString name;
  if(argc>3)
    return 1;
  // display others presentations
  Standard_Integer TypeOfOperation = (strcasecmp(argv[0],"vdispmode")==0)? 1:
    (strcasecmp(argv[0],"verasemode")==0) ? 2 :
      (strcasecmp(argv[0],"vsetdispmode")==0) ? 3 :
	(strcasecmp(argv[0],"vunsetdispmode")==0) ? 4 : -1;

  Handle(AIS_InteractiveContext) Ctx = ViewerTest::GetAISContext();

  //unset displaymode.. comportement particulier...
  if(TypeOfOperation==4){
    if(argc==1){
      if(Ctx->NbSelected()==0){
	Handle(AIS_InteractiveObject) IO;
	VwrTst_DispErase(IO,-1,4,Standard_False);
      }
      else{
	for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
	  VwrTst_DispErase(Ctx->SelectedInteractive(),-1,4,Standard_False);}
      Ctx->UpdateCurrentViewer();
    }
    else{
      Handle(AIS_InteractiveObject) IO;
      name = argv[1];
      if(GetMapOfAIS().IsBound2(name)){
	IO = Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(name));
        if (!IO.IsNull())
          VwrTst_DispErase(IO,-1,4,Standard_True);
      }
    }
  }
  else if(argc==2){
    Standard_Integer Dmode = Draw::Atoi(argv[1]);
    if(Ctx->NbSelected()==0 && TypeOfOperation==3){
      Handle(AIS_InteractiveObject) IO;
      VwrTst_DispErase(IO,Dmode,TypeOfOperation,Standard_True);
    }
    if(!Ctx->HasOpenedContext()){
      // set/unset display mode sur le Contexte...
      for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected()){
	VwrTst_DispErase(Ctx->SelectedInteractive(),Dmode,TypeOfOperation,Standard_False);
      }
      Ctx->UpdateCurrentViewer();
    }
    else{
      for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
	Ctx->Display(Ctx->SelectedInteractive(),Dmode);
    }
  }
  else{
    Handle(AIS_InteractiveObject) IO;
    name = argv[1];
    if(GetMapOfAIS().IsBound2(name))
      IO = Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(name));
    if (!IO.IsNull())
      VwrTst_DispErase(IO,Draw::Atoi(argv[2]),TypeOfOperation,Standard_True);
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
          Ctx->SubIntensityOn(IO);
        else
          Ctx->SubIntensityOff(IO);
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
  Standard_Integer        anInterStyle = Aspect_IS_SOLID;
  TCollection_AsciiString aStyleArg (theArgVec[anArgIter++]);
  aStyleArg.LowerCase();
  if (aStyleArg == "empty")
  {
    anInterStyle = 0;
  }
  else if (aStyleArg == "hollow")
  {
    anInterStyle = 1;
  }
  else if (aStyleArg == "hatch")
  {
    anInterStyle = 2;
  }
  else if (aStyleArg == "solid")
  {
    anInterStyle = 3;
  }
  else if (aStyleArg == "hiddenline")
  {
    anInterStyle = 4;
  }
  else
  {
    anInterStyle = aStyleArg.IntegerValue();
  }
  if (anInterStyle < Aspect_IS_EMPTY
   || anInterStyle > Aspect_IS_HIDDENLINE)
  {
    std::cout << "Error: style must be within a range [0 (Aspect_IS_EMPTY), "
              << Aspect_IS_HIDDENLINE << " (Aspect_IS_HIDDENLINE)]\n";
    return 1;
  }

  if (!aName.IsEmpty()
   && !GetMapOfAIS().IsBound2 (aName))
  {
    std::cout << "Error: object " << aName << " is not displayed!\n";
    return 1;
  }

  if (aCtx->HasOpenedContext())
  {
    aCtx->CloseLocalContext();
  }
  for (ViewTest_PrsIter anIter (aName); anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anIO = anIter.Current();
    if (!anIO.IsNull())
    {
      const Handle(Prs3d_Drawer)& aDrawer        = anIO->Attributes();
      Handle(Prs3d_ShadingAspect) aShadingAspect = aDrawer->ShadingAspect();
      Handle(Graphic3d_AspectFillArea3d) aFillAspect = aShadingAspect->Aspect();
      aFillAspect->SetInteriorStyle ((Aspect_InteriorStyle )anInterStyle);
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
    Sensitivity (-1) {}

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
        && ToSetSensitivity       == 0;
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

      TCollection_AsciiString aValue (theArgVec[anArgIter]);
      aValue.LowerCase();

      if (aValue.IsEqual ("solid"))
      {
        aChangeSet->TypeOfLine = Aspect_TOL_SOLID;
      }
      else if (aValue.IsEqual ("dot"))
      {
        aChangeSet->TypeOfLine = Aspect_TOL_DOT;
      }
      else if (aValue.IsEqual ("dash"))
      {
        aChangeSet->TypeOfLine = Aspect_TOL_DASH;
      }
      else if (aValue.IsEqual ("dotdash"))
      {
        aChangeSet->TypeOfLine = Aspect_TOL_DOTDASH;
      }
      else
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

  if (aCtx->HasOpenedContext())
  {
    aCtx->CloseLocalContext();
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
        if (aChangeSet->ToSetMaxParamValue != 0)
        {
          aDrawer->SetMaximalParameterValue (aChangeSet->MaxParamValue);
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

  if (aCtx->HasOpenedContext())
  {
    aCtx->CloseLocalContext();
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

  if (toRemoveLocal && !aCtx->HasOpenedContext())
  {
    std::cerr << "Error: local selection context is not open.\n";
    return 1;
  }
  else if (!toRemoveLocal && aCtx->HasOpenedContext())
  {
    aCtx->CloseAllContexts (Standard_False);
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
  if (aCtx->HasOpenedContext()
   && !aCtx->LocalContext()->DisplayedObjects (aLocalIO))
  {
    aCtx->CloseAllContexts (Standard_False);
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

  if (toEraseLocal && !aCtx->HasOpenedContext())
  {
    std::cerr << "Error: local selection context is not open.\n";
    return 1;
  }
  else if (!toEraseLocal && aCtx->HasOpenedContext())
  {
    aCtx->CloseAllContexts (Standard_False);
  }

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
        else
        {
          aCtx->Erase (anIO, Standard_False);
        }
      }
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

  if (toDisplayLocal && !aCtx->HasOpenedContext())
  {
    std::cerr << "Error: local selection context is not open.\n";
    return 1;
  }
  else if (!toDisplayLocal && aCtx->HasOpenedContext())
  {
    aCtx->CloseLocalContext (Standard_False);
  }

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

//! Auxiliary method to find presentation
inline Handle(PrsMgr_Presentation) findPresentation (const Handle(AIS_InteractiveContext)& theCtx,
                                                     const Handle(AIS_InteractiveObject)&  theIO,
                                                     const Standard_Integer                theMode)
{
  if (theIO.IsNull())
  {
    return Handle(PrsMgr_Presentation)();
  }

  if (theMode != -1)
  {
    if (theCtx->MainPrsMgr()->HasPresentation (theIO, theMode))
    {
      return theCtx->MainPrsMgr()->Presentation (theIO, theMode);
    }
  }
  else if (theCtx->MainPrsMgr()->HasPresentation (theIO, theIO->DisplayMode()))
  {
    return theCtx->MainPrsMgr()->Presentation (theIO, theIO->DisplayMode());
  }
  else if (theCtx->MainPrsMgr()->HasPresentation (theIO, theCtx->DisplayMode()))
  {
    return theCtx->MainPrsMgr()->Presentation (theIO, theCtx->DisplayMode());
  }
  return Handle(PrsMgr_Presentation)();
}

enum ViewerTest_BndAction
{
  BndAction_Hide,
  BndAction_Show,
  BndAction_Print
};

//! Auxiliary method to print bounding box of presentation
inline void bndPresentation (Draw_Interpretor&                  theDI,
                             const Handle(PrsMgr_Presentation)& thePrs,
                             const TCollection_AsciiString&     theName,
                             const ViewerTest_BndAction         theAction)
{
  switch (theAction)
  {
    case BndAction_Hide:
    {
      thePrs->Presentation()->GraphicUnHighlight();
      break;
    }
    case BndAction_Show:
    {
      Handle(Graphic3d_Structure) aPrs (thePrs->Presentation());
      aPrs->CStructure()->HighlightColor.r = 0.988235f;
      aPrs->CStructure()->HighlightColor.g = 0.988235f;
      aPrs->CStructure()->HighlightColor.b = 0.988235f;
      aPrs->CStructure()->HighlightWithBndBox (aPrs, Standard_True);
      break;
    }
    case BndAction_Print:
    {
      Bnd_Box aBox = thePrs->Presentation()->MinMaxValues();
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
      Handle(PrsMgr_Presentation)   aPrs = findPresentation (aCtx, anIO, aMode);
      if (aPrs.IsNull())
      {
        std::cout << "Error: presentation " << aName << " does not exist\n";
        return 1;
      }
      bndPresentation (theDI, aPrs, aName, anAction);
    }
  }
  else if (aCtx->NbSelected() > 0)
  {
    // remove all currently selected objects
    for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
    {
      Handle(AIS_InteractiveObject) anIO = aCtx->SelectedInteractive();
      Handle(PrsMgr_Presentation)   aPrs = findPresentation (aCtx, anIO, aMode);
      if (!aPrs.IsNull())
      {
        bndPresentation (theDI, aPrs, GetMapOfAIS().IsBound1 (anIO) ? GetMapOfAIS().Find1 (anIO) : "", anAction);
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
      Handle(PrsMgr_Presentation)   aPrs = findPresentation (aCtx, anIO, aMode);
      if (!aPrs.IsNull())
      {
        bndPresentation (theDI, aPrs, anIter.Key2(), anAction);
      }
    }
  }
  return 0;
}

//==============================================================================
//function : VTexture
//purpose  :
//==============================================================================
Standard_Integer VTexture (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgv)
{
  TCollection_AsciiString aCommandName (theArgv[0]);

  NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfAsciiString)> aMapOfArgs;
  if (aCommandName == "vtexture")
  {
    if (theArgsNb < 2)
    {
      std::cout << theArgv[0] << ": " << " invalid arguments.\n";
      std::cout << "Type help for more information.\n";
      return 1;
    }

    // look for options of vtexture command
    TCollection_AsciiString aParseKey;
    for (Standard_Integer anArgIt = 2; anArgIt < theArgsNb; ++anArgIt)
    {
      TCollection_AsciiString anArg (theArgv [anArgIt]);

      anArg.UpperCase();
      if (anArg.Value (1) == '-' && !anArg.IsRealValue())
      {
        aParseKey = anArg;
        aParseKey.Remove (1);
        aParseKey.UpperCase();
        aMapOfArgs.Bind (aParseKey, new TColStd_HSequenceOfAsciiString);
        continue;
      }

      if (aParseKey.IsEmpty())
      {
        continue;
      }

      aMapOfArgs(aParseKey)->Append (anArg);
    }
  }
  else if (aCommandName == "vtexscale"
        || aCommandName == "vtexorigin"
        || aCommandName == "vtexrepeat")
  {
    // scan for parameters of vtexscale, vtexorigin, vtexrepeat commands
    // equal to -scale, -origin, -repeat options of vtexture command
    if (theArgsNb < 2 || theArgsNb > 4)
    {
      std::cout << theArgv[0] << ": " << " invalid arguments.\n";
      std::cout << "Type help for more information.\n";
      return 1;
    }

    Handle(TColStd_HSequenceOfAsciiString) anArgs = new TColStd_HSequenceOfAsciiString;
    if (theArgsNb == 2)
    {
      anArgs->Append ("OFF");
    }
    else if (theArgsNb == 4)
    {
      anArgs->Append (TCollection_AsciiString (theArgv[2]));
      anArgs->Append (TCollection_AsciiString (theArgv[3]));
    }

    TCollection_AsciiString anArgKey;
    if (aCommandName == "vtexscale")
    {
      anArgKey = "SCALE";
    }
    else if (aCommandName == "vtexorigin")
    {
      anArgKey = "ORIGIN";
    }
    else
    {
      anArgKey = "REPEAT";
    }

    aMapOfArgs.Bind (anArgKey, anArgs);
  }
  else if (aCommandName == "vtexdefault")
  {
    // scan for parameters of vtexdefault command
    // equal to -default option of vtexture command
    aMapOfArgs.Bind ("DEFAULT", new TColStd_HSequenceOfAsciiString);
  }

  // Check arguments for validity
  NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfAsciiString)>::Iterator aMapIt (aMapOfArgs);
  for (; aMapIt.More(); aMapIt.Next())
  {
    const TCollection_AsciiString& aKey = aMapIt.Key();
    const Handle(TColStd_HSequenceOfAsciiString)& anArgs = aMapIt.Value();

    // -scale, -origin, -repeat: one argument "off", or two real values
    if ((aKey.IsEqual ("SCALE") || aKey.IsEqual ("ORIGIN") || aKey.IsEqual ("REPEAT"))
      && ((anArgs->Length() == 1 && anArgs->Value(1) == "OFF")
       || (anArgs->Length() == 2 && anArgs->Value(1).IsRealValue() && anArgs->Value(2).IsRealValue())))
    {
      continue;
    }

    // -modulate: single argument "on" / "off"
    if (aKey.IsEqual ("MODULATE") && anArgs->Length() == 1 && (anArgs->Value(1) == "OFF" || anArgs->Value(1) == "ON"))
    {
      continue;
    }

    // -default: no arguments
    if (aKey.IsEqual ("DEFAULT") && anArgs->IsEmpty())
    {
      continue;
    }

    TCollection_AsciiString aLowerKey;
    aLowerKey  = "-";
    aLowerKey += aKey;
    aLowerKey.LowerCase();
    std::cout << theArgv[0] << ": " << aLowerKey << " is unknown option, or the arguments are unacceptable.\n";
    std::cout << "Type help for more information.\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    std::cout << aCommandName << ": " << " please use 'vinit' command to initialize view.\n";
    return 1;
  }

  Standard_Integer aPreviousMode = 0;

  ViewerTest::CurrentView()->SetSurfaceDetail (V3d_TEX_ALL);

  TCollection_AsciiString aShapeName (theArgv[1]);
  Handle(AIS_InteractiveObject) anIO;

  const ViewerTest_DoubleMapOfInteractiveAndName& aMapOfIO = GetMapOfAIS();
  if (aMapOfIO.IsBound2 (aShapeName))
  {
    anIO = Handle(AIS_InteractiveObject)::DownCast (aMapOfIO.Find2 (aShapeName));
  }

  if (anIO.IsNull())
  {
    std::cout << aCommandName << ": shape " << aShapeName << " does not exists.\n";
    return 1;
  }

  Handle(AIS_TexturedShape) aTexturedIO;
  if (anIO->IsKind (STANDARD_TYPE (AIS_TexturedShape)))
  {
    aTexturedIO = Handle(AIS_TexturedShape)::DownCast (anIO);
    aPreviousMode = aTexturedIO->DisplayMode();
  }
  else
  {
    anAISContext->Remove (anIO, Standard_False);
    aTexturedIO = new AIS_TexturedShape (DBRep::Get (theArgv[1]));
    GetMapOfAIS().UnBind1 (anIO);
    GetMapOfAIS().UnBind2 (aShapeName);
    GetMapOfAIS().Bind (aTexturedIO, aShapeName);
  }

  // -------------------------------------------
  //  Turn texturing on/off - only for vtexture
  // -------------------------------------------

  if (aCommandName == "vtexture")
  {
    TCollection_AsciiString aTextureArg (theArgsNb > 2 ? theArgv[2] : "");

    if (aTextureArg.IsEmpty())
    {
      std::cout << aCommandName << ": " << " Texture mapping disabled.\n";
      std::cout << "To enable it, use 'vtexture NameOfShape NameOfTexture'\n" << "\n";

      anAISContext->SetDisplayMode (aTexturedIO, AIS_Shaded, Standard_False);
      if (aPreviousMode == 3)
      {
        anAISContext->RecomputePrsOnly (aTexturedIO);
      }

      anAISContext->Display (aTexturedIO, Standard_True);
      return 0;
    }
    else if (aTextureArg.Value(1) != '-') // "-option" on place of texture argument
    {
      if (aTextureArg == "?")
      {
        TCollection_AsciiString aTextureFolder = Graphic3d_TextureRoot::TexturesFolder();

        theDi << "\n Files in current directory : \n" << "\n";
        theDi.Eval ("glob -nocomplain *");

        TCollection_AsciiString aCmnd ("glob -nocomplain ");
        aCmnd += aTextureFolder;
        aCmnd += "/* ";

        theDi << "Files in " << aTextureFolder.ToCString() << " : \n" << "\n";
        theDi.Eval (aCmnd.ToCString());
        return 0;
      }
      else
      {
        aTexturedIO->SetTextureFileName (aTextureArg);
      }
    }
  }

  // ------------------------------------
  //  Process other options and commands
  // ------------------------------------

  Handle(TColStd_HSequenceOfAsciiString) aValues;
  if (aMapOfArgs.Find ("DEFAULT", aValues))
  {
    aTexturedIO->SetTextureRepeat (Standard_False);
    aTexturedIO->SetTextureOrigin (Standard_False);
    aTexturedIO->SetTextureScale  (Standard_False);
    aTexturedIO->EnableTextureModulate();
  }
  else
  {
    if (aMapOfArgs.Find ("SCALE", aValues))
    {
      if (aValues->Value(1) != "OFF")
      {
        aTexturedIO->SetTextureScale (Standard_True, aValues->Value(1).RealValue(), aValues->Value(2).RealValue());
      }
      else
      {
        aTexturedIO->SetTextureScale (Standard_False);
      }
    }

    if (aMapOfArgs.Find ("ORIGIN", aValues))
    {
      if (aValues->Value(1) != "OFF")
      {
        aTexturedIO->SetTextureOrigin (Standard_True, aValues->Value(1).RealValue(), aValues->Value(2).RealValue());
      }
      else
      {
        aTexturedIO->SetTextureOrigin (Standard_False);
      }
    }

    if (aMapOfArgs.Find ("REPEAT", aValues))
    {
      if (aValues->Value(1) != "OFF")
      {
        aTexturedIO->SetTextureRepeat (Standard_True, aValues->Value(1).RealValue(), aValues->Value(2).RealValue());
      }
      else
      {
        aTexturedIO->SetTextureRepeat (Standard_False);
      }
    }

    if (aMapOfArgs.Find ("MODULATE", aValues))
    {
      if (aValues->Value(1) == "ON")
      {
        aTexturedIO->EnableTextureModulate();
      }
      else
      {
        aTexturedIO->DisableTextureModulate();
      }
    }
  }

  if (aTexturedIO->DisplayMode() == 3 || aPreviousMode == 3)
  {
    anAISContext->RecomputePrsOnly (aTexturedIO);
  }
  else
  {
    anAISContext->SetDisplayMode (aTexturedIO, 3, Standard_False);
    anAISContext->Display (aTexturedIO, Standard_True);
    anAISContext->Update (aTexturedIO,Standard_True);
  }

  return 0;
}

//! Auxiliary method to parse transformation persistence flags
inline Standard_Boolean parseTrsfPersFlag (const TCollection_AsciiString& theFlagString,
                                           Standard_Integer&              theFlags)
{
  if (theFlagString == "pan")
  {
    theFlags |= Graphic3d_TMF_PanPers;
  }
  else if (theFlagString == "zoom")
  {
    theFlags |= Graphic3d_TMF_ZoomPers;
  }
  else if (theFlagString == "rotate")
  {
    theFlags |= Graphic3d_TMF_RotatePers;
  }
  else if (theFlagString == "trihedron")
  {
    theFlags = Graphic3d_TMF_TriedronPers;
  }
  else if (theFlagString == "full")
  {
    theFlags = Graphic3d_TMF_FullPers;
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
  Graphic3d_TransModeFlags aTrsfPersFlags = Graphic3d_TMF_None;
  gp_Pnt aTPPosition;
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
      aTrsfPersFlags = Graphic3d_TMF_None;
    }
    else if (aNameCase == "-2d")
    {
      toSetTrsfPers  = Standard_True;
      aTrsfPersFlags = Graphic3d_TMF_2d;
    }
    else if (aNameCase == "-2dtopdown")
    {
      toSetTrsfPers  = Standard_True;
      aTrsfPersFlags = Graphic3d_TMF_2d | Graphic3d_TMF_2d_IsTopDown;
    }
    else if (aNameCase == "-trsfpers"
          || aNameCase == "-pers")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at " << aName << ".\n";
        return 1;
      }

      toSetTrsfPers  = Standard_True;
      aTrsfPersFlags = Graphic3d_TMF_None;
      TCollection_AsciiString aPersFlags (theArgVec [anArgIter]);
      aPersFlags.LowerCase();
      for (Standard_Integer aParserPos = aPersFlags.Search ("|");; aParserPos = aPersFlags.Search ("|"))
      {
        if (aParserPos == -1)
        {
          if (!parseTrsfPersFlag (aPersFlags, aTrsfPersFlags))
          {
            std::cerr << "Error: wrong transform persistence flags " << theArgVec [anArgIter] << ".\n";
            return 1;
          }
          break;
        }

        TCollection_AsciiString anOtherFlags = aPersFlags.Split (aParserPos - 1);
        if (!parseTrsfPersFlag (aPersFlags, aTrsfPersFlags))
        {
          std::cerr << "Error: wrong transform persistence flags " << theArgVec [anArgIter] << ".\n";
          return 1;
        }
        aPersFlags = anOtherFlags;
      }
    }
    else if (aNameCase == "-trsfperspos"
          || aNameCase == "-perspos")
    {
      if (anArgIter + 2 >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at " << aName << ".\n";
        return 1;
      }

      TCollection_AsciiString aX (theArgVec[++anArgIter]);
      TCollection_AsciiString aY (theArgVec[++anArgIter]);
      TCollection_AsciiString aZ = "0";
      if (!aX.IsIntegerValue()
       || !aY.IsIntegerValue())
      {
        std::cerr << "Error: wrong syntax at " << aName << ".\n";
        return 1;
      }
      if (anArgIter + 1 < theArgNb)
      {
        TCollection_AsciiString aTemp = theArgVec[anArgIter + 1];
        if (aTemp.IsIntegerValue())
        {
          aZ = aTemp;
          ++anArgIter;
        }
      }
      aTPPosition.SetCoord (aX.IntegerValue(), aY.IntegerValue(), aZ.IntegerValue());
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
  if (toDisplayLocal && !aCtx->HasOpenedContext())
  {
    aCtx->OpenLocalContext (Standard_False);
  }
  else if (!toDisplayLocal && aCtx->HasOpenedContext())
  {
    aCtx->CloseAllContexts (Standard_False);
  }

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
          aShape->SetTransformPersistence (aTrsfPersFlags, aTPPosition);
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
          for (aCtx->CurrentViewer()->InitDefinedViews(); aCtx->CurrentViewer()->MoreDefinedViews(); aCtx->CurrentViewer()->NextDefinedViews())
          {
            aCtx->SetViewAffinity (aShape, aCtx->CurrentViewer()->DefinedView(), Standard_False);
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
      aShape->SetTransformPersistence (aTrsfPersFlags, aTPPosition);
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
        aCtx->Erase (aShape);
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
//function : VPerf
//purpose  : Test the annimation of an object along a
//           predifined trajectory
//Draw arg : vperf ShapeName 1/0(Transfo/Location) 1/0(Primitives sensibles ON/OFF)
//==============================================================================

static int VPerf(Draw_Interpretor& di, Standard_Integer , const char** argv) {

  OSD_Timer myTimer;
  if (TheAISContext()->HasOpenedContext())
    TheAISContext()->CloseLocalContext();

  Standard_Real Step=4*M_PI/180;
  Standard_Real Angle=0;

  Handle(AIS_InteractiveObject) aIO;
  if (GetMapOfAIS().IsBound2(argv[1]))
    aIO = Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(argv[1]));
  if (aIO.IsNull())
    return 1;

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(aIO);

  myTimer.Start();

  if (Draw::Atoi(argv[3])==1 ) {
    di<<" Primitives sensibles OFF"<<"\n";
    TheAISContext()->Deactivate(aIO);
  }
  else {
    di<<" Primitives sensibles ON"<<"\n";
  }
  // Movement par transformation
  if(Draw::Atoi(argv[2]) ==1) {
    di<<" Calcul par Transformation"<<"\n";
    for (Standard_Real myAngle=0;Angle<10*2*M_PI; myAngle++) {

      Angle=Step*myAngle;
      gp_Trsf myTransfo;
      myTransfo.SetRotation(gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1) ) ,Angle );
      TheAISContext()->SetLocation(aShape,myTransfo);
      TheAISContext() ->UpdateCurrentViewer();

    }
  }
  else {
    di<<" Calcul par Locations"<<"\n";
    gp_Trsf myAngleTrsf;
    myAngleTrsf.SetRotation(gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1) ), Step  );
    TopLoc_Location myDeltaAngle (myAngleTrsf);
    TopLoc_Location myTrueLoc;

    for (Standard_Real myAngle=0;Angle<10*2*M_PI; myAngle++) {

      Angle=Step*myAngle;
      myTrueLoc=myTrueLoc*myDeltaAngle;
      TheAISContext()->SetLocation(aShape,myTrueLoc );
      TheAISContext() ->UpdateCurrentViewer();
    }
  }
  if (Draw::Atoi(argv[3])==1 ){
    // On reactive la selection des primitives sensibles
    TheAISContext()->Activate(aIO,0);
  }
  a3DView() -> Redraw();
  myTimer.Stop();
  di<<" Temps ecoule "<<"\n";
  myTimer.Show();
  return 0;
}


//==================================================================================
// Function : VAnimation
//==================================================================================
static int VAnimation (Draw_Interpretor& di, Standard_Integer argc, const char** argv) {
  if (argc != 5) {
    di<<"Use: "<<argv[0]<<" CrankArmFile CylinderHeadFile PropellerFile EngineBlockFile"<<"\n";
    return 1;
  }

  Standard_Real thread = 4;
  Standard_Real angleA=0;
  Standard_Real angleB;
  Standard_Real X;
  gp_Ax1 Ax1(gp_Pnt(0,0,0),gp_Vec(0,0,1));

  BRep_Builder B;
  TopoDS_Shape CrankArm;
  TopoDS_Shape CylinderHead;
  TopoDS_Shape Propeller;
  TopoDS_Shape EngineBlock;

  //BRepTools::Read(CrankArm,"/dp_26/Indus/ege/assemblage/CrankArm.rle",B);
  //BRepTools::Read(CylinderHead,"/dp_26/Indus/ege/assemblage/CylinderHead.rle",B);
  //BRepTools::Read(Propeller,"/dp_26/Indus/ege/assemblage/Propeller.rle",B);
  //BRepTools::Read(EngineBlock,"/dp_26/Indus/ege/assemblage/EngineBlock.rle",B);
  BRepTools::Read(CrankArm,argv[1],B);
  BRepTools::Read(CylinderHead,argv[2],B);
  BRepTools::Read(Propeller,argv[3],B);
  BRepTools::Read(EngineBlock,argv[4],B);

  if (CrankArm.IsNull() || CylinderHead.IsNull() || Propeller.IsNull() || EngineBlock.IsNull()) {di<<" Syntaxe error:loading failure."<<"\n";}


  OSD_Timer myTimer;
  myTimer.Start();

  Handle(AIS_Shape) myAisCylinderHead = new AIS_Shape (CylinderHead);
  Handle(AIS_Shape) myAisEngineBlock  = new AIS_Shape (EngineBlock);
  Handle(AIS_Shape) myAisCrankArm     = new AIS_Shape (CrankArm);
  Handle(AIS_Shape) myAisPropeller    = new AIS_Shape (Propeller);

  GetMapOfAIS().Bind(myAisCylinderHead,"a");
  GetMapOfAIS().Bind(myAisEngineBlock,"b");
  GetMapOfAIS().Bind(myAisCrankArm,"c");
  GetMapOfAIS().Bind(myAisPropeller,"d");

  myAisCylinderHead->SetMutable (Standard_True);
  myAisEngineBlock ->SetMutable (Standard_True);
  myAisCrankArm    ->SetMutable (Standard_True);
  myAisPropeller   ->SetMutable (Standard_True);

  TheAISContext()->SetColor (myAisCylinderHead, Quantity_NOC_INDIANRED);
  TheAISContext()->SetColor (myAisEngineBlock,  Quantity_NOC_RED);
  TheAISContext()->SetColor (myAisPropeller,    Quantity_NOC_GREEN);

  TheAISContext()->Display (myAisCylinderHead, Standard_False);
  TheAISContext()->Display (myAisEngineBlock,  Standard_False);
  TheAISContext()->Display (myAisCrankArm,     Standard_False);
  TheAISContext()->Display (myAisPropeller,    Standard_False);

  TheAISContext()->Deactivate(myAisCylinderHead);
  TheAISContext()->Deactivate(myAisEngineBlock );
  TheAISContext()->Deactivate(myAisCrankArm    );
  TheAISContext()->Deactivate(myAisPropeller   );

  // Boucle de mouvement
  for (Standard_Real myAngle = 0;angleA<2*M_PI*10.175 ;myAngle++) {

    angleA = thread*myAngle*M_PI/180;
    X = Sin(angleA)*3/8;
    angleB = atan(X / Sqrt(-X * X + 1));
    Standard_Real decal(25*0.6);


    //Build a transformation on the display
    gp_Trsf aPropellerTrsf;
    aPropellerTrsf.SetRotation(Ax1,angleA);
    TheAISContext()->SetLocation(myAisPropeller,aPropellerTrsf);

    gp_Ax3 base(gp_Pnt(3*decal*(1-Cos(angleA)),-3*decal*Sin(angleA),0),gp_Vec(0,0,1),gp_Vec(1,0,0));
    gp_Trsf aCrankArmTrsf;
    aCrankArmTrsf.SetTransformation(   base.Rotated(gp_Ax1(gp_Pnt(3*decal,0,0),gp_Dir(0,0,1)),angleB));
    TheAISContext()->SetLocation(myAisCrankArm,aCrankArmTrsf);

    TheAISContext()->UpdateCurrentViewer();
  }

  TopoDS_Shape myNewCrankArm  =myAisCrankArm ->Shape().Located( myAisCrankArm ->Transformation() );
  TopoDS_Shape myNewPropeller =myAisPropeller->Shape().Located( myAisPropeller->Transformation() );

  myAisCrankArm ->ResetTransformation();
  myAisPropeller->ResetTransformation();

  myAisCrankArm  -> Set(myNewCrankArm );
  myAisPropeller -> Set(myNewPropeller);

  TheAISContext()->Activate(myAisCylinderHead,0);
  TheAISContext()->Activate(myAisEngineBlock,0 );
  TheAISContext()->Activate(myAisCrankArm ,0   );
  TheAISContext()->Activate(myAisPropeller ,0  );

  myTimer.Stop();
  myTimer.Show();
  myTimer.Start();

  TheAISContext()->Redisplay(myAisCrankArm ,Standard_False);
  TheAISContext()->Redisplay(myAisPropeller,Standard_False);

  TheAISContext()->UpdateCurrentViewer();
  a3DView()->Redraw();

  myTimer.Stop();
  myTimer.Show();

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

  if (TheAISContext()->HasOpenedContext())
    TheAISContext()->CloseLocalContext();

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

  TheAISContext()->Redisplay(TheAisIO);
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
      if (argc<2||argc>3) { di<<" Syntaxe error"<<"\n";return 1;}
      ThereIsName = (argc == 3);
    }
    else {
      // vunsetam
      if (argc>1) {di<<" Syntaxe error"<<"\n";return 1;}
      else {
        di<<" R.A.Z de tous les modes de selecion"<<"\n";
        di<<" Fermeture du Context local"<<"\n";
        if (TheAISContext()->HasOpenedContext())
          TheAISContext()->CloseLocalContext();
      }
    }

    // IL n'y a aps de nom de shape passe en argument
    if (HaveToSet && !ThereIsName){
      Standard_Integer aMode=Draw::Atoi(argv [1]);

      const char *cmode="???";
      switch (aMode) {
      case 0: cmode = "Shape"; break;
      case 1: cmode = "Vertex"; break;
      case 2: cmode = "Edge"; break;
      case 3: cmode = "Wire"; break;
      case 4: cmode = "Face"; break;
      case 5: cmode = "Shell"; break;
      case 6: cmode = "Solid"; break;
      case 7: cmode = "Compound"; break;
      }

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

	TheAISContext()->OpenLocalContext(Standard_False);
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
        const char *cmode="???";

        switch (aMode) {
        case 0: cmode = "Shape"; break;
        case 1: cmode = "Vertex"; break;
        case 2: cmode = "Edge"; break;
        case 3: cmode = "Wire"; break;
        case 4: cmode = "Face"; break;
        case 5: cmode = "Shell"; break;
        case 6: cmode = "Solid"; break;
        case 7: cmode = "Compound"; break;
        }

        if( !TheAISContext()->HasOpenedContext() ) {
          TheAISContext()->OpenLocalContext(Standard_False);
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
static void localCtxInfo (Draw_Interpretor& theDI)
{
  Handle(AIS_InteractiveContext) aCtx = TheAISContext();
  if (!aCtx->HasOpenedContext())
  {
    return;
  }

  TCollection_AsciiString aPrevName;
  Handle(AIS_LocalContext) aCtxLoc = aCtx->LocalContext();
  for (aCtxLoc->InitSelected(); aCtxLoc->MoreSelected(); aCtxLoc->NextSelected())
  {
    const TopoDS_Shape      aSubShape = aCtxLoc->SelectedShape();
    const Handle(AIS_Shape) aShapeIO  = Handle(AIS_Shape)::DownCast (aCtxLoc->SelectedInteractive());
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
    Handle(StdSelect_ViewerSelector3d) aSelector = aCtx->HasOpenedContext() ? aCtx->LocalSelector() : aCtx->MainSelector();
    for (aSelector->InitDetected(); aSelector->MoreDetected(); aSelector->NextDetected())
    {
      const Handle(SelectBasics_SensitiveEntity)& anEntity = aSelector->DetectedEntity();
      Handle(SelectMgr_EntityOwner) anOwner    = Handle(SelectMgr_EntityOwner)::DownCast (anEntity->OwnerId());
      Handle(AIS_InteractiveObject) anObj      = Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
      SelectMgr_SelectingVolumeManager aMgr =
        anObj->HasTransformation() ? aSelector->GetManager().ScaleAndTransform (1, anObj->InversedTransformation())
                                   : aSelector->GetManager();
      SelectBasics_PickResult aResult;
      anEntity->Matches (aMgr, aResult);
      gp_Pnt aDetectedPnt = aMgr.DetectedPoint (aResult.Depth());

      TCollection_AsciiString aName = GetMapOfAIS().Find1 (anObj);
      aName.LeftJustify (20, ' ');
      char anInfoStr[512];
      Sprintf (anInfoStr,
               " Depth: %+.3f Distance: %+.3f Point: %+.3f %+.3f %+.3f",
               aResult.Depth(),
               aResult.DistToGeomCenter(),
               aDetectedPnt.X(), aDetectedPnt.Y(), aDetectedPnt.Z());
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
    }
    return 0;
  }

  NCollection_Map<Handle(AIS_InteractiveObject)> aDetected;
  for (aCtx->InitDetected(); aCtx->MoreDetected(); aCtx->NextDetected())
  {
    aDetected.Add (aCtx->DetectedCurrentObject());
  }

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

  if (aCtx->NbSelected() > 0
   && !toShowAll)
  {
    for (aCtx->InitSelected(); aCtx->MoreSelected(); aCtx->NextSelected())
    {
      Handle(AIS_InteractiveObject) anObj = aCtx->SelectedInteractive();
      TCollection_AsciiString aName = GetMapOfAIS().Find1 (anObj);
      aName.LeftJustify (20, ' ');
      theDI << aName << " ";
      objInfo (aDetected, anObj, theDI);
      theDI << "\n";
    }
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
  localCtxInfo (theDI);
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
  Standard_Integer curindex = (TheType == AIS_KOI_None) ? 0 : TheAISContext()->OpenLocalContext();

  // step 1: prepare the data
  if(curindex !=0){
    Handle(AIS_SignatureFilter) F1 = new AIS_SignatureFilter(TheType,TheSignature);
    TheAISContext()->AddFilter(F1);
  }

  // step 2 : wait for the selection...
//  Standard_Boolean IsGood (Standard_False);
//  Standard_Integer NbPick(0);
  Standard_Boolean NbPickGood (0),NbToReach(arr->Length());
  Standard_Integer NbPickFail(0);
  Standard_Integer argccc = 5;
  const char *bufff[] = { "A", "B", "C","D", "E" };
  const char **argvvv = (const char **) bufff;


  while(NbPickGood<NbToReach && NbPickFail <= MaxPick){
    while(ViewerMainLoop(argccc,argvvv)){}
    Standard_Integer NbStored = TheAISContext()->NbSelected();
    if((unsigned int ) NbStored != NbPickGood)
      NbPickGood= NbStored;
    else
      NbPickFail++;
    cout<<"NbPicked =  "<<NbPickGood<<" |  Nb Pick Fail :"<<NbPickFail<<endl;
  }

  // step3 get result.

  if((unsigned int ) NbPickFail >= NbToReach) return Standard_False;

  Standard_Integer i(0);
  for(TheAISContext()->InitSelected();
      TheAISContext()->MoreSelected();
      TheAISContext()->NextSelected()){
    i++;
    Handle(AIS_InteractiveObject) IO2 = TheAISContext()->SelectedInteractive();
    arr->SetValue(i,IO2);
  }


  if(curindex>0)
    TheAISContext()->CloseLocalContext(curindex);

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
  Standard_Integer curindex = (TheType == AIS_KOI_None) ? 0 : TheAISContext()->OpenLocalContext();

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

  if(curindex!=0)
    TheAISContext()->CloseLocalContext(curindex);
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

  Standard_Integer curindex = TheAISContext()->OpenLocalContext();
  TopoDS_Shape result;

  if(TheType==TopAbs_SHAPE){
    Handle(AIS_TypeFilter) F1 = new AIS_TypeFilter(AIS_KOI_Shape);
    TheAISContext()->AddFilter(F1);
  }
  else{
    Handle(StdSelect_ShapeTypeFilter) TF = new StdSelect_ShapeTypeFilter(TheType);
    TheAISContext()->AddFilter(TF);
    TheAISContext()->ActivateStandardMode(TheType);

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

  if(curindex>0)
    TheAISContext()->CloseLocalContext(curindex);

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
    cout<<" WARNING : Pick with Shift+ MB1 for Selection of more than 1 object"<<"\n";

  // step 1: prepare the data
  Standard_Integer curindex = TheAISContext()->OpenLocalContext();
  if(TheType==TopAbs_SHAPE){
    Handle(AIS_TypeFilter) F1 = new AIS_TypeFilter(AIS_KOI_Shape);
    TheAISContext()->AddFilter(F1);
  }
  else{
    Handle(StdSelect_ShapeTypeFilter) TF = new StdSelect_ShapeTypeFilter(TheType);
    TheAISContext()->AddFilter(TF);
    TheAISContext()->ActivateStandardMode(TheType);

  }

  // step 2 : wait for the selection...

  Standard_Boolean NbPickGood (0),NbToReach(thearr->Length());
  Standard_Integer NbPickFail(0);
  Standard_Integer argccc = 5;
  const char *bufff[] = { "A", "B", "C","D", "E" };
  const char **argvvv = (const char **) bufff;


  while(NbPickGood<NbToReach && NbPickFail <= MaxPick){
    while(ViewerMainLoop(argccc,argvvv)){}
    Standard_Integer NbStored = TheAISContext()->NbSelected();
    if((unsigned int ) NbStored != NbPickGood)
      NbPickGood= NbStored;
    else
      NbPickFail++;
    cout<<"NbPicked =  "<<NbPickGood<<" |  Nb Pick Fail :"<<NbPickFail<<"\n";
  }

  // step3 get result.

  if((unsigned int ) NbPickFail >= NbToReach) return Standard_False;

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

  TheAISContext()->CloseLocalContext(curindex);
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
    TheAISContext()->Display(newsh);
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
	TheAISContext()->Display(newsh);

      }
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
    TheAISContext()->Display (aNewShape);
  }

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
  di<<"|"<<"\n";

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
    di<<"|"<<"\n";
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
    di<<"|"<<"\n";
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
    di<<"|"<<"\n";
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
    di<<"|"<<"\n";
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
  Ctx->Display(ais);
  return 0;
}

//===============================================================================================
//function : VBsdf
//purpose  :
//===============================================================================================
static int VBsdf (Draw_Interpretor& theDi,
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
  aCmd.AddOption ("print|echo|p", "Print BSDF");

  aCmd.AddOption ("kd", "Weight of the Lambertian BRDF");
  aCmd.AddOption ("kr", "Weight of the reflection BRDF");
  aCmd.AddOption ("kt", "Weight of the transmission BTDF");
  aCmd.AddOption ("ks", "Weight of the glossy Blinn BRDF");
  aCmd.AddOption ("le", "Self-emitted radiance");

  aCmd.AddOption ("fresnel|f", "Fresnel coefficients; Allowed fresnel formats are: Constant x, Schlick x y z, Dielectric x, Conductor x y");

  aCmd.AddOption ("roughness|r",    "Roughness of material (Blinn's exponent)");
  aCmd.AddOption ("absorpCoeff|af", "Absorption coeff (only for transparent material)");
  aCmd.AddOption ("absorpColor|ac", "Absorption color (only for transparent material)");

  aCmd.AddOption ("normalize|n", "Normalize BSDF coefficients");

  aCmd.Parse (theArgsNb, theArgVec);

  if (aCmd.HasOption ("help"))
  {
    theDi.PrintHelp (theArgVec[0]);
    return 0;
  }

  TCollection_AsciiString aName (aCmd.Arg ("", 0).c_str());

  // find object
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  if (!aMap.IsBound2 (aName) )
  {
    std::cerr << "Use 'vdisplay' before" << "\n";
    return 1;
  }

  Handle(AIS_InteractiveObject) anIObj = Handle(AIS_InteractiveObject)::DownCast (aMap.Find2 (aName));
  Graphic3d_MaterialAspect aMaterial = anIObj->Attributes()->ShadingAspect()->Material();
  Graphic3d_BSDF aBSDF = aMaterial.BSDF();

  if (aCmd.HasOption ("print"))
  {
    Graphic3d_Vec4 aFresnel = aBSDF.Fresnel.Serialize();

    std::cout << "\n"
      << "Kd:               " << aBSDF.Kd.r() << ", " << aBSDF.Kd.g() << ", " << aBSDF.Kd.b() << "\n"
      << "Kr:               " << aBSDF.Kr.r() << ", " << aBSDF.Kr.g() << ", " << aBSDF.Kr.b() << "\n"
      << "Kt:               " << aBSDF.Kt.r() << ", " << aBSDF.Kt.g() << ", " << aBSDF.Kt.b() << "\n"
      << "Ks:               " << aBSDF.Ks.r() << ", " << aBSDF.Ks.g() << ", " << aBSDF.Ks.b() << "\n"
      << "Le:               " << aBSDF.Le.r() << ", " << aBSDF.Le.g() << ", " << aBSDF.Le.b() << "\n"
      << "Fresnel:          ";

    if (aFresnel.x() >= 0.f)
    {
      std::cout
        << "|Schlick| " << aFresnel.x() << ", " << aFresnel.y() << ", " << aFresnel.z() << "\n";
    }
    else if (aFresnel.x() >= -1.5f)
    {
      std::cout
        << "|Constant| " << aFresnel.z() << "\n";
    }
    else if (aFresnel.x() >= -2.5f)
    {
      std::cout
        << "|Conductor| " << aFresnel.y() << ", " << aFresnel.z() << "\n";
    }
    else
    {
      std::cout
        << "|Dielectric| " << aFresnel.y() << "\n";
    }


    std::cout 
      << "Roughness:        " << aBSDF.Roughness           << "\n"
      << "Absorption coeff: " << aBSDF.AbsorptionCoeff     << "\n"
      << "Absorption color: " << aBSDF.AbsorptionColor.r() << ", "
                              << aBSDF.AbsorptionColor.g() << ", "
                              << aBSDF.AbsorptionColor.b() << "\n";

    return 0;
  }

  if (aCmd.HasOption ("roughness", 1, Standard_True))
  {
    aCmd.Arg ("roughness", 0);
    aBSDF.Roughness = aCmd.ArgFloat ("roughness");
  }

  if (aCmd.HasOption ("absorpCoeff", 1, Standard_True))
  {
    aBSDF.AbsorptionCoeff = aCmd.ArgFloat ("absorpCoeff");
  }

  if (aCmd.HasOption ("absorpColor", 3, Standard_True))
  {
    aBSDF.AbsorptionColor = aCmd.ArgVec3f ("absorpColor");
  }

  if (aCmd.HasOption ("kd", 3))
  {
    aBSDF.Kd = aCmd.ArgVec3f ("kd");
  }
  else if (aCmd.HasOption ("kd", 1, Standard_True))
  {
    aBSDF.Kd = Graphic3d_Vec3 (aCmd.ArgFloat ("kd"));
  }

  if (aCmd.HasOption ("kr", 3))
  {
    aBSDF.Kr = aCmd.ArgVec3f ("kr");
  }
  else if (aCmd.HasOption ("kr", 1, Standard_True))
  {
    aBSDF.Kr = Graphic3d_Vec3 (aCmd.ArgFloat ("kr"));
  }

  if (aCmd.HasOption ("kt", 3))
  {
    aBSDF.Kt = aCmd.ArgVec3f ("kt");
  }
  else if (aCmd.HasOption ("kt", 1, Standard_True))
  {
    aBSDF.Kt = Graphic3d_Vec3 (aCmd.ArgFloat ("kt"));
  }

  if (aCmd.HasOption ("ks", 3))
  {
    aBSDF.Ks = aCmd.ArgVec3f ("ks");
  }
  else if (aCmd.HasOption ("ks", 1, Standard_True))
  {
    aBSDF.Ks = Graphic3d_Vec3 (aCmd.ArgFloat ("ks"));
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
    "Error! Wrong Fresnel type. Allowed types are: Constant x, Schlick x y z, Dielectric x, Conductor x y.\n";

  if (aCmd.HasOption ("fresnel", 4)) // Schlick: type, x, y ,z
  {
    std::string aFresnelType = aCmd.Arg ("fresnel", 0);
    std::transform (aFresnelType.begin(), aFresnelType.end(), aFresnelType.begin(), ::tolower);

    if (aFresnelType == "schlick")
    {
      aBSDF.Fresnel = Graphic3d_Fresnel::CreateSchlick (
        Graphic3d_Vec3 (static_cast<Standard_ShortReal> (Draw::Atof (aCmd.Arg ("fresnel", 1).c_str())),
                        static_cast<Standard_ShortReal> (Draw::Atof (aCmd.Arg ("fresnel", 2).c_str())),
                        static_cast<Standard_ShortReal> (Draw::Atof (aCmd.Arg ("fresnel", 3).c_str()))));
    }
    else
    {
      std::cout << aFresnelErrorMessage;
    }
  }
  else if (aCmd.HasOption ("fresnel", 3)) // Conductor: type, x, y
  {
    std::string aFresnelType = aCmd.Arg ("fresnel", 0);
    std::transform (aFresnelType.begin(), aFresnelType.end(), aFresnelType.begin(), ::tolower);

    if (aFresnelType == "conductor")
    {
      aBSDF.Fresnel = Graphic3d_Fresnel::CreateConductor (
        static_cast<Standard_ShortReal> (Draw::Atof (aCmd.Arg ("fresnel", 1).c_str())),
        static_cast<Standard_ShortReal> (Draw::Atof (aCmd.Arg ("fresnel", 2).c_str())));
    }
    else
    {
      std::cout << aFresnelErrorMessage;
    }
  }
  else if (aCmd.HasOption ("fresnel", 2)) // Dielectric, Constant: type, x
  {
    std::string aFresnelType = aCmd.Arg ("fresnel", 0);
    std::transform (aFresnelType.begin(), aFresnelType.end(), aFresnelType.begin(), ::tolower);

    if (aFresnelType == "dielectric")
    {
      aBSDF.Fresnel = Graphic3d_Fresnel::CreateDielectric (
        static_cast<Standard_ShortReal> (Draw::Atof (aCmd.Arg ("fresnel", 1).c_str())));
    }
    else if (aFresnelType == "constant")
    {
      aBSDF.Fresnel = Graphic3d_Fresnel::CreateConstant (
        static_cast<Standard_ShortReal> (Draw::Atof (aCmd.Arg ("fresnel", 1).c_str())));
    }
    else
    {
      std::cout << aFresnelErrorMessage;
    }
  }

  if (aCmd.HasOption ("normalize"))
  {
    aBSDF.Normalize();
  }

  aMaterial.SetBSDF (aBSDF);
  anIObj->SetMaterial (aMaterial);

  aView->Redraw();

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
  if (isLocal && !aCtx->HasOpenedContext())
  {
    aCtx->OpenLocalContext (Standard_False);
  }
  else if (!isLocal && aCtx->HasOpenedContext())
  {
    aCtx->CloseAllContexts (Standard_False);
  }

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
//function : VAutoActivateSelection
//purpose  : Activates or deactivates auto computation of selection
//==============================================================================
static int VAutoActivateSelection (Draw_Interpretor& theDi,
                                   Standard_Integer theArgNb,
                                   const char** theArgVec)
{

  if (theArgNb > 2)
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

  if (theArgNb == 1)
  {
    TCollection_AsciiString aSelActivationString;
    if (aCtx->GetAutoActivateSelection())
    {
      aSelActivationString.Copy ("ON");
    }
    else
    {
      aSelActivationString.Copy ("OFF");
    }

    theDi << "Auto activation of selection is: " << aSelActivationString << "\n";
  }
  else
  {
    Standard_Boolean toActivate = Draw::Atoi (theArgVec[1]);
    aCtx->SetAutoActivateSelection (toActivate);
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
  ViewerTest::VoxelCommands(theCommands);
  ViewerTest::OpenGlCommands(theCommands);

  const char *group = "AIS_Display";

  // display
  theCommands.Add("visos",
      "visos [name1 ...] [nbUIsos nbVIsos IsoOnPlane(0|1)]\n"
      "\tIf last 3 optional parameters are not set prints numbers of U-, V- isolines and IsoOnPlane.\n",
      __FILE__, visos, group);

  theCommands.Add("vdisplay",
              "vdisplay [-noupdate|-update] [-local] [-mutable] [-neutral]"
      "\n\t\t:          [-trsfPers {pan|zoom|rotate|trihedron|full|none}=none] [-trsfPersPos X Y [Z]] [-3d|-2d|-2dTopDown]"
      "\n\t\t:          [-dispMode mode] [-highMode mode]"
      "\n\t\t:          [-layer index] [-top|-topmost|-overlay|-underlay]"
      "\n\t\t:          [-redisplay]"
      "\n\t\t:          name1 [name2] ... [name n]"
      "\n\t\t: Displays named objects."
      "\n\t\t: Option -local enables displaying of objects in local"
      "\n\t\t: selection context. Local selection context will be opened"
      "\n\t\t: if there is not any."
      "\n\t\t:  -noupdate    suppresses viewer redraw call."
      "\n\t\t:  -mutable     enables optimizations for mutable objects."
      "\n\t\t:  -neutral     draws objects in main viewer."
      "\n\t\t:  -layer       sets z-layer for objects. It can use -overlay|-underlay|-top|-topmost instead of -layer index for the default z-layers."
      "\n\t\t:  -top         draws objects on top of main presentations but below topmost."
      "\n\t\t:  -topmost     draws in overlay for 3D presentations with independent Depth."
      "\n\t\t:  -overlay     draws objects in overlay for 2D presentations (On-Screen-Display)."
      "\n\t\t:  -underlay    draws objects in underlay for 2D presentations (On-Screen-Display)."
      "\n\t\t:  -selectable|-noselect controls selection of objects."
      "\n\t\t:  -trsfPers    sets a transform persistence flags. Flag 'full' is pan, zoom and rotate."
      "\n\t\t:  -trsfPersPos sets an anchor point for transform persistence."
      "\n\t\t:  -2d|-2dTopDown displays object in screen coordinates."
      "\n\t\t:  -dispmode sets display mode for objects."
      "\n\t\t:  -highmode sets hilight mode for objects."
      "\n\t\t:  -redisplay recomputes presentation of objects.",
      __FILE__, VDisplay2, group);

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

  theCommands.Add("vdisplaymode",
		  "vdispmode       : vdispmode  [name] mode(1,2,..) : no name -> on selected objects ",
		  __FILE__,VDispMode,group);

  theCommands.Add("verasemode",
		  "verasemode      : verasemode [name] mode(1,2,..) : no name -> on selected objects",
		  __FILE__,VDispMode,group);

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
      "\n\t\t:          [-freeBoundary {off/on | 0/1}]"
      "\n\t\t:          [-setFreeBoundaryWidth Width] [-unsetFreeBoundaryWidth]"
      "\n\t\t:          [-setFreeBoundaryColor {ColorName | R G B}] [-unsetFreeBoundaryColor]"
      "\n\t\t:          [-subshapes subname1 [subname2 [...]]]"
      "\n\t\t:          [-isoontriangulation 0|1]"
      "\n\t\t:          [-setMaxParamValue {value}]"
      "\n\t\t:          [-setSensitivity {selection_mode} {value}]"
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

  theCommands.Add("vselprecision",
		  "vselprecision [-unset] [tolerance_value]"
                  "\n\t\t  Manages selection precision or prints current value if no parameter is passed."
                  "\n\t\t  -unset - restores default selection tolerance behavior, based on individual entity tolerance",
		  __FILE__,VSelPrecision,group);

  theCommands.Add("vperf",
      "vperf: vperf  ShapeName 1/0(Transfo/Location) 1/0(Primitives sensibles ON/OFF)"
      "\n\t\t: Tests the animation of an object along a predefined trajectory.",
      __FILE__,VPerf,group);

  theCommands.Add("vanimation",
		  "vanimation CrankArmFile CylinderHeadFile PropellerFile EngineBlockFile",
		  __FILE__,VAnimation,group);

  theCommands.Add("vsetshading",
      "vsetshading  : vsetshading name Quality(default=0.0008) "
      "\n\t\t: Sets deflection coefficient that defines the quality of the shape’s representation in the shading mode.",
      __FILE__,VShading,group);

  theCommands.Add("vunsetshading",
      "vunsetshading :vunsetshading name "
      "\n\t\t: Sets default deflection coefficient (0.0008) that defines the quality of the shape’s representation in the shading mode.",
      __FILE__,VShading,group);

  theCommands.Add ("vtexture",
                   "\n'vtexture NameOfShape [TextureFile | IdOfTexture]\n"
                   "                         [-scale u v]  [-scale off]\n"
                   "                         [-origin u v] [-origin off]\n"
                   "                         [-repeat u v] [-repeat off]\n"
                   "                         [-modulate {on | off}]"
                   "                         [-default]'\n"
                   " The texture can be specified by filepath or as ID (0<=IdOfTexture<=20)\n"
                   " specifying one of the predefined textures.\n"
                   " The options are: \n"
                   "  -scale u v : enable texture scaling and set scale factors\n"
                   "  -scale off : disable texture scaling\n"
                   "  -origin u v : enable texture origin positioning and set the origin\n"
                   "  -origin off : disable texture origin positioning\n"
                   "  -repeat u v : enable texture repeat and set texture coordinate scaling\n"
                   "  -repeat off : disable texture repeat\n"
                   "  -modulate {on | off} : enable or disable texture modulation\n"
                   "  -default : sets texture mapping default parameters\n"
                   "or 'vtexture NameOfShape' if you want to disable texture mapping\n"
                   "or 'vtexture NameOfShape ?' to list available textures\n",
                    __FILE__, VTexture, group);

  theCommands.Add("vtexscale",
		  "'vtexscale  NameOfShape ScaleU ScaleV' \n \
                   or 'vtexscale NameOfShape ScaleUV' \n \
                   or 'vtexscale NameOfShape' to disable scaling\n ",
		  __FILE__,VTexture,group);

  theCommands.Add("vtexorigin",
		  "'vtexorigin NameOfShape UOrigin VOrigin' \n \
                   or 'vtexorigin NameOfShape UVOrigin' \n \
                   or 'vtexorigin NameOfShape' to disable origin positioning\n ",
		  __FILE__,VTexture,group);

  theCommands.Add("vtexrepeat",
		  "'vtexrepeat  NameOfShape URepeat VRepeat' \n \
                   or 'vtexrepeat NameOfShape UVRepeat \n \
                   or 'vtexrepeat NameOfShape' to disable texture repeat \n ",
		  VTexture,group);

  theCommands.Add("vtexdefault",
		  "'vtexdefault NameOfShape' to set texture mapping default parameters \n",
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

  theCommands.Add("vpickselected", "vpickselected [name]: extract selected shape.",
    __FILE__, VPickSelected, group);

  theCommands.Add ("vloadselection",
    "vloadselection [-context] [name1] ... [nameN] : allows to load selection"
    "\n\t\t: primitives for the shapes with names given without displaying them."
    "\n\t\t:   -local - open local context before selection computation",
    __FILE__, VLoadSelection, group);

  theCommands.Add ("vautoactivatesel",
    "vautoactivatesel [0|1] : manage or display the option to automatically"
    "\n\t\t: activate selection for newly displayed objects"
    "\n\t\t:   [0|1] - turn off | on auto activation of selection",
    __FILE__, VAutoActivateSelection, group);

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
    cout <<"DONT_SWITCH_IS_VALID non positionne"<<"\n";
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
    di << "TEST : Plane is NULL" << "\n";
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
      di << "TEST : Can't find plane" << "\n";
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
    di << "TEST : NULL angle" << "\n";
    return 1;}

  if(Rev) anAngle = - anAngle;
  Draft.Add (Face, aDir, anAngle, aPln);
  Draft.Build ();
  if (!Draft.IsDone())  {
    di << "TEST : Draft Not DONE " << "\n";
    return 1;
  }
  TopTools_ListOfShape Larg;
  Larg.Append(Solid);
  if (!IsValid(Larg,Draft.Shape(),Standard_True,Standard_False)) {
    di << "TEST : DesignAlgo returns Not valid" << "\n";
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
  ViewerTest::AviCommands(theDI);

#ifdef OCCT_DEBUG
      theDI << "Draw Plugin : OCC V2d & V3d commands are loaded" << "\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(ViewerTest)
