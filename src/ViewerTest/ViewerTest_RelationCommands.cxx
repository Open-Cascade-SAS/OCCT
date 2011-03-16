// File:	ViewerTest_CstAndDimension.cxx
// Created:	Thu Nov 12 15:00:17 1998
// Author:	Robert COUBLANC
//		<rob@robox.paris1.matra-dtv.fr>


#include <ViewerTest.hxx>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <AIS_InteractiveContext.hxx>
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
#include <AIS_Point.hxx>
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

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>

#include <Draw_Window.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_DisplayMode.hxx>
#include <ElSLib.hxx>
#include <Geom_CartesianPoint.hxx>
#include <StdSelect.hxx>


extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
extern int ViewerMainLoop(Standard_Integer argc, const char** argv);
extern Handle(AIS_InteractiveContext)& TheAISContext ();


#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <AIS_AngleDimension.hxx>
#include <TCollection_ExtendedString.hxx>
#include <GC_MakePlane.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <Select3D_Projector.hxx>
#include <Precision.hxx>
#include <IntAna_Quadric.hxx>

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif



#define VertexMask 0x01
#define EdgeMask 0x02
#define FaceMask 0x04


static Standard_Boolean ComputeIntersection(const gp_Lin& L,const gp_Pln& ThePl, gp_Pnt& TheInter)
{
  static IntAna_Quadric TheQuad;
  TheQuad.SetQuadric(ThePl);
  static IntAna_IntConicQuad QQ;
   QQ.Perform(L,TheQuad);
  if(QQ.IsDone()){
    if(QQ.NbPoints()>0){
      TheInter = QQ.Point(1);
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : Get3DPointAtMousePosition
//purpose  : calcul du point 3D correspondant a la position souris dans le plan de 
// la vue...
//=======================================================================
static gp_Pnt Get3DPointAtMousePosition(){
  Handle(V3d_View) aview = ViewerTest::CurrentView();
  static Select3D_Projector prj;
  prj.SetView(aview);
  
  // le plan de la vue...
  Standard_Real xv,yv,zv;
  aview->Proj(xv,yv,zv);
  Standard_Real xat,yat,zat;
  aview->At(xat,yat,zat);
  gp_Pln ThePl(gp_Pnt(xat,yat,zat),gp_Dir(xv,yv,zv));
  Standard_Integer xpix,ypix;
  Standard_Real x,y;
  ViewerTest::GetMousePosition(xpix,ypix);
  aview->Convert(xpix,ypix,x,y); // espace reel 2D de la vue...
  gp_Lin L = prj.Shoot(x,y);
  gp_Pnt P(0.,0.,0.);

  ComputeIntersection(L,ThePl,P);
  return P;
}



//=======================================================================
//function : ComputeNewPlaneForDim
//purpose  : 
//=======================================================================
static void ComputeNewPlaneForDim(const Handle(AIS_Relation)& R,
				  gp_Pln& ,
				  gp_Pnt& )
{
// 0	COMPOUND,
// 1	COMPSOLID,
// 2	SOLID,
// 3	SHELL,
// 4	FACE,
// 5   	WIRE,
// 6	EDGE,
// 7	VERTEX,
// 8	SHAPE
  TopoDS_Shape S1 = R->FirstShape();
  TopoDS_Shape S2 = R->SecondShape();
  TopAbs_ShapeEnum Typ1 = S1.ShapeType();
  TopAbs_ShapeEnum Typ2 = S2.ShapeType();
  
  gp_Pnt thepoint [3];
  thepoint[0] = Get3DPointAtMousePosition();
  
  // on met l'objet le plus petit en 1...
  if((Standard_Integer)Typ2>(Standard_Integer)Typ1){
    
    TopoDS_Shape tmpS = S1;
    TopAbs_ShapeEnum tmpT = Typ1;
    S1= S2;
    Typ1 = Typ2;
    S2= tmpS;
    Typ2 = tmpT;
  }
/*  
  switch (Typ1){
  case TopAbs_VERTEX:{
    thepoint[0]  = BRep_Tool::Pnt(S1);
    if(Typ2==TopAbs_VERTEX)
      thepoint[1] = BRep_Tool::Pnt(S2);
    else if(Typ2==TopAbs_EDGE){
      TopoDS_Vertex  Va,Vb;
      TopExp::Vertices(S2,Va,Vb);
      thepoint[1] = BRep_Tool::Pnt(Va);
    }
    else if(Typ2==TopAbs_FACE){
    }
    break;
  }
  case TopAbs_EDGE:
  }
*/
}


//=======================================================================
//function : VAngleDimBuilder
//purpose  : 
//=======================================================================
static int VAngleDimBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  Standard_Integer myCurrentIndex;
  
  if (argc!=2) {di<<" vangledim error."<<"\n";return 1;}
  TheAISContext()->CloseAllContexts();
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  di<<" Select two  edges coplanar or not."<<"\n";
  Standard_Integer argcc = 5;
  const char *buff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argvv = (const char **) buff;
  while (ViewerMainLoop( argcc, argvv) ) { }
  
  TopoDS_Shape ShapeA;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeA = TheAISContext()->SelectedShape();
  }
  // Si ShapeA est un Edge.
  if (ShapeA.ShapeType()== TopAbs_EDGE ) {
    
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
    // ShapeB doit etre un Edge
    if (ShapeB.ShapeType()!= TopAbs_EDGE ) {
      di<<" vangledim error: you shoud have selected an edge."<<"\n";return 1;
    }
    
    // on recupere les vertexes de edgeA
    TopoDS_Vertex Va,Vb;
    TopExp::Vertices(TopoDS::Edge(ShapeA),Va ,Vb );
    // Recuperation des points.
    gp_Pnt A=BRep_Tool::Pnt(Va);
    gp_Pnt B=BRep_Tool::Pnt(Vb);
    gp_Pnt C(A.X()+5 ,A.Y()+5 ,A.Z()+5 );


    // Construction du plane. Methode pas orthodoxe!
    GC_MakePlane MkPlane(A ,B ,C );
    Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
    
    // Construction du texte.
    TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("d=")+TCollection_ExtendedString( 90 ) );
    
    // Fermeture du context local.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    
    // Construction de l'AIS dimension
    Handle (AIS_AngleDimension) myAISDim= new AIS_AngleDimension (TopoDS::Edge(ShapeA) ,TopoDS::Edge(ShapeB) ,theGeomPlane ,PI/2.0 ,TheMessage_Str );
    GetMapOfAIS().Bind (myAISDim,argv[1]);
    TheAISContext()->Display(myAISDim );
    
  }
else {
  di<<" vangledim error: you must select 2 edges."<<"\n";return 1;
}
  
  
  return 0;
}

//==============================================================================
//function : VDiameterDim
//purpose  : Display the diameter dimension of a face or an edge.
//Draw arg : vdiameterdim Name 
//==============================================================================
#include <AIS_DiameterDimension.hxx>
#include <TCollection_ExtendedString.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <gp_Circ.hxx>


static int VDiameterDimBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
  Standard_Real theRadius;
    
  // Verification
  if (argc!=2) {di<<" vdiameterdim error"<<"\n";return 1;}
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  
  // On active les modes de selections Edges et Faces.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  di<<" Select an circled edge."<<"\n";
  
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
  if (ShapeA.ShapeType()==TopAbs_EDGE  ) {
    
    // Recuperation du rayon 
    BRepAdaptor_Curve theCurve(TopoDS::Edge(ShapeA));
    if (theCurve.GetType()!=GeomAbs_Circle ) {di<<"vdiameterdim error: the edge is not a circular one."<<"\n";return 1;}
    else {
      gp_Circ theGpCircle=theCurve.Circle();
      theRadius=2.*theGpCircle.Radius();
    }
    
    // Construction du texte.
    TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("d=")+TCollection_ExtendedString(theRadius  ) );
    // Construction de L'AIS_AngleDimension.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    
    Handle (AIS_DiameterDimension) myDiamDim= new AIS_DiameterDimension(ShapeA ,theRadius ,TheMessage_Str );
    GetMapOfAIS().Bind (myDiamDim,argv[1]);
    TheAISContext()->Display(myDiamDim );
    
  }
  
  else {
    di<<" vdiameterdim error: the selection of a face or an edge was expected."<<"\n";return 1;
  }
  
  return 0;
  
}


//==============================================================================
// Fonction  vconcentric
// -----------------  Uniquement par selection dans le viewer.
//==============================================================================

//==============================================================================
//function : VConcentric
//purpose  : Display the concentric relation between two surfaces.
//Draw arg : vconcentric Name
//==============================================================================
#include <AIS_ConcentricRelation.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <GC_MakePlane.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TopExp_Explorer.hxx>


