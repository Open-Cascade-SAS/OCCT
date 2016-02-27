// Created on: 1998-11-12
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
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

#include <ViewerTest.hxx>

#include <Quantity_NameOfColor.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <DBRep.hxx>

#include <Font_BRepFont.hxx>
#include <Font_BRepTextBuilder.hxx>
#include <Font_FontMgr.hxx>
#include <OSD_Chronometer.hxx>
#include <TCollection_AsciiString.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <V3d.hxx>

#include <AIS_Shape.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_PointCloud.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <AIS_MapOfInteractive.hxx>
#include <ViewerTest_AutoUpdater.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_EventManager.hxx>

#include <TopoDS_Solid.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>

#include <Draw_Window.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_Shape.hxx>

#include <AIS_InteractiveContext.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <GC_MakePlane.hxx>
#include <gp_Circ.hxx>
#include <AIS_Axis.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Axis1Placement.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_Axis.hxx>
#include <gp_Trsf.hxx>
#include <TopLoc_Location.hxx>

#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <HLRBRep_PolyHLRToShape.hxx>
#include <Aspect_Window.hxx>

#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_ArrayOfTriangleFans.hxx>
#include <Graphic3d_ArrayOfTriangleStrips.hxx>
#include <Graphic3d_ArrayOfQuadrangles.hxx>
#include <Graphic3d_ArrayOfQuadrangleStrips.hxx>
#include <Graphic3d_ArrayOfPolygons.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Standard_Real.hxx>

#include <AIS_Circle.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Geom_Circle.hxx>
#include <GC_MakeCircle.hxx>
#include <Prs3d_Presentation.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <StdFail_NotDone.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <TopoDS_Wire.hxx>

#include <AIS_MultipleConnectedInteractive.hxx>
#include <AIS_ConnectedInteractive.hxx>
#include <AIS_TextLabel.hxx>
#include <TopLoc_Location.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <Select3D_SensitiveTriangle.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <StdPrs_Curve.hxx>

#include <BRepExtrema_ExtPC.hxx>
#include <BRepExtrema_ExtPF.hxx>

#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_VertexDrawMode.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_TextAspect.hxx>

#include <Image_AlienPixMap.hxx>
#include <TColStd_HArray1OfAsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>

#if defined(_MSC_VER)
# define _CRT_SECURE_NO_DEPRECATE
# pragma warning (disable:4996)
#endif

extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
extern Standard_Boolean VDisplayAISObject (const TCollection_AsciiString& theName,
                                           const Handle(AIS_InteractiveObject)& theAISObj,
                                           Standard_Boolean theReplaceIfExists = Standard_True);
extern int ViewerMainLoop(Standard_Integer argc, const char** argv);
extern Handle(AIS_InteractiveContext)& TheAISContext();


//==============================================================================
//function : Vtrihedron 2d
//purpose  : Create a plane with a 2D  trihedron from a faceselection
//Draw arg : vtri2d  name
//==============================================================================
#include <AIS_PlaneTrihedron.hxx>



static int VTrihedron2D (Draw_Interpretor& di, Standard_Integer argc, const char** argv)

{
  // Verification des arguments
  if ( argc!=2) {di<<argv[0]<<" error\n"; return 1;}

  // Declarations
  Standard_Integer myCurrentIndex;
  // Fermeture des contextes
  TheAISContext()->CloseAllContexts();
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  // On active les modes de selections faces.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
  di<<" Select a face .\n";

  // Boucle d'attente waitpick.
  Standard_Integer argccc = 5;
  const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argvvv = (const char **) bufff;
  while (ViewerMainLoop( argccc, argvvv) ) { }
  // fin de la boucle

  TopoDS_Shape ShapeB;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeB = TheAISContext()->SelectedShape();
  }

  TopoDS_Face  FaceB=TopoDS::Face(ShapeB);

  // Construction du Plane
  // recuperation des edges des faces.
  TopExp_Explorer FaceExpB(FaceB,TopAbs_EDGE);

  TopoDS_Edge EdgeB=TopoDS::Edge(FaceExpB.Current() );
  // declarations
  gp_Pnt A,B,C;

  // si il y a plusieurs edges
  if (FaceExpB.More() ) {
    FaceExpB.Next();
    TopoDS_Edge EdgeC=TopoDS::Edge(FaceExpB.Current() );
    BRepAdaptor_Curve theCurveB(EdgeB);
    BRepAdaptor_Curve theCurveC(EdgeC);
    A=theCurveC.Value(0.1);
    B=theCurveC.Value(0.9);
    C=theCurveB.Value(0.5);
  }
  else {
    // FaceB a 1 unique edge courbe
    BRepAdaptor_Curve theCurveB(EdgeB);
    A=theCurveB.Value(0.1);
    B=theCurveB.Value(0.9);
    C=theCurveB.Value(0.5);
  }
  // Construction du Geom_Plane
  GC_MakePlane MkPlane(A,B,C);
  Handle(Geom_Plane) theGeomPlane=MkPlane.Value();

  // Construction de l'AIS_PlaneTrihedron
  Handle(AIS_PlaneTrihedron) theAISPlaneTri= new AIS_PlaneTrihedron(theGeomPlane );

  // Fermeture du contexte local.
  TheAISContext()->CloseLocalContext(myCurrentIndex);

  // on le display & bind
  TheAISContext()->Display(theAISPlaneTri );
  GetMapOfAIS().Bind ( theAISPlaneTri ,argv[1]);

  return 0;
}



//==============================================================================
//function : VTriherdron
//purpose  : Create a trihedron. If no arguments are set, the default
//           trihedron (Oxyz) is created.
//Draw arg : vtrihedron  name  [Xo] [Yo] [Zo] [Zu] [Zv] [Zw] [Xu] [Xv] [Xw]
//==============================================================================

static int VTrihedron (Draw_Interpretor& /*theDi*/,
                       Standard_Integer  theArgsNb,
                       const char**      theArgVec)
{
  if (theArgsNb < 2 || theArgsNb > 11)
  {
    std::cout << theArgVec[0] << " syntax error\n";
    return 1;
  }

  // Parse parameters
  NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfAsciiString)> aMapOfArgs;
  TCollection_AsciiString aParseKey;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    TCollection_AsciiString anArg (theArgVec [anArgIt]);

    if (anArg.Value (1) == '-' && !anArg.IsRealValue())
    {
      aParseKey = anArg;
      aParseKey.Remove (1);
      aParseKey.LowerCase();
      aMapOfArgs.Bind (aParseKey, new TColStd_HSequenceOfAsciiString);
      continue;
    }

    if (aParseKey.IsEmpty())
    {
      continue;
    }

    aMapOfArgs(aParseKey)->Append (anArg);
  }

  // Check parameters
  if ( (aMapOfArgs.IsBound ("xaxis") && !aMapOfArgs.IsBound ("zaxis"))
    || (!aMapOfArgs.IsBound ("xaxis") && aMapOfArgs.IsBound ("zaxis")) )
  {
    std::cout << theArgVec[0] << " error: -xaxis and -yaxis parameters are to set together.\n";
    return 1;
  }

  for (NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfAsciiString)>::Iterator aMapIt (aMapOfArgs);
       aMapIt.More(); aMapIt.Next())
  {
    const TCollection_AsciiString& aKey = aMapIt.Key();
    const Handle(TColStd_HSequenceOfAsciiString)& anArgs = aMapIt.Value();

    // Bool key, without arguments
    if (aKey.IsEqual ("hidelabels") && anArgs->IsEmpty())
    {
      continue;
    }

    if ( (aKey.IsEqual ("xaxis") || aKey.IsEqual ("zaxis") || aKey.IsEqual ("origin")) && anArgs->Length() == 3
      && anArgs->Value(1).IsRealValue() && anArgs->Value(2).IsRealValue() && anArgs->Value(3).IsRealValue() )
    {
      continue;
    }
  }

  // Process parameters
  gp_Pnt anOrigin (0.0, 0.0, 0.0);
  gp_Dir aDirZ = gp::DZ();
  gp_Dir aDirX = gp::DX();

  Handle(TColStd_HSequenceOfAsciiString) aValues;

  if (aMapOfArgs.Find ("origin", aValues))
  {
    anOrigin.SetX (aValues->Value(1).RealValue());
    anOrigin.SetY (aValues->Value(2).RealValue());
    anOrigin.SetZ (aValues->Value(3).RealValue());
  }

  Handle(TColStd_HSequenceOfAsciiString) aValues2;
  if (aMapOfArgs.Find ("xaxis", aValues) && aMapOfArgs.Find ("zaxis", aValues2))
  {
    Standard_Real aX = aValues->Value(1).RealValue();
    Standard_Real aY = aValues->Value(2).RealValue();
    Standard_Real aZ = aValues->Value(3).RealValue();
    aDirX.SetCoord (aX, aY, aZ);

    aX = aValues->Value(1).RealValue();
    aY = aValues->Value(2).RealValue();
    aZ = aValues->Value(3).RealValue();
    aDirZ.SetCoord (aX, aY, aZ);
  }

  if (!aDirZ.IsNormal (aDirX, M_PI / 180.0))
  {
    std::cout << theArgVec[0] << " error - VectorX is not normal to VectorZ\n";
    return 1;
  }

  Handle(Geom_Axis2Placement) aPlacement = new Geom_Axis2Placement (anOrigin, aDirZ, aDirX);
  Handle(AIS_Trihedron) aShape = new AIS_Trihedron (aPlacement);

  if (aMapOfArgs.Find ("hidelabels", aValues))
  {
    const Handle(Prs3d_Drawer)& aDrawer = aShape->Attributes();

    if(!aDrawer->HasOwnDatumAspect())
    {
      Handle(Prs3d_DatumAspect) aDefAspect = ViewerTest::GetAISContext()->DefaultDrawer()->DatumAspect();

      Handle(Prs3d_DatumAspect) aDatumAspect = new Prs3d_DatumAspect();
      aDatumAspect->FirstAxisAspect()->SetAspect (aDefAspect->FirstAxisAspect()->Aspect());
      aDatumAspect->SecondAxisAspect()->SetAspect (aDefAspect->SecondAxisAspect()->Aspect());
      aDatumAspect->ThirdAxisAspect()->SetAspect (aDefAspect->ThirdAxisAspect()->Aspect());
      aDatumAspect->SetAxisLength (aDefAspect->FirstAxisLength(),
                                   aDefAspect->SecondAxisLength(),
                                   aDefAspect->ThirdAxisLength());

      aDrawer->SetDatumAspect (aDatumAspect);
    }

    aDrawer->DatumAspect()->SetToDrawLabels (Standard_False);
  }

  VDisplayAISObject (theArgVec[1], aShape);
  return 0;
}

//==============================================================================
//function : VSize
//author   : ege
//purpose  : Change the size of a named or selected trihedron
//           if no name : it affects the trihedrons witch are selected otherwise nothing is donne
//           if no value, the value is set at 100 by default
//Draw arg : vsize [name] [size]
//==============================================================================

static int VSize (Draw_Interpretor& di, Standard_Integer argc, const char** argv)

{
  // Declaration de booleens
  Standard_Boolean             ThereIsName;
  Standard_Boolean             ThereIsCurrent;
  Standard_Real                value;
  Standard_Boolean             hascol;

  Quantity_NameOfColor         col = Quantity_NOC_BLACK ;

  // Verification des arguments
  if ( argc>3 ) {di<<argv[0]<<" Syntaxe error\n"; return 1;}

  // Verification du nombre d'arguments
  if (argc==1)      {ThereIsName=Standard_False;value=100;}
  else if (argc==2) {ThereIsName=Standard_False;value=Draw::Atof(argv[1]);}
  else              {ThereIsName=Standard_True;value=Draw::Atof(argv[2]);}

  // On ferme le contexte local pour travailler dans le contexte global
  if(TheAISContext()->HasOpenedContext())
    TheAISContext()->CloseLocalContext();

  // On set le booleen ThereIsCurrent
  if (TheAISContext() -> NbSelected() > 0) {ThereIsCurrent=Standard_True;}
  else {ThereIsCurrent=Standard_False;}



  //===============================================================
  // Il n'y a pas de nom  mais des objets selectionnes
  //===============================================================
  if (!ThereIsName && ThereIsCurrent)
  {

    ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
      it (GetMapOfAIS());

    while ( it.More() ) {

      Handle(AIS_InteractiveObject) aShape=
        Handle(AIS_InteractiveObject)::DownCast(it.Key1());

      if (!aShape.IsNull() &&  TheAISContext()->IsSelected(aShape) )
      {

        // On verifie que l'AIS InteraciveObject selectionne est bien
        // un AIS_Trihedron
        if (aShape->Type()==AIS_KOI_Datum && aShape->Signature()==3) {

          if (aShape->HasColor()) {
            hascol=Standard_True;

            // On recupere la couleur de aShape
            col=aShape->Color();}

          else hascol=Standard_False;

          // On downcast aShape  de AIS_InteractiveObject a AIS_Trihedron
          // pour lui appliquer la methode SetSize()
          Handle(AIS_Trihedron) aTrihedron = Handle(AIS_Trihedron)::DownCast (aShape);

          // C'est bien un triedre,on chage sa valeur!
          aTrihedron->SetSize(value);

          // On donne la couleur au Trihedron
          if(hascol)   aTrihedron->SetColor(col);
          else         aTrihedron->UnsetColor();


          // The trihedron hasn't be errased from the map
          // so you just have to redisplay it
          TheAISContext() ->Redisplay(aTrihedron,Standard_False);

        }

      }

      it.Next();
    }

    TheAISContext() ->UpdateCurrentViewer();
  }

  //===============================================================
  // Il n'y a pas d'arguments et aucuns objets selectionne Rien A Faire!
  //===============================================================



  //===============================================================
  // Il y a un nom de triedre passe en argument
  //===============================================================
  if (ThereIsName) {
    TCollection_AsciiString name=argv[1];

    // on verifie que ce nom correspond bien a une shape
    Standard_Boolean IsBound= GetMapOfAIS().IsBound2(name);

    if (IsBound) {

      // on recupere la shape dans la map des objets displayes
      Handle(AIS_InteractiveObject) aShape =
        Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(name));

      // On verifie que l'AIS InteraciveObject est bien
      // un AIS_Trihedron
      if (!aShape.IsNull() &&
        aShape->Type()==AIS_KOI_Datum && aShape->Signature()==3)
      {

        if (aShape->HasColor()) {
          hascol=Standard_True;

          // On recupere la couleur de aShape
          col=aShape->Color();}

        else hascol=Standard_False;

        // On downcast aShape de AIS_InteractiveObject a AIS_Trihedron
        // pour lui appliquer la methode SetSize()
        Handle(AIS_Trihedron) aTrihedron = Handle(AIS_Trihedron)::DownCast (aShape);

        // C'est bien un triedre,on chage sa valeur
        aTrihedron->SetSize(value);

        // On donne la couleur au Trihedron
        if(hascol)   aTrihedron->SetColor(col);
        else         aTrihedron->UnsetColor();

        // The trihedron hasn't be errased from the map
        // so you just have to redisplay it
        TheAISContext() ->Redisplay(aTrihedron,Standard_False);

        TheAISContext() ->UpdateCurrentViewer();
      }
    }
  }
  return 0;
}


//==============================================================================

//==============================================================================
//function : VPlaneTrihedron
//purpose  : Create a plane from a trihedron selection. If no arguments are set, the default
//Draw arg : vplanetri  name
//==============================================================================
#include <AIS_Plane.hxx>



static int VPlaneTrihedron (Draw_Interpretor& di, Standard_Integer argc, const char** argv)

{
  // Verification des arguments
  if ( argc!=2) {di<<argv[0]<<" error\n"; return 1;}

  // Declarations
  Standard_Integer myCurrentIndex;
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();

  // On recupere tous les trihedrons de la GetMapOfAIS()
  // et on active le mode de selection par face.
  // =================================================

  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext(Standard_False);
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();

  ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName
    it (GetMapOfAIS());
  while(it.More()){
    Handle(AIS_InteractiveObject) ShapeA =
      Handle(AIS_InteractiveObject)::DownCast(it.Key1());
    // On verifie que c'est bien un trihedron
    if (!ShapeA.IsNull() &&
      ShapeA->Type()==AIS_KOI_Datum  && ShapeA->Signature()==3  ) {
        // on le downcast
        Handle(AIS_Trihedron) TrihedronA =(Handle(AIS_Trihedron)::DownCast (ShapeA));
        // on le charge dans le contexte et on active le mode Plane.
        TheAISContext()->Load(TrihedronA,0,Standard_False);
        TheAISContext()->Activate(TrihedronA,3);
      }
      it.Next();
  }

  di<<" Select a plane.\n";
  // Boucle d'attente waitpick.
  Standard_Integer argccc = 5;
  const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argvvv = (const char **) bufff;
  while (ViewerMainLoop( argccc, argvvv) ) { }
  // fin de la boucle

  Handle(AIS_InteractiveObject) theIOB;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    theIOB = TheAISContext()->SelectedInteractive();
  }
  // on le downcast
  Handle(AIS_Plane) PlaneB =(Handle(AIS_Plane)::DownCast (theIOB));

  // Fermeture du contexte local.
  TheAISContext()->CloseLocalContext(myCurrentIndex);

  // on le display & bind
  TheAISContext()->Display(PlaneB );
  GetMapOfAIS().Bind ( PlaneB ,argv[1]);

  return 0;
}



//==============================================================================
// Fonction        First click      2de click
//
// vaxis           vertex           vertex
//                 edge             None
// vaxispara       edge             vertex
// vaxisortho      edge             Vertex
// vaxisinter      Face             Face
//==============================================================================

//==============================================================================
//function : VAxisBuilder
//purpose  :
//Draw arg : vaxis AxisName Xa Ya Za Xb Yb Zb
//==============================================================================
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <Geom_Line.hxx>

static int VAxisBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  // Declarations
  Standard_Boolean HasArg;
  TCollection_AsciiString name;
  Standard_Integer MyCurrentIndex;

  // Verification
  if (argc<2 || argc>8 ) {di<<" Syntaxe error\n";return 1;}
  if (argc==8) HasArg=Standard_True;
  else HasArg=Standard_False;

  name=argv[1];
  // Fermeture des contextes
  TheAISContext()->CloseAllContexts();

  // Cas ou il y a des arguments
  // Purpose: Teste le constructeur AIS_Axis::AIS_Axis(x: Line from Geom)
  if (HasArg) {
    Standard_Real coord[6];
    for(Standard_Integer i=0;i<=5;i++){
      coord[i]=Draw::Atof(argv[2+i]);
    }
    gp_Pnt p1(coord[0],coord[1],coord[2]), p2(coord[3],coord[4],coord[5]) ;

    gp_Vec myVect (p1,p2);
    Handle(Geom_Line) myLine=new Geom_Line (p1 ,myVect );
    Handle(AIS_Axis) TheAxis=new AIS_Axis (myLine );
    GetMapOfAIS().Bind (TheAxis,name);
    TheAISContext()->Display(TheAxis);
  }

  // Pas d'arguments
  else {
    // fonction vaxis
    // Purpose: Teste le constructeur AIS_Axis::AIS_Axis (x:Axis1Placement from Geom)
    if ( !strcasecmp(argv[0], "vaxis")) {
      TheAISContext()->OpenLocalContext();
      MyCurrentIndex=TheAISContext()->IndexOfCurrentLocal();

      // Active le mode edge et le mode vertex
      TheAISContext()->ActivateStandardMode(AIS_Shape::SelectionType(1) );
      TheAISContext()->ActivateStandardMode(AIS_Shape::SelectionType(2) );
      di<<" Select an edge or a vertex.\n";

      // Boucle d'attente waitpick.
      Standard_Integer argcc = 5;
      const char *buff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
      const char **argvv = (const char **) buff;
      while (ViewerMainLoop( argcc, argvv) ) { }
      // fin de la boucle

      // recuperation de la shape.
      TopoDS_Shape ShapeA;
      for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
        ShapeA = TheAISContext()->SelectedShape();
      }
      // recuperation de l'AIS_InteractiveObject
      //Handle(AIS_InteractiveObject) myAISio=TheAISContext()->Current();
      // down cast en AIS_Point si sig et type
      // AIS_Point -> Geom_Pnt ....

      if (ShapeA.ShapeType()==TopAbs_VERTEX) {
        // on desactive le mode edge
        TheAISContext()->DeactivateStandardMode(AIS_Shape::SelectionType(2) );
        di<<" Select a different vertex.\n";

        TopoDS_Shape ShapeB;
        do {
          // Boucle d'attente waitpick.
          Standard_Integer argccc = 5;
          const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
          const char **argvvv = (const char **) bufff;
          while (ViewerMainLoop( argccc, argvvv) ) { }
          // fin de la boucle
          for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
            ShapeB = TheAISContext()->SelectedShape();
          }


        } while(ShapeB.IsSame(ShapeA) );

        // Fermeture du context local
        TheAISContext()->CloseLocalContext(MyCurrentIndex);

        // Construction de l'axe
        gp_Pnt   A=BRep_Tool::Pnt(TopoDS::Vertex(ShapeA)  );
        gp_Pnt   B=BRep_Tool::Pnt(TopoDS::Vertex(ShapeB)  );
        gp_Vec   V (A,B);
        gp_Dir   D (V);
        Handle(Geom_Axis1Placement) OrigineAndVect=new Geom_Axis1Placement (A,D);
        Handle(AIS_Axis) TheAxis=new AIS_Axis (OrigineAndVect);
        GetMapOfAIS().Bind (TheAxis,name);
        TheAISContext()->Display(TheAxis);
      }
      else {
        // Un unique edge (ShapeA) a ete picke
        // Fermeture du context local
        TheAISContext()->CloseLocalContext(MyCurrentIndex);
        // Constuction de l'axe
        TopoDS_Edge    ed =TopoDS::Edge(ShapeA);
        TopoDS_Vertex  Va,Vb;
        TopExp::Vertices(ed,Va,Vb );
        gp_Pnt A=BRep_Tool::Pnt(Va);
        gp_Pnt B=BRep_Tool::Pnt(Vb);
        gp_Vec  V (A,B);
        gp_Dir   D (V);
        Handle(Geom_Axis1Placement) OrigineAndVect=new Geom_Axis1Placement (A,D);
        Handle(AIS_Axis) TheAxis=new AIS_Axis (OrigineAndVect);
        GetMapOfAIS().Bind (TheAxis,name);
        TheAISContext()->Display(TheAxis);
      }

    }

    // Fonction axispara
    // Purpose: Teste le constructeur AIS_Axis::AIS_Axis(x: Axis2Placement from Geom, y: TypeOfAxis from AIS)
    else if ( !strcasecmp(argv[0], "vaxispara")) {

      TheAISContext()->OpenLocalContext();
      MyCurrentIndex=TheAISContext()->IndexOfCurrentLocal();

      // Active le mode edge
      TheAISContext()->ActivateStandardMode(AIS_Shape::SelectionType(2) );
      di<<" Select an edge.\n";

      // Boucle d'attente waitpick.
      Standard_Integer argcc = 5;
      const char *buff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
      const char **argvv = (const char **) buff;
      while (ViewerMainLoop( argcc, argvv) ) { }
      // fin de la boucle

      TopoDS_Shape ShapeA;
      for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
        ShapeA = TheAISContext()->SelectedShape();
      }
      // Active le mode vertex et deactive edges
      TheAISContext()->DeactivateStandardMode(AIS_Shape::SelectionType(2) );
      TheAISContext()->ActivateStandardMode(AIS_Shape::SelectionType(1) );
      di<<" Select a vertex.\n";

      // Boucle d'attente waitpick.
      Standard_Integer argccc = 5;
      const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
      const char **argvvv = (const char **) bufff;
      while (ViewerMainLoop( argccc, argvvv) ) { }
      // fin de la boucle

      // On peut choisir un pnt sur l'edge
      TopoDS_Shape ShapeB;
      for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
        ShapeB = TheAISContext()->SelectedShape();
      }
      // Fermeture du context local
      TheAISContext()->CloseLocalContext(MyCurrentIndex);

      // Construction de l'axe
      TopoDS_Edge    ed=TopoDS::Edge(ShapeA) ;
      gp_Pnt B=BRep_Tool::Pnt(TopoDS::Vertex(ShapeB) );
      TopoDS_Vertex  Va,Vc;
      TopExp::Vertices(ed,Va,Vc );
      gp_Pnt A=BRep_Tool::Pnt(Va);
      gp_Pnt C=BRep_Tool::Pnt(Vc);
      gp_Vec  V (A,C);
      gp_Dir   D (V);
      Handle(Geom_Axis1Placement) OrigineAndVect=new Geom_Axis1Placement (B,D);
      Handle(AIS_Axis) TheAxis=new AIS_Axis (OrigineAndVect);
      GetMapOfAIS().Bind (TheAxis,name);
      TheAISContext()->Display(TheAxis);

    }

    // Fonction axisortho
    else  {
      TheAISContext()->OpenLocalContext();
      MyCurrentIndex=TheAISContext()->IndexOfCurrentLocal();

      // Active le mode edge
      TheAISContext()->ActivateStandardMode(AIS_Shape::SelectionType(2) );
      di<<" Select an edge.\n";

      // Boucle d'attente waitpick.
      Standard_Integer argcc = 5;
      const char *buff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
      const char **argvv = (const char **) buff;
      while (ViewerMainLoop( argcc, argvv) ) { }
      // fin de la boucle

      TopoDS_Shape ShapeA;
      for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
        ShapeA = TheAISContext()->SelectedShape();
      }
      // Active le mode vertex et deactive edges
      TheAISContext()->DeactivateStandardMode(AIS_Shape::SelectionType(2) );
      TheAISContext()->ActivateStandardMode(AIS_Shape::SelectionType(1) );
      di<<" Slect a vertex.\n";

      // Boucle d'attente waitpick.
      Standard_Integer argccc = 5;
      const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
      const char **argvvv = (const char **) bufff;
      while (ViewerMainLoop( argccc, argvvv) ) { }
      // fin de la boucle

      // On peut choisir un pnt sur l'edge
      TopoDS_Shape ShapeB;
      for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
        ShapeB = TheAISContext()->SelectedShape();
      }
      // Fermeture du context local
      TheAISContext()->CloseLocalContext(MyCurrentIndex);

      // Construction de l'axe
      TopoDS_Edge    ed=TopoDS::Edge(ShapeA) ;
      gp_Pnt B=BRep_Tool::Pnt(TopoDS::Vertex(ShapeB) );
      TopoDS_Vertex  Va,Vc;
      TopExp::Vertices(ed,Va,Vc );
      gp_Pnt A=BRep_Tool::Pnt(Va);
      gp_Pnt C=BRep_Tool::Pnt(Vc);
      gp_Pnt E(A.Y()+A.Z()-C.Y()-C.Z()  ,C.X()-A.X() ,C.X()-A.X() );
      gp_Vec  V (A,E);
      gp_Dir   D (V);
      Handle(Geom_Axis1Placement) OrigineAndVect=new Geom_Axis1Placement (B,D);
      Handle(AIS_Axis) TheAxis=new AIS_Axis (OrigineAndVect);
      GetMapOfAIS().Bind (TheAxis,name);
      TheAISContext()->Display(TheAxis);

    }

  }
  return 0;
}


//==============================================================================
// Fonction        First click      Result
//
// vpoint          vertex           AIS_Point=Vertex
//                 edge             AIS_Point=Middle of the edge
//==============================================================================

//==============================================================================
//function : VPointBuilder
//purpose  : Build an AIS_Point from coordinates or with a selected vertex or edge
//Draw arg : vpoint PoinName [Xa] [Ya] [Za]
//==============================================================================
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <AIS_Point.hxx>
#include <Geom_CartesianPoint.hxx>

