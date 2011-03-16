// File:      Viewer2dTest_DisplayCommands.cxx
// Created:   22.01.02 16:21:20
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001

#include <Viewer2dTest.hxx>

#include <Viewer2dTest_EventManager.hxx>
#include <Viewer2dTest_DoubleMapOfInteractiveAndName.hxx>
#include <Viewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>

#include <Draw_Interpretor.hxx>

#include <HLRAlgo_Projector.hxx>

#include <V2d_Viewer.hxx>
#include <V2d_View.hxx>

#include <Prs2d_AspectRoot.hxx>
#include <Prs2d_AspectLine.hxx>

#include <gp_Ax3.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <BRep_Tool.hxx>

#include <TCollection_AsciiString.hxx>

#include <gp_Ax2.hxx>

#include <AIS2D_ProjShape.hxx>
#include <AIS2D_InteractiveContext.hxx>

#include <Quantity_NameOfColor.hxx>
#include <Quantity_Color.hxx>

#include <Graphic2d_Line.hxx>
#include <Graphic2d_Marker.hxx>
#include <Graphic2d_View.hxx>

#include <Graphic3d_NameOfMaterial.hxx>
#include <Prs2d_Arrow.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <GGraphic2d_Curve.hxx>
#include <Graphic2d_Text.hxx>
#include <AIS2D_ListOfIO.hxx>
#include <Prs2d_ToleranceFrame.hxx>
#include <Prs2d_Length.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <Graphic2d_Circle.hxx>
#include <Prs2d_Diameter.hxx>
#include <Prs2d_Point.hxx>
#include <Prs2d_Axis.hxx>
#include <Geom2d_Line.hxx>
#include <Graphic2d_Segment.hxx>
#include <GCE2d_MakeArcOfCircle.hxx>
#include <Graphic2d_Polyline.hxx>
#include <Graphic2d_Array1OfVertex.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <DrawTrSurf.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <GGraphic2d_SetOfCurves.hxx>
#include <Prs2d_AspectText.hxx>
#include <gp_Lin2d.hxx>
#include <Prs2d_Angle.hxx>
#include <Prs2d_Radius.hxx>
#include <Prs2d_Circularity.hxx>
#include <Prs2d_Parallelism.hxx>

#define OCC190 // jfa 04/03/2002 // for vertices load
#define OCC154 // jfa 06/03/2002 // for correct erasing

#define DEFAULT_COLOR    Quantity_NOC_GOLDENROD
#define DEFAULT_MATERIAL Graphic3d_NOM_BRASS

Viewer2dTest_DoubleMapOfInteractiveAndName& GetMapOfAIS2D();
Handle(AIS2D_InteractiveContext)& TheAIS2DContext();
TopoDS_Shape GetShapeFromName2d (const char* name);

//==============================================================================
//function : GetAIS2DShapeFromName
//purpose  : Compute an AIS2D_ProjShape from a draw variable or a file name
//==============================================================================
Handle(AIS2D_InteractiveObject) GetAIS2DShapeFromName
       (const char* name, gp_Ax2 anAx2 = gp::XOY())
{
  Handle(AIS2D_InteractiveObject) retsh;

  if (GetMapOfAIS2D().IsBound2(name))
  {
    Handle(AIS2D_InteractiveObject) IO = GetMapOfAIS2D().Find2(name);
//    if(IO->Type() == AIS2D_KOI_Shape)
//      if (IO->Signature() == 0){
//	retsh = *((Handle(AIS2D_ProjShape)*)&IO);
//      }
//    else
    cout << "an Object which is not an AIS2D_ProjShape already has this name!!!" << endl;
    return retsh;
  }

  TopoDS_Shape S = GetShapeFromName2d(name);
  if (!S.IsNull())
  {
#ifdef OCC190
    if (S.ShapeType() == TopAbs_VERTEX)
    {
      TopoDS_Vertex aVert = TopoDS::Vertex(S);
      gp_Pnt aPnt = BRep_Tool::Pnt(aVert);

      gp_Trsf aTrsf;
      gp_Ax3 aToSystem (anAx2);
      aTrsf.SetTransformation(aToSystem);
      aPnt.Transform(aTrsf);

      Handle(V2d_View) V = Viewer2dTest::CurrentView();
      retsh = new AIS2D_InteractiveObject();
      retsh->SetView(V->View());

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0

      Handle(Graphic2d_Marker) vertex = new Graphic2d_Marker
        (retsh, VERTEXMARKER, aPnt.X(), aPnt.Y(), DEFAULTMARKERSIZE, DEFAULTMARKERSIZE, 0.0);
    }
    else
#endif
    {
      HLRAlgo_Projector aProjector (anAx2);
      Handle(AIS2D_ProjShape) retprsh = new AIS2D_ProjShape(aProjector);
      retprsh->Add(S);
      retsh = retprsh;
    }
  }
  return retsh; 
}

//=======================================================================
//function : GetColorFromName2d
//purpose  : get the Quantity_NameOfColor from a string
//=======================================================================
static Quantity_NameOfColor GetColorFromName2d (const char *name) 
{ 
  Quantity_NameOfColor ret = DEFAULT_COLOR;
  
  Standard_Boolean Found = Standard_False;
  Standard_CString colstring;
  for (Standard_Integer i = 0; i <= 514 && !Found; i++)
  {
    colstring = Quantity_Color::StringName(Quantity_NameOfColor(i));
    if (!strcasecmp(name,colstring))
    {
      ret = (Quantity_NameOfColor)i;
      Found = Standard_True;
    }
  }

  return ret;
}
#if 0
//=======================================================================
//function : GetMaterialFromName2d
//purpose  : get the Graphic3d_NameOfMaterial from a string
//=======================================================================
static Graphic3d_NameOfMaterial GetMaterialFromName2d (const char *name) 
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

#ifdef DEB
  cout << " materiau" << name <<" "<<Standard_Integer(mat) << endl;
#endif
  return mat;
}
#endif

//==============================================================================
//function : Select2d
//purpose  : pick / select an object from the last MoveTo() on a
//            ButtonPress event
//==============================================================================
Handle(AIS2D_InteractiveObject) Select2d (Standard_Integer /*argc*/,
                                          const char** /*argv*/,
                                          Standard_Boolean shift,
                                          Standard_Boolean /*pick*/)
{
  Handle(AIS2D_InteractiveObject) ret;
  Handle (Viewer2dTest_EventManager) EM = Viewer2dTest::CurrentEventManager();
  if (shift) EM->ShiftSelect();
  else       EM->Select();

  const Handle(AIS2D_InteractiveContext) aContext = EM->Context();
  
  if (!aContext->HasOpenedContext())
  {
//    aContext->InitCurrent();
/*    while ( aContext->MoreCurrent() ) {
      Handle(AIS2D_InteractiveObject) aisPickedShape = 
	Handle(AIS2D_InteractiveObject)::DownCast(aContext->Current()); 
      
      const char *name = (GetMapOfAIS2D().IsBound1(aisPickedShape))? 
	GetMapOfAIS2D().Find1(aisPickedShape).ToCString() : 
	  "????";
      Handle(AIS2D_ProjShape) TheRealSh = Handle(AIS2D_ProjShape)::DownCast(aisPickedShape);
      if(!TheRealSh.IsNull()){
	cout << "Current is " << name 
	  << " (" << GetTypeNameFromShape2d(TheRealSh->Shape())
	    << ")" << endl;
      }
      ret = aisPickedShape;
      if(!TheRealSh.IsNull()){
	if ( pick && argc > 4 ) {
	  DBRep::Set(argv[4], TheRealSh->Shape());
	}
      }
      aContext->NextCurrent();
    }*/
  }
  else
  {
    // A LocalContext is opened, the use xxxxSelected() 
    // to select an object and its SubShape
/*    aContext->InitSelected();
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
        Handle(AIS2D_InteractiveObject) aisPicked = 
          Handle(AIS2D_InteractiveObject)::DownCast(aContext->Interactive());
        ret = aisPicked;
	Handle(AIS2D_ProjShape) aisPickedShape = Handle(AIS2D_ProjShape)::DownCast(aisPicked);
	
	// Get back its name
	const char *name = ( GetMapOfAIS2D().IsBound1(aisPicked) )? 
	  GetMapOfAIS2D().Find1(aisPicked).ToCString() : 
	    "????";
	
	if(!aisPickedShape.IsNull()){
	  if ( pick && argc > 4 ) {
	    // Create a draw variable to store the wohole shape 
	    // for vpick command
	    DBRep::Set(argv[4], aisPickedShape->Shape());
	  }
	  
	  cout << name << " (" << GetTypeNameFromShape2d(aisPickedShape->Shape()) 
	    << ")" << endl  ;
	}
      }
      // Goto the next selected object
      aContext->NextSelected();
    }*/
  }
  return ret;
}

//==============================================================================
//function : Displays,Erase...
//purpose  : 
//Draw arg : 
//==============================================================================
static int VwrTst_DispErase2d (const Handle(AIS2D_InteractiveObject)& IO,
                               const Standard_Integer Mode,
                               const Standard_Integer TypeOfOperation,
                               const Standard_Boolean Upd)
{
  Handle(AIS2D_InteractiveContext) Ctx = Viewer2dTest::GetAIS2DContext();
  
  switch (TypeOfOperation)
  {
  case 1:
    Ctx->Display(IO,Mode,Upd);
    break;
  case 2:
    Ctx->Erase(IO,Mode,Upd);
    break;
  case 3:
//    if(IO.IsNull())
//      Ctx->SetDisplayMode((AIS2D_DisplayMode)Mode,Upd);
//    else
//      Ctx->SetDisplayMode(IO,Mode,Upd);
    break;
  case 4:
//    if(IO.IsNull())
//      Ctx->SetDisplayMode(0,Upd);
//    else
//      Ctx->UnsetDisplayMode(IO,Upd);
    break;
  }

  return 0;
}