static int VConcentricBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
  
  // Verification
  if (argc!=2) {di<<"vconcentric  error."<<"\n";return 1;}
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  
  // On active les modes de selections Edges et Faces.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  di<<" Select a circled edge."<<"\n";
  
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
  // ShapeA est un edge
  // ==================
  if (ShapeA.ShapeType()==TopAbs_EDGE  ) {
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
      ShapeB = TheAISContext()->SelectedShape();
    }
    if (ShapeB.ShapeType()!=TopAbs_EDGE  ) {
      di<<" vconcentric error: select an edge."<<"\n";return 1;
    }
     
    // Construction du plane.
    // On recupere le centre du cercle A.
    BRepAdaptor_Curve theCurveA(TopoDS::Edge(ShapeA) );
    gp_Circ theCircleA=theCurveA.Circle();
    gp_Pnt theCenterA=theCircleA.Location();
    // On recupere deux points sur le cercle A
    gp_Pnt B= theCurveA.Value(0.25);
    gp_Pnt C= theCurveA.Value(0.75);
    // Construction du plane.
    GC_MakePlane MkPlane(theCenterA ,B ,C );
    Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
    
    // Fermeture du context local
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    
    Handle (AIS_ConcentricRelation) myConcentry= new AIS_ConcentricRelation(ShapeA, ShapeB, theGeomPlane );
    TheAISContext()->Display(myConcentry );
    GetMapOfAIS().Bind (myConcentry,argv[1]);
  }
  
  
  else {
    di<<" vconcentric  error: the selection of a face or an edge was expected."<<"\n";return 1;
  }
  
  return 0;
  
  
  
  
}

//==============================================================================
//function : VEqualDistRelation
//purpose  : 
//Draw arg : vdiameterdim Name DiameterValue
//==============================================================================
#include <AIS_EqualDistanceRelation.hxx>
#include <BRepExtrema_ExtCC.hxx>
#include <GC_MakePlane.hxx>


static int VEqualDistRelation(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
  
  // Verification
  if (argc!=2) {di<<" vequaldistrelation error: no arguments allowed."<<"\n";return 1;}
  
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  
  // On active les modes de selections Edges et Vertexes.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(1) );
  di<<" Select an edge or a vertex"<<"\n";
  
  // Boucle d'attente waitpick.
  Standard_Integer argc1 = 5;
  const char *buf1[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argv1 = (const char **) buf1;
  while (ViewerMainLoop( argc1, argv1) ) { }
  // fin de la boucle
  
  TopoDS_Shape ShapeA;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeA = TheAISContext()->SelectedShape();
  }
  
  di<<" Select an edge or a vertex"<<"\n";
  // Boucle d'attente waitpick.
  Standard_Integer argc2 = 5;
  const char *buf2[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argv2 = (const char **) buf2;
  while (ViewerMainLoop( argc2, argv2) ) { }
  // fin de la boucle
  
  TopoDS_Shape ShapeB;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeB = TheAISContext()->SelectedShape();
  }
  
  // Verification des resultats.
  if (ShapeA.ShapeType()==TopAbs_EDGE && ShapeB.ShapeType()==TopAbs_EDGE  ) {
    // A et B sont des edges ils doivent etre paralleles
    BRepExtrema_ExtCC myDeltaEdge (TopoDS::Edge(ShapeA) ,TopoDS::Edge(ShapeB)  );
    // on verifie qu'ils sont pas paralleles.
    if (!myDeltaEdge.IsParallel() ) {di<<"vequaldist error: non parallel edges."<<"\n";return 1; }
    
  }
  
  
  di<<" Select an edge or a vertex"<<"\n";
  // Boucle d'attente waitpick.
  Standard_Integer argc3 = 5;
  const char *buf3[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argv3 = (const char **) buf3;
  while (ViewerMainLoop( argc3, argv3) ) { }
  // fin de la boucle
  
  TopoDS_Shape ShapeC;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeC = TheAISContext()->SelectedShape();
  }
  
  di<<" Select an edge or a vertex"<<"\n";
  // Boucle d'attente waitpick.
  Standard_Integer argc4 = 5;
  const char *buf4[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argv4 = (const char **) buf4;
  while (ViewerMainLoop( argc4, argv4) ) { }
  // fin de la boucle
  
  TopoDS_Shape ShapeD;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeD = TheAISContext()->SelectedShape();
  }
  // Verification des resultats.
  if (ShapeC.ShapeType()==TopAbs_EDGE && ShapeD.ShapeType()==TopAbs_EDGE  ) {
    // C et D sont des edges ils doivent etre paralleles
    BRepExtrema_ExtCC myDeltaEdge2 (TopoDS::Edge(ShapeC) ,TopoDS::Edge(ShapeD)  );
    // on verifie qu'ils sont pas paralleles.
    if (!myDeltaEdge2.IsParallel() ) {di<<"vequaldist error: non parallel edges."<<"\n";return 1; }
    
  }
  
  // Creation du plan porteur de la contrainte.Methode lourde!
  gp_Pnt A,B,C;
  if (ShapeA.ShapeType()==TopAbs_EDGE) {
    // A est un edge
    TopoDS_Vertex Va,Vb;
    TopExp::Vertices (TopoDS::Edge(ShapeA) ,Va ,Vb );
    A=BRep_Tool::Pnt(Va);
    B=BRep_Tool::Pnt(Vb);
    
    if (ShapeB.ShapeType()==TopAbs_EDGE) {
      // B est un edge aussi
      TopoDS_Vertex Vc,Vd;
      TopExp::Vertices (TopoDS::Edge(ShapeB) ,Vc ,Vd );
      // besoin que de 1 point.
      C=BRep_Tool::Pnt(Vc);
      
    }
    else {
      // B est un vertex
      C=BRep_Tool::Pnt(TopoDS::Vertex(ShapeB) );
    }
  }
  else {
    // A est un vertex
    A=BRep_Tool::Pnt(TopoDS::Vertex(ShapeA) );
    
    if (ShapeB.ShapeType()==TopAbs_EDGE ) {
      // B est un edge 
      TopoDS_Vertex Vb,Vc;
      TopExp::Vertices (TopoDS::Edge(ShapeB) ,Vb ,Vc );
      // besoin que de 2 points.
      B=BRep_Tool::Pnt(Vb);
      C=BRep_Tool::Pnt(Vc);
      
    }
    else {
      // B est un vertex
      B=BRep_Tool::Pnt(TopoDS::Vertex(ShapeB) );
      C.SetX(B.X()+5.);
      C.SetY(B.Y()+5.);
      C.SetZ(B.Z()+5.);
      
    }
  }
  
  // Fermeture du context local.
  TheAISContext()->CloseLocalContext(myCurrentIndex);
  
  // construction du plane 
  GC_MakePlane MkPlane(A ,B ,C );
  Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
  
  // Construction de l'AIS_EqualDistanceRelation
  Handle (AIS_EqualDistanceRelation ) myRelation= new AIS_EqualDistanceRelation (ShapeA, ShapeB, ShapeC ,ShapeD , theGeomPlane );
  TheAISContext()->Display(myRelation );
  GetMapOfAIS().Bind (myRelation,argv[1]);
  

  
  return 0;
  
}

//==============================================================================
//function : VEqualRadiusRelation
//purpose  : 
//Draw arg : vdiameterdim Name DiameterValue
//==============================================================================
#include <AIS_EqualRadiusRelation.hxx>
#include <GC_MakePlane.hxx>
#include <BRepAdaptor_Curve.hxx>


static int VEqualRadiusRelation(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
  
  // Verification
  if (argc!=2) {di<<" vequalrad error: no arguments allowed."<<"\n";return 1;}
  
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  
  // On active les modes de selections Edges.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  di<<" Select an circled edge "<<"\n";
  
  // Boucle d'attente waitpick.
  Standard_Integer argc1 = 5;
  const char *buf1[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argv1 = (const char **) buf1;
  while (ViewerMainLoop( argc1, argv1) ) { }
  // fin de la boucle
  
  TopoDS_Shape ShapeA;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeA = TheAISContext()->SelectedShape();
  }
  
  di<<" Select the last circled edge."<<"\n";
  // Boucle d'attente waitpick.
  Standard_Integer argc2 = 5;
  const char *buf2[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argv2 = (const char **) buf2;
  while (ViewerMainLoop( argc2, argv2) ) { }
  // fin de la boucle
  
  TopoDS_Shape ShapeB;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeB = TheAISContext()->SelectedShape();
  }
  // creation du plan qui contient la contrainte.
  TopoDS_Edge EdgeA=TopoDS::Edge(ShapeA);
  TopoDS_Edge EdgeB=TopoDS::Edge(ShapeB);
  BRepAdaptor_Curve theCurveA(EdgeA);
  // On recupere 3 points A,B,C de la curve.
  gp_Pnt A=theCurveA.Value(0.1);
  gp_Pnt B=theCurveA.Value(0.5);
  gp_Pnt C=theCurveA.Value(0.9);
  
  // fermeture du contexte local.
  TheAISContext()->CloseLocalContext(myCurrentIndex);
  
  // Creation du plane.
  GC_MakePlane MkPlane (A ,B ,C );
  Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
  // Construction de l'AIS_EqualRadiusRelation
  Handle (AIS_EqualRadiusRelation ) myRelation= new AIS_EqualRadiusRelation (EdgeA,EdgeB, theGeomPlane );
  TheAISContext()->Display(myRelation );
  GetMapOfAIS().Bind (myRelation,argv[1]);
  
  return 0;
  
}