static int VPointBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  // Declarations
  Standard_Boolean HasArg;
  TCollection_AsciiString name;
  Standard_Integer myCurrentIndex;

  // Verification
  if (argc<2 || argc>5 ) {di<<" Syntaxe error\n";return 1;}
  if (argc==5) HasArg=Standard_True;
  else HasArg=Standard_False;

  name=argv[1];
  // Fermeture des contextes
  TheAISContext()->CloseAllContexts();

  // Il y a des arguments: teste l'unique constructeur AIS_Pnt::AIS_Pnt(Point from Geom)
  if (HasArg) {
    Standard_Real thecoord[3];
    for(Standard_Integer i=0;i<=2;i++)
      thecoord[i]=Draw::Atof(argv[2+i]);
    Handle(Geom_CartesianPoint )  myGeomPoint= new Geom_CartesianPoint (thecoord[0],thecoord[1],thecoord[2]);
    Handle(AIS_Point)  myAISPoint=new AIS_Point(myGeomPoint );
    GetMapOfAIS().Bind (myAISPoint,name);
    TheAISContext()->Display(myAISPoint);
  }

  // Il n'a pas d'arguments
  else {
    TheAISContext()->OpenLocalContext();
    myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();

    // Active le mode Vertex et Edges
    TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(1) );
    TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
    di<<" Select a vertex or an edge(build the middle)\n";

    // Boucle d'attente waitpick.
    Standard_Integer argcc = 5;
    const char *buff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
    const char **argvv = (const char **) buff;
    while (ViewerMainLoop( argcc, argvv) ) { }
    // fin de la boucle

    TopoDS_Shape ShapeA;
    for (TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
      ShapeA= TheAISContext()->SelectedShape();
    }

    if (ShapeA.ShapeType()==TopAbs_VERTEX ) {
      // Un vertex a ete selectionne
      // Fermeture du context local
      TheAISContext()->CloseLocalContext(myCurrentIndex);

      // Construction du point
      gp_Pnt A=BRep_Tool::Pnt(TopoDS::Vertex(ShapeA ) );
      Handle(Geom_CartesianPoint) myGeomPoint= new Geom_CartesianPoint (A );
      Handle(AIS_Point)  myAISPoint = new AIS_Point  (myGeomPoint );
      GetMapOfAIS().Bind(myAISPoint,name);
      TheAISContext()->Display(myAISPoint);
    }
    else {
      // Un Edge a ete selectionne
      // Fermeture du context local
      TheAISContext()->CloseLocalContext(myCurrentIndex);

      // Construction du point milieu de l'edge
      TopoDS_Edge myEdge=TopoDS::Edge(ShapeA);
      TopoDS_Vertex myVertexA,myVertexB;
      TopExp::Vertices (myEdge ,myVertexA ,myVertexB );
      gp_Pnt A=BRep_Tool::Pnt(myVertexA );
      gp_Pnt B=BRep_Tool::Pnt(myVertexB );
      // M est le milieu de [AB]
      Handle(Geom_CartesianPoint) myGeomPointM= new Geom_CartesianPoint ( (A.X()+B.X())/2  , (A.Y()+B.Y())/2  , (A.Z()+B.Z())/2  );
      Handle(AIS_Point)  myAISPointM = new AIS_Point  (myGeomPointM );
      GetMapOfAIS().Bind(myAISPointM,name);
      TheAISContext()->Display(myAISPointM);
    }

  }
  return 0;

}

//==============================================================================
// Function        1st click   2de click  3de click
// vplane          Vertex      Vertex     Vertex
//                 Vertex      Edge
//                 Edge        Vertex
//                 Face
// vplanepara      Face        Vertex
//                 Vertex      Face
// vplaneortho     Face        Edge
//                 Edge        Face
//==============================================================================

//==============================================================================
//function : VPlaneBuilder
//purpose  : Build an AIS_Plane from selected entities or Named AIS components
//Draw arg : vplane PlaneName [AxisName]  [PointName] [TypeOfSensitivity]
//                            [PointName] [PointName] [PointName] [TypeOfSensitivity]
//                            [PlaneName] [PointName] [TypeOfSensitivity]
//==============================================================================

static Standard_Integer VPlaneBuilder (Draw_Interpretor& /*di*/,
                                       Standard_Integer argc,
                                       const char** argv)
{
  // Declarations
  Standard_Boolean hasArg;
  TCollection_AsciiString aName;
  Standard_Integer aCurrentIndex;

  // Verification
  if (argc<2 || argc>6 )
  {
    std::cout<<" Syntax error\n";
    return 1;
  }
  if (argc == 6 || argc==5 || argc==4)
    hasArg=Standard_True;
  else 
    hasArg=Standard_False;

  aName=argv[1];
  // Close all contexts
  TheAISContext()->CloseAllContexts();

  // There are some arguments
  if (hasArg)
  {
    if (!GetMapOfAIS().IsBound2(argv[2] ))
    {
      std::cout<<"vplane: error 1st name doesn't exist in the GetMapOfAIS()\n";
      return 1;
    }
    // Get shape from map
    Handle(AIS_InteractiveObject) aShapeA =
      Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[2] ));

    // The first argument is an AIS_Point
    if (!aShapeA.IsNull() &&
        aShapeA->Type()==AIS_KOI_Datum &&
        aShapeA->Signature()==1)
    {
        // The second argument must also be an AIS_Point
        if (argc<5 || !GetMapOfAIS().IsBound2(argv[3]))
        {
          std::cout<<"vplane: error 2nd name doesn't exist in the GetMapOfAIS()\n";
          return 1;
        }
        // Get shape from map
        Handle(AIS_InteractiveObject) aShapeB =
          Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[3]));
        // If B is not an AIS_Point
        if (aShapeB.IsNull() ||
          (!(aShapeB->Type()==AIS_KOI_Datum && aShapeB->Signature()==1)))
        {
          std::cout<<"vplane: error 2nd object is expected to be an AIS_Point.\n";
          return 1;
        }
        // The third object is an AIS_Point
        if (!GetMapOfAIS().IsBound2(argv[4]) ) 
        {
          std::cout<<"vplane: error 3d name doesn't exist in the GetMapOfAIS().\n";
          return 1; 
        }
        // Get shape from map
        Handle(AIS_InteractiveObject) aShapeC =
          Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[4]));
        // If C is not an AIS_Point
        if (aShapeC.IsNull() ||
          (!(aShapeC->Type()==AIS_KOI_Datum && aShapeC->Signature()==1)))
        {
          std::cout<<"vplane: error 3d object is expected to be an AIS_Point.\n";
          return 1;
        }

        // Treatment of objects A, B, C
        // Downcast an AIS_IO to AIS_Point
        Handle(AIS_Point) anAISPointA = Handle(AIS_Point)::DownCast( aShapeA);
        Handle(AIS_Point) anAISPointB = Handle(AIS_Point)::DownCast( aShapeB);
        Handle(AIS_Point) anAISPointC = Handle(AIS_Point)::DownCast( aShapeC);

        Handle(Geom_CartesianPoint ) aCartPointA = 
          Handle(Geom_CartesianPoint)::DownCast( anAISPointA->Component());

        Handle(Geom_CartesianPoint ) aCartPointB = 
          Handle(Geom_CartesianPoint)::DownCast( anAISPointB->Component());

        Handle(Geom_CartesianPoint ) aCartPointC = 
          Handle(Geom_CartesianPoint)::DownCast( anAISPointC->Component());

        // Verification that the three points are different
        if(Abs(aCartPointB->X()-aCartPointA->X())<=Precision::Confusion() &&
           Abs(aCartPointB->Y()-aCartPointA->Y())<=Precision::Confusion() &&
           Abs(aCartPointB->Z()-aCartPointA->Z())<=Precision::Confusion())
        {
          // B=A
          std::cout<<"vplane error: same points\n";return 1;
        }
        if(Abs(aCartPointC->X()-aCartPointA->X())<=Precision::Confusion() &&
           Abs(aCartPointC->Y()-aCartPointA->Y())<=Precision::Confusion() &&
           Abs(aCartPointC->Z()-aCartPointA->Z())<=Precision::Confusion())
        {
          // C=A
          std::cout<<"vplane error: same points\n";return 1;
        }
        if(Abs(aCartPointC->X()-aCartPointB->X())<=Precision::Confusion() &&
           Abs(aCartPointC->Y()-aCartPointB->Y())<=Precision::Confusion() &&
           Abs(aCartPointC->Z()-aCartPointB->Z())<=Precision::Confusion())
        {
          // C=B
          std::cout<<"vplane error: same points\n";return 1;
        }

        gp_Pnt A = aCartPointA->Pnt();
        gp_Pnt B = aCartPointB->Pnt();
        gp_Pnt C = aCartPointC->Pnt();

        // Construction of AIS_Plane
        GC_MakePlane MkPlane (A,B,C);
        Handle(Geom_Plane) aGeomPlane = MkPlane.Value();
        Handle(AIS_Plane)  anAISPlane = new AIS_Plane(aGeomPlane );
        GetMapOfAIS().Bind (anAISPlane,aName );
        if (argc == 6)
        {
          Standard_Integer aType = Draw::Atoi (argv[5]);
          if (aType != 0 && aType != 1)
          {
            std::cout << "vplane error: wrong type of sensitivity!\n"
                      << "Should be one of the following values:\n"
                      << "0 - Interior\n"
                      << "1 - Boundary"
                      << std::endl;
            return 1;
          }
          else
          {
            anAISPlane->SetTypeOfSensitivity (Select3D_TypeOfSensitivity (aType));
          }
        }
        TheAISContext()->Display(anAISPlane);
      }

      // The first argument is an AIS_Axis
      // Creation of a plane orthogonal to the axis through a point
    else if (aShapeA->Type()==AIS_KOI_Datum && aShapeA->Signature()==2 ) {
      // The second argument should be an AIS_Point
      if (argc!=4 || !GetMapOfAIS().IsBound2(argv[3] ) )
      {
        std::cout<<"vplane: error 2d name doesn't exist in the GetMapOfAIS()\n";
        return 1;
      }
      // Get shape from map
      Handle(AIS_InteractiveObject) aShapeB =
        Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[3]));
      // If B is not an AIS_Point
      if (aShapeB.IsNull() ||
        (!(aShapeB->Type()==AIS_KOI_Datum && aShapeB->Signature()==1)))
      {
        std::cout<<"vplane: error 2d object is expected to be an AIS_Point\n";
        return 1;
      }

      // Treatment of objects A and B
      Handle(AIS_Axis) anAISAxisA = Handle(AIS_Axis)::DownCast(aShapeA);
      Handle(AIS_Point) anAISPointB = Handle(AIS_Point)::DownCast(aShapeB);

      Handle(Geom_Line ) aGeomLineA = anAISAxisA ->Component();
      Handle(Geom_Point) aGeomPointB = anAISPointB->Component()  ;

      gp_Ax1 anAxis = aGeomLineA->Position();
      Handle(Geom_CartesianPoint) aCartPointB = 
        Handle(Geom_CartesianPoint)::DownCast(aGeomPointB);

      gp_Dir D =anAxis.Direction();
      gp_Pnt B = aCartPointB->Pnt();

      // Construction of AIS_Plane
      Handle(Geom_Plane) aGeomPlane = new Geom_Plane(B,D);
      Handle(AIS_Plane) anAISPlane = new AIS_Plane(aGeomPlane,B );
      GetMapOfAIS().Bind (anAISPlane,aName );
      if (argc == 5)
      {
        Standard_Integer aType = Draw::Atoi (argv[4]);
        if (aType != 0 && aType != 1)
        {
          std::cout << "vplane error: wrong type of sensitivity!\n"
                    << "Should be one of the following values:\n"
                    << "0 - Interior\n"
                    << "1 - Boundary"
                    << std::endl;
          return 1;
        }
        else
        {
          anAISPlane->SetTypeOfSensitivity (Select3D_TypeOfSensitivity (aType));
        }
      }
      TheAISContext()->Display(anAISPlane);

    }
    // The first argumnet is an AIS_Plane
    // Creation of a plane parallel to the plane passing through the point
    else if (aShapeA->Type()==AIS_KOI_Datum && aShapeA->Signature()==7)
    {
      // The second argument should be an AIS_Point
      if (argc!=4 || !GetMapOfAIS().IsBound2(argv[3]))
      {
        std::cout<<"vplane: error 2d name doesn't exist in the GetMapOfAIS()\n";
        return 1;
      }
      // Get shape from map
      Handle(AIS_InteractiveObject) aShapeB =
        Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[3]));
      // B should be an AIS_Point
      if (aShapeB.IsNull() ||
         (!(aShapeB->Type()==AIS_KOI_Datum && aShapeB->Signature()==1)))
      {
        std::cout<<"vplane: error 2d object is expected to be an AIS_Point\n";
        return 1;
      }

      // Treatment of objects A and B
      Handle(AIS_Plane) anAISPlaneA = Handle(AIS_Plane)::DownCast(aShapeA);
      Handle(AIS_Point) anAISPointB = Handle(AIS_Point)::DownCast(aShapeB);

      Handle(Geom_Plane) aNewGeomPlane= anAISPlaneA->Component();
      Handle(Geom_Point) aGeomPointB = anAISPointB->Component();

      Handle(Geom_CartesianPoint) aCartPointB = 
        Handle(Geom_CartesianPoint)::DownCast(aGeomPointB);
      gp_Pnt B= aCartPointB->Pnt();

      // Construction of an AIS_Plane
      Handle(AIS_Plane) anAISPlane = new AIS_Plane(aNewGeomPlane, B);
      GetMapOfAIS().Bind (anAISPlane, aName);
      if (argc == 5)
      {
        Standard_Integer aType = Draw::Atoi (argv[4]);
        if (aType != 0 && aType != 1)
        {
          std::cout << "vplane error: wrong type of sensitivity!\n"
                    << "Should be one of the following values:\n"
                    << "0 - Interior\n"
                    << "1 - Boundary"
                    << std::endl;
          return 1;
        }
        else
        {
          anAISPlane->SetTypeOfSensitivity (Select3D_TypeOfSensitivity (aType));
        }
      }
      TheAISContext()->Display(anAISPlane);
    }
    // Error
    else
    {
      std::cout<<"vplane: error 1st object is not an AIS\n";
      return 1;
    }
  }
  // There are no arguments
  else 
  {
    // Function vplane
    // Test the constructor AIS_Plane::AIS_Plane(Geom_Plane, Standard_Boolean )
    if (!strcasecmp(argv[0], "vplane"))
    {
      TheAISContext()->OpenLocalContext();
      aCurrentIndex=TheAISContext()->IndexOfCurrentLocal();

      // Active modes Vertex, Edge and Face
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(1));
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2));
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4));
      std::cout<<"Select a vertex, a face or an edge\n";

      // Wait for picking
      Standard_Integer argcc = 5;
      const char *buff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
      const char **argvv = (const char **) buff;
      while (ViewerMainLoop( argcc, argvv) ) { }
      // end of the loop

      TopoDS_Shape aShapeA;
      for (TheAISContext()->InitSelected();
           TheAISContext()->MoreSelected();
           TheAISContext()->NextSelected())
      {
        aShapeA = TheAISContext()->SelectedShape();
      }

      // aShapeA is a Vertex
      if (aShapeA.ShapeType()==TopAbs_VERTEX )
      {
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4));
        std::cout<<" Select an edge or a different vertex\n";

        // Wait for picking
        Standard_Integer argccc = 5;
        const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvv = (const char **) bufff;
        while (ViewerMainLoop( argccc, argvvv) ) { }
        // end of the loop

        TopoDS_Shape aShapeB;
        for (TheAISContext()->InitSelected();
          TheAISContext()->MoreSelected();
          TheAISContext()->NextSelected())
        {
          aShapeB = TheAISContext()->SelectedShape();
        }
        // aShapeB is a Vertex
        if (aShapeB.ShapeType()==TopAbs_VERTEX)
        {
          // A and B are the same
          if (aShapeB.IsSame(aShapeA))
          {
            std::cout<<" vplane: error, same points selected\n";
            return 1;
          }
          TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(2));
          std::cout<<" Select a different vertex\n";

          // Wait for picking
          Standard_Integer argcccc = 5;
          const char *buffff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
          const char **argvvvv = (const char **) buffff;
          while (ViewerMainLoop( argcccc, argvvvv) ) { }
          // end of the loop

          TopoDS_Shape aShapeC;
          for (TheAISContext()->InitSelected();
               TheAISContext()->MoreSelected();
               TheAISContext()->NextSelected())
          {
            aShapeC = TheAISContext()->SelectedShape();
          }
          // aShapeC is the same as A or B
          if (aShapeC.IsSame(aShapeA)||aShapeC.IsSame(aShapeB))
          {
            std::cout<<" vplane: error, same points selected\n";
            return 1;
          }

          // Close the local context
          TheAISContext()->CloseLocalContext(aCurrentIndex);

          // Construction of plane
          gp_Pnt A = BRep_Tool::Pnt(TopoDS::Vertex(aShapeA));
          gp_Pnt B = BRep_Tool::Pnt(TopoDS::Vertex(aShapeB));
          gp_Pnt C = BRep_Tool::Pnt(TopoDS::Vertex(aShapeC));
          GC_MakePlane MkPlane(A, B, C);
          Handle(Geom_Plane) aGeomPlane = MkPlane.Value();
          Handle(AIS_Plane) anAISPlane = new AIS_Plane (aGeomPlane);
          GetMapOfAIS().Bind (anAISPlane, aName);
          TheAISContext()->Display(anAISPlane);
        }
        // ShapeB is an edge
        else
        {
          // Verify that the vertex is not on the edge ShapeB
          TopoDS_Edge anEdgeB = TopoDS::Edge(aShapeB);
          TopoDS_Vertex aVertA = TopoDS::Vertex(aShapeA);

          BRepExtrema_ExtPC OrthoProj(aVertA, anEdgeB);
          if (OrthoProj.SquareDistance(1)<Precision::Approximation())
          {
            // The vertex is on the edge
            std::cout<<" vplane: error point is on the edge\n";
            return 1;
          }
          else
          {
            // Close the local context
            TheAISContext()->CloseLocalContext(aCurrentIndex);
            // Construction of plane
            gp_Pnt A = BRep_Tool::Pnt(aVertA);
            TopoDS_Vertex aVBa, aVBb;
            TopExp::Vertices(anEdgeB ,aVBa ,aVBb);
            gp_Pnt aBa = BRep_Tool::Pnt(aVBa);
            gp_Pnt aBb = BRep_Tool::Pnt(aVBb);
            GC_MakePlane MkPlane (A, aBa, aBb);
            Handle(Geom_Plane) aGeomPlane = MkPlane.Value();
            Handle(AIS_Plane) anAISPlane = new AIS_Plane (aGeomPlane);
            GetMapOfAIS().Bind (anAISPlane, aName);
            TheAISContext()->Display(anAISPlane);
          }
        }
      }
      // aShapeA is an edge
      else if (aShapeA.ShapeType()==TopAbs_EDGE)
      {
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4));
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(2));
        std::cout<<" Select a vertex that don't belong to the edge\n";

        // Wait for picking
        Standard_Integer argccc = 5;
        const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvv = (const char **) bufff;
        while (ViewerMainLoop( argccc, argvvv) ) { }
        // end of the loop

        TopoDS_Shape aShapeB;
        for (TheAISContext()->InitSelected();
             TheAISContext()->MoreSelected();
             TheAISContext()->NextSelected())
        {
          aShapeB = TheAISContext()->SelectedShape();
        }
        // aShapeB should be a Vertex
        // Check that the vertex aShapeB is not on the edge
        TopoDS_Edge anEdgeA = TopoDS::Edge(aShapeA);
        TopoDS_Vertex aVertB = TopoDS::Vertex(aShapeB);

        BRepExtrema_ExtPC OrthoProj (aVertB, anEdgeA);
        if (OrthoProj.SquareDistance(1)<Precision::Approximation())
        {
          // The vertex is on the edge
          std::cout<<" vplane: error point is on the edge\n";
          return 1;
        }
        else
        {
          // Close the local context
          TheAISContext()->CloseLocalContext(aCurrentIndex);
          // Construction of plane
          gp_Pnt B = BRep_Tool::Pnt(aVertB);
          TopoDS_Vertex aVAa, aVAb;
          TopExp::Vertices(anEdgeA, aVAa, aVAb);
          gp_Pnt Aa = BRep_Tool::Pnt(aVAa);
          gp_Pnt Ab = BRep_Tool::Pnt(aVAb);
          GC_MakePlane MkPlane (B,Aa,Ab);
          Handle(Geom_Plane) aGeomPlane = MkPlane.Value();
          Handle(AIS_Plane) anAISPlane = new AIS_Plane (aGeomPlane);
          GetMapOfAIS().Bind (anAISPlane ,aName);
          TheAISContext()->Display(anAISPlane);
        }
      }
      // aShapeA is a Face
      else
      {
        // Close the local context: nothing to select
        TheAISContext()->CloseLocalContext(aCurrentIndex);
        // Construction of plane
        TopoDS_Face aFace = TopoDS::Face(aShapeA);
        BRepAdaptor_Surface aSurface (aFace, Standard_False);
        if (aSurface.GetType()==GeomAbs_Plane)
        {
          gp_Pln aPlane = aSurface.Plane();
          Handle(Geom_Plane) aGeomPlane = new Geom_Plane(aPlane);
          Handle(AIS_Plane) anAISPlane = new AIS_Plane(aGeomPlane);
          GetMapOfAIS().Bind (anAISPlane, aName);
          TheAISContext()->Display(anAISPlane);
        }
        else
        {
          std::cout<<" vplane: error\n";
          return 1;
        }
      }
    }

    // Function vPlanePara
    // ===================
    // test the constructor AIS_Plane::AIS_Plane(Geom_Plane,gp_Pnt)
    else if (!strcasecmp(argv[0], "vplanepara"))
    {
      TheAISContext()->OpenLocalContext();
      aCurrentIndex = TheAISContext()->IndexOfCurrentLocal();

      // Activate modes Vertex and Face
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(1));
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4));
      std::cout<<" Select a vertex or a face\n";

      // Wait for picking
      Standard_Integer argcc = 5;
      const char *buff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
      const char **argvv = (const char **) buff;
      while (ViewerMainLoop( argcc, argvv) ) { }
      // end of the loop

      TopoDS_Shape aShapeA;
      for (TheAISContext()->InitSelected();
           TheAISContext()->MoreSelected();
           TheAISContext()->NextSelected())
      {
        aShapeA = TheAISContext()->SelectedShape();
      }

      if (aShapeA.ShapeType()==TopAbs_VERTEX )
      {
        // aShapeA is a vertex
        // Deactivate the mode Vertex
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(1));
        std::cout<<" Select a face\n";

        // Wait for picking
        Standard_Integer argccc = 5;
        const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvv = (const char **) bufff;
        while (ViewerMainLoop( argccc, argvvv) ) { }
        // end of the loop

        TopoDS_Shape aShapeB;
        for (TheAISContext()->InitSelected();
             TheAISContext()->MoreSelected();
             TheAISContext()->NextSelected())
        {
          // A vertex ShapeA can be on Face ShapeB
          aShapeB = TheAISContext()->SelectedShape();
        }

        // Close the local context
        TheAISContext()->CloseLocalContext(aCurrentIndex);

        // Construction of plane
        gp_Pnt A = BRep_Tool::Pnt(TopoDS::Vertex(aShapeA));

        TopoDS_Face aFace = TopoDS::Face(aShapeB);
        BRepAdaptor_Surface aSurface (aFace, Standard_False);
        if (aSurface.GetType()==GeomAbs_Plane )
        {
          gp_Pln aPlane = aSurface.Plane();
          // Construct a plane parallel to aGeomPlane through A
          aPlane.SetLocation(A);
          Handle(Geom_Plane) aGeomPlane = new Geom_Plane (aPlane);
          Handle(AIS_Plane) aAISPlane = new AIS_Plane (aGeomPlane, A);
          GetMapOfAIS().Bind (aAISPlane ,aName);
          TheAISContext()->Display(aAISPlane);
        }
        else
        {
          std::cout<<" vplanepara: error\n";
          return 1;
        }
      }
      else
      {
        // ShapeA is a Face
        // Deactive the mode Face
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4));
        std::cout<<" Select a vertex\n";

        // Wait for picking
        Standard_Integer argccc = 5;
        const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvv = (const char **) bufff;
        while (ViewerMainLoop( argccc, argvvv) ) { }
        // end of the loop

        TopoDS_Shape aShapeB;
        for (TheAISContext()->InitSelected();
             TheAISContext()->MoreSelected();
             TheAISContext()->NextSelected())
        {
          // A vertex ShapeB can be on Face ShapeA
          aShapeB = TheAISContext()->SelectedShape();
        }
        // Close the local context
        TheAISContext()->CloseLocalContext(aCurrentIndex);

        // Construction of plane
        gp_Pnt B = BRep_Tool::Pnt(TopoDS::Vertex(aShapeB));

        TopoDS_Face aFace=TopoDS::Face(aShapeA);
        BRepAdaptor_Surface aSurface (aFace, Standard_False);
        if (aSurface.GetType()==GeomAbs_Plane )
        {
          gp_Pln aPlane = aSurface.Plane();
          aPlane.SetLocation(B);
          Handle(Geom_Plane) aGeomPlane = new Geom_Plane (aPlane);
          // Construct a plane parallel to aGeomPlane through B
          Handle(AIS_Plane) anAISPlane = new AIS_Plane (aGeomPlane, B);
          GetMapOfAIS().Bind (anAISPlane, aName);
          TheAISContext()->Display(anAISPlane);
        }
        else
        {
          std::cout<<" vplanepara: error\n";return 1;
        }
      }
    }

    // Function vplaneortho
    // ====================
    // test the constructor AIS_Plane::AIS_Plane(Geom_Plane,gp_Pnt,gp_Pnt,gp_Pnt)
    else
    {
      TheAISContext()->OpenLocalContext();
      aCurrentIndex = TheAISContext()->IndexOfCurrentLocal();

      // Activate the modes Edge and Face
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2));
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4));
      std::cout<<" Select a face and an edge coplanar\n";

      // Wait for picking
      Standard_Integer argcc = 5;
      const char *buff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
      const char **argvv = (const char **) buff;
      while (ViewerMainLoop( argcc, argvv) ) { }
      // end of the loop

      TopoDS_Shape aShapeA;
      for (TheAISContext()->InitSelected();
           TheAISContext()->MoreSelected();
           TheAISContext()->NextSelected())
      {
        aShapeA = TheAISContext()->SelectedShape();
      }

      if (aShapeA.ShapeType()==TopAbs_EDGE )
      {
        // ShapeA is an edge, deactivate the mode Edge...
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(2));
        std::cout<<" Select a face\n";

        // Wait for picking
        Standard_Integer argccc = 5;
        const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvv = (const char **) bufff;
        while (ViewerMainLoop( argccc, argvvv) ) { }
        // end of the loop

        TopoDS_Shape aShapeB;
        for (TheAISContext()->InitSelected();
             TheAISContext()->MoreSelected();
             TheAISContext()->NextSelected())
        {
          // Edge ShapeA can be on Face ShapeB
          aShapeB = TheAISContext()->SelectedShape();
        }

        // Close the local context
        TheAISContext()->CloseLocalContext(aCurrentIndex);

        // Construction of plane
        TopoDS_Edge anEdgeA = TopoDS::Edge(aShapeA);
        TopoDS_Vertex aVAa, aVAb;
        TopExp::Vertices(anEdgeA, aVAa, aVAb);
        gp_Pnt Aa = BRep_Tool::Pnt(aVAa);
        gp_Pnt Ab = BRep_Tool::Pnt(aVAb);
        gp_Vec ab (Aa,Ab);

        gp_Dir Dab (ab);
        // Creation of rotation axis
        gp_Ax1 aRotAxis (Aa,Dab);

        TopoDS_Face aFace = TopoDS::Face(aShapeB);
        // The edge must be parallel to the face
        BRepExtrema_ExtPF aHeightA (aVAa, aFace);
        BRepExtrema_ExtPF aHeightB (aVAb, aFace);
        // Compare to heights
        if (fabs(sqrt(aHeightA.SquareDistance(1)) - sqrt(aHeightB.SquareDistance(1)))
            >Precision::Confusion())
        {
          // the edge is not parallel to the face
          std::cout<<" vplaneortho error: the edge is not parallel to the face\n";
          return 1;
        }
        // the edge is OK
        BRepAdaptor_Surface aSurface (aFace, Standard_False);
        if (aSurface.GetType()==GeomAbs_Plane)
        {
          gp_Pln aPlane = aSurface.Plane();
          // It rotates a half turn round the axis of rotation
          aPlane.Rotate(aRotAxis , M_PI/2);

          Handle(Geom_Plane) aGeomPlane = new Geom_Plane (aPlane);
          // constructed aGeomPlane parallel to a plane containing the edge (center mid-edge)
          gp_Pnt aMiddle ((Aa.X()+Ab.X() )/2 ,(Aa.Y()+Ab.Y() )/2 ,(Aa.Z()+Ab.Z() )/2 );
          Handle(AIS_Plane) anAISPlane = new AIS_Plane (aGeomPlane, aMiddle);
          GetMapOfAIS().Bind (anAISPlane, aName);
          TheAISContext()->Display(anAISPlane);
        }
        else
        {
          std::cout<<" vplaneortho: error\n";
          return 1;
        }
      }
      else
      {
        // ShapeA is a Face, deactive the mode Face.
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4));
        std::cout<<" Select an edge\n";

        // Wait for picking
        Standard_Integer argccc = 5;
        const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvv = (const char **) bufff;
        while (ViewerMainLoop( argccc, argvvv) ) { }
        // end of the loop

        TopoDS_Shape aShapeB;
        for (TheAISContext()->InitSelected();
             TheAISContext()->MoreSelected();
             TheAISContext()->NextSelected())
        {
          // Edge ShapeB can be on Face ShapeA
          aShapeB = TheAISContext()->SelectedShape();
        }
        // Close the local context
        TheAISContext()->CloseLocalContext(aCurrentIndex);

        // Construction of plane
        TopoDS_Edge anEdgeB = TopoDS::Edge(aShapeB);
        TopoDS_Vertex aVBa, aVBb;
        TopExp::Vertices(anEdgeB, aVBa, aVBb);
        gp_Pnt aBa = BRep_Tool::Pnt(aVBa);
        gp_Pnt aBb = BRep_Tool::Pnt(aVBb);
        gp_Vec ab (aBa,aBb);
        gp_Dir Dab (ab);
        // Creation of rotation axe
        gp_Ax1 aRotAxis (aBa,Dab);

        TopoDS_Face aFace = TopoDS::Face(aShapeA);
        // The edge must be parallel to the face
        BRepExtrema_ExtPF aHeightA (aVBa, aFace);
        BRepExtrema_ExtPF aHeightB (aVBb, aFace);
        // Comparing the two heights
        if (fabs(sqrt(aHeightA.SquareDistance(1)) - sqrt(aHeightB.SquareDistance(1)))
            >Precision::Confusion())
        {
          // the edge is not parallel to the face
          std::cout<<" vplaneortho error: the edge is not parallel to the face\n";
          return 1;
        }
        // The edge is OK
        BRepAdaptor_Surface aSurface (aFace, Standard_False);
        if (aSurface.GetType()==GeomAbs_Plane)
        {
          gp_Pln aPlane = aSurface.Plane();
          // It rotates a half turn round the axis of rotation
          aPlane.Rotate(aRotAxis , M_PI/2);
          Handle(Geom_Plane) aGeomPlane = new Geom_Plane (aPlane);
          // constructed aGeomPlane parallel to a plane containing the edge theGeomPlane (center mid-edge)
          gp_Pnt aMiddle ((aBa.X()+aBb.X() )/2 , (aBa.Y()+aBb.Y() )/2 , (aBa.Z()+aBb.Z() )/2 );
          Handle(AIS_Plane) anAISPlane = new AIS_Plane (aGeomPlane, aMiddle);
          GetMapOfAIS().Bind (anAISPlane ,aName);
          TheAISContext()->Display(anAISPlane);
        }
        else
        {
          std::cout<<" vplaneortho: error\n";
          return 1;
        }
      }
    }
  }
  return 0;
}

