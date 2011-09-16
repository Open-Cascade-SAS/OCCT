// File:      ViewerTest_ObjectsCommands.cxx
// Created:   Thu Nov 12 15:50:42 1998
// Author:    Robert COUBLANC
// Copyright: OPEN CASCADE 1998


//===============================================
//
//    AIS Objects Creation : Datums (axis,trihedrons,lines,planes)
//
//===============================================

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ViewerTest.hxx>

#include <string.h>

#include <Quantity_NameOfColor.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <DBRep.hxx>

#include <OSD_Chronometer.hxx>
#include <TCollection_AsciiString.hxx>
#include <Visual3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <V3d_Plane.hxx>
#include <V3d.hxx>

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
#include <AIS_AngleDimension.hxx>
#include <TCollection_ExtendedString.hxx>
#include <GC_MakePlane.hxx>
#include <gp_Circ.hxx>
#include <AIS_Axis.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Axis1Placement.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_Axis.hxx>

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
#include <Graphic3d_Group.hxx>
#include <Standard_Real.hxx>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef WNT
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning (disable:4996)
#endif

extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
extern Standard_Boolean VDisplayAISObject (const TCollection_AsciiString& theName,
                                           const Handle(AIS_InteractiveObject)& theAISObj,
                                           Standard_Boolean theReplaceIfExists = Standard_True);
Standard_IMPORT int ViewerMainLoop(Standard_Integer argc, const char** argv);
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
  if ( argc!=2) {di<<argv[0]<<" error"<<"\n"; return 1;}

  // Declarations
  Standard_Integer myCurrentIndex;
  // Fermeture des contextes
  TheAISContext()->CloseAllContexts();
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  // On active les modes de selections faces.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
  di<<" Select a face ."<<"\n";

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
//author   : ege
//purpose  : Create a trihedron. If no arguments are set, the default
//           trihedron (Oxyz) is created.
//Draw arg : vtrihedron  name  [Xo] [Yo] [Zo] [Zu] [Zv] [Zw] [Xu] [Xv] [Xw]
//==============================================================================

static int VTrihedron (Draw_Interpretor& di, Standard_Integer argc, const char** argv)