//==============================================================================
//function : VFixRelation
//purpose  : 
//Draw arg : vdiameterdim Name DiameterValue
//==============================================================================
#include <AIS_FixRelation.hxx>
#include <GC_MakePlane.hxx>
#include <BRepAdaptor_Curve.hxx>

static int VFixRelation(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
  
  // Verification
  if (argc!=2) {di<<" vfix  error: no arguments allowed."<<"\n";return 1;}
  
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  
  // On active les modes de selections edge.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  di<<" Select an edge. "<<"\n";
  
  // Boucle d'attente waitpick.
  Standard_Integer argc1 = 5;
  const char *buf1[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argv1 = (const char **) buf1;
  while (ViewerMainLoop( argc1, argv1) ) { }
  // fin de la boucle
  
  TopoDS_Shape ShapeA;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeA = TheAISContext()->SelectedShape();
  }
  
  // creation du plan qui contient la contrainte.
  TopoDS_Edge EdgeA=TopoDS::Edge(ShapeA);
  BRepAdaptor_Curve theCurveA(EdgeA);
  // On recupere 3 points A,B,C de la curve.
  gp_Pnt A=theCurveA.Value(0.1);
  gp_Pnt B=theCurveA.Value(0.5);
  gp_Pnt D=theCurveA.Value(0.9);
  gp_Pnt C(B.X()+5,B.Y()+5,B.Z()+5);
  
  // fermeture du contexte local.
  TheAISContext()->CloseLocalContext(myCurrentIndex);
  
  // Creation du plane.
  GC_MakePlane MkPlane (A ,D ,C );
  Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
  // Construction de l'AIS_EqualRadiusRelation
  Handle (AIS_FixRelation) myRelation= new AIS_FixRelation (EdgeA,theGeomPlane );
  TheAISContext()->Display(myRelation );
  GetMapOfAIS().Bind (myRelation,argv[1]);
  
  
  return 0;
  
}

//==============================================================================
//function : VIdenticRelation
//purpose  : 
//Draw arg : vdiameterdim Name DiameterValue
//==============================================================================
#include <AIS_IdenticRelation.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TopExp_Explorer.hxx>