//===============================================================================================
//function : VChangePlane
//purpose  :
//===============================================================================================
static int VChangePlane (Draw_Interpretor& /*theDi*/, Standard_Integer theArgsNb, const char** theArgVec)
{
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    std::cout << theArgVec[0] << "AIS context is not available.\n";
    return 1;
  }

  if (theArgsNb < 3 || theArgsNb > 11)
  {
    std::cerr << theArgVec[0] 
              << ": incorrect number of command arguments.\n"
              << "Type help for more information.\n";
    return 1;
  }

  TCollection_AsciiString aName (theArgVec[1]);

  Handle(AIS_Plane) aPlane = GetMapOfAIS().IsBound2(aName)
    ? Handle(AIS_Plane)::DownCast (GetMapOfAIS().Find2 (aName))
    : NULL;

  if ( aPlane.IsNull() )
  {
    std::cout << theArgVec[0] 
              << ": there is no interactive plane with the given name."
              << "Type help for more information.\n";
    return 1;
  }

  Standard_Real aCenterX = aPlane->Center().X();
  Standard_Real aCenterY = aPlane->Center().Y();
  Standard_Real aCenterZ = aPlane->Center().Z();

  Standard_Real aDirX = aPlane->Component()->Axis().Direction().X();
  Standard_Real aDirY = aPlane->Component()->Axis().Direction().Y();
  Standard_Real aDirZ = aPlane->Component()->Axis().Direction().Z();

  Standard_Real aSizeX = 0.0;
  Standard_Real aSizeY = 0.0;
  aPlane->Size (aSizeX, aSizeY);
  Standard_Boolean isUpdate = Standard_True;

  TCollection_AsciiString aPName, aPValue;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    const TCollection_AsciiString anArg = theArgVec[anArgIt];
    TCollection_AsciiString anArgCase = anArg;
    anArgCase.UpperCase();
    if (ViewerTest::SplitParameter (anArg, aPName, aPValue))
    {
      aPName.UpperCase();
      if (aPName.IsEqual ("X"))
      {
        aCenterX = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("Y"))
      {
        aCenterY = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("Z"))
      {
        aCenterZ = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("DX"))
      {
        aDirX = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("DY"))
      {
        aDirY = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("DZ"))
      {
        aDirZ = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("SX"))
      {
        aSizeX = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("SY"))
      {
        aSizeY = aPValue.RealValue();
      }
    }
    else if (anArg.IsEqual ("NOUPDATE"))
    {
      isUpdate = Standard_False;
    }
  }

  gp_Dir aDirection (aDirX, aDirY, aDirZ);
  gp_Pnt aCenterPnt (aCenterX, aCenterY, aCenterZ);
  aPlane->SetCenter (aCenterPnt);
  aPlane->SetComponent (new Geom_Plane (aCenterPnt, aDirection));
  aPlane->SetSize (aSizeX, aSizeY);

  aContextAIS->Update (aPlane, isUpdate);

  return 0;
}

//==============================================================================
// Fonction  vline
// ---------------  Uniquement par parametre. Pas de selection dans le viewer.
//==============================================================================

//==============================================================================
//function : VLineBuilder
//purpose  : Build an AIS_Line
//Draw arg : vline LineName  [AIS_PointName] [AIS_PointName]
//                           [Xa] [Ya] [Za]   [Xb] [Yb] [Zb]
//==============================================================================
#include <Geom_CartesianPoint.hxx>
#include <AIS_Line.hxx>


static int VLineBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Standard_Integer myCurrentIndex;
  // Verifications
  if (argc!=4 && argc!=8 && argc!=2 )  {di<<"vline error: number of arguments not correct \n";return 1; }
  // Fermeture des contextes
  TheAISContext()->CloseAllContexts();

  // On recupere les parametres
  Handle(AIS_InteractiveObject) theShapeA;
  Handle(AIS_InteractiveObject) theShapeB;

  // Parametres: AIS_Point AIS_Point
  // ===============================
  if (argc==4) {
    theShapeA=
      Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[2]));
    // On verifie que c'est bien une AIS_Point
    if (!theShapeA.IsNull() &&
      theShapeA->Type()==AIS_KOI_Datum && theShapeA->Signature()==1) {
        // on recupere le deuxieme AIS_Point
        theShapeB=
          Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[3]));
        if (theShapeA.IsNull() ||
          (!(theShapeB->Type()==AIS_KOI_Datum && theShapeB->Signature()==1)))
        {
          di <<"vline error: wrong type of 2de argument.\n";
          return 1;
        }
      }
    else {di <<"vline error: wrong type of 1st argument.\n";return 1; }
    // Les deux parametres sont du bon type. On verifie que les points ne sont pas confondus
    Handle(AIS_Point) theAISPointA= Handle(AIS_Point)::DownCast (theShapeA);
    Handle(AIS_Point) theAISPointB= Handle(AIS_Point)::DownCast (theShapeB);

    Handle(Geom_Point ) myGeomPointBA=  theAISPointA->Component();
    Handle(Geom_CartesianPoint ) myCartPointA= Handle(Geom_CartesianPoint)::DownCast (myGeomPointBA);
    //    Handle(Geom_CartesianPoint ) myCartPointA= *(Handle(Geom_CartesianPoint)*)& (theAISPointA->Component() ) ;

    Handle(Geom_Point ) myGeomPointB=  theAISPointB->Component();
    Handle(Geom_CartesianPoint ) myCartPointB= Handle(Geom_CartesianPoint)::DownCast (myGeomPointB);
    //    Handle(Geom_CartesianPoint ) myCartPointB= *(Handle(Geom_CartesianPoint)*)& (theAISPointB->Component() ) ;

    if (myCartPointB->X()==myCartPointA->X() && myCartPointB->Y()==myCartPointA->Y() && myCartPointB->Z()==myCartPointA->Z() ) {
      // B=A
      di<<"vline error: same points\n";return 1;
    }
    // Les deux points sont OK...Construction de l'AIS_Line (en faite, le segment AB)
    Handle(AIS_Line) theAISLine= new AIS_Line(myCartPointA,myCartPointB );
    GetMapOfAIS().Bind(theAISLine,argv[1] );
    TheAISContext()->Display(theAISLine );

  }

  // Parametres 6 Reals
  // ==================

  else if (argc==8) {
    // On verifie que les deux points ne sont pas confondus

    Standard_Real coord[6];
    for(Standard_Integer i=0;i<=2;i++){
      coord[i]=Draw::Atof(argv[2+i]);
      coord[i+3]=Draw::Atof(argv[5+i]);
    }

    Handle(Geom_CartesianPoint ) myCartPointA=new Geom_CartesianPoint (coord[0],coord[1],coord[2] );
    Handle(Geom_CartesianPoint ) myCartPointB=new Geom_CartesianPoint (coord[3],coord[4],coord[5] );

    Handle(AIS_Line) theAISLine= new AIS_Line(myCartPointA,myCartPointB );
    GetMapOfAIS().Bind(theAISLine,argv[1] );
    TheAISContext()->Display(theAISLine );

  }

  // Pas de parametres: Selection dans le viewer.
  // ============================================

  else {
    TheAISContext()->OpenLocalContext();
    myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();

    // Active le mode Vertex.
    TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(1) );
    di<<" Select a vertex \n";

    // Boucle d'attente waitpick.
    Standard_Integer argcc = 5;
    const char *buff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
    const char **argvv = (const char **) buff;
    while (ViewerMainLoop( argcc, argvv) ) { }
    // fin de la boucle

    TopoDS_Shape ShapeA;
    for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
      ShapeA = TheAISContext()->SelectedShape();
    }

    // ShapeA est un Vertex
    if (ShapeA.ShapeType()==TopAbs_VERTEX ) {

      di<<" Select a different vertex.\n";

      TopoDS_Shape ShapeB;
      do {

        // Boucle d'attente waitpick.
        Standard_Integer argccc = 5;
        const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvv = (const char **) bufff;
        while (ViewerMainLoop( argccc, argvvv) ) { }
        // fin de la boucle

        for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
          ShapeB = TheAISContext()->SelectedShape();
        }


      } while(ShapeB.IsSame(ShapeA) );

      // Fermeture du context local
      TheAISContext()->CloseLocalContext(myCurrentIndex);

      // Construction de la line
      gp_Pnt   A=BRep_Tool::Pnt(TopoDS::Vertex(ShapeA)  );
      gp_Pnt   B=BRep_Tool::Pnt(TopoDS::Vertex(ShapeB)  );

      Handle(Geom_CartesianPoint ) myCartPointA=new Geom_CartesianPoint(A);
      Handle(Geom_CartesianPoint ) myCartPointB=new Geom_CartesianPoint(B);

      Handle(AIS_Line) theAISLine= new AIS_Line(myCartPointA,myCartPointB );
      GetMapOfAIS().Bind(theAISLine,argv[1] );
      TheAISContext()->Display(theAISLine );

    }
    else  {
      di<<"vline error.\n";
    }

  }

  return 0;
}

//==============================================================================
// class   : FilledCircle
// purpose : creates filled circle based on AIS_InteractiveObject 
//           and Geom_Circle.
//           This class is used to check method Matches() of class 
//           Select3D_SensitiveCircle with member myFillStatus = Standard_True, 
//           because none of AIS classes provides creation of 
//           Select3D_SensitiveCircle with member myFillStatus = Standard_True 
//           (look method ComputeSelection() )
//============================================================================== 

Handle(Geom_Circle) CreateCircle(gp_Pnt theCenter, Standard_Real theRadius) 
{
  gp_Ax2 anAxes(theCenter, gp_Dir(gp_Vec(0., 0., 1.))); 
  gp_Circ aCirc(anAxes, theRadius);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);
  return aCircle;
}

class FilledCircle : public AIS_InteractiveObject 
{
public:
    // CASCADE RTTI
    DEFINE_STANDARD_RTTI_INLINE(FilledCircle,AIS_InteractiveObject); 

    FilledCircle(gp_Pnt theCenter, Standard_Real theRadius);
    FilledCircle(Handle(Geom_Circle) theCircle);

private:
    TopoDS_Face ComputeFace();

    // Virtual methods implementation
    void Compute (  const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                  const Handle(Prs3d_Presentation)& thePresentation,
                  const Standard_Integer theMode) Standard_OVERRIDE;

    void ComputeSelection (  const Handle(SelectMgr_Selection)& theSelection, 
                           const Standard_Integer theMode) Standard_OVERRIDE;

protected:
    Handle(Geom_Circle) myCircle;
    Standard_Boolean myFilledStatus;

}; 


FilledCircle::FilledCircle(gp_Pnt theCenter, Standard_Real theRadius) 
{
  myCircle = CreateCircle(theCenter, theRadius);
  myFilledStatus = Standard_True;
}

FilledCircle::FilledCircle(Handle(Geom_Circle) theCircle) 
{
  myCircle = theCircle;
  myFilledStatus = Standard_True;
}

TopoDS_Face FilledCircle::ComputeFace() 
{
  // Create edge from myCircle 
  BRepBuilderAPI_MakeEdge anEdgeMaker(myCircle->Circ());
  TopoDS_Edge anEdge = anEdgeMaker.Edge(); 

  // Create wire from anEdge 
  BRepBuilderAPI_MakeWire aWireMaker(anEdge);
  TopoDS_Wire aWire = aWireMaker.Wire();

  // Create face from aWire
  BRepBuilderAPI_MakeFace aFaceMaker(aWire);
  TopoDS_Face aFace = aFaceMaker.Face();

  return aFace;
}

void FilledCircle::Compute(const Handle(PrsMgr_PresentationManager3d) &/*thePresentationManager*/, 
                           const Handle(Prs3d_Presentation) &thePresentation, 
                           const Standard_Integer theMode) 
{
  thePresentation->Clear();

  TopoDS_Face aFace = ComputeFace();

  if (aFace.IsNull()) return;
  if (theMode != 0) return;

  StdPrs_ShadedShape::Add(thePresentation, aFace, myDrawer);
}

void FilledCircle::ComputeSelection(const Handle(SelectMgr_Selection) &theSelection, 
                                    const Standard_Integer /*theMode*/)
{
  Handle(SelectMgr_EntityOwner) anEntityOwner = new SelectMgr_EntityOwner(this);
  Handle(Select3D_SensitiveCircle) aSensitiveCircle = new Select3D_SensitiveCircle(anEntityOwner, 
      myCircle, myFilledStatus);
  theSelection->Add(aSensitiveCircle);
}

//==============================================================================
// Fonction  vcircle
// -----------------  Uniquement par parametre. Pas de selection dans le viewer.
//==============================================================================

//==============================================================================
//function : VCircleBuilder
//purpose  : Build an AIS_Circle
//Draw arg : vcircle CircleName PlaneName PointName Radius IsFilled
//                              PointName PointName PointName IsFilled
//==============================================================================

void DisplayCircle (Handle (Geom_Circle) theGeomCircle,
                    TCollection_AsciiString theName, 
                    Standard_Boolean isFilled) 
{
  Handle(AIS_InteractiveObject) aCircle;
  if (isFilled) 
  {
    aCircle = new FilledCircle(theGeomCircle);
  }
  else
  {
    aCircle = new AIS_Circle(theGeomCircle);
    Handle(AIS_Circle)::DownCast (aCircle)->SetFilledCircleSens (Standard_False);
  }

  // Check if there is an object with given name
  // and remove it from context
  if (GetMapOfAIS().IsBound2(theName)) 
  {
    Handle(Standard_Transient) anObj = GetMapOfAIS().Find2(theName);
    Handle(AIS_InteractiveObject) anInterObj = 
         Handle(AIS_InteractiveObject)::DownCast(anObj);
    TheAISContext()->Remove(anInterObj, Standard_False);
    GetMapOfAIS().UnBind2(theName);
   }

   // Bind the circle to its name
   GetMapOfAIS().Bind(aCircle, theName);

   // Display the circle
   TheAISContext()->Display(aCircle);
  
}

static int VCircleBuilder(Draw_Interpretor& /*di*/, Standard_Integer argc, const char** argv)
{
  Standard_Integer myCurrentIndex;
  // Verification of the arguments
  if (argc>6 || argc<2) 
  { 
    std::cout << "vcircle error: expect 4 arguments.\n"; 
    return 1; // TCL_ERROR 
  }
  TheAISContext()->CloseAllContexts();

  // There are all arguments
  if (argc == 6) 
  {
    // Get arguments
    TCollection_AsciiString aName(argv[1]);
    Standard_Boolean isFilled = (Standard_Boolean)Draw::Atoi(argv[5]);

    Handle(AIS_InteractiveObject) theShapeA;
    Handle(AIS_InteractiveObject) theShapeB;

    theShapeA =
      Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(argv[2]));
    theShapeB =
      Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(argv[3]));


    // Arguments: AIS_Point AIS_Point AIS_Point
    // ========================================
    if (!theShapeA.IsNull() && !theShapeB.IsNull() &&
      theShapeA->Type()==AIS_KOI_Datum && theShapeA->Signature()==1)
    {
      if (theShapeB->Type()!=AIS_KOI_Datum || theShapeB->Signature()!=1 ) 
      {
        std::cout << "vcircle error: 2d argument is unexpected to be a point.\n";
        return 1; // TCL_ERROR 
      }
      // The third object must be a point
      Handle(AIS_InteractiveObject) theShapeC =
        Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(argv[4]));
      if (theShapeC.IsNull() ||
        theShapeC->Type()!=AIS_KOI_Datum || theShapeC->Signature()!=1 ) 
      {
        std::cout << "vcircle error: 3d argument is unexpected to be a point.\n";
        return 1; // TCL_ERROR 
      }
        // tag
        // Verify that the three points are different
        Handle(AIS_Point) theAISPointA = Handle(AIS_Point)::DownCast(theShapeA);
        Handle(AIS_Point) theAISPointB = Handle(AIS_Point)::DownCast(theShapeB);
        Handle(AIS_Point) theAISPointC = Handle(AIS_Point)::DownCast(theShapeC);
        
        Handle(Geom_Point) myGeomPointA = theAISPointA->Component();
        Handle(Geom_CartesianPoint) myCartPointA = 
          Handle(Geom_CartesianPoint)::DownCast(myGeomPointA);

        Handle(Geom_Point) myGeomPointB = theAISPointB->Component();
        Handle(Geom_CartesianPoint) myCartPointB =
          Handle(Geom_CartesianPoint)::DownCast(myGeomPointB);

        Handle(Geom_Point) myGeomPointC = theAISPointC->Component();
        Handle(Geom_CartesianPoint) myCartPointC =
          Handle(Geom_CartesianPoint)::DownCast(myGeomPointC);

        // Test A=B
        if (Abs(myCartPointA->X()-myCartPointB->X()) <= Precision::Confusion() && 
            Abs(myCartPointA->Y()-myCartPointB->Y()) <= Precision::Confusion() && 
            Abs(myCartPointA->Z()-myCartPointB->Z()) <= Precision::Confusion() ) 
        {
          std::cout << "vcircle error: Same points.\n"; 
          return 1; // TCL_ERROR 
        }
        // Test A=C
        if (Abs(myCartPointA->X()-myCartPointC->X()) <= Precision::Confusion() &&
            Abs(myCartPointA->Y()-myCartPointC->Y()) <= Precision::Confusion() && 
            Abs(myCartPointA->Z()-myCartPointC->Z()) <= Precision::Confusion() ) 
        {
          std::cout << "vcircle error: Same points.\n"; 
          return 1; // TCL_ERROR 
        }
        // Test B=C
        if (Abs(myCartPointB->X()-myCartPointC->X()) <= Precision::Confusion() && 
            Abs(myCartPointB->Y()-myCartPointC->Y()) <= Precision::Confusion() && 
            Abs(myCartPointB->Z()-myCartPointC->Z()) <= Precision::Confusion() ) 
        {
          std::cout << "vcircle error: Same points.\n"; 
          return 1;// TCL_ERROR 
        }
        // Construction of the circle
        GC_MakeCircle Cir = GC_MakeCircle (myCartPointA->Pnt(), 
          myCartPointB->Pnt(), myCartPointC->Pnt() );
        Handle (Geom_Circle) theGeomCircle;
        try 
        {
          theGeomCircle = Cir.Value();
        }
        catch (StdFail_NotDone)
        {
          std::cout << "vcircle error: can't create circle\n";
          return -1; // TCL_ERROR
        }
        
        DisplayCircle(theGeomCircle, aName, isFilled);
    }

    // Arguments: AIS_Plane AIS_Point Real
    // ===================================
    else if (theShapeA->Type() == AIS_KOI_Datum && 
      theShapeA->Signature() == 7 ) 
    {
      if (theShapeB->Type() != AIS_KOI_Datum || 
        theShapeB->Signature() != 1 ) 
      {
        std::cout << "vcircle error: 2d element is a unexpected to be a point.\n"; 
        return 1; // TCL_ERROR 
      }
      // Check that the radius is >= 0
      if (Draw::Atof(argv[4]) <= 0 ) 
      {
        std::cout << "vcircle error: the radius must be >=0.\n"; 
        return 1; // TCL_ERROR 
      }

      // Recover the normal to the plane
      Handle(AIS_Plane) theAISPlane = Handle(AIS_Plane)::DownCast(theShapeA);
      Handle(AIS_Point) theAISPointB = Handle(AIS_Point)::DownCast(theShapeB); 

      Handle(Geom_Plane) myGeomPlane = theAISPlane->Component();
      Handle(Geom_Point) myGeomPointB = theAISPointB->Component();
      Handle(Geom_CartesianPoint) myCartPointB = 
        Handle(Geom_CartesianPoint)::DownCast(myGeomPointB);

      gp_Pln mygpPlane = myGeomPlane->Pln();
      gp_Ax1 thegpAxe = mygpPlane.Axis();
      gp_Dir theDir = thegpAxe.Direction();
      gp_Pnt theCenter = myCartPointB->Pnt();
      Standard_Real TheR = Draw::Atof(argv[4]);
      GC_MakeCircle Cir = GC_MakeCircle (theCenter, theDir ,TheR);
      Handle (Geom_Circle) theGeomCircle;
      try 
      {
        theGeomCircle = Cir.Value();
      }
      catch (StdFail_NotDone)
      {
        std::cout << "vcircle error: can't create circle\n";
        return -1; // TCL_ERROR
      }

      DisplayCircle(theGeomCircle, aName, isFilled);

    }

    // Error
    else
    {
      std::cout << "vcircle error: 1st argument is a unexpected type.\n"; 
      return 1; // TCL_ERROR 
    }

  }
  // No arguments: selection in the viewer
  // =========================================
  else 
  {
    // Get the name of the circle 
    TCollection_AsciiString aName(argv[1]);

    TheAISContext()->OpenLocalContext();
    myCurrentIndex = TheAISContext()->IndexOfCurrentLocal();

    // Activate selection mode for vertices and faces
    TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(1) );
    TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
    std::cout << " Select a vertex or a face\n";

    // Wait for picking
    Standard_Integer argcc = 5;
    const char *buff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
    const char **argvv = (const char **) buff;
    while (ViewerMainLoop( argcc, argvv) ) { }
    // end of the loop

    TopoDS_Shape ShapeA;
    for(TheAISContext()->InitSelected(); 
      TheAISContext()->MoreSelected(); 
      TheAISContext()->NextSelected() ) 
    {
      ShapeA = TheAISContext()->SelectedShape();
    }

    // ShapeA is a Vertex
    if (ShapeA.ShapeType() == TopAbs_VERTEX ) 
    {
      TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4) );
      std::cout << " Select a different vertex\n";

      TopoDS_Shape ShapeB;
      do 
      {
        // Wait for picking
        Standard_Integer argccc = 5;
        const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvv = (const char **) bufff;
        while (ViewerMainLoop( argccc, argvvv) ) { }
        // end of the loop

        for(TheAISContext()->InitSelected(); 
          TheAISContext()->MoreSelected(); 
          TheAISContext()->NextSelected() ) 
        {
          ShapeB = TheAISContext()->SelectedShape();
        }
      } while(ShapeB.IsSame(ShapeA) );

      // Selection of ShapeC
      std::cout << " Select the last vertex\n";
      TopoDS_Shape ShapeC;
      do 
      {
        // Wait for picking
        Standard_Integer argcccc = 5;
        const char *buffff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvvv = (const char **) buffff;
        while (ViewerMainLoop( argcccc, argvvvv) ) { }
        // end of the loop

        for(TheAISContext()->InitSelected(); 
          TheAISContext()->MoreSelected(); 
          TheAISContext()->NextSelected() ) 
        {
          ShapeC = TheAISContext()->SelectedShape();
        }
      } while(ShapeC.IsSame(ShapeA) || ShapeC.IsSame(ShapeB) );
      
      // Get isFilled
      Standard_Boolean isFilled;
      std::cout << "Enter filled status (0 or 1)\n";
      cin >> isFilled;

      // Close the local context
      TheAISContext()->CloseLocalContext(myCurrentIndex);

      // Construction of the circle
      gp_Pnt A = BRep_Tool::Pnt(TopoDS::Vertex(ShapeA));
      gp_Pnt B = BRep_Tool::Pnt(TopoDS::Vertex(ShapeB));
      gp_Pnt C = BRep_Tool::Pnt(TopoDS::Vertex(ShapeC));

      GC_MakeCircle Cir = GC_MakeCircle (A, B, C);
      Handle (Geom_Circle) theGeomCircle;
      try 
      {
        theGeomCircle = Cir.Value();
      }
      catch (StdFail_NotDone)
      {
        std::cout << "vcircle error: can't create circle\n";
        return -1; // TCL_ERROR
      }

      DisplayCircle(theGeomCircle, aName, isFilled);

    }
    // Shape is a face
    else
    {
      std::cout << " Select a vertex (in your face)\n";
      TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4) );

      TopoDS_Shape ShapeB;
      // Wait for picking
      Standard_Integer argccc = 5;
      const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
      const char **argvvv = (const char **) bufff;
      while (ViewerMainLoop( argccc, argvvv) ) { }
      // end of the loop

      for(TheAISContext()->InitSelected(); 
        TheAISContext()->MoreSelected(); 
        TheAISContext()->NextSelected() ) 
      {
        ShapeB = TheAISContext()->SelectedShape();
      }

      // Recover the radius 
      Standard_Real theRad;
      do 
      {
        std::cout << " Enter the value of the radius:\n";
        cin >> theRad;
      } while (theRad <= 0);
      
      // Get filled status
      Standard_Boolean isFilled;
      std::cout << "Enter filled status (0 or 1)\n";
      cin >> isFilled;

      // Close the local context
      TheAISContext()->CloseLocalContext(myCurrentIndex);
      // Construction of the circle

      // Recover the normal to the plane. tag
      TopoDS_Face myFace = TopoDS::Face(ShapeA);
      BRepAdaptor_Surface mySurface (myFace, Standard_False);
      gp_Pln myPlane = mySurface.Plane();
      Handle(Geom_Plane) theGeomPlane = new Geom_Plane (myPlane);
      gp_Pln mygpPlane = theGeomPlane->Pln();
      gp_Ax1 thegpAxe = mygpPlane.Axis();
      gp_Dir theDir = thegpAxe.Direction();

      // Recover the center
      gp_Pnt theCenter = BRep_Tool::Pnt(TopoDS::Vertex(ShapeB));

      // Construct the circle
      GC_MakeCircle Cir = GC_MakeCircle (theCenter, theDir ,theRad);
      Handle (Geom_Circle) theGeomCircle;
      try 
      {
        theGeomCircle = Cir.Value();
      }
      catch (StdFail_NotDone)
      {
        std::cout << "vcircle error: can't create circle\n";
        return -1; // TCL_ERROR
      }

      DisplayCircle(theGeomCircle, aName, isFilled);
      
    }

  }

  return 0;
}

