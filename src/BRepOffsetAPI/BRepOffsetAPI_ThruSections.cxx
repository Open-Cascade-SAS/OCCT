// Created on: 1995-07-18
// Created by: Joelle CHAUVET
// Copyright (c) 1995-1999 Matra Datavision
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

// Modified:	Mon Jan 12 10:50:10 1998
//              gestion automatique de l'origine et de l'orientation
//              avec la methode ArrangeWires
// Modified:	Mon Jan 19 10:11:56 1998
//              traitement des cas particuliers cylindre, cone, plan 
//              (methodes DetectKPart et CreateKPart)
// Modified:	Mon Feb 23 09:28:46 1998
//              traitement des sections avec nombre d'elements different
//              + quelques ameliorations pour les cas particuliers
//              + cas de la derniere section ponctuelle
// Modified:	Mon Apr  6 15:47:44 1998
//              traitement des cas particuliers deplace dans BRepFill 
// Modified:	Thu Apr 30 15:24:17 1998
//              separation sections fermees / sections ouvertes + debug 
// Modified:	Fri Jul 10 11:23:35 1998
//              surface de CreateSmoothed par concatenation,approximation
//              et segmentation (PRO13924, CTS21295)
// Modified:	Tue Jul 21 16:48:35 1998
//              pb de ratio (BUC60281) 
// Modified:	Thu Jul 23 11:38:36 1998
//              sections bouclantes
// Modified:	Fri Aug 28 10:13:44 1998
//              traitement des sections ponctuelles
//              dans l'historique (cf. loft06 et loft09)
//              et dans le cas des solides
// Modified:	Tue Nov  3 10:06:15 1998
//              utilisation de BRepFill_CompatibleWires

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepFill_CompatibleWires.hxx>
#include <BRepFill_Generator.hxx>
#include <BRepLib.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BSplCLib.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NullObject.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <BRepAdaptor_Surface.hxx>

//=======================================================================
//function : PreciseUpar
//purpose  : pins the u-parameter of surface close to U-knot
//           to this U-knot
//=======================================================================
static Standard_Real PreciseUpar(const Standard_Real anUpar,
  const Handle(Geom_BSplineSurface)& aSurface)
{
  Standard_Real Tol = Precision::PConfusion();
  Standard_Integer i1, i2;

  aSurface->LocateU(anUpar, Tol, i1, i2);
  Standard_Real U1 = aSurface->UKnot(i1);
  Standard_Real U2 = aSurface->UKnot(i2);

  Standard_Real NewU = anUpar;

  NewU = (anUpar - U1 < U2 - anUpar)? U1 : U2;
  return NewU;
}