{
  // Verification des arguments
  if ( argc<2 || argc>11) {di<<argv[0]<<" Syntaxe error"<<"\n"; return 1;}

  // Declarations et creation des objets par default
  TCollection_AsciiString     name=argv[1];

  if(argc > 5 && argc!=11)
  {di<<argv[0]<<" Syntaxe error"<<"\n"; return 1;}

  // Cas ou il y a des arguments
  Standard_Real coord[9]={0.,0.,0.,0.,0.,1.,1.,0.,0.};
  if (argc>2){
    Standard_Integer i ;
    for( i=0;i<=2;i++)
      coord[i]= atof(argv[2+i]);

    if(argc>5){
      for(i=0;i<=2;i++){
        coord[3+i] = atof(argv[6+i]);
        coord[6+i] = atof(argv[8+i]);
      }
    }
  }
  gp_Pnt ThePoint(coord[0],coord[1],coord[2]);
  gp_Dir TheZVector(coord[3],coord[4],coord[5]);
  gp_Dir TheXVector(coord[6],coord[7],coord[8]);

  if ( !TheZVector.IsNormal(TheXVector,PI/180)) {di<<argv[0]<<" VectorX is not normal to VectorZ"<<"\n"; return 1;}

  Handle(Geom_Axis2Placement) OrigineAndAxii=new Geom_Axis2Placement(ThePoint,TheZVector,TheXVector);

  // Creation du triedre
  Handle(AIS_Trihedron) aShape= new AIS_Trihedron(OrigineAndAxii);
  GetMapOfAIS().Bind(aShape,name);
  TheAISContext()->Display(aShape);

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
#ifdef DEB
  Quantity_NameOfColor         col;
#else
  Quantity_NameOfColor         col = Quantity_NOC_BLACK ;
#endif

  // Verification des arguments
  if ( argc>3 ) {di<<argv[0]<<" Syntaxe error"<<"\n"; return 1;}

  // Verification du nombre d'arguments
  if (argc==1)      {ThereIsName=Standard_False;value=100;}
  else if (argc==2) {ThereIsName=Standard_False;value=atof(argv[1]);}
  else              {ThereIsName=Standard_True;value=atof(argv[2]);}

  // On ferme le contexte local pour travailler dans le contexte global
  if(TheAISContext()->HasOpenedContext())
    TheAISContext()->CloseLocalContext();

  // On set le booleen ThereIsCurrent
  if (TheAISContext() -> NbCurrents() > 0) {ThereIsCurrent=Standard_True;}
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

      if (!aShape.IsNull() &&  TheAISContext()->IsCurrent(aShape) )
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
          Handle(AIS_Trihedron) aTrihedron = *(Handle(AIS_Trihedron)*) &aShape;

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
        Handle(AIS_Trihedron) aTrihedron = *(Handle(AIS_Trihedron)*) &aShape;

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
  if ( argc!=2) {di<<argv[0]<<" error"<<"\n"; return 1;}

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
        Handle(AIS_Trihedron) TrihedronA =((*(Handle(AIS_Trihedron)*)&ShapeA));
        // on le charge dans le contexte et on active le mode Plane.
        TheAISContext()->Load(TrihedronA,0,Standard_False);
        TheAISContext()->Activate(TrihedronA,3);
      }
      it.Next();
  }

  di<<" Select a plane."<<"\n";
  // Boucle d'attente waitpick.
  Standard_Integer argccc = 5;
  const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argvvv = (const char **) bufff;
  while (ViewerMainLoop( argccc, argvvv) ) { }
  // fin de la boucle

  Handle(AIS_InteractiveObject) theIOB;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    theIOB = TheAISContext()->Interactive();
  }
  // on le downcast
  Handle(AIS_Plane) PlaneB =((*(Handle(AIS_Plane)*)&theIOB));

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
  if (argc<2 || argc>8 ) {di<<" Syntaxe error"<<"\n";return 1;}
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
      coord[i]=atof(argv[2+i]);
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
      di<<" Select an edge or a vertex."<<"\n";

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
        di<<" Select a different vertex."<<"\n";

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
      di<<" Select an edge."<<"\n";

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
      di<<" Select a vertex."<<"\n";

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
      di<<" Select an edge."<<"\n";

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
      di<<" Slect a vertex."<<"\n";

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
  if (argc<2 || argc>5 ) {di<<" Syntaxe error"<<"\n";return 1;}
  if (argc==5) HasArg=Standard_True;
  else HasArg=Standard_False;

  name=argv[1];
  // Fermeture des contextes
  TheAISContext()->CloseAllContexts();

  // Il y a des arguments: teste l'unique constructeur AIS_Pnt::AIS_Pnt(Point from Geom)
  if (HasArg) {
    Standard_Real thecoord[3];
    for(Standard_Integer i=0;i<=2;i++)
      thecoord[i]=atof(argv[2+i]);
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
    di<<" Select a vertex or an edge(build the middle)"<<"\n";

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
// Fonction        1st click   2de click  3de click
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
//purpose  : Build an AIS_Plane from selected entities or Named AIs components
//Draw arg : vplane PlaneName [AxisName]  [PointName]
//                            [PointName] [PointName] [PointName]
//                            [PlaneName] [PointName]
//==============================================================================

#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp.hxx>
#include <AIS_Plane.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Plane.hxx>
#include <BRepExtrema_ExtPC.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <gp_Pln.hxx>
#include <GC_MakePlane.hxx>

static int VPlaneBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  // Declarations
  Standard_Boolean HasArg;
  TCollection_AsciiString name;
  Standard_Integer myCurrentIndex;

  // Verification
  if (argc<2 || argc>5 ) {di<<" Syntaxe error"<<"\n";return 1;}
  if (argc==5 || argc==4) HasArg=Standard_True;
  else HasArg=Standard_False;

  name=argv[1];
  // Fermeture des contextes
  TheAISContext()->CloseAllContexts();


  // Il y a des arguments
  if (HasArg) {
    if (!GetMapOfAIS().IsBound2(argv[2] ) ) {di<<"vplane: error 1st name doesn't exist in the GetMapOfAIS()."<<"\n";return 1;}
    // on recupere la shape dans la map
    Handle(AIS_InteractiveObject) theShapeA =
      Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[2] ));

    // Le premier argument est un AIS_Point 1
    if (!theShapeA.IsNull() &&
      theShapeA->Type()==AIS_KOI_Datum && theShapeA->Signature()==1) {
        // le deuxieme argument doit etre un AIS_Point aussi
        if (argc<5 || !GetMapOfAIS().IsBound2(argv[3] ) ) {di<<"vplane: error 2de name doesn't exist in the GetMapOfAIS()."<<"\n";return 1;}
        // on recupere la shape dans la map
        Handle(AIS_InteractiveObject) theShapeB =
          Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[3]));
        // si B n'est pas un AIS_Point
        if (theShapeB.IsNull() ||
          (!(theShapeB->Type()==AIS_KOI_Datum && theShapeB->Signature()==1)))
        {
          di<<"vplane: error 2de object is expected to be an AIS_Point. "<<"\n";
          return 1;
        }
        // le troisieme objet est un AIS_Point
        if (!GetMapOfAIS().IsBound2(argv[4]) ) {di<<"vplane: error 3de name doesn't exist in the GetMapOfAIS()."<<"\n";return 1; }
        // on recupere la shape dans la map
        Handle(AIS_InteractiveObject) theShapeC =
          Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[4]));
        // si C n'est pas un AIS_Point
        if (theShapeC.IsNull() ||
          (!(theShapeC->Type()==AIS_KOI_Datum && theShapeC->Signature()==1)))
        {
          di<<"vplane: error 3de object is expected to be an AIS_Point. "<<"\n";
          return 1;
        }

        // Traitement des objets A,B,C
        // Downcaste de AIS_IO en AIS_Point
        Handle(AIS_Point) theAISPointA= *(Handle(AIS_Point)*)& theShapeA;
        Handle(AIS_Point) theAISPointB= *(Handle(AIS_Point)*)& theShapeB;
        Handle(AIS_Point) theAISPointC= *(Handle(AIS_Point)*)& theShapeC;

        Handle(Geom_Point ) myGeomPointA=  theAISPointA->Component();
        Handle(Geom_CartesianPoint ) myCartPointA= *((Handle(Geom_CartesianPoint)*)&  myGeomPointA);
        //      Handle(Geom_CartesianPoint ) myCartPointA= *(Handle(Geom_CartesianPoint)*)& (theAISPointA->Component() ) ;

        Handle(Geom_Point ) myGeomPointB =  theAISPointB->Component();
        Handle(Geom_CartesianPoint ) myCartPointB= *((Handle(Geom_CartesianPoint)*)&  theAISPointB);
        //      Handle(Geom_CartesianPoint ) myCartPointB= *(Handle(Geom_CartesianPoint)*)& (theAISPointB->Component() ) ;

        Handle(Geom_Point ) myGeomPointBC=  theAISPointC->Component();
        Handle(Geom_CartesianPoint ) myCartPointC= *((Handle(Geom_CartesianPoint)*)&  theAISPointC);
        //      Handle(Geom_CartesianPoint ) myCartPointC= *(Handle(Geom_CartesianPoint)*)& (theAISPointC->Component() ) ;

        // Verification que les 3 points sont bien differents.
        if (myCartPointB->X()==myCartPointA->X() && myCartPointB->Y()==myCartPointA->Y() && myCartPointB->Z()==myCartPointA->Z() ) {
          // B=A
          di<<"vplane error: same points"<<"\n";return 1;
        }
        if (myCartPointC->X()==myCartPointA->X() && myCartPointC->Y()==myCartPointA->Y() && myCartPointC->Z()==myCartPointA->Z() ) {
          // C=A
          di<<"vplane error: same points"<<"\n";return 1;
        }
        if (myCartPointC->X()==myCartPointB->X() && myCartPointC->Y()==myCartPointB->Y() && myCartPointC->Z()==myCartPointB->Z() ) {
          // C=B
          di<<"vplane error: same points"<<"\n";return 1;
        }

        gp_Pnt A= myCartPointA->Pnt();
        gp_Pnt B= myCartPointB->Pnt();
        gp_Pnt C= myCartPointC->Pnt();

        // Construction de l'AIS_Plane
        GC_MakePlane MkPlane (A,B,C);
        Handle(Geom_Plane) myGeomPlane = MkPlane.Value();
        Handle(AIS_Plane)  myAISPlane = new AIS_Plane(myGeomPlane );
        GetMapOfAIS().Bind (myAISPlane,name );
        TheAISContext()->Display(myAISPlane);
      }

      // si le premier argument est un AIS_Axis 2
      // creation d'un plan orthogonal a l'axe passant par un point
    else if (theShapeA->Type()==AIS_KOI_Datum && theShapeA->Signature()==2 ) {
      // le deuxieme argument doit etre un AIS_Point
      if (argc!=4 || !GetMapOfAIS().IsBound2(argv[3] ) )
      {
        di<<"vplane: error 2de name doesn't exist in the GetMapOfAIS()."<<"\n";
        return 1;
      }
      // on recupere la shape dans la map
      Handle(AIS_InteractiveObject) theShapeB =
        Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[3]));
      // si B n'est pas un AIS_Point
      if (theShapeB.IsNull() ||
        (!(theShapeB->Type()==AIS_KOI_Datum && theShapeB->Signature()==1)))
      {
        di<<"vplane: error 2de object is expected to be an AIS_Point. "<<"\n";
        return 1;
      }

      // Traitement des objets A et B
      Handle(AIS_Axis) theAISAxisA= *(Handle(AIS_Axis)*)& theShapeA;
      Handle(AIS_Point) theAISPointB= *(Handle(AIS_Point)*)& theShapeB;

      Handle(Geom_Line ) myGeomLineA = theAISAxisA ->Component();
      Handle(Geom_Point) myGeomPointB= theAISPointB->Component()  ;

      gp_Ax1 myAxis= myGeomLineA->Position();
      Handle(Geom_CartesianPoint ) myCartPointB= *(Handle(Geom_CartesianPoint )*)& myGeomPointB;

      // Pas de moyens de verifier que le point B n'est pas sur l'axe

      gp_Dir D=myAxis.Direction();
      gp_Pnt B= myCartPointB->Pnt();

      // Construction de l'AIS_Plane
      Handle(Geom_Plane) myGeomPlane= new Geom_Plane(B,D);
      Handle(AIS_Plane)  myAISPlane = new AIS_Plane(myGeomPlane,B );
      GetMapOfAIS().Bind (myAISPlane,name );
      TheAISContext()->Display(myAISPlane);

    }
    // Si le premier argument est un AIS_Plane 7
    // Creation d'un Plan parallele a ce plan passant par le point
    else if (theShapeA->Type()==AIS_KOI_Datum && theShapeA->Signature()==7 ) {
      // le deuxieme argument doit etre un AISPoint
      if (argc!=4 || !GetMapOfAIS().IsBound2(argv[3] ) ) {
        di<<"vplane: error 2de name doesn't exist in the GetMapOfAIS()."<<"\n";
        return 1;
      }
      // on recupere la shape dans la map
      Handle(AIS_InteractiveObject) theShapeB =
        Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[3]));
      // si B n'est pas un AIS_Point
      if (theShapeB.IsNull() ||
        (!(theShapeB->Type()==AIS_KOI_Datum && theShapeB->Signature()==1)))
      {
        di<<"vplane: error 2de object is expected to be an AIS_Point. "<<"\n";
        return 1;
      }

      // Traitement des objets A et B
      Handle(AIS_Plane) theAISPlaneA= *(Handle(AIS_Plane)*)& theShapeA;
      Handle(AIS_Point) theAISPointB= *(Handle(AIS_Point)*)& theShapeB;

      Handle (Geom_Plane) theNewGeomPlane= theAISPlaneA->Component();
      Handle(Geom_Point) myGeomPointB= theAISPointB->Component()  ;

      Handle(Geom_CartesianPoint ) myCartPointB= *(Handle(Geom_CartesianPoint )*)& myGeomPointB;
      gp_Pnt B= myCartPointB->Pnt();

      // Construction de l'AIS_Plane
      Handle(AIS_Plane)  myAISPlane = new AIS_Plane(theNewGeomPlane,B );
      GetMapOfAIS().Bind (myAISPlane,name );
      TheAISContext()->Display(myAISPlane);

    }
    // Sinon erreur
    else {di<<"vplane: error 1st object is not an AIS. "<<"\n";return 1;}

  }

  // Il n'y a pas d'arguments
  else {

    // Fonction vplane
    // Teste le constructeur AIS_Plane::AIS_Plane(Geom_Plane, Standard_Boolean )
    if (!strcasecmp(argv[0] ,"vplane" ) ) {
      TheAISContext()->OpenLocalContext();
      myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();

      // Active les modes Vertex, Edge et Face
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(1) );
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
      di<<"Select a vertex, a face or an edge. "<<"\n";

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
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4) );
        di<<" Select an edge or a different vertex."<<"\n";

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
        // ShapeB est un Vertex
        if (ShapeB.ShapeType()==TopAbs_VERTEX ) {
          // Si A et B sont le meme point
          if (ShapeB.IsSame(ShapeA) ) {di<<" vplane: error, same points selected"<<"\n";return 1; }
          TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(2) );
          di<<" Select a different vertex."<<"\n";

          // Boucle d'attente waitpick.
          Standard_Integer argcccc = 5;
          const char *buffff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
          const char **argvvvv = (const char **) buffff;
          while (ViewerMainLoop( argcccc, argvvvv) ) { }
          // fin de la boucle

          TopoDS_Shape ShapeC;
          for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
            ShapeC = TheAISContext()->SelectedShape();
          }
          // ShapeC est aussi un vertex...
          if (ShapeC.IsSame(ShapeA)||ShapeC.IsSame(ShapeB) ) {di<<" vplane: error, same points selected"<<"\n";return 1; }

          // Fermeture du contexte local
          TheAISContext()->CloseLocalContext(myCurrentIndex);

          // Construction du plane
          gp_Pnt A=BRep_Tool::Pnt(TopoDS::Vertex(ShapeA ) );
          gp_Pnt B=BRep_Tool::Pnt(TopoDS::Vertex(ShapeB ) );
          gp_Pnt C=BRep_Tool::Pnt(TopoDS::Vertex(ShapeC ) );
          GC_MakePlane MkPlane(A,B,C);
          Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
          Handle(AIS_Plane) myAISPlane=new AIS_Plane (theGeomPlane );
          GetMapOfAIS().Bind (myAISPlane ,name );
          TheAISContext()->Display(myAISPlane);

        }
        // ShapeB est un edge
        else  {
          // il s'agit de verifier que le  vertex ShapeA n'est pas sur l'edge ShapeB
          TopoDS_Edge EdgeB=TopoDS::Edge(ShapeB);
          TopoDS_Vertex VertA=TopoDS::Vertex(ShapeA);

          BRepExtrema_ExtPC OrthoProj (VertA, EdgeB );
          if (OrthoProj.SquareDistance(1)<1e-6 ) {
            // Le vertex est sur l'edge
            di<<" vplane: error point is on the edge."<<"\n";return 1;
          }
          else {
            // le vertex n'appartient pes a l'edge on peut construire le plane
            // Fermeture du contexte local
            TheAISContext()->CloseLocalContext(myCurrentIndex);
            // Construction du plane
            gp_Pnt A=BRep_Tool::Pnt(VertA );
            TopoDS_Vertex VBa,VBb;
            TopExp::Vertices(EdgeB ,VBa ,VBb );
            gp_Pnt Ba=BRep_Tool::Pnt(VBa);
            gp_Pnt Bb=BRep_Tool::Pnt(VBb);
            GC_MakePlane MkPlane (A,Ba,Bb);
            Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
            Handle(AIS_Plane) myAISPlane=new AIS_Plane (theGeomPlane );
            GetMapOfAIS().Bind (myAISPlane ,name );
            TheAISContext()->Display(myAISPlane);

          }

        }

      }
      // ShapeA est un edge
      else if (ShapeA.ShapeType()==TopAbs_EDGE ) {

        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4) );
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(2) );
        di<<" Select a vertex that don't belong to the edge."<<"\n";

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
        // ShapeB est forcement un Vertex
        // On verifie que le vertex ShapeB n'est pas sur l'edge ShapeA
        TopoDS_Edge EdgeA=TopoDS::Edge(ShapeA);
        TopoDS_Vertex VertB=TopoDS::Vertex(ShapeB);

        BRepExtrema_ExtPC OrthoProj (VertB,EdgeA );
        if (OrthoProj.SquareDistance(1)<1e-6) {
          // Le vertex est sur l'edge
          di<<" vplane: error point is on the edge."<<"\n";return 1;
        }
        else {
          // le vertex n'appartient pas a l'edge on peut construire le plane
          // Fermeture du contexte local
          TheAISContext()->CloseLocalContext(myCurrentIndex);
          // Construction du plane
          gp_Pnt B=BRep_Tool::Pnt(VertB );
          TopoDS_Vertex VAa,VAb;
          TopExp::Vertices(EdgeA ,VAa ,VAb );
          gp_Pnt Aa=BRep_Tool::Pnt(VAa);
          gp_Pnt Ab=BRep_Tool::Pnt(VAb);
          GC_MakePlane MkPlane (B,Aa,Ab);
          Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
          Handle(AIS_Plane) myAISPlane=new AIS_Plane (theGeomPlane );
          GetMapOfAIS().Bind (myAISPlane ,name );
          TheAISContext()->Display(myAISPlane);

        }


      }
      // ShapeA est une Face
      else {
        // Fermeture du contexte local: Plus rien a selectionner
        TheAISContext()->CloseLocalContext(myCurrentIndex);
        // Construction du plane
        TopoDS_Face myFace=TopoDS::Face(ShapeA);
        BRepAdaptor_Surface mySurface (myFace, Standard_False );
        if (mySurface.GetType()==GeomAbs_Plane ) {
          gp_Pln myPlane=mySurface.Plane();
          Handle(Geom_Plane) theGeomPlane=new Geom_Plane (myPlane );
          Handle(AIS_Plane) myAISPlane=new AIS_Plane (theGeomPlane );
          GetMapOfAIS().Bind (myAISPlane ,name );
          TheAISContext()->Display(myAISPlane);

        }
        else {
          di<<" vplane: error"<<"\n";return 1;
        }

      }

    }

    // Fonction vPlanePara
    // ===================
    // teste le constructeur AIS_Plane::AIS_Plane(Geom_Plane,gp_Pnt )
    else if (!strcasecmp(argv[0] ,"vplanepara" )) {

      TheAISContext()->OpenLocalContext();
      myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();

      // Active les modes Vertex et Face
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(1) );
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
      di<<" Select a vertex or a face."<<"\n";

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

      if (ShapeA.ShapeType()==TopAbs_VERTEX ) {
        // ShapeA est un vertex
        // On desactive le mode Vertex
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(1) );
        di<<" Select a face."<<"\n";

        // Boucle d'attente waitpick.
        Standard_Integer argccc = 5;
        const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvv = (const char **) bufff;
        while (ViewerMainLoop( argccc, argvvv) ) { }
        // fin de la boucle

        TopoDS_Shape ShapeB;
        for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
          // Le vertex ShapeA peut etre dans la Face ShapeB
          ShapeB = TheAISContext()->SelectedShape();
        }

        // Fermeture du context local
        TheAISContext()->CloseLocalContext(myCurrentIndex);

        // Construction du plane
        gp_Pnt A=BRep_Tool::Pnt(TopoDS::Vertex(ShapeA ) );

        TopoDS_Face myFace=TopoDS::Face(ShapeB);
        BRepAdaptor_Surface mySurface (myFace, Standard_False );
        if (mySurface.GetType()==GeomAbs_Plane ) {
          gp_Pln myPlane=mySurface.Plane();
          // construit un plan parallele a theGeomPlane passant par A
          myPlane.SetLocation(A);
          Handle(Geom_Plane) theGeomPlane=new Geom_Plane (myPlane );
          Handle(AIS_Plane) myAISPlane=new AIS_Plane (theGeomPlane ,A );
          GetMapOfAIS().Bind (myAISPlane ,name );
          TheAISContext()->Display(myAISPlane);

        }
        else {
          di<<" vplane: error"<<"\n";return 1;
        }

      }
      else{
        // ShapeA est une Face
        // On desactive le mode Face
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4) );
        di<<" Select a vertex."<<"\n";

        // Boucle d'attente waitpick.
        Standard_Integer argccc = 5;
        const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvv = (const char **) bufff;
        while (ViewerMainLoop( argccc, argvvv) ) { }
        // fin de la boucle

        TopoDS_Shape ShapeB;
        for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
          // Le vertex ShapeB peut etre dans la Face ShapeA
          ShapeB = TheAISContext()->SelectedShape();
        }
        // Fermeture du context local
        TheAISContext()->CloseLocalContext(myCurrentIndex);

        // Construction du plane
        gp_Pnt B=BRep_Tool::Pnt(TopoDS::Vertex(ShapeB ) );

        TopoDS_Face myFace=TopoDS::Face(ShapeA);
        BRepAdaptor_Surface mySurface (myFace, Standard_False );
        if (mySurface.GetType()==GeomAbs_Plane ) {
          gp_Pln myPlane=mySurface.Plane();
          myPlane.SetLocation(B);
          Handle(Geom_Plane) theGeomPlane=new Geom_Plane (myPlane );
          // construit un plan parallele a theGeomPlane passant par B
          Handle(AIS_Plane) myAISPlane=new AIS_Plane (theGeomPlane ,B );
          GetMapOfAIS().Bind (myAISPlane ,name );
          TheAISContext()->Display(myAISPlane);

        }
        else {
          di<<" vplane: error"<<"\n";return 1;
        }

      }

    }

    // Fonction vplaneortho
    // ====================
    // teste le constructeur AIS_Plane::AIS_Plane(Geom_Plane,gp_Pnt,gp_Pnt,gp_Pnt)
    else {

      TheAISContext()->OpenLocalContext();
      myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();

      // Active les modes Edge et Face
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
      TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
      di<<" Select a face and an edge coplanar."<<"\n";

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

      if (ShapeA.ShapeType()==TopAbs_EDGE ) {
        // ShapeA est un edge, on desactive le mode edge...
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(2) );
        di<<" Select a face."<<"\n";

        // Boucle d'attente waitpick.
        Standard_Integer argccc = 5;
        const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvv = (const char **) bufff;
        while (ViewerMainLoop( argccc, argvvv) ) { }
        // fin de la boucle

        TopoDS_Shape ShapeB;
        for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
          // L'edge ShapeA peut etre dans la Face ShapeB
          ShapeB = TheAISContext()->SelectedShape();
        }

        // Fermeture du context local
        TheAISContext()->CloseLocalContext(myCurrentIndex);

        // Construction du plane
        TopoDS_Edge  EdgeA=TopoDS::Edge(ShapeA);
        TopoDS_Vertex VAa,VAb;
        // vi
        TopExp::Vertices(EdgeA ,VAa ,VAb );
        gp_Pnt Aa=BRep_Tool::Pnt(VAa);
        gp_Pnt Ab=BRep_Tool::Pnt(VAb);
        gp_Vec ab  (Aa,Ab);

        gp_Dir Dab (ab);
        // Creation de mon axe de rotation
        gp_Ax1 myRotAxis (Aa,Dab);

        TopoDS_Face myFace=TopoDS::Face(ShapeB);
        // Il faut imperativement que l'edge soit parallele a la face
        // vi
        BRepExtrema_ExtPF myHauteurA (VAa , myFace );
        BRepExtrema_ExtPF myHauteurB (VAb , myFace );
        // on compare les deux hauteurs a la tolerance pres
        if ( fabs(sqrt(myHauteurA.SquareDistance(1)) - sqrt (myHauteurB.SquareDistance(1)) )>0.1 ) {
          // l'edge n'est pas parallele a la face
          di<<" vplaneOrtho error: l'edge n'est pas parallele a la face."<<"\n";return 1;
        }
        // l'edge est OK
        BRepAdaptor_Surface mySurface (myFace, Standard_False );
        if (mySurface.GetType()==GeomAbs_Plane ) {
          gp_Pln myPlane=mySurface.Plane();
          // On effectue une rotation d'1/2 tour autour de l'axe de rotation
          myPlane.Rotate(myRotAxis , PI/2 );

          Handle(Geom_Plane) theGeomPlane=new Geom_Plane (myPlane );
          // construit un plan parallele a theGeomPlane contenant l'edgeA (De centre le milieu de l'edgeA)
          gp_Pnt theMiddle ((Aa.X()+Ab.X() )/2 ,(Aa.Y()+Ab.Y() )/2 ,(Aa.Z()+Ab.Z() )/2 );
          Handle(AIS_Plane) myAISPlane=new AIS_Plane (theGeomPlane ,theMiddle );
          GetMapOfAIS().Bind (myAISPlane ,name );
          TheAISContext()->Display(myAISPlane);

        }
        else {
          di<<" vplaneOrtho: error"<<"\n";return 1;
        }

      }

      else {
        // ShapeA est une Face, on desactive le mode face.
        TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4) );
        di<<" Select an edge."<<"\n";

        // Boucle d'attente waitpick.
        Standard_Integer argccc = 5;
        const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvv = (const char **) bufff;
        while (ViewerMainLoop( argccc, argvvv) ) { }
        // fin de la boucle

        TopoDS_Shape ShapeB;
        for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
          // L'edge ShapeB peut etre dans la Face ShapeA
          ShapeB = TheAISContext()->SelectedShape();
        }

        // Fermeture du context local
        TheAISContext()->CloseLocalContext(myCurrentIndex);

        // Construction du plane
        TopoDS_Edge  EdgeB=TopoDS::Edge(ShapeB);
        TopoDS_Vertex VBa,VBb;
        TopExp::Vertices(EdgeB ,VBa ,VBb );
        gp_Pnt Ba=BRep_Tool::Pnt(VBa);
        gp_Pnt Bb=BRep_Tool::Pnt(VBb);
        gp_Vec ab  (Ba,Bb);
        gp_Dir Dab (ab);
        // Creation de mon axe de rotation
        gp_Ax1 myRotAxis (Ba,Dab);

        TopoDS_Face myFace=TopoDS::Face(ShapeA);
        // Il faut imperativement que l'edge soit parallele a la face
        BRepExtrema_ExtPF myHauteurA (VBa , myFace );
        BRepExtrema_ExtPF myHauteurB (VBb , myFace );
        // on compare les deux hauteurs a la tolerance pres
        if ( fabs(sqrt(myHauteurA.SquareDistance(1)) - sqrt(myHauteurB.SquareDistance(1)) )>0.1 ) {
          // l'edge n'est pas parallele a la face
          di<<" vplaneOrtho error: l'edge n'est pas parallele a la face."<<"\n";return 1;
        }
        // l'edge est OK
        BRepAdaptor_Surface mySurface (myFace, Standard_False );
        if (mySurface.GetType()==GeomAbs_Plane ) {
          gp_Pln myPlane=mySurface.Plane();
          // On effectue une rotation d'1/2 tour autour de l'axe de rotation
          myPlane.Rotate(myRotAxis , PI/2  );
          Handle(Geom_Plane) theGeomPlane=new Geom_Plane (myPlane );
          // construit un plan parallele a theGeomPlane contenant l'edgeA (De centre le milieu de l'edgeA)
          gp_Pnt theMiddle ((Ba.X()+Bb.X() )/2 , (Ba.Y()+Bb.Y() )/2 , (Ba.Z()+Bb.Z() )/2 );
          Handle(AIS_Plane) myAISPlane=new AIS_Plane (theGeomPlane ,theMiddle );
          GetMapOfAIS().Bind (myAISPlane ,name );
          TheAISContext()->Display(myAISPlane);

        }
        else {
          di<<" vplaneOrtho: error"<<"\n";return 1;
        }

      }

    }

  }
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
  if (argc!=4 && argc!=8 && argc!=2 )  {di<<"vline error: number of arguments not correct "<<"\n";return 1; }
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
          di <<"vline error: wrong type of 2de argument."<<"\n";
          return 1;
        }
      }
    else {di <<"vline error: wrong type of 1st argument."<<"\n";return 1; }
    // Les deux parametres sont du bon type. On verifie que les points ne sont pas confondus
    Handle(AIS_Point) theAISPointA= *(Handle(AIS_Point)*)& theShapeA;
    Handle(AIS_Point) theAISPointB= *(Handle(AIS_Point)*)& theShapeB;

    Handle(Geom_Point ) myGeomPointBA=  theAISPointA->Component();
    Handle(Geom_CartesianPoint ) myCartPointA= *((Handle(Geom_CartesianPoint)*)&  myGeomPointBA);
    //    Handle(Geom_CartesianPoint ) myCartPointA= *(Handle(Geom_CartesianPoint)*)& (theAISPointA->Component() ) ;

    Handle(Geom_Point ) myGeomPointB=  theAISPointB->Component();
    Handle(Geom_CartesianPoint ) myCartPointB= *((Handle(Geom_CartesianPoint)*)&  myGeomPointB);
    //    Handle(Geom_CartesianPoint ) myCartPointB= *(Handle(Geom_CartesianPoint)*)& (theAISPointB->Component() ) ;

    if (myCartPointB->X()==myCartPointA->X() && myCartPointB->Y()==myCartPointA->Y() && myCartPointB->Z()==myCartPointA->Z() ) {
      // B=A
      di<<"vline error: same points"<<"\n";return 1;
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
      coord[i]=atof(argv[2+i]);
      coord[i+3]=atof(argv[5+i]);
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
    di<<" Select a vertex "<<"\n";

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

      di<<" Select a different vertex."<<"\n";

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
      di<<"vline error."<<"\n";
    }

  }

  return 0;
}