//=======================================================================
//function : VDrawText
//purpose  :
//=======================================================================
static int VDrawText (Draw_Interpretor& theDI,
                      Standard_Integer  theArgsNb,
                      const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (theArgsNb < 3)
  {
    std::cout << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }
  else if (aContext.IsNull())
  {
    std::cout << "Error: no active view!\n";
    return 1;
  }

  Standard_Integer           anArgIt = 1;
  TCollection_ExtendedString aName (theArgVec[anArgIt++], Standard_True);
  TCollection_ExtendedString aText (theArgVec[anArgIt++], Standard_True);
  Handle(AIS_TextLabel)      aTextPrs;
  ViewerTest_AutoUpdater     anAutoUpdater (aContext, ViewerTest::CurrentView());

  if (GetMapOfAIS().IsBound2 (aName))
  {
    aTextPrs  = Handle(AIS_TextLabel)::DownCast (GetMapOfAIS().Find2 (aName));
  }
  else
  {
    aTextPrs = new AIS_TextLabel();
    aTextPrs->SetFont ("Courier");
  }

  aTextPrs->SetText (aText);

  Graphic3d_TransModeFlags aTrsfPersFlags = Graphic3d_TMF_None;
  gp_Pnt aTPPosition;
  Aspect_TypeOfDisplayText aDisplayType = Aspect_TODT_NORMAL;


  Standard_Boolean aHasPlane = Standard_False;
  gp_Dir           aNormal;
  gp_Dir           aDirection;
  gp_Pnt           aPos;

  for (; anArgIt < theArgsNb; ++anArgIt)
  {
    TCollection_AsciiString aParam (theArgVec[anArgIt]);
    aParam.LowerCase();

    if (anAutoUpdater.parseRedrawMode (aParam))
    {
      continue;
    }
    else if (aParam == "-pos"
          || aParam == "-position")
    {
      if (anArgIt + 3 >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      aPos.SetX (Draw::Atof (theArgVec[++anArgIt]));
      aPos.SetY (Draw::Atof (theArgVec[++anArgIt]));
      aPos.SetZ (Draw::Atof (theArgVec[++anArgIt]));
      aTextPrs->SetPosition (aPos);
    }
    else if (aParam == "-color")
    {
      if (anArgIt + 1 >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      TCollection_AsciiString aColor (theArgVec[anArgIt + 1]);
      Quantity_NameOfColor aNameOfColor = Quantity_NOC_BLACK;
      if (Quantity_Color::ColorFromName (aColor.ToCString(), aNameOfColor))
      {
        anArgIt += 1;
        aTextPrs->SetColor (aNameOfColor);
        continue;
      }
      else if (anArgIt + 3 >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      TCollection_AsciiString aGreen (theArgVec[anArgIt + 2]);
      TCollection_AsciiString aBlue  (theArgVec[anArgIt + 3]);
      if (!aColor.IsRealValue()
       || !aGreen.IsRealValue()
       || !aBlue.IsRealValue())
      {
        std::cout << "Error: wrong syntax at '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      const Graphic3d_Vec3d anRGB (aColor.RealValue(),
                                   aGreen.RealValue(),
                                   aBlue.RealValue());

      aTextPrs->SetColor (Quantity_Color (anRGB.r(), anRGB.g(), anRGB.b(), Quantity_TOC_RGB));
      anArgIt += 3;
    }
    else if (aParam == "-halign")
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      TCollection_AsciiString aType (theArgVec[anArgIt]);
      aType.LowerCase();
      if (aType == "left")
      {
        aTextPrs->SetHJustification (Graphic3d_HTA_LEFT);
      }
      else if (aType == "center")
      {
        aTextPrs->SetHJustification (Graphic3d_HTA_CENTER);
      }
      else if (aType == "right")
      {
        aTextPrs->SetHJustification (Graphic3d_HTA_RIGHT);
      }
      else
      {
        std::cout << "Error: wrong syntax at '" << aParam.ToCString() << "'.\n";
        return 1;
      }
    }
    else if (aParam == "-valign")
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      TCollection_AsciiString aType (theArgVec[anArgIt]);
      aType.LowerCase();
      if (aType == "top")
      {
        aTextPrs->SetVJustification (Graphic3d_VTA_TOP);
      }
      else if (aType == "center")
      {
        aTextPrs->SetVJustification (Graphic3d_VTA_CENTER);
      }
      else if (aType == "bottom")
      {
        aTextPrs->SetVJustification (Graphic3d_VTA_BOTTOM);
      }
      else if (aType == "topfirstline")
      {
        aTextPrs->SetVJustification (Graphic3d_VTA_TOPFIRSTLINE);
      }
      else
      {
        std::cout << "Error: wrong syntax at '" << aParam.ToCString() << "'.\n";
        return 1;
      }
    }
    else if (aParam == "-angle")
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      aTextPrs->SetAngle (Draw::Atof (theArgVec[anArgIt]) * (M_PI / 180.0));
    }
    else if (aParam == "-zoom")
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      aTextPrs->SetZoomable (Draw::Atoi (theArgVec[anArgIt]) == 1);
    }
    else if (aParam == "-height")
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      aTextPrs->SetHeight (Draw::Atof(theArgVec[anArgIt]));
    }
    else if (aParam == "-aspect")
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      TCollection_AsciiString anOption (theArgVec[anArgIt]);
      anOption.LowerCase();
      if (anOption.IsEqual ("regular"))
      {
        aTextPrs->SetFontAspect (Font_FA_Regular);
      }
      else if (anOption.IsEqual ("bold"))
      {
        aTextPrs->SetFontAspect (Font_FA_Bold);
      }
      else if (anOption.IsEqual ("italic"))
      {
        aTextPrs->SetFontAspect (Font_FA_Italic);
      }
      else if (anOption.IsEqual ("bolditalic"))
      {
        aTextPrs->SetFontAspect (Font_FA_BoldItalic);
      }
    }
    else if (aParam == "-font")
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      aTextPrs->SetFont (theArgVec[anArgIt]);
    }
    else if (aParam == "-plane")
    {
      if (anArgIt + 6 >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      Standard_Real aX = Draw::Atof (theArgVec[++anArgIt]);
      Standard_Real aY = Draw::Atof (theArgVec[++anArgIt]);
      Standard_Real aZ = Draw::Atof (theArgVec[++anArgIt]);
      aNormal.SetCoord (aX, aY, aZ);

      aX = Draw::Atof (theArgVec[++anArgIt]);
      aY = Draw::Atof (theArgVec[++anArgIt]);
      aZ = Draw::Atof (theArgVec[++anArgIt]);
      aDirection.SetCoord (aX, aY, aZ);

      aHasPlane = Standard_True;
    }
    else if (aParam == "-disptype"
          || aParam == "-displaytype")
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }
      TCollection_AsciiString aType (theArgVec[anArgIt]);
      aType.LowerCase();
      if (aType == "subtitle")
        aDisplayType = Aspect_TODT_SUBTITLE;
      else if (aType == "decal")
        aDisplayType = Aspect_TODT_DEKALE;
      else if (aType == "blend")
        aDisplayType = Aspect_TODT_BLEND;
      else if (aType == "dimension")
        aDisplayType = Aspect_TODT_DIMENSION;
      else if (aType == "normal")
        aDisplayType = Aspect_TODT_NORMAL;
      else
      {
        std::cout << "Error: wrong display type '" << aType << "'.\n";
        return 1;
      }
    }
    else if (aParam == "-subcolor"
          || aParam == "-subtitlecolor")
    {
      if (anArgIt + 1 >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      TCollection_AsciiString aColor (theArgVec[anArgIt + 1]);
      Quantity_NameOfColor aNameOfColor = Quantity_NOC_BLACK;
      if (Quantity_Color::ColorFromName (aColor.ToCString(), aNameOfColor))
      {
        anArgIt += 1;
        aTextPrs->SetColorSubTitle (aNameOfColor);
        continue;
      }
      else if (anArgIt + 3 >= theArgsNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      TCollection_AsciiString aGreen (theArgVec[anArgIt + 2]);
      TCollection_AsciiString aBlue  (theArgVec[anArgIt + 3]);
      if (!aColor.IsRealValue()
       || !aGreen.IsRealValue()
       || !aBlue.IsRealValue())
      {
        std::cout << "Error: wrong syntax at '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      const Graphic3d_Vec3d anRGB (aColor.RealValue(),
                                   aGreen.RealValue(),
                                   aBlue.RealValue());

      aTextPrs->SetColorSubTitle (Quantity_Color (anRGB.r(), anRGB.g(), anRGB.b(), Quantity_TOC_RGB));
      anArgIt += 3;
    }
    else if (aParam == "-2d")
    {
      aTrsfPersFlags = Graphic3d_TMF_2d;
    }
    else if (aParam == "-trsfperspos"
          || aParam == "-perspos")
    {
      if (anArgIt + 2 >= theArgsNb)
      {
        std::cerr << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      TCollection_AsciiString aX (theArgVec[++anArgIt]);
      TCollection_AsciiString aY (theArgVec[++anArgIt]);
      TCollection_AsciiString aZ = "0";
      if (!aX.IsIntegerValue()
       || !aY.IsIntegerValue())
      {
        std::cerr << "Error: wrong syntax at '" << aParam << "'.\n";
        return 1;
      }
      if (anArgIt + 1 < theArgsNb)
      {
        TCollection_AsciiString aTemp = theArgVec[anArgIt + 1];
        if (aTemp.IsIntegerValue())
        {
          aZ = aTemp;
          ++anArgIt;
        }
      }
      aTPPosition.SetCoord (aX.IntegerValue(), aY.IntegerValue(), aZ.IntegerValue());
    }
    else
    {
      std::cout << "Error: unknown argument '" << aParam << "'\n";
      return 1;
    }
  }

  if (aHasPlane)
  {
    aTextPrs->SetOrientation3D (gp_Ax2 (aPos, aNormal, aDirection));
  }

  if (aTrsfPersFlags != Graphic3d_TMF_None)
  {
    aTextPrs->SetTransformPersistence (aTrsfPersFlags, aTPPosition);
    aTextPrs->SetDisplayType (aDisplayType);
    aTextPrs->SetZLayer(Graphic3d_ZLayerId_TopOSD);
    if (aTextPrs->Position().Z() != 0)
    {
      aTextPrs->SetPosition (gp_Pnt(aTextPrs->Position().X(), aTextPrs->Position().Y(), 0));
    }
  }
  else if (aTrsfPersFlags != aTextPrs->TransformPersistence().Flags)
  {
    aTextPrs->SetTransformPersistence (aTrsfPersFlags);
    aTextPrs->SetDisplayType (Aspect_TODT_NORMAL);
  }
  ViewerTest::Display (aName, aTextPrs, Standard_False);
  return 0;
}

#include <math.h>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <TShort_HArray1OfShortReal.hxx>

#include <AIS_Triangulation.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <Poly_Connect.hxx>
#include <TColgp_Array1OfDir.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <StdSelect_ShapeTypeFilter.hxx>
#include <AIS_InteractiveObject.hxx>


//===============================================================================================
//function : CalculationOfSphere
//author   : psn
//purpose  : Create a Sphere
//===============================================================================================

Handle( Poly_Triangulation ) CalculationOfSphere( double X , double Y , double Z ,
                                                  int res ,
                                                  double Radius ){
  double mRadius = Radius;
  double mCenter[3] = {X,Y,Z};
  int mThetaResolution;
  int mPhiResolution;
  double mStartTheta = 0;//StartTheta;
  double mEndTheta = 360;//EndTheta;
  double mStartPhi = 0;//StartPhi;
  double mEndPhi = 180;//EndPhi;
  res = res < 4 ? 4 : res;

  mThetaResolution = res;
  mPhiResolution = res;

  int i, j;
  int jStart, jEnd, numOffset;
  double x[3], n[3], deltaPhi, deltaTheta, phi, theta, radius;
  double startTheta, endTheta, startPhi, endPhi;
  int base, numPoles=0, thetaResolution, phiResolution;

  int pts[3];
  int piece = -1;
  int numPieces = 1;
  if ( numPieces > mThetaResolution ) {
    numPieces = mThetaResolution;
  }

  int localThetaResolution =  mThetaResolution;
  double localStartTheta =  mStartTheta;
  double localEndTheta =  mEndTheta;

  while ( localEndTheta < localStartTheta ) {
    localEndTheta += 360.0;
  }

  deltaTheta = (localEndTheta - localStartTheta) / localThetaResolution;

  // Change the ivars based on pieces.
  int start, end;
  start = piece * localThetaResolution / numPieces;
  end = (piece+1) * localThetaResolution / numPieces;
  localEndTheta = localStartTheta + (double)(end) * deltaTheta;
  localStartTheta = localStartTheta + (double)(start) * deltaTheta;
  localThetaResolution = end - start;

  // Create north pole if needed
  int number_point = 0;
  int number_pointArray = 0;

  if ( mStartPhi <= 0.0 ) {
    number_pointArray++;
    numPoles++;
  }
  if ( mEndPhi >= 180.0 ) {
    number_pointArray++;
    numPoles++;
  }

  // Check data, determine increments, and convert to radians
  startTheta = (localStartTheta < localEndTheta ? localStartTheta : localEndTheta);
  startTheta *= M_PI  / 180.0;
  endTheta = (localEndTheta > localStartTheta ? localEndTheta : localStartTheta);
  endTheta *= M_PI  / 180.0;


  startPhi = ( mStartPhi <  mEndPhi ?  mStartPhi :  mEndPhi);
  startPhi *= M_PI  / 180.0;
  endPhi = ( mEndPhi >  mStartPhi ?  mEndPhi :  mStartPhi);
  endPhi *= M_PI  / 180.0;

  phiResolution =  mPhiResolution - numPoles;
  deltaPhi = (endPhi - startPhi) / ( mPhiResolution - 1);
  thetaResolution = localThetaResolution;
  if ( fabs(localStartTheta - localEndTheta) < 360.0 ) {
    ++localThetaResolution;
  }
  deltaTheta = (endTheta - startTheta) / thetaResolution;

  jStart = ( mStartPhi <= 0.0 ? 1 : 0);
  jEnd = ( mEndPhi >= 180.0 ?  mPhiResolution - 1 :  mPhiResolution);

  // Create intermediate points
  for ( i = 0; i < localThetaResolution; i++ ) {
    for ( j = jStart; j < jEnd; j++ ) {
        number_pointArray++;
    }
  }

  //Generate mesh connectivity
  base = phiResolution * localThetaResolution;

  int number_triangle = 0 ;
  if ( mStartPhi <= 0.0 ) { // around north pole
    number_triangle += localThetaResolution;
  }

  if ( mEndPhi >= 180.0 ) { // around south pole
    number_triangle += localThetaResolution;
  }

  // bands in-between poles
  for ( i=0; i < localThetaResolution; i++){
    for ( j=0; j < (phiResolution-1); j++){
       number_triangle +=2;
    }
  }

  Handle( Poly_Triangulation ) polyTriangulation = new Poly_Triangulation(number_pointArray, number_triangle, false);
  TColgp_Array1OfPnt& PointsOfArray = polyTriangulation->ChangeNodes();
  Poly_Array1OfTriangle& pArrayTriangle = polyTriangulation->ChangeTriangles();

  if (  mStartPhi <= 0.0 ){
      x[0] =  mCenter[0];
      x[1] =  mCenter[1];
      x[2] =  mCenter[2] +  mRadius;
      PointsOfArray.SetValue(1,gp_Pnt(x[0],x[1],x[2]));
  }

  // Create south pole if needed
  if (  mEndPhi >= 180.0 ){
      x[0] =  mCenter[0];
      x[1] =  mCenter[1];
      x[2] =  mCenter[2] -  mRadius;
      PointsOfArray.SetValue(2,gp_Pnt(x[0],x[1],x[2]));
  }

  number_point = 3;
  for ( i=0; i < localThetaResolution; i++){
    theta = localStartTheta * M_PI / 180.0 + i*deltaTheta;
    for ( j = jStart; j < jEnd; j++){
        phi = startPhi + j*deltaPhi;
        radius =  mRadius * sin((double)phi);
        n[0] = radius * cos((double)theta);
        n[1] = radius * sin((double)theta);
        n[2] =  mRadius * cos((double)phi);
        x[0] = n[0] +  mCenter[0];
        x[1] = n[1] +  mCenter[1];
        x[2] = n[2] +  mCenter[2];
        PointsOfArray.SetValue(number_point,gp_Pnt(x[0],x[1],x[2]));
        number_point++;
      }
    }

  numPoles = 3;
  number_triangle = 1;
  if ( mStartPhi <= 0.0 ){// around north pole
    for (i=0; i < localThetaResolution; i++){
        pts[0] = phiResolution*i + numPoles;
        pts[1] = (phiResolution*(i+1) % base) + numPoles;
        pts[2] = 1;
        pArrayTriangle.SetValue(number_triangle,Poly_Triangle(pts[0],pts[1],pts[2]));
        number_triangle++;
      }
    }

  if (  mEndPhi >= 180.0 ){ // around south pole
    numOffset = phiResolution - 1 + numPoles;
    for (i=0; i < localThetaResolution; i++){
        pts[0] = phiResolution*i + numOffset;
        pts[2] = ((phiResolution*(i+1)) % base) + numOffset;
        pts[1] = numPoles - 1;
        pArrayTriangle.SetValue(number_triangle,Poly_Triangle(pts[0],pts[1],pts[2]));
        number_triangle++;
      }
    }

  // bands in-between poles

  for (i=0; i < localThetaResolution; i++){
    for (j=0; j < (phiResolution-1); j++){
        pts[0] = phiResolution*i + j + numPoles;
        pts[1] = pts[0] + 1;
        pts[2] = ((phiResolution*(i+1)+j) % base) + numPoles + 1;
        pArrayTriangle.SetValue(number_triangle,Poly_Triangle(pts[0],pts[1],pts[2]));
        number_triangle++;
        pts[1] = pts[2];
        pts[2] = pts[1] - 1;
        pArrayTriangle.SetValue(number_triangle,Poly_Triangle(pts[0],pts[1],pts[2]));
        number_triangle++;
      }
    }

  Poly_Connect* pc = new Poly_Connect(polyTriangulation);

  Handle(TShort_HArray1OfShortReal) Normals = new TShort_HArray1OfShortReal(1, polyTriangulation->NbNodes() * 3);

  Standard_Integer index[3];
  Standard_Real Tol = Precision::Confusion();

  gp_Dir Nor;
  for (i = PointsOfArray.Lower(); i <= PointsOfArray.Upper(); i++) {
      gp_XYZ eqPlan(0, 0, 0);
      for ( pc->Initialize(i); pc->More(); pc->Next()) {
        pArrayTriangle(pc->Value()).Get(index[0], index[1], index[2]);
        gp_XYZ v1(PointsOfArray(index[1]).Coord()-PointsOfArray(index[0]).Coord());
        gp_XYZ v2(PointsOfArray(index[2]).Coord()-PointsOfArray(index[1]).Coord());
        gp_XYZ vv = v1^v2;
        Standard_Real mod = vv.Modulus();
        if(mod < Tol) continue;
        eqPlan += vv/mod;
      }

      Standard_Real modmax = eqPlan.Modulus();

      if(modmax > Tol)
        Nor = gp_Dir(eqPlan);
      else
        Nor = gp_Dir(0., 0., 1.);
      
      Standard_Integer k = (i - PointsOfArray.Lower()) * 3;
      Normals->SetValue(k + 1, (Standard_ShortReal)Nor.X());
      Normals->SetValue(k + 2, (Standard_ShortReal)Nor.Y());
      Normals->SetValue(k + 3, (Standard_ShortReal)Nor.Z());
  }

  delete pc;
  polyTriangulation->SetNormals(Normals);

  return polyTriangulation;
}

//===============================================================================================
//function : VDrawSphere
//author   : psn
//purpose  : Create an AIS shape.
//===============================================================================================
static int VDrawSphere (Draw_Interpretor& /*di*/, Standard_Integer argc, const char** argv)
{
  // check for errors
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    std::cout << "Call vinit before!\n";
    return 1;
  }
  else if (argc < 3)
  {
    std::cout << "Use: " << argv[0]
              << " shapeName Fineness [X=0.0 Y=0.0 Z=0.0] [Radius=100.0] [ToShowEdges=0]\n";
    return 1;
  }

  // read the arguments
  TCollection_AsciiString aShapeName (argv[1]);
  Standard_Integer aResolution = Draw::Atoi (argv[2]);
  Standard_Real aCenterX = (argc > 5) ? Draw::Atof (argv[3]) : 0.0;
  Standard_Real aCenterY = (argc > 5) ? Draw::Atof (argv[4]) : 0.0;
  Standard_Real aCenterZ = (argc > 5) ? Draw::Atof (argv[5]) : 0.0;
  Standard_Real aRadius =  (argc > 6) ? Draw::Atof (argv[6]) : 100.0;
  Standard_Boolean toShowEdges = (argc > 7) ? Draw::Atoi (argv[7]) == 1 : Standard_False;
  Standard_Boolean toPrintInfo = (argc > 8) ? Draw::Atoi (argv[8]) == 1 : Standard_True;

  // remove AIS object with given name from map
  VDisplayAISObject (aShapeName, Handle(AIS_InteractiveObject)());

  if (toPrintInfo)
    std::cout << "Compute Triangulation...\n";
  Handle(AIS_Triangulation) aShape
    = new AIS_Triangulation (CalculationOfSphere (aCenterX, aCenterY, aCenterZ,
                                                  aResolution,
                                                  aRadius));
  Standard_Integer aNumberPoints    = aShape->GetTriangulation()->Nodes().Length();
  Standard_Integer aNumberTriangles = aShape->GetTriangulation()->Triangles().Length();

  // stupid initialization of Green color in RGBA space as integer
  // probably wrong for big-endian CPUs
  Standard_Integer aRed    = 0;
  Standard_Integer aGreen  = 255;
  Standard_Integer aBlue   = 0;
  Standard_Integer anAlpha = 0; // not used
  Standard_Integer aColorInt = aRed;
  aColorInt += aGreen  << 8;
  aColorInt += aBlue   << 16;
  aColorInt += anAlpha << 24;

  // setup colors array per vertex
  Handle(TColStd_HArray1OfInteger) aColorArray = new TColStd_HArray1OfInteger (1, aNumberPoints);
  for (Standard_Integer aNodeId = 1; aNodeId <= aNumberPoints; ++aNodeId)
  {
    aColorArray->SetValue (aNodeId, aColorInt);
  }
  aShape->SetColors (aColorArray);

  // show statistics
  Standard_Integer aPointsSize      = aNumberPoints * 3 * sizeof(float);  // 3x GLfloat
  Standard_Integer aNormalsSize     = aNumberPoints * 3 * sizeof(float);  // 3x GLfloat
  Standard_Integer aColorsSize      = aNumberPoints * 3 * sizeof(float);  // 3x GLfloat without alpha
  Standard_Integer aTrianglesSize   = aNumberTriangles * 3 * sizeof(int); // 3x GLint
  Standard_Integer aPolyConnectSize = aNumberPoints * 4 + aNumberTriangles * 6 * 4;
  Standard_Integer aTotalSize       = aPointsSize + aNormalsSize + aColorsSize + aTrianglesSize;
  aTotalSize >>= 20; //MB
  aNormalsSize >>= 20;
  aColorsSize >>= 20;
  aTrianglesSize >>= 20;
  aPolyConnectSize >>= 20;
  if (toPrintInfo)
  {
    std::cout << "NumberOfPoints:    " << aNumberPoints << "\n"
      << "NumberOfTriangles: " << aNumberTriangles << "\n"
      << "Amount of memory required for PolyTriangulation without Normals: " << (aTotalSize - aNormalsSize) << " Mb\n"
      << "Amount of memory for colors: " << aColorsSize << " Mb\n"
      << "Amount of memory for PolyConnect: " << aPolyConnectSize << " Mb\n"
      << "Amount of graphic card memory required: " << aTotalSize << " Mb\n";
  }

  // Setting material properties, very important for desirable visual result!
  Graphic3d_MaterialAspect aMat (Graphic3d_NOM_PLASTIC);
  aMat.SetAmbient (0.2);
  aMat.SetSpecular (0.5);
  Handle(Graphic3d_AspectFillArea3d) anAspect
    = new Graphic3d_AspectFillArea3d (Aspect_IS_SOLID,
                                      Quantity_NOC_RED,
                                      Quantity_NOC_YELLOW,
                                      Aspect_TOL_SOLID,
                                      1.0,
                                      aMat,
                                      aMat);
  Handle(Prs3d_ShadingAspect) aShAsp = new Prs3d_ShadingAspect();
  if (toShowEdges)
  {
    anAspect->SetEdgeOn();
  }
  else
  {
    anAspect->SetEdgeOff();
  }
  aShAsp->SetAspect (anAspect);
  aShape->Attributes()->SetShadingAspect (aShAsp);

  VDisplayAISObject (aShapeName, aShape);
  return 0;
}

//=============================================================================
//function : VComputeHLR
//purpose  :
//=============================================================================