#if 0
//=======================================================================
//function :V2dDispMode
//purpose  : 
//=======================================================================
static int V2dDispMode (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  TCollection_AsciiString name;
  if (argc > 3) return 1;

  // display others presentations
  Standard_Integer TypeOfOperation = (strcasecmp(argv[0],"v2ddispmode") == 0)? 1:
    (strcasecmp(argv[0],"v2derasemode") == 0) ? 2 :
      (strcasecmp(argv[0],"v2dsetdispmode") == 0) ? 3 :
	(strcasecmp(argv[0],"v2dunsetdispmode") == 0) ? 4 : -1;
  
  Handle(AIS2D_InteractiveContext) Ctx = Viewer2dTest::GetAIS2DContext();
  
  //unset displaymode.. comportement particulier...
  if (TypeOfOperation == 4)
  {
    if (argc == 1)
    {
/*      if(Ctx->NbCurrents()==0 ||
	 Ctx->NbSelected()==0){
	Handle(AIS2D_InteractiveObject) IO;
	VwrTst_DispErase2d(IO,-1,4,Standard_False);
      }
      else if(!Ctx->HasOpenedContext()){
      	for(Ctx->InitCurrent();Ctx->MoreCurrent();Ctx->NextCurrent())
	  VwrTst_DispErase2d(Ctx->Current(),-1,4,Standard_False);
      }
      else{
	for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
	  VwrTst_DispErase2d(Ctx->Interactive(),-1,4,Standard_False);}*/
      Ctx->UpdateCurrentViewer();
    }
    else
    {
      Handle(AIS2D_InteractiveObject) IO;
      name = argv[1];
      if (GetMapOfAIS2D().IsBound2(name))
      {
	IO = GetMapOfAIS2D().Find2(name);
	VwrTst_DispErase2d(IO,-1,4,Standard_True);
      }
    }
  }
  else if (argc == 2)
  {
//    Standard_Integer Dmode = atoi(argv[1]);
//    if(Ctx->NbCurrents()==0 && TypeOfOperation==3){
//      Handle(AIS2D_InteractiveObject) IO;
//      VwrTst_DispErase2d(IO,Dmode,TypeOfOperation,Standard_True);
//    }
    if (!Ctx->HasOpenedContext())
    {
      // set/unset display mode sur le Contexte...
//      for(Ctx->InitCurrent();Ctx->MoreCurrent();Ctx->NextCurrent()){
//	VwrTst_DispErase2d(Ctx->Current(),Dmode,TypeOfOperation,Standard_False);
//      }
      Ctx->UpdateCurrentViewer();
    }
    else
    {
//      for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
//	Ctx->Display(Ctx->Interactive(),Dmode);
    }
  }
  else
  {
    Handle(AIS2D_InteractiveObject) IO;
    name = argv[1];
    if (GetMapOfAIS2D().IsBound2(name))
      IO = GetMapOfAIS2D().Find2(name);
    VwrTst_DispErase2d(IO,atoi(argv[2]),TypeOfOperation,Standard_True);
  }

  return 0;
}
#endif

//==============================================================================
//function : V2dSetBGColor
//purpose  : v2dsetbgcolor colorname : change background color
//==============================================================================
static int V2dSetBGColor (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc != 2)
  {
    cout << "Usage : v2dsetbgcolor colorname" << endl;
    return 1;
  }
  Handle(V2d_View) V = Viewer2dTest::CurrentView(); 
  if (V.IsNull()) return 1;

  V->SetBackground(GetColorFromName2d(argv[1]));
  V->Viewer()->Update();
  return 0;
}

//==============================================================================
//function : V2dColor
//purpose  : change the color of a selected or named or displayed shape 
//Draw arg : v2dsetcolor [name] color 
//==============================================================================
static int V2dSetUnsetColor (Handle(AIS2D_InteractiveObject) theShape,
                             Standard_Boolean HaveToSet,
                             Quantity_NameOfColor theColor = Quantity_NOC_WHITE)
{
  theShape->SetContext(TheAIS2DContext());

  Handle(Graphic2d_Primitive) aPrim;
  Handle(Prs2d_AspectRoot)    anAspect;
  Standard_Integer i = 1, n = theShape->Length();
  for (; i <= n; i++)
  {
    aPrim = theShape->Primitive(i);
    if (theShape->HasAspect(aPrim))
    {
      anAspect = theShape->GetAspect(aPrim);
      if (anAspect->GetAspectName() == Prs2d_AN_LINE) break;
      else anAspect = NULL;
    }
  }

  if (HaveToSet)
  {
    if (anAspect.IsNull())
    {
      theShape->SetAspect
        (new Prs2d_AspectLine(theColor, Aspect_TOL_SOLID, Aspect_WOL_THIN,
                              theColor, Graphic2d_TOPF_FILLED));
    }
    else
    {
      Handle(Prs2d_AspectLine) anAspectLine = Handle(Prs2d_AspectLine)::DownCast(anAspect);
      anAspectLine->SetTypeOfFill(Graphic2d_TOPF_FILLED);
      anAspectLine->SetColor(theColor);
      anAspectLine->SetInterColor(theColor);
      theShape->SetAspect(anAspect);
    }
  }
  else // unset color
  {
    Handle(Prs2d_AspectLine) anAspectLine;
    if ( !anAspect.IsNull() )
      anAspectLine = Handle(Prs2d_AspectLine)::DownCast(anAspect);
    else
      anAspectLine = new Prs2d_AspectLine();

    anAspectLine->SetTypeOfFill(Graphic2d_TOPF_EMPTY);
    if (theShape->IsKind(STANDARD_TYPE(AIS2D_ProjShape)))
      {
        anAspectLine->SetColor(Quantity_NOC_YELLOW);
        anAspectLine->SetInterColor(Quantity_NOC_YELLOW);
      }
    else
      {
        anAspectLine->SetColor(Quantity_NOC_WHITE);
        anAspectLine->SetInterColor(Quantity_NOC_WHITE);
      }
    theShape->SetAspect( anAspectLine );
  }

  return 0;
}

static int V2dColor (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  Standard_Boolean ThereIsArgument = Standard_False;
  Standard_Boolean HaveToSet = Standard_False;

  if (strcasecmp(argv[0],"v2dsetcolor") == 0) HaveToSet = Standard_True;

  Quantity_NameOfColor aColor = Quantity_NOC_BLACK;
  if (HaveToSet)
  {
    if (argc < 2 || argc > 3)
    {
      cout << "Usage: v2dsetcolor [name] color" << endl;
      return 1;
    }
    if (argc == 3)
    {
      ThereIsArgument = Standard_True;
      aColor = GetColorFromName2d(argv[2]);
    }
    else
    {
      aColor = GetColorFromName2d(argv[1]);
    }
  }
  else
  {
    if (argc > 2)
    {
      cout << "Usage: v2dunsetcolor [name]" << endl;
      return 1;
    }
    if (argc == 2) ThereIsArgument = Standard_True;
  }
  
  if (Viewer2dTest::CurrentView().IsNull()) return -1;
  TheAIS2DContext()->CloseLocalContext();

  if (ThereIsArgument)
  {
    TCollection_AsciiString name = argv[1];

    if (GetMapOfAIS2D().IsBound2(name))
    {
      Handle(AIS2D_InteractiveObject) aShape = GetMapOfAIS2D().Find2(name);
      V2dSetUnsetColor(aShape, HaveToSet, aColor);
    }
  }
  else
  {
    Standard_Boolean hasCurrent = Standard_False;
    Viewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
    for (; it.More() && !hasCurrent; it.Next())
    {
      if (TheAIS2DContext()->IsCurrent(it.Key1())) hasCurrent = Standard_True;
    }
    for (it.Reset(); it.More(); it.Next())
    {
      Handle(AIS2D_InteractiveObject) aShape = it.Key1();
      if ((hasCurrent && TheAIS2DContext()->IsCurrent(aShape)) || !hasCurrent)
      {
        V2dSetUnsetColor(aShape, HaveToSet, aColor);
      }
    }
//    TheAIS2DContext()->UpdateCurrentViewer();
  }
  return 0;
}

