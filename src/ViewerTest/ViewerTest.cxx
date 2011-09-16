// File:	ViewerTest.cxx
// Created:	Wed Jul 23 14:14:01 1997
// Author:	Henri JEANNIN
//		<bbl@entrax.paris3.matra-dtv.fr>
// Modified by  Eric Gouthiere [sep-oct 98] -> add commands for display...
//
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
#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <OSD_Timer.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Axis1Placement.hxx>
#include <gp_Trsf.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <StdSelect_ShapeTypeFilter.hxx>
#include <AIS.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_Axis.hxx>
#include <AIS_Relation.hxx>
#include <AIS_TypeFilter.hxx>
#include <AIS_SignatureFilter.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Image_PixMap.hxx>

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
#include <Viewer2dTest.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(4:4190)
#pragma warning (disable:4996)
#endif

#include <NIS_InteractiveContext.hxx>
#include <NIS_Triangulated.hxx>
extern int ViewerMainLoop(Standard_Integer argc, const char** argv);

//=======================================================================
//function : GetColorFromName
//purpose  : get the Quantity_NameOfColor from a string
//=======================================================================

#include <Quantity_NameOfColor.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_NameOfColor.hxx>

#include <Graphic3d_NameOfMaterial.hxx>

#define DEFAULT_COLOR    Quantity_NOC_GOLDENROD
#define DEFAULT_MATERIAL Graphic3d_NOM_BRASS

static Quantity_NameOfColor GetColorFromName( const char *name )
{
  Quantity_NameOfColor ret = DEFAULT_COLOR;

  Standard_Boolean Found = Standard_False;
  Standard_CString colstring;
  for(Standard_Integer i=0;i<=514 && !Found;i++)
    {
      colstring = Quantity_Color::StringName(Quantity_NameOfColor(i));
      if (!strcasecmp(name,colstring)) {
	ret = (Quantity_NameOfColor)i;
	Found = Standard_True;
      }
    }

  return ret;
}

//=======================================================================
//function : GetMaterialFromName
//purpose  : get the Graphic3d_NameOfMaterial from a string
//=======================================================================