static int VIdenticRelation(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
  
  // Verification
  if (argc!=2) {di<<" videntity error: no arguments allowed."<<"\n";return 1;}
  
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  
  // On active les modes de selections  vertex et face.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(1) );
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
  di<<" Select an edge, a face or a vertex. "<<"\n";
  
  // Boucle d'attente waitpick.
  Standard_Integer argc1 = 5;
  const char *buf1[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argv1 = (const char **) buf1;
  while (ViewerMainLoop( argc1, argv1) ) { }
  // fin de la boucle
  
  TopoDS_Shape ShapeA;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeA = TheAISContext()->SelectedShape();
  }
  
  di<<" Select an edge, a face or a vertex. "<<"\n";
  // Boucle d'attente waitpick.
  Standard_Integer argc2 = 5;
  const char *buf2[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argv2 = (const char **) buf2;
  while (ViewerMainLoop( argc2, argv2) ) { }
  // fin de la boucle
  
  TopoDS_Shape ShapeB;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeB = TheAISContext()->SelectedShape();
  }
  
  // Recuperation de points pour construir le Geom_Plnae.
  gp_Pnt A,B,C;
  if (ShapeA.ShapeType()==TopAbs_EDGE) {
    // A est un edge; on peut supposer qu'il sera element d'une face ou possesseur d'un vertex ou identic a un autre edge.
    // on recupere deux points sur l'edge (il peut etre rectiligne)
    TopoDS_Edge EdgeA=TopoDS::Edge(ShapeA);
    BRepAdaptor_Curve theCurveA(EdgeA);
    // Creation des 3 points.
    A=theCurveA.Value(0.1);
    B=theCurveA.Value(0.9);
    C.SetX(B.X()+5.);
    C.SetY(B.Y()+5.);
    C.SetZ(B.Z()+5.);
  }
  else if (ShapeA.ShapeType()==TopAbs_VERTEX ) {
    // SahpeA est un Vertex
    // On va utiliser ShapeB
    if (ShapeB.ShapeType()==TopAbs_EDGE) {
      // B est un edge 
      TopoDS_Edge EdgeB=TopoDS::Edge(ShapeB);
      BRepAdaptor_Curve theCurveB(EdgeB);
      // Creation des 3 points.
      A=theCurveB.Value(0.1);
      B=theCurveB.Value(0.9);
      C.SetX(B.X()+5.);
      C.SetY(B.Y()+5.);
      C.SetZ(B.Z()+5.);
      
    }
    else if (ShapeB.ShapeType()==TopAbs_FACE ) {
      // Shape B est une face
      TopoDS_Face  FaceB=TopoDS::Face(ShapeB);
      // On recupere 1 edge de FaceB(la face n'a pas forcement de vertex) (l'element A est forcement dans B).
      TopExp_Explorer FaceExp(FaceB,TopAbs_EDGE);
      TopoDS_Edge EdgeFromB=TopoDS::Edge(FaceExp.Current() );
      // On recupere les 3 points de l'edge de face B
      BRepAdaptor_Curve theCurveB(EdgeFromB);
      // On recupere 3 points A,B,C de la curve.
      A=theCurveB.Value(0.1);
      B=theCurveB.Value(0.5);
      C=theCurveB.Value(0.9);
      
    }
    else {
      // B ets un vetex aussi
      A=BRep_Tool::Pnt(TopoDS::Vertex(ShapeA) );
      B=BRep_Tool::Pnt(TopoDS::Vertex(ShapeB) );
      C.SetX(B.X()+5.);
      C.SetY(B.Y()+5.);
      C.SetZ(B.Z()+5.);
      
    }
    
  }
  else {
    // A est une face.
    TopoDS_Face  FaceA=TopoDS::Face(ShapeA);
    // On recupere 1 edge de FaceA
    TopExp_Explorer FaceExp(FaceA,TopAbs_EDGE);
    TopoDS_Edge EdgeFromA=TopoDS::Edge(FaceExp.Current() );
    // On recupere les 3 points de l'edge
    BRepAdaptor_Curve theCurveA(EdgeFromA);
    // On recupere 3 points A,B,C de la curve.
    A=theCurveA.Value(0.1);
    B=theCurveA.Value(0.5);
    C=theCurveA.Value(0.9);
    
  }
  
  // Fermeture du context local.
  TheAISContext()->CloseLocalContext(myCurrentIndex);
  // On construit le plane 
  GC_MakePlane MkPlane (A ,B ,C );
  Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
  
  // Construction de l'AIS_IdenticRelation
  Handle ( AIS_IdenticRelation ) myRelation= new AIS_IdenticRelation  (ShapeA ,ShapeB, theGeomPlane );
  TheAISContext()->Display(myRelation );
  GetMapOfAIS().Bind (myRelation,argv[1]);
  

  
  return 0;
  
}
//==============================================================================
//function : VLengthDimension
//purpose  : Display the diameter dimension of a face or an edge.
//Draw arg : vdiameterdim Name DiameterValue
//==============================================================================
#include <AIS_LengthDimension.hxx>
#include <BRepExtrema_ExtCC.hxx>
#include <BRepExtrema_ExtPC.hxx>
#include <BRepExtrema_ExtCF.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <BRepExtrema_ExtFF.hxx>
#include <TCollection_ExtendedString.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <GC_MakePlane.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
static int VLenghtDimension(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
  Standard_Real theDist;
  
  // Verification
  if (argc!=2) {di<<" videntity error: no arguments allowed."<<"\n";return 1;}
  
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  
  // Ouverture d'un contexte local et recuperation de son index.
  myCurrentIndex = TheAISContext()->OpenLocalContext();
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(1) );
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
  di<<" Select an edge, a face or a vertex. "<<"\n";
  
  // Boucle d'attente waitpick.
  Standard_Integer argc1 = 5;
  const char *buf1[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argv1 = (const char **) buf1;
  while (ViewerMainLoop( argc1, argv1) ) { }

  // fin de la boucle
  TopoDS_Shape ShapeA;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeA = TheAISContext()->SelectedShape();
  }
  di<<" Select an edge, a face or a vertex. "<<"\n";
  // Boucle d'attente waitpick.
  Standard_Integer argc2 = 5;
  const char *buf2[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argv2 = (const char **) buf2;
  while (ViewerMainLoop( argc2, argv2) ) { }
  // fin de la boucle
  
  TopoDS_Shape ShapeB;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeB = TheAISContext()->SelectedShape();
  }
  // ShapeA est un edge.
  // ===================
  if (ShapeA.ShapeType()==TopAbs_EDGE ) {
    TopoDS_Edge  EdgeA=TopoDS::Edge(ShapeA);
    
    // Si ShapeB est un edge
    if (ShapeB.ShapeType()==TopAbs_EDGE ) {
      TopoDS_Edge  EdgeB=TopoDS::Edge(ShapeB);
      BRepExtrema_ExtCC myDeltaEdge (EdgeA ,EdgeB );
      // on verifie qu'ils ne sont pas paralleles.
      if (!myDeltaEdge.IsParallel() ) {di<<"vlenghtdim error: non parallel edges."<<"\n";return 1; }
      
      // On saisit la distance et on l'arrondit!
      theDist=Round (sqrt(myDeltaEdge.SquareDistance(1))*10. )/10.;
      
      // On recupere 3 Points des edges pour construire un plane.
      TopoDS_Vertex  Va,Vb,Vc,Vd;
      TopExp::Vertices(EdgeA,Va,Vb);
      TopExp::Vertices(EdgeB,Vc,Vd);
      gp_Pnt A=BRep_Tool::Pnt(Va);
      gp_Pnt B=BRep_Tool::Pnt(Vb);
      gp_Pnt C=BRep_Tool::Pnt(Vc);
      
      // Creation du Plane contenant la relation.
      GC_MakePlane MkPlane(A,B,C);
      Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
      
      TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("d=")+TCollection_ExtendedString(theDist ) );
      
      // On ferme le contexte local.
      TheAISContext()->CloseLocalContext(myCurrentIndex);
      
      // on construit l'AISLenghtDimension.
      Handle(AIS_LengthDimension ) myLenghtDim=new AIS_LengthDimension (EdgeA,EdgeB,theGeomPlane,theDist,TheMessage_Str );
      TheAISContext()->Display(myLenghtDim );
      GetMapOfAIS().Bind (myLenghtDim ,argv[1]);
    }
    
    // Si ShapeB est un vertex.
    else if (ShapeB.ShapeType()==TopAbs_VERTEX ) {
      
      TopoDS_Vertex  VertexB=TopoDS::Vertex(ShapeB);
      BRepExtrema_ExtPC myDeltaEdgeVertex  (VertexB ,EdgeA );
      
      // On saisit la distance et on l'arrondit!
      theDist=Round (sqrt (myDeltaEdgeVertex.SquareDistance(1))*10. )/10.;
      
      // On recupere 2 Points de EdgeA pour construire un plane.
      TopoDS_Vertex  Va,Vb;
      TopExp::Vertices(EdgeA,Va,Vb);
      gp_Pnt A=BRep_Tool::Pnt(Va);
      gp_Pnt B=BRep_Tool::Pnt(Vb);
      gp_Pnt C=BRep_Tool::Pnt(VertexB);
      
      GC_MakePlane MkPlane(A,B,C);
      Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
      
      TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("d=")+TCollection_ExtendedString(theDist ) );
      TheAISContext()->CloseLocalContext(myCurrentIndex);
      Handle(AIS_LengthDimension ) myLenghtDim=new AIS_LengthDimension (EdgeA,VertexB,theGeomPlane,theDist,TheMessage_Str );
      TheAISContext()->Display(myLenghtDim );
      GetMapOfAIS().Bind (myLenghtDim ,argv[1]);
      
      
    }
    
    // Si ShapeB est une Face
    else {
      
      TopoDS_Face FaceB=TopoDS::Face(ShapeB);
      BRepExtrema_ExtCF myDeltaEdgeFace  (EdgeA,FaceB );
      // On verifie que l'edge est bien parallele a la face.
      if (!myDeltaEdgeFace.IsParallel() ) {di<<"vdistdim error: the edge isn't parallel to the face;can't compute the distance. "<<"\n";return 1; }
      
      // On saisit la distance et on l'arrondit!
      theDist=Round (sqrt (myDeltaEdgeFace.SquareDistance(1))*10. )/10.;
      
      // Construction du texte.
      TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("d=")+TCollection_ExtendedString(theDist ) );
      
      // on construit l'AISLenghtDimension.
      Handle(AIS_LengthDimension ) myLenghtDim=new AIS_LengthDimension (FaceB,EdgeA,theDist,TheMessage_Str );
      TheAISContext()->Display(myLenghtDim );
      GetMapOfAIS().Bind (myLenghtDim ,argv[1]);
      
    }
    
  }
  
  // ShapeA est un vertex
  // ====================
  if (ShapeA.ShapeType()==TopAbs_VERTEX ) {
    TopoDS_Vertex  VertexA=TopoDS::Vertex(ShapeA);
    
    // Si ShapeB est un edge.
    if (ShapeB.ShapeType()==TopAbs_EDGE ) {
      TopoDS_Edge  EdgeB=TopoDS::Edge(ShapeB);
      BRepExtrema_ExtPC myDeltaEdgeVertex  (VertexA ,EdgeB );
      // On saisit la distance et on l'arrondit!
      theDist=Round (sqrt (myDeltaEdgeVertex.SquareDistance(1))*10. )/10.;
      
      // On recupere 2 Points de EdgeB pour construire un plane.
      TopoDS_Vertex  Va,Vb;
      TopExp::Vertices(EdgeB,Va,Vb);
      gp_Pnt A=BRep_Tool::Pnt(Va);
      gp_Pnt B=BRep_Tool::Pnt(Vb);
      gp_Pnt C=BRep_Tool::Pnt(VertexA);
      
      // Creation du Plane contenant la relation.
      GC_MakePlane MkPlane(A,B,C);
      Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
      
      // Fermeture du contexte local.
      TheAISContext()->CloseLocalContext(myCurrentIndex);
      // Construction du texte.
      TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("d=")+TCollection_ExtendedString(theDist ) );
      
      // on construit l'AISLenghtDimension.
      Handle(AIS_LengthDimension ) myLenghtDim=new AIS_LengthDimension (EdgeB,VertexA,theGeomPlane,theDist,TheMessage_Str );
      TheAISContext()->Display(myLenghtDim );
      GetMapOfAIS().Bind (myLenghtDim ,argv[1]);
      
    }
    
    // Si ShapeB est un vertex.
    else if (ShapeB.ShapeType()==TopAbs_VERTEX ) {
      TopoDS_Vertex  VertexB=TopoDS::Vertex(ShapeB);
      BRepExtrema_DistShapeShape myDeltaVertexVertex  (VertexA ,VertexB );
      // On saisit la distance et on l'arrondit!
      theDist=Round (myDeltaVertexVertex.Value()*10. )/10.;
      
      // Les deux premiers points.
      gp_Pnt A=BRep_Tool::Pnt(VertexA);
      gp_Pnt B=BRep_Tool::Pnt(VertexB);
      gp_Pnt C(B.X()+10,B.Y()+10,B.Z()+10);

      GC_MakePlane MkPlane(A,B,C);
      Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
      
      // Fermeture du contexte local.
      TheAISContext()->CloseLocalContext(myCurrentIndex);
      // Construction du texte.
      TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("d=")+TCollection_ExtendedString(theDist ) );
      
      // on construit l'AISLenghtDimension.
      Handle(AIS_LengthDimension ) myLenghtDim=new AIS_LengthDimension (VertexA,VertexB,theGeomPlane,theDist,TheMessage_Str );
      TheAISContext()->Display(myLenghtDim );
      GetMapOfAIS().Bind (myLenghtDim ,argv[1]);
      
      
      
      
    }
    
    // Si ShapeB est une Face
    else {
      
      TopoDS_Face  FaceB=TopoDS::Face(ShapeB);
      BRepExtrema_ExtPF myDeltaVertexFace  (VertexA ,FaceB );
      // On saisit la distance et on l'arrondit!
      theDist=Round (sqrt (myDeltaVertexFace.SquareDistance(1))*10. )/10.;
      
      // Premier point.
      gp_Pnt A=BRep_Tool::Pnt(VertexA);
      
      // On recupere 1 edge de FaceB.
      TopExp_Explorer FaceExp(FaceB,TopAbs_EDGE);
      TopoDS_Edge EdFromB=TopoDS::Edge(FaceExp.Current() );
      // On recupere les deux vertexes extremites de l'edge de face B
      TopoDS_Vertex  Vb,Vc;
      TopExp::Vertices(EdFromB,Vb,Vc);
#ifdef DEB
      gp_Pnt B=
#endif
               BRep_Tool::Pnt(Vb);
      gp_Pnt C=BRep_Tool::Pnt(Vc);
      
      // On projette le point B sur la Face car il 
      // n'existe pas de constructeurs AIS_LD PointFace
      // on est donc oblige de creer un nouveau TopoDS_Vertex.
      gp_Pnt theProjA=myDeltaVertexFace.Point(1);
      BRepBuilderAPI_MakeVertex theVertexMaker(theProjA);
      TopoDS_Vertex VertexAproj=theVertexMaker.Vertex();
      // Creation du Plane contenant la relation.
      GC_MakePlane MkPlane(A,theProjA,C);
      Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
      
      // Fermeture du contexte local.
      TheAISContext()->CloseLocalContext(myCurrentIndex);
      // Construction du texte.
      TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("d=")+TCollection_ExtendedString(theDist ) );
      
      // on construit l'AISLenghtDimension.
      Handle(AIS_LengthDimension ) myLenghtDim=new AIS_LengthDimension (VertexA,VertexAproj,theGeomPlane,theDist,TheMessage_Str );
      TheAISContext()->Display(myLenghtDim );
      GetMapOfAIS().Bind (myLenghtDim ,argv[1]);
      
    }
    
    
    
  }
  
  // ShapeA est une Face
  // ===================
  else {
    TopoDS_Face  FaceA=TopoDS::Face(ShapeA);
    // Si ShapeB est un edge.
    if (ShapeB.ShapeType()==TopAbs_EDGE ) {
      TopoDS_Edge EdgeB=TopoDS::Edge(ShapeB);
      BRepExtrema_ExtCF myDeltaEdgeFace  (EdgeB,FaceA );
      // On verifie que l'edge est bien parallele a la face.
      if (!myDeltaEdgeFace.IsParallel() ) {di<<"vdistdim error: the edge isn't parallel to the face;can't compute the distance. "<<"\n";return 1; }
      
      // On saisit la distance et on l'arrondit!
      theDist=Round (sqrt (myDeltaEdgeFace.SquareDistance(1))*10. )/10.;
      
      // Construction du texte.
      TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("d=")+TCollection_ExtendedString(theDist ) );
      
      // on construit l'AISLenghtDimension.
      Handle(AIS_LengthDimension ) myLenghtDim=new AIS_LengthDimension (FaceA,EdgeB,theDist,TheMessage_Str );
      TheAISContext()->Display(myLenghtDim );
      GetMapOfAIS().Bind (myLenghtDim ,argv[1]);
      
    }
    
    // Si ShapeB est un vertex.
    else if (ShapeB.ShapeType()==TopAbs_VERTEX ) {
      
      TopoDS_Vertex  VertexB=TopoDS::Vertex(ShapeB);
      BRepExtrema_ExtPF myDeltaVertexFace  (VertexB ,FaceA );
      // On saisit la distance et on l'arrondit!
      theDist=Round (sqrt (myDeltaVertexFace.SquareDistance(1))*10. )/10.;
      
      // Premier point.
      gp_Pnt B=BRep_Tool::Pnt(VertexB);
      
      // On recupere 1 edge de FaceA.
      TopExp_Explorer FaceExp(FaceA,TopAbs_EDGE);
      TopoDS_Edge EdFromA=TopoDS::Edge(FaceExp.Current() );
      // On recupere les deux vertexes extremites de l'edge de face A
      TopoDS_Vertex  Va,Vc;
      TopExp::Vertices(EdFromA,Va,Vc);
      gp_Pnt A=BRep_Tool::Pnt(Va);
#ifdef DEB
      gp_Pnt C=
#endif
               BRep_Tool::Pnt(Vc);

      // On projette le point B sur la Face car il 
      // n'existe pas de constructeurs AIS_LD PointFace
      // on est donc oblige de creer un nouveau TopoDS_Vertex.
      gp_Pnt theProjB=myDeltaVertexFace.Point(1);
      BRepBuilderAPI_MakeVertex theVertexMaker(theProjB);
      TopoDS_Vertex VertexBproj=theVertexMaker.Vertex();
      // Creation du Plane contenant la relation.
      GC_MakePlane MkPlane(A,B,theProjB);
      Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
      
      // Fermeture du contexte local.
      TheAISContext()->CloseLocalContext(myCurrentIndex);
      // Construction du texte.
      TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("d=")+TCollection_ExtendedString(theDist ) );
      
      // on construit l'AISLenghtDimension mais en utilisant le constructeur Vertex Vertex.
      Handle(AIS_LengthDimension ) myLenghtDim=new AIS_LengthDimension (VertexB,VertexBproj,theGeomPlane,theDist,TheMessage_Str );
      TheAISContext()->Display(myLenghtDim );
      GetMapOfAIS().Bind (myLenghtDim ,argv[1]);
      
    }
    
    // Si ShapeB est une Face
    else {
      
      TopoDS_Face  FaceB=TopoDS::Face(ShapeB);
      BRepExtrema_ExtFF myDeltaFaceFace  (FaceA ,FaceB );
      // On verifie que les deux faces sont bien parelles.
      if (!myDeltaFaceFace.IsParallel() ) {di<<"vdistdim error: the faces are not parallel. "<<"\n";return 1; }
      
      // On saisit la distance et on l'arrondit!
      theDist=Round (sqrt (myDeltaFaceFace.SquareDistance(1))*10. )/10.;
      // Fermeture du contexte local.
      TheAISContext()->CloseLocalContext(myCurrentIndex);
      // Construction du texte.
      TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("d=")+TCollection_ExtendedString(theDist ) );
      
      // on construit l'AISLenghtDimension.
      Handle(AIS_LengthDimension ) myLenghtDim=new AIS_LengthDimension (FaceA,FaceB,theDist,TheMessage_Str );
      TheAISContext()->Display(myLenghtDim );
      GetMapOfAIS().Bind (myLenghtDim ,argv[1]);
      
    }
    
  }
  
  
  
  return 0;
  
}