static int VComputeHLR (Draw_Interpretor& di,
                        Standard_Integer argc,
                        const char** argv)
{
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext ();

  if (aContextAIS.IsNull ())
  {
    di << "Please call vinit before\n";
    return 1;
  }

  if ( argc != 3 &&  argc != 12 )
  {
    di << "Usage: " << argv[0] << " ShapeName HlrName "
       << "[ eye_x eye_y eye_z dir_x dir_y dir_z upx upy upz ]\n"
       << "                    ShapeName - name of the initial shape\n"
       << "                    HlrName - result hlr object from initial shape\n"
       << "                    eye, dir are eye position and look direction\n"
       << "                    up is the look up direction vector\n"
       << "                    Use vtop to see projected hlr shape\n";
    return 1;
  }

  // shape and new object name
  TCollection_AsciiString aShapeName (argv[1]);
  TCollection_AsciiString aHlrName (argv[2]);

  TopoDS_Shape aSh = DBRep::Get (argv[1]);
  if (aSh.IsNull()) 
  {
    BRep_Builder aBrepBuilder;
    BRepTools::Read (aSh, argv[1], aBrepBuilder);
    if (aSh.IsNull ())
    {
      di << "No shape with name " << argv[1] << " found\n";
      return 1;
    }
  }

  if (GetMapOfAIS ().IsBound2 (aHlrName))
  {
    di << "Presentable object with name " << argv[2] << " already exists\n";
    return 1;
  }

  // close local context
  if (aContextAIS->HasOpenedContext ())
    aContextAIS->CloseLocalContext ();

  Handle(HLRBRep_PolyAlgo) aPolyAlgo = new HLRBRep_PolyAlgo();
  HLRBRep_PolyHLRToShape aHLRToShape;

  gp_Pnt anEye;
  gp_Dir aDir;
  gp_Ax2 aProjAx;
  if (argc == 9)
  {
    gp_Dir anUp;

    anEye.SetCoord (Draw::Atof (argv[3]), Draw::Atof (argv[4]), Draw::Atof (argv[5]));
    aDir.SetCoord (Draw::Atof (argv[6]), Draw::Atof (argv[7]), Draw::Atof (argv[8]));
    anUp.SetCoord (Draw::Atof (argv[9]), Draw::Atof (argv[10]), Draw::Atof (argv[11]));
    aProjAx.SetLocation (anEye);
    aProjAx.SetDirection (aDir);
    aProjAx.SetYDirection (anUp);
  }
  else
  {
    gp_Dir aRight;

    Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
    Handle(V3d_View)   aView   = ViewerTest::CurrentView();
    Standard_Integer aWidth, aHeight;
    Standard_Real aCentX, aCentY, aCentZ, aDirX, aDirY, aDirZ;
    Standard_Real aRightX, aRightY, aRightZ;
    aView->Window()->Size (aWidth, aHeight);

    aView->ConvertWithProj (aWidth, aHeight/2, 
                            aRightX, aRightY, aRightZ,
                            aDirX, aDirY, aDirZ);

    aView->ConvertWithProj (aWidth/2, aHeight/2, 
                            aCentX, aCentY, aCentZ,
                            aDirX, aDirY, aDirZ);

    anEye.SetCoord (-aCentX, -aCentY, -aCentZ);
    aDir.SetCoord (-aDirX, -aDirY, -aDirZ);
    aRight.SetCoord (aRightX - aCentX, aRightY - aCentY, aRightZ - aCentZ);
    aProjAx.SetLocation (anEye);
    aProjAx.SetDirection (aDir);
    aProjAx.SetXDirection (aRight);
  }

  HLRAlgo_Projector aProjector (aProjAx);
  aPolyAlgo->Projector (aProjector);
  aPolyAlgo->Load (aSh);
  aPolyAlgo->Update ();

  aHLRToShape.Update (aPolyAlgo);

  // make hlr shape from input shape
  TopoDS_Compound aHlrShape;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound (aHlrShape);

  TopoDS_Shape aCompound = aHLRToShape.VCompound();
  if (!aCompound.IsNull ())
  {
    aBuilder.Add (aHlrShape, aCompound);
  }
  
  // extract visible outlines
  aCompound = aHLRToShape.OutLineVCompound();
  if (!aCompound.IsNull ())
  {
    aBuilder.Add (aHlrShape, aCompound);
  }

  // create an AIS shape and display it
  Handle(AIS_Shape) anObject = new AIS_Shape (aHlrShape);
  GetMapOfAIS().Bind (anObject, aHlrName);
  aContextAIS->Display (anObject);

  aContextAIS->UpdateCurrentViewer ();

  return 0;
}

// This class is a wrap for Graphic3d_ArrayOfPrimitives; it is used for
// manipulating and displaying such an array with AIS context

class MyPArrayObject : public AIS_InteractiveObject
{

public:

  MyPArrayObject (Handle(TColStd_HArray1OfAsciiString) theArrayDescription,
                  Handle(Graphic3d_AspectMarker3d) theMarkerAspect = NULL)
  {
    myArrayDescription = theArrayDescription;
    myMarkerAspect = theMarkerAspect;
  }

  DEFINE_STANDARD_RTTI_INLINE(MyPArrayObject,AIS_InteractiveObject);

private:

  void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                const Handle(Prs3d_Presentation)& aPresentation,
                const Standard_Integer aMode) Standard_OVERRIDE;

  void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                         const Standard_Integer /*theMode*/) Standard_OVERRIDE;

  bool CheckInputCommand (const TCollection_AsciiString theCommand,
                          const Handle(TColStd_HArray1OfAsciiString) theArgsArray,
                          Standard_Integer &theArgIndex,
                          Standard_Integer theArgCount,
                          Standard_Integer theMaxArgs);

protected:

  Handle(TColStd_HArray1OfAsciiString) myArrayDescription;
  Handle(Graphic3d_AspectMarker3d) myMarkerAspect;

};


void MyPArrayObject::Compute (const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                              const Handle(Prs3d_Presentation)& aPresentation,
                              const Standard_Integer /*aMode*/)
{

  // Parsing array description
  Standard_Integer aVertexNum = 0, aBoundNum = 0, aEdgeNum = 0;
  Standard_Boolean hasVColors, hasBColors, hasNormals, hasTexels;
  hasVColors = hasNormals = hasBColors = hasTexels = Standard_False;

  Standard_Integer anArgIndex = 0;
  Standard_Integer anArgsCount = myArrayDescription->Length();
  TCollection_AsciiString anArrayType = myArrayDescription->Value (anArgIndex++);

  TCollection_AsciiString aCommand;
  while (anArgIndex < anArgsCount)
  {
    aCommand = myArrayDescription->Value (anArgIndex);
    aCommand.LowerCase();

    // vertex command
    if (CheckInputCommand ("v", myArrayDescription, anArgIndex, 3, anArgsCount))
    {
      // vertex has a normal or normal with color or texel
      if (CheckInputCommand ("n", myArrayDescription, anArgIndex, 3, anArgsCount))
        hasNormals = Standard_True;

      // vertex has a color
      if (CheckInputCommand ("c", myArrayDescription, anArgIndex, 3, anArgsCount))
        hasVColors = Standard_True;

      // vertex has a texel
      if (CheckInputCommand ("t", myArrayDescription, anArgIndex, 2, anArgsCount))
        hasTexels = Standard_True;

      aVertexNum++;
    }
    // bound command
    else if (CheckInputCommand ("b", myArrayDescription, anArgIndex, 1, anArgsCount))
    {
      // bound has color
      if (CheckInputCommand ("c", myArrayDescription, anArgIndex, 3, anArgsCount))
        hasBColors = Standard_True;

      aBoundNum++;
    }
    // edge command
    else if (CheckInputCommand ("e", myArrayDescription, anArgIndex, 1, anArgsCount))
    {
      aEdgeNum++;
    }
    // unknown command
    else
      anArgIndex++;
  }

  Handle(Graphic3d_ArrayOfPrimitives) anArray;
  if (anArrayType == "points")
  {
    anArray = new Graphic3d_ArrayOfPoints (aVertexNum);
  }
  else if (anArrayType == "segments")
    anArray = new Graphic3d_ArrayOfSegments (aVertexNum, aEdgeNum, hasVColors);
  else if (anArrayType == "polylines")
    anArray = new Graphic3d_ArrayOfPolylines (aVertexNum, aBoundNum, aEdgeNum,
                                              hasVColors, hasBColors);
  else if (anArrayType == "triangles")
    anArray = new Graphic3d_ArrayOfTriangles (aVertexNum, aEdgeNum, hasNormals,
                                              hasVColors, hasTexels);
  else if (anArrayType == "trianglefans")
    anArray = new Graphic3d_ArrayOfTriangleFans (aVertexNum, aBoundNum,
                                                 hasNormals, hasVColors,
                                                 hasBColors, hasTexels);
  else if (anArrayType == "trianglestrips")
    anArray = new Graphic3d_ArrayOfTriangleStrips (aVertexNum, aBoundNum,
                                                   hasNormals, hasVColors,
                                                   hasBColors, hasTexels);
  else if (anArrayType == "quads")
    anArray = new Graphic3d_ArrayOfQuadrangles (aVertexNum, aEdgeNum,
                                                hasNormals, hasVColors,
                                                hasTexels);
  else if (anArrayType == "quadstrips")
    anArray = new Graphic3d_ArrayOfQuadrangleStrips (aVertexNum, aBoundNum,
                                                     hasNormals, hasVColors,
                                                     hasBColors, hasTexels);
  else if (anArrayType == "polygons")
    anArray = new Graphic3d_ArrayOfPolygons (aVertexNum, aBoundNum, aEdgeNum,
                                             hasNormals, hasVColors, hasBColors,
                                             hasTexels);

  anArgIndex = 1;
  while (anArgIndex < anArgsCount)
  {
    aCommand = myArrayDescription->Value (anArgIndex);
    aCommand.LowerCase();
    if (!aCommand.IsAscii())
      break;

    // vertex command
    if (CheckInputCommand ("v", myArrayDescription, anArgIndex, 3, anArgsCount))
    {
      anArray->AddVertex (myArrayDescription->Value (anArgIndex - 3).RealValue(),
                          myArrayDescription->Value (anArgIndex - 2).RealValue(),
                          myArrayDescription->Value (anArgIndex - 1).RealValue());
      const Standard_Integer aVertIndex = anArray->VertexNumber();

      // vertex has a normal or normal with color or texel
      if (CheckInputCommand ("n", myArrayDescription, anArgIndex, 3, anArgsCount))
        anArray->SetVertexNormal (aVertIndex,
                                  myArrayDescription->Value (anArgIndex - 3).RealValue(),
                                  myArrayDescription->Value (anArgIndex - 2).RealValue(),
                                  myArrayDescription->Value (anArgIndex - 1).RealValue());
      
      if (CheckInputCommand ("c", myArrayDescription, anArgIndex, 3, anArgsCount))
        anArray->SetVertexColor (aVertIndex,
                                 myArrayDescription->Value (anArgIndex - 3).RealValue(),
                                 myArrayDescription->Value (anArgIndex - 2).RealValue(),
                                 myArrayDescription->Value (anArgIndex - 1).RealValue());
      
      if (CheckInputCommand ("t", myArrayDescription, anArgIndex, 2, anArgsCount))
        anArray->SetVertexTexel (aVertIndex,
                                 myArrayDescription->Value (anArgIndex - 2).RealValue(),
                                 myArrayDescription->Value (anArgIndex - 1).RealValue());
    }
    // bounds command
    else if (CheckInputCommand ("b", myArrayDescription, anArgIndex, 1, anArgsCount))
    {
      Standard_Integer aVertCount = myArrayDescription->Value (anArgIndex - 1).IntegerValue();

      if (CheckInputCommand ("c", myArrayDescription, anArgIndex, 3, anArgsCount))
        anArray->AddBound (aVertCount,
                           myArrayDescription->Value (anArgIndex - 3).RealValue(),
                           myArrayDescription->Value (anArgIndex - 2).RealValue(),
                           myArrayDescription->Value (anArgIndex - 1).RealValue());

      else
        anArray->AddBound (aVertCount);
    }
    // edge command
    else if (CheckInputCommand ("e", myArrayDescription, anArgIndex, 1, anArgsCount))
    {
      const Standard_Integer aVertIndex = myArrayDescription->Value (anArgIndex - 1).IntegerValue();
      anArray->AddEdge (aVertIndex);
    }
    // unknown command
    else
      anArgIndex++;
  }

  aPresentation->Clear();
  if (!myMarkerAspect.IsNull())
  {
    Prs3d_Root::CurrentGroup (aPresentation)->SetGroupPrimitivesAspect (myMarkerAspect);
  }
  Prs3d_Root::CurrentGroup (aPresentation)->AddPrimitiveArray (anArray);
}

void MyPArrayObject::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                       const Standard_Integer /*theMode*/)
{
  Handle(SelectMgr_EntityOwner) anEntityOwner = new SelectMgr_EntityOwner (this);

  Standard_Integer anArgIndex = 1;
  while (anArgIndex < myArrayDescription->Length())
  {
    if (CheckInputCommand ("v", myArrayDescription, anArgIndex, 3, myArrayDescription->Length()))
    {
      gp_Pnt aPoint (myArrayDescription->Value (anArgIndex - 3).RealValue(),
                     myArrayDescription->Value (anArgIndex - 2).RealValue(),
                     myArrayDescription->Value (anArgIndex - 1).RealValue());
      Handle(Select3D_SensitivePoint) aSensetivePoint = new Select3D_SensitivePoint (anEntityOwner, aPoint);
      theSelection->Add (aSensetivePoint);
    }
    else
    {
      anArgIndex++;
    }
  }
}

bool MyPArrayObject::CheckInputCommand (const TCollection_AsciiString theCommand,
                                       const Handle(TColStd_HArray1OfAsciiString) theArgsArray,
                                       Standard_Integer &theArgIndex,
                                       Standard_Integer theArgCount,
                                       Standard_Integer theMaxArgs)
{
  // check if there is more elements than expected
  if (theArgIndex >= theMaxArgs)
    return false;

  TCollection_AsciiString aStrCommand = theArgsArray->Value (theArgIndex);
  aStrCommand.LowerCase();
  if (aStrCommand.Search(theCommand) != 1 ||
      theArgIndex + (theArgCount - 1) >= theMaxArgs)
    return false;

  // go to the first data element
  theArgIndex++;

  // check data if it can be converted to numeric
  for (int aElement = 0; aElement < theArgCount; aElement++, theArgIndex++)
  {
    aStrCommand = theArgsArray->Value (theArgIndex);
    if (!aStrCommand.IsRealValue())
      return false;
  }

  return true;
}

//=============================================================================
//function : VDrawPArray
//purpose  : Draws primitives array from list of vertexes, bounds, edges
//=============================================================================

static int VDrawPArray (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    di << "Call vinit before!\n";
    return 1;
  }
  else if (argc < 3)
  {
    di << "Use: " << argv[0] << " Name TypeOfArray"
       << " [vertex] ... [bounds] ... [edges]\n"
       << "  TypeOfArray={ points | segments | polylines | triangles |\n"
       << "                trianglefans | trianglestrips | quads |\n"
       << "                quadstrips | polygons }\n"
       << "  vertex={ 'v' x y z [normal={ 'n' nx ny nz }] [color={ 'c' r g b }]"
       << " [texel={ 't' tx ty }] } \n"
       << "  bounds={ 'b' verticies_count [color={ 'c' r g b }] }\n"
       << "  edges={ 'e' vertex_id }\n";
    return 1;
  }

  // read the arguments
  Standard_Integer aArgIndex = 1;
  TCollection_AsciiString aName (argv[aArgIndex++]);
  TCollection_AsciiString anArrayType (argv[aArgIndex++]);

  Standard_Boolean hasVertex = Standard_False;

  Handle(TColStd_HArray1OfAsciiString) anArgsArray = new TColStd_HArray1OfAsciiString (0, argc - 2);
  anArgsArray->SetValue (0, anArrayType);

  if (anArrayType != "points"         &&
      anArrayType != "segments"       &&
      anArrayType != "polylines"      &&
      anArrayType != "triangles"      &&
      anArrayType != "trianglefans"   &&
      anArrayType != "trianglestrips" &&
      anArrayType != "quads"          &&
      anArrayType != "quadstrips"     &&
      anArrayType != "polygons")
  {
    di << "Unexpected type of primitives array\n";
    return 1;
  }

  TCollection_AsciiString aCommand;
  for (Standard_Integer anArgIndex = 3; anArgIndex < argc; anArgIndex++)
  {
    aCommand = argv[anArgIndex];
    aCommand.LowerCase();
    if (!aCommand.IsAscii())
    {
      di << "Unexpected argument: #" << aArgIndex - 1 << " , "
         << "should be an array element: 'v', 'b', 'e' \n";
      break;
    }

    if (aCommand == "v")
    {
      hasVertex = Standard_True;
    }

    anArgsArray->SetValue (anArgIndex - 2, aCommand);
  }

  if (!hasVertex)
  {
    di << "You should pass any verticies in the list of array elements\n";
    return 1;
  }

  Handle(Graphic3d_AspectMarker3d)    anAspPoints;
  if (anArrayType == "points")
  {
    anAspPoints = new Graphic3d_AspectMarker3d (Aspect_TOM_POINT, Quantity_NOC_YELLOW, 1.0f);
  }

  // create primitives array object
  Handle(MyPArrayObject) aPObject = new MyPArrayObject (anArgsArray, anAspPoints);

  // register the object in map
  VDisplayAISObject (aName, aPObject);

  return 0;
}

//=======================================================================
//function : VSetLocation
//purpose  : Change location of AIS interactive object
//=======================================================================

static Standard_Integer VSetLocation (Draw_Interpretor& /*di*/,
                                      Standard_Integer  theArgNb,
                                      const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aContext, ViewerTest::CurrentView());
  if (aContext.IsNull())
  {
    std::cout << "Error: no active view!\n";
    return 1;
  }

  TCollection_AsciiString aName;
  gp_Vec aLocVec;
  Standard_Boolean isSetLoc = Standard_False;

  Standard_Integer anArgIter = 1;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString anArg = theArgVec[anArgIter];
    if (anUpdateTool.parseRedrawMode (theArgVec[anArgIter]))
    {
      continue;
    }
    else if (aName.IsEmpty())
    {
      aName = anArg;
    }
    else if (!isSetLoc)
    {
      isSetLoc = Standard_True;
      if (anArgIter + 1 >= theArgNb)
      {
        std::cout << "Error: syntax error at '" << anArg << "'\n";
        return 1;
      }
      aLocVec.SetX (Draw::Atof (theArgVec[anArgIter++]));
      aLocVec.SetY (Draw::Atof (theArgVec[anArgIter]));
      if (anArgIter + 1 < theArgNb)
      {
        aLocVec.SetZ (Draw::Atof (theArgVec[++anArgIter]));
      }
    }
    else
    {
      std::cout << "Error: unknown argument '" << anArg << "'\n";
      return 1;
    }
  }

  // find object
  const ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(AIS_InteractiveObject) anIObj;
  if (aMap.IsBound2 (aName))
  {
    anIObj = Handle(AIS_InteractiveObject)::DownCast (aMap.Find2 (aName));
  }
  if (anIObj.IsNull())
  {
    std::cout << "Error: object '" << aName << "' is not displayed!\n";
    return 1;
  }

  gp_Trsf aTrsf;
  aTrsf.SetTranslation (aLocVec);
  TopLoc_Location aLocation (aTrsf);
  aContext->SetLocation (anIObj, aLocation);
  return 0;
}

//=======================================================================
//function : TransformPresentation
//purpose  : Change transformation of AIS interactive object
//=======================================================================
static Standard_Integer LocalTransformPresentation (Draw_Interpretor& /*theDi*/,
                                                    Standard_Integer theArgNb,
                                                    const char** theArgVec)
{
  if (theArgNb <= 1)
  {
    std::cout << "Error: too few arguments.\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool(aContext, ViewerTest::CurrentView());
  if (aContext.IsNull())
  {
    std::cout << "Error: no active view!\n";
    return 1;
  }

  gp_Trsf aTrsf;
  Standard_Integer aLast = theArgNb;
  const char* aName = theArgVec[0];

  Standard_Boolean isReset = Standard_False;
  Standard_Boolean isMove = Standard_False;

  // Prefix 'vloc'
  aName += 4;

  if (!strcmp (aName, "reset"))
  {
    isReset = Standard_True;
  }
  else if (!strcmp (aName, "move"))
  {
    if (theArgNb < 3)
    {
      std::cout << "Error: too few arguments.\n";
      return 1;
    }

    const ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();

    Handle(AIS_InteractiveObject) anIObj;
    if (aMap.IsBound2 (theArgVec[theArgNb - 1]))
    {
      anIObj = Handle(AIS_InteractiveObject)::DownCast (aMap.Find2 (theArgVec[theArgNb - 1]));
    }

    if (anIObj.IsNull())
    {
      std::cout << "Error: object '" << theArgVec[theArgNb - 1] << "' is not displayed!\n";
      return 1;
    }

    isMove = Standard_True;

    aTrsf = anIObj->Transformation();
    aLast = theArgNb - 1;
  }
  else if (!strcmp (aName, "translate"))
  {
    if (theArgNb < 5)
    {
      std::cout << "Error: too few arguments.\n";
      return 1;
    }
    aTrsf.SetTranslation (gp_Vec (Draw::Atof (theArgVec[theArgNb - 3]),
                                  Draw::Atof (theArgVec[theArgNb - 2]),
                                  Draw::Atof (theArgVec[theArgNb - 1])));
    aLast = theArgNb - 3;
  }
  else if (!strcmp (aName, "rotate"))
  {
    if (theArgNb < 9)
    {
      std::cout << "Error: too few arguments.\n";
      return 1;
    }

    aTrsf.SetRotation (
      gp_Ax1 (gp_Pnt (Draw::Atof (theArgVec[theArgNb - 7]),
                      Draw::Atof (theArgVec[theArgNb - 6]),
                      Draw::Atof (theArgVec[theArgNb - 5])),
              gp_Vec (Draw::Atof (theArgVec[theArgNb - 4]),
                      Draw::Atof (theArgVec[theArgNb - 3]),
                      Draw::Atof (theArgVec[theArgNb - 2]))),
      Draw::Atof (theArgVec[theArgNb - 1]) * (M_PI / 180.0));

    aLast = theArgNb - 7;
  }
  else if (!strcmp (aName, "mirror"))
  {
    if (theArgNb < 8)
    {
      std::cout << "Error: too few arguments.\n";
      return 1;
    }

    aTrsf.SetMirror (gp_Ax2 (gp_Pnt (Draw::Atof(theArgVec[theArgNb - 6]),
                                     Draw::Atof(theArgVec[theArgNb - 5]),
                                     Draw::Atof(theArgVec[theArgNb - 4])),
                             gp_Vec (Draw::Atof(theArgVec[theArgNb - 3]),
                                     Draw::Atof(theArgVec[theArgNb - 2]),
                                     Draw::Atof(theArgVec[theArgNb - 1]))));
    aLast = theArgNb - 6;
  }
  else if (!strcmp (aName, "scale"))
  {
    if (theArgNb < 6)
    {
      std::cout << "Error: too few arguments.\n";
      return 1;
    }

    aTrsf.SetScale (gp_Pnt (Draw::Atof(theArgVec[theArgNb - 4]),
                            Draw::Atof(theArgVec[theArgNb - 3]),
                            Draw::Atof(theArgVec[theArgNb - 2])),
                    Draw::Atof(theArgVec[theArgNb - 1]));
    aLast = theArgNb - 4;
  }

  for (Standard_Integer anIdx = 1; anIdx < aLast; anIdx++)
  {
    // find object
    const ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
    Handle(AIS_InteractiveObject) anIObj;
    if (aMap.IsBound2 (theArgVec[anIdx]))
    {
      anIObj = Handle(AIS_InteractiveObject)::DownCast (aMap.Find2 (theArgVec[anIdx]));
    }
    if (anIObj.IsNull())
    {
      std::cout << "Error: object '" << theArgVec[anIdx] << "' is not displayed!\n";
      return 1;
    }
    
    if (isReset)
    {
      // aTrsf already identity
    }
    else if (isMove)
    {
      aTrsf = anIObj->LocalTransformation() * anIObj->Transformation().Inverted() * aTrsf;
    }
    else
    {
      aTrsf = anIObj->LocalTransformation() * aTrsf;
    }

    TopLoc_Location aLocation (aTrsf);
    aContext->SetLocation (anIObj, aLocation);
  }

  return 0;
}

//===============================================================================================
//function : VConnect
//purpose  : Creates and displays AIS_ConnectedInteractive object from input object and location 
//Draw arg : vconnect name Xo Yo Zo object1 object2 ... [color=NAME]
//===============================================================================================

static Standard_Integer VConnect (Draw_Interpretor& /*di*/, 
                                  Standard_Integer argc, 
                                  const char ** argv) 
{
  // Check the viewer
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    std::cout << "vconnect error : call vinit before\n";
    return 1; // TCL_ERROR
  }
  // Check argumnets 
  if (argc < 6)
  {
    std::cout << "vconnect error: expect at least 5 arguments\n";
    return 1; // TCL_ERROR
  }

  // Get values
  Standard_Integer anArgIter = 1;
  TCollection_AsciiString aName (argv[anArgIter++]);
  Handle(AIS_MultipleConnectedInteractive) anOriginObject;
  TCollection_AsciiString aColorString (argv[argc-1]);
  Standard_CString aColorName = "";
  Standard_Boolean hasColor = Standard_False;
  if (aColorString.Search ("color=") != -1)
  {
    hasColor = Standard_True;
    aColorString.Remove (1, 6);
    aColorName = aColorString.ToCString();
  }
  Handle(AIS_InteractiveObject) anObject;

  // AIS_MultipleConnectedInteractive
  const Standard_Integer aNbShapes = hasColor ? (argc - 1) : argc;
  for (Standard_Integer i = 5; i < aNbShapes; ++i)
  {
    TCollection_AsciiString anOriginObjectName (argv[i]);
    if (aName.IsEqual (anOriginObjectName))
    {
      std::cout << "vconnect error: equal names for connected objects\n";
      continue;
    }
    if (GetMapOfAIS().IsBound2 (anOriginObjectName))
    {
      Handle(Standard_Transient) anObj = GetMapOfAIS().Find2 (anOriginObjectName);
      anObject = Handle(AIS_InteractiveObject)::DownCast(anObj);
      if (anObject.IsNull())
      {
        std::cout << "Object " << anOriginObjectName << " is used for non AIS viewer\n";
        continue;
      }
    }
    else
    {
      Standard_CString aOriginName = anOriginObjectName.ToCString();
      TopoDS_Shape aTDShape = DBRep::Get (aOriginName);
      if (aTDShape.IsNull())
      {
        std::cout << "vconnect error: object " << anOriginObjectName << " doesn't exist\n";
        continue;
      }
      anObject = new AIS_Shape (aTDShape);
      aContext->Load (anObject);
      anObject->SetColor (ViewerTest::GetColorFromName (aColorName));
    }

    if (anOriginObject.IsNull())
    {
      anOriginObject = new AIS_MultipleConnectedInteractive();
    }

    anOriginObject->Connect (anObject);
  }
  if (anOriginObject.IsNull())
  {
    std::cout << "vconect error : can't connect input objects\n";
    return 1; // TCL_ERROR
  }

  // Get location data
  Standard_Real aXo = Draw::Atof (argv[anArgIter++]);
  Standard_Real aYo = Draw::Atof (argv[anArgIter++]);
  Standard_Real aZo = Draw::Atof (argv[anArgIter++]);

  // Create transformation
  gp_Vec aTranslation (aXo, aYo, aZo);

  gp_Trsf aTrsf; 
  aTrsf.SetTranslationPart (aTranslation);
  TopLoc_Location aLocation (aTrsf);

  anOriginObject->SetLocalTransformation (aTrsf);

  // Check if there is another object with given name
  // and remove it from context
  if(GetMapOfAIS().IsBound2(aName))
  {
    Handle(AIS_InteractiveObject) anObj = 
      Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(aName));
    TheAISContext()->Remove(anObj, Standard_False);
    GetMapOfAIS().UnBind2(aName);
  }

  // Bind connected object to its name
  GetMapOfAIS().Bind (anOriginObject, aName);

  // Display connected object
  TheAISContext()->Display (anOriginObject);

  return 0;
}

//===============================================================================================
//function : VConnectTo
//purpose  : Creates and displays AIS_ConnectedInteractive object from input object and location 
//Draw arg : vconnectto name Xo Yo Zo object [-nodisplay|-noupdate|-update]
//===============================================================================================