//==============================================================================
// Fonction  vcircle
// -----------------  Uniquement par parametre. Pas de selection dans le viewer.
//==============================================================================

//==============================================================================
//function : VCircleBuilder
//purpose  : Build an AIS_Circle
//Draw arg : vcircle CircleName PlaneName PointName Radius
//                              PointName PointName PointName
//==============================================================================
#include <Geom_CartesianPoint.hxx>
#include <Geom_Circle.hxx>
#include <AIS_Circle.hxx>
#include <GC_MakeCircle.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>

static int VCircleBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Standard_Integer myCurrentIndex;
  // verification of the arguments
  if (argc>5 ||  argc<2 ) {di<<"vcircle error: expect 3 arguments."<<"\n";return 1; }
  TheAISContext()->CloseAllContexts();

  // Il y a des arguments
  if (argc==5 ) {
    Handle(AIS_InteractiveObject) theShapeA;
    Handle(AIS_InteractiveObject) theShapeB;

    theShapeA=
      Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[2]));
    theShapeB=
      Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[3]));

    // Arguments: AIS_Point AIS_Point AIS_Point
    // ========================================
    if (!theShapeA.IsNull() && theShapeB.IsNull() &&
      theShapeA->Type()==AIS_KOI_Datum && theShapeA->Signature()==1)
    {
      if (theShapeB->Type()!=AIS_KOI_Datum || theShapeB->Signature()!=1 ) {
        di<<"vcircle error: 2de argument is unexpected to be a point."<<"\n";
        return 1;
      }
      // Le troisieme objet doit etre un point
      Handle(AIS_InteractiveObject) theShapeC =
        Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2(argv[4]));
      if (theShapeC.IsNull() ||
        theShapeC->Type()!=AIS_KOI_Datum || theShapeC->Signature()!=1 ) {
          di<<"vcircle error: 3de argument is unexpected to be a point."<<"\n";
          return 1;
        }
        // tag
        // On verifie que les 3 points sont differents.
        Handle(AIS_Point) theAISPointA= *(Handle(AIS_Point)*)& theShapeA;
        Handle(AIS_Point) theAISPointB= *(Handle(AIS_Point)*)& theShapeB;
        Handle(AIS_Point) theAISPointC= *(Handle(AIS_Point)*)& theShapeC;

        Handle(Geom_Point ) myGeomPointA=  theAISPointA->Component();
        Handle(Geom_CartesianPoint ) myCartPointA= *((Handle(Geom_CartesianPoint)*)&  myGeomPointA);

        Handle(Geom_Point ) myGeomPointB =  theAISPointB->Component();
        Handle(Geom_CartesianPoint ) myCartPointB= *((Handle(Geom_CartesianPoint)*)&  theAISPointB);

        Handle(Geom_Point ) myGeomPointBC=  theAISPointC->Component();
        Handle(Geom_CartesianPoint ) myCartPointC= *((Handle(Geom_CartesianPoint)*)&  theAISPointC);

        // Test A=B
        if (myCartPointA->X()==myCartPointB->X() && myCartPointA->Y()==myCartPointB->Y() && myCartPointA->Z()==myCartPointB->Z()  ) {
          di<<"vcircle error: Same points."<<"\n";return 1;
        }
        // Test A=C
        if (myCartPointA->X()==myCartPointC->X() && myCartPointA->Y()==myCartPointC->Y() && myCartPointA->Z()==myCartPointC->Z()  ) {
          di<<"vcircle error: Same points."<<"\n";return 1;
        }
        // Test B=C
        if (myCartPointB->X()==myCartPointC->X() && myCartPointB->Y()==myCartPointC->Y() && myCartPointB->Z()==myCartPointC->Z()  ) {
          di<<"vcircle error: Same points."<<"\n";return 1;
        }
        // Construction du cercle
        GC_MakeCircle Cir=GC_MakeCircle (myCartPointA->Pnt(),myCartPointB->Pnt(),myCartPointC->Pnt() );
        Handle (Geom_Circle) theGeomCircle=Cir.Value();
        Handle(AIS_Circle) theAISCircle=new AIS_Circle(theGeomCircle );
        GetMapOfAIS().Bind(theAISCircle,argv[1] );
        TheAISContext()->Display(theAISCircle );

    }

    // Arguments: ASI_Plane AIS_Point Real
    // ===================================
    else if (theShapeA->Type()==AIS_KOI_Datum && theShapeA->Signature()==7 ) {
      if (theShapeB->Type()!=AIS_KOI_Datum || theShapeB->Signature()!=1 ) {
        di<<"vcircle error: 2de element is a unexpected to be a point."<<"\n";return 1;
      }
      // On verifie que le rayon est bien >=0
      if (atof(argv[4])<=0 ) {di<<"vcircle error: the radius must be >=0."<<"\n";return 1;  }

      // On recupere la normale au Plane.
      Handle(AIS_Plane) theAISPlane= *(Handle(AIS_Plane)*)& theShapeA;
      Handle(AIS_Point) theAISPointB= *(Handle(AIS_Point)*)& theShapeB;


      //      Handle(Geom_Plane ) myGeomPlane= *(Handle(Geom_Plane)*)& (theAISPlane->Component() );
      Handle(Geom_Plane ) myGeomPlane= theAISPlane->Component();
      Handle(Geom_Point ) myGeomPointB =  theAISPointB->Component();
      Handle(Geom_CartesianPoint ) myCartPointB= *((Handle(Geom_CartesianPoint)*)&  theAISPointB);

      gp_Pln mygpPlane = myGeomPlane->Pln();
      gp_Ax1 thegpAxe = mygpPlane.Axis();
      gp_Dir theDir = thegpAxe.Direction();
      gp_Pnt theCenter=myCartPointB->Pnt();
      Standard_Real TheR = atof(argv[4]);
      GC_MakeCircle Cir=GC_MakeCircle (theCenter, theDir ,TheR);
      Handle (Geom_Circle) theGeomCircle=Cir.Value();
      Handle(AIS_Circle) theAISCircle=new AIS_Circle(theGeomCircle );
      GetMapOfAIS().Bind(theAISCircle,argv[1] );
      TheAISContext()->Display(theAISCircle );

    }

    // Error
    else{
      di<<"vcircle error: !st argument is a unexpected type."<<"\n";return 1;
    }

  }
  // Pas d'arguments: selection dans le viewer
  // =========================================
  else {

    TheAISContext()->OpenLocalContext();
    myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();

    // Active le mode Vertex et face.
    TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(1) );
    TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
    di<<" Select a vertex or a face."<<"\n";

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
      TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4) );
      di<<" Select a different vertex."<<"\n";

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

      // Selection de ShapeC
      di<<" Select the last vertex."<<"\n";
      TopoDS_Shape ShapeC;
      do {

        // Boucle d'attente waitpick.
        Standard_Integer argcccc = 5;
        const char *buffff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
        const char **argvvvv = (const char **) buffff;
        while (ViewerMainLoop( argcccc, argvvvv) ) { }
        // fin de la boucle

        for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
          ShapeC = TheAISContext()->SelectedShape();
        }


      } while(ShapeC.IsSame(ShapeA) || ShapeC.IsSame(ShapeB) );

      // Fermeture du context local
      TheAISContext()->CloseLocalContext(myCurrentIndex);

      // Construction du cercle
      gp_Pnt   A=BRep_Tool::Pnt(TopoDS::Vertex(ShapeA)  );
      gp_Pnt   B=BRep_Tool::Pnt(TopoDS::Vertex(ShapeB)  );
      gp_Pnt   C=BRep_Tool::Pnt(TopoDS::Vertex(ShapeC)  );

      GC_MakeCircle Cir=GC_MakeCircle (A,B,C );
      Handle (Geom_Circle) theGeomCircle=Cir.Value();
      Handle(AIS_Circle) theAISCircle=new AIS_Circle(theGeomCircle );
      GetMapOfAIS().Bind(theAISCircle,argv[1] );
      TheAISContext()->Display(theAISCircle );

    }
    // ShapeA est une face.
    else  {
      di<<" Select a vertex (in your face)."<<"\n";
      TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4) );

      TopoDS_Shape ShapeB;
      // Boucle d'attente waitpick.
      Standard_Integer argccc = 5;
      const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
      const char **argvvv = (const char **) bufff;
      while (ViewerMainLoop( argccc, argvvv) ) { }
      // fin de la boucle

      for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
        ShapeB = TheAISContext()->SelectedShape();
      }

      // Recuperation du rayon.
      Standard_Integer theRad;
      do {
        di<<" Enter the value of the radius:"<<"\n";
        cin>>theRad;
      } while (theRad<=0);

      // Fermeture du context local
      TheAISContext()->CloseLocalContext(myCurrentIndex);
      // Construction du cercle.

      // On recupere la normale au Plane. tag
      TopoDS_Face myFace=TopoDS::Face(ShapeA);
      BRepAdaptor_Surface mySurface (myFace, Standard_False );
      gp_Pln myPlane=mySurface.Plane();
      Handle(Geom_Plane) theGeomPlane=new Geom_Plane (myPlane );
      gp_Pln mygpPlane = theGeomPlane->Pln();
      gp_Ax1 thegpAxe = mygpPlane.Axis();
      gp_Dir theDir = thegpAxe.Direction();

      // On recupere le centre.
      gp_Pnt   theCenter=BRep_Tool::Pnt(TopoDS::Vertex(ShapeB)  );

      // On construit l'AIS_Circle
      GC_MakeCircle Cir=GC_MakeCircle (theCenter, theDir ,theRad  );
      Handle (Geom_Circle) theGeomCircle=Cir.Value();
      Handle(AIS_Circle) theAISCircle=new AIS_Circle(theGeomCircle );
      GetMapOfAIS().Bind(theAISCircle,argv[1] );
      TheAISContext()->Display(theAISCircle );

    }


  }

  return 0;
}