//=======================================================================
//function :  PerformPlan
//purpose  : Construct a plane of filling if exists
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
    throw StdFail_NotDone("Thrusections is not build");
  Standard_Boolean B = shell.Closed();
  BRep_Builder BB;

  if (!B)
  {
    // It is necessary to close the extremities 
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

BRepOffsetAPI_ThruSections::BRepOffsetAPI_ThruSections(const Standard_Boolean isSolid,
                                                       const Standard_Boolean ruled,
                                                       const Standard_Real pres3d):
  myNbEdgesInSection(0),
  myIsSolid(isSolid), myIsRuled(ruled),
  myPres3d(pres3d),
  myDegen1(Standard_False), myDegen2(Standard_False)
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
  myInputWires.Append(wire);
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

  TopoDS_Wire DegWire;
  BB.MakeWire( DegWire );
  BB.Add( DegWire, DegEdge );
  DegWire.Closed( Standard_True );

  myWires.Append( DegWire );
  myInputWires.Append(DegWire);
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
      throw Standard_Failure("Wrong usage of punctual sections");
  }
  if (myWires.Length() <= 2)
  {
    Standard_Boolean wdeg = Standard_True;
    for (i = 1; i <= myWires.Length(); i++)
    {
      for (explo.Init(myWires(i), TopAbs_EDGE); explo.More(); explo.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(explo.Current());
        wdeg = wdeg && (BRep_Tool::Degenerated(anEdge));
      }
    }
    if (wdeg)
    {
      throw Standard_Failure("Wrong usage of punctual sections");
    }
  }

  myNbEdgesInSection = 0;
  
  if (myWCheck) {
    // compute origin and orientation on wires to avoid twisted results
    // and update wires to have same number of edges

    // use BRepFill_CompatibleWires
    TopTools_SequenceOfShape WorkingSections;
    WorkingSections.Clear();
    TopTools_DataMapOfShapeListOfShape WorkingMap;
    WorkingMap.Clear();

    // Calculate the working sections
    BRepFill_CompatibleWires Georges(myWires);
    Georges.Perform();
    if (Georges.IsDone()) {
      WorkingSections = Georges.Shape();
      WorkingMap = Georges.Generated();
      myDegen1 = Georges.IsDegeneratedFirstSection();
      myDegen2 = Georges.IsDegeneratedLastSection();
      //For each sub-edge of each section
      //we save its splits
      Standard_Integer IndFirstSec = 1;
      if (Georges.IsDegeneratedFirstSection())
        IndFirstSec = 2;
      TopoDS_Shape aWorkingSection = WorkingSections(IndFirstSec);
      TopoDS_Iterator itw(aWorkingSection);
      for (; itw.More(); itw.Next())
        myNbEdgesInSection++;
      for (Standard_Integer ii = 1; ii <= myWires.Length(); ii++)
      {
        TopExp_Explorer Explo(myWires(ii), TopAbs_EDGE);
        for (; Explo.More(); Explo.Next())
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
          Standard_Integer aSign = 1;
          TopoDS_Vertex Vfirst, Vlast;
          TopExp::Vertices(anEdge, Vfirst, Vlast);
          TopTools_ListOfShape aNewEdges = Georges.GeneratedShapes(anEdge);
          TColStd_ListOfInteger IList;
          aWorkingSection = WorkingSections(ii);
          Standard_Integer NbNewEdges = aNewEdges.Extent();
          TopTools_ListIteratorOfListOfShape itl(aNewEdges);
          for (Standard_Integer kk = 1; itl.More(); itl.Next(),kk++)
          {
            const TopoDS_Edge& aNewEdge = TopoDS::Edge(itl.Value());
            Standard_Integer inde = 1;
            for (itw.Initialize(aWorkingSection); itw.More(); itw.Next(),inde++)
            {
              const TopoDS_Shape& aWorkingEdge = itw.Value();
              if (aWorkingEdge.IsSame(aNewEdge))
              {
                aSign = (aWorkingEdge.Orientation() == TopAbs_FORWARD)? 1 : -1;
                break;
              }
            }
            IList.Append(inde);
            if (kk == 1 || kk == NbNewEdges)
            {
              //For each sub-vertex of each section
              //we save its index of new edge
              TopoDS_Vertex NewVfirst, NewVlast;
              TopExp::Vertices(aNewEdge, NewVfirst, NewVlast);
              if (NewVfirst.IsSame(Vfirst) && !myVertexIndex.IsBound(Vfirst))
                myVertexIndex.Bind(Vfirst, aSign*inde);
              if (NewVlast.IsSame(Vlast) && !myVertexIndex.IsBound(Vlast))
                myVertexIndex.Bind(Vlast, aSign*(-inde));
            }
          }
          myEdgeNewIndices.Bind(anEdge, IList);
        }
      }
    }
    myWires = WorkingSections;
  } //if (myWCheck)
  else //no check
  {
    TopoDS_Edge anEdge;
    for (Standard_Integer ii = 1; ii <= myWires.Length(); ii++)
    {
      TopExp_Explorer Explo(myWires(ii), TopAbs_EDGE);
      Standard_Integer inde = 1;
      for (; Explo.More(); Explo.Next(),inde++)
      {
        anEdge = TopoDS::Edge(Explo.Current());
        TColStd_ListOfInteger IList;
        IList.Append(inde);
        myEdgeNewIndices.Bind(anEdge, IList);
        TopoDS_Vertex V1, V2;
        TopExp::Vertices(anEdge, V1, V2);
        if (!myVertexIndex.IsBound(V1))
          myVertexIndex.Bind(V1, inde);
        if (!myVertexIndex.IsBound(V2))
          myVertexIndex.Bind(V2, -inde);
      }
      inde--;
      if (inde > myNbEdgesInSection)
        myNbEdgesInSection = inde;
      if (inde == 1 && BRep_Tool::Degenerated(anEdge))
      {
        if (ii == 1)
          myDegen1 = Standard_True;
        else
          myDegen2 = Standard_True;
      }
    }
  }

  try {
    // Calculate the resulting shape
    if (myWires.Length() == 2 || myIsRuled) {
      // create a ruled shell
      CreateRuled();
    }
    else {
      // create a smoothed shell
      CreateSmoothed();
    }
  }
  catch (Standard_Failure)
  {
    NotDone();
    return;
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

    // check if the first wire is the same as the last
    Standard_Boolean vClosed = (myWires(1).IsSame(myWires(nbSects))) ;

    if (vClosed) {

      TopoDS_Solid solid;      
      BRep_Builder B;
      B.MakeSolid(solid); 
      B.Add(solid, shell);

      // verify the orientation of the solid
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
            myEdgeFace.Bind(edge1, Face);
            break;
          }
        }
      }
      else {
        for (it.Initialize(M.FindFromKey(edge1)); it.More(); it.Next()) {
          const TopoDS_Shape& Face = it.Value();
          if (MapFaces.Contains(Face)) {
            myEdgeFace.Bind(edge1, Face);
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
  // check if the first wire is punctual
  for(anExp.Init(TopoDS::Wire(myWires(1))); anExp.More(); anExp.Next()) {
    w1Point = w1Point && (BRep_Tool::Degenerated(anExp.Current()));
  }

  Standard_Boolean w2Point = Standard_True;
  // check if the last wire is punctual
  for(anExp.Init(TopoDS::Wire(myWires(nbSects))); anExp.More(); anExp.Next()) {
    w2Point = w2Point && (BRep_Tool::Degenerated(anExp.Current()));
  }

  Standard_Boolean vClosed = Standard_False;
  // check if the first wire is the same as last
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
      // check if the vertices are the same
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(wire,V1,V2);
      if ( !V1.IsSame(V2)) uClosed = Standard_False;
    }
    if ( (i==1 && w1Point) || (i==nbSects && w2Point) ) {
      // if the wire is punctual
      anExp.Init(TopoDS::Wire(wire));
      for(j=1; j<=nbEdges; j++) {
        nb++;
        shapes(nb) = anExp.Current();
      }
    }
    else {
      // otherwise
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

  Standard_Integer nbPnts = 21;
  TColgp_Array2OfPnt points(1, nbPnts, 1, nbSects);

  // concatenate each section to get a total surface that will be segmented
  Handle(Geom_BSplineSurface) TS;
  TS = TotalSurf(shapes,nbSects,nbEdges,w1Point,w2Point,vClosed);

  if(TS.IsNull()) {
    return;
  }

  TopoDS_Shape firstEdge;
  for (i=1; i<=nbEdges; i++) {  

    // segmentation of TS
    Handle(Geom_BSplineSurface) surface;
    surface = Handle(Geom_BSplineSurface)::DownCast(TS->Copy());
    Standard_Real Ui1,Ui2,V0,V1;
    Ui1 = i-1;
    Ui2 = i;
    Ui1 = PreciseUpar(Ui1, surface);
    Ui2 = PreciseUpar(Ui2, surface);
    V0  = surface->VKnot(surface->FirstVKnotIndex());
    V1  = surface->VKnot(surface->LastVKnotIndex());
    surface->Segment(Ui1,Ui2,V0,V1);

    // return vertices
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
      // copy the degenerated edge
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
    // processing of looping sections
    // store edges of the 1st section
    if (vClosed)
      vcouture(i) = edge1;


    // --- edge 2
    if (vClosed)
      edge2 = TopoDS::Edge(vcouture(i));
    else {
      if ( w2Point ) {
        // copy of the degenerated edge
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
    myEdgeFace.Bind(firstEdge, face);
  }

  if (uClosed && w1Point && w2Point)
    shell.Closed(Standard_True);

  if (myIsSolid) {

    if (vClosed) {

      TopoDS_Solid solid;
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
//function : EdgeToBSpline
//purpose  : auxiliary -- get curve from edge and convert it to bspline
//           parameterized from 0 to 1
//=======================================================================

// NOTE: this code duplicates the same function in BRepFill_NSections.cxx
static Handle(Geom_BSplineCurve) EdgeToBSpline (const TopoDS_Edge& theEdge)
{
  Handle(Geom_BSplineCurve) aBSCurve;
  if (BRep_Tool::Degenerated(theEdge)) {
    // degenerated edge : construction of a point curve
    TColStd_Array1OfReal aKnots (1,2);
    aKnots(1) = 0.;
    aKnots(2) = 1.;

    TColStd_Array1OfInteger aMults (1,2);
    aMults(1) = 2;
    aMults(2) = 2;

    TColgp_Array1OfPnt aPoles(1,2);
    TopoDS_Vertex vf, vl;
    TopExp::Vertices(theEdge,vl,vf);
    aPoles(1) = BRep_Tool::Pnt(vf);
    aPoles(2) = BRep_Tool::Pnt(vl);

    aBSCurve = new Geom_BSplineCurve (aPoles, aKnots, aMults, 1);
  }
  else
  {
    // get the curve of the edge
    TopLoc_Location aLoc;
    Standard_Real aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve (theEdge, aLoc, aFirst, aLast);
    if (aCurve.IsNull())
      throw Standard_NullObject("Null 3D curve in edge");

    // convert its part used by edge to bspline; note that if edge curve is bspline,
    // conversion made via trimmed curve is still needed -- it will copy it, segment 
    // as appropriate, and remove periodicity if it is periodic (deadly for approximator)
    Handle(Geom_TrimmedCurve) aTrimCurve = new Geom_TrimmedCurve (aCurve, aFirst, aLast);

    // special treatment of conic curve
    if (aTrimCurve->BasisCurve()->IsKind(STANDARD_TYPE(Geom_Conic)))
    {
      const Handle(Geom_Curve)& aCurveTrimmed = aTrimCurve; // to avoid ambiguity
      GeomConvert_ApproxCurve anAppr (aCurveTrimmed, Precision::Confusion(), GeomAbs_C1, 16, 14);
      if (anAppr.HasResult())
        aBSCurve = anAppr.Curve();
    }

    // general case
    if (aBSCurve.IsNull())
      aBSCurve = GeomConvert::CurveToBSplineCurve (aTrimCurve);

    // apply transformation if needed
    if (! aLoc.IsIdentity())
      aBSCurve->Transform (aLoc.Transformation());

    // reparameterize to [0,1]
    TColStd_Array1OfReal aKnots (1, aBSCurve->NbKnots());
    aBSCurve->Knots (aKnots);
    BSplCLib::Reparametrize (0., 1., aKnots);
    aBSCurve->SetKnots (aKnots);
  }

  // reverse curve if edge is reversed
  if (theEdge.Orientation() == TopAbs_REVERSED)
    aBSCurve->Reverse();

  return aBSCurve;
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
  TopoDS_Vertex vf,vl;

  GeomFill_SectionGenerator section;
  Handle(Geom_BSplineSurface) surface;
  Handle(Geom_BSplineCurve) BS, BS1;
  Handle(Geom_TrimmedCurve) curvTrim;

  if (w1Point) {
    jdeb++;
    TopoDS_Edge edge =  TopoDS::Edge(shapes(1));
    TopExp::Vertices(edge,vl,vf);
    TColgp_Array1OfPnt Extremities(1,2);
    Extremities(1) = BRep_Tool::Pnt(vf);
    Extremities(2) = BRep_Tool::Pnt(vl);
    TColStd_Array1OfReal Bounds(1,2);
    Bounds(1) = 0.;
    Bounds(2) = 1.;
    TColStd_Array1OfInteger Mult(1,2);
    Mult(1) = 2;
    Mult(2) = 2;
    Handle(Geom_BSplineCurve) BSPoint
      = new Geom_BSplineCurve(Extremities,Bounds,Mult,1);
    section.AddCurve(BSPoint);
  }

  if (w2Point) {
    jfin--;
  }

  for (j=jdeb; j<=jfin; j++) {

    // case of looping sections 
    if (j==jfin && vClosed) {
      section.AddCurve(BS1);
    }

    else {
      // read the first edge to initialise CompBS;
      TopoDS_Edge aPrevEdge = TopoDS::Edge (shapes((j-1)*NbEdges+1));
      Handle(Geom_BSplineCurve) curvBS = EdgeToBSpline (aPrevEdge);

      // initialization
      GeomConvert_CompCurveToBSplineCurve CompBS(curvBS);

      for (i=2; i<=NbEdges; i++) {  
        // read the edge
        TopoDS_Edge aNextEdge = TopoDS::Edge (shapes((j-1)*NbEdges+i));
        Standard_Real aTolV = Precision::Confusion();  
        TopExp::Vertices(aNextEdge,vf,vl);
        aTolV = Max(aTolV, BRep_Tool::Tolerance(vf));
        aTolV = Max(aTolV, BRep_Tool::Tolerance(vl));
        aTolV = Min(aTolV, 1.e-3);
        curvBS = EdgeToBSpline (aNextEdge);

        // concatenation
        CompBS.Add(curvBS, aTolV, Standard_True, Standard_False, 1);
      }

      // return the final section
      BS = CompBS.BSplineCurve();
      section.AddCurve(BS);

      // case of looping sections
      if (j==jdeb && vClosed) {
        BS1 = BS;
      }

    }
  }

  if (w2Point) {
    TopoDS_Edge edge =  TopoDS::Edge(shapes(NbSects*NbEdges));
    TopExp::Vertices(edge,vl,vf);
    TColgp_Array1OfPnt Extremities(1,2);
    Extremities(1) = BRep_Tool::Pnt(vf);
    Extremities(2) = BRep_Tool::Pnt(vl);
    TColStd_Array1OfReal Bounds(1,2);
    Bounds(1) = 0.;
    Bounds(2) = 1.;
    TColStd_Array1OfInteger Mult(1,2);
    Mult(1) = 2;
    Mult(2) = 2;
    Handle(Geom_BSplineCurve) BSPoint
      = new Geom_BSplineCurve(Extremities,Bounds,Mult,1);
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
//function : Generated
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& 
BRepOffsetAPI_ThruSections::Generated(const TopoDS_Shape& S) 
{
  myGenerated.Clear();

  TopTools_SequenceOfShape AllFaces;
  TopExp_Explorer Explo(myShape, TopAbs_FACE);
  for (; Explo.More(); Explo.Next())
    AllFaces.Append(Explo.Current());

  if (S.ShapeType() == TopAbs_EDGE)
  {
    if (!myEdgeNewIndices.IsBound(S))
      return myGenerated;

    const TColStd_ListOfInteger& Indices = myEdgeNewIndices(S);
    //Append the faces corresponding to <Indices>
    //These faces "grow" from the first section
    TColStd_ListIteratorOfListOfInteger itl(Indices);
    for (; itl.More(); itl.Next())
    {
      Standard_Integer IndOfFace = itl.Value();
      myGenerated.Append(AllFaces(IndOfFace));
    }

    if (myIsRuled)
      //Append the next faces corresponding to <Indices>
      for (Standard_Integer i = 2; i < myWires.Length(); i++)
        for (itl.Initialize(Indices); itl.More(); itl.Next())
        {
          Standard_Integer IndOfFace = itl.Value();
          IndOfFace += (i-1)*myNbEdgesInSection;
          myGenerated.Append(AllFaces(IndOfFace));
        }
  }
  else if (S.ShapeType() == TopAbs_VERTEX)
  {
    if (!myVertexIndex.IsBound(S))
      return myGenerated;

    TopTools_IndexedDataMapOfShapeListOfShape VEmap;
    
    Standard_Boolean IsDegen [2] = {Standard_False, Standard_False};
    if (myDegen1 || myDegen2)
    {
      TopoDS_Shape EndSections [2];
      EndSections[0] = myWires(1);
      EndSections[1] = myWires(myWires.Length());
      for (Standard_Integer i = 0; i < 2; i++)
      {
        if (i == 0 && !myDegen1)
          continue;
        if (i == 1 && !myDegen2)
          continue;
        
        Explo.Init(EndSections[i], TopAbs_VERTEX);
        const TopoDS_Shape& aVertex = Explo.Current();
        if (S.IsSame(aVertex))
        {
          IsDegen[i] = Standard_True;
          break;
        }
      }
    }
    // Only one of <IsDegen> can be True:
    // in case of one vertex for start and end degenerated sections
    // IsDegen[0] is True;
    if (IsDegen[0] || IsDegen[1])
    {
      //For start or end degenerated section
      //we return the whole bunch of longitudinal edges
      TopExp::MapShapesAndAncestors(myShape, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
      TopTools_IndexedMapOfShape Emap;
      const TopTools_ListOfShape& Elist = VEmap.FindFromKey(S);
      TopTools_ListIteratorOfListOfShape itl(Elist);
      for (; itl.More(); itl.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
        if (!BRep_Tool::Degenerated(anEdge))
        {
          TopoDS_Vertex VV [2];
          TopExp::Vertices(anEdge, VV[0], VV[1]);
          //Comprehensive check for possible case of
          //one vertex for start and end degenerated sections:
          //we must take only outgoing or only ingoing edges
          if ((IsDegen[0] && S.IsSame(VV[0])) ||
              (IsDegen[1] && S.IsSame(VV[1])))
            Emap.Add(anEdge);
        }
      }
      for (Standard_Integer j = 1; j <= Emap.Extent(); j++)
      {
        TopoDS_Edge anEdge = TopoDS::Edge(Emap(j));
        myGenerated.Append(anEdge);
        if (myIsRuled)
        {
          Standard_Integer i,k;
          for (i = 2,k = myWires.Length()-1; i < myWires.Length(); i++,k--)
          {
            Standard_Integer IndOfSec = (IsDegen[0])? i : k;
            TopoDS_Vertex aVertex = (IsDegen[0])?
              TopExp::LastVertex(anEdge) : TopExp::FirstVertex(anEdge);
            const TopTools_ListOfShape& EElist = VEmap.FindFromKey(aVertex);
            TopTools_IndexedMapOfShape EmapOfSection;
            TopExp::MapShapes(myWires(IndOfSec), TopAbs_EDGE, EmapOfSection);
            TopoDS_Edge NextEdge;
            for (itl.Initialize(EElist); itl.More(); itl.Next())
            {
              NextEdge = TopoDS::Edge(itl.Value());
              if (!NextEdge.IsSame(anEdge) &&
                  !EmapOfSection.Contains(NextEdge))
                break;
            }
            myGenerated.Append(NextEdge);
            anEdge = NextEdge;
          }
        }
      }
      return myGenerated;
    } //end of if (IsDegen[0] || IsDegen[1])
    
    Standard_Integer Eindex = myVertexIndex(S);
    Standard_Integer Vindex = (Eindex > 0)? 0 : 1;
    Eindex = Abs(Eindex);
    const TopoDS_Shape& FirstSection = myWires(1);
    TopoDS_Edge FirstEdge;
    TopoDS_Iterator itw(FirstSection);
    for (Standard_Integer inde = 1; itw.More(); itw.Next(),inde++)
    {
      FirstEdge = TopoDS::Edge(itw.Value());
      if (inde == Eindex)
        break;
    }

    //Find the first longitudinal edge
    TopoDS_Face FirstFace = TopoDS::Face(AllFaces(Eindex));
    FirstFace.Orientation(TopAbs_FORWARD);
    Explo.Init(FirstFace, TopAbs_EDGE);
    TopoDS_Edge anEdge;
    BRepAdaptor_Surface BAsurf(FirstFace, Standard_False);
    TopoDS_Vertex FirstVertex;
    TopExp::MapShapesAndAncestors(FirstFace, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
    if (myDegen1 && BAsurf.GetType() == GeomAbs_Plane)
    {
      //There are only 3 edges in the face in this case:
      //we take 1-st or 3-rd edge
      if (Vindex == 0)
      {
        Explo.Next();
        Explo.Next();
      }
      anEdge = TopoDS::Edge(Explo.Current());
    }
    else
    {
      TopoDS_Shape FirstEdgeInFace;
      FirstEdgeInFace = Explo.Current();
      TopoDS_Vertex VV [2];
      TopExp::Vertices(FirstEdge, VV[0], VV[1]);
      FirstVertex = VV[Vindex];
      const TopTools_ListOfShape& Elist = VEmap.FindFromKey(FirstVertex);
      TopTools_ListIteratorOfListOfShape itl(Elist);
      TopAbs_Orientation anEdgeOr = (Vindex == 0)? TopAbs_REVERSED : TopAbs_FORWARD;
      for (; itl.More(); itl.Next())
      {
        anEdge = TopoDS::Edge(itl.Value());
        if (!anEdge.IsSame(FirstEdgeInFace) &&
            !BRep_Tool::Degenerated(anEdge) &&
            anEdge.Orientation() == anEdgeOr)
          break;
      }
    }
    myGenerated.Append(anEdge);
    if (myIsRuled)
      //Find the chain of longitudinal edges from first to last
      for (Standard_Integer i = 2; i < myWires.Length(); i++)
      {
        FirstVertex = TopExp::LastVertex(anEdge);
        const TopTools_ListOfShape& Elist1 = VEmap.FindFromKey(FirstVertex);
        FirstEdge = (anEdge.IsSame(Elist1.First()))?
          TopoDS::Edge(Elist1.Last()) : TopoDS::Edge(Elist1.First());
        Eindex += myNbEdgesInSection;
        FirstFace = TopoDS::Face(AllFaces(Eindex));
        FirstFace.Orientation(TopAbs_FORWARD);
        VEmap.Clear();
        TopExp::MapShapesAndAncestors(FirstFace, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
        const TopTools_ListOfShape& Elist2 = VEmap.FindFromKey(FirstVertex);
        anEdge = (FirstEdge.IsSame(Elist2.First()))?
          TopoDS::Edge(Elist2.Last()) : TopoDS::Edge(Elist2.First());
        myGenerated.Append(anEdge);
      }
  }

  return myGenerated;
}

//=======================================================================
//function : GeneratedFace
//purpose  : 
//=======================================================================

TopoDS_Shape BRepOffsetAPI_ThruSections::GeneratedFace(const TopoDS_Shape& edge) const
{
  TopoDS_Shape bid;
  if (myEdgeFace.IsBound(edge)) {
    return myEdgeFace(edge);
  }
  else {
    return bid;
  }
}


//=======================================================================
//function : CriteriumWeight
//purpose  : returns the Weights associated  to the criterium used in
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
  if (W1 < 0 || W2 < 0 || W3 < 0 ) throw Standard_DomainError();
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