static Standard_Integer VConnectTo (Draw_Interpretor& /*di*/, 
                                    Standard_Integer argc, 
                                    const char ** argv) 
{
  // Check the viewer
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aContext, ViewerTest::CurrentView());
  if (aContext.IsNull())
  {
    std::cout << "vconnect error : call vinit before\n";
    return 1; // TCL_ERROR
  }
  // Check argumnets 
  if (argc != 6 && argc != 7)
  {
    std::cout << "vconnect error: expect at least 5 arguments\n";
    return 1; // TCL_ERROR
  }

  // Get values
  Standard_Integer anArgIter = 1;
  TCollection_AsciiString aName (argv[anArgIter++]);
  Handle(AIS_InteractiveObject) anOriginObject;

  TCollection_AsciiString anOriginObjectName(argv[5]);
  if (aName.IsEqual (anOriginObjectName))
  {
    std::cout << "vconnect error: equal names for connected objects\n"; 
    return 1; // TCL_ERROR
  }
  if (GetMapOfAIS().IsBound2 (anOriginObjectName))
  {
    Handle(Standard_Transient) anObj = GetMapOfAIS().Find2 (anOriginObjectName);
    anOriginObject = Handle(AIS_InteractiveObject)::DownCast(anObj);
    if (anOriginObject.IsNull())
    {
      std::cout << "Object " << anOriginObjectName << " is used for non AIS viewer\n";
      return 1; // TCL_ERROR
    }
  }
  else
  {
    Standard_CString aOriginName = anOriginObjectName.ToCString();
    TopoDS_Shape aTDShape = DBRep::Get (aOriginName);
    if (aTDShape.IsNull())
    {
      std::cout << "vconnect error: object " << anOriginObjectName << " doesn't exist\n";
      return 1; // TCL_ERROR
    }
    anOriginObject = new AIS_Shape (aTDShape);
    GetMapOfAIS().Bind (anOriginObject, anOriginObjectName);
  }
 
  // Get location data
  Standard_Real aXo = Draw::Atof (argv[anArgIter++]);
  Standard_Real aYo = Draw::Atof (argv[anArgIter++]);
  Standard_Real aZo = Draw::Atof (argv[anArgIter++]);

  // Create transformation
  gp_Vec aTranslation (aXo, aYo, aZo);

  gp_Trsf aTrsf; 
  aTrsf.SetTranslationPart (aTranslation);
 
  Handle(AIS_ConnectedInteractive) aConnected;

  aConnected = new AIS_ConnectedInteractive();

  aConnected->Connect (anOriginObject, aTrsf);

  // Check if there is another object with given name
  // and remove it from context
  if(GetMapOfAIS().IsBound2(aName))
  {
    Handle(AIS_InteractiveObject) anObj = 
      Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(aName));
    TheAISContext()->Remove(anObj, Standard_False);
    GetMapOfAIS().UnBind2(aName);
  }

  // Bind connected object to its name
  GetMapOfAIS().Bind (aConnected, aName);

  if (argc == 7)
  {
    TCollection_AsciiString anArg = argv[6];
    anArg.LowerCase();
    if (anArg == "-nodisplay")
      return 0;

    if (!anUpdateTool.parseRedrawMode (anArg))
    {
      std::cout << "Warning! Unknown argument '" << anArg << "' passed, -nodisplay|-noupdate|-update expected at this point.\n";
    }
  }

  // Display connected object
  TheAISContext()->Display (aConnected, Standard_False);

  return 0;
}

//=======================================================================
//function : VDisconnect
//purpose  :
//=======================================================================
static Standard_Integer VDisconnect (Draw_Interpretor& di,
                                     Standard_Integer argc,
                                     const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    std::cout << argv[0] << "ERROR : use 'vinit' command before \n";
    return 1;
  }
  
  if (argc != 3)
  {
    std::cout << "ERROR : Usage : " << argv[0] << " name object\n";
    return 1;
  }

  TCollection_AsciiString aName (argv[1]);
  TCollection_AsciiString anObject (argv[2]);
  Standard_Integer anObjectNumber = Draw::Atoi (argv[2]);

  // find objects
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(AIS_MultipleConnectedInteractive) anAssembly;
  if (!aMap.IsBound2 (aName) )
  {
    std::cout << "Use 'vdisplay' before\n";
    return 1;
  }

  anAssembly = Handle(AIS_MultipleConnectedInteractive)::DownCast (aMap.Find2 (aName));
  if (anAssembly.IsNull())
  {
    di << "Not an assembly\n";
    return 1;
  }

  Handle(AIS_InteractiveObject) anIObj;
  if (!aMap.IsBound2 (anObject))
  {
    // try to interpret second argument as child number
    if (anObjectNumber > 0 && anObjectNumber <= anAssembly->Children().Size())
    {
      Standard_Integer aCounter = 1;
      for (PrsMgr_ListOfPresentableObjectsIter anIter (anAssembly->Children()); anIter.More(); anIter.Next())
      {
        if (aCounter == anObjectNumber)
        {
          anIObj = Handle(AIS_InteractiveObject)::DownCast (anIter.Value());
          break;
        }
        ++aCounter;
      }
    }
    else
    {
      std::cout << "Use 'vdisplay' before\n";
      return 1;
    }    
  }

  // if object was found by name
  if (anIObj.IsNull())
  {
    anIObj = Handle(AIS_InteractiveObject)::DownCast (aMap.Find2 (anObject));
  }

  aContext->Disconnect (anAssembly, anIObj);
  aContext->UpdateCurrentViewer();

  return 0;
}

//=======================================================================
//function : VAddConnected
//purpose  :
//=======================================================================
static Standard_Integer VAddConnected (Draw_Interpretor& di,
                                       Standard_Integer argc,
                                       const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    std::cout << argv[0] << "error : use 'vinit' command before \n";
    return 1;
  }
  
  if (argc != 6)
  {
    std::cout << argv[0] << " error: expect 5 arguments\n";
    return 1;
  }

  TCollection_AsciiString aName (argv[1]);
  TCollection_AsciiString anObject (argv[5]);
  Standard_Real aX = Draw::Atof (argv[2]);
  Standard_Real aY = Draw::Atof (argv[3]);
  Standard_Real aZ = Draw::Atof (argv[4]);

  // find object
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(AIS_MultipleConnectedInteractive) anAssembly;
  if (!aMap.IsBound2 (aName) )
  {
    std::cout << "Use 'vdisplay' before\n";
    return 1;
  }

  anAssembly = Handle(AIS_MultipleConnectedInteractive)::DownCast (aMap.Find2 (aName));
  if (anAssembly.IsNull())
  {
    di << "Not an assembly\n";
    return 1;
  }

  Handle(AIS_InteractiveObject) anIObj;
  if (!aMap.IsBound2 (anObject))
  {
      std::cout << "Use 'vdisplay' before\n";
      return 1; 
  }

  anIObj = Handle(AIS_InteractiveObject)::DownCast (aMap.Find2 (anObject));

  gp_Trsf aTrsf;
  aTrsf.SetTranslation (gp_Vec (aX, aY, aZ));
 
  anAssembly->Connect (anIObj, aTrsf);
  TheAISContext()->Display (anAssembly);
  TheAISContext()->RecomputeSelectionOnly (anAssembly);
  aContext->UpdateCurrentViewer();

  return 0;
}

//=======================================================================
//function : VListConnected
//purpose  :
//=======================================================================
static Standard_Integer VListConnected (Draw_Interpretor& /*di*/,
                                        Standard_Integer argc,
                                        const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    std::cout << argv[0] << "ERROR : use 'vinit' command before \n";
    return 1;
  }
  
  if (argc != 2)
  {
    std::cout << "ERROR : Usage : " << argv[0] << " name\n";
    return 1;
  }

  TCollection_AsciiString aName (argv[1]);

  // find object
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(AIS_MultipleConnectedInteractive) anAssembly;
  if (!aMap.IsBound2 (aName) )
  {
    std::cout << "Use 'vdisplay' before\n";
    return 1;
  }

  anAssembly = Handle(AIS_MultipleConnectedInteractive)::DownCast (aMap.Find2 (aName));
  if (anAssembly.IsNull())
  {
    std::cout << "Not an assembly\n";
    return 1;
  }

  std::cout << "Children of " << aName << ":\n";

  Standard_Integer aCounter = 1;
  for (PrsMgr_ListOfPresentableObjectsIter anIter (anAssembly->Children()); anIter.More(); anIter.Next())
  {
    if (GetMapOfAIS().IsBound1 (anIter.Value()))
    {
      TCollection_AsciiString aCuurrentName = GetMapOfAIS().Find1 (anIter.Value());
      std::cout << aCounter << ")  " << aCuurrentName << "    (" << anIter.Value()->DynamicType()->Name() << ")";
    }

    std::cout << aCounter << ")  " << anIter.Value()->DynamicType()->Name();

    Handle(AIS_ConnectedInteractive) aConnected = Handle(AIS_ConnectedInteractive)::DownCast (anIter.Value());
    if (!aConnected.IsNull() && !aConnected->ConnectedTo().IsNull() && aMap.IsBound1 (aConnected->ConnectedTo()))
    {
      std::cout << " connected to " << aMap.Find1 (aConnected->ConnectedTo());
    }
    std::cout << std::endl;
    
    ++aCounter;
  }

  return 0;
}

namespace
{
  //! Checks if theMode is already turned on for theObj.
  static Standard_Boolean InList (const Handle(AIS_InteractiveContext)& theAISContext,
                                  const Handle(AIS_InteractiveObject)&  theObj,
                                  const Standard_Integer                theMode)
  {
    TColStd_ListOfInteger anActiveModes;
    theAISContext->ActivatedModes (theObj, anActiveModes);
    for (TColStd_ListIteratorOfListOfInteger aModeIt (anActiveModes); aModeIt.More(); aModeIt.Next())
    {
      if (aModeIt.Value() == theMode)
      {
        return Standard_True;
      }
    }
    return Standard_False;
  }
}

//===============================================================================================
//function : VSetSelectionMode
//purpose  : Sets input selection mode for input object or for all displayed objects 
//Draw arg : vselmode [object] mode On/Off (1/0)
//===============================================================================================
static Standard_Integer VSetSelectionMode (Draw_Interpretor& /*di*/,
                                           Standard_Integer  theArgc,
                                           const char**      theArgv)
{
  // Check errors
  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    std::cerr << "Call vinit before!" << std::endl;
    return 1;
  }

  // Check the arguments
  if (theArgc < 3 && theArgc > 5)
  {
    std::cerr << "vselmode error : expects at least 2 arguments.\n"
              << "Type help "<< theArgv[0] <<" for more information." << std::endl;
    return 1;
  }

  TCollection_AsciiString aLastArg (theArgv[theArgc - 1]);
  aLastArg.LowerCase();
  Standard_Boolean isToOpenLocalCtx = aLastArg == "-local";

  // get objects to change selection mode
  AIS_ListOfInteractive aTargetIOs;
  Standard_Integer anArgNb = isToOpenLocalCtx ? theArgc - 1 : theArgc;
  if (anArgNb == 3)
  {
    anAISContext->DisplayedObjects (aTargetIOs);
  }
  else
  {
    // Check if there is an object with given name in context
    const TCollection_AsciiString aNameIO (theArgv[1]);
    if (GetMapOfAIS().IsBound2 (aNameIO))
    {
      Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (aNameIO));
      if (anIO.IsNull())
      {
        std::cerr << "vselmode error : object name is used for non AIS viewer" << std::endl;
        return 1;
      }
      aTargetIOs.Append (anIO);
    }
  }

  const Standard_Integer aSelectionMode = Draw::Atoi (anArgNb == 3 ? theArgv[1] : theArgv[2]);
  const Standard_Boolean toTurnOn       = Draw::Atoi (anArgNb == 3 ? theArgv[2] : theArgv[3]);
  if (aSelectionMode == 0 && anAISContext->HasOpenedContext())
  {
    anAISContext->CloseLocalContext();
  }

  if (aSelectionMode == 0)
  {
    if (toTurnOn)
    {
      for (AIS_ListIteratorOfListOfInteractive aTargetIt (aTargetIOs); aTargetIt.More(); aTargetIt.Next())
      {
        const Handle(AIS_InteractiveObject)& anIO = aTargetIt.Value();
        TColStd_ListOfInteger anActiveModes;
        anAISContext->ActivatedModes (anIO, anActiveModes);
        if (!anActiveModes.IsEmpty())
        {
          anAISContext->Deactivate (anIO);
        }
        if (!InList (anAISContext, anIO, aSelectionMode))
        {
          anAISContext->Activate (anIO);
        }
      }
    }
    else
    {
      for (AIS_ListIteratorOfListOfInteractive aTargetIt (aTargetIOs); aTargetIt.More(); aTargetIt.Next())
      {
        const Handle(AIS_InteractiveObject)& anIO = aTargetIt.Value();
        if (InList (anAISContext, anIO, aSelectionMode))
        {
          anAISContext->Deactivate (anIO);
        }
      }
    }
  }

  if (aSelectionMode != 0 && toTurnOn) // Turn on specified mode
  {
    if (!anAISContext->HasOpenedContext() && isToOpenLocalCtx)
    {
      anAISContext->OpenLocalContext (Standard_False);
    }

    for (AIS_ListIteratorOfListOfInteractive aTargetIt (aTargetIOs); aTargetIt.More(); aTargetIt.Next())
    {
      const Handle(AIS_InteractiveObject)& anIO = aTargetIt.Value();
      if (InList (anAISContext, anIO, 0))
      {
        anAISContext->Deactivate (anIO, 0);
      }
      if (!InList (anAISContext, anIO, aSelectionMode))
      {
        anAISContext->Load (anIO, -1, Standard_True);
        anAISContext->Activate (anIO, aSelectionMode);
      }
    }
  }

  if (aSelectionMode != 0 && !toTurnOn) // Turn off specified mode
  {
    for (AIS_ListIteratorOfListOfInteractive aTargetIt (aTargetIOs); aTargetIt.More(); aTargetIt.Next())
    {
      const Handle(AIS_InteractiveObject)& anIO = aTargetIt.Value();
      if (InList (anAISContext, anIO, aSelectionMode))
      {
        anAISContext->Deactivate (anIO, aSelectionMode);
      }
    }
  }

  return 0;
}

//===============================================================================================
//function : VSelectionNext
//purpose  : 
//===============================================================================================
static Standard_Integer VSelectionNext(Draw_Interpretor& /*theDI*/,
                                 Standard_Integer /*theArgsNb*/,
                                 const char** /*theArgVec*/)
{
  // Check errors
  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  Handle(V3d_View) aView = ViewerTest::CurrentView();

  if (anAISContext.IsNull())
  {
    std::cerr << "Call vinit before!" << std::endl;
    return 1;
  }

  anAISContext->HilightNextDetected(aView);
  return 0;
}

//===============================================================================================
//function : VSelectionPrevious
//purpose  : 
//===============================================================================================
static Standard_Integer VSelectionPrevious(Draw_Interpretor& /*theDI*/,
                                 Standard_Integer /*theArgsNb*/,
                                 const char** /*theArgVec*/)
{
  // Check errors
  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  Handle(V3d_View) aView = ViewerTest::CurrentView();

  if (anAISContext.IsNull())
  {
    std::cerr << "Call vinit before!" << std::endl;
    return 1;
  }

  anAISContext->HilightPreviousDetected(aView);
  return 0;
}


//==========================================================================
//class   : Triangle 
//purpose : creates Triangle based on AIS_InteractiveObject. 
//          This class was implemented for testing Select3D_SensitiveTriangle
//===========================================================================

class Triangle: public AIS_InteractiveObject 
{
public: 
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(Triangle,AIS_InteractiveObject);
  Triangle (const gp_Pnt& theP1, 
            const gp_Pnt& theP2, 
            const gp_Pnt& theP3);
protected:
  void Compute (  const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                  const Handle(Prs3d_Presentation)& thePresentation,
                  const Standard_Integer theMode) Standard_OVERRIDE;

  void ComputeSelection (  const Handle(SelectMgr_Selection)& theSelection, 
                           const Standard_Integer theMode) Standard_OVERRIDE;
private: 
  gp_Pnt myPoint1;
  gp_Pnt myPoint2;
  gp_Pnt myPoint3;
};


Triangle::Triangle (const gp_Pnt& theP1,
                    const gp_Pnt& theP2,
                    const gp_Pnt& theP3)
{
  myPoint1 = theP1;
  myPoint2 = theP2;
  myPoint3 = theP3;
}

void Triangle::Compute(const Handle(PrsMgr_PresentationManager3d)& /*thePresentationManager*/,
                       const Handle(Prs3d_Presentation)& thePresentation,
                       const Standard_Integer /*theMode*/)
{
  thePresentation->Clear();

  BRepBuilderAPI_MakeEdge anEdgeMaker1(myPoint1, myPoint2),
                          anEdgeMaker2(myPoint2, myPoint3),
                          anEdgeMaker3(myPoint3, myPoint1);

  TopoDS_Edge anEdge1 = anEdgeMaker1.Edge(),
              anEdge2 = anEdgeMaker2.Edge(),
              anEdge3 = anEdgeMaker3.Edge();
  if(anEdge1.IsNull() || anEdge2.IsNull() || anEdge3.IsNull())
    return;

  BRepBuilderAPI_MakeWire aWireMaker(anEdge1, anEdge2, anEdge3);
  TopoDS_Wire aWire = aWireMaker.Wire();
  if(aWire.IsNull()) return;

  BRepBuilderAPI_MakeFace aFaceMaker(aWire);
  TopoDS_Face aFace = aFaceMaker.Face();
  if(aFace.IsNull()) return;

  StdPrs_ShadedShape::Add(thePresentation, aFace, myDrawer);
}

void Triangle::ComputeSelection(const Handle(SelectMgr_Selection)& theSelection, 
                                const Standard_Integer /*theMode*/)
{
  Handle(SelectMgr_EntityOwner) anEntityOwner = new SelectMgr_EntityOwner(this);
  Handle(Select3D_SensitiveTriangle) aSensTriangle = 
    new Select3D_SensitiveTriangle(anEntityOwner, myPoint1, myPoint2, myPoint3);
  theSelection->Add(aSensTriangle);
}

//===========================================================================
//function : VTriangle 
//Draw arg : vtriangle Name PointName PointName PointName
//purpose  : creates and displays Triangle
//===========================================================================

//function: IsPoint
//purpose : checks if the object with theName is AIS_Point, 
//          if yes initialize thePoint from MapOfAIS
Standard_Boolean IsPoint (const TCollection_AsciiString& theName,
                          Handle(AIS_Point)& thePoint)
{
  Handle(AIS_InteractiveObject) anObject = 
    Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(theName));
  if(anObject.IsNull() || 
     anObject->Type() != AIS_KOI_Datum || 
     anObject->Signature() != 1)
  {
    return Standard_False;
  }
  thePoint = Handle(AIS_Point)::DownCast(anObject);
  if(thePoint.IsNull())
    return Standard_False;
  return Standard_True;
}

//function: IsMatch
//purpose: checks if thePoint1 is equal to thePoint2
Standard_Boolean IsMatch (const Handle(Geom_CartesianPoint)& thePoint1,
                          const Handle(Geom_CartesianPoint)& thePoint2)
{
  if(Abs(thePoint1->X()-thePoint2->X()) <= Precision::Confusion() &&
     Abs(thePoint1->Y()-thePoint2->Y()) <= Precision::Confusion() &&
     Abs(thePoint1->Z()-thePoint2->Z()) <= Precision::Confusion())
  {
    return Standard_True;
  }
  return Standard_False;
}

static Standard_Integer VTriangle (Draw_Interpretor& /*di*/,
                                   Standard_Integer argc,
                                   const char ** argv)
{
  // Check arguments
  if (argc != 5)
  {
    std::cout<<"vtriangle error: expects 4 argumnets\n";
    return 1; // TCL_ERROR
  }

  TheAISContext()->CloseAllContexts();

  // Get and check values
  TCollection_AsciiString aName(argv[1]);

  Handle(AIS_Point) aPoint1, aPoint2, aPoint3;
  if (!IsPoint(argv[2], aPoint1))
  {
    std::cout<<"vtriangle error: the 2nd argument must be a point\n";
    return 1; // TCL_ERROR
  }
  if (!IsPoint(argv[3], aPoint2))
  {
    std::cout<<"vtriangle error: the 3d argument must be a point\n";
    return 1; // TCL_ERROR
  }
  if (!IsPoint(argv[4], aPoint3))
  {
    std::cout<<"vtriangle error: the 4th argument must be a point\n";
    return 1; // TCL_ERROR
  }

  // Check that points are different
  Handle(Geom_CartesianPoint) aCartPoint1 = 
    Handle(Geom_CartesianPoint)::DownCast(aPoint1->Component());
  Handle(Geom_CartesianPoint) aCartPoint2 = 
    Handle(Geom_CartesianPoint)::DownCast(aPoint2->Component());
  // Test aPoint1 = aPoint2
  if (IsMatch(aCartPoint1, aCartPoint2))
  {
    std::cout<<"vtriangle error: the 1st and the 2nd points are equal\n";
    return 1; // TCL_ERROR
  }
  // Test aPoint2 = aPoint3
  Handle(Geom_CartesianPoint) aCartPoint3 = 
    Handle(Geom_CartesianPoint)::DownCast(aPoint3->Component());
  if (IsMatch(aCartPoint2, aCartPoint3))
  {
    std::cout<<"vtriangle error: the 2nd and the 3d points are equal\n";
    return 1; // TCL_ERROR
  }
  // Test aPoint3 = aPoint1
  if (IsMatch(aCartPoint1, aCartPoint3))
  {
    std::cout<<"vtriangle error: the 1st and the 3d points are equal\n";
    return 1; // TCL_ERROR
  }

  // Create triangle
  Handle(Triangle) aTriangle = new Triangle(aCartPoint1->Pnt(),
                                            aCartPoint2->Pnt(),
                                            aCartPoint3->Pnt());

  // Check if there is an object with given name
  // and remove it from context
  if (GetMapOfAIS().IsBound2(aName))
  {
    Handle(Standard_Transient) anObj = GetMapOfAIS().Find2(aName);
    Handle(AIS_InteractiveObject) anInterObj = 
         Handle(AIS_InteractiveObject)::DownCast(anObj);
    TheAISContext()->Remove(anInterObj, Standard_False);
    GetMapOfAIS().UnBind2(aName);
  }

  // Bind triangle to its name
  GetMapOfAIS().Bind(aTriangle, aName);

  // Display triangle
  TheAISContext()->Display(aTriangle);
  return 0;
}

//class  : SegmentObject
//purpose: creates segment based on AIS_InteractiveObject.
//         This class was implemented for testing Select3D_SensitiveCurve

class SegmentObject: public AIS_InteractiveObject
{
public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(SegmentObject,AIS_InteractiveObject); 
  SegmentObject (const gp_Pnt& thePnt1, const gp_Pnt& thePnt2);
protected:
  void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                const Handle(Prs3d_Presentation)& thePresentation,
                const Standard_Integer theMode) Standard_OVERRIDE;

  void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection, 
                         const Standard_Integer theMode) Standard_OVERRIDE;
private:
  gp_Pnt myPoint1;
  gp_Pnt myPoint2;
};


SegmentObject::SegmentObject (const gp_Pnt& thePnt1, const gp_Pnt& thePnt2)
{
  myPoint1 = thePnt1;
  myPoint2 = thePnt2;
}

void SegmentObject::Compute (const Handle(PrsMgr_PresentationManager3d) &/*thePresentationManager*/,
                             const Handle(Prs3d_Presentation) &thePresentation,
                             const Standard_Integer /*theMode*/)
{
  thePresentation->Clear();
  BRepBuilderAPI_MakeEdge anEdgeMaker(myPoint1, myPoint2);
  TopoDS_Edge anEdge = anEdgeMaker.Edge();
  if (anEdge.IsNull())
    return;
  BRepAdaptor_Curve aCurveAdaptor(anEdge);
  StdPrs_Curve::Add(thePresentation, aCurveAdaptor, myDrawer);
}

void SegmentObject::ComputeSelection (const Handle(SelectMgr_Selection) &theSelection,
                                      const Standard_Integer /*theMode*/)
{
  Handle(SelectMgr_EntityOwner) anOwner = new SelectMgr_EntityOwner(this);
  Handle(TColgp_HArray1OfPnt) anArray = new TColgp_HArray1OfPnt(1, 2);
  anArray->SetValue(1, myPoint1);
  anArray->SetValue(2, myPoint2);
  Handle(Select3D_SensitiveCurve) aSensCurve = 
    new Select3D_SensitiveCurve(anOwner, anArray);
  theSelection->Add(aSensCurve);
}

//=======================================================================
//function  : VSegment
//Draw args : vsegment Name PointName PointName
//purpose   : creates and displays Segment
//=======================================================================
static Standard_Integer VSegment (Draw_Interpretor& /*di*/,
                                  Standard_Integer argc,
                                  const char ** argv)
{
  // Check arguments
  if(argc!=4)
  {
    std::cout<<"vsegment error: expects 3 arguments\n";
    return 1; // TCL_ERROR
  }

  TheAISContext()->CloseAllContexts();

  // Get and check arguments
  TCollection_AsciiString aName(argv[1]);
  Handle(AIS_Point) aPoint1, aPoint2;
  if (!IsPoint(argv[2], aPoint1))
  {
    std::cout<<"vsegment error: the 2nd argument should be a point\n";
    return 1; // TCL_ERROR
  }
  if (!IsPoint(argv[3], aPoint2))
  {
    std::cout<<"vsegment error: the 3d argument should be a point\n";
    return 1; // TCL_ERROR
  }
  //Check that points are different
  Handle(Geom_CartesianPoint) aCartPoint1 = 
    Handle(Geom_CartesianPoint)::DownCast(aPoint1->Component());
  Handle(Geom_CartesianPoint) aCartPoint2 = 
    Handle(Geom_CartesianPoint)::DownCast(aPoint2->Component());
  if(IsMatch(aCartPoint1, aCartPoint2))
  {
    std::cout<<"vsegment error: equal points\n";
    return 1; // TCL_ERROR
  }
  
  // Create segment
  Handle(SegmentObject) aSegment = new SegmentObject(aCartPoint1->Pnt(), aCartPoint2->Pnt());
  // Check if there is an object with given name
  // and remove it from context
  if (GetMapOfAIS().IsBound2(aName))
  {
    Handle(Standard_Transient) anObj = GetMapOfAIS().Find2(aName);
    Handle(AIS_InteractiveObject) anInterObj = 
         Handle(AIS_InteractiveObject)::DownCast(anObj);
    TheAISContext()->Remove(anInterObj, Standard_False);
    GetMapOfAIS().UnBind2(aName);
  }

  // Bind segment to its name
  GetMapOfAIS().Bind(aSegment, aName);

  // Display segment
  TheAISContext()->Display(aSegment);
  return 0;
}

//=======================================================================
//function : VObjZLayer
//purpose  : Set or get z layer id for presentable object
//=======================================================================

static Standard_Integer VObjZLayer (Draw_Interpretor& di,
                                    Standard_Integer argc,
                                    const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << argv[0] << "Call 'vinit' before!\n";
    return 1;
  }

  // get operation
  TCollection_AsciiString aOperation;
  if (argc >= 2)
    aOperation = TCollection_AsciiString (argv [1]);

  // check for correct arguments
  if (!(argc == 4 && aOperation.IsEqual ("set")) &&
      !(argc == 3 && aOperation.IsEqual ("get")))
  {
    di << "Usage : " << argv[0] << " set/get object [layerid]\n";
    di << " set - set layer id for interactive object, layerid - z layer id\n";
    di << " get - get layer id of interactive object\n";
    di << " argument layerid should be passed for set operation only\n";
    return 1;
  }

  // find object
  TCollection_AsciiString aName (argv[2]);
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  if (!aMap.IsBound2 (aName))
  {
    di << "Use 'vdisplay' before\n";
    return 1;
  }

  // find interactive object
  Handle(Standard_Transient) anObj = GetMapOfAIS().Find2 (aName);
  Handle(AIS_InteractiveObject) anInterObj =
    Handle(AIS_InteractiveObject)::DownCast (anObj);
  if (anInterObj.IsNull())
  {
    di << "Not an AIS interactive object!\n";
    return 1;
  }

  // process operation
  if (aOperation.IsEqual ("set"))
  {
    Standard_Integer aLayerId = Draw::Atoi (argv [3]);
    aContext->SetZLayer (anInterObj, aLayerId);
  }
  else if (aOperation.IsEqual ("get"))
  {
    di << "Z layer id: " << aContext->GetZLayer (anInterObj);
  }
  
  return 0;
}