static Graphic3d_NameOfMaterial GetMaterialFromName( const char *name )
{
  Graphic3d_NameOfMaterial mat = DEFAULT_MATERIAL;

  if      ( !strcasecmp(name,"BRASS" ) ) 	 mat = Graphic3d_NOM_BRASS;
  else if ( !strcasecmp(name,"BRONZE" ) )        mat = Graphic3d_NOM_BRONZE;
  else if ( !strcasecmp(name,"COPPER" ) ) 	 mat = Graphic3d_NOM_COPPER;
  else if ( !strcasecmp(name,"GOLD" ) ) 	 mat = Graphic3d_NOM_GOLD;
  else if ( !strcasecmp(name,"PEWTER" ) ) 	 mat = Graphic3d_NOM_PEWTER;
  else if ( !strcasecmp(name,"SILVER" ) ) 	 mat = Graphic3d_NOM_SILVER;
  else if ( !strcasecmp(name,"STEEL" ) ) 	 mat = Graphic3d_NOM_STEEL;
  else if ( !strcasecmp(name,"METALIZED" ) ) 	 mat = Graphic3d_NOM_METALIZED;
  else if ( !strcasecmp(name,"STONE" ) ) 	 mat = Graphic3d_NOM_STONE;
  else if ( !strcasecmp(name,"CHROME" ) )	 mat = Graphic3d_NOM_CHROME;
  else if ( !strcasecmp(name,"ALUMINIUM" ) )     mat = Graphic3d_NOM_ALUMINIUM;
  else if ( !strcasecmp(name,"STONE" ) )	 mat = Graphic3d_NOM_STONE;
  else if ( !strcasecmp(name,"NEON_PHC" ) )	 mat = Graphic3d_NOM_NEON_PHC;
  else if ( !strcasecmp(name,"NEON_GNC" ) )	 mat = Graphic3d_NOM_NEON_GNC;
  else if ( !strcasecmp(name,"PLASTER" ) )	 mat = Graphic3d_NOM_PLASTER;
  else if ( !strcasecmp(name,"SHINY_PLASTIC" ) ) mat = Graphic3d_NOM_SHINY_PLASTIC;
  else if ( !strcasecmp(name,"SATIN" ) )	 mat = Graphic3d_NOM_SATIN;
  else if ( !strcasecmp(name,"PLASTIC" ) )	 mat = Graphic3d_NOM_PLASTIC;
  else if ( !strcasecmp(name,"OBSIDIAN" ) )	 mat = Graphic3d_NOM_OBSIDIAN;
  else if ( !strcasecmp(name,"JADE" ) )	         mat = Graphic3d_NOM_JADE;

  return mat;
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
//function : GetTypeNameFromShape
//purpose  : get the shape type as a string from a shape
//==============================================================================

static const char* GetTypeNameFromShape( const TopoDS_Shape& aShape )
{ const char *ret = "????";

  if ( aShape.IsNull() ) ret = "Null Shape";

	    switch ( aShape.ShapeType() ) {
	      case TopAbs_COMPOUND  : ret = "COMPOUND" ; break;
	      case TopAbs_COMPSOLID : ret = "COMPSOLID" ; break;
	      case TopAbs_SOLID     : ret = "SOLID" ; break;
	      case TopAbs_SHELL     : ret = "SHELL" ; break;
	      case TopAbs_FACE      : ret = "FACE" ; break;
	      case TopAbs_WIRE      : ret = "WIRE" ; break;
	      case TopAbs_EDGE      : ret = "EDGE" ; break;
	      case TopAbs_VERTEX    : ret = "VERTEX" ; break;
	      case TopAbs_SHAPE     : ret = "SHAPE" ; break;
	    }
  return ret;
}
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

static Handle(V3d_View)&  a3DView(){
  static Handle(V3d_View) Viou;
  return Viou;
}

Standard_EXPORT Handle(AIS_InteractiveContext)& TheAISContext(){
  static Handle(AIS_InteractiveContext) aContext;
  return aContext;
}

Handle(V3d_View) ViewerTest::CurrentView()
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

Handle(AIS_InteractiveContext) ViewerTest::GetAISContext()
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
//function : GetShapeFromName
//purpose  : Compute an Shape from a draw variable or a file name
//==============================================================================
// Unused :
#ifdef DEB
static TopoDS_Shape GetShapeFromAIS(const AIS_InteractiveObject & TheAisIO )
{
  TopoDS_Shape TheShape=((*(Handle(AIS_Shape)*)&TheAisIO))->Shape();
  return TheShape;
}
#endif
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
      if(IO->Type()==AIS_KOI_Shape)
        if(IO->Signature()==0){
          retsh = *((Handle(AIS_Shape)*)&IO);
        }
        else
          cout << "an Object which is not an AIS_Shape "
            "already has this name!!!"<<endl;
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
    case 7: cmode = "Compound"; break;
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
//function : SelectFromContext
//purpose  : pick / select an object from the last MoveTo() on a
//            ButtonPress event
//==============================================================================

Handle(AIS_InteractiveObject) Select(Standard_Integer argc,
				     const char** argv,
				     Standard_Boolean shift,
				     Standard_Boolean pick )
{
  Handle(AIS_InteractiveObject) ret;
  Handle (ViewerTest_EventManager) EM = ViewerTest::CurrentEventManager();
  if ( shift ) {
    EM->ShiftSelect();
  }
  else {
    EM->Select();
  }
  const Handle(AIS_InteractiveContext) aContext = EM->Context();

  if ( !aContext->HasOpenedContext() ) {
    aContext->InitCurrent();
    while ( aContext->MoreCurrent() ) {
      Handle(AIS_InteractiveObject) aisPickedShape =
	Handle(AIS_InteractiveObject)::DownCast(aContext->Current());

//JR/Hp
      const char *name = (GetMapOfAIS().IsBound1(aisPickedShape))?
//      const char *name = (GetMapOfAIS().IsBound1(aisPickedShape))?
	GetMapOfAIS().Find1(aisPickedShape).ToCString() :
	  "????";
      Handle(AIS_Shape) TheRealSh = Handle(AIS_Shape)::DownCast(aisPickedShape);
      if(!TheRealSh.IsNull()){
	cout << "Current is " << name
	  << " (" << GetTypeNameFromShape(TheRealSh->Shape())
	    << ")" << endl;
      }
      ret = aisPickedShape;
      if(!TheRealSh.IsNull()){
	if ( pick && argc > 4 ) {
	  DBRep::Set(argv[4], TheRealSh->Shape());
	}
      }
      aContext->NextCurrent();
    }
  }
  else {
    // A LocalContext is opened, the use xxxxSelected()
    // to select an object and its SubShape
    aContext->InitSelected();
    while ( aContext->MoreSelected() ) {
      if ( !aContext->HasSelectedShape() ) {
      }
      else {
        TopoDS_Shape PickedShape = aContext->SelectedShape();
	if ( pick && argc > 5 ) {
	  DBRep::Set(argv[5], PickedShape);
	}
      }

      if ( aContext->Interactive().IsNull() ) {
        cout << "??? (No InteractiveObject selected)" << endl;
      }
      else {
        Handle(AIS_InteractiveObject) aisPicked =
          Handle(AIS_InteractiveObject)::DownCast(aContext->Interactive());
        ret = aisPicked;
	Handle(AIS_Shape) aisPickedShape = Handle(AIS_Shape)::DownCast(aisPicked);

	// Get back its name
//JR/Hp
	const char *name = ( GetMapOfAIS().IsBound1(aisPicked) )?
//	const char *name = ( GetMapOfAIS().IsBound1(aisPicked) )?
	  GetMapOfAIS().Find1(aisPicked).ToCString() :
	    "????";

	if(!aisPickedShape.IsNull()){
	  if ( pick && argc > 4 ) {
	    // Create a draw variable to store the wohole shape
	    // for vpick command
	    DBRep::Set(argv[4], aisPickedShape->Shape());
	  }

	  cout << name << " (" << GetTypeNameFromShape(aisPickedShape->Shape())
	    << ")" << endl  ;
	}
      }
      // Goto the next selected object
      aContext->NextSelected();
    }
  }
  return ret;
}

//==============================================================================
//function : DetectedFromContext
//purpose  : hilight dynamicaly an object from the last MoveTo() on a
//            MouseMove event
//==============================================================================
Handle(AIS_InteractiveObject) DetectedFromContext(
	Handle(AIS_InteractiveContext) aContext )
{
  Handle(AIS_InteractiveObject) ret;
  if ( aContext->HasDetected() ) {
    if ( !aContext->HasDetectedShape() ) {
      //No SubShape selected
    }
    else {
      // Get the detected SubShape
      TopoDS_Shape PickedShape = aContext->DetectedShape();
    }
    if ( !aContext->DetectedInteractive().IsNull() ) {
      Handle(AIS_InteractiveObject) aisPickedShape =
	Handle(AIS_InteractiveObject)::DownCast(aContext->DetectedInteractive());
      ret = aisPickedShape;
#ifdef DEB
      const char *name =
#endif
                   (  GetMapOfAIS().IsBound1(aisPickedShape) )?
	GetMapOfAIS().Find1(aisPickedShape).ToCString() :
	 (char *)  "????";
    }
  }
  return ret;
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

  Image_TypeOfImage aBufferType = Image_TOI_RGB;

  if (argc > 2)
  {
    TCollection_AsciiString aBuffTypeStr (argv[2]);
    if (TCollection_AsciiString::ISSIMILAR (aBuffTypeStr, TCollection_AsciiString ("rgb")))
    {
      aBufferType = Image_TOI_RGB;
    }
    else if (TCollection_AsciiString::ISSIMILAR (aBuffTypeStr, TCollection_AsciiString ("rgba")))
    {
      aBufferType = Image_TOI_RGBA;
    }
    else if (TCollection_AsciiString::ISSIMILAR (aBuffTypeStr, TCollection_AsciiString ("depth")))
    {
      aBufferType = Image_TOI_FLOAT;
    }
  }

  Standard_Integer aWidth  = (argc > 3) ? atoi (argv[3]) : 0;
  Standard_Integer aHeight = (argc > 4) ? atoi (argv[4]) : 0;

  Handle(AIS_InteractiveContext) IC;
  Handle(V3d_View) view;
  GetCtxAndView (IC, view);
  if (!view.IsNull())
  {
    if (aWidth > 0 && aHeight > 0)
    {
      return view->ToPixMap (aWidth, aHeight, aBufferType)->Dump (argv[1]) ? 0 : 1;
    }
    else
    {
      return view->Dump (argv[1], aBufferType) ? 0 : 1;
    }
  }
  else
  {
    di << "Cannot find an active viewer/view" << "\n";
    return 1;
  }
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
    Ctx->Erase(IO,Mode,Upd);
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
    Standard_Integer Dmode = atoi(argv[1]);
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
      VwrTst_DispErase(IO,atoi(argv[2]),TypeOfOperation,Standard_True);
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
  Standard_Integer On = atoi(argv[1]);
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
      if (!IO.IsNull())
        if(On==1)
          Ctx->SubIntensityOn(IO);
        else
          Ctx->SubIntensityOff(IO);

    }
    else return 1;
  }
  return 0;

}
//==============================================================================
//function : VColor2
//Author   : ege
//purpose  : change the color of a selected or named or displayed shape
//Draw arg : vcolor2 [name] color
//==============================================================================
static int VColor2 (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{

  Standard_Boolean    ThereIsCurrent;
  Standard_Boolean    ThereIsArgument;
  Standard_Boolean    IsBound = Standard_False ;
  Standard_Boolean    HaveToSet;

  if (!strcasecmp( argv[0],"vsetcolor")) HaveToSet=Standard_True;
  else HaveToSet=Standard_False;
  if (HaveToSet) {
    if ( argc<2 || argc > 3 ) { di << argv[0] << " syntax error: Passez 2 ou 3 arguments" << "\n"; return 1; }
    if ( argc == 2 ) {ThereIsArgument=Standard_False;}
    else ThereIsArgument=Standard_True;

  }
  else {
    if ( argc > 2 ) { di << argv[0] << " syntax error: Passez au plus un argument" << "\n"; return 1; }
    if(argc==2) ThereIsArgument=Standard_True;
    else ThereIsArgument=Standard_False;
  }

  if ( !a3DView().IsNull() ) {
    TCollection_AsciiString name;
    if (ThereIsArgument) {
      name = argv[1];
      IsBound= GetMapOfAIS().IsBound2(name);
    }
    if (TheAISContext()->HasOpenedContext())
      TheAISContext()->CloseLocalContext();

    //  On set le Booleen There is current
    if (TheAISContext() -> NbCurrents() > 0  ) {ThereIsCurrent =Standard_True; }
    else ThereIsCurrent =Standard_False;

    //=======================================================================
    // Il y a un  argument
    //=======================================================================
    if ( ThereIsArgument && IsBound ) {
      const Handle(Standard_Transient) anObj = GetMapOfAIS().Find2(name);
      if (anObj->IsKind(STANDARD_TYPE(AIS_InteractiveObject))) {
        Handle(AIS_InteractiveObject) ashape =
          Handle(AIS_InteractiveObject)::DownCast (anObj);
#ifdef DEB
          //cout  << "HaveToSet "<<HaveToSet <<" Color Given "<< argv[2] << " Color returned "<< GetColorFromName(argv[2]) << endl;
          if (HaveToSet)
            di  << "HaveToSet "<< "1" <<" Color Given "<< argv[2] << " Color returned "<< GetColorFromName(argv[2]) << "\n";
          else
            di  << "HaveToSet "<< "0" <<" Color Given "<< argv[2] << " Color returned "<< GetColorFromName(argv[2]) << "\n";
#endif

        if(HaveToSet) {
          TheAISContext()->SetColor(ashape,GetColorFromName(argv[2]) );
        }
        else
          TheAISContext()->UnsetColor(ashape);
      } else if (anObj->IsKind(STANDARD_TYPE(NIS_InteractiveObject))) {
        Handle(NIS_Triangulated) ashape =
          Handle(NIS_Triangulated)::DownCast (anObj);
        if (ashape.IsNull() == Standard_False)
          ashape->SetColor (GetColorFromName(argv[2]));
      }
    }


    //=======================================================================
    // Il n'y a pas d'arguments
    // Mais un ou plusieurs objets on des current representation
    //=======================================================================
    if (ThereIsCurrent && !ThereIsArgument) {
      for (TheAISContext() -> InitCurrent() ;
           TheAISContext() -> MoreCurrent() ;
           TheAISContext() ->NextCurrent() )
      {
	const Handle(AIS_InteractiveObject) ashape= TheAISContext()->Current();
        if (ashape.IsNull())
          continue;
#ifdef DEB
	//cout  << "HaveToSet "<<HaveToSet <<" Color Given "<< argv[2] << " Color returned "<< GetColorFromName(argv[2]) << endl;
	if (HaveToSet)
	  di  << "HaveToSet "<< "1" <<" Color Given "<< argv[2] << " Color returned "<< GetColorFromName(argv[2]) << "\n";
	else
	  di  << "HaveToSet "<< "0" <<" Color Given "<< argv[2] << " Color returned "<< GetColorFromName(argv[2]) << "\n";
#endif
	if(HaveToSet)
	  TheAISContext()->SetColor(ashape,GetColorFromName(argv[1]),Standard_False);
	else
	  TheAISContext()->UnsetColor(ashape,Standard_False);
      }

      TheAISContext()->UpdateCurrentViewer();
    }

    //=======================================================================
    // Il n'y a pas d'arguments(nom de shape) ET aucun objet courrant
    // on impose a tous les objets du viewer la couleur passee
    //=======================================================================
    else if (!ThereIsCurrent && !ThereIsArgument){
      ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it(GetMapOfAIS());
      while ( it.More() ) {
	const Handle(AIS_InteractiveObject) ashape =
          Handle(AIS_InteractiveObject)::DownCast(it.Key1());
        if (!ashape.IsNull())
          if(HaveToSet)
            TheAISContext()->SetColor(ashape,GetColorFromName(argv[1]),
                                      Standard_False);
          else
            TheAISContext()->UnsetColor(ashape,Standard_False);
	it.Next();
      }
      TheAISContext()->UpdateCurrentViewer();
    }
  }
  return 0;
}

//==============================================================================
//function : VTransparency
//Author   : ege
//purpose  : change the transparency of a selected or named or displayed shape
//Draw arg : vtransparency [name] TransparencyCoeficient
//==============================================================================


static int VTransparency  (Draw_Interpretor& di, Standard_Integer argc,
                           const char** argv)
{
  Standard_Boolean    ThereIsCurrent;
  Standard_Boolean    ThereIsArgument;
  Standard_Boolean    IsBound = Standard_False ;
  Standard_Boolean    HaveToSet;
  if (!strcasecmp( argv[0],"vsettransparency"))
    HaveToSet=Standard_True;
  else
    HaveToSet=Standard_False;

  if (HaveToSet) {
    if ( argc < 2 || argc > 3 ) { di << argv[0] << " syntax error passez 1 ou 2 arguments" << "\n"; return 1; }
    if ( argc == 2 ) {ThereIsArgument=Standard_False;}
    else ThereIsArgument=Standard_True;
  }
  else{
    if ( argc > 2 ) { di << argv[0] << " syntax error: Passez au plus un argument" << "\n"; return 1; }
    if(argc==2) ThereIsArgument=Standard_True;
    else ThereIsArgument=Standard_False;
  }

  if ( !a3DView().IsNull() ) {
    TCollection_AsciiString name;
    if (ThereIsArgument) {
      name = argv[1];
      IsBound= GetMapOfAIS().IsBound2(name);
    }
    if (TheAISContext()->HasOpenedContext())
      TheAISContext()->CloseLocalContext();

    if (TheAISContext() -> NbCurrents() > 0  ) {ThereIsCurrent =Standard_True; }
    else ThereIsCurrent = Standard_False;

    //=======================================================================
    // Il y a des arguments: un nom et une couleur
    //=======================================================================
    if ( ThereIsArgument && IsBound ) {
      const Handle(Standard_Transient) anObj = GetMapOfAIS().Find2(name);
      if (anObj->IsKind(STANDARD_TYPE(AIS_InteractiveObject))) {
        const Handle(AIS_InteractiveObject) ashape =
          Handle(AIS_InteractiveObject)::DownCast(anObj);
        if(HaveToSet)
          TheAISContext()->SetTransparency(ashape,atof(argv[2]) );
        else
          TheAISContext()->UnsetTransparency(ashape);
} else if (anObj->IsKind(STANDARD_TYPE(NIS_InteractiveObject))) {
        const Handle(NIS_InteractiveObject) ashape =
          Handle(NIS_InteractiveObject)::DownCast(anObj);
        if(HaveToSet)
          ashape->SetTransparency(atof(argv[2]) );
        else
          ashape->UnsetTransparency();
      }
    }
    //=======================================================================
    // Il n'y a pas d'arguments
    // Mais un ou plusieurs objets on des current representation
    //=======================================================================
    if (ThereIsCurrent && !ThereIsArgument) {
      for (TheAISContext() -> InitCurrent() ;
           TheAISContext() -> MoreCurrent() ;
           TheAISContext() ->NextCurrent() )
      {
	Handle(AIS_InteractiveObject) ashape =  TheAISContext() -> Current();
	if(HaveToSet) {
	  TheAISContext()->SetTransparency(ashape,atof(argv[1]),Standard_False);
	}
	else
	  TheAISContext()->UnsetTransparency(ashape,Standard_False);
      }

      TheAISContext()->UpdateCurrentViewer();
    }
    //=======================================================================
    // Il n'y a pas d'arguments ET aucun objet courrant
    //=======================================================================
    else if ( !ThereIsCurrent && !ThereIsArgument ) {
      ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
        it(GetMapOfAIS());
      while ( it.More() ) {
	Handle(AIS_InteractiveObject) ashape =
          Handle(AIS_InteractiveObject)::DownCast(it.Key1());
        if (!ashape.IsNull())
          if(HaveToSet)
            TheAISContext()->SetTransparency(ashape,atof(argv[1]),
                                             Standard_False);
          else
            TheAISContext()->UnsetTransparency(ashape,Standard_False);
	it.Next();
      }
      TheAISContext()->UpdateCurrentViewer();
    }
  }
  return 0;
}


//==============================================================================
//function : VMaterial
//Author   : ege
//purpose  : change the Material of a selected or named or displayed shape
//Draw arg : vmaterial  [Name] Material
//==============================================================================
static int VMaterial (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{

  Standard_Boolean    ThereIsCurrent;
  Standard_Boolean    ThereIsName;
  Standard_Boolean    IsBound = Standard_False ;

  Standard_Boolean    HaveToSet;
  if (!strcasecmp( argv[0],"vsetmaterial")) HaveToSet=Standard_True;
  else HaveToSet=Standard_False;
  if (HaveToSet) {
    if ( argc < 2 || argc > 3 ) { di << argv[0] << " syntax error passez 1 ou 2 arguments" << "\n"; return 1; }
    if ( argc == 2 ) {ThereIsName=Standard_False;}
    else ThereIsName=Standard_True;
  }
  else {
    if ( argc>2 ) { di << argv[0] << " syntax error passez au plus un argument" << "\n"; return 1; }
    if (argc==2) ThereIsName=Standard_True;
    else ThereIsName=Standard_False;

  }

  if ( !a3DView().IsNull() ) {
    TCollection_AsciiString name;
    if (ThereIsName) {
      name = argv[1];
      IsBound= GetMapOfAIS().IsBound2(name);
    }
    if (TheAISContext()->HasOpenedContext())
      TheAISContext()->CloseLocalContext();
    if (TheAISContext() -> NbCurrents() > 0  )
      ThereIsCurrent =Standard_True;
    else
      ThereIsCurrent =Standard_False;

    //=======================================================================
    // Ther is a name of shape and a material name
    //=======================================================================
    if ( ThereIsName && IsBound ) {
      Handle(AIS_InteractiveObject) ashape =
        Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(name));
      if (!ashape.IsNull())
        if (HaveToSet)
          TheAISContext()->SetMaterial (ashape,
                                        GetMaterialFromName (argv[2]) );
        else
          TheAISContext()->UnsetMaterial(ashape);
    }
    //=======================================================================
    // Il n'y a pas de nom de shape
    // Mais un ou plusieurs objets on des current representation
    //=======================================================================
    if (ThereIsCurrent && !ThereIsName) {
      for (TheAISContext() -> InitCurrent() ;
           TheAISContext() -> MoreCurrent() ;
           TheAISContext() ->NextCurrent() )
      {
	Handle(AIS_InteractiveObject) ashape =  TheAISContext() -> Current();
	if (HaveToSet)
	  TheAISContext()->SetMaterial(ashape,GetMaterialFromName(argv[1]),
                                       Standard_False);
	else
	  TheAISContext()->UnsetMaterial(ashape,Standard_False);
      }
      TheAISContext()->UpdateCurrentViewer();
    }

    //=======================================================================
    // Il n'y a pas de noms de shape ET aucun objet courrant
    // On impose a tous les objets du viewer le material passe en argument
    //=======================================================================
    else if (!ThereIsCurrent && !ThereIsName){
      ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
        it(GetMapOfAIS());
      while ( it.More() ) {
	Handle(AIS_InteractiveObject) ashape =
          Handle(AIS_InteractiveObject)::DownCast (it.Key1());
	if (!ashape.IsNull())
          if (HaveToSet)
	  TheAISContext()->SetMaterial(ashape,GetMaterialFromName(argv[1]),
                                       Standard_False );
          else
            TheAISContext()->UnsetMaterial(ashape,Standard_False);
	it.Next();
      }
      TheAISContext()->UpdateCurrentViewer();
    }
  }
  return 0;
}



//==============================================================================
//function : VWidth
//Author   : ege
//purpose  : change the width of the edges of a selected or named or displayed shape
//Draw arg : vwidth  [Name] WidthValue(1->10)
//==============================================================================
static int VWidth (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{

  Standard_Boolean    ThereIsCurrent;
  Standard_Boolean    ThereIsArgument;
  Standard_Boolean    IsBound = Standard_False ;

  Standard_Boolean    HaveToSet;
  if (!strcasecmp( argv[0],"vsetwidth")) HaveToSet=Standard_True;
  else HaveToSet=Standard_False;
  if (HaveToSet) {
    if ( argc < 2 || argc > 3 ) { di << argv[0] << " syntax error passez 1 ou 2 arguments" << "\n"; return 1; }
    if ( argc == 2 ) {ThereIsArgument=Standard_False;}
    else ThereIsArgument=Standard_True;
  }
  else {
    if ( argc>2 ) { di << argv[0] << " syntax error passez au plus 1  argument" << "\n"; return 1; }
   if (argc==2) ThereIsArgument=Standard_True;
    else ThereIsArgument=Standard_False;
  }
  if ( !a3DView().IsNull() ) {
    TCollection_AsciiString name;
    if (ThereIsArgument) {
      name = argv[1];
      IsBound= GetMapOfAIS().IsBound2(name);
    }
    if (TheAISContext()->HasOpenedContext())
      TheAISContext()->CloseLocalContext();

    if (TheAISContext() -> NbCurrents() > 0  )
      ThereIsCurrent =Standard_True;
    else
      ThereIsCurrent =Standard_False;

    if ( ThereIsArgument && IsBound ) {
      const Handle(Standard_Transient) anObj = GetMapOfAIS().Find2(name);
      if (anObj->IsKind(STANDARD_TYPE(AIS_InteractiveObject))) {
        const Handle(AIS_InteractiveObject) ashape =
          Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(name));
        if (HaveToSet)
          TheAISContext()->SetWidth ( ashape,atof (argv[2]) );
        else
          TheAISContext()->UnsetWidth (ashape);
      } else if (anObj->IsKind(STANDARD_TYPE(NIS_InteractiveObject))) {
        const Handle(NIS_Triangulated) ashape =
          Handle(NIS_Triangulated)::DownCast(GetMapOfAIS().Find2(name));
        if (HaveToSet && !ashape.IsNull())
          ashape->SetLineWidth ( atof (argv[2]) );
      }
    }

    //=======================================================================
    // Il n'y a pas d'arguments
    // Mais un ou plusieurs objets on des current representation
    //=======================================================================
    if (ThereIsCurrent && !ThereIsArgument) {
      for (TheAISContext() -> InitCurrent() ;
           TheAISContext() -> MoreCurrent() ;
           TheAISContext() ->NextCurrent() )
      {
	Handle(AIS_InteractiveObject) ashape =  TheAISContext() -> Current();
	if (HaveToSet)
	  TheAISContext()->SetWidth(ashape,atof (argv[1]),Standard_False);
	else
	  TheAISContext()->UnsetWidth (ashape,Standard_False);

      }
      TheAISContext()->UpdateCurrentViewer();
    }
    //=======================================================================
    // Il n'y a pas d'arguments ET aucun objet courrant
    //=======================================================================
    else if (!ThereIsCurrent && !ThereIsArgument){
     ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
       it(GetMapOfAIS());
     while ( it.More() ) {
       Handle(AIS_InteractiveObject) ashape =
         Handle(AIS_InteractiveObject)::DownCast (it.Key1());
       if (!ashape.IsNull())
         if (HaveToSet)
           TheAISContext()->SetWidth(ashape,atof (argv[1]),Standard_False );
         else
           TheAISContext()->UnsetWidth (ashape,Standard_False);
       it.Next();
     }
     TheAISContext()->UpdateCurrentViewer();
   }
  }
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
//function : VErase2
//author   : ege
//purpose  : Erase some  selected or named  objects
//           if there is no selected or named objects, the whole viewer is erased
//Draw arg : verase2 [name1] ... [name n]
//==============================================================================
static int VErase2(Draw_Interpretor& , 	Standard_Integer argc, 	const char** argv)

{
  if ( a3DView().IsNull() )
    return 1;

  Standard_Boolean ThereIsCurrent = TheAISContext() -> NbCurrents() > 0;
  Standard_Boolean ThereIsArgument= argc>1;
  if(TheAISContext()->HasOpenedContext())
    TheAISContext()->CloseLocalContext();

  //===============================================================
  // Il n'y a pas d'arguments mais des objets selectionnes(current)
  // dans le viewer
  //===============================================================
  if (!ThereIsArgument && ThereIsCurrent) {
    ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
      it (GetMapOfAIS());
    while ( it.More() ) {
      if (it.Key1()->IsKind(STANDARD_TYPE(AIS_InteractiveObject))) {
        const Handle(AIS_InteractiveObject) aShape =
          Handle(AIS_InteractiveObject)::DownCast(it.Key1());
        if (TheAISContext()->IsCurrent(aShape))
          TheAISContext()->Erase(aShape,Standard_False);
      }
      it.Next();
    }

    TheAISContext() ->UpdateCurrentViewer();
  }

  //===============================================================
  // Il n'y a pas d'arguments et aucuns objets selectionnes
  // dans le viewer:
  // On erase tout le viewer
  //===============================================================

  if (!ThereIsArgument && !ThereIsCurrent) {
    ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS());
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
    TheAISContext() ->UpdateCurrentViewer();
//    TheNISContext()->UpdateViews();
  }

  //===============================================================
  // Il y a des arguments
  //===============================================================
  if (ThereIsArgument) {
    for (int i=1; i<argc ; i++) {
      TCollection_AsciiString name=argv[i];
      Standard_Boolean IsBound= GetMapOfAIS().IsBound2(name);
      if (IsBound) {
        const Handle(Standard_Transient) anObj = GetMapOfAIS().Find2(name);
        if (anObj->IsKind(STANDARD_TYPE(AIS_InteractiveObject))) {
          const Handle(AIS_InteractiveObject) aShape =
            Handle(AIS_InteractiveObject)::DownCast (anObj);
          TheAISContext()->Erase(aShape,Standard_False);
        } else if (anObj->IsKind(STANDARD_TYPE(NIS_InteractiveObject))) {
          const Handle(NIS_InteractiveObject) aShape =
            Handle(NIS_InteractiveObject)::DownCast (anObj);
          TheNISContext()->Erase(aShape);
        }
      }
    }
    TheAISContext() ->UpdateCurrentViewer();
//    TheNISContext() ->UpdateViews();
  }
  return 0;
}

//==============================================================================
//function : VEraseAll
//author   : ege
//purpose  : Erase all the objects displayed in the viewer
//Draw arg : veraseall
//==============================================================================
static int VEraseAll(Draw_Interpretor& di, Standard_Integer argc, const char** argv)

{
  // Verification des arguments
  if (argc>1){ di<<" Syntaxe error: "<<argv[0]<<" too much arguments."<<"\n";return 1;}
  if (a3DView().IsNull() ) {di<<" Error: vinit hasn't been called."<<"\n";return 1;}
  TheAISContext()->CloseAllContexts(Standard_False);
  ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
    it(GetMapOfAIS());
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
  TheAISContext() ->UpdateCurrentViewer();
//  TheNISContext() ->UpdateViews();
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


static TCollection_AsciiString GetEnvir (Draw_Interpretor& di) {

  static Standard_Boolean IsDefined=Standard_False ;
  static TCollection_AsciiString VarName;
  if ( !IsDefined ) {
    const char *envir, *casroot ;
    envir = getenv("CSF_MDTVTexturesDirectory") ;

    Standard_Boolean HasDefinition = Standard_False ;
    if ( !envir ) {
      casroot  = getenv("CASROOT");
      if ( casroot ) {
	VarName = TCollection_AsciiString  (casroot);
	VarName += "/src/Textures" ;
	HasDefinition = Standard_True ;
      }
    } else {
      VarName = TCollection_AsciiString  (envir);
      HasDefinition = Standard_True ;
    }
    if ( HasDefinition ) {
      OSD_Path aPath ( VarName );
      OSD_Directory aDir(aPath);
      if ( aDir.Exists () ) {
	TCollection_AsciiString aTexture = VarName + "/2d_MatraDatavision.rgb" ;
	OSD_File TextureFile ( aTexture );
	if ( !TextureFile.Exists() ) {
	  di << " CSF_MDTVTexturesDirectory or CASROOT not correctly setted " << "\n";
	  di << " not all files are found in : "<<VarName.ToCString() << "\n";
	  Standard_Failure::Raise ( "CSF_MDTVTexturesDirectory or CASROOT not correctly setted " );
	}
      } else {
	di << " CSF_MDTVTexturesDirectory or CASROOT not correctly setted " << "\n";
	di << " Directory : "<< VarName.ToCString() << " not exist " << "\n";
	Standard_Failure::Raise ( "CSF_MDTVTexturesDirectory or CASROOT not correctly setted " );
      }
      return VarName ;
    } else {
      di << " CSF_MDTVTexturesDirectory and CASROOT not setted " << "\n";
      di << " one of these variable are mandatory to use this fonctionnality" << "\n";
      Standard_Failure::Raise ( "CSF_MDTVTexturesDirectory and CASROOT not setted " );
    }
    IsDefined = Standard_True ;
  }
  return VarName ;

}

//#######################################################################################################


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
	      TCollection_AsciiString monPath = GetEnvir (di) ;

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
				( argv[2] ? atof(argv[2])    : 1.0 ),
				( argv[2] ? atof(argv[argc-1]) : 1.0 ) );
      break;

    case 3: // vtexorigin : we change the origin of the texture on the shape
      if(argc<2)
	{
	  di << argv[0] <<" syntax error - Type 'help vtex'" << "\n";
	  return 1;
	}
      myShape->SetTextureOrigin (( argv[2] ? Standard_True    : Standard_False ),
				 ( argv[2] ? atof(argv[2])    : 0.0 ),
				 ( argv[2] ? atof(argv[argc-1]) : 0.0 ));
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
	  myShape->SetTextureRepeat(Standard_True, atof(argv[2]), atof(argv[argc-1]) );
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

//==============================================================================
//function : VMoveA
//purpose  : Test the annimation of an object along a
//           predifined trajectory
//Draw arg : vmove ShapeName
//==============================================================================

#ifdef DEB
static int VMoveA (Draw_Interpretor& di, Standard_Integer argc, const char** argv) {

  OSD_Timer myTimer;
  myTimer.Start();

  if (TheAISContext()->HasOpenedContext())
    TheAISContext()->CloseLocalContext();

  Standard_Real Step=2*PI/180;
  Standard_Real Angle=0;
  // R est le rayon de l'hellicoide
  Standard_Real R=50;
  // D est la distance parcourue en translation en 1 tour
  Standard_Real D=50;

  Handle(AIS_InteractiveObject) aIO;

  if (GetMapOfAIS().IsBound2(argv[1]))
    aIO = Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(argv[1]));

  if (aIO.IsNull()) {
    di<<" Syntaxe error: "<<argv[1]<<" doesn't exist"<<"\n";
    return 1;
  }
  TheAISContext()->Deactivate(aIO);

  // boucle generant le mouvement
  if(argc==3) {
    di<<" Transformations"<<"\n";
    for (Standard_Real myAngle=0;Angle<5*2*PI; myAngle++) {

      Angle=Step*myAngle;
      gp_Ax3 newBase(gp_Pnt(R*cos(Angle), R*sin(Angle), D*Angle/(2*PI) ), gp_Vec(0,0,1), gp_Vec(1,0,0)  );
      gp_Trsf myTransfo;
      myTransfo.SetTransformation(newBase.Rotated(gp_Ax1(gp_Pnt(R*cos(Angle),R*sin(Angle),0), gp_Dir(0,0,1)  ),Angle ) );
      TheAISContext()->SetLocation(aIO,myTransfo);

      TheAISContext() ->UpdateCurrentViewer();

    }
  }
  else {
    di<<" Locations"<<"\n";
    gp_Trsf myAngleTrsf;
    myAngleTrsf.SetRotation(gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1) ), Step  );
    TopLoc_Location myDeltaAngle (myAngleTrsf);
    gp_Trsf myDistTrsf;
    myDistTrsf.SetTranslation(gp_Dir(0,0,1) );
    TopLoc_Location myDeltaDist (myDistTrsf);
    TopLoc_Location myTrueLoc;

    for (Standard_Real myAngle=0;Angle<5*2*PI; myAngle++) {

      Angle=Step*myAngle;
      myTrueLoc=myTrueLoc*myDeltaAngle*myDeltaDist;
      TheAISContext()->SetLocation(aIO,myTrueLoc );
      TheAISContext() ->UpdateCurrentViewer();
    }
  }


  TopoDS_Shape ShapeBis=((*(Handle(AIS_Shape)*)&aIO)->Shape()).Located( aIO->Location() );

  //TopLoc_Location Tempo=aIO->Location();
  //TopoDS_Shape ShapeBis=((*(Handle(AIS_Shape)*)&aIO)->Shape());
  //ShapeBis.Located(Tempo);


  // On reset la location (origine) contenue dans l'AISInteractiveObject
  TheAISContext() ->ResetLocation(aIO);

  // On force aShape a devenir l'AIS IO propre a ShapeBis

  // Pour cela on force aShape(AIS IO) a devenir une AISShape
  // ->Set() est une methode de AIS_Shape
  (*(Handle(AIS_Shape)*)& aIO)->Set(ShapeBis);

  // On donne a ShapeBis le nom de l'AIS IO
  //Rep::Set(argv[1],ShapeBis);


  TheAISContext()->Redisplay(aIO,Standard_False);

  // On reactive la selection des primitives sensibles
  TheAISContext()->Activate(aIO,0);

  TheAISContext() ->UpdateCurrentViewer();
  a3DView() -> Redraw();

  myTimer.Stop();
  di<<" Temps ecoule "<<"\n";
  myTimer.Show();

  return 0;
}
#endif


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

  Standard_Real Step=4*PI/180;
  Standard_Real Angle=0;

  Handle(AIS_InteractiveObject) aIO;
  if (GetMapOfAIS().IsBound2(argv[1]))
    aIO = Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(argv[1]));
  if (aIO.IsNull())
    return 1;

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(aIO);

  myTimer.Start();

  if (atoi(argv[3])==1 ) {
    di<<" Primitives sensibles OFF"<<"\n";
    TheAISContext()->Deactivate(aIO);
  }
  else {
    di<<" Primitives sensibles ON"<<"\n";
  }
  // Movement par transformation
  if(atoi(argv[2]) ==1) {
    di<<" Calcul par Transformation"<<"\n";
    for (Standard_Real myAngle=0;Angle<10*2*PI; myAngle++) {

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

    for (Standard_Real myAngle=0;Angle<10*2*PI; myAngle++) {

      Angle=Step*myAngle;
      myTrueLoc=myTrueLoc*myDeltaAngle;
      TheAISContext()->SetLocation(aShape,myTrueLoc );
      TheAISContext() ->UpdateCurrentViewer();
    }
  }
  if (atoi(argv[3])==1 ){
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
  for (Standard_Real myAngle = 0;angleA<2*PI*10.175 ;myAngle++) {

    angleA = thread*myAngle*PI/180;
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
static int VShading(Draw_Interpretor& ,Standard_Integer argc, const char** argv) {
  Standard_Boolean HaveToSet;
//  Standard_Real    myDevAngle;
  Standard_Real    myDevCoef;
  Handle(AIS_InteractiveObject) TheAisIO;

  // Verifications
  if (!strcasecmp(argv[0],"vsetshading") ) HaveToSet=Standard_True;
  else HaveToSet=Standard_False;
  if (TheAISContext()->HasOpenedContext())
    TheAISContext()->CloseLocalContext();

  if (argc < 3) {
    myDevCoef  = 0.0008;
  } else {
    myDevCoef  =atof(argv[2]);
  }


  TCollection_AsciiString name=argv[1];
  if (GetMapOfAIS().IsBound2(name ))
    TheAisIO =
      Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(name));
  if (TheAisIO.IsNull())
    TheAisIO=GetAISShapeFromName((const char *)name.ToCString());

  if (HaveToSet) {
    TheAISContext()->SetDeviationCoefficient(TheAisIO,myDevCoef,Standard_True);
  }

  else {
    TheAISContext()->SetDeviationCoefficient(TheAisIO,0.0008,Standard_True);

  }

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
  Standard_Boolean HaveToSet;
  Standard_Boolean ThereIsName = Standard_False ;

  if(!a3DView().IsNull()){

    if (!strcasecmp(argv[0],"vsetam")) HaveToSet=Standard_True;
    else HaveToSet=Standard_False;

    // verification des arguments
    if (HaveToSet) {
      if (argc<2||argc>3) { di<<" Syntaxe error"<<"\n";return 1;}
      if (argc==3) ThereIsName=Standard_True;
      else ThereIsName=Standard_False;
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
      Standard_Integer aMode=atoi(argv [1]);

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
      Standard_Integer aMode=atoi(argv [2]);
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
  Standard_Boolean ThereIsCurrent=Standard_False;
  Standard_Boolean ThereIsArguments=Standard_False;
  TheAISContext()->CloseAllContexts();
  if (argc>=2 ) {
    ThereIsArguments=Standard_True;
  }
  if (TheAISContext()->NbCurrents()>0 ) {
    ThereIsCurrent=Standard_True;
  }
  // Debut...
  // ==================
  // Il y a un argument
  // ==================
  if (ThereIsArguments ) {
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
//function : VPickObject
//purpose  : filters can be set (type of Object to pick....
//
//=======================================================================
// Unused :
#ifdef DEB
static int VPickObject( Draw_Interpretor& , Standard_Integer /*argc*/, const char** /*argv*/)
{
  // preparation de la nomination automatique
  static TCollection_AsciiString ObjType[] ={"None","Datum","Shape","Object","Relation"};
//  static char* DatumSig [8] ={"Point","Axis","Trih","PlTri","Line","Circle","Plane","UnK"};

/*  TCollection_AsciiString name;

  Standard_Integer NbToPick = argc>2 ? argc-2 : 1;
  if(NbToPick==1){
    PickSh = ViewerTest::PickObject(theType);

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
    cout<<"Nom de la shape pickee : "<<name<<endl;
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
	cout<<"display of picke shape #"<<i<<" - nom : "<<name<<endl;
	TheAISContext()->Display(newsh);

      }
    }
  }
  */
  return 0;
}
#endif

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
  Standard_Integer persFlag1 = atoi(argv[2]);
  Standard_Integer persFlag2 = 0;
  Standard_Integer persFlag3 = 0;
  gp_Pnt origin = gp_Pnt( 0.0, 0.0, 0.0 );
  if ( argc == 4 || argc == 5 || argc == 7 || argc == 8 ) {
    persFlag2 = atoi(argv[3]);
  }
  if ( argc == 5 || argc == 8 ) {
    persFlag3 = atoi(argv[4]);
  }
  if ( argc >= 6 ) {
    origin.SetX( atof(argv[argc - 3]) );
    origin.SetY( atof(argv[argc - 2]) );
    origin.SetZ( atof(argv[argc - 1]) );
  }

  Standard_Boolean IsBound = GetMapOfAIS().IsBound2(shapeName);
  Standard_Boolean IsDatum = Standard_False;
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
//============================================================================
#include <AIS2D_InteractiveContext.hxx>
#include <HLRAlgo_Projector.hxx>
#include <Prs3d_Projector.hxx>
#include <AIS2D_ProjShape.hxx>
#include <gp_Ax2.hxx>
#include <gp.hxx>
Standard_Integer hlrtest(Draw_Interpretor&,   Standard_Integer n,   const char** a)
{

  Handle(AIS2D_InteractiveContext) aContext2D = Viewer2dTest::GetAIS2DContext();
  /////////////////////
  TopoDS_Shape aShape =  DBRep::Get(a[1]);
  aContext2D->EraseAll(Standard_True);
  Standard_Integer aPolyAlgo = 0;
  Standard_Boolean IsPoly = Standard_False;
  gp_Ax2 anAx2 = gp::XOY();

  //if(n > 2) aPolyAlgo = atoi(a[2]);

  //IsPoly = aPolyAlgo > 0;

  TopoDS_Shape aResult = aShape;


  if (n == 11)
  {
    Standard_Real x = atof(a[2]);
    Standard_Real y = atof(a[3]);
    Standard_Real z = atof(a[4]);

    Standard_Real dx = atof(a[5]);
    Standard_Real dy = atof(a[6]);
    Standard_Real dz = atof(a[7]);

    Standard_Real dx1 = atof(a[8]);
    Standard_Real dy1 = atof(a[9]);
    Standard_Real dz1 = atof(a[10]);

    gp_Pnt anOrigin (x, y, z);
    gp_Dir aNormal  (dx, dy, dz);
    gp_Dir aDX      (dx1, dy1, dz1);
    anAx2 = gp_Ax2(anOrigin, aNormal, aDX);
  }


  HLRAlgo_Projector aProjector(anAx2);

  //Prs3d_Projector aProjector_t(Standard_False,1, 0,0,1,0,0,1000,0,1,0);

  //aProjector =  aProjector_t.Projector();

  Handle(AIS2D_ProjShape) myDisplayableShape =
    new AIS2D_ProjShape(aProjector,0,IsPoly, Standard_False);

  myDisplayableShape->ShowEdges(Standard_True, Standard_False,
				Standard_False, Standard_True, Standard_False);

  myDisplayableShape->Add( aResult );


  aContext2D->Display( myDisplayableShape,Standard_True );
  aContext2D->UpdateCurrentViewer();

  return 0;
}

Standard_Integer phlrtest(Draw_Interpretor&,   Standard_Integer n,   const char** a)
{

  Handle(AIS2D_InteractiveContext) aContext2D = Viewer2dTest::GetAIS2DContext();
  /////////////////////
  TopoDS_Shape aShape =  DBRep::Get(a[1]);
  aContext2D->EraseAll(Standard_True);
  Standard_Integer aPolyAlgo = 0;
  Standard_Boolean IsPoly = Standard_True;
  gp_Ax2 anAx2 = gp::XOY();

  //if(n > 2) aPolyAlgo = atoi(a[2]);

  //IsPoly = aPolyAlgo > 0;

  TopoDS_Shape aResult = aShape;


  if (n == 11)
  {
    Standard_Real x = atof(a[2]);
    Standard_Real y = atof(a[3]);
    Standard_Real z = atof(a[4]);

    Standard_Real dx = atof(a[5]);
    Standard_Real dy = atof(a[6]);
    Standard_Real dz = atof(a[7]);

    Standard_Real dx1 = atof(a[8]);
    Standard_Real dy1 = atof(a[9]);
    Standard_Real dz1 = atof(a[10]);

    gp_Pnt anOrigin (x, y, z);
    gp_Dir aNormal  (dx, dy, dz);
    gp_Dir aDX      (dx1, dy1, dz1);
    anAx2 = gp_Ax2(anOrigin, aNormal, aDX);
  }


  HLRAlgo_Projector aProjector(anAx2);

  //Prs3d_Projector aProjector_t(Standard_False,1, 0,0,1,0,0,1000,0,1,0);

  //aProjector =  aProjector_t.Projector();

  Handle(AIS2D_ProjShape) myDisplayableShape =
    new AIS2D_ProjShape(aProjector,0,IsPoly, Standard_False);

  myDisplayableShape->ShowEdges(Standard_True, Standard_False,
				Standard_False, Standard_True, Standard_False);

  myDisplayableShape->Add( aResult );


  aContext2D->Display( myDisplayableShape,Standard_True );
  aContext2D->UpdateCurrentViewer();

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

  const char *group = "AIS_Display";

  // display

  theCommands.Add("vdisplay",
		  "vdisplay         : vdisplay2 name1 [name2] ... [name n] ",
		  __FILE__,VDisplay2,group);

  theCommands.Add("verase",
		  "verase      : verase2 [name1] ...  [name n] ",
		  __FILE__,VErase2,group);

  theCommands.Add("vdonly",
		  "vdonly         : vdonly2 [name1] ...  [name n]",
		  __FILE__,VDonly2,group);

  theCommands.Add("vdisplayall",
		  "vdisplayall         : vdisplayall",
		  __FILE__,VDisplayAll,group);

  theCommands.Add("veraseall",
		  "veraseall        : veraseall ",
		  __FILE__,VEraseAll,group);

  theCommands.Add("verasetype",
		  "verasetype        : verasetype <Type>  \n\t erase all the displayed objects of one given kind (see vtypes)",
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
		  "vsetdispmode    : vsetdispmode [name] mode(1,2,..): no name -> on selected objects ",
		  __FILE__,VDispMode,group);

  theCommands.Add("vunsetdispmode",
		  "vunsetdispmode : vunsetdispmode [name] mode(1,2,..): no name -> on selected objects",
		  __FILE__,VDispMode,group);

  theCommands.Add("vdir",
		  "vdir",
		  __FILE__,VDebug,group);

  theCommands.Add("vdump",
                  "<filename>.{png|bmp|jpg|gif} [buffer={rgb|rgba|depth}] [width height]\n\t\t: Dump contents of viewer window to PNG, BMP, JPEG or GIF file",
		  __FILE__,VDump,group);

  theCommands.Add("vsub",      "vsub 0/1 (off/on) [obj]        : Subintensity(on/off) of selected objects",
		  __FILE__,VSubInt,group);

  theCommands.Add("vsetcolor",
		  "vsetcolor         : vsetcolor [name of shape] ColorName",
		  __FILE__,VColor2,group);

  theCommands.Add("vunsetcolor",
		  "vunsetcolor     : vunsetcolor   [name of shape] ",
		  __FILE__,VColor2,group);

  theCommands.Add("vsettransparency",
		  "vsettransparency          : vtransparency [name of shape] TransparenctCoef (0 -> 1)",
		  __FILE__,VTransparency,group);

  theCommands.Add("vunsettransparency",
		  "vunsettransparency          : vtransparency [name of shape]",
		  __FILE__,VTransparency,group);

  theCommands.Add("vsetmaterial",
		  "vmaterial          : vmaterial  [name of shape] MaterialName",
		  __FILE__,VMaterial,group);

  theCommands.Add("vunsetmaterial",
		  "vmaterial          : vmaterial  [name of shape]",
		  __FILE__,VMaterial,group);

  theCommands.Add("vsetwidth",
		  "vsetwidth          : vwidth  [name of shape] width(0->10)",
		  __FILE__,VWidth,group);

  theCommands.Add("vunsetwidth",
		  "vunsetwidth          : vwidth  [name of shape]",
		  __FILE__,VWidth,group);

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

  theCommands.Add("vstate",   "vstate [Name1] ... [NameN] :No arg, select currents; no currrent select all  ",
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

  theCommands.Add("hlrtest"       , "Usage: hlrtest s1 s2 ...", __FILE__, hlrtest, group);
  theCommands.Add("phlrtest"       , "Usage: hlrtest s1 s2 ...", __FILE__, phlrtest, group);

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

//  sprintf(prefix, argv[1]);
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
  anAngle = atof(argv[4]);
  anAngle = 2*PI * anAngle / 360.0;
  gp_Pln aPln;
  Handle( Geom_Surface )aSurf;
  AIS_KindOfSurface aSurfType;
  Standard_Real Offset;
  gp_Dir aDir;
  if(argc > 4) { // == 5
    rev = atoi(argv[5]);
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
      Viewer2dTest::Commands(theDI);
#ifdef DEB
      theDI << "Draw Plugin : OCC V2d & V3d commands are loaded" << "\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(ViewerTest)
