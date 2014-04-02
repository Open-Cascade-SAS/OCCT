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
#include <TopLoc_Location.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <OSD_Timer.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Axis1Placement.hxx>
#include <gp_Trsf.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <StdSelect_ShapeTypeFilter.hxx>
#include <AIS.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_ColoredShape.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_Axis.hxx>
#include <AIS_Relation.hxx>
#include <AIS_TypeFilter.hxx>
#include <AIS_SignatureFilter.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_TextureRoot.hxx>
#include <Image_AlienPixMap.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_IsoAspect.hxx>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#include <Draw_Interpretor.hxx>
#include <TCollection_AsciiString.hxx>
#include <Draw_PluginMacro.hxx>
#include <ViewerTest.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(4:4190)
#pragma warning (disable:4996)
#endif

#include <NIS_InteractiveContext.hxx>
#include <NIS_Triangulated.hxx>
extern int ViewerMainLoop(Standard_Integer argc, const char** argv);

#include <Quantity_Color.hxx>
#include <Quantity_NameOfColor.hxx>

#include <Graphic3d_NameOfMaterial.hxx>

#define DEFAULT_COLOR    Quantity_NOC_GOLDENROD
#define DEFAULT_MATERIAL Graphic3d_NOM_BRASS


//=======================================================================
//function : GetColorFromName
//purpose  : get the Quantity_NameOfColor from a string
//=======================================================================

Quantity_NameOfColor ViewerTest::GetColorFromName (const Standard_CString theName)
{
  for (Standard_Integer anIter = Quantity_NOC_BLACK; anIter <= Quantity_NOC_WHITE; ++anIter)
  {
    Standard_CString aColorName = Quantity_Color::StringName (Quantity_NameOfColor (anIter));
    if (strcasecmp (theName, aColorName) == 0)
    {
      return Quantity_NameOfColor (anIter);
    }
  }

  return DEFAULT_COLOR;
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


//==============================================================================
//function : VDisplayAISObject
//purpose  : register interactive object in the map of AIS objects;
//           if other object with such name already registered, it will be kept
//           or replaced depending on value of <theReplaceIfExists>,
//           if "true" - the old object will be cleared from AIS context;
//           returns Standard_True if <theAISObj> registered in map;
//==============================================================================
Standard_EXPORT Standard_Boolean VDisplayAISObject (const TCollection_AsciiString& theName,
                                                    const Handle(AIS_InteractiveObject)& theAISObj,
                                                    Standard_Boolean theReplaceIfExists = Standard_True)
{
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    std::cout << "AIS context is not available.\n";
    return Standard_False;
  }

  if (aMap.IsBound2 (theName))
  {
    if (!theReplaceIfExists)
    {
      std::cout << "Other interactive object has been already "
                << "registered with name: " << theName << ".\n"
                << "Please use another name.\n";
      return Standard_False;
    }

    // stop displaying object
    Handle(AIS_InteractiveObject) anOldObj =
       Handle(AIS_InteractiveObject)::DownCast (aMap.Find2 (theName));

    if (!anOldObj.IsNull())
      aContextAIS->Clear (anOldObj, Standard_True);

    // remove name and old object from map
    aMap.UnBind2 (theName);
  }

  if (theAISObj.IsNull())
  {
    // object with specified name already unbound
    return Standard_True;
  }

  // unbind AIS object if was bound with another name
  aMap.UnBind1 (theAISObj);

  // can be registered without rebinding
  aMap.Bind (theAISObj, theName);
  aContextAIS->Display (theAISObj, Standard_True);
  return Standard_True;
}

static TColStd_MapOfInteger theactivatedmodes(8);
static TColStd_ListOfTransient theEventMgrs;