#if 0
//==============================================================================
//function : V2dTransparency
//purpose  : change the transparency of a selected or named or displayed shape 
//Draw arg : v2dsettransp [name] TransparencyCoeficient 
//==============================================================================
static int V2dTransparency (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{ 
  Standard_Boolean ThereIsArgument;
  Standard_Boolean HaveToSet;

  if (strcasecmp(argv[0], "v2dsettransp") == 0)
    HaveToSet = Standard_True;
  else 
    HaveToSet = Standard_False;
  
  if (HaveToSet)
  {
    if (argc < 2 || argc > 3)
    {
      cout << "Usage: v2dsettransp [name] color" << endl;
      return 1;
    }
    if (argc == 2) ThereIsArgument = Standard_False;
    else ThereIsArgument = Standard_True;
  }
  else
  {
    if (argc > 2)
    {
      cout << "Usage: v2dunsettransp [name]" << endl;
      return 1;
    }
    if (argc == 2) ThereIsArgument = Standard_True;
    else ThereIsArgument = Standard_False;
  }
  
  if (Viewer2dTest::CurrentView().IsNull()) return -1;
  TheAIS2DContext()->CloseLocalContext();

  if (ThereIsArgument)
  {
    TCollection_AsciiString name = argv[1];
    if (GetMapOfAIS2D().IsBound2(name))
    {
      Handle(AIS2D_InteractiveObject) ashape = GetMapOfAIS2D().Find2(name);
//      if(HaveToSet)
//	TheAIS2DContext()->SetTransparency(ashape,atof(argv[2]) );
//      else 
//	TheAIS2DContext()->UnsetTransparency(ashape);
    }
  }
  else
  {
//    if (TheAIS2DContext()->NbCurrents() > 0)
    {
/*      for (TheAIS2DContext()->InitCurrent(); TheAIS2DContext()->MoreCurrent();
           TheAIS2DContext()->NextCurrent())
      {
	Handle(AIS2D_InteractiveObject) ashape = TheAIS2DContext()->Current();
	if (HaveToSet)
        {
	  TheAIS2DContext()->SetTransparency(ashape, atof(argv[1]), Standard_False);
	}
	else 
	  TheAIS2DContext()->UnsetTransparency(ashape, Standard_False);
      }*/

      TheAIS2DContext()->UpdateCurrentViewer();
    }
//    else // shape not defined (neither current, nor named)
    {
      Viewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
      while (it.More())
      {
        Handle(AIS2D_InteractiveObject) ashape = it.Key1();
//	if(HaveToSet)
//	  TheAIS2DContext()->SetTransparency(ashape,atof(argv[1]),Standard_False);
//	else 
//	  TheAIS2DContext()->UnsetTransparency(ashape,Standard_False);
        it.Next();
      }
      TheAIS2DContext()->UpdateCurrentViewer();
    }
  }
  return 0;
}
#endif

#if 0
//==============================================================================
//function : V2dMaterial
//purpose  : change the Material of a selected or named or displayed shape 
//Draw arg : v2dmaterial  [Name] Material 
//==============================================================================
static int V2dMaterial (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{ 
  Standard_Boolean ThereIsName;
  Standard_Boolean HaveToSet;

  if (strcasecmp(argv[0], "v2dsetmaterial") == 0) HaveToSet = Standard_True;
  else HaveToSet = Standard_False;

  if (HaveToSet)
  {
    if (argc < 2 || argc > 3)
    {
      cout << "Usage: v2dsetmaterial [name] material" << endl;
      return 1;
    }
    if (argc == 2) ThereIsName = Standard_False;
    else ThereIsName = Standard_True;
  }
  else
  {
    if (argc > 2)
    {
      cout << "Usage: v2dunsetmaterial [name]" << endl;
      return 1;
    }
    if (argc == 2) ThereIsName = Standard_True;
    else ThereIsName = Standard_False;
  }

  if (Viewer2dTest::CurrentView().IsNull()) return -1;
  TheAIS2DContext()->CloseLocalContext();

  if (ThereIsName)
  {
    TCollection_AsciiString name = argv[1];
    if (GetMapOfAIS2D().IsBound2(name))
    {
      Handle(AIS2D_InteractiveObject) ashape = GetMapOfAIS2D().Find2(name);
      if (HaveToSet)
      {
//	TheAIS2DContext()->SetMaterial (ashape,GetMaterialFromName2d (argv[2]) );
      }
      else
      {
//	TheAIS2DContext()->UnsetMaterial(ashape);
      }
    }
  }
  else
  {
//    if (TheAIS2DContext()->NbCurrents() > 0)
    {
/*      for (TheAIS2DContext()->InitCurrent(); TheAIS2DContext()->MoreCurrent();
           TheAIS2DContext()->NextCurrent())
      {
	Handle(AIS2D_InteractiveObject) ashape = TheAIS2DContext()->Current();
	if (HaveToSet)
        {
	  TheAIS2DContext()->SetMaterial(ashape, GetMaterialFromName2d(argv[1]), Standard_False);
	}
	else
	  TheAIS2DContext()->UnsetMaterial(ashape, Standard_False);
      }*/
      TheAIS2DContext()->UpdateCurrentViewer();
    }
//    else // shape not defined (neither current, nor named)
    {
      Viewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
      while (it.More())
      {
	Handle(AIS2D_InteractiveObject) ashape = it.Key1();
	
//	if (HaveToSet)
//	  TheAIS2DContext()->SetMaterial(ashape,GetMaterialFromName2d(argv[1]),Standard_False);
//	else TheAIS2DContext()->UnsetMaterial(ashape,Standard_False);
	it.Next();
      }
      TheAIS2DContext()->UpdateCurrentViewer();
    }
  }
  return 0;
}
#endif

//==============================================================================
//function : V2dWidth
//purpose  : change the width of the edges of a selected or named or displayed shape 
//Draw arg : v2dsetwidth [Name] WidthEnum(THIN MEDIUM THICK VERYTHICK)
//==============================================================================
static int V2dSetUnsetWidth (Handle(AIS2D_InteractiveObject) theShape,
                             Standard_Boolean HaveToSet,
                             Aspect_WidthOfLine theWOL = Aspect_WOL_THIN)
{
  theShape->SetContext(TheAIS2DContext());

  Handle(Graphic2d_Primitive) aPrim;
  Handle(Prs2d_AspectRoot)    anAspect;
  Standard_Integer i = 1, n = theShape->Length();
  for (; i <= n; i++)
  {
    aPrim = theShape->Primitive(i);
    if (theShape->HasAspect(aPrim))
    {
      anAspect = theShape->GetAspect(aPrim);
      if (anAspect->GetAspectName() == Prs2d_AN_LINE) break;
      else anAspect = NULL;
    }
  }

  if (HaveToSet)
  {
    if (anAspect.IsNull())
    {
      Quantity_NameOfColor aColor;
      if (theShape->IsKind(STANDARD_TYPE(AIS2D_ProjShape)))
      {
        aColor = Quantity_NOC_YELLOW;
      }
      else
      {
        aColor = Quantity_NOC_WHITE;
      }
      anAspect = new Prs2d_AspectLine(aColor, Aspect_TOL_SOLID, theWOL,
                                      aColor, Graphic2d_TOPF_EMPTY);
    }
    else
    {
      Handle(Prs2d_AspectLine) anAspectLine = Handle(Prs2d_AspectLine)::DownCast(anAspect);
      anAspectLine->SetWidth(theWOL);
    }

    theShape->SetAspect(anAspect);
  }
  else // unset width
  {
    if (!anAspect.IsNull())
    {
      Handle(Prs2d_AspectLine) anAspectLine = Handle(Prs2d_AspectLine)::DownCast(anAspect);
      anAspectLine->SetWidth(Aspect_WOL_THIN);
//          anAspectLine->SetWidthIndex(0);
      theShape->SetAspect(anAspect);
    }
  }

  return 0;
}

static int V2dWidth (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  Standard_Boolean ThereIsArgument = Standard_False;
  Standard_Boolean HaveToSet = Standard_False;

  if (strcasecmp(argv[0], "v2dsetwidth") == 0) HaveToSet = Standard_True;

  const char* aStr;
  if (HaveToSet)
  {
    if (argc < 2 || argc > 3)
    {
      cout << "Usage: v2dsetwidth [name] width" << endl;
      return 1;
    }
    if (argc == 3)
    {
      ThereIsArgument = Standard_True;
      aStr = argv[2];
    }
    else
    {
      aStr = argv[1];
    }
  }
  else
  {
    if ( argc > 2 )
    {
      cout << "Usage: v2dunsetwidth [name]" << endl;
      return 1;
    }
    if (argc == 2) ThereIsArgument = Standard_True;
  }

  if (Viewer2dTest::CurrentView().IsNull()) return -1;
  TheAIS2DContext()->CloseLocalContext();

  Aspect_WidthOfLine aWOL = Aspect_WOL_THIN;
  if (HaveToSet)
  {
    if      (strcmp(aStr, "THIN"     ) == 0) aWOL = Aspect_WOL_THIN;
    else if (strcmp(aStr, "MEDIUM"   ) == 0) aWOL = Aspect_WOL_MEDIUM;
    else if (strcmp(aStr, "THICK"    ) == 0) aWOL = Aspect_WOL_THICK;
    else if (strcmp(aStr, "VERYTHICK") == 0) aWOL = Aspect_WOL_VERYTHICK;
    else aWOL = Aspect_WOL_USERDEFINED;
  }

  if (ThereIsArgument)
  {
    TCollection_AsciiString name = argv[1];

    if (GetMapOfAIS2D().IsBound2(name))
    {
      Handle(AIS2D_InteractiveObject) aShape = GetMapOfAIS2D().Find2(name);
      V2dSetUnsetWidth(aShape, HaveToSet, aWOL);
    }
  }
  else
  {
    Standard_Boolean hasCurrent = Standard_False;
    Viewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
    for (; it.More() && !hasCurrent; it.Next())
    {
      if (TheAIS2DContext()->IsCurrent(it.Key1())) hasCurrent = Standard_True;
    }
    for (it.Reset(); it.More(); it.Next())
    {
      Handle(AIS2D_InteractiveObject) aShape = it.Key1();
      if ((hasCurrent && TheAIS2DContext()->IsCurrent(aShape)) || !hasCurrent)
      {
        V2dSetUnsetWidth(aShape, HaveToSet, aWOL);
      }
    }
//    TheAIS2DContext()->UpdateCurrentViewer();
  }
  return 0;
}

//==============================================================================
//function : V2dDisplay
//purpose  : Display an object from its name
//Draw arg : v2ddisplay name [projection (Origin(x y z) Normal(dx dy dz) DX(dx dy dz)]
//==============================================================================
static int V2dDisplay (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  // Verification des arguments
  if (argc != 2 && argc != 11)
  {
    cout << "Usage: v2ddisplay name [x y z dx dy dz dx1 dy1 dz1]" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }
  
  TCollection_AsciiString name;
  TheAIS2DContext()->CloseLocalContext();
  
  name = argv[1];
  gp_Ax2 anAx2 = gp::XOY();
  if (argc == 11)
  {
    Standard_Real x = atof(argv[2]);
    Standard_Real y = atof(argv[3]);
    Standard_Real z = atof(argv[4]);

    Standard_Real dx = atof(argv[5]);
    Standard_Real dy = atof(argv[6]);
    Standard_Real dz = atof(argv[7]);

    Standard_Real dx1 = atof(argv[8]);
    Standard_Real dy1 = atof(argv[9]);
    Standard_Real dz1 = atof(argv[10]);

    gp_Pnt anOrigin (x, y, z);
    gp_Dir aNormal  (dx, dy, dz);
    gp_Dir aDX      (dx1, dy1, dz1);
    anAx2 = gp_Ax2(anOrigin, aNormal, aDX);
  }

  Standard_Boolean IsBound = GetMapOfAIS2D().IsBound2(name);
  Standard_Boolean IsDatum = Standard_False;
  Handle(AIS2D_InteractiveObject) aShape;
  if (IsBound)
  {
    aShape = GetMapOfAIS2D().Find2(name);
//    if (aShape->Type() == AIS2D_KOI_Datum)
    {
      IsDatum = Standard_True;
      TheAIS2DContext()->Display(aShape, Standard_False);
#ifdef OCC154
      TheAIS2DContext()->HighlightCurrent();
#endif
    }
    if (!IsDatum)
    {	  
      cout << "Display " << name << endl;
      // Get the Shape from a name
      TopoDS_Shape NewShape = GetShapeFromName2d((const char *)name.ToCString());

      // Update the Shape in the AIS2D_ProjShape				
      Handle(AIS2D_ProjShape) TheRealSh = Handle(AIS2D_ProjShape)::DownCast(aShape);
//    if(!TheRealSh.IsNull())	TheRealSh->Set(NewShape);
      TheAIS2DContext()->Redisplay(aShape, Standard_False);
      TheAIS2DContext()->Display(aShape, Standard_False);
    }
    aShape.Nullify();
  }
  else // Create the AIS2D_ProjShape from a name
  {
    aShape = GetAIS2DShapeFromName((const char *)name.ToCString(), anAx2);
    if (!aShape.IsNull())
    {
      GetMapOfAIS2D().Bind(aShape, name);
      TheAIS2DContext()->Display(aShape, Standard_False);
    }
  }

  // Update the screen and redraw the view
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0; 
}

//==============================================================================
//function : V2dDonly
//purpose  : Display only a selected or named  object
//           if there is no selected or named object s, the whole viewer is erased
//Draw arg : vdonly [name1] ... [name n]
//==============================================================================
static int V2dDonly (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (Viewer2dTest::CurrentView().IsNull()) return 1;
  
  Standard_Boolean ThereIsArgument = argc > 1;
  
  if (TheAIS2DContext()->HasOpenedContext()) TheAIS2DContext()->CloseLocalContext();

  if (ThereIsArgument)
  {
    TheAIS2DContext()->EraseAll(Standard_True, Standard_False);            

    for (int i = 1; i < argc; i++)
    {
      TCollection_AsciiString name = argv[i];
      Standard_Boolean IsBound = GetMapOfAIS2D().IsBound2(name);
      if (IsBound)
      {
	Handle(AIS2D_InteractiveObject) aShape = GetMapOfAIS2D().Find2(name);
	TheAIS2DContext()->Display(aShape, Standard_False);
      }
    }
    TheAIS2DContext()->CurrentViewer()->Update();
  }
  else
  {
    Standard_Boolean ThereIsCurrent = Standard_True/*= TheAIS2DContext()->NbCurrents() > 0*/;
    if (ThereIsCurrent)
    {
      Viewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
      while (it.More())
      {
        Handle(AIS2D_InteractiveObject) aShape = it.Key1();
        if (!TheAIS2DContext()->IsCurrent(aShape))
          TheAIS2DContext()->Erase(aShape, Standard_False);        
        it.Next();
      }
      TheAIS2DContext()->CurrentViewer()->Update();
    }
  }

  return 0; 
}

//==============================================================================
//function : V2dErase
//purpose  : Erase some selected or named objects.
//           If there is no selected or named objects, does nothing
//Draw arg : v2derase [name1] ... [name n]
//==============================================================================
static int V2dErase (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (Viewer2dTest::CurrentView().IsNull()) return 1;
  
  if (TheAIS2DContext()->HasOpenedContext()) TheAIS2DContext()->CloseLocalContext();

  if (argc > 1) // has arguments
  {
    for (int i = 1; i < argc; i++)
    {
      TCollection_AsciiString name = argv[i];
      Standard_Boolean IsBound = GetMapOfAIS2D().IsBound2(name);
      if (IsBound)
      {
	Handle(AIS2D_InteractiveObject) aShape = GetMapOfAIS2D().Find2(name);
	TheAIS2DContext()->Erase(aShape, Standard_False);  
      }
    }
//    TheAIS2DContext()->UpdateCurrentViewer();
    TheAIS2DContext()->CurrentViewer()->Update();
  }
  else
  {
    Viewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
    while (it.More())
    {
      Handle(AIS2D_InteractiveObject) aShape = it.Key1();
      if (TheAIS2DContext()->IsCurrent(aShape))
        TheAIS2DContext()->Erase(aShape, Standard_False);        
      it.Next();
    }
//    TheAIS2DContext()->UpdateCurrentViewer();
    TheAIS2DContext()->CurrentViewer()->Update();
  }

  return 0; 
}

//==============================================================================
//function : V2dEraseAll
//purpose  : Erase all the objects displayed in the viewer
//Draw arg : v2deraseall
//==============================================================================
static int V2dEraseAll (Draw_Interpretor& , Standard_Integer argc, const char** )
{
  if (argc > 1)
  {
    cout << "Usage: v2deraseall" << endl;
    return 1;
  }

  if (Viewer2dTest::CurrentView().IsNull())
  {
    cout << " Error: v2dinit hasn't been called." << endl;
    return 1;
  }

  TheAIS2DContext()->CloseAllContext(Standard_False);

  // EraseAll(Standard_True, Standard_True);            
  TheAIS2DContext()->EraseAll();
  return 0;
}

//==============================================================================
//function : V2dDisplayAll
//purpose  : Display all the objects of the Map
//Draw arg : v2ddisplayall
//==============================================================================
static int V2dDisplayAll (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (Viewer2dTest::CurrentView().IsNull()) return 1;

  if (argc > 1)
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  TheAIS2DContext()->CloseLocalContext();
  Viewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
//  while (it.More())
//  {
//    Handle(AIS2D_InteractiveObject) aShape = it.Key1();
//    TheAIS2DContext()->Erase(aShape,Standard_False);        
//    it.Next();
//  }
//  it.Reset();
  while (it.More())
  {
    Handle(AIS2D_InteractiveObject) aShape = it.Key1();
    TheAIS2DContext()->Display(aShape, Standard_False);        
    it.Next();
  }
#ifdef OCC154
  TheAIS2DContext()->HighlightCurrent();
#endif
  TheAIS2DContext()->UpdateCurrentViewer();
//  TheAIS2DContext()->DisplayAll();
  return 0; 
}

//==============================================================================
//function : VTexture
//purpose  : 
//Draw arg : 
//==============================================================================
/*
Standard_Integer VTexture (Draw_Interpretor& di,Standard_Integer argc, Standard_Character** argv )
{
  Standard_Integer command = (strcasecmp(argv[0],"vtexture")==0)? 1:
    (strcasecmp(argv[0],"vtexscale")==0) ? 2 :
      (strcasecmp(argv[0],"vtexorigin")==0) ? 3 :
	(strcasecmp(argv[0],"vtexrepeat")==0) ? 4 :
	  (strcasecmp(argv[0],"vtexdefault")==0) ? 5 : -1;
  
  Handle(AIS2D_InteractiveContext) myAIS2DContext = Viewer2dTest::GetAIS2DContext();
  if(myAIS2DContext.IsNull()) 
    { 
      cerr << "use 'vinit' command before " << argv[0] << endl;
      return 0;
    }
  
  Handle(AIS2D_InteractiveObject) TheAisIO;
  Handle(AIS2D_TexturedShape) myShape;
  Standard_Integer myPreviousMode = 0;

  if (!argv[1])
    {
      cout << argv[0] <<" syntax error - Type 'help vtex'"<<endl;
      return 0;
    }
  
  TCollection_AsciiString name = argv[1];
  Viewer2dTest::CurrentView()->SetSurfaceDetail(V2d_TEX_ALL);
  
  if (GetMapOfAIS2D().IsBound2(name)) 
    TheAisIO = GetMapOfAIS2D().Find2(name);
  else
    {
      cout <<"shape "<<name<<" doesn\'t exist"<<endl;
      return 0;
    }
  
  if (TheAisIO->IsKind(STANDARD_TYPE(AIS2D_TexturedShape)) && !TheAisIO.IsNull())
    {
      myShape = Handle(AIS2D_TexturedShape)::DownCast(TheAisIO);
      myPreviousMode = myShape->DisplayMode() ;
    }
  else
    {
      myAIS2DContext->Erase(TheAisIO,Standard_False);
      myShape = new AIS2D_TexturedShape (DBRep::Get(argv[1]));
      GetMapOfAIS2D().UnBind1(TheAisIO);
      GetMapOfAIS2D().UnBind2(name);
      GetMapOfAIS2D().Bind(myShape, name);  
    }
  switch (command)
    {
    case 1: // vtexture : we only map a texture on the shape
      if(argc<=1)
	{ 
	  cerr << argv[0] <<" syntax error - Type 'help vtex'" << endl;
	  return 0;
	}
      if (argv[2])
	{
	  if(strcasecmp(argv[2],"?")==0)
	    { 
	      TCollection_AsciiString monPath = GetEnvir2d();
	      
	      cout<<"\n Files in current directory : \n"<<endl;
	      TCollection_AsciiString Cmnd ("glob -nocomplain *");
	      di.Eval(Cmnd.ToCString());
	      
	      Cmnd = TCollection_AsciiString("glob -nocomplain ") ;
	      Cmnd += monPath ;
	      Cmnd += "/" ;
	      Cmnd += "* " ;
	      cout<<"Files in "<<monPath<<" : \n"<<endl;
	      di.Eval(Cmnd.ToCString());
	      
	      return 0;
	    }
	  else
	    myShape->SetTextureFileName(argv[2]);
	}
      else
	{
	  cout <<"Texture mapping disabled \n \
                  To enable it, use 'vtexture NameOfShape NameOfTexture' \n"<<endl;

	  myAIS2DContext->SetDisplayMode(myShape,1,Standard_False);
	  if (myPreviousMode == 3 )
	    myAIS2DContext->RecomputePrsOnly(myShape);
	  myAIS2DContext->Display(myShape, Standard_True);
	  return 0;
	}
      break;
      
    case 2: // vtexscale : we change the scaling factor of the texture
      
      if(argc<2) 
	{ 
	  cerr << argv[0] <<" syntax error - Type 'help vtex'" << endl;
	  return 0;
	}
      
      myShape->SetTextureScale (( argv[2] ? Standard_True    : Standard_False ),
				( argv[2] ? atof(argv[2])    : 1.0 ),
				( argv[2] ? atof(argv[argc-1]) : 1.0 ) );
      break;
      
    case 3: // vtexorigin : we change the origin of the texture on the shape
      if(argc<2) 
	{ 
	  cerr << argv[0] <<" syntax error - Type 'help vtex'" << endl;
	  return 0;
	}
      myShape->SetTextureOrigin (( argv[2] ? Standard_True    : Standard_False ),
				 ( argv[2] ? atof(argv[2])    : 0.0 ),
				 ( argv[2] ? atof(argv[argc-1]) : 0.0 ));
      break;
      
    case 4: // vtexrepeat : we change the number of occurences of the texture on the shape
      if(argc<2)
	{ 
	  cerr << argv[0] <<" syntax error - Type 'help vtex'" << endl;
	  return 0;
	}
      if (argv[2])
	{
	  cout <<"Texture repeat enabled"<<endl;
	  myShape->SetTextureRepeat(Standard_True, atof(argv[2]), atof(argv[argc-1]) );
	}
      else
	{
	  cout <<"Texture repeat disabled"<<endl;
	  myShape->SetTextureRepeat(Standard_False);
	}
      break;
      
    case 5: // vtexdefault : default texture mapping
      // ScaleU = ScaleV = 100.0
      // URepeat = VRepeat = 1.0
      // Uorigin = VOrigin = 0.0
      
      if(argc<2)
	{ 
	  cerr << argv[0] <<" syntax error - Type 'help vtex'" << endl;
	  return 0;
	}
      myShape->SetTextureRepeat(Standard_False);
      myShape->SetTextureOrigin(Standard_False);
      myShape->SetTextureScale (Standard_False);
      break;
    }

  if ((myShape->DisplayMode() == 3) || (myPreviousMode == 3 ))
    myAIS2DContext->RecomputePrsOnly(myShape);
  else 
    {
      myAIS2DContext->SetDisplayMode(myShape,3,Standard_False);
      myAIS2DContext->Display(myShape, Standard_True);
      myAIS2DContext->Update(myShape,Standard_True);
    } 
  return 1;
}
*/

#if 0
//==============================================================================
//function : V2dShading
//purpose  : Sharpen or roughten the quality of the shading
//Draw arg : vshading ShapeName 0.1->0.00001  1 deg-> 30 deg
//==============================================================================
static int V2dShading (Draw_Interpretor& ,Standard_Integer argc, const char** argv)
{
  Standard_Boolean HaveToSet;
//  Standard_Real    myDevAngle;
  Standard_Real    myDevCoef;
  Handle(AIS2D_InteractiveObject) TheAisIO;
  
  // Verifications
  if (!strcasecmp(argv[0],"vsetshading")) HaveToSet = Standard_True;
  else HaveToSet = Standard_False;
  TheAIS2DContext()->CloseLocalContext();

  if (argc < 3)
  {
    myDevCoef = 0.0008;
  }
  else
  {
    myDevCoef = atof(argv[2]);
  }

  TCollection_AsciiString name = argv[1];
  if (GetMapOfAIS2D().IsBound2(name)) TheAisIO = GetMapOfAIS2D().Find2(name);
  else
  { 
    TheAisIO = GetAIS2DShapeFromName(name.ToCString());
  }
  
  if (HaveToSet)
  {
//    TheAIS2DContext()->SetDeviationCoefficient(TheAisIO,myDevCoef,Standard_True);
  }
  else
  {
//    TheAIS2DContext()->SetDeviationCoefficient(TheAisIO,0.0008,Standard_True);
  }
  
  TheAIS2DContext()->Redisplay(TheAisIO);
  return 0;
}
#endif

//=======================================================================
//function : VEraseType
//purpose  : 
//=======================================================================
/*static int VEraseType (Draw_Interpretor& , Standard_Integer argc, const char** argv) 
{
  if (argc!=2) return 1;

  AIS2D_KindOfInteractive TheType;
  Standard_Integer TheSign(-1);
  GetTypeAndSignfromString2d(argv[1],TheType,TheSign);
  
  AIS2D_ListOfInteractive LIO;

  // en attendant l'amelioration ais pour les dimensions...
  //
  Standard_Integer dimension_status(-1);
  if(TheType==AIS2D_KOI_Relation){
    dimension_status = TheSign ==1 ? 1 : 0;
    TheSign=-1;
  }
  
  TheAIS2DContext()->DisplayedObjects(TheType,TheSign,LIO);
  Handle(AIS2D_InteractiveObject) curio;
  for(AIS2D_ListIteratorOfListOfInteractive it(LIO);it.More();it.Next()){
    curio  = it.Value();
    
    if(dimension_status == -1)
      TheAIS2DContext()->Erase(curio,Standard_False);
    else {
      AIS2D_KindOfDimension KOD = (*((Handle(AIS2D_Relation)*)&curio))->KindOfDimension();
      if ((dimension_status==0 && KOD == AIS2D_KOD_NONE)||
	  (dimension_status==1 && KOD != AIS2D_KOD_NONE))
	TheAIS2DContext()->Erase(curio,Standard_False);
    }
  }
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;
}*/

//=======================================================================
//function : V2dDisplayType
//purpose  : 
//=======================================================================
/*static int V2dDisplayType (Draw_Interpretor& , Standard_Integer argc, const char** argv) 
{
  if (argc!=2) return 1;

  AIS2D_KindOfInteractive TheType;
  Standard_Integer TheSign(-1);
  GetTypeAndSignfromString2d(argv[1],TheType,TheSign);

  // en attendant l'amelioration ais pour les dimensions...
  //
  Standard_Integer dimension_status(-1);
  if(TheType==AIS2D_KOI_Relation){
    dimension_status = TheSign ==1 ? 1 : 0;
    TheSign=-1;
  }
  
  AIS2D_ListOfInteractive LIO;
  TheAIS2DContext()->ObjectsInside(LIO,TheType,TheSign);
  Handle(AIS2D_InteractiveObject) curio;
  for(AIS2D_ListIteratorOfListOfInteractive it(LIO);it.More();it.Next()){
    curio  = it.Value();
    if(dimension_status == -1)
      TheAIS2DContext()->Display(curio,Standard_False);
    else {
      AIS2D_KindOfDimension KOD = (*((Handle(AIS2D_Relation)*)&curio))->KindOfDimension();
      if ((dimension_status==0 && KOD == AIS2D_KOD_NONE)||
	  (dimension_status==1 && KOD != AIS2D_KOD_NONE))
	TheAIS2DContext()->Display(curio,Standard_False);
    }

  }
  
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;
}*/


//Additional commands

 //=======================================================================
//function : GetMaterialFromName2d
//purpose  : get the Graphic3d_NameOfMaterial from a string
//=======================================================================
static Aspect_TypeOfLine GetLineTypeFromName2d (const char *name) 
{ 

  Aspect_TypeOfLine aLT = Aspect_TOL_SOLID;
  if      ( !strcasecmp(name,"DASH" ) ) 	 aLT = Aspect_TOL_DASH;
  else if ( !strcasecmp(name,"DOT" ) )           aLT = Aspect_TOL_DOT;
  else if ( !strcasecmp(name,"DOTDASH" ) ) 	 aLT = Aspect_TOL_DOTDASH;
  return aLT;
}


//==============================================================================
//function : V2dTypeofLine
//purpose  : change the line type of a selected or named or displayed shape 
//Draw arg : v2dsetlinetype [name] type 
//==============================================================================

static int V2dSetUnsetTypeOfLine (Handle(AIS2D_InteractiveObject) theShape,
                                  Standard_Boolean HaveToSet,
                                  Aspect_TypeOfLine theType = Aspect_TOL_SOLID)
{
  theShape->SetContext(TheAIS2DContext());

  Handle(Graphic2d_Primitive) aPrim;
  Handle(Prs2d_AspectRoot)    anAspect;
  //get all primitives
  Standard_Integer i = 1, n = theShape->Length();
  for (; i <= n; i++)
  {
    aPrim = theShape->Primitive(i);
    if (theShape->HasAspect(aPrim))
    {
      anAspect = theShape->GetAspect(aPrim);
      if (anAspect->GetAspectName() == Prs2d_AN_LINE) break;
      else anAspect = NULL;
    }
  }

  if (HaveToSet)
  {
    if (anAspect.IsNull())
    {
      theShape->SetAspect
        (new Prs2d_AspectLine(Quantity_NOC_YELLOW, theType, Aspect_WOL_THIN,
                              Quantity_NOC_YELLOW, Graphic2d_TOPF_FILLED));
    }
    else
    {
      Handle(Prs2d_AspectLine) anAspectLine = Handle(Prs2d_AspectLine)::DownCast(anAspect);
      anAspectLine->SetTypeOfFill(Graphic2d_TOPF_FILLED);
      anAspectLine->SetType( theType );
      theShape->SetAspect(anAspect);
    }
  }
  else // unset type line
  {
    Handle(Prs2d_AspectLine) anAspectLine;
    if ( !anAspect.IsNull() )
      anAspectLine = Handle(Prs2d_AspectLine)::DownCast(anAspect);
    else
      anAspectLine = new Prs2d_AspectLine();

    anAspectLine->SetTypeOfFill(Graphic2d_TOPF_EMPTY);
    anAspectLine->SetType(Aspect_TOL_SOLID);
    theShape->SetAspect( anAspectLine );
  }

  return 0;
}

static int V2dTypeOfLine (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  Standard_Boolean ThereIsArgument = Standard_False;
  Standard_Boolean HaveToSet = Standard_False;

  if (strcasecmp(argv[0],"v2dsetlinetype") == 0) HaveToSet = Standard_True;

  Aspect_TypeOfLine aType = Aspect_TOL_SOLID;
  //Quantity_NameOfColor aColor = Quantity_NOC_BLACK;
  if (HaveToSet)
  {
    if (argc < 2 || argc > 3)
    {
      cout << "Usage: v2dsetlinetype [name] type" << endl;
      return 1;
    }
    if (argc == 3)
    {
      ThereIsArgument = Standard_True;
      aType = GetLineTypeFromName2d(argv[2]);
    }
    else
    {
      aType = GetLineTypeFromName2d(argv[1]);
    }
  }
  else
  {
    if (argc > 2)
    {
      cout << "Usage: v2dunsetlinetype [name]" << endl;
      return 1;
    }
    if (argc == 2) ThereIsArgument = Standard_True;
  }
  
  if (Viewer2dTest::CurrentView().IsNull()) return -1;
  TheAIS2DContext()->CloseLocalContext();

  if (ThereIsArgument)
  {
    TCollection_AsciiString name = argv[1];

    if (GetMapOfAIS2D().IsBound2(name))
    {
      Handle(AIS2D_InteractiveObject) aShape = GetMapOfAIS2D().Find2(name);
      V2dSetUnsetTypeOfLine(aShape, HaveToSet, aType);
    }
  }
  else
  {
    Standard_Boolean hasCurrent = Standard_False;
    Viewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
    for (; it.More() && !hasCurrent; it.Next())
    {
      if (TheAIS2DContext()->IsCurrent(it.Key1())) hasCurrent = Standard_True;
    }
    for (it.Reset(); it.More(); it.Next())
    {
      Handle(AIS2D_InteractiveObject) aShape = it.Key1();
      if ((hasCurrent && TheAIS2DContext()->IsCurrent(aShape)) || !hasCurrent)
      {
        V2dSetUnsetTypeOfLine(aShape, HaveToSet, aType);
      }
    }
  }
  return 0;
}


static int drawPoint (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if( argc < 1 ) {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull()) { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }
  
  Standard_Real x = 24., y = 24.;
  if( argc >= 2 )
    x = atof( argv[1] );
  if( argc >= 3 )
    y = atof( argv[2] );

  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  Handle_Prs2d_Point origin = new Prs2d_Point(aIO, gp_Pnt2d( x, y ), 
                                              Aspect_TypeOfMarker(argc >= 4 ? atoi(argv[3]) : Aspect_TOM_PLUS), 2, 2, 0);
  TheAIS2DContext()->Display(aIO, Standard_True);
  return 0;
}
static int drawAxis (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if( argc < 1 ) {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull()) { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }
  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  
  Standard_Real loc_x = 24., loc_y = 84.;
  if( argc >= 2 )
    loc_x = atof( argv[1] );
  if( argc >= 3 )
    loc_y = atof( argv[2] );
  gp_Pnt2d location(loc_x, loc_y);
  
  Standard_Real dir_x = 0., dir_y = 1.;
  if( argc >= 4 )
    dir_x = atof( argv[3] );
  if( argc >= 5 )
    dir_y = atof( argv[4] );
  gp_Dir2d direction(dir_x, dir_y);
  
  gp_Dir2d refdirection(0.0, 1.0);
  gp_Ax22d axis(location, direction, refdirection);
  Handle_Prs2d_Axis axis2 = new Prs2d_Axis(aIO, axis, 48, 24.0, 24.0, Prs2d_TOA_OPENED, 10.0); 
  TheAIS2DContext()->Display(aIO, Standard_True);
  return 0;
}

static int drawLine (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if( argc < 1 ) {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull()) { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }
  Standard_Real x1 = -11., y1 = -250., d1 = 100., d2 = 200.;
   
  if( argc >= 2 )
    x1 = atof( argv[1] );
  if( argc >= 3 )
    y1 = atof( argv[2] );
  if( argc >= 4 )
    d1 = atof( argv[3] );
  if( argc == 5 )
    d2 = atof( argv[4] );

  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  gp_Pnt2d FirstPt(x1, y1);
  gp_Dir2d direction(d1, d2);
  Handle_Geom2d_Line line = new Geom2d_Line(FirstPt, direction);
  Handle(GGraphic2d_Curve) grCurve = new GGraphic2d_Curve(aIO, line);

  TheAIS2DContext()->Display(aIO, Standard_True );

  return 0;
}
static int drawSegment (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if( argc < 1 ) {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull()) { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  Standard_Real x1 = 0., y1 = 20., x2 = 100., y2 = 200.;
   
  if( argc >= 2 )
    x1 = atof( argv[1] );
  if( argc >= 3 )
    y1 = atof( argv[2] );
  if( argc >= 4 )
    x2 = atof( argv[3] );
  if( argc == 5 )
    y2 = atof( argv[4] );

  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  Handle(Graphic2d_Segment) theSegment = new Graphic2d_Segment( aIO, x1, y1, x2, y2 );
  TheAIS2DContext()->Display(aIO, Standard_True);
  return 0;
}

static int drawArrowLine (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if( argc < 1 ) {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull()) { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }
  
  Standard_Real x1 = 24, y1 = 24, x2 = 420, y2 = 420;
   
  if( argc >= 3 )
    x1 = atof( argv[2] );
  if( argc >= 4 )
    y1 = atof( argv[3] );
  if( argc >= 5 )
    x2 = atof( argv[4] );
  if( argc == 6 )
    y2 = atof( argv[5] );


  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  gp_Pnt2d FirstPt(x1, y1);
  gp_Pnt2d LastPt(x2, y2);
  double arrow_angle = 24;
  double arrow_length = FirstPt.Distance(LastPt)/10.;
  int arrow_type = 1;
  
  gp_Ax2d ax;
  ax = gp_Ax2d(FirstPt, gp_Dir2d(gp_Vec2d(LastPt, FirstPt)));
  
  Handle(Prs2d_Arrow) h_arrow;
  h_arrow = new Prs2d_Arrow(aIO, ax, arrow_angle, arrow_length, Prs2d_TypeOfArrow(arrow_type));
  
  GCE2d_MakeSegment segment(FirstPt, LastPt);
  Handle(Geom2d_TrimmedCurve) trCur = segment.Value();
  Handle(GGraphic2d_Curve) grCurve = new GGraphic2d_Curve(aIO, trCur);

  GetMapOfAIS2D().Bind(aIO, argv[1]);
  TheAIS2DContext()->Display(aIO, Standard_True );
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;
}


static int draw2DText (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
 
  if( argc < 2 ) {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }
  
  Standard_Real x1 = 24, y1 = 24;
  Quantity_PlaneAngle anAngle = 0.0;
  Quantity_Factor     aScale = 1.0;
  Standard_Boolean isUnderline = Standard_False;
   
  if( argc >= 4 )
    x1 = atof( argv[3] );
  if( argc >= 5 )
    y1 = atof( argv[4] );
  if( argc >= 6 )
    anAngle = atof( argv[5] );
  if( argc >= 7 )
    aScale = atof( argv[6] );
  if( argc >= 8 )
    isUnderline = atoi( argv[7] );
  
    

  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  TCollection_ExtendedString aString(argv[2]);
  Handle(Graphic2d_Text) aText = new Graphic2d_Text(aIO,aString,x1,y1, anAngle, Aspect_TOT_SOLID, aScale);
  aText->SetUnderline(isUnderline);
  
  Handle(Prs2d_AspectText) aspect = new Prs2d_AspectText();
  aspect->SetFont(Aspect_TOF_HELVETICA);
  aspect->SetColor(Quantity_NOC_WHITE);
  aIO->SetAspect(aspect);
  
  Standard_Integer color_index = 2;
  aText->SetColorIndex(color_index);
   
  
  GetMapOfAIS2D().Bind(aIO, argv[1]);
  TheAIS2DContext()->Display(aIO, Standard_True );
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;
}

static int drawToleanceFrame( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 1 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  Standard_Real aX = 24, aY = 48;
  Standard_Real height = 24;

  if( argc >= 2 )
    height = atof( argv[1] );
  
  if( argc >= 3 )
    aX = atof( argv[2] );
  if( argc >= 4 )
    aX = atof( argv[3] );

  gp_Pnt2d location(aX, aY);
  
  int tol_type = 10;
  if( argc >= 5 )
   tol_type = atoi( argv[4] );

  double angle = 0.0;
  double text_scale = 10.0;
  
  Handle(Prs2d_ToleranceFrame) symbol;
  symbol = new Prs2d_ToleranceFrame( aIO, location.X(), location.Y(), height,
                                    Prs2d_TypeOfTolerance(tol_type), "B", "-0.01", " A", angle, text_scale);
  
  TheAIS2DContext()->Display(aIO, Standard_True );
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;
}

static int drawArc( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 1 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
	
  Standard_Real x1 = 0., y1 = 0., x2 = 24., y2 = 24., x3 = 12., y3 = 48.;
   
  if( argc >= 2 )
    x1 = atof( argv[1] );
  if( argc >= 3 )
    y1 = atof( argv[2] );
  if( argc >= 4 )
    x2 = atof( argv[3] );
  if( argc >= 5 )
    y2 = atof( argv[4] );
  if( argc >= 6)
    x3= atof( argv[5] );
  if( argc >= 7 )
    y3 = atof( argv[6] );

  gp_Pnt2d point1(x1, y1);
  gp_Pnt2d point2(x2, y2);
  gp_Pnt2d point3(x3, y3);
  
  GCE2d_MakeArcOfCircle arc(point1, point2, point3);
  Handle_Geom2d_TrimmedCurve trCur = arc.Value();
  
  Handle_GGraphic2d_Curve grCurve = new GGraphic2d_Curve(aIO, trCur);
  TheAIS2DContext()->Display(aIO, Standard_True);
  return 0;
}

static int drawFinishSymbol( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 1 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  gp_Pnt2d location(48, 84);
  double length = 10;
  double angle = 0;

  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  
  Handle(Graphic2d_Polyline) symbol;
  Handle(Graphic2d_Text) text;
  
  gp_Pnt2d startPoint(location.X() + length/4, location.Y() + length/2);
  gp_Pnt2d leftTopPoint(location.X() - length/4, location.Y() + length/2);
  gp_Pnt2d btCenPoint(location.X(), location.Y());
  gp_Pnt2d rightTopPoint(location.X() + length/2, location.Y() + length);
				
  gp_Trsf2d trsf;
  trsf.SetRotation(location, angle*PI/180);
  
  startPoint.Transform(trsf);
  leftTopPoint.Transform(trsf);
  btCenPoint.Transform(trsf);
  rightTopPoint.Transform(trsf);
  
  Graphic2d_Array1OfVertex vertex_arr(1, 4);
  vertex_arr.SetValue( 1, Graphic2d_Vertex(startPoint.X(), startPoint.Y()));
  vertex_arr.SetValue( 2, Graphic2d_Vertex(leftTopPoint.X(), leftTopPoint.Y()));
  vertex_arr.SetValue( 3, Graphic2d_Vertex(btCenPoint.X(), btCenPoint.Y()));
  vertex_arr.SetValue( 4, Graphic2d_Vertex(rightTopPoint.X(), rightTopPoint.Y()));
  
  symbol = new Graphic2d_Polyline(aIO, vertex_arr);
  TheAIS2DContext()->Display(aIO, Standard_True);
  return 0;
}
static int drawBezierCurve( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 1 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  
  TColgp_Array1OfPnt2d ptarr(1, 5);
  gp_Pnt2d p1(24, 24);  
  gp_Pnt2d p2(-48, 42);  
  gp_Pnt2d p3(80, 82);  
  gp_Pnt2d p4(240, -24);  
  gp_Pnt2d p5(-24, -84);  
  
  ptarr.SetValue( 1, p1);
  ptarr.SetValue( 2, p2);
  ptarr.SetValue( 3, p3);
  ptarr.SetValue( 4, p4);
  ptarr.SetValue( 5, p5);
  
  Handle(Geom2d_BezierCurve) bezierCurve = 
    new Geom2d_BezierCurve(ptarr);
  Handle_GGraphic2d_Curve grCurve = new GGraphic2d_Curve(aIO, bezierCurve);
  TheAIS2DContext()->Display(aIO, Standard_True);
  return 0;
}


static int draw2DCurve( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 1 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  Handle(Geom2d_Curve) aCurve = DrawTrSurf::GetCurve2d(argv[2]);
  Handle(GGraphic2d_Curve) grCurve = new GGraphic2d_Curve(aIO, aCurve);
  
  GetMapOfAIS2D().Bind(aIO, argv[1]);                                                     
  TheAIS2DContext()->Display(aIO, Standard_True);
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;
}

static int v2dtransform( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 7 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }
  
  if(!GetMapOfAIS2D().IsBound2(argv[1])) {
    cout<<"Object not found"<<endl;
    return 0;
  }
  
  Handle(AIS2D_InteractiveObject) aIO = GetMapOfAIS2D().Find2(argv[1]);
  gp_Trsf2d aTrans, aT1,aT2;
  gp_Pnt2d aP(atof(argv[2]),atof(argv[3]));
  Standard_Real anAngle = atof(argv[4]);
  gp_Pnt2d aP2(atof(argv[5]),atof(argv[6]));
  aT1.SetRotation(aP,anAngle);
  aT2.SetTranslation(aP2.XY());
  aTrans = aT1*aT2;
  
  aIO->SetTransform(gp_GTrsf2d(aTrans));
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;  
}

static int v2dsetstyle( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 3 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }
  
  if(!GetMapOfAIS2D().IsBound2(argv[1])) {
    cout<<"Object not found"<<endl;
    return 0;
  }
  
  Handle(AIS2D_InteractiveObject) aIO = GetMapOfAIS2D().Find2(argv[1]);
  
  Handle(Graphic2d_Primitive) grPrim = aIO->Primitive(1);
  if(aIO->HasAspect(grPrim)) {
    Handle(Prs2d_AspectRoot) aspRoot = aIO->GetAspect(grPrim);
    if(!aspRoot.IsNull() && aspRoot->GetAspectName() == Prs2d_AN_LINE) {
      Handle(Prs2d_AspectLine) aspLine = Handle(Prs2d_AspectLine)::DownCast(aspRoot);
      aspLine->SetType(Aspect_TypeOfLine(atoi(argv[2])));
    }	
  }
  
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;  
}



static int v2dtest1 (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }
  
  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  
  gp_Pnt2d firstPoint ( 911.84, 851.86);
  gp_Pnt2d newPoint (947.78, 847.375);
  
  Handle(Prs2d_Length) theLength;
  theLength = new Prs2d_Length( aIO, firstPoint, newPoint,"     -0.2", 3, 2, 
                               Prs2d_TypeOfDist(Prs2d_TOD_AUTOMATIC), 
                               20, 5, Prs2d_TypeOfArrow(Prs2d_TOA_FILLED), 
                               Prs2d_ArrowSide(Prs2d_AS_BOTHAR), Standard_True);
  
  TheAIS2DContext()->Display(aIO, Standard_True );
  
  
  aIO = new AIS2D_InteractiveObject();
  gp_Pnt2d pt1 ( 209.30, 1405.29 );
  gp_Pnt2d pt2 ( 409.3, 1396.79);
  gp_Pnt2d pt3 (209.3, 1205.278);

  
  Handle(Prs2d_Angle) theAngle;
  theAngle = new Prs2d_Angle( aIO, pt1, pt2, pt3,100,"  ", 3, 20, 6, Prs2d_TypeOfArrow(Prs2d_TOA_FILLED),
                             Prs2d_ArrowSide(Prs2d_AS_BOTHAR), Standard_False);
  
  TheAIS2DContext()->Display(aIO, Standard_True );
  
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;  
}