//==============================================================================
//function : VRadiusDim
//purpose  : Display the radius dimension of a face or an edge.
//Draw arg : vradiusdim Name 
//==============================================================================
#include <AIS_RadiusDimension.hxx>
#include <TCollection_ExtendedString.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <gp_Circ.hxx>


static int VRadiusDimBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
  Standard_Real theRadius;
    
  // Verification
  if (argc!=2) {di<<" vradiusdim error"<<"\n";return 1;}
  
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  
  // On active les modes de selections Edges et Faces.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
  di<<" Select an circled edge or face."<<"\n";
  
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
  
  // Shape A est un edge.
  if (ShapeA.ShapeType()==TopAbs_EDGE  ) {
    
    TopoDS_Edge  EdgeA=TopoDS::Edge(ShapeA);
    // Recuperation du rayon
    BRepAdaptor_Curve theCurve(TopoDS::Edge(ShapeA));
    if (theCurve.GetType()!=GeomAbs_Circle ) {di<<"vradiusdim error: the edge is not a circular one."<<"\n";return 1;}
    else {
      gp_Circ theGpCircle=theCurve.Circle();
      theRadius=theGpCircle.Radius();
      // On arrondit le rayon
      theRadius=Round (theRadius*10. )/10.;
    }
    
    // Construction du texte.
    TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("r=")+TCollection_ExtendedString(theRadius  ) );
    
    // Fermeture du contexte.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    
    // Construction de L'AIS_RadiusDimension.
    Handle (AIS_RadiusDimension) myRadDim= new AIS_RadiusDimension(ShapeA ,theRadius ,TheMessage_Str );
    GetMapOfAIS().Bind (myRadDim,argv[1]);
    TheAISContext()->Display(myRadDim );
    
    
  }
  
  // Shape A est une face
  else if (ShapeA.ShapeType()==TopAbs_FACE ) {
    
    // on recupere un edge de la face.
    TopoDS_Face  FaceA=TopoDS::Face(ShapeA);
    // on explore.
    TopExp_Explorer FaceExp(FaceA,TopAbs_EDGE);
    TopoDS_Edge EdgeFromA=TopoDS::Edge(FaceExp.Current() );
    
    // Recuperation du rayon
    BRepAdaptor_Curve theCurve(EdgeFromA );
    if (theCurve.GetType()!=GeomAbs_Circle ) {di<<"vradiusdim error: the face is not a circular one."<<"\n";return 1;}
    else {
      gp_Circ theGpCircle=theCurve.Circle();
      theRadius=theGpCircle.Radius();
      // On arrondit le rayon
      theRadius=Round (theRadius*10. )/10.;
    }
    
    // Construction du texte.
    TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("r=")+TCollection_ExtendedString(theRadius  ) );
    
    // Fermeture du contexte.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    
    // Construction de L'AIS_RadiusDimension.
    Handle (AIS_RadiusDimension) myRadDim= new AIS_RadiusDimension(ShapeA ,theRadius ,TheMessage_Str );
    //Handle (AIS_MinRadiusDimension) myRadDim= new AIS_MinRadiusDimension(EdgeFromA ,theRadius ,TheMessage_Str );
    GetMapOfAIS().Bind (myRadDim,argv[1]);
    TheAISContext()->Display(myRadDim );
    
    
  }
  
  else {
    di<<" vradiusdim error: the selection of a face or an edge was expected."<<"\n";return 1;
  }
  
  return 0;
  
}