//===============================================================================================
//function : VDrawText
//author   : psn
//purpose  : Create a text.
//Draw arg : vdrawtext  name  [X] [Y] [Z] [R] [G] [B] [hor_align] [ver_align] [angle] [zoomable]
//===============================================================================================
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_NameOfFont.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic2d_GraphicObject.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>

#include <Visual3d_ViewManager.hxx>
#include <ViewerTest_Tool.ixx>

#include <Standard_DefineHandle.hxx>

#include <AIS_Drawer.hxx>

#include <Prs3d_Root.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <PrsMgr_PresentationManager3d.hxx>

#include <TCollection_ExtendedString.hxx>
#include <TCollection_AsciiString.hxx>

#include <gp_Pnt.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Quantity_Color.hxx>


DEFINE_STANDARD_HANDLE(MyTextClass, AIS_InteractiveObject)

class MyTextClass:public AIS_InteractiveObject
{
public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(MyTextClass );

  MyTextClass(){};

  MyTextClass
    (
      const TCollection_ExtendedString& , const gp_Pnt& ,
      Quantity_Color color,
      Standard_Integer aHJust,
      Standard_Integer aVJust ,
      Standard_Real Angle ,
      Standard_Boolean Zoom ,
      Standard_Real  Height,
      OSD_FontAspect FontAspect,
      Standard_CString Font
    );

private:

  void Compute (  const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                  const Handle(Prs3d_Presentation)& aPresentation,
                  const Standard_Integer aMode);

  void ComputeSelection (  const Handle(SelectMgr_Selection)& aSelection,
                           const Standard_Integer aMode){} ;

protected:
  TCollection_ExtendedString          aText;
  gp_Pnt                              aPosition;
  Standard_Real                       Red;
  Standard_Real                       Green;
  Standard_Real                       Blue;
  Standard_Real                       aAngle;
  Standard_Real                       aHeight;
  Standard_Boolean                    aZoomable;
  Quantity_Color                      aColor;
  Standard_CString                    aFont;
  OSD_FontAspect                      aFontAspect;
  Graphic3d_HorizontalTextAlignment   aHJustification;
  Graphic3d_VerticalTextAlignment     aVJustification;
};



IMPLEMENT_STANDARD_HANDLE(MyTextClass, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(MyTextClass, AIS_InteractiveObject)


MyTextClass::MyTextClass( const TCollection_ExtendedString& text, const gp_Pnt& position,
                          Quantity_Color    color       = Quantity_NOC_YELLOW,
                          Standard_Integer  aHJust      = Graphic3d_HTA_LEFT,
                          Standard_Integer  aVJust      = Graphic3d_VTA_BOTTOM,
                          Standard_Real     angle       = 0.0 ,
                          Standard_Boolean  zoomable    = Standard_True,
                          Standard_Real     height      = 12.,
                          OSD_FontAspect    fontAspect  = OSD_FA_Regular,
                          Standard_CString  font        = "Courier")
{
  aText           = text;
  aPosition       = position;
  aHJustification = Graphic3d_HorizontalTextAlignment(aHJust);
  aVJustification = Graphic3d_VerticalTextAlignment(aVJust);
  aAngle          = angle;
  aZoomable       = zoomable;
  aHeight         = height;
  aColor          = color;
  aFontAspect     = fontAspect;
  aFont           = font;
};



//////////////////////////////////////////////////////////////////////////////
void MyTextClass::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                          const Handle(Prs3d_Presentation)& aPresentation,
                          const Standard_Integer aMode)
{

  aPresentation->Clear();

  Handle_Prs3d_TextAspect asp = myDrawer->TextAspect();

  asp->SetFont(aFont);
  asp->SetColor(aColor);
  asp->SetHeight(aHeight); // I am changing the myHeight value

  asp->SetHorizontalJustification(aHJustification);
  asp->SetVerticalJustification(aVJustification);
  asp->Aspect()->SetTextZoomable(aZoomable);
  asp->Aspect()->SetTextAngle(aAngle);
  asp->Aspect()->SetTextFontAspect(aFontAspect);
  Prs3d_Text::Draw(aPresentation, asp, aText, aPosition);

  /* This comment code is worked
  Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
  Handle(Graphic3d_AspectFillArea3d) aspect = myDrawer->ShadingAspect()->Aspect();
  Graphic3d_Vertex vertices_text;
  vertices_text.SetCoord(aPosition.X(),aPosition.Y(),aPosition.Y());
  TheGroup->SetPrimitivesAspect(aspect);
  TheGroup->BeginPrimitives();
  TheGroup->Text(aText,vertices_text,aHeight,Standard_True);
  TheGroup->EndPrimitives();
  */
};

