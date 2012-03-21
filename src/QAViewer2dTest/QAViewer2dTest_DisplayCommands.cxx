// Created on: 2002-01-22
// Created by: Julia DOROVSKIKH
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#if defined(WOKC40)

#include <QAViewer2dTest.hxx>

#include <QAViewer2dTest_EventManager.hxx>
#include <QAViewer2dTest_DoubleMapOfInteractiveAndName.hxx>
#include <QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>

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

#define OCC190 // jfa 04/03/2002 // for vertices load
#define OCC154 // jfa 06/03/2002 // for correct erasing

#define DEFAULT_COLOR    Quantity_NOC_GOLDENROD
#define DEFAULT_MATERIAL Graphic3d_NOM_BRASS

QAViewer2dTest_DoubleMapOfInteractiveAndName& GetMapOfAIS2D();
Handle(AIS2D_InteractiveContext)& TheAIS2DContext();
TopoDS_Shape GetShapeFromName2d (char* name);

//==============================================================================
//function : GetAIS2DShapeFromName
//purpose  : Compute an AIS2D_ProjShape from a draw variable or a file name
//==============================================================================
Handle(AIS2D_InteractiveObject) GetAIS2DShapeFromName
       (char* name, gp_Ax2 anAx2 = gp::XOY())
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

      Handle(V2d_View) V = QAViewer2dTest::CurrentView();
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
static Quantity_NameOfColor GetColorFromName2d (char *name) 
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
static Graphic3d_NameOfMaterial GetMaterialFromName2d (char *name) 
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
                                          char** /*argv*/,
                                          Standard_Boolean shift,
                                          Standard_Boolean /*pick*/)
{
  Handle(AIS2D_InteractiveObject) ret;
  Handle (QAViewer2dTest_EventManager) EM = QAViewer2dTest::CurrentEventManager();
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
  Handle(AIS2D_InteractiveContext) Ctx = QAViewer2dTest::GetAIS2DContext();
  
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
static int V2dDispMode (Draw_Interpretor& , Standard_Integer argc, char** argv)
{
  TCollection_AsciiString name;
  if (argc > 3) return 1;

  // display others presentations
  Standard_Integer TypeOfOperation = (strcasecmp(argv[0],"v2ddispmode") == 0)? 1:
    (strcasecmp(argv[0],"v2derasemode") == 0) ? 2 :
      (strcasecmp(argv[0],"v2dsetdispmode") == 0) ? 3 :
	(strcasecmp(argv[0],"v2dunsetdispmode") == 0) ? 4 : -1;
  
  Handle(AIS2D_InteractiveContext) Ctx = QAViewer2dTest::GetAIS2DContext();
  
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
static int V2dSetBGColor (Draw_Interpretor& di, Standard_Integer argc, char** argv)
{
  if (argc != 2)
  {
    di << "Usage : v2dsetbgcolor colorname" << "\n";
    return 1;
  }
  Handle(V2d_View) V = QAViewer2dTest::CurrentView(); 
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

static int V2dColor (Draw_Interpretor& di, Standard_Integer argc, char** argv)
{
  Standard_Boolean ThereIsArgument = Standard_False;
  Standard_Boolean HaveToSet = Standard_False;

  if (strcasecmp(argv[0],"v2dsetcolor") == 0) HaveToSet = Standard_True;

  Quantity_NameOfColor aColor = Quantity_NOC_BLACK;
  if (HaveToSet)
  {
    if (argc < 2 || argc > 3)
    {
      di << "Usage: v2dsetcolor [name] color" << "\n";
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
      di << "Usage: v2dunsetcolor [name]" << "\n";
      return 1;
    }
    if (argc == 2) ThereIsArgument = Standard_True;
  }
  
  if (QAViewer2dTest::CurrentView().IsNull()) return -1;
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
    QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
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
static int V2dTransparency (Draw_Interpretor& di, Standard_Integer argc, char** argv)
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
      di << "Usage: v2dsettransp [name] color" << "\n";
      return 1;
    }
    if (argc == 2) ThereIsArgument = Standard_False;
    else ThereIsArgument = Standard_True;
  }
  else
  {
    if (argc > 2)
    {
      di << "Usage: v2dunsettransp [name]" << "\n";
      return 1;
    }
    if (argc == 2) ThereIsArgument = Standard_True;
    else ThereIsArgument = Standard_False;
  }
  
  if (QAViewer2dTest::CurrentView().IsNull()) return -1;
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
      QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
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
static int V2dMaterial (Draw_Interpretor& di, Standard_Integer argc, char** argv)
{ 
  Standard_Boolean ThereIsName;
  Standard_Boolean HaveToSet;

  if (strcasecmp(argv[0], "v2dsetmaterial") == 0) HaveToSet = Standard_True;
  else HaveToSet = Standard_False;

  if (HaveToSet)
  {
    if (argc < 2 || argc > 3)
    {
      di << "Usage: v2dsetmaterial [name] material" << "\n";
      return 1;
    }
    if (argc == 2) ThereIsName = Standard_False;
    else ThereIsName = Standard_True;
  }
  else
  {
    if (argc > 2)
    {
      di << "Usage: v2dunsetmaterial [name]" << "\n";
      return 1;
    }
    if (argc == 2) ThereIsName = Standard_True;
    else ThereIsName = Standard_False;
  }

  if (QAViewer2dTest::CurrentView().IsNull()) return -1;
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
      QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
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

static int V2dWidth (Draw_Interpretor& di, Standard_Integer argc, char** argv)
{
  Standard_Boolean ThereIsArgument = Standard_False;
  Standard_Boolean HaveToSet = Standard_False;

  if (strcasecmp(argv[0], "v2dsetwidth") == 0) HaveToSet = Standard_True;

  char* aStr;
  if (HaveToSet)
  {
    if (argc < 2 || argc > 3)
    {
      di << "Usage: v2dsetwidth [name] width" << "\n";
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
      di << "Usage: v2dunsetwidth [name]" << "\n";
      return 1;
    }
    if (argc == 2) ThereIsArgument = Standard_True;
  }

  if (QAViewer2dTest::CurrentView().IsNull()) return -1;
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
    QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
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
static int V2dDisplay (Draw_Interpretor& di, Standard_Integer argc, char** argv)
{
  // Verification des arguments
  if (argc != 2 && argc != 11)
  {
    di << "Usage: v2ddisplay name [x y z dx dy dz dx1 dy1 dz1]" << "\n";
    return 1;
  }
  
  if (QAViewer2dTest::CurrentView().IsNull())
  { 
    di << "2D AIS Viewer Initialization ..." << "\n";
    QAViewer2dTest::ViewerInit(); 
    di << "Done" << "\n";
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
      di << "Display " << name << "\n";
      // Get the Shape from a name
      TopoDS_Shape NewShape = GetShapeFromName2d(name.ToCString());

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
    aShape = GetAIS2DShapeFromName(name.ToCString(), anAx2);
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
static int V2dDonly (Draw_Interpretor& , Standard_Integer argc, char** argv)
{
  if (QAViewer2dTest::CurrentView().IsNull()) return 1;
  
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
      QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
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
static int V2dErase (Draw_Interpretor& , Standard_Integer argc, char** argv)
{
  if (QAViewer2dTest::CurrentView().IsNull()) return 1;
  
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
    QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
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
static int V2dEraseAll (Draw_Interpretor& di, Standard_Integer argc, char** )
{
  if (argc > 1)
  {
    di << "Usage: v2deraseall" << "\n";
    return 1;
  }

  if (QAViewer2dTest::CurrentView().IsNull())
  {
    di << " Error: v2dinit hasn't been called." << "\n";
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
static int V2dDisplayAll (Draw_Interpretor& di, Standard_Integer argc, char** argv)
{
  if (QAViewer2dTest::CurrentView().IsNull()) return 1;

  if (argc > 1)
  {
    di << argv[0] << " Syntaxe error" << "\n";
    return 1;
  }
  TheAIS2DContext()->CloseLocalContext();
  QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
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
  
  Handle(AIS2D_InteractiveContext) myAIS2DContext = QAViewer2dTest::GetAIS2DContext();
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
  QAViewer2dTest::CurrentView()->SetSurfaceDetail(V2d_TEX_ALL);
  
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
static int V2dShading (Draw_Interpretor& ,Standard_Integer argc, char** argv)
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
/*static int VEraseType (Draw_Interpretor& , Standard_Integer argc, char** argv) 
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
/*static int V2dDisplayType (Draw_Interpretor& , Standard_Integer argc, char** argv) 
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

//==============================================================================
//function : QAViewer2dTest::Commands
//purpose  : Add all the viewer command in the Draw_Interpretor
//==============================================================================
void QAViewer2dTest::DisplayCommands (Draw_Interpretor& theCommands)
{
  char *group = "2D AIS Viewer - Display Commands";

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
}
#endif