static void VwrTst_InitEventMgr(const Handle(NIS_View)& aView,
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

Standard_EXPORT const Handle(NIS_InteractiveContext)& TheNISContext()
{
  static Handle(NIS_InteractiveContext) aContext;
  if (aContext.IsNull()) {
    aContext = new NIS_InteractiveContext;
    aContext->SetSelectionMode (NIS_InteractiveContext::Mode_Normal);
  }
  return aContext;
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
  const Handle(NIS_View) aView =
    Handle(NIS_View)::DownCast(ViewerTest::CurrentView());
  VwrTst_InitEventMgr(aView, ViewerTest::GetAISContext());
}

Handle(ViewerTest_EventManager) ViewerTest::CurrentEventManager()
{
  Handle(ViewerTest_EventManager) EM;
  if(theEventMgrs.IsEmpty()) return EM;
  Handle(Standard_Transient) Tr =  theEventMgrs.First();
  EM = *((Handle(ViewerTest_EventManager)*)&Tr);
  return EM;
}


//=======================================================================
//function : Get Context and active viou..
//purpose  :
//=======================================================================
void GetCtxAndView(Handle(AIS_InteractiveContext)& Ctx,
		   Handle(V3d_View)& Viou)
{
  Ctx = ViewerTest::GetAISContext();
  if (!Ctx.IsNull())
  {
    const Handle(V3d_Viewer)& Vwr = Ctx->CurrentViewer();
    Vwr->InitActiveViews();
    if(Vwr->MoreActiveViews())
      Viou = Vwr->ActiveView();
  }
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
          retsh = *((Handle(AIS_Shape)*)&IO);
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
      if (it.Key1()->IsKind(STANDARD_TYPE(AIS_InteractiveObject))) {
        const Handle(AIS_InteractiveObject) anObj =
          Handle(AIS_InteractiveObject)::DownCast (it.Key1());
        TheAISContext()->Remove(anObj,Standard_False);
      } else if (it.Key1()->IsKind(STANDARD_TYPE(NIS_InteractiveObject))) {
        const Handle(NIS_InteractiveObject) anObj =
          Handle(NIS_InteractiveObject)::DownCast (it.Key1());
        TheNISContext()->Remove(anObj);
      }
      it.Next();
    }
    TheAISContext()->UpdateCurrentViewer();
//    TheNISContext()->UpdateViews();
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
      aContext->UnhilightCurrents(Standard_False);
      // Open a local Context in order to be able to select subshape from
      // the selected shape if any or for all if there is no selection
      if (!aContext->FirstCurrentObject().IsNull()){
	aContext->OpenLocalContext(Standard_False);

	for(aContext->InitCurrent();aContext->MoreCurrent();aContext->NextCurrent()){
	  aContext->Load(	aContext->Current(),-1,Standard_True);
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
        Handle(AIS_Drawer) CurDrawer = aShape->Attributes();
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

//==============================================================================
//function : VDispAreas,VDispSensitive,...
//purpose  : Redraw the view
//Draw arg : No args
//==============================================================================
static int VDispAreas (Draw_Interpretor& ,Standard_Integer , const char** )
{

  Handle(AIS_InteractiveContext) Ctx;
  Handle(V3d_View) Viou;
  GetCtxAndView(Ctx,Viou);
  Ctx->DisplayActiveAreas(Viou);
  return 0;
}
static  int VClearAreas (Draw_Interpretor& ,Standard_Integer , const char** )
{
  Handle(AIS_InteractiveContext) Ctx;
  Handle(V3d_View) Viou;
  GetCtxAndView(Ctx,Viou);
  Ctx->ClearActiveAreas(Viou);
  return 0;

}
static  int VDispSensi (Draw_Interpretor& ,Standard_Integer , const char** )
{
  Handle(AIS_InteractiveContext) Ctx;
  Handle(V3d_View) Viou;
  GetCtxAndView(Ctx,Viou);
  Ctx->DisplayActiveSensitive(Viou);
  return 0;

}
static  int VClearSensi (Draw_Interpretor& ,Standard_Integer , const char** )
{
  Handle(AIS_InteractiveContext) Ctx;
  Handle(V3d_View) Viou;
  GetCtxAndView(Ctx,Viou);
  Ctx->ClearActiveSensitive(Viou);
  return 0;
}

//==============================================================================
//function : VDebug
//purpose  : To list the displayed object with their attributes
//Draw arg : No args
//==============================================================================
static int VDebug(Draw_Interpretor& di, Standard_Integer , const char** )
{ if ( !a3DView().IsNull() ) {
     di << "List of object in the viewer :" << "\n";

    ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it(GetMapOfAIS());

    while ( it.More() ) {
	di << "\t" << it.Key2().ToCString() << "\n";
      	it.Next();
    }
  }

  return 0;
}

//==============================================================================
//function : VSelPrecision
//purpose  : To set the selection precision mode and tolerance value
//Draw arg : Selection precision mode (0 for window, 1 for view) and tolerance
//           value (integer number of pixel for window mode, double value of
//           sensitivity for view mode). Without arguments the function just
//           prints the current precision mode and the corresponding tolerance.
//==============================================================================
static int VSelPrecision(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if( argc > 3 )
  {
    di << "Use: " << argv[0] << " [precision_mode [tolerance_value]]\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if( aContext.IsNull() )
    return 1;

  if( argc == 1 )
  {
    StdSelect_SensitivityMode aMode = aContext->SensitivityMode();
    if( aMode == StdSelect_SM_WINDOW )
    {
      Standard_Integer aPixelTolerance = aContext->PixelTolerance();
      di << "Precision mode  : 0 (window)\n";
      di << "Pixel tolerance : " << aPixelTolerance << "\n";
    }
    else if( aMode == StdSelect_SM_VIEW )
    {
      Standard_Real aSensitivity = aContext->Sensitivity();
      di << "Precision mode : 1 (view)\n";
      di << "Sensitivity    : " << aSensitivity << "\n";
    }
  }
  else if( argc > 1 )
  {
    StdSelect_SensitivityMode aMode = ( StdSelect_SensitivityMode )Draw::Atoi( argv[1] );
    aContext->SetSensitivityMode( aMode );
    if( argc > 2 )
    {
      if( aMode == StdSelect_SM_WINDOW )
      {
        Standard_Integer aPixelTolerance = Draw::Atoi( argv[2] );
        aContext->SetPixelTolerance( aPixelTolerance );
      }
      else if( aMode == StdSelect_SM_VIEW )
      {
        Standard_Real aSensitivity = Draw::Atof( argv[2] );
        aContext->SetSensitivity( aSensitivity );
      }
    }
  }
  return 0;
}

//==============================================================================
//function : VDump
//purpose  : To dump the active view snapshot to image file
//Draw arg : Picture file name with extension corresponding to desired format
//==============================================================================
static Standard_Integer VDump (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2)
  {
    di<<"Use: "<<argv[0]<<" <filename>.{png|bmp|jpg|gif} [buffer={rgb|rgba|depth}] [width height]\n";
    return 1;
  }

  Graphic3d_BufferType aBufferType = Graphic3d_BT_RGB;
  if (argc > 2)
  {
    const char* aBuffTypeStr = argv[2];
    if ( strcasecmp( aBuffTypeStr, "rgb" ) == 0 ) // 4 is to compare '\0' as well
    {
      aBufferType = Graphic3d_BT_RGB;
    }
    else if ( strcasecmp( aBuffTypeStr, "rgba" ) == 0 )
    {
      aBufferType = Graphic3d_BT_RGBA;
    }
    else if ( strcasecmp( aBuffTypeStr, "depth" ) == 0 )
    {
      aBufferType = Graphic3d_BT_Depth;
    }
  }

  Standard_Integer aWidth  = (argc > 3) ? Draw::Atoi (argv[3]) : 0;
  Standard_Integer aHeight = (argc > 4) ? Draw::Atoi (argv[4]) : 0;

  Handle(AIS_InteractiveContext) IC;
  Handle(V3d_View) view;
  GetCtxAndView (IC, view);
  if (view.IsNull())
  {
    di << "Cannot find an active viewer/view\n";
    return 1;
  }

  if (aWidth <= 0 || aHeight <= 0)
  {
    if (!view->Dump (argv[1], aBufferType))
    {
      di << "Dumping failed!\n";
      return 1;
    }
    return 0;
  }

  Image_AlienPixMap aPixMap;
  if (!view->ToPixMap (aPixMap, aWidth, aHeight, aBufferType))
  {
    di << "Dumping failed!\n";
    return 1;
  }

  if (aPixMap.SizeX() != Standard_Size(aWidth)
   || aPixMap.SizeY() != Standard_Size(aHeight))
  {
    std::cout << "Warning! Dumped dimensions " << aPixMap.SizeX() << "x" << aPixMap.SizeY()
              << " are lesser than requested " << aWidth          << "x" << aHeight << "\n";
  }
  if (!aPixMap.Save (argv[1]))
  {
    di << "Saving image failed!\n";
    return 1;
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
      if(Ctx->NbCurrents()==0 ||
	 Ctx->NbSelected()==0){
	Handle(AIS_InteractiveObject) IO;
	VwrTst_DispErase(IO,-1,4,Standard_False);
      }
      else if(!Ctx->HasOpenedContext()){
      	for(Ctx->InitCurrent();Ctx->MoreCurrent();Ctx->NextCurrent())
	  VwrTst_DispErase(Ctx->Current(),-1,4,Standard_False);
      }
      else{
	for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
	  VwrTst_DispErase(Ctx->Interactive(),-1,4,Standard_False);}
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
    if(Ctx->NbCurrents()==0 && TypeOfOperation==3){
      Handle(AIS_InteractiveObject) IO;
      VwrTst_DispErase(IO,Dmode,TypeOfOperation,Standard_True);
    }
    if(!Ctx->HasOpenedContext()){
      // set/unset display mode sur le Contexte...
      for(Ctx->InitCurrent();Ctx->MoreCurrent();Ctx->NextCurrent()){
	VwrTst_DispErase(Ctx->Current(),Dmode,TypeOfOperation,Standard_False);
      }
      Ctx->UpdateCurrentViewer();
    }
    else{
      for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
	Ctx->Display(Ctx->Interactive(),Dmode);
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

  if(argc==2){

    if(!Ctx->HasOpenedContext()){
      di<<"sub intensite ";
      if(On==1) di<<"On";
      else di<<"Off";
      di<<" pour "<<Ctx->NbCurrents()<<"  objets"<<"\n";
      for(Ctx->InitCurrent();Ctx->MoreCurrent();Ctx->NextCurrent()){
	if(On==1){
	  Ctx->SubIntensityOn(Ctx->Current(),Standard_False);}
	else{
	  di <<"passage dans off"<<"\n";
	  Ctx->SubIntensityOff(Ctx->Current(),Standard_False);
	}
      }
    }
    else{
      for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected()){
	if(On==1){
	  Ctx->SubIntensityOn(Ctx->Interactive(),Standard_False);}
	else{
	  Ctx->SubIntensityOff(Ctx->Interactive(),Standard_False);}
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
    else if (aCtx->NbCurrents() > 0)
    {
      mySource  = IterSource_Selected;
      mySelIter = aCtx;
      mySelIter->InitCurrent();
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
      case IterSource_Selected: return mySelIter->MoreCurrent();
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
        mySelIter->NextCurrent();
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
        if (mySelIter->MoreCurrent())
        {
          myCurrentName = GetMapOfAIS().Find1 (mySelIter->Current());
          myCurrent     = mySelIter->Current();
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
static int VSetInteriorStyle (Draw_Interpretor& /*theDI*/,
                              Standard_Integer  theArgNb,
                              const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cerr << "Error: no active view!\n";
    return 1;
  }

  Standard_Integer anArgIter = 1;
  TCollection_AsciiString aName;
  if (theArgNb - anArgIter == 2)
  {
    aName = theArgVec[anArgIter++];
  }
  else if (theArgNb - anArgIter != 1)
  {
    std::cout << "Error: wrong number of arguments!\n";
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

  // update the screen and redraw the view
  TheAISContext()->UpdateCurrentViewer();
  return 0;
}

//! Auxiliary structure for VAspects
struct ViewerTest_AspectsChangeSet
{
  Standard_Integer         ToSetColor;
  Quantity_Color           Color;

  Standard_Integer         ToSetLineWidth;
  Standard_Real            LineWidth;

  Standard_Integer         ToSetTransparency;
  Standard_Real            Transparency;

  Standard_Integer         ToSetMaterial;
  Graphic3d_NameOfMaterial Material;
  TCollection_AsciiString  MatName;

  NCollection_Sequence<TopoDS_Shape> SubShapes;

  //! Empty constructor
  ViewerTest_AspectsChangeSet()
  : ToSetColor        (0),
    Color             (DEFAULT_COLOR),
    ToSetLineWidth    (0),
    LineWidth         (1.0),
    ToSetTransparency (0),
    Transparency      (0.0),
    ToSetMaterial     (0),
    Material (Graphic3d_NOM_DEFAULT) {}

  //! @return true if no changes have been requested
  Standard_Boolean IsEmpty() const
  {
    return ToSetLineWidth    == 0
        && ToSetTransparency == 0
        && ToSetColor        == 0
        && ToSetMaterial     == 0;
  }

  //! @return true if properties are valid
  Standard_Boolean Validate (const Standard_Boolean theIsSubPart) const
  {
    Standard_Boolean isOk = Standard_True;
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
  if (aCtx.IsNull())
  {
    std::cerr << "Error: no active view!\n";
    return 1;
  }

  Standard_Integer      anArgIter = 1;
  NCollection_Sequence<TCollection_AsciiString> aNames;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg = theArgVec[anArgIter];
    if (!anArg.IsEmpty()
      && anArg.Value (1) != '-')
    {
      aNames.Append (anArg);
    }
    else
    {
      break;
    }
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
    aChangeSet->Color  = ViewerTest::GetColorFromName (aNames.Last().ToCString());
    aNames.Remove (aNames.Length());
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
          || anArg == "-settransparancy")
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
          || anArg == "-unsettransparancy"
          || anArg == "-unsetalpha"
          || anArg == "-opaque")
    {
      aChangeSet->ToSetTransparency = -1;
      aChangeSet->Transparency = 0.0;
    }
    else if (anArg == "-setcolor")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      aChangeSet->ToSetColor = 1;
      aChangeSet->Color = ViewerTest::GetColorFromName (theArgVec[anArgIter]);
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
    else if (anArg == "-unset")
    {
      aChangeSet->ToSetLineWidth = -1;
      aChangeSet->LineWidth = 1.0;
      aChangeSet->ToSetTransparency = -1;
      aChangeSet->Transparency = 0.0;
      aChangeSet->ToSetColor = -1;
      aChangeSet->Color = DEFAULT_COLOR;
      aChangeSet->ToSetMaterial = -1;
      aChangeSet->Material = Graphic3d_NOM_DEFAULT;
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
  for (ViewTest_PrsIter aPrsIter (aNames); aPrsIter.More(); aPrsIter.Next())
  {
    const TCollection_AsciiString& aName = aPrsIter.CurrentName();
    Handle(AIS_InteractiveObject)  aPrs  = aPrsIter.Current();
    Handle(AIS_ColoredShape) aColoredPrs;
    Standard_Boolean toDisplay = Standard_False;
    if (aChanges.Length() > 1)
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
      if (aChangeSet->ToSetMaterial == 1)
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

      for (aChangesIter.Next(); aChangesIter.More(); aChangesIter.Next())
      {
        aChangeSet = &aChangesIter.ChangeValue();
        for (NCollection_Sequence<TopoDS_Shape>::Iterator aSubShapeIter (aChangeSet->SubShapes);
             aSubShapeIter.More(); aSubShapeIter.Next())
        {
          const TopoDS_Shape& aSubShape = aSubShapeIter.Value();
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
        }
      }
      if (toDisplay)
      {
        aCtx->Display (aPrs, Standard_False);
      }
      else if (!aColoredPrs.IsNull())
      {
        aColoredPrs->Redisplay();
      }
    }
    else
    {
      Handle(NIS_InteractiveObject) aNisObj = Handle(NIS_InteractiveObject)::DownCast (aPrsIter.CurrentTrs());
      Handle(NIS_Triangulated)      aNisTri = Handle(NIS_Triangulated)::DownCast (aNisObj);
      if (!aNisObj.IsNull())
      {
        if (aChangeSet->ToSetTransparency != 0)
        {
          aNisObj->SetTransparency (aChangeSet->Transparency);
        }
      }
      if (!aNisTri.IsNull())
      {
        if (aChangeSet->ToSetColor != 0)
        {
          aNisTri->SetColor (aChangeSet->Color);
        }
        if (aChangeSet->ToSetLineWidth != 0)
        {
          aNisTri->SetLineWidth (aChangeSet->LineWidth);
        }
      }
    }
  }

  // update the screen and redraw the view
  TheAISContext()->UpdateCurrentViewer();
  return 0;
}

//==============================================================================
//function : VDonly2
//author   : ege
//purpose  : Display only a selected or named  object
//           if there is no selected or named object s, nothing is donne
//Draw arg : vdonly2 [name1] ... [name n]
//==============================================================================
static int VDonly2(Draw_Interpretor& , Standard_Integer argc, const char** argv)

{
  if ( a3DView().IsNull() )
    return 1;

  Standard_Boolean ThereIsCurrent = TheAISContext() -> NbCurrents() > 0;
  Standard_Boolean ThereIsArgument= argc>1;

  if (TheAISContext()->HasOpenedContext())
    TheAISContext()->CloseLocalContext();

  //===============================================================
  // Il n'y a pas d'arguments mais des objets selectionnes(current)
  // dans le viewer
  //===============================================================
  if (!ThereIsArgument && ThereIsCurrent) {

#ifdef DEB
    ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
      it (GetMapOfAIS());
#endif
    for(TheAISContext()->InitCurrent();
	TheAISContext()->MoreCurrent();
	TheAISContext()->NextCurrent()){
      Handle(AIS_InteractiveObject) aShape = TheAISContext()->Current();
      TheAISContext()->Erase(aShape,Standard_False);}
  }
  TheAISContext() ->UpdateCurrentViewer();
  //===============================================================
  // Il y a des arguments
  //===============================================================
  if (ThereIsArgument) {
    ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
      it (GetMapOfAIS());
    while ( it.More() ) {
      if (it.Key1()->IsKind(STANDARD_TYPE(AIS_InteractiveObject))) {
        const Handle(AIS_InteractiveObject) aShape =
          Handle(AIS_InteractiveObject)::DownCast(it.Key1());
        TheAISContext()->Erase(aShape,Standard_False);
      } else if (it.Key1()->IsKind(STANDARD_TYPE(NIS_InteractiveObject))) {
        const Handle(NIS_InteractiveObject) aShape =
          Handle(NIS_InteractiveObject)::DownCast(it.Key1());
        TheNISContext()->Erase(aShape);
      }
      it.Next();
    }

    // On display les objets passes par parametre
    for (int i=1; i<argc ; i++) {
      TCollection_AsciiString name=argv[i];
      Standard_Boolean IsBound = GetMapOfAIS().IsBound2(name);
      if (IsBound) {
        const Handle(Standard_Transient) anObj = GetMapOfAIS().Find2(name);
        if (anObj->IsKind(STANDARD_TYPE(AIS_InteractiveObject))) {
          const Handle(AIS_InteractiveObject) aShape =
            Handle(AIS_InteractiveObject)::DownCast (anObj);
          TheAISContext()->Display(aShape, Standard_False);
        } else if (anObj->IsKind(STANDARD_TYPE(NIS_InteractiveObject))) {
          Handle(NIS_InteractiveObject) aShape =
            Handle(NIS_InteractiveObject)::DownCast (anObj);
          TheNISContext()->Display(aShape);
        }
      }
    }
    TheAISContext() ->UpdateCurrentViewer();
//    TheNISContext() ->UpdateViews();
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
  if (a3DView().IsNull())
  {
    return 1;
  }

  TheAISContext()->CloseAllContexts (Standard_False);

  //Standard_Boolean isStayedInMap = Standard_False;
  TCollection_AsciiString aContextOnlyStr ("-context");
  TCollection_AsciiString aRemoveAllStr ("-all");
  
  Standard_Boolean isContextOnly = (theArgNb > 1 && TCollection_AsciiString (theArgVec[1]) == aContextOnlyStr);
  
  Standard_Boolean isRemoveAll = (theArgNb == 3 && TCollection_AsciiString (theArgVec[2]) == aRemoveAllStr) ||
                                 (theArgNb == 2 && TCollection_AsciiString (theArgVec[1]) == aRemoveAllStr);
  
  NCollection_List<TCollection_AsciiString> anIONameList;
  
  if (isRemoveAll)
  {
    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
         anIter.More(); anIter.Next())
    {
      anIONameList.Append (anIter.Key2());
    }
  }
  else if (isContextOnly ? theArgNb > 2 : theArgNb > 1) // removed objects names are in argument list
  {
    for (Standard_Integer anIt = isContextOnly ? 2 : 1; anIt < theArgNb; ++anIt)
    {
      TCollection_AsciiString aName = theArgVec[anIt];

      if (!GetMapOfAIS().IsBound2 (aName))
      {
        theDI << aName.ToCString() << " was not bound to some object.\n";
        continue;
      }

      const Handle(Standard_Transient)& aTransientObj = GetMapOfAIS().Find2 (aName);

      const Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (aTransientObj);
      if (!anIO.IsNull())
      {
        if (anIO->GetContext() != TheAISContext())
        {
          theDI << aName.ToCString() << " was not displayed in current context.\n";
          theDI << "Please activate view with this object displayed and try again.\n";
          continue;
        }

        anIONameList.Append (aName);
        continue;
      }

      const Handle(NIS_InteractiveObject) aNisIO = Handle(NIS_InteractiveObject)::DownCast (aTransientObj);
      if (!aNisIO.IsNull())
      {
        anIONameList.Append (aName);
      }
    }
  }
  else if (TheAISContext()->NbCurrents() > 0
        || TheNISContext()->GetSelected().Extent() > 0)
  {
    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
         anIter.More(); anIter.Next())
    {
      const Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (anIter.Key1());
      if (!anIO.IsNull())
      {
        if (!TheAISContext()->IsCurrent (anIO))
        {
          continue;
        }

        anIONameList.Append (anIter.Key2());
        continue;
      }

      const Handle(NIS_InteractiveObject) aNisIO = Handle(NIS_InteractiveObject)::DownCast (anIter.Key1());
      if (!aNisIO.IsNull())
      {
        if (!TheNISContext()->IsSelected (aNisIO))
        {
          continue;
        }

        anIONameList.Append (anIter.Key2());
      }
    }
  }

  // Unbind all removed objects from the map of displayed IO.
  for (NCollection_List<TCollection_AsciiString>::Iterator anIter (anIONameList);
       anIter.More(); anIter.Next())
  {
      const Handle(AIS_InteractiveObject) anIO  = Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (anIter.Value()));
      
      if (!anIO.IsNull())
      {
        TheAISContext()->Remove (anIO, Standard_False);
        theDI << anIter.Value().ToCString() << " was removed\n";
      }
      else
      {
        const Handle(NIS_InteractiveObject) aNisIO = Handle(NIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (anIter.Value()));
        if (!aNisIO.IsNull())
        {
          TheNISContext()->Remove (aNisIO);
          theDI << anIter.Value().ToCString() << " was removed\n";
        }
      }
      
      if (!isContextOnly)
      {
        GetMapOfAIS().UnBind2 (anIter.Value());
      }
  }

  TheAISContext()->UpdateCurrentViewer();
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
  if (a3DView().IsNull())
  {
    return 1;
  }

  TheAISContext()->CloseAllContexts (Standard_False);
  const Standard_Boolean isEraseAll = TCollection_AsciiString (theArgNb > 0 ? theArgVec[0] : "").IsEqual ("veraseall");
  if (theArgNb > 1)
  {
    if (isEraseAll)
    {
      std::cerr << " Syntax error: " << theArgVec[0] << " too much arguments.\n";
      return 1;
    }

    // has a list of names
    for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
    {
      TCollection_AsciiString aName = theArgVec[anArgIter];
      if (!GetMapOfAIS().IsBound2 (aName))
      {
        continue;
      }

      const Handle(Standard_Transient)    anObj = GetMapOfAIS().Find2 (aName);
      const Handle(AIS_InteractiveObject) anIO  = Handle(AIS_InteractiveObject)::DownCast (anObj);
      theDI << aName.ToCString() << " ";
      if (!anIO.IsNull())
      {
        TheAISContext()->Erase (anIO, Standard_False);
      }
      else
      {
        const Handle(NIS_InteractiveObject) aNisIO = Handle(NIS_InteractiveObject)::DownCast (anObj);
        if (!aNisIO.IsNull())
        {
          TheNISContext()->Erase (aNisIO);
        }
      }
    }
    TheAISContext()->UpdateCurrentViewer();
    return 0;
  }

  if (!isEraseAll
   && TheAISContext()->NbCurrents() > 0)
  {
    // remove all currently selected objects
    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
         anIter.More(); anIter.Next())
    {
      const Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (anIter.Key1());
      if (!anIO.IsNull()
       && TheAISContext()->IsCurrent (anIO))
      {
        theDI << anIter.Key2().ToCString() << " ";
        TheAISContext()->Erase (anIO, Standard_False);
      }
    }

    TheAISContext()->UpdateCurrentViewer();
    return 0;
  }

  // erase entire viewer
  for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (GetMapOfAIS());
       anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (anIter.Key1());
    if (!anIO.IsNull())
    {
      TheAISContext()->Erase (anIO, Standard_False);
    }
    else
    {
      const Handle(NIS_InteractiveObject) aNisIO = Handle(NIS_InteractiveObject)::DownCast (anIter.Key1());
      if (!aNisIO.IsNull())
      {
        TheNISContext()->Erase (aNisIO);
      }
    }
  }
  TheAISContext()->UpdateCurrentViewer();
  return 0;
}

//==============================================================================
//function : VDisplayAll
//author   : ege
//purpose  : Display all the objects of the Map
//Draw arg : vdisplayall
//==============================================================================
static int VDisplayAll(	Draw_Interpretor& di, Standard_Integer argc, const char** argv)

{
  if (! a3DView().IsNull() ) {
    if (argc >  1) {di<<argv[0]<<" Syntaxe error"<<"\n"; return 1;}
    if (TheAISContext()->HasOpenedContext())
      TheAISContext()->CloseLocalContext();
    ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
      it (GetMapOfAIS());
    while ( it.More() ) {
      if (it.Key1()->IsKind(STANDARD_TYPE(AIS_InteractiveObject))) {
        const Handle(AIS_InteractiveObject) aShape =
          Handle(AIS_InteractiveObject)::DownCast(it.Key1());
        TheAISContext()->Erase(aShape,Standard_False);
      } else if (it.Key1()->IsKind(STANDARD_TYPE(NIS_InteractiveObject))) {
        const Handle(NIS_InteractiveObject) aShape =
          Handle(NIS_InteractiveObject)::DownCast(it.Key1());
        TheNISContext()->Erase(aShape);
      }
      it.Next();
    }
    it.Reset();
    while ( it.More() ) {
      if (it.Key1()->IsKind(STANDARD_TYPE(AIS_InteractiveObject))) {
        const Handle(AIS_InteractiveObject) aShape =
          Handle(AIS_InteractiveObject)::DownCast(it.Key1());
        TheAISContext()->Display(aShape, Standard_False);
      } else if (it.Key1()->IsKind(STANDARD_TYPE(NIS_InteractiveObject))) {
        Handle(NIS_InteractiveObject) aShape =
          Handle(NIS_InteractiveObject)::DownCast(it.Key1());
        TheNISContext()->Display(aShape);
      }
      it.Next();
    }
    TheAISContext() ->UpdateCurrentViewer();
//    TheNISContext() ->UpdateViews();
  }
  return 0;
}

//#######################################################################################################

//##     VTexture

//#######################################################################################################

Standard_Integer VTexture (Draw_Interpretor& di,Standard_Integer argc, const char** argv )
{
  Standard_Integer command = (strcasecmp(argv[0],"vtexture")==0)? 1:
    (strcasecmp(argv[0],"vtexscale")==0) ? 2 :
      (strcasecmp(argv[0],"vtexorigin")==0) ? 3 :
	(strcasecmp(argv[0],"vtexrepeat")==0) ? 4 :
	  (strcasecmp(argv[0],"vtexdefault")==0) ? 5 : -1;

  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull())
    {
      di << "use 'vinit' command before " << argv[0] << "\n";
      return 1;
    }

  Handle(AIS_InteractiveObject) TheAisIO;
  Handle(AIS_TexturedShape) myShape;
  Standard_Integer myPreviousMode = 0;

  if (argc<2 || !argv[1])
    {
      di << argv[0] <<" syntax error - Type 'help vtex'"<<"\n";
      return 1;
    }

  TCollection_AsciiString name = argv[1];
  ViewerTest::CurrentView()->SetSurfaceDetail(V3d_TEX_ALL);

  if (GetMapOfAIS().IsBound2(name))
    TheAisIO = Handle(AIS_InteractiveObject)::DownCast
      (GetMapOfAIS().Find2(name));
  if (TheAisIO.IsNull())
  {
    di <<"shape "<<name.ToCString()<<" doesn\'t exist"<<"\n";
    return 1;
  }

  if (TheAisIO->IsKind(STANDARD_TYPE(AIS_TexturedShape)) && !TheAisIO.IsNull())
    {
      myShape = Handle(AIS_TexturedShape)::DownCast(TheAisIO);
      myPreviousMode = myShape->DisplayMode() ;
    }
  else
    {
      myAISContext->Clear(TheAisIO,Standard_False);
      myShape = new AIS_TexturedShape (DBRep::Get(argv[1]));
      GetMapOfAIS().UnBind1(TheAisIO);
      GetMapOfAIS().UnBind2(name);
      GetMapOfAIS().Bind(myShape, name);
    }
  switch (command)
    {
    case 1: // vtexture : we only map a texture on the shape
      if(argc<=1)
	{
	  di << argv[0] <<" syntax error - Type 'help vtex'" << "\n";
	  return 1;
	}
      if (argc>2 && argv[2])
	{
	  if(strcasecmp(argv[2],"?")==0)
	    {
	      TCollection_AsciiString monPath = Graphic3d_TextureRoot::TexturesFolder();
	      di<<"\n Files in current directory : \n"<<"\n";
	      TCollection_AsciiString Cmnd ("glob -nocomplain *");
	      di.Eval(Cmnd.ToCString());

	      Cmnd = TCollection_AsciiString("glob -nocomplain ") ;
	      Cmnd += monPath ;
	      Cmnd += "/* " ;
	      di<<"Files in "<<monPath.ToCString()<<" : \n"<<"\n";
	      di.Eval(Cmnd.ToCString());

	      return 0;
	    }
	  else
	    myShape->SetTextureFileName(argv[2]);
	}
      else
	{
	  di <<"Texture mapping disabled \n \
                  To enable it, use 'vtexture NameOfShape NameOfTexture' \n"<<"\n";

	  myAISContext->SetDisplayMode(myShape,1,Standard_False);
	  if (myPreviousMode == 3 )
	    myAISContext->RecomputePrsOnly(myShape);
	  myAISContext->Display(myShape, Standard_True);
	  return 0;
	}
      break;

    case 2: // vtexscale : we change the scaling factor of the texture

      if(argc<2)
	{
	  di << argv[0] <<" syntax error - Type 'help vtex'" << "\n";
	  return 1;
	}

      myShape->SetTextureScale (( argv[2] ? Standard_True    : Standard_False ),
				( argv[2] ? Draw::Atof(argv[2])    : 1.0 ),
				( argv[2] ? Draw::Atof(argv[argc-1]) : 1.0 ) );
      break;

    case 3: // vtexorigin : we change the origin of the texture on the shape
      if(argc<2)
	{
	  di << argv[0] <<" syntax error - Type 'help vtex'" << "\n";
	  return 1;
	}
      myShape->SetTextureOrigin (( argv[2] ? Standard_True    : Standard_False ),
				 ( argv[2] ? Draw::Atof(argv[2])    : 0.0 ),
				 ( argv[2] ? Draw::Atof(argv[argc-1]) : 0.0 ));
      break;

    case 4: // vtexrepeat : we change the number of occurences of the texture on the shape
      if(argc<2)
	{
	  di << argv[0] <<" syntax error - Type 'help vtex'" << "\n";
	  return 1;
	}
      if (argc>2 && argv[2])
	{
	  di <<"Texture repeat enabled"<<"\n";
	  myShape->SetTextureRepeat(Standard_True, Draw::Atof(argv[2]), Draw::Atof(argv[argc-1]) );
	}
      else
	{
	  di <<"Texture repeat disabled"<<"\n";
	  myShape->SetTextureRepeat(Standard_False);
	}
      break;

    case 5: // vtexdefault : default texture mapping
      // ScaleU = ScaleV = 100.0
      // URepeat = VRepeat = 1.0
      // Uorigin = VOrigin = 0.0

      if(argc<2)
	{
	  di << argv[0] <<" syntax error - Type 'help vtex'" << "\n";
	  return 1;
	}
      myShape->SetTextureRepeat(Standard_False);
      myShape->SetTextureOrigin(Standard_False);
      myShape->SetTextureScale (Standard_False);
      break;
    }

  if ((myShape->DisplayMode() == 3) || (myPreviousMode == 3 ))
    myAISContext->RecomputePrsOnly(myShape);
  else
    {
      myAISContext->SetDisplayMode(myShape,3,Standard_False);
      myAISContext->Display(myShape, Standard_True);
      myAISContext->Update(myShape,Standard_True);
    }
  return 0;
}

//==============================================================================
//function : VDisplay2
//author   : ege
//purpose  : Display an object from its name
//Draw arg : vdisplay name1 [name2] ... [name n]
//==============================================================================
static int VDisplay2 (Draw_Interpretor& di, Standard_Integer argc, const char** argv)

{
  // Verification des arguments
  if ( a3DView().IsNull() ) {

    ViewerTest::ViewerInit();
    di<<"La commande vinit n'a pas ete appele avant"<<"\n";
    //    return 1;
  }

  if ( argc<2) {
    di<<argv[0]<<" Syntaxe error"<<"\n";
    return 1;
  }


  TCollection_AsciiString name;
  if (TheAISContext()->HasOpenedContext())
    TheAISContext()->CloseLocalContext();

  for ( int i = 1; i < argc; i++ ) {
    name = argv[i];
    Standard_Boolean IsBound      = GetMapOfAIS().IsBound2(name);
    Standard_Boolean IsDatum = Standard_False;
    Handle(Standard_Transient) anObj;
    if ( IsBound ) {
      anObj = GetMapOfAIS().Find2(name);
      if (anObj->IsKind(STANDARD_TYPE(AIS_InteractiveObject))) {
        Handle(AIS_InteractiveObject) aShape =
          Handle(AIS_InteractiveObject)::DownCast(anObj);
        if (aShape->Type()==AIS_KOI_Datum) {
          IsDatum  = Standard_True;
          TheAISContext()->Display(aShape, Standard_False);
        }
        if (!IsDatum) {
          di << "Display " << name.ToCString() << "\n";
          // Get the Shape from a name
          TopoDS_Shape NewShape = GetShapeFromName((const char *)name.ToCString());

          // Update the Shape in the AIS_Shape
          Handle(AIS_Shape) TheRealSh = Handle(AIS_Shape)::DownCast(aShape);
          if(!TheRealSh.IsNull())
            TheRealSh->Set(NewShape);
          TheAISContext()->Redisplay(aShape, Standard_False);
          TheAISContext()->Display(aShape, Standard_False);
        }
        aShape.Nullify();
      } else if (anObj->IsKind(STANDARD_TYPE(NIS_InteractiveObject))) {
        Handle(NIS_InteractiveObject) aShape =
          Handle(NIS_InteractiveObject)::DownCast(anObj);
        TheNISContext()->Display(aShape);
      }
    }
    else { // Create the AIS_Shape from a name
      const Handle(AIS_InteractiveObject) aShape =
        GetAISShapeFromName((const char *)name.ToCString());
      if ( !aShape.IsNull() ) {
        GetMapOfAIS().Bind(aShape, name);
        TheAISContext()->Display(aShape, Standard_False);
      }
    }
  }
  // Upadate the screen and redraw the view
  TheAISContext()->UpdateCurrentViewer();
//  TheNISContext()->UpdateViews();
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

  TheAISContext()->SetColor(myAisCylinderHead, Quantity_NOC_INDIANRED);
  TheAISContext()->SetColor(myAisEngineBlock , Quantity_NOC_RED);
  TheAISContext()->SetColor(myAisPropeller   , Quantity_NOC_GREEN);

  TheAISContext()->Display(myAisCylinderHead,Standard_False);
  TheAISContext()->Display(myAisEngineBlock,Standard_False );
  TheAISContext()->Display(myAisCrankArm,Standard_False    );
  TheAISContext()->Display(myAisPropeller,Standard_False);

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

  TopoDS_Shape myNewCrankArm  =myAisCrankArm ->Shape().Located( myAisCrankArm ->Location() );
  TopoDS_Shape myNewPropeller =myAisPropeller->Shape().Located( myAisPropeller->Location() );

  myAisCrankArm ->ResetLocation();
  myAisPropeller->ResetLocation();

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
        if (TheAISContext()->NbCurrents()>0 ){
          TheAISContext()->UnhilightCurrents(Standard_False);

          for (TheAISContext()->InitCurrent(); TheAISContext()->MoreCurrent(); TheAISContext()->NextCurrent() ){
            ListOfIO.Append(TheAISContext()->Current() );
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
	    Handle(AIS_InteractiveObject) aIO=TheAISContext()->Interactive();


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



//==============================================================================
// function : WhoAmI
// user : vState
//==============================================================================
void WhoAmI (const Handle(AIS_InteractiveObject )& theShape ,Draw_Interpretor& di) {

  // AIS_Datum
  if (theShape->Type()==AIS_KOI_Datum) {
    if      (theShape->Signature()==3 ) { di<<"  AIS_Trihedron"; }
    else if (theShape->Signature()==2 ) { di<<"  AIS_Axis"; }
    else if (theShape->Signature()==6 ) { di<<"  AIS_Circle"; }
    else if (theShape->Signature()==5 ) { di<<"  AIS_Line"; }
    else if (theShape->Signature()==7 ) { di<<"  AIS_Plane"; }
    else if (theShape->Signature()==1 ) { di<<"  AIS_Point"; }
    else if (theShape->Signature()==4 ) { di<<"  AIS_PlaneTrihedron"; }
  }
  // AIS_Shape
  else if (theShape->Type()==AIS_KOI_Shape && theShape->Signature()==0 ) { di<<"  AIS_Shape"; }
  // AIS_Dimentions et AIS_Relations
  else if (theShape->Type()==AIS_KOI_Relation) {
    Handle(AIS_Relation) TheShape= ((*(Handle(AIS_Relation)*)&theShape));

    if      (TheShape->KindOfDimension()==AIS_KOD_PLANEANGLE)      {di<<"  AIS_AngleDimension";}
    else if (TheShape->KindOfDimension()==AIS_KOD_LENGTH )         {di<<"  AIS_Chamf2/3dDimension/AIS_LengthDimension ";  }
    else if (TheShape->KindOfDimension()==AIS_KOD_DIAMETER  )      {di<<"  AIS_DiameterDimension ";}
    else if (TheShape->KindOfDimension()==AIS_KOD_ELLIPSERADIUS  ) {di<<"  AIS_EllipseRadiusDimension ";}
    //else if (TheShape->KindOfDimension()==AIS_KOD_FILLETRADIUS  )  {di<<" AIS_FilletRadiusDimension "<<endl;}
    else if (TheShape->KindOfDimension()==AIS_KOD_OFFSET  )        {di<<"  AIS_OffsetDimension ";}
    else if (TheShape->KindOfDimension()==AIS_KOD_RADIUS  )        {di<<"  AIS_RadiusDimension ";}
    // AIS no repertorie.
    else {di<<"  Type Unknown.";}
  }
}



//==============================================================================
//function : VState
//purpose  :
//Draw arg : vstate [nameA] ... [nameN]
//==============================================================================
static int VState(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  TheAISContext()->CloseAllContexts();
  const Standard_Boolean ThereIsArguments=(argc>=2);
  const Standard_Boolean ThereIsCurrent=(TheAISContext()->NbCurrents()>0);
  // Debut...
  // ==================
  // Il y a un argument
  // ==================
  if (ThereIsArguments) {
    for (int cpt=1;cpt<argc;cpt++) {
      // Verification que lq piece est bien bindee.
      if (GetMapOfAIS().IsBound2(argv[cpt]) ) {
	const Handle(AIS_InteractiveObject) theShape=
          Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2
                                                  (argv[cpt]));
        if (!theShape.IsNull()) {
          di<<argv[cpt];WhoAmI(theShape, di);
          if (TheAISContext()->IsDisplayed(theShape) ) {
            di<<"    Displayed"<<"\n";
          }
          else {
            di<<"    Not Displayed"<<"\n";
          }
        }
      }
      else {
	di<<"vstate error: Shape "<<cpt<<" doesn't exist;" <<"\n";return 1;
      }
    }
  }
  else if (ThereIsCurrent ) {
    for (TheAISContext() -> InitCurrent() ;
         TheAISContext() -> MoreCurrent() ;
         TheAISContext() ->NextCurrent() )
    {
      Handle(AIS_InteractiveObject) theShape=TheAISContext()->Current();
      di<<GetMapOfAIS().Find1(theShape).ToCString(); WhoAmI(theShape, di);
      if (TheAISContext()->IsDisplayed(theShape) ) {
	di<<"    Displayed"<<"\n";
      }
      else {
	di<<"    Not Displayed"<<"\n";
      }

    }
  }
  else {
    ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
      it(GetMapOfAIS());
    while ( it.More() ) {
      Handle(AIS_InteractiveObject) theShape =
        Handle(AIS_InteractiveObject)::DownCast(it.Key1());
      if (!theShape.IsNull()) {
        di<<it.Key2().ToCString();
        WhoAmI(theShape, di);
        if (TheAISContext()->IsDisplayed(theShape) ) {
          di<<"    Displayed"<<"\n";
        }
        else {
          di<<"    Not Displayed"<<"\n";
        }
      }
      it.Next();
    }
  }

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
      result = (*((Handle(AIS_Shape)*) &IO))->Shape();
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
      thearr->SetValue(i,(*((Handle(AIS_Shape)*) &IO))->Shape());
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
      AIS_KindOfDimension KOD = (*((Handle(AIS_Relation)*)&curio))->KindOfDimension();
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
      AIS_KindOfDimension KOD = (*((Handle(AIS_Relation)*)&curio))->KindOfDimension();
      if ((dimension_status==0 && KOD == AIS_KOD_NONE)||
	  (dimension_status==1 && KOD != AIS_KOD_NONE))
	TheAISContext()->Display(curio,Standard_False);
    }

  }

  TheAISContext()->UpdateCurrentViewer();
  return 0;
}

//==============================================================================
//function : VSetTransMode
//purpose  :
//Draw arg : vsettransmode shape flag1 [flag2] [flag3] [X Y Z]
//==============================================================================

static int VSetTransMode ( Draw_Interpretor& di, Standard_Integer argc, const char** argv ) {
  // Verification des arguments
  if ( a3DView().IsNull() ) {
    ViewerTest::ViewerInit();
    di << "La commande vinit n'a pas ete appele avant" << "\n";
  }

  if ( argc < 3 || argc > 8 ) {
    di << argv[0] << " Invalid number of arguments" << "\n";
    return 1;
  }

  TCollection_AsciiString shapeName;
  shapeName = argv[1];
  Standard_Integer persFlag1 = Draw::Atoi(argv[2]);
  Standard_Integer persFlag2 = 0;
  Standard_Integer persFlag3 = 0;
  gp_Pnt origin = gp_Pnt( 0.0, 0.0, 0.0 );
  if ( argc == 4 || argc == 5 || argc == 7 || argc == 8 ) {
    persFlag2 = Draw::Atoi(argv[3]);
  }
  if ( argc == 5 || argc == 8 ) {
    persFlag3 = Draw::Atoi(argv[4]);
  }
  if ( argc >= 6 ) {
    origin.SetX( Draw::Atof(argv[argc - 3]) );
    origin.SetY( Draw::Atof(argv[argc - 2]) );
    origin.SetZ( Draw::Atof(argv[argc - 1]) );
  }

  Standard_Boolean IsBound = GetMapOfAIS().IsBound2(shapeName);
  Handle(Standard_Transient) anObj;
  if ( IsBound ) {
    anObj = GetMapOfAIS().Find2(shapeName);
    if ( anObj->IsKind(STANDARD_TYPE(AIS_InteractiveObject)) ) {
      Handle(AIS_InteractiveObject) aShape = Handle(AIS_InteractiveObject)::DownCast(anObj);
      aShape->SetTransformPersistence( (persFlag1 | persFlag2 | persFlag3), origin );
      if ( persFlag1 == 0 && persFlag2 == 0 && persFlag3 == 0 ) {
        di << argv[0] << " All persistence modifiers were removed" << "\n";
      }
    } else {
      di << argv[0] << " Wrong object type" << "\n";
      return 1;
    }
  } else { // Create the AIS_Shape from a name
    const Handle(AIS_InteractiveObject) aShape = GetAISShapeFromName((const char* )shapeName.ToCString());
    if ( !aShape.IsNull() ) {
      GetMapOfAIS().Bind( aShape, shapeName );
      aShape->SetTransformPersistence( (persFlag1 | persFlag2 | persFlag3), origin );
      TheAISContext()->Display( aShape, Standard_False );
    } else {
      di << argv[0] << " Object not found" << "\n";
      return 1;
    }
  }

  // Upadate the screen and redraw the view
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
		  "vdisplay name1 [name2] ... [name n]"
      "\n\t\t: Displays named objects",
		  __FILE__,VDisplay2,group);

  theCommands.Add ("vupdate",
      "vupdate name1 [name2] ... [name n]"
      "\n\t\t: Updates named objects in interactive context",
      __FILE__, VUpdate, group);

  theCommands.Add("verase",
      "verase [name1] ...  [name n]"
      "\n\t\t: Erases selected or named objects."
      "\n\t\t: If there are no selected or named objects the whole viewer is erased.",
		  __FILE__, VErase, group);

  theCommands.Add("vremove",
    "vremove [-context] [name1] ...  [name n]"
    "or vremove [-context] -all to remove all objects"
      "\n\t\t: Removes selected or named objects."
      "\n\t\t  If -context is in arguments, the objects are not deleted"
      "\n\t\t  from the map of objects and names.",
      __FILE__, VRemove, group);

  theCommands.Add("vdonly",
		  "vdonly [name1] ...  [name n]"
      "\n\t\t: Displays only selected or named objects",
		  __FILE__,VDonly2,group);

  theCommands.Add("vdisplayall",
		  "Displays all erased interactive objects (see vdir and vstate)",
		  __FILE__,VDisplayAll,group);

  theCommands.Add("veraseall",
		  "Erases all objects displayed in the viewer",
		  __FILE__, VErase, group);

  theCommands.Add("verasetype",
		  "verasetype <Type>"
      "\n\t\t: Erase all the displayed objects of one given kind (see vtypes)",
		  __FILE__,VEraseType,group);

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
		  __FILE__,VDebug,group);

  theCommands.Add("vdump",
    #ifdef HAVE_FREEIMAGE
      "<filename>.{png|bmp|jpg|gif} [buffer={rgb|rgba|depth}] [width height]"
      "\n\t\t: Dumps contents of viewer window to PNG, BMP, JPEG or GIF file",
    #else
      "<filename>.{ppm} [buffer={rgb|rgba|depth}] [width height]"
      "\n\t\t: Dumps contents of viewer window to PPM image file",
    #endif
		  __FILE__,VDump,group);

  theCommands.Add("vsub",      "vsub 0/1 (off/on) [obj]        : Subintensity(on/off) of selected objects",
		  __FILE__,VSubInt,group);

  theCommands.Add("vaspects",
              "vaspects [name1 [name2 [...]]]"
      "\n\t\t:          [-setcolor ColorName] [-unsetcolor]"
      "\n\t\t:          [-setmaterial MatName] [-unsetmaterial]"
      "\n\t\t:          [-settransparency Transp] [-unsettransparancy]"
      "\n\t\t:          [-setwidth LineWidth] [-unsetwidth]"
      "\n\t\t:          [-subshapes subname1 [subname2 [...]]]"
      "\n\t\t: Manage presentation properties of all, selected or named objects."
      "\n\t\t: When -subshapes is specified than following properties will be"
      "\n\t\t: assigned to specified sub-shapes.",
		  __FILE__,VAspects,group);

  theCommands.Add("vsetcolor",
      "vsetcolor [name] ColorName"
      "\n\t\t: Sets color for all, selected or named objects."
      "\n\t\t: Alias for vaspects -setcolor [name] ColorName.",
		  __FILE__,VAspects,group);

  theCommands.Add("vunsetcolor",
		  "vunsetcolor [-noupdate|-update] [name]"
      "\n\t\t: Resets color for all, selected or named objects."
      "\n\t\t: Alias for vaspects -unsetcolor [name].",
		  __FILE__,VAspects,group);

  theCommands.Add("vsettransparency",
		  "vsettransparency [name] Coefficient"
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
		  "vsetmaterial [name] MaterialName"
      "\n\t\t: Alias for vaspects -setmaterial [name] MaterialName.",
		  __FILE__,VAspects,group);

  theCommands.Add("vunsetmaterial",
		  "vunsetmaterial [name]"
      "\n\t\t: Alias for vaspects -unsetmaterial [name].",
		  __FILE__,VAspects,group);

  theCommands.Add("vsetwidth",
		  "vsetwidth [name] width(0->10)"
      "\n\t\t: Alias for vaspects -setwidth [name] width.",
		  __FILE__,VAspects,group);

  theCommands.Add("vunsetwidth",
		  "vunsetwidth [name]"
      "\n\t\t: Alias for vaspects -unsetwidth [name] width.",
		  __FILE__,VAspects,group);

  theCommands.Add("vsetinteriorstyle",
		  "vsetinteriorstyle [name] style"
      "\n\t\t: Where style is: 0 = EMPTY, 1 = HOLLOW, 2 = HATCH, 3 = SOLID, 4 = HIDDENLINE.",
		  __FILE__,VSetInteriorStyle,group);

  theCommands.Add("vardis",
		  "vardis          : display activeareas",
		  __FILE__,VDispAreas,group);

  theCommands.Add("varera",
		  "varera           : erase activeareas",
		  __FILE__,VClearAreas,group);

  theCommands.Add("vsensdis",
		  "vardisp           : display active entities",
		  __FILE__,VDispSensi,group);
  theCommands.Add("vsensera",
		  "vardisp           : erase  active entities",
		  __FILE__,VClearSensi,group);

  theCommands.Add("vselprecision",
		  "vselprecision : vselprecision [precision_mode [tolerance_value]]",
		  __FILE__,VSelPrecision,group);

  theCommands.Add("vperf",
		  "vperf: vperf  ShapeName 1/0(Transfo/Location) 1/0(Primitives sensibles ON/OFF)",
		  __FILE__,VPerf,group);

  theCommands.Add("vanimation",
		  "vanimation CrankArmFile CylinderHeadFile PropellerFile EngineBlockFile",
		  __FILE__,VAnimation,group);

  theCommands.Add("vsetshading",
		  "vsetshading  : vsetshading name Quality(default=0.0008) ",
		  __FILE__,VShading,group);

  theCommands.Add("vunsetshading",
		  "vunsetshading :vunsetshading name ",
		  __FILE__,VShading,group);

  theCommands.Add("vtexture",
		  "'vtexture NameOfShape TextureFile' \n \
                  or 'vtexture NameOfShape' if you want to disable texture mapping \n \
                  or 'vtexture NameOfShape ?' to list available textures\n \
                  or 'vtexture NameOfShape IdOfTexture' (0<=IdOfTexture<=20)' to use predefined  textures\n ",
		  __FILE__,VTexture,group);

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
		  "vsetActivatedModes: vsetam mode(1->7)  ",
		  __FILE__,VActivatedMode,group);

  theCommands.Add("vunsetam",
		  "vunsetActivatedModes:   vunsetam  ",
		  __FILE__,VActivatedMode,group);

  theCommands.Add("vstate",
      "vstate [name1] ... [nameN]"
      "\n\t\t: Reports show/hidden state for selected or named objects",
		  __FILE__,VState,group);

  theCommands.Add("vpickshapes",
		  "vpickshape subtype(VERTEX,EDGE,WIRE,FACE,SHELL,SOLID) [name1 or .] [name2 or .] [name n or .]",
		  __FILE__,VPickShape,group);

  theCommands.Add("vtypes",
		  "vtypes : list of known types and signatures in AIS - To be Used in vpickobject command for selection with filters",
		  VIOTypes,group);

  theCommands.Add("vsettransmode",
		  "vsettransmode   : vsettransmode shape flag1 [flag2] [flag3] [X Y Z]",
		  __FILE__,VSetTransMode,group);

  theCommands.Add("vr", "vr : reading of the shape",
		  __FILE__,vr, group);

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
#ifdef DEB
    cout <<"DONT_SWITCH_IS_VALID positionnee a :"<<checkValid.ToCString()<<"\n";
#endif
    if ( checkValid=="true" || checkValid=="TRUE" ) {
      ToCheck= Standard_False;
    }
  } else {
#ifdef DEB
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
//purpose  : Split parameter string to parameter name an parameter value
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

#ifdef DEB
      theDI << "Draw Plugin : OCC V2d & V3d commands are loaded" << "\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(ViewerTest)