static int v2dtable (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
 
  if( argc < 6 ) {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }
  
  Standard_Real x1 = 24, y1 = 24;
  Standard_Integer numberOfColumn = 1, numberOfRows = 1;
  Quantity_PlaneAngle anAngle = 0.0;
  Quantity_Factor     aScale = 1.0;
   
  if( argc >= 3 )
    x1 = atof( argv[2] );
  if( argc >= 4 )
    y1 = atof( argv[3] );
  if( argc >= 5 )
    numberOfColumn = atoi( argv[4] );
  if( argc >= 6 )
    numberOfRows = atoi( argv[5] );
  
  Handle(AIS2D_InteractiveObject) aisObj = new AIS2D_InteractiveObject();

  Handle(GGraphic2d_SetOfCurves) setOfCurve = new GGraphic2d_SetOfCurves(aisObj);

  gp_Pnt2d location(x1,y1);
  Standard_Real columnWidth = 100;
  Standard_Real rowHeight = 10;
  Standard_Integer i;
  
  Handle(Geom2d_Curve) trCur;
  
  for( i=0; i<=numberOfColumn; ++i) {
    GCE2d_MakeSegment segment(	gp_Pnt2d(location.X() + (columnWidth * i), location.Y()),
                                gp_Pnt2d(location.X() + (columnWidth * i), location.Y() + (rowHeight * numberOfRows)) );
    
    trCur = segment.Value();
    setOfCurve->Add(trCur);
  }

  for( i=0; i<=numberOfRows; ++i) {
    GCE2d_MakeSegment segment1(	gp_Pnt2d(location.X(), location.Y() + (rowHeight * i)),
                                gp_Pnt2d(location.X() + (numberOfColumn * columnWidth),	location.Y() + (rowHeight * i)) );
    trCur = segment1.Value();
    setOfCurve->Add(trCur);
  }

  Handle(Graphic2d_Text) text;
  Standard_Real locationX = (location.X() + (columnWidth / 2));
  Standard_Real locationY = (location.Y() + (rowHeight / 2));
  Handle(Prs2d_Point) point;
  
  Standard_Real text_scale = 2.0;
  
  for(Standard_Integer j=0,  k=0; j<numberOfRows; ++j) {
    for( i=0; i<numberOfColumn; ++i) {
      
      TCollection_ExtendedString str("TableCell");
      str+=i;
      str+=" ";
      str+=k;
      
      Standard_Integer str_len = str.Length();
      
      Standard_Real textLength = str_len * text_scale * 1.8;
      Standard_Real textHight = text_scale * 1.6;

      text = new Graphic2d_Text(aisObj, str, locationX + (columnWidth * i) - (textLength /2 ),
                                locationY + (rowHeight * j ) - (textHight / 2),
                                0, Aspect_TypeOfText(Aspect_TOT_SOLID), text_scale);
      ++k;
    }
  }
  
  GetMapOfAIS2D().Bind(aisObj, argv[1]);
  TheAIS2DContext()->Display(aisObj, Standard_True );
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;
}