static int VDrawText (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  // Check arguments
  if (argc < 14)
  {
    di<<"Error: "<<argv[0]<<" - invalid number of arguments\n";
    di<<"Usage: type help "<<argv[0]<<"\n";
    return 1; //TCL_ERROR
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();

  // Create 3D view if it doesn't exist
  if ( aContext.IsNull() )
  {
    ViewerTest::ViewerInit();
    aContext = ViewerTest::GetAISContext();
    if( aContext.IsNull() )
    {
      di << "Error: Cannot create a 3D view\n";
      return 1; //TCL_ERROR
    }
  }

  // Text position
  const Standard_Real X = atof(argv[2]);
  const Standard_Real Y = atof(argv[3]);
  const Standard_Real Z = atof(argv[4]);
  const gp_Pnt pnt(X,Y,Z);

  // Text color
  const Quantity_Parameter R = atof(argv[5])/255.;
  const Quantity_Parameter G = atof(argv[6])/255.;
  const Quantity_Parameter B = atof(argv[7])/255.;
  const Quantity_Color aColor( R, G, B, Quantity_TOC_RGB );

  // Text alignment
  const int hor_align = atoi(argv[8]);
  const int ver_align = atoi(argv[9]);

  // Text angle
  const Standard_Real angle = atof(argv[10]);

  // Text zooming
  const Standard_Boolean zoom = atoi(argv[11]);

  // Text height
  const Standard_Real height = atof(argv[12]);

  // Text aspect
  const OSD_FontAspect aspect = OSD_FontAspect(atoi(argv[13]));

  // Text font
  TCollection_AsciiString font;
  if(argc < 15)
    font.AssignCat("Courier");
  else
    font.AssignCat(argv[14]);

  // Text is multibyte
  const Standard_Boolean isMultibyte = (argc < 16)? Standard_False : (atoi(argv[15]) != 0);

  // Read text string
  TCollection_ExtendedString name;
  if (isMultibyte)
  {
    const char *str = argv[1];
    while (*str)
    {
      unsigned short c1 = *str++;
      unsigned short c2 = *str++;
      if (!c1 || !c2) break;
      name += (Standard_ExtCharacter)((c1 << 8) | c2);
    }
  }
  else
  {
    name += argv[1];
  }

  if (name.Length())
  {
    Handle(MyTextClass) myT = new MyTextClass(name,pnt,aColor,hor_align,ver_align,angle,zoom,height,aspect,font.ToCString());
    aContext->Display(myT,Standard_True);
  }

  return 0;
}

#include <math.h>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <TShort_HArray1OfShortReal.hxx>

#include <AIS_Triangulation.hxx>
#include <Aspect_GraphicDevice.hxx>
#include <StdPrs_ToolShadedShape.hxx>
#include <Poly_Connect.hxx>
#include <TColgp_Array1OfDir.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#include <AIS_Drawer.hxx>
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
  int numPts, numPolys;
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

  numPts =  mPhiResolution * localThetaResolution + 2;
  numPolys =  mPhiResolution * 2 * localThetaResolution;

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
  startTheta *= Standard_PI  / 180.0;
  endTheta = (localEndTheta > localStartTheta ? localEndTheta : localStartTheta);
  endTheta *= Standard_PI  / 180.0;


  startPhi = ( mStartPhi <  mEndPhi ?  mStartPhi :  mEndPhi);
  startPhi *= Standard_PI  / 180.0;
  endPhi = ( mEndPhi >  mStartPhi ?  mEndPhi :  mStartPhi);
  endPhi *= Standard_PI  / 180.0;

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
    theta = localStartTheta * Standard_PI / 180.0 + i*deltaTheta;
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

      Standard_Integer j = (i - PointsOfArray.Lower()) * 3;
      Normals->SetValue(j + 1, (Standard_ShortReal)Nor.X());
      Normals->SetValue(j + 2, (Standard_ShortReal)Nor.Y());
      Normals->SetValue(j + 3, (Standard_ShortReal)Nor.Z());
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
static int VDrawSphere (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
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
              << " shapeName Fineness [X=0.0 Y=0.0 Z=0.0] [Radius=100.0] [ToEnableVBO=1] [NumberOfViewerUpdate=1] [ToShowEdges=0]\n";
    return 1;
  }

  // read the arguments
  TCollection_AsciiString aShapeName (argv[1]);
  Standard_Integer aResolution = atoi (argv[2]);
  Standard_Real aCenterX = (argc > 5) ? atof (argv[3]) : 0.0;
  Standard_Real aCenterY = (argc > 5) ? atof (argv[4]) : 0.0;
  Standard_Real aCenterZ = (argc > 5) ? atof (argv[5]) : 0.0;
  Standard_Real aRadius =  (argc > 6) ? atof (argv[6]) : 100.0;
  Standard_Boolean isVBOEnabled = (argc > 7) ? atoi (argv[7]) : Standard_True;
  Standard_Integer aRedrawsNb =   (argc > 8) ? atoi (argv[8]) : 1;
  Standard_Boolean toShowEdges =  (argc > 9) ? atoi (argv[9]) : Standard_False;

  if (aRedrawsNb <= 0)
  {
    aRedrawsNb = 1;
  }

  // remove AIS object with given name from map
  if (GetMapOfAIS().IsBound2 (aShapeName))
  {
    Handle(Standard_Transient) anObj = GetMapOfAIS().Find2 (aShapeName);
    Handle(AIS_InteractiveObject) anInterObj = Handle(AIS_InteractiveObject)::DownCast (anObj);
    if (anInterObj.IsNull())
    {
      std::cout << "Shape name was used for non AIS viewer\n!";
      return 1;
    }
    aContextAIS->Remove (anInterObj, Standard_False);
    GetMapOfAIS().UnBind2 (aShapeName);
  }

  // enable/disable VBO
  Handle(Graphic3d_GraphicDriver) aDriver =
         Handle(Graphic3d_GraphicDriver)::DownCast (aContextAIS->CurrentViewer()->Device()->GraphicDriver());
  if (!aDriver.IsNull())
  {
    aDriver->EnableVBO (isVBOEnabled);
  }

  std::cout << "Compute Triangulation...\n";
  Handle(AIS_Triangulation) aShape
    = new AIS_Triangulation (CalculationOfSphere (aCenterX, aCenterY, aCenterZ,
                                                  aResolution,
                                                  aRadius));
  Standard_Integer aNumberPoints    = aShape->GetTriangulation()->Nodes().Length();
  Standard_Integer aNumberTriangles = aShape->GetTriangulation()->Triangles().Length();

  // register the object in map
  GetMapOfAIS().Bind (aShape, aShapeName);

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
  std::cout << "NumberOfPoints:    " << aNumberPoints << "\n"
            << "NumberOfTriangles: " << aNumberTriangles << "\n"
            << "Amount of memory required for PolyTriangulation without Normals: " << (aTotalSize - aNormalsSize) << " Mb\n"
            << "Amount of memory for colors: " << aColorsSize << " Mb\n"
            << "Amount of memory for PolyConnect: " << aPolyConnectSize << " Mb\n"
            << "Amount of graphic card memory required: " << aTotalSize << " Mb\n";

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

  aContextAIS->Display (aShape, Standard_False);

  // Two viewer updates are needed in order to measure time spent on
  // loading triangulation to graphic card memory + redrawing (1st update) and
  // time spent on redrawing itself (2nd and all further updates)
  OSD_Chronometer aTimer;
  Standard_Real aUserSeconds, aSystemSeconds;
  aTimer.Start();
  const Handle(V3d_Viewer)& aViewer = aContextAIS->CurrentViewer();
  for (Standard_Integer anInteration = 0; anInteration < aRedrawsNb; ++anInteration)
  {
    for (aViewer->InitActiveViews(); aViewer->MoreActiveViews(); aViewer->NextActiveViews())
    {
      if (anInteration == 0)
      {
        aViewer->ActiveView()->Update();
      }
      else
      {
        aViewer->ActiveView()->Redraw();
      }
    }
  }
  aTimer.Show (aUserSeconds, aSystemSeconds);
  aTimer.Stop();
  std::cout << "Number of scene redrawings: " << aRedrawsNb << "\n"
            << "CPU user time: "
            << std::setiosflags(std::ios::fixed) << std::setprecision(16) << 1000.0 * aUserSeconds
            << " msec\n"
            << "CPU system time: "
            << std::setiosflags(std::ios::fixed) << std::setprecision(16) << 1000.0 * aSystemSeconds
            << " msec\n"
            << "CPU average time of scene redrawing: "
            << std::setiosflags(std::ios::fixed) << std::setprecision(16) << 1000.0 * (aUserSeconds / (Standard_Real )aRedrawsNb)
            << " msec\n";
  return 0;
}