//==============================================================================
//function : VOffsetDim
//purpose  : Display the offset dimension
//Draw arg : voffsetdim Name 
//==============================================================================
#include <AIS_OffsetDimension.hxx>
#include <TCollection_ExtendedString.hxx>
#include <BRepExtrema_ExtFF.hxx>


static int VOffsetDimBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
  Standard_Real    theDist;
  
  // Verification
  if (argc!=2) {di<<" voffsetdim error"<<"\n";return 1;}
  
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  
  // On active les modes de selections Faces.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
  di<<" Select a face."<<"\n";
  
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
  
  di<<" Select a face."<<"\n";
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
  
  
  // Shape A et B est une face
  if (ShapeA.ShapeType()==TopAbs_FACE && ShapeB.ShapeType()==TopAbs_FACE ) {
    
    TopoDS_Face  FaceA=TopoDS::Face(ShapeA);
    TopoDS_Face  FaceB=TopoDS::Face(ShapeB);
    
    BRepExtrema_ExtFF myDeltaFaceFace  (FaceA ,FaceB );
    // On verifie que les deux faces sont bien parelles.
    if (!myDeltaFaceFace.IsParallel() ) {di<<"vdistdim error: the faces are not parallel. "<<"\n";return 1; }
    
    // On saisit la distance et on l'arrondit!
    theDist=Round (sqrt (myDeltaFaceFace.SquareDistance(1))*10. )/10.;
    // Fermeture du contexte local.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    // Construction du texte.
    TCollection_ExtendedString TheMessage_Str(TCollection_ExtendedString("offset=")+TCollection_ExtendedString(theDist ) );
    
    // on construit l'AIS_OffsetDimension
    Handle(AIS_OffsetDimension) myOffsetDim=new AIS_OffsetDimension (FaceA,FaceB,theDist,TheMessage_Str );
    TheAISContext()->Display(myOffsetDim );
    GetMapOfAIS().Bind (myOffsetDim ,argv[1]);
    
    
    
  }
  
  else {
    di<<" voffsetdim error: the selection of a face was expected."<<"\n";return 1;
  }
  
  return 0;
  
}




//==============================================================================
//function : VParallel
//purpose  : Display the parallel relation 
//Draw arg : vparallel Name 
//==============================================================================
#include <AIS_ParallelRelation.hxx>
#include <TCollection_ExtendedString.hxx>
#include <BRepExtrema_ExtFF.hxx>
#include <BRepExtrema_ExtCC.hxx>
#include <GC_MakePlane.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TopExp_Explorer.hxx>


static int VParallelBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
 
  // Verification
  if (argc!=2) {di<<" vparallel error"<<"\n";return 1;}
  
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  
  // On active les modes de selections Edges.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
  di<<" Select an edge or a face "<<"\n";
  
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
  
  // SahpeA est un edge.
  // ===================
  if (ShapeA.ShapeType()==TopAbs_EDGE ) {
    
    // desactivation du mode face
    TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4) );
    di<<" Select a second edge"<<"\n";
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
    
    // recuperation des edges.
    TopoDS_Edge  EdgeA=TopoDS::Edge(ShapeA);
    TopoDS_Edge  EdgeB=TopoDS::Edge(ShapeB);
    BRepExtrema_ExtCC myDeltaEdge (EdgeA ,EdgeB );
    // on verifie qu'ils ne sont pas paralleles.
    if (!myDeltaEdge.IsParallel() ) {di<<"vparallel error: non parallel edges."<<"\n";return 1; }
    
    
    // On recupere les  vertexes extremites des edge A et B.
    BRepAdaptor_Curve theCurveA(EdgeA);
    BRepAdaptor_Curve theCurveB(EdgeB);
    // On recupere 3 points A,B,C des  curves.
    gp_Pnt A=theCurveA.Value(0.1);
    gp_Pnt B=theCurveA.Value(0.9);
    gp_Pnt C=theCurveB.Value(0.5);
    
    // Construction du Geom_Plane
    GC_MakePlane MkPlane(A,B,C);
    Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
    // Fermeture du contexte local.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    // Construction de l'AIS_ParallelRelation
    Handle(AIS_ParallelRelation) myParaRelation= new AIS_ParallelRelation(EdgeA ,EdgeB ,theGeomPlane );
    TheAISContext()->Display(myParaRelation );
    GetMapOfAIS().Bind (myParaRelation ,argv[1]);
    
    
  }
  
  // Shape A est une face
  // ====================
  else {
    
    // desactivation du mode edge
    TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(2) );
    di<<" Select a second edge"<<"\n";
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
    
    TopoDS_Face  FaceA=TopoDS::Face(ShapeA);
    TopoDS_Face  FaceB=TopoDS::Face(ShapeB);
    
    BRepExtrema_ExtFF myDeltaFaceFace  (FaceA ,FaceB );
    // On verifie que les deux faces sont bien parelles.
    if (!myDeltaFaceFace.IsParallel() ) {di<<"vdistdim error: the faces are not parallel. "<<"\n";return 1; }
    
    // recuperation des edges des faces.
    TopExp_Explorer FaceExpA(FaceA,TopAbs_EDGE);
    TopExp_Explorer FaceExpB(FaceB,TopAbs_EDGE);
    
    TopoDS_Edge EdgeA=TopoDS::Edge(FaceExpA.Current() );
    TopoDS_Edge EdgeB=TopoDS::Edge(FaceExpB.Current() );
    
    // On recupere les  vertexes extremites des edge A et B.
    BRepAdaptor_Curve theCurveA(EdgeA);
    BRepAdaptor_Curve theCurveB(EdgeB);
    // On recupere 3 points A,B,C des  curves.
    gp_Pnt A=theCurveA.Value(0.1);
    gp_Pnt B=theCurveA.Value(0.9);
    gp_Pnt C=theCurveB.Value(0.5);
    
    // Construction du Geom_Plane
    GC_MakePlane MkPlane(A,B,C);
    Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
    // Fermeture du contexte local.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    // Construction de l'AIS_ParallelRelation
    Handle(AIS_ParallelRelation) myParaRelation= new AIS_ParallelRelation(FaceA ,FaceB ,theGeomPlane );
    TheAISContext()->Display(myParaRelation );
    GetMapOfAIS().Bind (myParaRelation ,argv[1]);
    
    
    
  }
  
  
  return 0;
  
}




//==============================================================================
//function : VPerpendicularRelation
//purpose  : Display the Perpendicular Relation
//Draw arg : vperpendicular Name 
//==============================================================================
#include <AIS_PerpendicularRelation.hxx>
#include <TCollection_ExtendedString.hxx>
#include <GC_MakePlane.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TopExp_Explorer.hxx>



