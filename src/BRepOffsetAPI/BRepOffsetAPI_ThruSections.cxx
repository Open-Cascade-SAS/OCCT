// File:	BRepOffsetAPI_ThruSections.cxx
// Created:	Tue Jul 18 16:18:09 1995
// Author:	Jing Cheng MEI
//		<mei@junon>
// Modified:	Mon Jan 12 10:50:10 1998
// Author:	Joelle CHAUVET
//		<jct@sgi64>
//              gestion automatique de l'origine et de l'orientation
//              avec la methode ArrangeWires
// Modified:	Mon Jan 19 10:11:56 1998
// Author:	Joelle CHAUVET
//		<jct@sgi64>
//              traitement des cas particuliers cylindre, cone, plan 
//              (methodes DetectKPart et CreateKPart)
// Modified:	Mon Feb 23 09:28:46 1998
// Author:	Joelle CHAUVET
//		<jct@sgi64>
//              traitement des sections avec nombre d'elements different
//              + quelques ameliorations pour les cas particuliers
//              + cas de la derniere section ponctuelle
// Modified:	Mon Apr  6 15:47:44 1998
// Author:	Joelle CHAUVET
//		<jct@sgi64>
//              traitement des cas particuliers deplace dans BRepFill 
// Modified:	Thu Apr 30 15:24:17 1998
// Author:	Joelle CHAUVET
//		<jct@sgi64>
//              separation sections fermees / sections ouvertes + debug 
// Modified:	Fri Jul 10 11:23:35 1998
// Author:	Joelle CHAUVET
//		<jct@sgi64>
//              surface de CreateSmoothed par concatenation,approximation
//              et segmentation (PRO13924, CTS21295)
// Modified:	Tue Jul 21 16:48:35 1998
// Author:	Joelle CHAUVET
//		<jct@sgi64>
//              pb de ratio (BUC60281) 
// Modified:	Thu Jul 23 11:38:36 1998
// Author:	Joelle CHAUVET
//		<jct@sgi64>
//              sections bouclantes
// Modified:	Fri Aug 28 10:13:44 1998
// Author:	Joelle CHAUVET
//		<jct@sgi64>
//              traitement des sections ponctuelles
//              dans l'historique (cf. loft06 et loft09)
//              et dans le cas des solides
// Modified:	Tue Nov  3 10:06:15 1998
// Author:	Joelle CHAUVET
//		<jct@sgi64>
//              utilisation de BRepFill_CompatibleWires
 

#include <BRepOffsetAPI_ThruSections.ixx>

#include <Precision.hxx>
#include <Standard_DomainError.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <TColgp_Array1OfPnt.hxx>

#include <GeomAbs_Shape.hxx>
#include <Geom_Curve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Conic.hxx>
#include <Geom2d_Line.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <BSplCLib.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopExp.hxx>
#include <TopoDS_Iterator.hxx>


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools_WireExplorer.hxx>

#include <BRepLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <BRepFill_Generator.hxx>
#include <BRepFill_CompatibleWires.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>




//=======================================================================
//function :  PerformPlan
//purpose  : Construit s'il existe un plan de remplissage
//=======================================================================

static Standard_Boolean PerformPlan(const TopoDS_Wire& W,
				    const Standard_Real presPln,
				    TopoDS_Face& theFace)
{
  Standard_Boolean isDegen = Standard_True;
  TopoDS_Iterator iter(W);
  for (; iter.More(); iter.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(iter.Value());
      if (!BRep_Tool::Degenerated(anEdge))
	isDegen = Standard_False;
    }
  if (isDegen)
    return Standard_True;

  Standard_Boolean Ok = Standard_False;
  if (!W.IsNull()) {
    BRepBuilderAPI_FindPlane Searcher( W, presPln );
    if (Searcher.Found())
      {
	theFace = BRepBuilderAPI_MakeFace(Searcher.Plane(), W);
	Ok = Standard_True;
      }
    else // try to find another surface
      {
	BRepBuilderAPI_MakeFace MF( W );
	if (MF.IsDone())
	  {
	    theFace = MF.Face();
	    Ok = Standard_True;
	  }
      }
  }

 return Ok;
}

//=============================================================================
//function :  IsSameOriented
//purpose  : Checks whether aFace is oriented to the same side as aShell or not
//=============================================================================