//===============================================================================================
//function : VClipPlane
//purpose  :
//===============================================================================================
static int VClipPlane (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  Standard_Real coeffA, coeffB, coeffC, coeffD;
  if (aViewer.IsNull() || aView.IsNull())
  {
    std::cout << "Viewer not initialized!\n";
    return 1;
  }

  // count an active planes count
  Standard_Integer aNewPlaneId = 1;
  Standard_Integer anActivePlanes = 0;
  for (aViewer->InitDefinedPlanes(); aViewer->MoreDefinedPlanes(); aViewer->NextDefinedPlanes(), ++aNewPlaneId)
  {
    Handle(V3d_Plane) aPlaneV3d = aViewer->DefinedPlane();
    if (aView->IsActivePlane (aPlaneV3d))
    {
      ++anActivePlanes;
    }
  }

  if (argc == 1)
  {
    // just show info about existing planes
    Standard_Integer aPlaneId = 1;
    std::cout << "Active planes: " << anActivePlanes << " from maximal " << aView->View()->PlaneLimit() << "\n";
    for (aViewer->InitDefinedPlanes(); aViewer->MoreDefinedPlanes(); aViewer->NextDefinedPlanes(), ++aPlaneId)
    {
      Handle(V3d_Plane) aPlaneV3d = aViewer->DefinedPlane();
      aPlaneV3d->Plane (coeffA, coeffB, coeffC, coeffD);
      gp_Pln aPlane (coeffA, coeffB, coeffC, coeffD);
      const gp_Pnt& aLoc = aPlane.Location();
      const gp_Dir& aNor = aPlane.Axis().Direction();
      Standard_Boolean isActive = aView->IsActivePlane (aPlaneV3d);
      std::cout << "Plane #" << aPlaneId
        << " " << aLoc.X() << " " << aLoc.Y() << " " << aLoc.Z()
        << " " << aNor.X() << " " << aNor.Y() << " " << aNor.Z()
        << (isActive ? " on" : " off")
        << (aPlaneV3d->IsDisplayed() ? ", displayed" : ", hidden")
        << "\n";
    }
    if (aPlaneId == 1)
    {
      std::cout << "No defined clipping planes\n";
    }
    return 0;
  }
  else if (argc == 2 || argc == 3)
  {
    Standard_Integer aPlaneIdToOff = (argc == 3) ? atoi (argv[1]) : 1;
    Standard_Boolean toIterateAll = (argc == 2);
    TCollection_AsciiString isOnOffStr ((argc == 3) ? argv[2] : argv[1]);
    isOnOffStr.LowerCase();
    Standard_Integer aPlaneId = 1;
    for (aViewer->InitDefinedPlanes(); aViewer->MoreDefinedPlanes(); aViewer->NextDefinedPlanes(), ++aPlaneId)
    {
      if (aPlaneIdToOff == aPlaneId || toIterateAll)
      {
        Handle(V3d_Plane) aPlaneV3d = aViewer->DefinedPlane();
        if (isOnOffStr.Search ("off") >= 0)
        {
          aView->SetPlaneOff (aPlaneV3d);
          std::cout << "Clipping plane #" << aPlaneId << " was disabled\n";
        }
        else if (isOnOffStr.Search ("on") >= 0)
        {
          // avoid z-fighting glitches
          aPlaneV3d->Erase();
          if (!aView->IsActivePlane (aPlaneV3d))
          {
            if (anActivePlanes < aView->View()->PlaneLimit())
            {
              aView->SetPlaneOn (aPlaneV3d);
              std::cout << "Clipping plane #" << aPlaneId << " was enabled\n";
            }
            else
            {
              std::cout << "Maximal active planes limit exceeded (" << anActivePlanes << ")\n"
                        << "You should disable or remove some existing plane to activate this one\n";
            }
          }
          else
          {
            std::cout << "Clipping plane #" << aPlaneId << " was already enabled\n";
          }
        }
        else if (isOnOffStr.Search ("del") >= 0 || isOnOffStr.Search ("rem") >= 0)
        {
          aPlaneV3d->Erase(); // not performed on destructor!!!
          aView->SetPlaneOff (aPlaneV3d);
          aViewer->DelPlane (aPlaneV3d);
          std::cout << "Clipping plane #" << aPlaneId << " was removed\n";
          if (toIterateAll)
          {
            for (aViewer->InitDefinedPlanes(); aViewer->MoreDefinedPlanes(); aViewer->InitDefinedPlanes(), ++aPlaneId)
            {
              aPlaneV3d = aViewer->DefinedPlane();
              aPlaneV3d->Erase(); // not performed on destructor!!!
              aView->SetPlaneOff (aPlaneV3d);
              aViewer->DelPlane (aPlaneV3d);
              std::cout << "Clipping plane #" << aPlaneId << " was removed\n";
            }
            break;
          }
          else
          {
            break;
          }
        }
        else if (isOnOffStr.Search ("disp") >= 0 || isOnOffStr.Search ("show") >= 0)
        {
          // avoid z-fighting glitches
          aView->SetPlaneOff (aPlaneV3d);
          aPlaneV3d->Display (aView);
          std::cout << "Clipping plane #" << aPlaneId << " was shown and disabled\n";
        }
        else if (isOnOffStr.Search ("hide") >= 0)
        {
          aPlaneV3d->Erase();
          std::cout << "Clipping plane #" << aPlaneId << " was hidden\n";
        }
        else
        {
          std::cout << "Usage: " << argv[0] << " [x y z dx dy dz] [planeId {on/off/del/display/hide}]\n";
          return 1;
        }
      }
    }
    if (aPlaneIdToOff >= aPlaneId && !toIterateAll)
    {
      std::cout << "Clipping plane with id " << aPlaneIdToOff << " not found!\n";
      return 1;
    }
    aView->Update();
    return 0;
  }
  else if (argc != 7)
  {
    std::cout << "Usage: " << argv[0] << " [x y z dx dy dz] [planeId {on/off/del/display/hide}]\n";
    return 1;
  }

  Standard_Real aLocX = atof (argv[1]);
  Standard_Real aLocY = atof (argv[2]);
  Standard_Real aLocZ = atof (argv[3]);
  Standard_Real aNormDX = atof (argv[4]);
  Standard_Real aNormDY = atof (argv[5]);
  Standard_Real aNormDZ = atof (argv[6]);

  Handle(V3d_Plane) aPlaneV3d = new V3d_Plane();
  gp_Pln aPlane (gp_Pnt (aLocX, aLocY, aLocZ), gp_Dir (aNormDX, aNormDY, aNormDZ));
  aPlane.Coefficients (coeffA, coeffB, coeffC, coeffD);
  aPlaneV3d->SetPlane(coeffA, coeffB, coeffC, coeffD);

  aViewer->AddPlane (aPlaneV3d); // add to defined planes list
  std::cout << "Added clipping plane #" << aNewPlaneId << "\n";
  if (anActivePlanes < aView->View()->PlaneLimit())
  {
    aView->SetPlaneOn (aPlaneV3d); // add to enabled planes list
    aView->Update();
  }
  else
  {
    std::cout << "Maximal active planes limit exceeded (" << anActivePlanes << ")\n"
              << "You should disable or remove some existing plane to activate the new one\n";
  }
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
       << "[ eye_x eye_y eye_z dir_x dir_y dir_z upx upy upz ]" << "\n"
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

    anEye.SetCoord (atof (argv[3]), atof (argv[4]), atof (argv[5]));
    aDir.SetCoord (atof (argv[6]), atof (argv[7]), atof (argv[8]));
    anUp.SetCoord (atof (argv[9]), atof (argv[10]), atof (argv[11]));
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
DEFINE_STANDARD_HANDLE(MyPArrayObject, AIS_InteractiveObject)
class MyPArrayObject : public AIS_InteractiveObject
{

public:

  MyPArrayObject (const Handle(Graphic3d_ArrayOfPrimitives) theArray)
  {
    myArray = theArray;
  }

  DEFINE_STANDARD_RTTI(MyPArrayObject);

private:

  void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                const Handle(Prs3d_Presentation)& aPresentation,
                const Standard_Integer aMode);

  void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection,
                         const Standard_Integer aMode) {};

protected:

  Handle(Graphic3d_ArrayOfPrimitives) myArray;

};