static int VPerpendicularBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
 
  // Verification
  if (argc!=2) {di<<" vortho error"<<"\n";return 1;}
  
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  
  // On active les modes de selections Edges.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
  di<<" Select an edge or a face "<<"\n";
  
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
  
  // ShapeA est un edge.
  // ===================
  if (ShapeA.ShapeType()==TopAbs_EDGE ) {
    
    // desactivation du mode face
    TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4) );
    di<<" Select a second edge"<<"\n";
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
    
    // recuperation des edges.
    TopoDS_Edge  EdgeA=TopoDS::Edge(ShapeA);
    TopoDS_Edge  EdgeB=TopoDS::Edge(ShapeB);
    
    // On recupere les  curves
    BRepAdaptor_Curve theCurveA(EdgeA);
    BRepAdaptor_Curve theCurveB(EdgeB);
    // on verifie si les edges sont orthogonaux.
    //gp_Lin theLineA=theCurveA.Line();
    //gp_Lin theLineB=theCurveB.Line();
    //if (abs(theLineA.Angle(theLineB) ) != PI/2 ) {cout<<"vperpendicular error: Edges are not  othogonals."<<endl;return 1;}
    
    // On recupere 3 points A,B,C des  curves.
    gp_Pnt A=theCurveA.Value(0.1);
    gp_Pnt B=theCurveA.Value(0.9);
    gp_Pnt C=theCurveB.Value(0.5);
    // Construction du Geom_Plane
    GC_MakePlane MkPlane(A,B,C);
    Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
    // Fermeture du contexte local.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    // Construction de l'AIS_ParallelRelation
    Handle(AIS_PerpendicularRelation) myOrthoRelation= new AIS_PerpendicularRelation (EdgeA ,EdgeB ,theGeomPlane );
    TheAISContext()->Display(myOrthoRelation );
    GetMapOfAIS().Bind (myOrthoRelation ,argv[1]);
    
    
  }
  
  // Shape A est une face
  // ====================
  else {
    
    // desactivation du mode edge
    TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(2) );
    di<<" Select a second edge"<<"\n";
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
    
    // pas de verification d'orthogonalite.
    TopoDS_Face  FaceA=TopoDS::Face(ShapeA);
    TopoDS_Face  FaceB=TopoDS::Face(ShapeB);
    
    // recuperation des edges des faces.
    TopExp_Explorer FaceExpA(FaceA,TopAbs_EDGE);
    TopExp_Explorer FaceExpB(FaceB,TopAbs_EDGE);
    
    TopoDS_Edge EdgeA=TopoDS::Edge(FaceExpA.Current() );
    TopoDS_Edge EdgeB=TopoDS::Edge(FaceExpB.Current() );
    
    // On recupere les  vertexes extremites des edge A et B.
    BRepAdaptor_Curve theCurveA(EdgeA);
    BRepAdaptor_Curve theCurveB(EdgeB);
    // On recupere 3 points A,B,C des  curves.
    gp_Pnt A=theCurveA.Value(0.1);
    gp_Pnt B=theCurveA.Value(0.9);
    gp_Pnt C=theCurveB.Value(0.5);
    // Construction du Geom_Plane
    GC_MakePlane MkPlane(A,B,C);
    Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
    // Fermeture du contexte local.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    // Construction de l'AIS_PerpendicularRelation
    Handle(AIS_PerpendicularRelation) myOrthoRelation= new AIS_PerpendicularRelation(FaceA ,FaceB );
    TheAISContext()->Display(myOrthoRelation );
    GetMapOfAIS().Bind (myOrthoRelation  ,argv[1]);
    
    
    
  }
  
  
  return 0;
  
}


//==============================================================================
//function : VTangentRelation
//purpose  : Display the tangent Relation
//Draw arg : vtangent Name 
//==============================================================================
#include <AIS_TangentRelation.hxx>


static int VTangentBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
 
  // Verification
  if (argc!=2) {di<<" vtangent error"<<"\n";return 1;}
  
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  
  // On active les modes de selections Edges.
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(4) );
  di<<" Select two coplanar edges(First the circular edge then the tangent edge) or two faces "<<"\n";
  
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
  
  // ShapeA est un edge.
  // ===================
  if (ShapeA.ShapeType()==TopAbs_EDGE ) {
    
    // desactivation du mode face
    TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(4) );
    di<<" Select a second edge"<<"\n";
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
    
    // recuperation des edges.
    TopoDS_Edge  EdgeA=TopoDS::Edge(ShapeA);
    TopoDS_Edge  EdgeB=TopoDS::Edge(ShapeB);
    
    // On recupere les  curves
    BRepAdaptor_Curve theCurveA(EdgeA);
    BRepAdaptor_Curve theCurveB(EdgeB);
    
    // On recupere 3 points A,B,C des  curves.
    gp_Pnt A=theCurveA.Value(0.1);
    gp_Pnt B=theCurveA.Value(0.9);
    gp_Pnt C=theCurveB.Value(0.5);

    // Construction du Geom_Plane
    GC_MakePlane MkPlane(A,B,C);
    Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
    // Fermeture du contexte local.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    // Construction de l'AIS_TangentRelation
    Handle(AIS_TangentRelation) myTanRelation= new AIS_TangentRelation  (EdgeA ,EdgeB ,theGeomPlane );
    TheAISContext()->Display(myTanRelation );
    GetMapOfAIS().Bind (myTanRelation ,argv[1]);
    
    
  }
  
  // Shape A est une face
  // ====================
  else {
    
    // desactivation du mode edge
    TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(2) );
    di<<" Select a second edge"<<"\n";
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
    
    TopoDS_Face  FaceA=TopoDS::Face(ShapeA);
    TopoDS_Face  FaceB=TopoDS::Face(ShapeB);
    
    // recuperation des edges des faces.
    TopExp_Explorer FaceExpA(FaceA,TopAbs_EDGE);
    TopExp_Explorer FaceExpB(FaceB,TopAbs_EDGE);
    
    TopoDS_Edge EdgeA=TopoDS::Edge(FaceExpA.Current() );
    TopoDS_Edge EdgeB=TopoDS::Edge(FaceExpB.Current() );
    
    // On recupere les  vertexes extremites des edge A et B.
    BRepAdaptor_Curve theCurveA(EdgeA);
    BRepAdaptor_Curve theCurveB(EdgeB);
    // On recupere 3 points A,B,C des  curves.
    gp_Pnt A=theCurveA.Value(0.1);
    gp_Pnt B=theCurveA.Value(0.9);
    gp_Pnt C=theCurveB.Value(0.5);

    // Construction du Geom_Plane
    GC_MakePlane MkPlane(A,B,C);
    Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
    // Fermeture du contexte local.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    // Construction de l'AIS_PerpendicularRelation
    Handle(AIS_TangentRelation) myTanRelation= new AIS_TangentRelation(FaceA ,FaceB,theGeomPlane );
    TheAISContext()->Display(myTanRelation );
    GetMapOfAIS().Bind (myTanRelation  ,argv[1]);
    
    
    
  }
  
  
  return 0;
  
}

//==============================================================================
//function : VSymetricalRelation
//purpose  : Display the Symetrical Relation
//Draw arg : vsymetric Name 
//==============================================================================
#include <AIS_SymmetricRelation.hxx>


static int VSymmetricBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  // Declarations
  Standard_Integer myCurrentIndex;
 
  // Verification
  if (argc!=2) {di<<" vSymmetric error"<<"\n";return 1;}
  
  // Fermeture des contextes locaux
  TheAISContext()->CloseAllContexts();
  
  // Ouverture d'un contexte local et recuperation de son index.
  TheAISContext()->OpenLocalContext();
  myCurrentIndex=TheAISContext()->IndexOfCurrentLocal();
  
  // On active les modes de selections
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  di<<" Select an edge:the axis of symetry "<<"\n";
  
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
  // recuperation des edges.
  TopoDS_Edge  EdgeA=TopoDS::Edge(ShapeA);
  
  // On active les modes de selections
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(2) );
  TheAISContext()->ActivateStandardMode (AIS_Shape::SelectionType(1) );
  di<<" Select two edges or two vertices. "<<"\n";
  
  // Boucle d'attente waitpick.
  Standard_Integer argcc2 = 5;