//=======================================================================
//function : VPolygonOffset
//purpose  : Set or get polygon offset parameters
//=======================================================================
static Standard_Integer VPolygonOffset(Draw_Interpretor& /*di*/,
                                       Standard_Integer argc,
                                       const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    std::cout << argv[0] << " Call 'vinit' before!\n";
    return 1;
  }

  if (argc > 2 && argc != 5)
  {
    std::cout << "Usage : " << argv[0] << " [object [mode factor units]] - sets/gets polygon offset parameters for an object,"
      "without arguments prints the default values" << std::endl;
    return 1;
  }

  // find object
  Handle(AIS_InteractiveObject) anInterObj;
  if (argc >= 2)
  {
    TCollection_AsciiString aName (argv[1]);
    ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
    if (!aMap.IsBound2 (aName))
    {
      std::cout << "Use 'vdisplay' before" << std::endl;
      return 1;
    }

    // find interactive object
    Handle(Standard_Transient) anObj = GetMapOfAIS().Find2 (aName);
    anInterObj = Handle(AIS_InteractiveObject)::DownCast (anObj);
    if (anInterObj.IsNull())
    {
      std::cout << "Not an AIS interactive object!" << std::endl;
      return 1;
    }
  }

  Standard_Integer aMode;
  Standard_ShortReal    aFactor, aUnits;
  if (argc == 5)
  {
    aMode   = Draw::Atoi(argv[2]);
    aFactor = (Standard_ShortReal) Draw::Atof(argv[3]);
    aUnits  = (Standard_ShortReal) Draw::Atof(argv[4]);

    anInterObj->SetPolygonOffsets(aMode, aFactor, aUnits);
    aContext->UpdateCurrentViewer();
    return 0;
  }
  else if (argc == 2)
  {
    if (anInterObj->HasPolygonOffsets())
    {
      anInterObj->PolygonOffsets(aMode, aFactor, aUnits);
      std::cout << "Current polygon offset parameters for " << argv[1] << ":" << std::endl;
      std::cout << "\tMode: "   << aMode   << std::endl;
      std::cout << "\tFactor: " << aFactor << std::endl;
      std::cout << "\tUnits: "  << aUnits  << std::endl;
      return 0;
    }
    else
    {
      std::cout << "Specific polygon offset parameters are not set for " << argv[1] << std::endl;
    }
  }

  std::cout << "Default polygon offset parameters:" << std::endl;
  aContext->DefaultDrawer()->ShadingAspect()->Aspect()->PolygonOffsets(aMode, aFactor, aUnits);
  std::cout << "\tMode: "   << aMode   << std::endl;
  std::cout << "\tFactor: " << aFactor << std::endl;
  std::cout << "\tUnits: "  << aUnits  << std::endl;

  return 0;
}

//=======================================================================
//function : VShowFaceBoundaries
//purpose  : Set face boundaries drawing on/off for ais object
//=======================================================================
static Standard_Integer VShowFaceBoundary (Draw_Interpretor& /*di*/,
                                           Standard_Integer argc,
                                           const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext ();
  if (aContext.IsNull ())
  {
    std::cout << argv[0] << " Call 'vinit' before!\n";
    return 1;
  }

  if ((argc != 3 && argc < 6) || argc > 8)
  {
    std::cout << "Usage :\n " << argv[0]
              << " ObjectName isOn [R G B [LineWidth [LineStyle]]]\n"
              << "   ObjectName - name of AIS interactive object. \n"
              << "                if ObjectName = \"\", then set as default\n"
              << "                settings for all newly displayed objects\n"
              << "   isOn       - flag indicating whether the boundaries\n"
              << "                should be turned on or off (can be set\n"
              << "                to 0 (off) or 1 (on)).\n"
              << "   R, G, B    - red, green and blue components of boundary\n"
              << "                color in range (0 - 255).\n"
              << "                (default is (0, 0, 0)\n"
              << "   LineWidth  - line width\n"
              << "                (default is 1)\n"
              << "   LineStyle  - line fill style :\n"
              << "                 0 - solid  \n"
              << "                 1 - dashed \n"
              << "                 2 - dot    \n"
              << "                 3 - dashdot\n"
              << "                 (default is solid)";
    return 1;
  }

  TCollection_AsciiString aName (argv[1]);

  Quantity_Parameter aRed      = 0.0;
  Quantity_Parameter aGreen    = 0.0;
  Quantity_Parameter aBlue     = 0.0;
  Standard_Real      aWidth    = 1.0;
  Aspect_TypeOfLine  aLineType = Aspect_TOL_SOLID;
  
  // find object
  Handle(AIS_InteractiveObject) anInterObj;

  // if name is empty - apply attributes for default aspect
  if (!aName.IsEmpty ())
  {
    ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS ();
    if (!aMap.IsBound2 (aName))
    {
      std::cout << "Use 'vdisplay' on " << aName << " before" << std::endl;
      return 1;
    }

    // find interactive object
    Handle(Standard_Transient) anObj = GetMapOfAIS ().Find2 (aName);
    anInterObj = Handle(AIS_InteractiveObject)::DownCast (anObj);
    if (anInterObj.IsNull ())
    {
      std::cout << "Not an AIS interactive object!" << std::endl;
      return 1;
    }
  }
  
  const Handle(Prs3d_Drawer)& aDrawer = (aName.IsEmpty ()) ?
    TheAISContext ()->DefaultDrawer () : anInterObj->Attributes ();

  // turn boundaries on/off
  Standard_Boolean isBoundaryDraw = (Draw::Atoi (argv[2]) == 1);
  aDrawer->SetFaceBoundaryDraw (isBoundaryDraw);
  
  // set boundary line color
  if (argc >= 6)
  {
    // Text color
    aRed   = Draw::Atof (argv[3])/255.;
    aGreen = Draw::Atof (argv[4])/255.;
    aBlue  = Draw::Atof (argv[5])/255.;
  }

  // set line width
  if (argc >= 7)
  {
    aWidth = (Standard_Real)Draw::Atof (argv[6]);
  }

  // select appropriate line type
  if (argc == 8)
  {
    switch (Draw::Atoi (argv[7]))
    {
      case 1: aLineType = Aspect_TOL_DASH;    break;
      case 2: aLineType = Aspect_TOL_DOT;     break;
      case 3: aLineType = Aspect_TOL_DOTDASH; break;
      default:
        aLineType = Aspect_TOL_SOLID;
    }
  }

  Quantity_Color aColor (aRed, aGreen, aBlue, Quantity_TOC_RGB);

  Handle(Prs3d_LineAspect) aBoundaryAspect = 
    new Prs3d_LineAspect (aColor, aLineType, aWidth);

  aDrawer->SetFaceBoundaryAspect (aBoundaryAspect);

  TheAISContext()->Redisplay (anInterObj);
  
  return 0;
}

// This class is used for testing markers.

class ViewerTest_MarkersArrayObject : public AIS_InteractiveObject
{

public:

  ViewerTest_MarkersArrayObject (const gp_XYZ& theStartPoint,
                                 const Standard_Integer& thePointsOnSide,
                                 Handle(Graphic3d_AspectMarker3d) theMarkerAspect = NULL)
  {
    myStartPoint = theStartPoint;
    myPointsOnSide = thePointsOnSide;
    myMarkerAspect = theMarkerAspect;
  }

  DEFINE_STANDARD_RTTI_INLINE(MyPArrayObject,AIS_InteractiveObject);

private:

  void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                const Handle(Prs3d_Presentation)& aPresentation,
                const Standard_Integer aMode) Standard_OVERRIDE;

  void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                         const Standard_Integer /*theMode*/) Standard_OVERRIDE;

protected:

  gp_XYZ myStartPoint;
  Standard_Integer myPointsOnSide;
  Handle(Graphic3d_AspectMarker3d) myMarkerAspect;
};


void ViewerTest_MarkersArrayObject::Compute (const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                              const Handle(Prs3d_Presentation)& aPresentation,
                              const Standard_Integer /*aMode*/)
{
  Handle(Graphic3d_ArrayOfPrimitives) anArray = new Graphic3d_ArrayOfPoints ((Standard_Integer )Pow (myPointsOnSide, 3), myPointsOnSide != 1);
  if (myPointsOnSide == 1)
  {
    anArray->AddVertex (myStartPoint);
  }
  else
  {
    for (Standard_Real i = 1; i <= myPointsOnSide; i++)
    {
      for (Standard_Real j = 1; j <= myPointsOnSide; j++)
      {
        for (Standard_Real k = 1; k <= myPointsOnSide; k++)
        {
          anArray->AddVertex (myStartPoint.X() + i, myStartPoint.Y() + j, myStartPoint.Z() + k);
          anArray->SetVertexColor (anArray->VertexNumber(),
                                   i / myPointsOnSide,
                                   j / myPointsOnSide,
                                   k / myPointsOnSide);
        }
      }
    }
  }

  aPresentation->Clear();
  if (!myMarkerAspect.IsNull())
  {
    Prs3d_Root::CurrentGroup (aPresentation)->SetGroupPrimitivesAspect (myMarkerAspect);
  }
  Prs3d_Root::CurrentGroup (aPresentation)->AddPrimitiveArray (anArray);
}

void ViewerTest_MarkersArrayObject::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                       const Standard_Integer /*theMode*/)
{
  Handle(SelectMgr_EntityOwner) anEntityOwner = new SelectMgr_EntityOwner (this);

  if (myPointsOnSide == 1)
  {
    gp_Pnt aPoint (myStartPoint);
    Handle(Select3D_SensitivePoint) aSensetivePoint = new Select3D_SensitivePoint (anEntityOwner, aPoint);
    theSelection->Add (aSensetivePoint);
  }
  else
  {
    for (Standard_Real i = 1; i <= myPointsOnSide; i++)
    {
      for (Standard_Real j = 1; j <= myPointsOnSide; j++)
      {
        for (Standard_Real k = 1; k <= myPointsOnSide; k++)
        {
          gp_Pnt aPoint (myStartPoint.X() + i, myStartPoint.Y() + j, myStartPoint.Z() + k);
          Handle(Select3D_SensitivePoint) aSensetivePoint = new Select3D_SensitivePoint (anEntityOwner, aPoint);
          theSelection->Add (aSensetivePoint);
        }
      }
    }
  }
}
//=======================================================================
//function : VMarkersTest
//purpose  : Draws an array of markers for testing purposes.
//=======================================================================
static Standard_Integer VMarkersTest (Draw_Interpretor&,
                                      Standard_Integer  theArgNb,
                                      const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    std::cerr << "Call 'vinit' before!\n";
    return 1;
  }

  if (theArgNb < 5)
  {
    std::cerr << "Usage :\n " << theArgVec[0]
              << "name X Y Z [PointsOnSide=10] [MarkerType=0] [Scale=1.0] [FileName=ImageFile]\n";
    return 1;
  }

  Standard_Integer anArgIter = 1;

  TCollection_AsciiString aName (theArgVec[anArgIter++]);
  TCollection_AsciiString aFileName;
  gp_XYZ aPnt (Atof (theArgVec[anArgIter]),
               Atof (theArgVec[anArgIter + 1]),
               Atof (theArgVec[anArgIter + 2]));
  anArgIter += 3;

  Standard_Integer aPointsOnSide = 10;
  Standard_Integer aMarkerType   = -1;
  Standard_Real    aScale        = 1.0;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    const TCollection_AsciiString anArg (theArgVec[anArgIter]);
    if (anArg.Search ("PointsOnSide=") > -1)
    {
      aPointsOnSide = anArg.Token ("=", 2).IntegerValue();
    }
    else if (anArg.Search ("MarkerType=") > -1)
    {
      aMarkerType = anArg.Token ("=", 2).IntegerValue();
    }
    else if (anArg.Search ("Scale=") > -1)
    {
      aScale = anArg.Token ("=", 2).RealValue();
    }
    else if (anArg.Search ("FileName=") > -1)
    {
      aFileName = anArg.Token ("=", 2);
    }
    else
    {
      std::cerr << "Wrong argument: " << anArg << "\n";
      return 1;
    }
  }

  Handle(Graphic3d_AspectMarker3d) anAspect;
  Handle(Image_AlienPixMap) anImage;
  Quantity_Color aColor (Quantity_NOC_GREEN1);
  if ((aMarkerType == Aspect_TOM_USERDEFINED || aMarkerType < 0)
   && !aFileName.IsEmpty())
  {
    anImage = new Image_AlienPixMap();
    if (!anImage->Load (aFileName))
    {
      std::cerr << "Could not load image from file '" << aFileName << "'!\n";
      return 1;
    }
    if (anImage->Format() == Image_PixMap::ImgGray)
    {
      anImage->SetFormat (Image_PixMap::ImgAlpha);
    }
    else if (anImage->Format() == Image_PixMap::ImgGrayF)
    {
      anImage->SetFormat (Image_PixMap::ImgAlphaF);
    }
    anAspect = new Graphic3d_AspectMarker3d (anImage);
  }
  else
  {
    anAspect = new Graphic3d_AspectMarker3d (aMarkerType >= 0 ? (Aspect_TypeOfMarker )aMarkerType : Aspect_TOM_POINT, aColor, aScale);
  }

  Handle(ViewerTest_MarkersArrayObject) aMarkersArray = new ViewerTest_MarkersArrayObject (aPnt, aPointsOnSide, anAspect);
  VDisplayAISObject (aName, aMarkersArray);

  return 0;
}

//! Auxiliary function to parse font aspect style argument
static Standard_Boolean parseFontStyle (const TCollection_AsciiString& theArg,
                                        Font_FontAspect&               theAspect)
{
  if (theArg == "regular"
   || *theArg.ToCString() == 'r')
  {
    theAspect = Font_FA_Regular;
    return Standard_True;
  }
  else if (theArg == "bolditalic")
  {
    theAspect = Font_FA_BoldItalic;
    return Standard_True;
  }
  else if (theArg == "bold"
        || *theArg.ToCString() == 'b')
  {
    theAspect = Font_FA_Bold;
    return Standard_True;
  }
  else if (theArg == "italic"
        || *theArg.ToCString() == 'i')
  {
    theAspect = Font_FA_Italic;
    return Standard_True;
  }
  return Standard_False;
}

//! Auxiliary function
static TCollection_AsciiString fontStyleString (const Font_FontAspect theAspect)
{
  switch (theAspect)
  {
    case Font_FA_Regular:    return "regular";
    case Font_FA_BoldItalic: return "bolditalic";
    case Font_FA_Bold:       return "bold";
    case Font_FA_Italic:     return "italic";
    default:                 return "undefined";
  }
}

//=======================================================================
//function : TextToBrep
//purpose  : Tool for conversion text to occt-shapes
//=======================================================================
static int TextToBRep (Draw_Interpretor& /*theDI*/,
                       Standard_Integer  theArgNb,
                       const char**      theArgVec)
{
  // Check arguments
  if (theArgNb < 3)
  {
    std::cerr << "Error: " << theArgVec[0] << " - invalid syntax\n";
    return 1;
  }

  Standard_Integer anArgIt = 1;
  Standard_CString aName   = theArgVec[anArgIt++];
  Standard_CString aText   = theArgVec[anArgIt++];

  Font_BRepFont           aFont;
  TCollection_AsciiString aFontName ("Courier");
  Standard_Real           aTextHeight        = 16.0;
  Font_FontAspect         aFontAspect        = Font_FA_Regular;
  Standard_Boolean        anIsCompositeCurve = Standard_False;
  gp_Ax3                  aPenAx3    (gp::XOY());
  gp_Dir                  aNormal    (0.0, 0.0, 1.0);
  gp_Dir                  aDirection (1.0, 0.0, 0.0);
  gp_Pnt                  aPenLoc;

  Graphic3d_HorizontalTextAlignment aHJustification = Graphic3d_HTA_LEFT;
  Graphic3d_VerticalTextAlignment   aVJustification = Graphic3d_VTA_BOTTOM;

  for (; anArgIt < theArgNb; ++anArgIt)
  {
    TCollection_AsciiString aParam (theArgVec[anArgIt]);
    aParam.LowerCase();

    if (aParam == "-pos"
     || aParam == "-position")
    {
      if (anArgIt + 3 >= theArgNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      aPenLoc.SetX (Draw::Atof(theArgVec[++anArgIt]));
      aPenLoc.SetY (Draw::Atof(theArgVec[++anArgIt]));
      aPenLoc.SetZ (Draw::Atof(theArgVec[++anArgIt]));
    }
    else if (aParam == "-halign")
    {
      if (++anArgIt >= theArgNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      TCollection_AsciiString aType (theArgVec[anArgIt]);
      aType.LowerCase();
      if (aType == "left")
      {
        aHJustification = Graphic3d_HTA_LEFT;
      }
      else if (aType == "center")
      {
        aHJustification = Graphic3d_HTA_CENTER;
      }
      else if (aType == "right")
      {
        aHJustification = Graphic3d_HTA_RIGHT;
      }
      else
      {
        std::cout << "Error: wrong syntax at '" << aParam.ToCString() << "'.\n";
        return 1;
      }
    }
    else if (aParam == "-valign")
    {
      if (++anArgIt >= theArgNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      TCollection_AsciiString aType (theArgVec[anArgIt]);
      aType.LowerCase();
      if (aType == "top")
      {
        aVJustification = Graphic3d_VTA_TOP;
      }
      else if (aType == "center")
      {
        aVJustification = Graphic3d_VTA_CENTER;
      }
      else if (aType == "bottom")
      {
        aVJustification = Graphic3d_VTA_BOTTOM;
      }
      else if (aType == "topfirstline")
      {
        aVJustification = Graphic3d_VTA_TOPFIRSTLINE;
      }
      else
      {
        std::cout << "Error: wrong syntax at '" << aParam.ToCString() << "'.\n";
        return 1;
      }
    }
    else if (aParam == "-height")
    {
      if (++anArgIt >= theArgNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      aTextHeight = Draw::Atof(theArgVec[anArgIt]);
    }
    else if (aParam == "-aspect")
    {
      if (++anArgIt >= theArgNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      TCollection_AsciiString anOption (theArgVec[anArgIt]);
      anOption.LowerCase();

      if (anOption.IsEqual ("regular"))
      {
        aFontAspect = Font_FA_Regular;
      }
      else if (anOption.IsEqual ("bold"))
      {
        aFontAspect = Font_FA_Bold;
      }
      else if (anOption.IsEqual ("italic"))
      {
        aFontAspect = Font_FA_Italic;
      }
      else if (anOption.IsEqual ("bolditalic"))
      {
        aFontAspect = Font_FA_BoldItalic;
      }
      else
      {
        std::cout << "Error: wrong syntax at '" << aParam.ToCString() << "'.\n";
        return 1;
      }
    }
    else if (aParam == "-font")
    {
      if (++anArgIt >= theArgNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      aFontName = theArgVec[anArgIt];
    }
    else if (aParam == "-composite")
    {
      if (++anArgIt >= theArgNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      ViewerTest::ParseOnOff (theArgVec[anArgIt], anIsCompositeCurve);
    }
    else if (aParam == "-plane")
    {
      if (anArgIt + 6 >= theArgNb)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam.ToCString() << "'.\n";
        return 1;
      }

      Standard_Real aX = Draw::Atof (theArgVec[++anArgIt]);
      Standard_Real aY = Draw::Atof (theArgVec[++anArgIt]);
      Standard_Real aZ = Draw::Atof (theArgVec[++anArgIt]);
      aNormal.SetCoord (aX, aY, aZ);

      aX = Draw::Atof (theArgVec[++anArgIt]);
      aY = Draw::Atof (theArgVec[++anArgIt]);
      aZ = Draw::Atof (theArgVec[++anArgIt]);
      aDirection.SetCoord (aX, aY, aZ);
    }
    else
    {
      std::cerr << "Warning! Unknown argument '" << aParam << "'\n";
    }
  }

  aFont.SetCompositeCurveMode (anIsCompositeCurve);
  if (!aFont.Init (aFontName.ToCString(), aFontAspect, aTextHeight))
  {
    std::cerr << "Font initialization error\n";
    return 1;
  }

  aPenAx3 = gp_Ax3 (aPenLoc, aNormal, aDirection);

  Font_BRepTextBuilder aBuilder;
  DBRep::Set (aName, aBuilder.Perform (aFont, aText, aPenAx3, aHJustification, aVJustification));
  return 0;
}

//=======================================================================
//function : VFont
//purpose  : Font management
//=======================================================================

static int VFont (Draw_Interpretor& theDI,
                  Standard_Integer  theArgNb,
                  const char**      theArgVec)
{
  Handle(Font_FontMgr) aMgr = Font_FontMgr::GetInstance();
  if (theArgNb < 2)
  {
    // just print the list of available fonts
    Standard_Boolean isFirst = Standard_True;
    for (Font_NListOfSystemFont::Iterator anIter (aMgr->GetAvailableFonts());
         anIter.More(); anIter.Next())
    {
      const Handle(Font_SystemFont)& aFont = anIter.Value();
      if (!isFirst)
      {
        theDI << "\n";
      }

      theDI << aFont->FontName()->String()
            << " " << fontStyleString (aFont->FontAspect())
            << " " << aFont->FontPath()->String();
      isFirst = Standard_False;
    }
    return 0;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    const TCollection_AsciiString anArg (theArgVec[anArgIter]);
    TCollection_AsciiString anArgCase (anArg);
    anArgCase.LowerCase();
    if (anArgCase == "find")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg.ToCString() << "'!\n";
        return 1;
      }

      Standard_CString aFontName   = theArgVec[anArgIter];
      Font_FontAspect  aFontAspect = Font_FA_Undefined;
      if (++anArgIter < theArgNb)
      {
        anArgCase = theArgVec[anArgIter];
        anArgCase.LowerCase();
        if (!parseFontStyle (anArgCase, aFontAspect))
        {
          --anArgIter;
        }
      }
      Handle(Font_SystemFont) aFont = aMgr->FindFont (new TCollection_HAsciiString (aFontName), aFontAspect, -1);
      if (aFont.IsNull())
      {
        std::cerr << "Error: font '" << aFontName << "' is not found!\n";
        continue;
      }

      theDI << aFont->FontName()->String()
            << " " << fontStyleString (aFont->FontAspect())
            << " " << aFont->FontPath()->String();
    }
    else if (anArgCase == "add"
          || anArgCase == "register")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg.ToCString() << "'!\n";
        return 1;
      }
      Standard_CString aFontPath   = theArgVec[anArgIter];
      Standard_CString aFontName   = NULL;
      Font_FontAspect  aFontAspect = Font_FA_Undefined;
      if (++anArgIter < theArgNb)
      {
        if (!parseFontStyle (anArgCase, aFontAspect))
        {
          aFontName = theArgVec[anArgIter];
        }
        if (++anArgIter < theArgNb)
        {
          anArgCase = theArgVec[anArgIter];
          anArgCase.LowerCase();
          if (!parseFontStyle (anArgCase, aFontAspect))
          {
            --anArgIter;
          }
        }
      }

      Handle(Font_SystemFont) aFont = aMgr->CheckFont (aFontPath);
      if (aFont.IsNull())
      {
        std::cerr << "Error: font '" << aFontPath << "' is not found!\n";
        continue;
      }

      if (aFontAspect != Font_FA_Undefined
       || aFontName   != NULL)
      {
        if (aFontAspect == Font_FA_Undefined)
        {
          aFontAspect = aFont->FontAspect();
        }
        Handle(TCollection_HAsciiString) aName = aFont->FontName();
        if (aFontName != NULL)
        {
          aName = new TCollection_HAsciiString (aFontName);
        }
        aFont = new Font_SystemFont (aName, aFontAspect, new TCollection_HAsciiString (aFontPath));
      }

      aMgr->RegisterFont (aFont, Standard_True);
      theDI << aFont->FontName()->String()
            << " " << fontStyleString (aFont->FontAspect())
            << " " << aFont->FontPath()->String();
    }
    else
    {
      std::cerr << "Warning! Unknown argument '" << anArg.ToCString() << "'\n";
    }
  }

  return 0;
}

//=======================================================================
//function : VSetEdgeType
//purpose  : Edges type management
//=======================================================================

static int VSetEdgeType (Draw_Interpretor& theDI,
                         Standard_Integer  theArgNum,
                         const char**      theArgs)
{
  if (theArgNum < 4 || theArgNum > 9)
  {
    theDI << theArgs[0] << " error: wrong number of parameters. Type 'help "
          << theArgs[0] << "' for more information.\n";
    return 1;
  }

  Standard_Boolean isForceRedisplay = Standard_False;

  // Get shape name
  TCollection_AsciiString aName(theArgs[1]);
  if (!GetMapOfAIS().IsBound2 (aName))
  {
    theDI <<  theArgs[0] << " error: wrong object name.\n";
    return 1;
  }
  
  Handle(AIS_InteractiveObject) anObject = 
    Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2(aName));
  
  // Enable trianle edge mode
  anObject->Attributes()->ShadingAspect()->Aspect()->SetEdgeOn();

  // Parse parameters
  for (Standard_Integer anIt = 2; anIt < theArgNum; ++anIt)
  {
    TCollection_AsciiString aParam ((theArgs[anIt]));
    if (aParam.Value (1) == '-' && !aParam.IsRealValue())
    {
      if (aParam.IsEqual ("-type"))
      {
        if (theArgNum <= anIt + 1)
        {
          theDI <<  theArgs[0] << " error: wrong number of values for parameter '"
                << aParam.ToCString() << "'.\n";
          return 1;
        }

        TCollection_AsciiString aType = theArgs[++anIt];
        aType.UpperCase();

        if (aType.IsEqual ("SOLID"))
        {
          anObject->Attributes()->ShadingAspect()->Aspect()->SetEdgeLineType(Aspect_TOL_SOLID);
        }
        else if (aType.IsEqual ("DASH"))
        {
          anObject->Attributes()->ShadingAspect()->Aspect()->SetEdgeLineType(Aspect_TOL_DASH);
        }
        else if (aType.IsEqual ("DOT"))
        {
          anObject->Attributes()->ShadingAspect()->Aspect()->SetEdgeLineType(Aspect_TOL_DOT);
        }
        else if (aType.IsEqual ("DOTDASH"))
        {
          anObject->Attributes()->ShadingAspect()->Aspect()->SetEdgeLineType(Aspect_TOL_DOTDASH);
        }
        else
        {
          theDI <<  theArgs[0] << " error: wrong line type: '" << aType.ToCString() << "'.\n";
          return 1;
        }
        
      }
      else if (aParam.IsEqual ("-color"))
      {
        if (theArgNum <= anIt + 3)
        {
          theDI <<  theArgs[0] << " error: wrong number of values for parameter '"
                << aParam.ToCString() << "'.\n";
          return 1;
        }

        Quantity_Parameter aR = Draw::Atof(theArgs[++anIt]);
        Quantity_Parameter aG = Draw::Atof(theArgs[++anIt]);
        Quantity_Parameter aB = Draw::Atof(theArgs[++anIt]);
        Quantity_Color aColor = Quantity_Color (aR > 1 ? aR / 255.0 : aR,
                                                aG > 1 ? aG / 255.0 : aG,
                                                aB > 1 ? aB / 255.0 : aB,
                                                Quantity_TOC_RGB);

        anObject->Attributes()->ShadingAspect()->Aspect()->SetEdgeColor (aColor);
      }
      else if (aParam.IsEqual ("-force"))
      {
        isForceRedisplay = Standard_True;
      }
      else
      {
        theDI <<  theArgs[0] << " error: unknown parameter '"
              << aParam.ToCString() << "'.\n";
        return 1;
      }
    }
  }

  // Update shape presentation as aspect parameters were changed
  if (isForceRedisplay)
  {
    ViewerTest::GetAISContext()->Redisplay (anObject);
  }
  else
  {
    anObject->SetAspect (anObject->Attributes()->ShadingAspect());
  }

  //Update view
  ViewerTest::CurrentView()->Redraw();

  return 0;
}

//=======================================================================
//function : VUnsetEdgeType
//purpose  : Unsets edges visibility in shading mode
//=======================================================================

static int VUnsetEdgeType (Draw_Interpretor& theDI,
                         Standard_Integer  theArgNum,
                         const char**      theArgs)
{
  if (theArgNum != 2 && theArgNum != 3)
  {
    theDI << theArgs[0] << " error: wrong number of parameters. Type 'help "
          << theArgs[0] << "' for more information.\n";
    return 1;
  }

  Standard_Boolean isForceRedisplay = Standard_False;

  // Get shape name
  TCollection_AsciiString aName (theArgs[1]);
  if (!GetMapOfAIS().IsBound2 (aName))
  {
    theDI <<  theArgs[0] << " error: wrong object name.\n";
    return 1;
  }

  Handle(AIS_InteractiveObject) anObject = 
    Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(aName));

  // Enable trianle edge mode
  anObject->Attributes()->ShadingAspect()->Aspect()->SetEdgeOff();

  // Parse parameters
  if (theArgNum == 3)
  {
    TCollection_AsciiString aParam ((theArgs[2]));
    if (aParam.IsEqual ("-force"))
    {
      isForceRedisplay = Standard_True;
    }
    else
    {
       theDI <<  theArgs[0] << " error: unknown parameter '"
              << aParam.ToCString() << "'.\n";
       return 1;
    }
  }

  // Update shape presentation as aspect parameters were changed
  if (isForceRedisplay)
  {
    ViewerTest::GetAISContext()->Redisplay (anObject);
  }
  else
  {
    anObject->SetAspect (anObject->Attributes()->ShadingAspect());
  }

  //Update view
  ViewerTest::CurrentView()->Redraw();

  return 0;
}