IMPLEMENT_STANDARD_HANDLE(MyPArrayObject, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(MyPArrayObject, AIS_InteractiveObject)

void MyPArrayObject::Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                              const Handle(Prs3d_Presentation)& aPresentation,
                              const Standard_Integer aMode)
{
  aPresentation->Clear();

  Handle (Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (aPresentation);
  aGroup->BeginPrimitives ();
  aGroup->AddPrimitiveArray (myArray);
  aGroup->EndPrimitives ();
}

static bool CheckInputCommand (const TCollection_AsciiString theCommand,
                               const char **theArgStr, int &theArgIndex,
                               int theArgCount, int theMaxArgs)
{
  // check if there is more elements than expected
  if (theArgIndex >= theMaxArgs)
    return false;

  TCollection_AsciiString aStrCommand(theArgStr[theArgIndex]);
  aStrCommand.LowerCase();
  if (aStrCommand.Search(theCommand) != 1 ||
      theArgIndex + (theArgCount - 1) >= theMaxArgs)
    return false;

  // go to the first data element
  theArgIndex++;

  // check data if it can be converted to numeric
  for (int aElement = 0; aElement < theArgCount; aElement++, theArgIndex++)
  {
    aStrCommand = theArgStr[theArgIndex];
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
    di << "Use: " << argv[0] << " Name TypeOfArray [EnableVBO={0 | 1}]"
       << " [vertex] ... [bounds] ... [edges]\n"
       << "  TypeOfArray={ points | segments | polylines | triangles |\n"
       << "                trianglefan | trianglestrips | quads |\n"
       << "                quadstrips | polygons }\n"
       << "  vertex={ 'v' x y z [normal={ 'n' nx ny nz }] [color={ 'c' r g b }]"
       << " [texel={ 't' tx ty }] } \n"
       << "  bounds={ 'b' verticies_count [color={ 'c' r g b }] }\n"
       << "  edges={ 'e' vertex_id [hidden_edge={'h'}] }\n";
    return 1;
  }

  // read the arguments
  TCollection_AsciiString aName (argv[1]);
  TCollection_AsciiString anArrayType (argv[2]);
  
  // is argument list has an vbo flag
  Standard_Boolean hasFlagVbo = Standard_False;
  if (isdigit (argv[3][0]) && atoi (argv[3]) >= 0 && atoi (argv[3]) <= 1)
    hasFlagVbo = Standard_True;

  // parse number of verticies, bounds, edges
  Standard_Integer aVertexNum = 0, aBoundNum = 0, aEdgeNum = 0;
  Standard_Boolean hasVColors, hasBColors, hasNormals, hasInfos, hasTexels;
  hasVColors = hasNormals = hasBColors = hasInfos = hasTexels = Standard_False;

  Standard_Integer aArgIndex = (hasFlagVbo) ? 4 : 3;
  TCollection_AsciiString aCommand;
  while (aArgIndex < argc)
  {
    aCommand = argv[aArgIndex];
    aCommand.LowerCase();
    if (!aCommand.IsAscii())
    {
      di << "Unexpected argument: #" << aArgIndex - 1 << " , "
         << "should be an array element: 'v', 'b', 'e' \n";
      break;
    }

    // vertex command
    if (CheckInputCommand ("v", argv, aArgIndex, 3, argc))
    {
      // vertex has a normal or normal with color or texel
      if (CheckInputCommand ("n", argv, aArgIndex, 3, argc))
        hasNormals = Standard_True;

      // vertex has a color
      if (CheckInputCommand ("c", argv, aArgIndex, 3, argc))
        hasVColors = Standard_True;

      // vertex has a texel
      if (CheckInputCommand ("t", argv, aArgIndex, 2, argc))
        hasTexels = Standard_True;

      aVertexNum++;
    }
    // bound command
    else if (CheckInputCommand ("b", argv, aArgIndex, 1, argc))
    {
      // bound has color
      if (CheckInputCommand ("c", argv, aArgIndex, 3, argc))
        hasBColors = Standard_True;

      aBoundNum++;
    }
    // edge command
    else if (CheckInputCommand ("e", argv, aArgIndex, 1, argc))
    {
      // edge has a hide flag
      if (CheckInputCommand ("h", argv, aArgIndex, 0, argc))
        hasInfos = Standard_True;

      aEdgeNum++;
    }
    // unknown command
    else
      aArgIndex++;
  }

  if (aVertexNum == 0)
  {
    di << "You should pass any verticies in the list of array elements\n";
    return 1;
  }

  // create an array of primitives by types
  Handle(Graphic3d_ArrayOfPrimitives) anArray;
  if (anArrayType == "points")
    anArray = new Graphic3d_ArrayOfPoints (aVertexNum);
  else if (anArrayType == "segments")
    anArray = new Graphic3d_ArrayOfSegments (aVertexNum, aEdgeNum, hasVColors);
  else if (anArrayType == "polylines")
    anArray = new Graphic3d_ArrayOfPolylines (aVertexNum, aBoundNum, aEdgeNum,
                                              hasVColors, hasBColors, hasInfos);
  else if (anArrayType == "triangles")
    anArray = new Graphic3d_ArrayOfTriangles (aVertexNum, aEdgeNum, hasNormals,
                                              hasVColors, hasTexels, hasInfos);
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
                                                hasTexels, hasInfos);
  else if (anArrayType == "quadstrips")
    anArray = new Graphic3d_ArrayOfQuadrangleStrips (aVertexNum, aBoundNum,
                                                     hasNormals, hasVColors,
                                                     hasBColors, hasTexels);
  else if (anArrayType == "polygons")
    anArray = new Graphic3d_ArrayOfPolygons (aVertexNum, aBoundNum, aEdgeNum,
                                             hasNormals, hasVColors, hasBColors,
                                             hasTexels, hasInfos);
  else
  {
    di << "Unexpected type of primitiives array\n";
    return 1;
  }

  // parse an array of primitives
  aArgIndex = (hasFlagVbo) ? 4 : 3;
  while (aArgIndex < argc)
  {
    aCommand = argv[aArgIndex];
    aCommand.LowerCase();
    if (!aCommand.IsAscii())
      break;

    // vertex command
    if (CheckInputCommand ("v", argv, aArgIndex, 3, argc))
    {
      anArray->AddVertex (atof (argv[aArgIndex - 3]),
                          atof (argv[aArgIndex - 2]),
                          atof (argv[aArgIndex - 1]));

      // vertex has a normal or normal with color or texel
      if (CheckInputCommand ("n", argv, aArgIndex, 3, argc))
        anArray->SetVertexNormal (anArray->VertexNumber (),
                                  atof (argv[aArgIndex - 3]),
                                  atof (argv[aArgIndex - 2]),
                                  atof (argv[aArgIndex - 1]));
      
      if (CheckInputCommand ("c", argv, aArgIndex, 3, argc))
        anArray->SetVertexColor (anArray->VertexNumber (),
                                 atof (argv[aArgIndex - 3]),
                                 atof (argv[aArgIndex - 2]),
                                 atof (argv[aArgIndex - 1]));
      
      if (CheckInputCommand ("t", argv, aArgIndex, 2, argc))
        anArray->SetVertexTexel (anArray->VertexNumber (),
                                 atof (argv[aArgIndex - 2]),
                                 atof (argv[aArgIndex - 1]));
    }
    // bounds command
    else if (CheckInputCommand ("b", argv, aArgIndex, 1, argc))
    {
      Standard_Integer aVertCount = atoi (argv[aArgIndex - 1]);

      if (CheckInputCommand ("c", argv, aArgIndex, 3, argc))
        anArray->AddBound (aVertCount,
                           atof (argv[aArgIndex - 3]),
                           atof (argv[aArgIndex - 2]),
                           atof (argv[aArgIndex - 1]));

      else
        anArray->AddBound (aVertCount);
    }
    // edge command
    else if (CheckInputCommand ("e", argv, aArgIndex, 1, argc))
    {
      Standard_Integer aVertIndex = atoi (argv[aArgIndex - 1]);

      // edge has/hasn't hide flag
      if (CheckInputCommand ("h", argv, aArgIndex, 0, argc))
        anArray->AddEdge (aVertIndex, Standard_False);
      else
        anArray->AddEdge (aVertIndex, Standard_True);
    }
    // unknown command
    else
      aArgIndex++;
  }

  if (hasFlagVbo)
  {
    // enable / disable vbo
    Handle(Graphic3d_GraphicDriver) aDriver =
      Handle(Graphic3d_GraphicDriver)::DownCast (
                      aContextAIS->CurrentViewer()->Device()->GraphicDriver());

    if (!aDriver.IsNull())
      aDriver->EnableVBO ((Standard_Boolean) atoi (argv[3]));
  }

  // create primitives array object
  Handle (MyPArrayObject) aPObject = new MyPArrayObject (anArray);

  // register the object in map
  VDisplayAISObject (aName, aPObject);

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
    "vtrihedron         : vtrihedron name [Xo] [Yo] [Zo] [Zu] [Zv] [Zw] [Xu] [Xv] [Xw] ",
    __FILE__,VTrihedron,group);

  theCommands.Add("vtri2d",
    "vtri2d Name Selection in the viewer only ",
    __FILE__,VTrihedron2D ,group);

  theCommands.Add("vplanetri",
    "vplanetri Name Selection in the viewer only ",
    __FILE__,VPlaneTrihedron ,group);

  theCommands.Add("vsize",
    "vsize       : vsize [name(Default=Current)] [size(Default=100)] ",
    __FILE__,VSize,group);

  theCommands.Add("vaxis",
    "vaxis nom [Xa] [Ya] [Za] [Xb] [Yb] [Zb]",
    __FILE__,VAxisBuilder,group);

  theCommands.Add("vaxispara",
    "vaxispara  nom ",
    __FILE__,VAxisBuilder,group);

  theCommands.Add("vaxisortho",
    "vaxisotho  nom ",
    __FILE__,VAxisBuilder,group);

  theCommands.Add("vpoint",
    "vpoint  PointName [Xa] [Ya] [Za] ",
    __FILE__,VPointBuilder,group);

  theCommands.Add("vplane",
    "vplane  PlaneName [AxisName/PlaneName/PointName] [PointName/PointName/PointName] [Nothing/Nothing/PointName] ",
    __FILE__,VPlaneBuilder,group);

  theCommands.Add("vplanepara",
    "vplanepara  PlaneName  ",
    __FILE__,VPlaneBuilder,group);

  theCommands.Add("vplaneortho",
    "vplaneortho  PlaneName  ",
    __FILE__,VPlaneBuilder,group);

  theCommands.Add("vline",
    "vline: vline LineName [Xa/PointName] [Ya/PointName] [Za] [Xb] [Yb] [Zb]  ",
    __FILE__,VLineBuilder,group);

  theCommands.Add("vcircle",
    "vcircle CircleName [PointName/PlaneName] [PointName] [Radius] ",
    __FILE__,VCircleBuilder,group);

  theCommands.Add("vdrawtext",
    "vdrawtext  : vdrawtext name X Y Z R G B hor_align ver_align angle zoomable height Aspect [Font [isMultiByte]]",
    __FILE__,VDrawText,group);

  theCommands.Add("vdrawsphere",
    "vdrawsphere: vdrawsphere shapeName Fineness [X=0.0 Y=0.0 Z=0.0] [Radius=100.0] [ToEnableVBO=1] [NumberOfViewerUpdate=1] [ToShowEdges=0]\n",
    __FILE__,VDrawSphere,group);

  theCommands.Add("vclipplane",
    "vclipplane : vclipplane [x y z dx dy dz] [planeId {on/off/del/display/hide}]",
    __FILE__,VClipPlane,group);

  theCommands.Add (
    "vcomputehlr",
    "vcomputehlr: shape hlrname [ eyex eyey eyez lookx looky lookz ]",
    __FILE__, VComputeHLR, group);

  theCommands.Add("vdrawparray",
    "vdrawparray : vdrawparray Name TypeOfArray [EnableVbo=1] [vertex = { 'v' x y z [vertex_normal = { 'n' x y z }] [vertex_color = { 'c' r g b }] ] ... [bound = { 'b' vertex_count [bound_color = { 'c' r g b }] ] ... [edge = { 'e' vertex_id [edge_hidden = { 'h' }] ]",
    __FILE__,VDrawPArray,group);
}