static int drawEllipse( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 1 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  Handle(Geom2d_Ellipse) ellipse ;
  gp_Ax2d axis(gp_Pnt2d(24, 42), gp_Dir2d(1, 0));
  ellipse = new Geom2d_Ellipse(axis, 420, 240, Standard_True);
  Handle_GGraphic2d_Curve grCurve = new GGraphic2d_Curve(aIO, ellipse);
  TheAIS2DContext()->Display(aIO, Standard_True);
  return 0;
}

static int drawLengthDim( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 1 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  

  int distance_type = 0; 
  if (argc >= 2) {
    distance_type = atoi(argv[1]);
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  gp_Pnt2d FirstPt(0, 0);
  gp_Pnt2d LastPt(240, 240);
  
  double text_scale = 5;
  double length = 100; 
  double arrow_angle = 24;
  double arrow_length = 24;
  int arrow_type = 2;
  int arrow_side = 3;
  Standard_Boolean isReverse = Standard_True;
  
  double distance = FirstPt.Distance(LastPt);
  
  char str_[10];
  sprintf(str_, "%.2f", distance);
  
  TCollection_ExtendedString str(str_);
  
  Handle(Prs2d_Length) theLength;			
  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  
  theLength = 
    new Prs2d_Length(aIO, FirstPt, LastPt, 
                     str, text_scale, length, Prs2d_TypeOfDist(distance_type), 
                     arrow_angle, arrow_length, Prs2d_TypeOfArrow(arrow_type), 
                     Prs2d_ArrowSide(arrow_side), isReverse);
  
  theLength->SetTextRelPos(.5, .5);
  
  TheAIS2DContext()->Display(aIO, Standard_True);
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;
}

static int drawLengthDim1( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 1 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  gp_Pnt2d FirstPt(0, 0);
  gp_Pnt2d LastPt(2, 2);
  
  gp_Lin2d aL1(FirstPt,gp_Dir2d(0,1));
  gp_Lin2d aL2(LastPt,gp_Dir2d(0,1));
  
  double text_scale = 15;
  double length = 0; 
  int distance_type = 0; 
  double arrow_angle = 24;
  double arrow_length = 24;
  int arrow_type = 2;
  int arrow_side = 3;
  
  double distance = FirstPt.Distance(LastPt);
  
  char str_[10];
  sprintf(str_, "%.2f", distance);
  
  TCollection_ExtendedString str(str_);
  
  Handle(Prs2d_Length) theLength;			
  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  
  
  theLength = 
    new Prs2d_Length(aIO, aL1, aL2, 
                     str, text_scale, length);
//  theLength = 
//    new Prs2d_Length(aIO, aL1, aL2, 
//                     str, text_scale, length, arrow_angle,arrow_length,
//                     Prs2d_TypeOfDist(distance_type), 
//                     Prs2d_TypeOfArrow(arrow_type), 
//                     Prs2d_ArrowSide(arrow_side), Standard_False);
  
  theLength->SetTextRelPos(.5, .5);
  
  TheAIS2DContext()->Display(aIO, Standard_True);
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;
}



static int drawAngleDim( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 1 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  Standard_Boolean isReverse = Standard_False;

  if( argc >= 1 )
    isReverse = atoi(argv[1]);
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  gp_Pnt2d anAp1(0, 0);
  gp_Pnt2d anAp2(240, 240);
  gp_Pnt2d anAp3(0, 240);
  
  Quantity_Length aRadius = 100;
  TCollection_ExtendedString aText("Rad = 10%");
  
  Handle(Prs2d_Angle) theAngle;			
  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();

  if(isReverse) {
    theAngle = new Prs2d_Angle(aIO, anAp1, anAp3, anAp2, aRadius, aText,1,10.,5.,Prs2d_TypeOfArrow(1),Prs2d_ArrowSide(2),isReverse);
  }
  else {
  theAngle = 
    new Prs2d_Angle(aIO, anAp1, anAp2, anAp3, aRadius, aText,1,10.,5.,Prs2d_TypeOfArrow(1),Prs2d_ArrowSide(2),isReverse);
  }
  
  theAngle->SetTextRelPos(.5, .5);
  
  TheAIS2DContext()->Display(aIO, Standard_True);
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;
}  

static int drawRadiusDim( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 1 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  Standard_Boolean isReverse = Standard_False;

  if( argc >= 2 )
    isReverse = atoi(argv[1]);
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  gp_Pnt2d anAp1(0, 0);
  gp_Pnt2d anAp2(240, 240);
  gp_Circ2d aCirc(gp_Ax2d(anAp2,gp_Dir2d(0,1)),100.);
  
  Quantity_Length aRadius = 100;
  TCollection_ExtendedString aText("Rad = 10%");
  
  Handle(Prs2d_Radius) theRadius;			
  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();

  theRadius = new Prs2d_Radius(aIO, anAp1, aCirc,  aText,1,10.,5.,Prs2d_TypeOfArrow(1),Prs2d_AS_BOTHAR,isReverse);
  theRadius->SetTextRelPos(.5, .5);
  
  TheAIS2DContext()->Display(aIO, Standard_True);
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;
}  


static int drawSymbDim( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 1 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  gp_Pnt2d anAp1(0, 0);
  gp_Pnt2d anAp2(240, 240);
  gp_Circ2d aCirc(gp_Ax2d(anAp2,gp_Dir2d(0,1)),100.);
  
  Quantity_Length aRadius = 100;
  TCollection_ExtendedString aText("Rad = 10%");
  
  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  Handle(Prs2d_Circularity) symbol;
  
  Standard_Real length = 100;
  Standard_Real angle = 270;
  symbol = new Prs2d_Circularity( aIO, anAp2.X(), anAp2.Y(), length, angle);

  
  TheAIS2DContext()->Display(aIO, Standard_True);
  
  aIO = new AIS2D_InteractiveObject();
  Handle(Prs2d_Parallelism) symbol1;
  symbol1 = new Prs2d_Parallelism( aIO, anAp2.X()+10, anAp2.Y()+10, length, angle); 
  TheAIS2DContext()->Display(aIO, Standard_True);
  
  TheAIS2DContext()->UpdateCurrentViewer();
  return 0;
}  


static int drawDiameter( Draw_Interpretor& , Standard_Integer argc, const char** argv )
{
  if( argc < 2 )
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (Viewer2dTest::CurrentView().IsNull())
  { 
    cout << "2D AIS Viewer Initialization ..." << endl;
    Viewer2dTest::ViewerInit(); 
    cout << "Done" << endl;
  }

  Standard_Real aRadius = 240.;
  aRadius = atof( argv[1] );

  Standard_Real aX = 0., aY = 0.;
  
  if( argc >= 3 )
    aX = atof( argv[2] );
  if( argc >= 4 )
    aY = atof( argv[3] );
    
  gp_Pnt2d cirLoc( aX, aY );

  Handle(AIS2D_InteractiveObject) aIO = new AIS2D_InteractiveObject();
  gp_Dir2d xdir(1.0,0.0);
  gp_Dir2d ydir(0.0,1.0);
  gp_Ax22d ax2d(cirLoc, xdir, ydir);
  
  
  gp_Pnt2d attachPt(cirLoc.X()+aRadius, cirLoc.Y());
  gp_Trsf2d trsf;
  trsf.SetRotation(cirLoc, (3.142/180)*-60);
  attachPt.Transform(trsf);
  gp_Circ2d aCircle(ax2d, aRadius);
  Handle(Graphic2d_Circle) theCircle = new Graphic2d_Circle( aIO, 0., 0.,aRadius);
  Handle(Prs2d_Diameter) theDiam =
    new Prs2d_Diameter(aIO, attachPt, aCircle, TCollection_ExtendedString( aRadius*2. ), 
                       24.0, 24.0, 24.0, Prs2d_TOA_CLOSED, Prs2d_AS_BOTHAR, Standard_False);
  theDiam->DrawSymbol(Standard_True); 
  theDiam->SetSymbolCode(233);
  
  TheAIS2DContext()->Display(aIO, Standard_True);
  TheAIS2DContext()->UpdateCurrentViewer();
  
  return 0;
}

static int NbAllObjects (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc > 1)  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }

  if (Viewer2dTest::CurrentView().IsNull()) return 1;
  
  AIS2D_ListOfIO aListObj;
  TheAIS2DContext()->DisplayedObjects( aListObj );
  int aNb = aListObj.Extent();

  cout << " Number all objects = " << aNb << endl;
  return 0;
}