//=======================================================================
//function : VVertexMode
//purpose  : Switches vertex display mode for AIS_Shape or displays the current value
//=======================================================================

static int VVertexMode (Draw_Interpretor& theDI,
                         Standard_Integer  theArgNum,
                         const char**      theArgs)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    std::cout << "Error: no view available, call 'vinit' before!" << std::endl;
    return 1;
  }

  // No arguments --> print the current default vertex draw mode
  if (theArgNum == 1)
  {
    Prs3d_VertexDrawMode aCurrMode = aContext->DefaultDrawer()->VertexDrawMode();
    theDI <<  "Default vertex draw mode: " << (aCurrMode == Prs3d_VDM_Isolated ? "'isolated'" : "'all'") << "\n";
    return 0;
  }

  // -set argument --> change the default vertex draw mode and the mode for all displayed or given object(s)
  TCollection_AsciiString aParam (theArgs[1]);
  if (aParam == "-set")
  {
    if (theArgNum == 2)
    {
      std::cout << "Error: '-set' option not followed by the mode and optional object name(s)" << std::endl;
      std::cout << "Type 'help vvertexmode' for usage hints" << std::endl;
      return 1;
    }

    TCollection_AsciiString aModeStr (theArgs[2]);
    Prs3d_VertexDrawMode aNewMode =
       aModeStr == "isolated" ? Prs3d_VDM_Isolated :
      (aModeStr == "all"      ? Prs3d_VDM_All :
                                Prs3d_VDM_Inherited);

    Standard_Boolean aRedrawNeeded = Standard_False;
    AIS_ListOfInteractive anObjs;

    // No object(s) specified -> use all displayed
    if (theArgNum == 3)
    {
      theDI << "Setting the default vertex draw mode and updating all displayed objects...\n";
      aContext->DisplayedObjects (anObjs);
      aContext->DefaultDrawer()->SetVertexDrawMode (aNewMode);
      aRedrawNeeded = Standard_True;
    }

    Handle(AIS_InteractiveObject) anObject;
    for (Standard_Integer aCount = 3; aCount < theArgNum; aCount++)
    {
      TCollection_AsciiString aName (theArgs[aCount]);
      if (!GetMapOfAIS().IsBound2 (aName))
      {
        theDI << "Warning: wrong object name ignored - " << theArgs[0] << "\n";
        continue;
      }
      anObject = Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(aName));
      anObjs.Append (anObject);
    }

    for (AIS_ListIteratorOfListOfInteractive anIt (anObjs); anIt.More(); anIt.Next())
    {
      anObject = anIt.Value();
      anObject->Attributes()->SetVertexDrawMode (aNewMode);
      aContext->Redisplay (anObject, Standard_False);
      aRedrawNeeded = Standard_True;
    }

    if (aRedrawNeeded)
      ViewerTest::CurrentView()->Redraw();

    return 0;
  }

  if (theArgNum > 2)
  {
    std::cout << "Error: invalid number of arguments" << std::endl;
    std::cout << "Type 'help vvertexmode' for usage hints" << std::endl;
    return 1;
  }

  // One argument (object name) --> print the current vertex draw mode for the object
  Handle(AIS_InteractiveObject) anObject =
    Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (aParam));
  Prs3d_VertexDrawMode aCurrMode = anObject->Attributes()->VertexDrawMode();
  theDI <<  "Object's vertex draw mode: " << (aCurrMode == Prs3d_VDM_Isolated ? "'isolated'" : "'all'") << "\n";
  return 0;
}

//=======================================================================
//function : VPointCloud
//purpose  : Create interactive object for arbitary set of points.
//=======================================================================
static Standard_Integer VPointCloud (Draw_Interpretor& theDI,
                                     Standard_Integer  theArgNum,
                                     const char**      theArgs)
{
  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    std::cerr << "Error: no active view!\n";
    return 1;
  }

  // command to execute
  enum Command
  {
    CloudForShape, // generate point cloud for shape
    CloudSphere,   // generate point cloud for generic sphere
    Unknow
  };

  // count number of non-optional command arguments
  Command aCmd = Unknow;
  Standard_Integer aCmdArgs = 0;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNum; ++anArgIter)
  {
    Standard_CString anArg = theArgs[anArgIter];
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (aFlag.IsRealValue() || aFlag.Search ("-") != 1)
    {
      aCmdArgs++;
    }
  }
  switch (aCmdArgs)
  {
    case 2  : aCmd = CloudForShape; break;
    case 7  : aCmd = CloudSphere; break;
    default :
      std::cout << "Error: wrong number of arguments! See usage:\n";
      theDI.PrintHelp (theArgs[0]);
      return 1;
  }

  // parse options
  Standard_Boolean toRandColors = Standard_False;
  Standard_Boolean hasNormals   = Standard_True;
  Standard_Boolean isSetArgNorm = Standard_False;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNum; ++anArgIter)
  {
    Standard_CString anArg = theArgs[anArgIter];
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (aFlag == "-randcolors"
     || aFlag == "-randcolor")
    {
      if (isSetArgNorm && hasNormals)
      {
        std::cout << "Error: wrong syntax - normals can not be enabled with colors at the same time\n";
        return 1;
      }
      toRandColors = Standard_True;
      hasNormals   = Standard_False;
    }
    else if (aFlag == "-normals"
          || aFlag == "-normal")
    {
      if (toRandColors)
      {
        std::cout << "Error: wrong syntax - normals can not be enabled with colors at the same time\n";
        return 1;
      }
      isSetArgNorm = Standard_True;
      hasNormals   = Standard_True;
    }
    else if (aFlag == "-nonormals"
          || aFlag == "-nonormal")
    {
      isSetArgNorm = Standard_True;
      hasNormals   = Standard_False;
    }
  }

  Standard_CString aName = theArgs[1];

  // generate arbitrary set of points
  Handle(Graphic3d_ArrayOfPoints) anArrayPoints;
  if (aCmd == CloudForShape)
  {
    Standard_CString aShapeName = theArgs[2];
    TopoDS_Shape     aShape     = DBRep::Get (aShapeName);

    if (aShape.IsNull())
    {
      std::cout << "Error: no shape with name '" << aShapeName << "' found\n";
      return 1;
    }

    // calculate number of points
    TopLoc_Location  aLocation;
    Standard_Integer aNbPoints = 0;
    for (TopExp_Explorer aFaceIt (aShape, TopAbs_FACE); aFaceIt.More(); aFaceIt.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face (aFaceIt.Current());
      Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation (aFace, aLocation);
      if (!aTriangulation.IsNull())
      {
        aNbPoints += aTriangulation->NbNodes();
      }
    }
    if (aNbPoints < 3)
    {
      std::cout << "Error: shape should be triangulated!\n";
      return 1;
    }

    anArrayPoints = new Graphic3d_ArrayOfPoints (aNbPoints, toRandColors, hasNormals);
    for (TopExp_Explorer aFaceIt (aShape, TopAbs_FACE); aFaceIt.More(); aFaceIt.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face (aFaceIt.Current());
      Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation (aFace, aLocation);
      if (aTriangulation.IsNull())
      {
        continue;
      }

      const TColgp_Array1OfPnt& aNodes = aTriangulation->Nodes();
      const gp_Trsf&            aTrsf  = aLocation.Transformation();

      // extract normals from nodes
      TColgp_Array1OfDir aNormals (aNodes.Lower(), hasNormals ? aNodes.Upper() : aNodes.Lower());
      if (hasNormals)
      {
        Poly_Connect aPolyConnect (aTriangulation);
        StdPrs_ToolTriangulatedShape::Normal (aFace, aPolyConnect, aNormals);
      }

      for (Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
      {
        gp_Pnt aPoint = aNodes (aNodeIter);
        if (!aLocation.IsIdentity())
        {
          aPoint.Transform (aTrsf);
          if (hasNormals)
          {
            aNormals (aNodeIter).Transform (aTrsf);
          }
        }

        // add vertex into array of points
        const Standard_Integer anIndexOfPoint = anArrayPoints->AddVertex (aPoint);
        if (toRandColors)
        {
          Quantity_Color aColor (360.0 * Standard_Real(anIndexOfPoint) / Standard_Real(aNbPoints),
                                 1.0, 0.5, Quantity_TOC_HLS);
          anArrayPoints->SetVertexColor (anIndexOfPoint, aColor);
        }

        if (hasNormals)
        {
          anArrayPoints->SetVertexNormal (anIndexOfPoint, aNormals (aNodeIter));
        }
      }
    }
  }
  else if (aCmd == CloudSphere)
  {
    Standard_Real aCenterX       = Draw::Atof (theArgs[2]);
    Standard_Real aCenterY       = Draw::Atof (theArgs[3]);
    Standard_Real aCenterZ       = Draw::Atof (theArgs[4]);
    Standard_Real aRadius        = Draw::Atof (theArgs[5]);
    Standard_Integer aNbPoints   = Draw::Atoi (theArgs[6]);

    TCollection_AsciiString aDistribution = TCollection_AsciiString(theArgs[7]);
    aDistribution.LowerCase();
    if ( aDistribution != "surface" && aDistribution != "volume" )
    {
      std::cout << "Error: wrong arguments! See usage:\n";
      theDI.PrintHelp (theArgs[0]);
      return 1;
    }
    Standard_Boolean isSurface = aDistribution == "surface";

    gp_Pnt aCenter(aCenterX, aCenterY, aCenterZ);

    anArrayPoints = new Graphic3d_ArrayOfPoints (aNbPoints, toRandColors, hasNormals);
    for (Standard_Integer aPntIt = 0; aPntIt < aNbPoints; ++aPntIt)
    {
      Standard_Real anAlpha   = (Standard_Real (rand() % 2000) / 1000.0) * M_PI;
      Standard_Real aBeta     = (Standard_Real (rand() % 2000) / 1000.0) * M_PI;
      Standard_Real aDistance = isSurface ?
        aRadius : (Standard_Real (rand() % aNbPoints) / aNbPoints) * aRadius;

      gp_Dir aDir (Cos (anAlpha) * Sin (aBeta),
                   Sin (anAlpha),
                   Cos (anAlpha) * Cos (aBeta));
      gp_Pnt aPoint = aCenter.Translated (aDir.XYZ() * aDistance);

      const Standard_Integer anIndexOfPoint = anArrayPoints->AddVertex (aPoint);
      if (toRandColors)
      {
        Quantity_Color aColor (360.0 * Standard_Real (anIndexOfPoint) / Standard_Real (aNbPoints),
                               1.0, 0.5, Quantity_TOC_HLS);
        anArrayPoints->SetVertexColor (anIndexOfPoint, aColor);
      }

      if (hasNormals)
      {
        anArrayPoints->SetVertexNormal (anIndexOfPoint, aDir);
      }
    }
  }

  // set array of points in point cloud object
  Handle(AIS_PointCloud) aPointCloud = new AIS_PointCloud();
  aPointCloud->SetPoints (anArrayPoints);
  VDisplayAISObject (aName, aPointCloud);
  return 0;
}

//=======================================================================
//function : VPriority
//purpose  : Prints or sets the display priority for an object
//=======================================================================

static int VPriority (Draw_Interpretor& theDI,
                      Standard_Integer  theArgNum,
                      const char**      theArgs)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aContext, ViewerTest::CurrentView());
  if (aContext.IsNull())
  {
    std::cout << "Error: no view available, call 'vinit' before!" << std::endl;
    return 1;
  }

  TCollection_AsciiString aLastArg (theArgs[theArgNum - 1]);
  Standard_Integer aPriority = -1;
  Standard_Integer aNbArgs   = theArgNum;
  if (aLastArg.IsIntegerValue())
  {
    aPriority = aLastArg.IntegerValue();
    --aNbArgs;
    if (aPriority < 0 || aPriority > 10)
    {
      std::cout << "Error: the specified display priority value '" << aLastArg
                << "' is outside the valid range [0..10]" << std::endl;
      return 1;
    }
  }
  else
  {
    anUpdateTool.Invalidate();
  }

  if (aNbArgs < 2)
  {
    std::cout << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgs[0]);
    return 1;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < aNbArgs; ++anArgIter)
  {
    if (anUpdateTool.parseRedrawMode (theArgs[anArgIter]))
    {
      continue;
    }

    TCollection_AsciiString aName (theArgs[anArgIter]);
    Handle(AIS_InteractiveObject) anIObj;
    if (GetMapOfAIS().IsBound2 (aName))
    {
      anIObj = Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (aName));
    }

    if (anIObj.IsNull())
    {
      std::cout << "Error: the object '" << theArgs[1] << "' is not displayed" << std::endl;
      return 1;
    }

    if (aPriority < 1)
    {
      theDI << aContext->DisplayPriority (anIObj) << " ";
    }
    else
    {
      aContext->SetDisplayPriority (anIObj, aPriority);
    }
  }
  return 0;
}

//=======================================================================
//function : ObjectsCommands
//purpose  :
//=======================================================================

void ViewerTest::ObjectCommands(Draw_Interpretor& theCommands)
{
  const char *group ="AISObjects";
  theCommands.Add("vtrihedron",
    "vtrihedron         : vtrihedron name [ -origin x y z ] [ -zaxis u v w -xaxis u v w ] [ -hidelabels ]"
    "\n\t\t: Creates a new *AIS_Trihedron* object. If no argument is set, the default trihedron (0XYZ) is created."
     "\n\t\t: -hidelabels allows to draw trihedron without axes labels. By default, they are displayed.",
    __FILE__,VTrihedron,group);

  theCommands.Add("vtri2d",
    "vtri2d Name"
    "\n\t\t: Creates a plane with a 2D trihedron from an interactively selected face.",
    __FILE__,VTrihedron2D ,group);

  theCommands.Add("vplanetri",
    "vplanetri name"
    "\n\t\t: Create a plane from a trihedron selection. If no arguments are set, the default",
    __FILE__,VPlaneTrihedron ,group);

  theCommands.Add("vsize",
    "vsize       : vsize [name(Default=Current)] [size(Default=100)] "
    "\n\t\t: Changes the size of a named or selected trihedron."
    "\n\t\t: If the name is not defined: it affects the selected trihedrons otherwise nothing is done."
    "\n\t\t: If the value is not defined: it is set to 100 by default.",
    __FILE__,VSize,group);

  theCommands.Add("vaxis",
    "vaxis name [Xa] [Ya] [Za] [Xb] [Yb] [Zb]"
    "\n\t\t: Creates an axis. If  the values are not defined, an axis is created by interactive selection of two vertices or one edge",
    __FILE__,VAxisBuilder,group);

  theCommands.Add("vaxispara",
    "vaxispara name "
    "\n\t\t: Creates an axis by interactive selection of an edge and a vertex.",
    __FILE__,VAxisBuilder,group);

  theCommands.Add("vaxisortho",
    "vaxisortho name "
    "\n\t\t: Creates an axis by interactive selection of an edge and a vertex. The axis will be orthogonal to the selected edge.",
    __FILE__,VAxisBuilder,group);

  theCommands.Add("vpoint",
    "vpoint  PointName [Xa] [Ya] [Za] "
    "\n\t\t: Creates a point from coordinates. If the values are not defined,"
    "\n\t\t: a point is created by interactive selection of a vertice or an edge (in the center of the edge).",
    __FILE__,VPointBuilder,group);

  theCommands.Add("vplane",
    "vplane  PlaneName [AxisName/PlaneName/PointName] [PointName/PointName/PointName] [Nothing/Nothing/PointName] [TypeOfSensitivity {0|1}]"
    "\n\t\t: Creates a plane from named or interactively selected entities."
    "\n\t\t: TypeOfSensitivity:"
    "\n\t\t:   0 - Interior"
    "\n\t\t:   1 - Boundary",
    __FILE__,VPlaneBuilder,group);

  theCommands.Add ("vchangeplane", "vchangeplane usage: \n"
    "   vchangeplane <plane_name>"
    " [x=center_x y=center_y z=center_z]"
    " [dx=dir_x dy=dir_y dz=dir_z]"
    " [sx=size_x sy=size_y]"
    " [noupdate]\n"
    "   - changes parameters of the plane:\n"
    "   - x y z     - center\n"
    "   - dx dy dz  - normal\n"
    "   - sx sy     - plane sizes\n"
    "   - noupdate  - do not update/redisplay the plane in context\n"
    "   Please enter coordinates in format \"param=value\" in arbitrary order.",
    __FILE__, VChangePlane, group);

  theCommands.Add("vplanepara",
    "vplanepara  PlaneName  "
    "\n\t\t: Creates a plane from interactively selected vertex and face.",
    __FILE__,VPlaneBuilder,group);

  theCommands.Add("vplaneortho",
    "vplaneortho  PlaneName  "
    "\n\t\t: Creates a plane from interactive selected face and coplanar edge. ",
    __FILE__,VPlaneBuilder,group);

  theCommands.Add("vline",
    "vline LineName [Xa/PointName] [Ya/PointName] [Za] [Xb] [Yb] [Zb]  "
    "\n\t\t: Creates a line from coordinates, named or interactively selected vertices. ",
    __FILE__,VLineBuilder,group);

  theCommands.Add("vcircle",
    "vcircle CircleName [PointName PointName PointName IsFilled]\n\t\t\t\t\t[PlaneName PointName Radius IsFilled]"
    "\n\t\t: Creates a circle from named or interactively selected entities."
    "\n\t\t: Parameter IsFilled is defined as 0 or 1.",
    __FILE__,VCircleBuilder,group);

  theCommands.Add ("vdrawtext",
                   "vdrawtext name text"
                   "\n\t\t: [-pos X=0 Y=0 Z=0]"
                   "\n\t\t: [-color {R G B|name}=yellow]"
                   "\n\t\t: [-halign {left|center|right}=left]"
                   "\n\t\t: [-valign {top|center|bottom|topfirstline}=bottom}]"
                   "\n\t\t: [-angle angle=0]"
                   "\n\t\t: [-zoom {0|1}=0]"
                   "\n\t\t: [-height height=16]"
                   "\n\t\t: [-aspect {regular|bold|italic|bolditalic}=regular]"
                   "\n\t\t: [-font font=Times]"
                   "\n\t\t: [-2d]"
                   "\n\t\t: [-perspos {X Y Z}=0 0 0], where"
                   "\n\t\t X and Y define the coordinate origin in 2d space relative to the view window"
                   "\n\t\t Example: X=0 Y=0 is center, X=1 Y=1 is upper right corner etc..."
                   "\n\t\t Z coordinate defines the gap from border of view window (except center position)."
                   "\n\t\t: [-disptype {blend|decal|subtitle|dimension|normal}=normal}"
                   "\n\t\t: [-subcolor {R G B|name}=white]"
                   "\n\t\t: [-noupdate]"
                   "\n\t\t: [-plane NormX NormY NormZ DirX DirY DirZ]"
                   "\n\t\t: Display text label at specified position.",
    __FILE__, VDrawText, group);

  theCommands.Add("vdrawsphere",
    "vdrawsphere: vdrawsphere shapeName Fineness [X=0.0 Y=0.0 Z=0.0] [Radius=100.0] [ToShowEdges=0] [ToPrintInfo=1]\n",
    __FILE__,VDrawSphere,group);

  theCommands.Add ("vsetlocation",
                   "vsetlocation [-noupdate|-update] name x y z"
                   "\n\t\t: Set new location for an interactive object.",
        __FILE__, VSetLocation, group);

  theCommands.Add (
    "vcomputehlr",
    "vcomputehlr: shape hlrname [ eyex eyey eyez lookx looky lookz ]",
    __FILE__, VComputeHLR, group);

  theCommands.Add("vdrawparray",
    "vdrawparray : vdrawparray Name TypeOfArray [vertex = { 'v' x y z [vertex_normal = { 'n' x y z }] [vertex_color = { 'c' r g b }] ] ... [bound = { 'b' vertex_count [bound_color = { 'c' r g b }] ] ... [edge = { 'e' vertex_id ]",
    __FILE__,VDrawPArray,group);

  theCommands.Add("vconnect", 
    "vconnect name Xo Yo Zo object1 object2 ... [color=NAME]"
    "\n\t\t: Creates and displays AIS_ConnectedInteractive object from input object and location.",
    __FILE__, VConnect, group);

  theCommands.Add("vconnectto",
    "vconnectto : instance_name Xo Yo Zo object [-nodisplay|-noupdate|-update]"
    "  Makes an instance 'instance_name' of 'object' with position (Xo Yo Zo)."
    "\n\t\t:   -nodisplay - only creates interactive object, but not displays it",
    __FILE__, VConnectTo,group);

  theCommands.Add("vdisconnect",
    "vdisconnect assembly_name (object_name | object_number | 'all')"
    "  Disconnects all objects from assembly or disconnects object by name or number (use vlistconnected to enumerate assembly children).",
    __FILE__,VDisconnect,group);

  theCommands.Add("vaddconnected",
    "vaddconnected assembly_name object_name"
    "Adds object to assembly.",
    __FILE__,VAddConnected,group);

  theCommands.Add("vlistconnected",
    "vlistconnected assembly_name"
    "Lists objects in assembly.",
    __FILE__,VListConnected,group);


  theCommands.Add("vselmode", 
    "vselmode : [object] mode_number is_turned_on=(1|0)\n"
    "  switches selection mode for the determined object or\n"
    "  for all objects in context.\n"
    "  mode_number is non-negative integer that has different\n"
    "    meaning for different interactive object classes.\n"
    "    For shapes the following mode_number values are allowed:\n"
    "      0 - shape\n"
    "      1 - vertex\n"
    "      2 - edge\n"
    "      3 - wire\n"
    "      4 - face\n"
    "      5 - shell\n"
    "      6 - solid\n"
    "      7 - compsolid\n"
    "      8 - compound\n"
    "  is_turned_on is:\n"
    "    1 if mode is to be switched on\n"
    "    0 if mode is to be switched off\n", 
    __FILE__, VSetSelectionMode, group);

  theCommands.Add("vselnext",
    "vselnext : hilight next detected",
    __FILE__, VSelectionNext, group);

  theCommands.Add("vselprev",
    "vselnext : hilight previous detected",
    __FILE__, VSelectionPrevious, group);

  theCommands.Add("vtriangle",
    "vtriangle Name PointName PointName PointName"
    "\n\t\t: Creates and displays a filled triangle from named points.", 
    __FILE__, VTriangle,group);

  theCommands.Add("vsegment",
    "vsegment Name PointName PointName"
    "\n\t\t: Creates and displays a segment from named points.", 
    __FILE__, VSegment,group);

  theCommands.Add("vobjzlayer",
    "vobjzlayer : set/get object [layerid] - set or get z layer id for the interactive object",
    __FILE__, VObjZLayer, group);
  
  theCommands.Add("vpolygonoffset",
    "vpolygonoffset : [object [mode factor units]] - sets/gets polygon offset parameters for an object, without arguments prints the default values",
    __FILE__, VPolygonOffset, group);

  theCommands.Add ("vshowfaceboundary",
    "vshowfaceboundary : ObjectName isOn (1/0) [R G B [LineWidth [LineStyle]]]"
    "- turns on/off drawing of face boundaries for ais object "
    "and defines boundary line style.",
    __FILE__, VShowFaceBoundary, group);

  theCommands.Add ("vmarkerstest",
                   "vmarkerstest: name X Y Z [PointsOnSide=10] [MarkerType=0] [Scale=1.0] [FileName=ImageFile]\n",
                   __FILE__, VMarkersTest, group);

  theCommands.Add ("text2brep",
                   "text2brep: name text"
                   "\n\t\t: [-pos X=0 Y=0 Z=0]"
                   "\n\t\t: [-halign {left|center|right}=left]"
                   "\n\t\t: [-valign {top|center|bottom|topfirstline}=bottom}]"
                   "\n\t\t: [-height height=16]"
                   "\n\t\t: [-aspect {regular|bold|italic|bolditalic}=regular]"
                   "\n\t\t: [-font font=Courier]"
                   "\n\t\t: [-composite {on|off}=off]"
                   "\n\t\t: [-plane NormX NormY NormZ DirX DirY DirZ]",
                   __FILE__, TextToBRep, group);
  theCommands.Add ("vfont",
                            "vfont [add pathToFont [fontName] [regular,bold,italic,bolditalic=undefined]]"
                   "\n\t\t:        [find fontName [regular,bold,italic,bolditalic=undefined]]",
                   __FILE__, VFont, group);
  
  theCommands.Add ("vsetedgetype",
                   "vsetedgetype usage:\n"
                   "vsetedgetype ShapeName [-force] [-type {solid, dash, dot}] [-color R G B] "
                   "\n\t\t:        Sets edges type and color for input shape",
                   __FILE__, VSetEdgeType, group);

  theCommands.Add ("vunsetedgetype",
                   "vunsetedgetype usage:\n"
                   "vunsetedgetype ShapeName [-force]"
                   "\n\t\t:        Unsets edges type and color for input shape",
                   __FILE__, VUnsetEdgeType, group);

  theCommands.Add ("vvertexmode",
                   "vvertexmode [name | -set {isolated | all | inherited} [name1 name2 ...]]\n"
                   "vvertexmode - prints the default vertex draw mode\n"
                   "vvertexmode name - prints the vertex draw mode of the given object\n"
                   "vvertexmode -set {isolated | all | inherited} - sets the default vertex draw mode and updates the mode for all displayed objects\n"
                   "vvertexmode -set {isolated | all | inherited} name1 name2 ... - sets the vertex draw mode for the specified object(s)\n",
                   __FILE__, VVertexMode, group);

  theCommands.Add ("vpointcloud",
                   "vpointcloud name shape [-randColor] [-normals] [-noNormals]"
                   "\n\t\t: Create an interactive object for arbitary set of points"
                   "\n\t\t: from triangulated shape."
                   "\n"
                   "vpointcloud name x y z r npts {surface|volume}\n"
                   "            ... [-randColor] [-normals] [-noNormals]"
                   "\n\t\t: Create arbitrary set of points (npts) randomly distributed"
                   "\n\t\t: on spheric surface or within spheric volume (x y z r)."
                   "\n\t\t:"
                   "\n\t\t: Additional options:"
                   "\n\t\t:  -randColor - generate random color per point"
                   "\n\t\t:  -normals   - generate normal per point (default)"
                   "\n\t\t:  -noNormals - do not generate normal per point"
                   "\n",
                   __FILE__, VPointCloud, group);

  theCommands.Add("vlocreset",
    "vlocreset name1 name2 ...\n\t\t  remove object local transformation",
    __FILE__,
    LocalTransformPresentation, group);

  theCommands.Add("vlocmove",
    "vlocmove name1 name2 ... name\n\t\t  set local transform to match transform of 'name'",
    __FILE__,
    LocalTransformPresentation, group);

  theCommands.Add("vloctranslate",
    "vloctranslate name1 name2 ... dx dy dz\n\t\t  applies translation to local transformation",
    __FILE__,
    LocalTransformPresentation, group);

  theCommands.Add("vlocrotate",
    "vlocrotate name1 name2 ... x y z dx dy dz angle\n\t\t  applies rotation to local transformation",
    __FILE__,
    LocalTransformPresentation, group);

  theCommands.Add("vlocmirror",
    "vlocmirror name x y z dx dy dz\n\t\t  applies mirror to local transformation",
    __FILE__,
    LocalTransformPresentation, group);

  theCommands.Add("vlocscale",
    "vlocscale name x y z scale\n\t\t  applies scale to local transformation",
    __FILE__,
    LocalTransformPresentation, group);

  theCommands.Add("vpriority",
    "vpriority [-noupdate|-update] name [value]\n\t\t  prints or sets the display priority for an object",
    __FILE__,
    VPriority, group);
}