//  const char *buff2[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argvv2 = (const char **) buff;
  while (ViewerMainLoop( argcc2, argvv2) ) { }
  // fin de la boucle
  
  TopoDS_Shape ShapeB;
  for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
    ShapeB = TheAISContext()->SelectedShape();
  }
  
  
  
  
  
  // ShapeB est un edge.
  // ===================
  if (ShapeB.ShapeType()==TopAbs_EDGE ) {
    
    // desactivation du mode vertex
    TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(1) );
    di<<" Select a second edge"<<"\n";
    // Boucle d'attente waitpick.
    Standard_Integer argccc = 5;
    const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
    const char **argvvv = (const char **) bufff;
    while (ViewerMainLoop( argccc, argvvv) ) { }
    // fin de la boucle
    
    TopoDS_Shape ShapeC;
    for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
      ShapeC = TheAISContext()->SelectedShape();
    }
    
    // recuperation des edges.
    TopoDS_Edge  EdgeB=TopoDS::Edge(ShapeB);
    TopoDS_Edge  EdgeC=TopoDS::Edge(ShapeC);
    // on verifie que les edges sont paralleles
    BRepExtrema_ExtCC myDeltaEdgeAB (EdgeA ,EdgeB );
    BRepExtrema_ExtCC myDeltaEdgeAC (EdgeA ,EdgeC );
    // on verifie qu'ils  sont paralleles.
    if (!myDeltaEdgeAB.IsParallel() ) {di<<"vsymetric error: non parallel edges."<<"\n";return 1; }
    if (!myDeltaEdgeAC.IsParallel() ) {di<<"vsymetric error: non parallel edges."<<"\n";return 1; }
    // on recupere les vertexs
    TopoDS_Vertex  Va,Vb,Vc,Vd;
    TopExp::Vertices(EdgeB,Va,Vb );
    TopExp::Vertices(EdgeC,Vc,Vd );
    gp_Pnt A=BRep_Tool::Pnt(Va);
    gp_Pnt B=BRep_Tool::Pnt(Vc);
    gp_Pnt C = Get3DPointAtMousePosition();
    
    //    gp_Pnt C=BRep_Tool::Pnt(Vc);
    // Construction du Geom_Plane
    GC_MakePlane MkPlane(A,B,C);
    Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
    // Fermeture du contexte local.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    // Construction de l'AIS_SymmetricRelation
    Handle(AIS_SymmetricRelation) mySymRelation= new AIS_SymmetricRelation (EdgeA ,EdgeB ,EdgeC, theGeomPlane );
    TheAISContext()->Display(mySymRelation );
    GetMapOfAIS().Bind (mySymRelation ,argv[1]);
    
    
  }
  
  // Shape B est un vertex
  // =====================
  else {
    
    // desactivation du mode edge
    TheAISContext()->DeactivateStandardMode (AIS_Shape::SelectionType(2) );
    di<<" Select a second edge"<<"\n";
    // Boucle d'attente waitpick.
    Standard_Integer argccc = 5;
    const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
    const char **argvvv = (const char **) bufff;
    while (ViewerMainLoop( argccc, argvvv) ) { }
    // fin de la boucle
    
    TopoDS_Shape ShapeC;
    for(TheAISContext()->InitSelected() ;TheAISContext()->MoreSelected() ;TheAISContext()->NextSelected() ) {
      ShapeC = TheAISContext()->SelectedShape();
    }
    
    // recuperation des Vertex
    TopoDS_Vertex  VertexB=TopoDS::Vertex(ShapeB);
    TopoDS_Vertex  VertexC=TopoDS::Vertex(ShapeC);
    // transfo en gp_Pnt
    gp_Pnt B=BRep_Tool::Pnt(VertexB);
    gp_Pnt C=BRep_Tool::Pnt(VertexC);
    
    // on recupere les vertexes de l'axe de sym
    TopoDS_Vertex  Va,Vb;
    TopExp::Vertices(EdgeA,Va,Vb );
    gp_Pnt A=BRep_Tool::Pnt(Va);
    // Construction du Geom_Plane
    GC_MakePlane MkPlane(A,B,C);
    Handle(Geom_Plane) theGeomPlane=MkPlane.Value();
    // Fermeture du contexte local.
    TheAISContext()->CloseLocalContext(myCurrentIndex);
    // Construction de l'AIS_SymmetricRelation
    Handle(AIS_SymmetricRelation) mySymRelation= new AIS_SymmetricRelation (EdgeA ,VertexB ,VertexC, theGeomPlane );
    TheAISContext()->Display(mySymRelation );
    GetMapOfAIS().Bind (mySymRelation ,argv[1]);
    
    
    
  }
  
  
  return 0;
  
}
//=======================================================================
//function : VMoveDim
//purpose  : 
//=======================================================================
static int VMoveDim(Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  if(argc>2) return 1;
  
  const Handle(V3d_View) aview = ViewerTest::CurrentView();
  Handle(AIS_InteractiveObject) pickedobj;
  if(argc==1){
    pickedobj = TheAISContext()->FirstCurrentObject();
    if(pickedobj.IsNull() || pickedobj->Type()!=AIS_KOI_Relation)
      pickedobj = ViewerTest::PickObject(AIS_KOI_Relation);
  }
  else{
    // reperage dans le viewer...
    if(!strcasecmp(argv[1],".")){
      pickedobj = ViewerTest::PickObject(AIS_KOI_Relation);
      
    }
    else if(GetMapOfAIS().IsBound2(argv[1]))
      pickedobj = Handle(AIS_InteractiveObject)::DownCast
        (GetMapOfAIS().Find2(argv[1]));
  }
  
  if(pickedobj.IsNull()){
    di<<"Bad Type Object"<<"\n";
    return 1 ;}
  
  if(pickedobj->Type() != AIS_KOI_Relation)
    return 1;
  Standard_Integer argccc = 5;
  
  const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argvvv = (const char **) bufff;

  Handle(AIS_Relation) R = *((Handle(AIS_Relation)*)&pickedobj);
  Handle(Geom_Plane) ThePl;
//  Standard_Real x,y,z,xv,yv,zv;
  Standard_Real x,y,xv,yv,zv;
  static Standard_Real last_xv(0),last_yv(0),last_zv(0);
  Standard_Integer xpix,ypix;
//  Standard_Boolean SameAsLast(Standard_False);
  Select3D_Projector prj(aview);
  
  
  while (ViewerMainLoop( argccc, argvvv) ) {
    //....... la ligne de tir
    ViewerTest::GetMousePosition(xpix,ypix);
    aview->Convert(xpix,ypix,x,y); // espace reel 2D de la vue...
    gp_Lin L = prj.Shoot(x,y);

    
    
    // ....... le plan de la vue...
    aview->Proj(xv,yv,zv);
    static Standard_Boolean haschanged(Standard_False);
    if(Abs(last_xv-xv)>Precision::Confusion() ||
       Abs(last_yv-yv)>Precision::Confusion() ||
       Abs(last_zv-zv)>Precision::Confusion() ){
      last_xv = xv;
      last_yv = yv;
      last_zv = zv;
      Standard_Real xat,yat,zat;
      aview->At(xat,yat,zat);
      ThePl = new Geom_Plane(gp_Pnt(xat,yat,zat),gp_Dir(xv,yv,zv));
      haschanged = Standard_True;
      di <<"changement de plan"<<"\n";
    }
    
    //  
//    Standard_Integer xpix,ypix;
//    Standard_Real x,y;
    gp_Pnt GoodPoint;
    if(haschanged){
      gp_Pln NewPlane;;
      ComputeNewPlaneForDim(R,NewPlane,GoodPoint);
      haschanged = Standard_False;
    }
    else{
      if(ComputeIntersection(L,ThePl->Pln(),GoodPoint)){
	R->SetPosition(GoodPoint);
      }
      TheAISContext()->Redisplay(R);
    }
  }
  
  return 0;
}
//=======================================================================
//function : RelationsCommands
//purpose  : 
//=======================================================================
      

void ViewerTest::RelationCommands(Draw_Interpretor& theCommands)
{
  const char *group = "AISRelations";

  
  theCommands.Add("vangledim",
		  "vangledim Name:Selection in the viewer only ",
		  __FILE__,VAngleDimBuilder,group);
  
  theCommands.Add("vdiameterdim",
		  "vdiameterdim Name : Selection in the viewer only ",
		  __FILE__,VDiameterDimBuilder,group);
  
  theCommands.Add("vconcentric",
		  "vconcentric Name : Selection in the viewer only ",
		  __FILE__,VConcentricBuilder,group);

  theCommands.Add("vequaldist",
		  "vequaldist Name Selection in the viewer only ",
		  __FILE__,VEqualDistRelation ,group);
  
  theCommands.Add("vequalrad",
		  "vequalrad Name Selection in the viewer only ",
		  __FILE__,VEqualRadiusRelation  ,group);
  
  theCommands.Add("vfix",  
		  "vfix Name Selection in the viewer only ",
		  __FILE__,VFixRelation  ,group);
  
  theCommands.Add("videntity",
		  "videntity Name Selection in the viewer only ",
		  __FILE__,VIdenticRelation  ,group);
  
  theCommands.Add("vdistdim",
		  "vdistdim Name Selection in the viewer only ",
		  __FILE__,VLenghtDimension ,group);
  
  theCommands.Add("vradiusdim",
		  "vradiusdim Name Selection in the viewer only ",
		  __FILE__,VRadiusDimBuilder ,group);
  
  theCommands.Add("voffsetdim",
		  "voffsetdim Name Selection in the viewer only ",
		  __FILE__,VOffsetDimBuilder ,group);
  
  theCommands.Add("vparallel",
		  "vparallel Name Selection in the viewer only ",
		  __FILE__,VParallelBuilder ,group);
  
  theCommands.Add("vortho",
		  "vortho Name Selection in the viewer only ",
		  __FILE__,VPerpendicularBuilder ,group);
  
  theCommands.Add("vtangent",  
		  "vtangent Name Selection in the viewer only ",
		  __FILE__,VTangentBuilder ,group);
  
  
  theCommands.Add("vsymetric",
		  "vsymetric Name Selection in the viewer only ",
		  __FILE__,VSymmetricBuilder ,group);

  theCommands.Add("vmovedim",
		  "vmovedim [name] : move Mouse in the viewer; click MB1 to stop motion...",
		  __FILE__,VMoveDim,group);

}