//==============================================================================
//function : Viewer2dTest::Commands
//purpose  : Add all the viewer command in the Draw_Interpretor
//==============================================================================
void Viewer2dTest::DisplayCommands (Draw_Interpretor& theCommands)
{
  const char *group = "2D AIS Viewer - Display Commands";

  theCommands.Add("v2ddisplay",
		  "v2ddisplay name [projection(Origin(x y z) Normal(dx dy dz) DX(dx dy dz)]",
		  __FILE__, V2dDisplay, group);

  theCommands.Add("v2derase",
		  "v2derase [name1] ...  [name n]",
		  __FILE__, V2dErase, group);

  theCommands.Add("v2ddonly",
		  "v2ddonly [name1] ...  [name n]",
		  __FILE__, V2dDonly, group);

  theCommands.Add("v2ddisplayall",
		  "v2ddisplayall",
		  __FILE__, V2dDisplayAll, group);

  theCommands.Add("v2deraseall",
		  "v2deraseall",
		  __FILE__, V2dEraseAll, group);

//  theCommands.Add("v2derasetype",
//		  "v2derasetype <Type> : erase all the displayed objects of one given kind (see vtypes)",
//		  __FILE__, V2dEraseType, group);

//  theCommands.Add("v2ddisplaytype",
//		  "v2ddisplaytype <Type> <Signature> : display all the objects of one given kind (see vtypes) which are stored the AIS2DContext",
//		  __FILE__, V2dDisplayType, group);
  
/*
  theCommands.Add("v2ddisplaymode",
		  "v2ddispmode [name] mode(1,2,..) : no name -> on selected objects",
		  __FILE__, V2dDispMode, group);

  theCommands.Add("v2derasemode",
		  "v2derasemode [name] mode(1,2,..) : no name -> on selected objects",
		  __FILE__, V2dDispMode, group);

  theCommands.Add("v2dsetdispmode",
		  "v2dsetdispmode [name] mode(1,2,..) : no name -> on selected objects",
		  __FILE__, V2dDispMode, group);

  theCommands.Add("v2dunsetdispmode",
		  "v2dunsetdispmode [name] mode(1,2,..) : no name -> on selected objects",
		  __FILE__, V2dDispMode, group);
*/
  theCommands.Add("v2dsetcolor",
		  "v2dsetcolor [name of shape] ColorName",
		  __FILE__, V2dColor, group);

  theCommands.Add("v2dunsetcolor",
		  "v2dunsetcolor [name of shape]",
		  __FILE__, V2dColor, group);

  theCommands.Add("v2dsetbgcolor",
		  "v2dsetbgcolor colorname       : Change background color",
		  __FILE__, V2dSetBGColor, group);

 /*
 theCommands.Add("v2dsettransparency",
		  "v2dsettransparency [name of shape] TransparencyCoef(0 -> 1)",
		  __FILE__, V2dTransparency, group);

  theCommands.Add("v2dunsettransparency",
		  "v2dunsettransparency [name of shape]",
		  __FILE__, V2dTransparency, group);

  theCommands.Add("v2dsetmaterial",
		  "v2dsetmaterial [name of shape] MaterialName",
		  __FILE__, V2dMaterial, group);

  theCommands.Add("v2dunsetmaterial",
		  "v2dunsetmaterial [name of shape]",
		  __FILE__, V2dMaterial, group);
*/
  theCommands.Add("v2dsetwidth",
		  "v2dsetwidth [name_of_shape] width(THIN, MEDIUM, THICK, VERYTHICK)",
		  __FILE__, V2dWidth, group);

  theCommands.Add("v2dunsetwidth",
		  "v2dunsetwidth [name of shape]",
		  __FILE__, V2dWidth, group);
/*
  theCommands.Add("v2dsetshading",
		  "v2dsetshading name Quality(default=0.0008)",
		  __FILE__, V2dShading, group);

  theCommands.Add("v2dunsetshading",
		  "v2dunsetshading name",
		  __FILE__, V2dShading, group);
*/
/*  theCommands.Add("v2dtexture",
		  "'v2dtexture NameOfShape TextureFile' \n \
                  or 'vtexture NameOfShape' if you want to disable texture mapping \n \
                  or 'vtexture NameOfShape ?' to list available textures\n \
                  or 'vtexture NameOfShape IdOfTexture' (0<=IdOfTexture<=20)' to use predefined textures\n",
		  __FILE__, VTexture, group);*/
  
  //Additional commands for primitives
  theCommands.Add("v2dsetlinetype",
		  "v2dsetlinetype [name of shape] LineType(SOLID, DASH, DOT, DOTDASH )",
		  __FILE__, V2dTypeOfLine, group);

  theCommands.Add("v2dunsetlinetype",
		  "v2dunsetlinetype [name of shape]",
		  __FILE__, V2dTypeOfLine, group);

  
  theCommands.Add("v2dpoint",
		  "v2dpoint [x y marker_type( 0(TOM_POINT),\n \
                                        1(TOM_PLUS),\n \
                                        2(TOM_STAR),\n \
                                        3(TOM_O),\n \
                                        4(TOM_X),\n \
                                        5(TOM_O_POINT),\n \
                                        6(TOM_O_PLUS),\n \
                                        7(TOM_O_STAR),\n \
                                        8(TOM_O_X) )]",
		  __FILE__, drawPoint, group);
  
  theCommands.Add("v2ddrawaxis",
		  "v2ddrawaxis[loc_x loc_y dir_x dir_y]",
		  __FILE__, drawAxis, group);
  
  theCommands.Add("v2dline",
		  "v2dline [ x y dir_x dir_y ]",
		  __FILE__, drawLine, group);
  
  theCommands.Add("v2dsegment",
		  "v2dsegment [ x1 y1 x2 y2 ]",
		  __FILE__, drawSegment, group);

  theCommands.Add("v2darrow",
		  "v2ddrawArrowLine name [ x1 y1 x2 y2 ]",
		  __FILE__, drawArrowLine, group);

  theCommands.Add("v2ddrawArc",
		  "v2ddrawArc [ x1 y1 x2 y2 x3 y3]",
		  __FILE__, drawArc, group);
  
  theCommands.Add("v2dFinishSymbol",
		  "v2dFinishSymbol",
		  __FILE__, drawFinishSymbol, group);
  
  theCommands.Add("v2dBezierCurve",
		  "v2dBezierCurve",
		  __FILE__, drawBezierCurve, group);
  
  theCommands.Add("v2dEllipse",
		  "v2dEllipse",
		  __FILE__, drawEllipse, group);
    
  theCommands.Add("v2daistext",
                  "v2daistext atextname text_str x1 y1 [angle scale]",
		  __FILE__, draw2DText, group);

  theCommands.Add("v2dtoleranceframe",
                  "v2dtoleranceframe [ height loc_x loc_y tol_type( 0(Prs2d_TOT_TAPER),\n \
                                                                    1(Prs2d_TOT_SYMTOTAL),\n \
                                                                    2(Prs2d_TOT_SYMCIRCULAR),\n \
                                                                    3(Prs2d_TOT_SYMMETRY),\n \
                                                                    4(Prs2d_TOT_CONCENTRIC),\n \
                                                                    5(Prs2d_TOT_POSITION),\n \
                                                                    6(Prs2d_TOT_ANGULARITY),\n \
                                                                    7(Prs2d_TOT_PERPENDIC),\n \
                                                                    8(Prs2d_TOT_PARALLELISM),\n \
                                                                    9(Prs2d_TOT_SURFACEPROF),\n \
                                                                    10(Prs2d_TOT_LINEPROF),\n \
                                                                    11(Prs2d_TOT_CYLINDRIC),\n \
                                                                    12(Prs2d_TOT_CIRCULARITY),\n \
                                                                    13(Prs2d_TOT_FLATNESS),\n \
                                                                    14(Prs2d_TOT_STRAIGHTNESS) )]",
		  __FILE__, drawToleanceFrame, group);

  theCommands.Add("v2dlengthdim",
                  "v2dlengthdim ",
		  __FILE__, drawLengthDim, group);
  
  theCommands.Add("v2dangledim",
                  "v2dangledim",
		  __FILE__, drawAngleDim, group);
  
  theCommands.Add("v2dradiusdim",
                  "v2dradiusdim",
		  __FILE__, drawRadiusDim, group);
  
  theCommands.Add("v2dsymbols",
                  "v2dsymbols",
		  __FILE__, drawSymbDim, group);
  

  
  theCommands.Add("v2dlen1",
                  "v2dlengthdim ",
		  __FILE__, drawLengthDim1, group);


  theCommands.Add("v2ddiameter",
                  "v2ddiameter radius [ circle_x_coord  circle_y_coord ]",
		  __FILE__, drawDiameter, group);
  
  theCommands.Add("v2dcurve",
                  "v2dcurve name name",
                  __FILE__,draw2DCurve, group);
  
  theCommands.Add("v2dtransform",
                  "v2dtransform name a11 a12 a21 a22 s1 s2",
                  __FILE__, v2dtransform, group);
  
  theCommands.Add("v2dtable",
                  "v2dtable name xSt ySt nbR nbC",
                  __FILE__, v2dtable, group);
  
  theCommands.Add("v2dsetsyle",
                  "v2dsetsyle name style",
                   __FILE__, v2dsetstyle, group);
  
  theCommands.Add("v2dtest1",
                  "v2dtest1",
                   __FILE__, v2dtest1, group);


  theCommands.Add("v2dcount",
		  "v2dcount",
		  __FILE__, NbAllObjects, group);
}