static Standard_Boolean IsSameOriented(const TopoDS_Shape& aFace,
				       const TopoDS_Shape& aShell)
{
  TopExp_Explorer Explo(aFace, TopAbs_EDGE);
  TopoDS_Shape anEdge = Explo.Current();
  TopAbs_Orientation Or1 = anEdge.Orientation();

  TopTools_IndexedDataMapOfShapeListOfShape EFmap;
  TopExp::MapShapesAndAncestors( aShell, TopAbs_EDGE, TopAbs_FACE, EFmap );

  const TopoDS_Shape& AdjacentFace = EFmap.FindFromKey(anEdge).First();
  TopoDS_Shape theEdge;
  for (Explo.Init(AdjacentFace, TopAbs_EDGE); Explo.More(); Explo.Next())
    {
      theEdge = Explo.Current();
      if (theEdge.IsSame(anEdge))
	break;
    }

  TopAbs_Orientation Or2 = theEdge.Orientation();
  if (Or1 == Or2)
    return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : MakeSolid
//purpose  : 
//=======================================================================

static TopoDS_Solid MakeSolid(TopoDS_Shell& shell, const TopoDS_Wire& wire1,
			      const TopoDS_Wire& wire2, const Standard_Real presPln,
			      TopoDS_Face& face1, TopoDS_Face& face2)
{
  if (shell.IsNull())
    StdFail_NotDone::Raise("Thrusections is not build");
  Standard_Boolean B = shell.Closed();
  BRep_Builder BB;

  if (!B)
    {
      // Il faut boucher les extremites 
      B =  PerformPlan(wire1, presPln, face1);
      if (B) {
	B =  PerformPlan(wire2, presPln, face2);
	if (B) {
	  if (!face1.IsNull() && !IsSameOriented( face1, shell ))
	    face1.Reverse();
	  if (!face2.IsNull() && !IsSameOriented( face2, shell ))
	    face2.Reverse();
	  
	  if (!face1.IsNull())
	    BB.Add(shell, face1);
	  if (!face2.IsNull())
	    BB.Add(shell, face2);
	  
	  shell.Closed(Standard_True);
	}
      }
    }

  TopoDS_Solid solid;
  BB.MakeSolid(solid); 
  BB.Add(solid, shell);
  
  // verify the orientation the solid
  BRepClass3d_SolidClassifier clas3d(solid);
  clas3d.PerformInfinitePoint(Precision::Confusion());
  if (clas3d.State() == TopAbs_IN) {
    BB.MakeSolid(solid); 
    TopoDS_Shape aLocalShape = shell.Reversed();
    BB.Add(solid, TopoDS::Shell(aLocalShape));
//    B.Add(solid, TopoDS::Shell(newShell.Reversed()));
  }

  solid.Closed(Standard_True);
  return solid;
}


//=======================================================================
//function : BRepOffsetAPI_ThruSections
//purpose  : 
//=======================================================================

BRepOffsetAPI_ThruSections::BRepOffsetAPI_ThruSections(const Standard_Boolean isSolid, const Standard_Boolean ruled,
					     const Standard_Real pres3d):
					     myIsSolid(isSolid), myIsRuled(ruled), myPres3d(pres3d)
{
  myWCheck = Standard_True;
//----------------------------
  myParamType = Approx_ChordLength; 
  myDegMax    = 8; 
  myContinuity = GeomAbs_C2;
  myCritWeights[0] = .4; 
  myCritWeights[1] = .2; 
  myCritWeights[2] = .4; 
  myUseSmoothing = Standard_False;
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepOffsetAPI_ThruSections::Init(const Standard_Boolean isSolid, const Standard_Boolean ruled,
				 const Standard_Real pres3d)
{
  myIsSolid = isSolid;
  myIsRuled = ruled;
  myPres3d = pres3d;
  myWCheck = Standard_True;
//----------------------------
  myParamType = Approx_ChordLength; 
  myDegMax    = 6; 
  myContinuity = GeomAbs_C2;
  myCritWeights[0] = .4; 
  myCritWeights[1] = .2; 
  myCritWeights[2] = .4; 
  myUseSmoothing = Standard_False;

}


//=======================================================================
//function : AddWire
//purpose  : 
//=======================================================================

void BRepOffsetAPI_ThruSections::AddWire(const TopoDS_Wire& wire)
{
  myWires.Append(wire);
}

//=======================================================================
//function : AddVertex
//purpose  : 
//=======================================================================

void BRepOffsetAPI_ThruSections::AddVertex(const TopoDS_Vertex& aVertex)
{
  BRep_Builder BB;

  TopoDS_Edge DegEdge;
  BB.MakeEdge( DegEdge );
  BB.Add( DegEdge, aVertex.Oriented(TopAbs_FORWARD) );
  BB.Add( DegEdge, aVertex.Oriented(TopAbs_REVERSED) );
  BB.Degenerated( DegEdge, Standard_True );
  DegEdge.Closed( Standard_True );

  TopoDS_Wire DegWire;
  BB.MakeWire( DegWire );
  BB.Add( DegWire, DegEdge );
  DegWire.Closed( Standard_True );

  myWires.Append( DegWire );
}

//=======================================================================
//function : CheckCompatibility
//purpose  : 
//=======================================================================

void BRepOffsetAPI_ThruSections::CheckCompatibility(const Standard_Boolean check)
{
  myWCheck = check;
}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepOffsetAPI_ThruSections::Build()
{
  //Check set of section for right configuration of punctual sections
  Standard_Integer i;
  TopExp_Explorer explo;
  for (i = 2; i <= myWires.Length()-1; i++)
    {
      Standard_Boolean wdeg = Standard_True;
      for (explo.Init(myWires(i), TopAbs_EDGE); explo.More(); explo.Next())
	{
	  const TopoDS_Edge& anEdge = TopoDS::Edge(explo.Current());
	  wdeg = wdeg && (BRep_Tool::Degenerated(anEdge));
	}
      if (wdeg)
	Standard_Failure::Raise("Wrong usage of punctual sections");
    }
  if (myWires.Length() <= 2)
    {
      Standard_Boolean wdeg = Standard_True;
      for (i = 1; i <= myWires.Length(); i++)
	for (explo.Init(myWires(i), TopAbs_EDGE); explo.More(); explo.Next())
	  {
	    const TopoDS_Edge& anEdge = TopoDS::Edge(explo.Current());
	    wdeg = wdeg && (BRep_Tool::Degenerated(anEdge));
	  }
      if (wdeg)
	Standard_Failure::Raise("Wrong usage of punctual sections");
    }

  if (myWCheck) {
    // compute origin and orientation on wires to avoid twisted results
    // and update wires to have same number of edges
    
    // on utilise BRepFill_CompatibleWires
    TopTools_SequenceOfShape WorkingSections;
    WorkingSections.Clear();
    TopTools_DataMapOfShapeListOfShape WorkingMap;
    WorkingMap.Clear();
    
    // Calcul des sections de travail
    BRepFill_CompatibleWires Georges(myWires);
    Georges.Perform();
    if (Georges.IsDone()) {
      WorkingSections = Georges.Shape();
      WorkingMap = Georges.Generated();
    }
    myWires = WorkingSections;
  }

  // Calcul de la shape resultat
  if (myWires.Length() == 2 || myIsRuled) {
    // create a ruled shell
    CreateRuled();
  }
  else {
    // create a smoothed shell
    CreateSmoothed();
  }
  // Encode the Regularities
  BRepLib::EncodeRegularity(myShape);
  
}


//=======================================================================
//function : CreateRuled
//purpose  : 
//=======================================================================

void BRepOffsetAPI_ThruSections::CreateRuled()
{
  Standard_Integer nbSects = myWires.Length();
  BRepFill_Generator aGene;
//  for (Standard_Integer i=1; i<=nbSects; i++) {
  Standard_Integer i;
  for (i=1; i<=nbSects; i++) {
    aGene.AddWire(TopoDS::Wire(myWires(i)));
  }
  aGene.Perform();
  TopoDS_Shell shell = aGene.Shell();

  if (myIsSolid) {

    // on regarde si le premier wire est identique au dernier
    Standard_Boolean vClosed = (myWires(1).IsSame(myWires(nbSects))) ;

    if (vClosed) {

      TopoDS_Solid solid;      
      BRep_Builder B;
      B.MakeSolid(solid); 
      B.Add(solid, shell);
      
      // verify the orientation the solid
      BRepClass3d_SolidClassifier clas3d(solid);
      clas3d.PerformInfinitePoint(Precision::Confusion());
      if (clas3d.State() == TopAbs_IN) {
	B.MakeSolid(solid); 
	TopoDS_Shape aLocalShape = shell.Reversed();
	B.Add(solid, TopoDS::Shell(aLocalShape));
//	B.Add(solid, TopoDS::Shell(shell.Reversed()));
      }
      myShape = solid;

    }

    else {

      TopoDS_Wire wire1 = TopoDS::Wire(myWires.First());
      TopoDS_Wire wire2 = TopoDS::Wire(myWires.Last());
      myShape = MakeSolid(shell, wire1, wire2, myPres3d, myFirst, myLast);

    }

    Done();
  }

  else {
    myShape = shell;
    Done();
  }

  // history
  BRepTools_WireExplorer anExp1, anExp2;
  TopTools_IndexedDataMapOfShapeListOfShape M;
  TopExp::MapShapesAndAncestors(shell, TopAbs_EDGE, TopAbs_FACE, M);
  TopTools_ListIteratorOfListOfShape it;

  TopTools_IndexedDataMapOfShapeListOfShape MV;
  TopExp::MapShapesAndAncestors(shell, TopAbs_VERTEX, TopAbs_FACE, MV);
  
  for (i=1; i<=nbSects-1; i++) {
    
    const TopoDS_Wire& wire1 = TopoDS::Wire(myWires(i));
    const TopoDS_Wire& wire2 = TopoDS::Wire(myWires(i+1));
    
    anExp1.Init(wire1);
    anExp2.Init(wire2);

    Standard_Boolean tantque = anExp1.More() && anExp2.More();

    while (tantque) {

      const TopoDS_Shape& edge1 = anExp1.Current();
      const TopoDS_Shape& edge2 = anExp2.Current();
      Standard_Boolean degen1 = BRep_Tool::Degenerated(anExp1.Current());
      Standard_Boolean degen2 = BRep_Tool::Degenerated(anExp2.Current());
      
      TopTools_MapOfShape MapFaces;
      if (degen2){
	TopoDS_Vertex Vdegen = TopExp::FirstVertex(TopoDS::Edge(edge2));
	for (it.Initialize(MV.FindFromKey(Vdegen)); it.More(); it.Next()) {
	  MapFaces.Add(it.Value());
	}
      }
      else {
	for (it.Initialize(M.FindFromKey(edge2)); it.More(); it.Next()) {
	  MapFaces.Add(it.Value());
	}
      }
      
      if (degen1) {
	TopoDS_Vertex Vdegen = TopExp::FirstVertex(TopoDS::Edge(edge1));
	for (it.Initialize(MV.FindFromKey(Vdegen)); it.More(); it.Next()) {
	  const TopoDS_Shape& Face = it.Value();
	  if (MapFaces.Contains(Face)) {
	    myGenerated.Bind(edge1, Face);
	    break;
	  }
	}
      }
      else {
	for (it.Initialize(M.FindFromKey(edge1)); it.More(); it.Next()) {
	  const TopoDS_Shape& Face = it.Value();
	  if (MapFaces.Contains(Face)) {
	    myGenerated.Bind(edge1, Face);
	    break;
	  }
	}
      }
      
      if (!degen1) anExp1.Next();
      if (!degen2) anExp2.Next();
      
      tantque = anExp1.More() && anExp2.More();
      if (degen1) tantque = anExp2.More();
      if (degen2) tantque = anExp1.More();
      
    }
      
  }
      
}

//=======================================================================
//function : CreateSmoothed
//purpose  : 
//=======================================================================

void BRepOffsetAPI_ThruSections::CreateSmoothed()
{
  // initialisation
  Standard_Integer nbSects = myWires.Length();
  BRepTools_WireExplorer anExp;

  Standard_Boolean w1Point = Standard_True;
  // on regarde si le premier wire est ponctuel
  for(anExp.Init(TopoDS::Wire(myWires(1))); anExp.More(); anExp.Next()) {
    w1Point = w1Point && (BRep_Tool::Degenerated(anExp.Current()));
  }

  Standard_Boolean w2Point = Standard_True;
  // on regarde si le dernier wire est ponctuel
  for(anExp.Init(TopoDS::Wire(myWires(nbSects))); anExp.More(); anExp.Next()) {
    w2Point = w2Point && (BRep_Tool::Degenerated(anExp.Current()));
  }

  Standard_Boolean vClosed = Standard_False;
  // on regarde si le premier wire est identique au dernier
  if (myWires(1).IsSame(myWires(myWires.Length()))) vClosed = Standard_True;

  // find the dimension
  Standard_Integer nbEdges=0;
  if (!w1Point) {
    for(anExp.Init(TopoDS::Wire(myWires(1))); anExp.More(); anExp.Next()) {
      nbEdges++;
    }
  }
  else {
    for(anExp.Init(TopoDS::Wire(myWires(2))); anExp.More(); anExp.Next()) {
      nbEdges++;
    }
  }

  // recover the shapes
  Standard_Boolean uClosed = Standard_True;
  TopTools_Array1OfShape shapes(1, nbSects*nbEdges);
  Standard_Integer nb=0, i, j;

  for (i=1; i<=nbSects; i++) {
    const TopoDS_Wire& wire = TopoDS::Wire(myWires(i));
    if (!wire.Closed()) {
      // on regarde quand meme si les vertex sont les memes.
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(wire,V1,V2);
      if ( !V1.IsSame(V2)) uClosed = Standard_False;
    }
    if ( (i==1 && w1Point) || (i==nbSects && w2Point) ) {
      // si le wire est ponctuel
      anExp.Init(TopoDS::Wire(wire));
      for(j=1; j<=nbEdges; j++) {
	nb++;
	shapes(nb) = anExp.Current();
      }
    }
    else {
      // sinon
      for(anExp.Init(TopoDS::Wire(wire)); anExp.More(); anExp.Next()) {
	nb++;
	shapes(nb) = anExp.Current();
      }
    }
  }

  // create the new surface
  TopoDS_Shell shell;
  TopoDS_Face face;
  TopoDS_Wire W;
  TopoDS_Edge edge, edge1, edge2, edge3, edge4, couture;
  TopTools_Array1OfShape vcouture(1, nbEdges);

  BRep_Builder B;
  B.MakeShell(shell);

  TopoDS_Wire newW1, newW2;
  BRep_Builder BW1, BW2;
  BW1.MakeWire(newW1);
  BW2.MakeWire(newW2);

  TopLoc_Location loc;
  TopoDS_Vertex v1f,v1l,v2f,v2l;

  GeomFill_SectionGenerator section;
  Standard_Integer nbPnts = 21;
  TColgp_Array2OfPnt points(1, nbPnts, 1, nbSects);

  // on concatene chaque section pour obtenir une surface totale que
  // l'on va segmenter
  Handle(Geom_BSplineSurface) TS;
  TS = TotalSurf(shapes,nbSects,nbEdges,w1Point,w2Point,vClosed);

  if(TS.IsNull()) {
    return;
  }

  TopoDS_Shape firstEdge;
  for (i=1; i<=nbEdges; i++) {  

    // segmentation de TS
    Handle(Geom_BSplineSurface) surface;
    surface = Handle(Geom_BSplineSurface)::DownCast(TS->Copy());
    Standard_Real Ui1,Ui2,V0,V1;
    Ui1 = i-1;
    Ui2 = i;
    V0  = surface->VKnot(surface->FirstVKnotIndex());
    V1  = surface->VKnot(surface->LastVKnotIndex());
    surface->Segment(Ui1,Ui2,V0,V1);

    // recuperation des vertices
    edge =  TopoDS::Edge(shapes(i));
    TopExp::Vertices(edge,v1f,v1l);
    if (edge.Orientation() == TopAbs_REVERSED)
      TopExp::Vertices(edge,v1l,v1f);
    firstEdge = edge;

    edge =  TopoDS::Edge(shapes((nbSects-1)*nbEdges+i));
    TopExp::Vertices(edge,v2f,v2l);
    if (edge.Orientation() == TopAbs_REVERSED)
      TopExp::Vertices(edge,v2l,v2f);

    // make the face
    B.MakeFace(face, surface, Precision::Confusion());

    // make the wire
    B.MakeWire(W);
    
    // make the missing edges
    Standard_Real f1, f2, l1, l2;
    surface->Bounds(f1,l1,f2,l2);
  
    // --- edge 1
    if ( w1Point ) {
      // copie de l'edge degeneree
      TopoDS_Shape aLocalShape = shapes(1).EmptyCopied();
      edge1 =  TopoDS::Edge(aLocalShape);
//      edge1 =  TopoDS::Edge(shapes(1).EmptyCopied());
      edge1.Orientation(TopAbs_FORWARD);
    }
    else {
      B.MakeEdge(edge1, surface->VIso(f2), Precision::Confusion());
    }
    v1f.Orientation(TopAbs_FORWARD);
    B.Add(edge1, v1f);
    v1l.Orientation(TopAbs_REVERSED);
    B.Add(edge1, v1l);
    B.Range(edge1, f1, l1);
    // traitement des sections bouclantes
    // on stocke les edges de la 1ere section
    if (vClosed)
      vcouture(i) = edge1;
    

    // --- edge 2
    if (vClosed)
      edge2 = TopoDS::Edge(vcouture(i));
    else {
      if ( w2Point ) {
	// copie de l'edge degeneree
	TopoDS_Shape aLocalShape = shapes(nbSects*nbEdges).EmptyCopied();
	edge2 =  TopoDS::Edge(aLocalShape);
//	edge2 =  TopoDS::Edge(shapes(nbSects*nbEdges).EmptyCopied());
	edge2.Orientation(TopAbs_FORWARD);
      }
      else {
	B.MakeEdge(edge2, surface->VIso(l2), Precision::Confusion());
      }
      v2f.Orientation(TopAbs_FORWARD);
      B.Add(edge2, v2f);
      v2l.Orientation(TopAbs_REVERSED);
      B.Add(edge2, v2l);
      B.Range(edge2, f1, l1);
    }
    edge2.Reverse();


    // --- edge 3
    if (i==1) {
      B.MakeEdge(edge3, surface->UIso(f1), Precision::Confusion());
      v1f.Orientation(TopAbs_FORWARD);
      B.Add(edge3, v1f);
      v2f.Orientation(TopAbs_REVERSED);
      B.Add(edge3, v2f);
      B.Range(edge3, f2, l2);
      if (uClosed) {
	couture = edge3;
      }
    }
    else {
      edge3 = edge4;
    }
    edge3.Reverse();

    // --- edge 4
    if ( uClosed && i==nbEdges) {
      edge4 = couture;
    }
    else {
      B.MakeEdge(edge4, surface->UIso(l1), Precision::Confusion());
      v1l.Orientation(TopAbs_FORWARD);
      B.Add(edge4, v1l);
      v2l.Orientation(TopAbs_REVERSED);
      B.Add(edge4, v2l);
      B.Range(edge4, f2, l2);
    }

    B.Add(W,edge1);
    B.Add(W,edge4);
    B.Add(W,edge2);
    B.Add(W,edge3);

    // set PCurve
    if (vClosed) {
      B.UpdateEdge(edge1,
		   new Geom2d_Line(gp_Pnt2d(0,f2),gp_Dir2d(1,0)),
		   new Geom2d_Line(gp_Pnt2d(0,l2),gp_Dir2d(1,0)),face,
		   Precision::Confusion());
      B.Range(edge1,face,f1,l1);
    }
    else {
      B.UpdateEdge(edge1,new Geom2d_Line(gp_Pnt2d(0,f2),gp_Dir2d(1,0)),face,
		   Precision::Confusion());
      B.Range(edge1,face,f1,l1);
      B.UpdateEdge(edge2,new Geom2d_Line(gp_Pnt2d(0,l2),gp_Dir2d(1,0)),face,
		   Precision::Confusion());
      B.Range(edge2,face,f1,l1);
    }

    if ( uClosed && nbEdges ==1 )  {
      B.UpdateEdge(edge3,
		   new Geom2d_Line(gp_Pnt2d(l1,0),gp_Dir2d(0,1)),
		   new Geom2d_Line(gp_Pnt2d(f1,0),gp_Dir2d(0,1)),face,
		   Precision::Confusion());
      B.Range(edge3,face,f2,l2);

    }
    else {
      B.UpdateEdge(edge3,new Geom2d_Line(gp_Pnt2d(f1,0),gp_Dir2d(0,1)),face,
		   Precision::Confusion());
      B.Range(edge3,face,f2,l2);
      B.UpdateEdge(edge4,new Geom2d_Line(gp_Pnt2d(l1,0),gp_Dir2d(0,1)),face,
		   Precision::Confusion());
      B.Range(edge4,face,f2,l2);
    }
    B.Add(face,W);
    B.Add(shell, face);

    // complete newW1 newW2
    TopoDS_Edge edge12 = edge1;
    TopoDS_Edge edge22 = edge2;
    edge12.Reverse();
    edge22.Reverse();
    BW1.Add(newW1, edge12);
    BW2.Add(newW2, edge22);

    // history
    myGenerated.Bind(firstEdge, face);
  }

  if (uClosed && w1Point && w2Point)
    shell.Closed(Standard_True);

  if (myIsSolid) {

    if (vClosed) {

      TopoDS_Solid solid;
      BRep_Builder B;
      B.MakeSolid(solid); 
      B.Add(solid, shell);
      
      // verify the orientation the solid
      BRepClass3d_SolidClassifier clas3d(solid);
      clas3d.PerformInfinitePoint(Precision::Confusion());
      if (clas3d.State() == TopAbs_IN) {
	B.MakeSolid(solid); 
	TopoDS_Shape aLocalShape = shell.Reversed();
	B.Add(solid, TopoDS::Shell(aLocalShape));
//	B.Add(solid, TopoDS::Shell(shell.Reversed()));
      }
      myShape = solid;

    }

    else {
      myShape = MakeSolid(shell, newW1, newW2, myPres3d, myFirst, myLast);
    }

    Done();
  }

  else {
    myShape = shell;
    Done();
  }
  
  TopExp_Explorer ex(myShape,TopAbs_EDGE);
  while (ex.More()) {
    const TopoDS_Edge& CurE = TopoDS::Edge(ex.Current());
    B.SameRange(CurE, Standard_False);
    B.SameParameter(CurE, Standard_False);
    Standard_Real tol = BRep_Tool::Tolerance(CurE);
    BRepLib::SameParameter(CurE,tol);
    ex.Next();
  }
}

//=======================================================================
//function : TotalSurf
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) BRepOffsetAPI_ThruSections::
                                   TotalSurf(const TopTools_Array1OfShape& shapes,
					     const Standard_Integer NbSects,
					     const Standard_Integer NbEdges,
					     const Standard_Boolean w1Point,
					     const Standard_Boolean w2Point,
					     const Standard_Boolean vClosed) const
{
  Standard_Integer i,j,jdeb=1,jfin=NbSects;
  TopoDS_Edge edge;
  TopLoc_Location loc;
  Standard_Real first, last;
  TopoDS_Vertex vf,vl;

  GeomFill_SectionGenerator section;
  Handle(Geom_BSplineSurface) surface;
  Handle(Geom_BSplineCurve) BS, BS1;
  Handle(Geom_TrimmedCurve) curvTrim;
  Handle(Geom_BSplineCurve) curvBS;

  if (w1Point) {
    jdeb++;
    edge =  TopoDS::Edge(shapes(1));
    TopExp::Vertices(edge,vl,vf);
    TColgp_Array1OfPnt Extremities(1,2);
    Extremities(1) = BRep_Tool::Pnt(vf);
    Extremities(2) = BRep_Tool::Pnt(vl);
    TColStd_Array1OfReal Bounds(1,2);
    Bounds(1) = 0.;
    Bounds(2) = 1.;
    Standard_Integer Deg = 1;
    TColStd_Array1OfInteger Mult(1,2);
    Mult(1) = Deg+1;
    Mult(2) = Deg+1;
    Handle(Geom_BSplineCurve) BSPoint
      = new Geom_BSplineCurve(Extremities,Bounds,Mult,Deg);
    section.AddCurve(BSPoint);
  }

  if (w2Point) {
    jfin--;
  }

  for (j=jdeb; j<=jfin; j++) {

    // cas des sections bouclantes
    if (j==jfin && vClosed) {
      section.AddCurve(BS1);
    }

    else {
      // read the first edge to initialise CompBS;
      edge =  TopoDS::Edge(shapes((j-1)*NbEdges+1));
      if (BRep_Tool::Degenerated(edge)) {
	// edge degeneree : construction d'une courbe ponctuelle
	TopExp::Vertices(edge,vl,vf);
	TColgp_Array1OfPnt Extremities(1,2);
	Extremities(1) = BRep_Tool::Pnt(vf);
	Extremities(2) = BRep_Tool::Pnt(vl);
	Handle(Geom_Curve) curv = new Geom_BezierCurve(Extremities);
	curvTrim = new Geom_TrimmedCurve(curv,
					 curv->FirstParameter(),
					 curv->LastParameter());
      }
      else {
	// recuperation de la courbe sur l'edge
	Handle(Geom_Curve) curv = BRep_Tool::Curve(edge, loc, first, last);
	curvTrim = new Geom_TrimmedCurve(curv, first, last);
	curvTrim->Transform(loc.Transformation());
      }
      if (edge.Orientation() == TopAbs_REVERSED) {
	curvTrim->Reverse();
      }

      // transformation en BSpline reparametree sur [i-1,i]
      curvBS = Handle(Geom_BSplineCurve)::DownCast(curvTrim);
      if (curvBS.IsNull()) { 
	Handle(Geom_Curve) theCurve = curvTrim->BasisCurve();
	if (theCurve->IsKind(STANDARD_TYPE(Geom_Conic)))
	  {
	    GeomConvert_ApproxCurve appr(curvTrim, Precision::Confusion(), GeomAbs_C1, 16, 14);
	    if (appr.HasResult())
	      curvBS = appr.Curve();
	  }
	if (curvBS.IsNull())
	  curvBS = GeomConvert::CurveToBSplineCurve(curvTrim);
      }
      TColStd_Array1OfReal BSK(1,curvBS->NbKnots());
      curvBS->Knots(BSK);
      BSplCLib::Reparametrize(0.,1.,BSK);
      curvBS->SetKnots(BSK);
      
      // initialisation
      GeomConvert_CompCurveToBSplineCurve CompBS(curvBS);

      for (i=2; i<=NbEdges; i++) {  
	// read the edge
	edge =  TopoDS::Edge(shapes((j-1)*NbEdges+i));
	if (BRep_Tool::Degenerated(edge)) {
	  // edge degeneree : construction d'une courbe ponctuelle
	  TopExp::Vertices(edge,vl,vf);
	  TColgp_Array1OfPnt Extremities(1,2);
	  Extremities(1) = BRep_Tool::Pnt(vf);
	  Extremities(2) = BRep_Tool::Pnt(vl);
	  Handle(Geom_Curve) curv = new Geom_BezierCurve(Extremities);
	  curvTrim = new Geom_TrimmedCurve(curv,
					   curv->FirstParameter(),
					   curv->LastParameter());
	}
	else {
	  // recuperation de la courbe sur l'edge
	  Handle(Geom_Curve) curv = BRep_Tool::Curve(edge, loc, first, last);
	  curvTrim = new Geom_TrimmedCurve(curv, first, last);
	  curvTrim->Transform(loc.Transformation());
	}
	if (edge.Orientation() == TopAbs_REVERSED) {
	  curvTrim->Reverse();
	}

	// transformation en BSpline reparametree sur [i-1,i]
	curvBS = Handle(Geom_BSplineCurve)::DownCast(curvTrim);
	if (curvBS.IsNull()) { 
	  Handle(Geom_Curve) theCurve = curvTrim->BasisCurve();
	  if (theCurve->IsKind(STANDARD_TYPE(Geom_Conic)))
	    {
	      GeomConvert_ApproxCurve appr(curvTrim, Precision::Confusion(), GeomAbs_C1, 16, 14);
	      if (appr.HasResult())
		curvBS = appr.Curve();
	    }
	  if (curvBS.IsNull())
	    curvBS = GeomConvert::CurveToBSplineCurve(curvTrim);
	}
	TColStd_Array1OfReal BSK(1,curvBS->NbKnots());
	curvBS->Knots(BSK);
	BSplCLib::Reparametrize(i-1,i,BSK);
	curvBS->SetKnots(BSK);

	// concatenation
	CompBS.Add(curvBS, 
		   Precision::Confusion(),
		   Standard_True,
		   Standard_False,
		   1);
      }

      // recuperation de la section finale
      BS = CompBS.BSplineCurve();
      section.AddCurve(BS);

      // cas des sections bouclantes
      if (j==jdeb && vClosed) {
	BS1 = BS;
      }

    }
  }

  if (w2Point) {
    edge =  TopoDS::Edge(shapes(NbSects*NbEdges));
    TopExp::Vertices(edge,vl,vf);
    TColgp_Array1OfPnt Extremities(1,2);
    Extremities(1) = BRep_Tool::Pnt(vf);
    Extremities(2) = BRep_Tool::Pnt(vl);
    TColStd_Array1OfReal Bounds(1,2);
    Bounds(1) = 0.;
    Bounds(2) = 1.;
    Standard_Integer Deg = 1;
    TColStd_Array1OfInteger Mult(1,2);
    Mult(1) = Deg+1;
    Mult(2) = Deg+1;
    Handle(Geom_BSplineCurve) BSPoint
      = new Geom_BSplineCurve(Extremities,Bounds,Mult,Deg);
    section.AddCurve(BSPoint);
  }

  section.Perform(Precision::PConfusion());
  Handle(GeomFill_Line) line = new GeomFill_Line(NbSects);

  Standard_Integer nbIt = 3;
  if(myPres3d <= 1.e-3) nbIt = 0;

  Standard_Integer degmin = 2, degmax = Max(myDegMax, degmin);
  Standard_Boolean SpApprox = Standard_True;

  GeomFill_AppSurf anApprox(degmin, degmax, myPres3d, myPres3d, nbIt);
  anApprox.SetContinuity(myContinuity);

  if(myUseSmoothing) {
    anApprox.SetCriteriumWeight(myCritWeights[0], myCritWeights[1], myCritWeights[2]);
    anApprox.PerformSmoothing(line, section);
  } 
  else {
    anApprox.SetParType(myParamType);
    anApprox.Perform(line, section, SpApprox);
  }

  if(anApprox.IsDone()) {
    surface = 
      new Geom_BSplineSurface(anApprox.SurfPoles(), anApprox.SurfWeights(),
			    anApprox.SurfUKnots(), anApprox.SurfVKnots(),
			    anApprox.SurfUMults(), anApprox.SurfVMults(),
			    anApprox.UDegree(), anApprox.VDegree());
  }

  return surface;
  
}

//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepOffsetAPI_ThruSections::FirstShape() const
{
  return myFirst;
}

//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepOffsetAPI_ThruSections::LastShape() const
{
  return myLast;
}

//=======================================================================
//function : GeneratedFace
//purpose  : 
//=======================================================================

TopoDS_Shape BRepOffsetAPI_ThruSections::GeneratedFace(const TopoDS_Shape& edge) const
{
  TopoDS_Shape bid;
  if (myGenerated.IsBound(edge)) {
    return myGenerated(edge);
  }
  else {
    return bid;
  }
}


//=======================================================================
//function : CriteriumWeight
//purpose  : returns the Weights associed  to the criterium used in
//           the  optimization.
//=======================================================================
//
void BRepOffsetAPI_ThruSections::CriteriumWeight(Standard_Real& W1, Standard_Real& W2, Standard_Real& W3) const 
{
  W1 = myCritWeights[0];
  W2 = myCritWeights[1];
  W3 = myCritWeights[2];
}
//=======================================================================
//function : SetCriteriumWeight
//purpose  : 
//=======================================================================

void BRepOffsetAPI_ThruSections::SetCriteriumWeight(const Standard_Real W1, const Standard_Real W2, const Standard_Real W3)
{
  if (W1 < 0 || W2 < 0 || W3 < 0 ) Standard_DomainError::Raise();
  myCritWeights[0] = W1;
  myCritWeights[1] = W2;
  myCritWeights[2] = W3;
}
//=======================================================================
//function : SetContinuity
//purpose  : 
//=======================================================================

void BRepOffsetAPI_ThruSections::SetContinuity (const GeomAbs_Shape TheCont)
{
  myContinuity = TheCont;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape BRepOffsetAPI_ThruSections::Continuity () const
{
  return myContinuity;
}

//=======================================================================
//function : SetParType
//purpose  : 
//=======================================================================

void BRepOffsetAPI_ThruSections::SetParType (const Approx_ParametrizationType ParType)
{
  myParamType = ParType;
}

//=======================================================================
//function : ParType
//purpose  : 
//=======================================================================

Approx_ParametrizationType BRepOffsetAPI_ThruSections::ParType () const
{
  return myParamType;
}

//=======================================================================
//function : SetMaxDegree
//purpose  : 
//=======================================================================

void BRepOffsetAPI_ThruSections:: SetMaxDegree(const Standard_Integer MaxDeg)
{
  myDegMax = MaxDeg;
}

//=======================================================================
//function : MaxDegree
//purpose  : 
//=======================================================================

Standard_Integer  BRepOffsetAPI_ThruSections::MaxDegree () const
{
  return myDegMax;
}

//=======================================================================
//function : SetSmoothing
//purpose  : 
//=======================================================================

void BRepOffsetAPI_ThruSections::SetSmoothing(const Standard_Boolean UseVar)
{
  myUseSmoothing = UseVar;
}

//=======================================================================
//function : UseSmoothing
//purpose  : 
//=======================================================================

Standard_Boolean BRepOffsetAPI_ThruSections::UseSmoothing () const
{
  return myUseSmoothing;
}





