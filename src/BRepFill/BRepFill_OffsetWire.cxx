// Created on: 1995-04-20
// Created by: Bruno DUMORTIER
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

//  Modified by skv - Fri Jul  8 11:21:38 2005 OCC9145

#include <Adaptor3d_Curve.hxx>
#include <Adaptor2d_OffsetCurve.hxx>
#include <Bisector_Bisec.hxx>
#include <BRep_Builder.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepFill_DataMapOfNodeShape.hxx>
#include <BRepFill_DataMapOfOrientedShapeListOfShape.hxx>
#include <BRepFill_DataMapOfShapeSequenceOfPnt.hxx>
#include <BRepFill_DataMapOfShapeSequenceOfReal.hxx>
#include <BRepFill_OffsetWire.hxx>
#include <BRepFill_TrimEdgeTool.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepMAT2d_BisectingLocus.hxx>
#include <BRepMAT2d_Explorer.hxx>
#include <BRepMAT2d_LinkTopoBilo.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Substitution.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <MAT2d_CutCurve.hxx>
#include <MAT_Arc.hxx>
#include <MAT_Graph.hxx>
#include <MAT_Node.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeSequenceOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

#include <stdio.h>
#ifdef OCCT_DEBUG
//#define DRAW
#ifdef DRAW
#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Curve2d.hxx>
#include <DBRep.hxx>
#include <Geom_Curve.hxx>
static Standard_Boolean AffichGeom  = Standard_False;
static Standard_Boolean Affich2d    = Standard_False;
static Standard_Boolean AffichEdge  = Standard_False;
static Standard_Integer NbTRIMEDGES = 0;
static Standard_Integer NbOFFSET    = 0;
static Standard_Integer NbEDGES     = 0;
static Standard_Integer NbBISSEC    = 0;
#endif
#endif

//  Modified by Sergey KHROMOV - Thu Nov 16 17:24:39 2000 Begin

static void QuasiFleche(const Adaptor3d_Curve& C,
  const Standard_Real Deflection2, 
  const Standard_Real Udeb,
  const gp_Pnt& Pdeb,
  const gp_Vec& Vdeb,
  const Standard_Real Ufin,
  const gp_Pnt& Pfin,
  const gp_Vec& Vfin,
  const Standard_Integer Nbmin,
  const Standard_Real Eps,
  TColStd_SequenceOfReal& Parameters,
  TColgp_SequenceOfPnt& Points);

static Standard_Boolean PerformCurve (TColStd_SequenceOfReal& Parameters,
  TColgp_SequenceOfPnt&   Points,
  const Adaptor3d_Curve& C, 
  const Standard_Real Deflection,
  const Standard_Real U1,
  const Standard_Real U2,
  const Standard_Real EPSILON,
  const Standard_Integer Nbmin);

static void CheckBadEdges(const TopoDS_Face& Spine, const Standard_Real Offset,
  const BRepMAT2d_BisectingLocus& Locus, 
  const BRepMAT2d_LinkTopoBilo&   Link,
  TopTools_ListOfShape& BadEdges);

static Standard_Integer CutEdge (const TopoDS_Edge& E, 
  const TopoDS_Face& F,
  Standard_Integer ForceCut,
  TopTools_ListOfShape& Cuts);


static void CutCurve (const Handle(Geom2d_TrimmedCurve)& C,
  const Standard_Integer nbParts,
  TColGeom2d_SequenceOfCurve& theCurves);
//  Modified by Sergey KHROMOV - Thu Nov 16 17:24:47 2000 End


static void EdgeVertices (const TopoDS_Edge&   E,
  TopoDS_Vertex& V1, 
  TopoDS_Vertex& V2)
{
  if (E.Orientation() == TopAbs_REVERSED) {
    TopExp::Vertices(E,V2,V1);
  }
  else {
    TopExp::Vertices(E,V1,V2);
  }
}
static Standard_Boolean VertexFromNode
  (const Handle(MAT_Node)&      aNode, 
  const Standard_Real          Offset,
  gp_Pnt2d&                    PN,
  BRepFill_DataMapOfNodeShape& MapNodeVertex,
  TopoDS_Vertex&               VN);

static void StoreInMap (const TopoDS_Shape& V1,
  const TopoDS_Shape& V2,
  TopTools_IndexedDataMapOfShapeShape& MapVV);

static void TrimEdge (const TopoDS_Edge&              CurrentEdge,
                      const TopoDS_Shape&             CurrentSpine,
                      const TopoDS_Face&              AllSpine,
                      const TopTools_ListOfShape&     D,
                      TopTools_SequenceOfShape& Sv,  
                      TColStd_SequenceOfReal&   MapverPar,
                      TopTools_SequenceOfShape& S,
                      TopTools_IndexedDataMapOfShapeShape& MapVV,
                      const Standard_Integer IndOfE);

static Standard_Boolean IsInnerEdge(const TopoDS_Shape& ProE,
                                    const TopoDS_Face&  AllSpine,
                                    Standard_Real& TrPar1,
                                    Standard_Real& TrPar2);

static Standard_Boolean DoubleOrNotInside (const TopTools_ListOfShape& EC,
  const TopoDS_Vertex&        V);

static Standard_Boolean IsSmallClosedEdge(const TopoDS_Edge& anEdge,
  const TopoDS_Vertex& aVertex);

static void MakeCircle 
  (const TopoDS_Edge&                          E, 
  const TopoDS_Vertex&                        V, 
  const TopoDS_Face&                          F,
  const Standard_Real                         Offset, 
  BRepFill_IndexedDataMapOfOrientedShapeListOfShape& Map,
  const Handle(Geom_Plane)&                   RefPlane);

static void MakeOffset 
  (const TopoDS_Edge&                          E,
  const TopoDS_Face&                          F,
  const Standard_Real                         Offset, 
  BRepFill_IndexedDataMapOfOrientedShapeListOfShape& Map,
  const Handle(Geom_Plane)&                   RefPlane,
  const Standard_Boolean                      IsOpenResult,
 const GeomAbs_JoinType                      theJoinType,
  const TopoDS_Vertex *                       Ends);

Standard_Boolean CheckSmallParamOnEdge(const TopoDS_Edge& anEdge);

//=======================================================================
//function : KPartCircle
//purpose  : 
//=======================================================================

static Standard_Boolean KPartCircle
  (const TopoDS_Face&  mySpine,
  const Standard_Real myOffset,
  const Standard_Boolean IsOpenResult,
  const Standard_Real Alt,
  TopoDS_Shape&       myShape, 
  BRepFill_IndexedDataMapOfOrientedShapeListOfShape& myMap,
  Standard_Boolean&    myIsDone)
{
  TopExp_Explorer exp(mySpine,TopAbs_EDGE);
  Standard_Integer NbEdges = 0;
  TopoDS_Edge      E;

  for (; exp.More(); exp.Next()) {
    NbEdges++;
    E = TopoDS::Edge(exp.Current());
    if (NbEdges > 1) return Standard_False;
  }

  Standard_Real      f,l;
  TopLoc_Location    L;
  Handle(Geom_Curve) C =  BRep_Tool::Curve(E,L,f,l);

  if (C->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    Handle(Geom_TrimmedCurve) Ct = Handle(Geom_TrimmedCurve)::DownCast(C);
    C = Ct->BasisCurve();
  }

  if ((C->IsKind(STANDARD_TYPE(Geom_Circle)) && BRep_Tool::IsClosed(E)) || //closed circle
      IsOpenResult)
  {
    Standard_Real anOffset = myOffset;
    
    Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(E, mySpine, f, l);
    Handle(Geom2dAdaptor_HCurve) AHC = new Geom2dAdaptor_HCurve(aPCurve, f, l);
    Handle(Geom2d_Curve) OC;
    if (AHC->GetType() == GeomAbs_Line)
    {
      if (E.Orientation() == TopAbs_REVERSED)
        anOffset *= -1;
      Adaptor2d_OffsetCurve Off(AHC,anOffset);
      OC = new Geom2d_Line(Off.Line());
    }
    else if (AHC->GetType() == GeomAbs_Circle)
    {
      if (E.Orientation() == TopAbs_FORWARD)
        anOffset *= -1;
      gp_Circ2d theCirc = AHC->Circle();
      if (anOffset > 0. || Abs(anOffset) < theCirc.Radius())
        OC = new Geom2d_Circle (theCirc.Position(), theCirc.Radius() + anOffset);
      else
      {
        myIsDone = Standard_False;
        return Standard_False;
    }
    }
    else
    {
      if (E.Orientation() == TopAbs_FORWARD)
        anOffset *= -1;
      Handle(Geom2d_TrimmedCurve) G2dT = new Geom2d_TrimmedCurve(aPCurve, f, l);
      OC = new Geom2d_OffsetCurve( G2dT, anOffset);
    }
    Handle(Geom_Surface) aSurf = BRep_Tool::Surface(mySpine);
    Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurf);
    myShape = BRepLib_MakeEdge(OC, aPlane, f, l);
    BRepLib::BuildCurve3d(TopoDS::Edge(myShape));
    
    myShape.Orientation(E.Orientation());
    myShape.Location(L);
    if (fabs(Alt) > gp::Resolution()) {
      BRepAdaptor_Surface S(mySpine,0);
      gp_Ax1 Nor = S.Plane().Axis();
      gp_Trsf T;
      gp_Vec Trans(Nor.Direction());
      Trans = Alt*Trans;
      T.SetTranslation(Trans);
      myShape.Move(TopLoc_Location(T));
    }
      
    TopTools_ListOfShape LL;
    LL.Append(myShape);
    myMap.Add(E,LL);
    
    TopoDS_Edge myEdge = TopoDS::Edge(myShape);
    myShape = BRepLib_MakeWire(myEdge);
    
    myIsDone = Standard_True;
    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : BRepFill_OffsetWire
//purpose  : 
//=======================================================================

BRepFill_OffsetWire::BRepFill_OffsetWire() 
  : myIsOpenResult(Standard_False),
    myIsDone(Standard_False)
{
}


//=======================================================================
//function : BRepFill_OffsetWire
//purpose  : 
//=======================================================================

BRepFill_OffsetWire::BRepFill_OffsetWire(const TopoDS_Face&     Spine,
  const GeomAbs_JoinType Join,
  const Standard_Boolean IsOpenResult)
{
  Init(Spine,Join,IsOpenResult);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepFill_OffsetWire::Init(const TopoDS_Face&     Spine,
  const GeomAbs_JoinType Join,
  const Standard_Boolean IsOpenResult)
{
  myIsDone   = Standard_False;
  TopoDS_Shape aLocalShape = Spine.Oriented(TopAbs_FORWARD);
  mySpine    = TopoDS::Face(aLocalShape);
  //  mySpine    = TopoDS::Face(Spine.Oriented(TopAbs_FORWARD));
  myJoinType = Join;
  myIsOpenResult = IsOpenResult;
  
  myMap.Clear();
  myMapSpine.Clear();
  //------------------------------------------------------------------
  // cut the spine for bissectors.
  //------------------------------------------------------------------
  //  Modified by Sergey KHROMOV - Tue Nov 26 17:39:03 2002 Begin
  static BRepMAT2d_Explorer Exp;

  Exp.Perform(mySpine);

  //  TopoDS_Face anOldSpine = mySpine;

  mySpine = TopoDS::Face(Exp.ModifiedShape(mySpine));
  PrepareSpine  ();

  //  Modified by Sergey KHROMOV - Tue Nov 26 17:39:03 2002 End
  TopoDS_Shape aShape;
  BRepFill_IndexedDataMapOfOrientedShapeListOfShape aMap;
  Standard_Boolean Done;
  if (KPartCircle(myWorkSpine,1.,myIsOpenResult,0.,aShape,aMap,Done)) return;


  //-----------------------------------------------------
  // Calculate the map of bissectors to the left.  
  // and Links Topology -> base elements of the map.
  //-----------------------------------------------------
  
  //  Modified by Sergey KHROMOV - Tue Nov 26 17:39:03 2002 Begin
  //   static BRepMAT2d_Explorer Exp;
  //  Modified by Sergey KHROMOV - Tue Nov 26 17:39:03 2002 End
  Exp.Perform(myWorkSpine);
  myBilo.Compute(Exp, 1 ,MAT_Left, myJoinType, myIsOpenResult);
  myLink.Perform(Exp,myBilo);
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_OffsetWire::IsDone() const 
{
  return myIsDone;
}


//=======================================================================
//function : Spine
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepFill_OffsetWire::Spine() const 
{
  return mySpine;
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepFill_OffsetWire::Shape() const 
{
  return myShape;
}


//=======================================================================
//function : GeneratedShapes
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFill_OffsetWire::GeneratedShapes
  (const TopoDS_Shape& SpineShape)
{  
  if (!myCallGen) {
    if (!myMapSpine.IsEmpty()) {
      // myMapSpine can be empty if passed by PerformWithBilo.
      TopTools_DataMapIteratorOfDataMapOfShapeShape it(myMapSpine);
      for (; it.More(); it.Next()) {
        if (myMap.Contains(it.Key())) {
          if (!myMap.Contains(it.Value())) {
            TopTools_ListOfShape L;
            myMap.Add(it.Value(),L);
          }
          if ( !it.Value().IsSame(it.Key())) {
            myMap.ChangeFromKey(it.Value()).Append(myMap.ChangeFromKey(it.Key()));
            myMap.RemoveKey(it.Key());
          }
        }
        if (myMap.Contains(it.Key().Reversed())) {
          if (!myMap.Contains(it.Value().Reversed())) {
            TopTools_ListOfShape L;
            myMap.Add(it.Value().Reversed(),L);
          }
          if ( !it.Value().IsSame(it.Key())) {
            myMap.ChangeFromKey(it.Value().Reversed()).Append(myMap.ChangeFromKey(it.Key().Reversed()));
            myMap.RemoveKey(it.Key().Reversed());
          }
        }
      }
    }
    myCallGen = Standard_True;
  }

  if (myMap.Contains(SpineShape)) {
    return myMap.FindFromKey(SpineShape);
  }
  else {
    static TopTools_ListOfShape Empty;
    return Empty;
  }
}


//=======================================================================
//function : JoinType
//purpose  : 
//=======================================================================

GeomAbs_JoinType BRepFill_OffsetWire::JoinType() const 
{
  return myJoinType;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepFill_OffsetWire::Perform (const Standard_Real Offset,
  const Standard_Real Alt)
{
  //  Modified by skv - Fri Jul  8 11:21:38 2005 OCC9145 Begin
  try
  {
    OCC_CATCH_SIGNALS
      myCallGen = Standard_False;
    if (KPartCircle(myWorkSpine,Offset,myIsOpenResult,Alt,myShape,myMap,myIsDone)) return;

    TopoDS_Face oldWorkSpain = myWorkSpine;

    TopTools_ListOfShape BadEdges;
    CheckBadEdges(myWorkSpine,Offset,myBilo,myLink,BadEdges);

    if(!BadEdges.IsEmpty())
    {
      // Modification of myWorkSpine;
      //cout << "Modification of myWorkSpine : " << BadEdges.Extent() << endl;
      BRepTools_Substitution aSubst;
      TopTools_ListIteratorOfListOfShape it(BadEdges);
      TopTools_ListOfShape aL;
      Standard_Real aDefl = .01 * Abs(Offset);
      TColStd_SequenceOfReal Parameters;
      TColgp_SequenceOfPnt Points;

      for(; it.More(); it.Next()) {
        aL.Clear();
        Parameters.Clear();
        Points.Clear();
        const TopoDS_Shape& anE = it.Value();

        TopoDS_Vertex Vf, Vl;
        TopExp::Vertices(TopoDS::Edge(anE), Vf, Vl);

        Standard_Real f, l;
        Handle(Geom_Curve) G3d = BRep_Tool::Curve(TopoDS::Edge(anE),f,l);
        GeomAdaptor_Curve  AC(G3d,f,l);

        PerformCurve(Parameters, Points, AC, aDefl, f, 
          l, Precision::Confusion(), 2);

        Standard_Integer NPnts = Points.Length();
        if(NPnts > 2)
        {
          //cout << NPnts << " points " << endl;
          TopoDS_Vertex FV = Vf;
          TopoDS_Vertex LV;
          TopoDS_Edge newE;
          Standard_Integer np;
          for(np = 2; np < NPnts; np++) {
            gp_Pnt LP = Points(np);
            LV = BRepLib_MakeVertex(LP);
            newE = BRepLib_MakeEdge(FV, LV);
            aL.Append(newE);
            FV = LV;
          }
          LV = Vl;
          newE = BRepLib_MakeEdge(FV, LV);
          aL.Append(newE);
        }
        else
        {
          //cout << " 2 points " << endl;
          TopoDS_Edge newE = BRepLib_MakeEdge(Vf, Vl);
          aL.Append(newE);
        }
        //Update myMapSpine
        if (myMapSpine.IsBound( anE ))
        {
          TopTools_ListIteratorOfListOfShape newit( aL );
          for (; newit.More(); newit.Next())
          {
            TopoDS_Edge NewEdge = TopoDS::Edge( newit.Value() );
            myMapSpine.Bind( NewEdge, myMapSpine(anE) );
            TopoDS_Vertex NewV1, NewV2;
            EdgeVertices( NewEdge, NewV1, NewV2 );
            if (!myMapSpine.IsBound(NewV1)) myMapSpine.Bind( NewV1, myMapSpine(anE) );
            if (!myMapSpine.IsBound(NewV2)) myMapSpine.Bind( NewV2, myMapSpine(anE) );
          }
          myMapSpine.UnBind( anE );
        }
        ///////////////////
        aSubst.Substitute(anE, aL);
      }

      TopTools_DataMapOfShapeListOfShape wwmap;
      TopoDS_Iterator itws( myWorkSpine );
      for (; itws.More(); itws.Next())
      {
        TopoDS_Shape aWire = itws.Value();
        aSubst.Build( aWire );
        if (aSubst.IsCopied(aWire))
        {
          TopoDS_Wire NewWire = TopoDS::Wire( aSubst.Copy(aWire).First() );
          NewWire.Closed( aWire.Closed() );
          TopTools_ListOfShape Lw;
          Lw.Append( NewWire );
          wwmap.Bind( aWire, Lw );
        }
      }
      aSubst.Clear();
      TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itmap( wwmap );
      for (; itmap.More(); itmap.Next())
        aSubst.Substitute( itmap.Key(), itmap.Value() );

      aSubst.Build(myWorkSpine);

      if(aSubst.IsCopied(myWorkSpine)) {
        myWorkSpine = TopoDS::Face(aSubst.Copy(myWorkSpine).First());

        BRepMAT2d_Explorer newExp;
        newExp.Perform(myWorkSpine);
        BRepMAT2d_BisectingLocus newBilo;
        BRepMAT2d_LinkTopoBilo newLink;
        newBilo.Compute(newExp, 1, MAT_Left, myJoinType, myIsOpenResult);

        if(!newBilo.IsDone())
        {
          myShape.Nullify();
          myIsDone = Standard_False;
          return;
        }

        newLink.Perform(newExp,newBilo);
        PerformWithBiLo(myWorkSpine,Offset,newBilo,newLink,myJoinType,Alt);
        myWorkSpine = oldWorkSpain;
      }
      else {
        PerformWithBiLo(myWorkSpine,Offset,myBilo,myLink,myJoinType,Alt);
      }
    }
    else
    {
      PerformWithBiLo(myWorkSpine,Offset,myBilo,myLink,myJoinType,Alt);
    }
  }
  catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout<<"An exception was caught in BRepFill_OffsetWire::Perform : ";
    anException.Print(cout);
    cout<<endl;
#endif
    (void)anException;
    myShape.Nullify();
    myIsDone = Standard_False;

    return;
  }

  //  Modified by skv - Fri Jul  8 11:21:38 2005 OCC9145 End
  //  Modified by Sergey KHROMOV - Thu Mar 14 10:48:15 2002 Begin
  if (!myIsOpenResult)
  {
    TopExp_Explorer anExp(myShape, TopAbs_WIRE);

    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape &aWire = anExp.Current();

      if (!aWire.Closed()) {
        myShape.Nullify();
        myIsDone = Standard_False;
        throw Standard_ConstructionError("Offset wire is not closed.");
      }
    }
  }
  //  Modified by Sergey KHROMOV - Thu Mar 14 10:48:16 2002 End
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void Compute (const TopoDS_Face&  Spine,
  TopoDS_Shape& aShape,
  BRepFill_IndexedDataMapOfOrientedShapeListOfShape& Map,
  const Standard_Real Alt)
{
  BRep_Builder B;
  B.MakeCompound(TopoDS::Compound(aShape));
  Standard_Real ALT = Alt;
  if ( Spine.Orientation() == TopAbs_REVERSED) ALT = -Alt;
  gp_Trsf T;
  T.SetTranslation(gp_Vec(0.,0.,ALT));
  TopLoc_Location L(T);

  for ( TopExp_Explorer exp(Spine,TopAbs_WIRE); exp.More(); exp.Next()) {
    const TopoDS_Wire& CurW = TopoDS::Wire(exp.Current());
    TopoDS_Shape aLocalShape = CurW.Moved(L);
    TopoDS_Wire        NewW = TopoDS::Wire(aLocalShape);
    //    TopoDS_Wire        NewW = TopoDS::Wire(CurW.Moved(L));
    B.Add(aShape,NewW);
    // update Map.
    TopoDS_Iterator it1( CurW);
    TopoDS_Iterator it2( NewW);
    for ( ; it1.More(); it1.Next(), it2.Next()) {
      TopTools_ListOfShape List;
      List.Append(it2.Value());
      Map.Add(it1.Value(), List);
    }
  }
}

//=======================================================================
//function : PerformWithBiLo
//purpose  : 
//=======================================================================

void BRepFill_OffsetWire::PerformWithBiLo
  (const TopoDS_Face&              Spine,
  const Standard_Real             Offset,
  const BRepMAT2d_BisectingLocus& Locus, 
  BRepMAT2d_LinkTopoBilo&   Link,
  const GeomAbs_JoinType          Join,
  const Standard_Real             Alt)
{
  myIsDone     = Standard_False;
  TopoDS_Shape aLocalShapeWork = Spine.Oriented(TopAbs_FORWARD);
  myWorkSpine  = TopoDS::Face(aLocalShapeWork);
  //  myWorkSpine  = TopoDS::Face(Spine.Oriented(TopAbs_FORWARD));
  myJoinType   = Join;
  myOffset     = Offset ;
  myShape.Nullify();


  if (mySpine.IsNull()) {
    TopoDS_Shape aLocalShape = Spine.Oriented(TopAbs_FORWARD);
    mySpine = TopoDS::Face(aLocalShape);
    //    mySpine = TopoDS::Face(Spine.Oriented(TopAbs_FORWARD));
  }
  myMap.Clear();

  if ( Abs(myOffset) < Precision::Confusion()) {
    Compute(mySpine,myShape,myMap,Alt);
    myIsDone = Standard_True;
    return;
  }

  //********************************
  // Calculate for a non null offset 
  //********************************
  if (KPartCircle(myWorkSpine,Offset,myIsOpenResult,Alt,myShape,myMap,myIsDone))
    return;

  BRep_Builder myBuilder;
  myBuilder.MakeCompound(TopoDS::Compound(myShape));

  //---------------------------------------------------------------------
  // MapNodeVertex : associate to each node of the map (key1) and to
  //                 each element of the profile (key2) a vertex (item).
  // MapBis        : all edges or vertices (item) generated by 
  //                 a bisectrice on a face or an edge (key) of revolution tubes.
  // MapVerPar     : Map of parameters of vertices on parallel edges 
  //                 the list contained in MapVerPar (E) corresponds to 
  //                 parameters on E of vertices contained in MapBis(E);
  //---------------------------------------------------------------------


  BRepFill_DataMapOfNodeShape               MapNodeVertex; 
  TopTools_DataMapOfShapeSequenceOfShape    MapBis;  
  BRepFill_DataMapOfShapeSequenceOfReal     MapVerPar;

  TopTools_DataMapOfShapeShape              EmptyMap;
  TopTools_SequenceOfShape                  EmptySeq;
  TopTools_ListOfShape                      EmptyList;
  TColStd_SequenceOfReal                    EmptySeqOfReal;

  Handle(Geom_Plane) RefPlane = 
    Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(myWorkSpine));
  if (fabs(Alt) > gp::Resolution()) {
    Standard_Real anAlt = Alt;
    if ( myWorkSpine.Orientation() == TopAbs_REVERSED) anAlt = -Alt;
    RefPlane = Handle(Geom_Plane)::DownCast
      (RefPlane->Translated( anAlt * gp_Vec(RefPlane->Axis().Direction() )));
  }

  //---------------------------------------------------------------
  // Construction of Circles and OffsetCurves
  //---------------------------------------------------------------

  TopoDS_Vertex Ends [2];
  if (myIsOpenResult)
  {
    TopoDS_Wire theWire;
    TopoDS_Iterator iter(mySpine);
    theWire = TopoDS::Wire(iter.Value());
    TopExp::Vertices(theWire, Ends[0], Ends[1]);
  }

  for (Standard_Integer ic = 1; ic <= Locus.NumberOfContours(); ic++) {
    TopoDS_Shape PEE = Link.GeneratingShape(Locus.BasicElt(ic,Locus.NumberOfElts(ic)));
    TopoDS_Shape& PE = PEE ;      
    for (Standard_Integer ie = 1; ie <= Locus.NumberOfElts(ic); ie++) {
      const TopoDS_Shape& SE = Link.GeneratingShape(Locus.BasicElt(ic,ie));
      if (SE.ShapeType() == TopAbs_VERTEX) {
        if (!SE.IsSame(Ends[0]) && !SE.IsSame(Ends[1]))
          MakeCircle (TopoDS::Edge(PE),TopoDS::Vertex(SE),
                      myWorkSpine,myOffset,myMap,RefPlane);
      }
      else {
        MakeOffset (TopoDS::Edge(SE),myWorkSpine,myOffset,myMap,RefPlane,
                    myIsOpenResult, myJoinType, Ends);
        PE = SE;
      }
    }
  }

  //Remove possible hanging arcs on vertices
  if (myIsOpenResult && myJoinType == GeomAbs_Arc)
  {
    if (!myMap.IsEmpty() &&
        myMap.FindKey(1).ShapeType() == TopAbs_VERTEX)
    {
      myMap.RemoveFromIndex(1);
    }
    if (!myMap.IsEmpty() &&
        myMap.FindKey(myMap.Extent()).ShapeType() == TopAbs_VERTEX)
      myMap.RemoveLast();
  }

#ifdef OCCT_DEBUG
#ifdef DRAW
  if (AffichEdge) {
    cout << " End Construction of geometric primitives "<<endl;
  }
#endif
#endif


  //---------------------------------------------------
  // Construction of offset vertices.
  //---------------------------------------------------
  BRepFill_DataMapOfOrientedShapeListOfShape Detromp;
  Handle(MAT_Arc)        CurrentArc;
  Handle(Geom2d_Curve)   Bis, PCurve1, PCurve2 ;
  Handle(Geom_Curve)     CBis;
  Standard_Boolean       Reverse;
  TopoDS_Edge            CurrentEdge;
  TopoDS_Shape           S       [2];
  TopoDS_Edge            E       [2];
  TopLoc_Location        L;
  Standard_Integer       j, k;

  for (Standard_Integer i = 1; i <= Locus.Graph()->NumberOfArcs(); i++) {

    CurrentArc           = Locus.Graph()->Arc(i);
    Bisector_Bisec Bisec = Locus.GeomBis(CurrentArc,Reverse);
#ifdef OCCT_DEBUG
#ifdef DRAW

    if ( AffichGeom) {
      char name[256];
      sprintf(name,"BISSEC_%d",NbBISSEC++);
      DrawTrSurf::Set(name,Bisec.Value());
    }
#endif
#endif

    //-------------------------------------------------------------------
    // Return elements of the spine corresponding to separate basicElts.
    //-------------------------------------------------------------------
    S [0] = Link.GeneratingShape(CurrentArc->FirstElement());
    S [1] = Link.GeneratingShape(CurrentArc->SecondElement());

    TopTools_SequenceOfShape Vertices;
    TColgp_SequenceOfPnt     Params;

    TopTools_DataMapOfShapeSequenceOfShape MapSeqVer;
    BRepFill_DataMapOfShapeSequenceOfPnt   MapSeqPar;

    //-----------------------------------------------------------
    // Return parallel edges on each face.
    // If no offset generated => move to the next bissectrice. 
    //--------------------------------------------------------------
    if (myMap.Contains(S[0]) && myMap.Contains(S[1])) {
      E [0] = TopoDS::Edge(myMap.FindFromKey(S[0]).First());
      E [1] = TopoDS::Edge(myMap.FindFromKey(S[1]).First());
    }
    else continue;

    //-----------------------------------------------------------
    // Construction of vertices corresponding to the node of the map.
    // if they are on the offset.
    //-----------------------------------------------------------
    TopoDS_Vertex VS,VE;
    Handle(MAT_Node) Node1, Node2;

    if (Reverse) {
      Node1 = CurrentArc->SecondNode();
      Node2 = CurrentArc->FirstNode();
    }
    else  {
      Node1 = CurrentArc->FirstNode();
      Node2 = CurrentArc->SecondNode();
    }

    Standard_Boolean StartOnEdge = 0, EndOnEdge = 0;

    if (!Node1->Infinite()) {
      gp_Pnt2d aLocalPnt2d = Locus.GeomElt(Node1);
      StartOnEdge = VertexFromNode(Node1, myOffset, aLocalPnt2d ,MapNodeVertex,VS);
      //      StartOnEdge = VertexFromNode(Node1, myOffset, Locus.GeomElt(Node1),
      //				   MapNodeVertex,VS);
    }
    if (!Node2->Infinite()) {
      gp_Pnt2d aLocalPnt2d = Locus.GeomElt(Node2) ;
      EndOnEdge   = VertexFromNode(Node2, myOffset, aLocalPnt2d ,MapNodeVertex,VE);
      //      EndOnEdge   = VertexFromNode(Node2, myOffset, Locus.GeomElt(Node2),
      //				   MapNodeVertex,VE);
    }

    if (myJoinType == GeomAbs_Intersection)
      StartOnEdge = EndOnEdge = 0;

    //---------------------------------------------
    // Construction of geometries.
    //---------------------------------------------
    BRepFill_TrimEdgeTool Trim (Bisec, 
      Locus.GeomElt(CurrentArc->FirstElement()),
      Locus.GeomElt(CurrentArc->SecondElement()),
      myOffset);

    //-----------------------------------------------------------
    // Construction of vertices on edges parallel to the spine.
    //-----------------------------------------------------------

    Trim.IntersectWith(E[0], E[1], S[0], S[1], Ends[0], Ends[1],
                       myJoinType, myIsOpenResult, Params);

    for (Standard_Integer s = 1; s <= Params.Length(); s++) {
      TopoDS_Vertex VC;
      myBuilder.MakeVertex (VC);
      gp_Pnt2d P2  = Bisec.Value()->Value(Params.Value(s).X());
      gp_Pnt   PVC(P2.X(),P2.Y(),0.);

      myBuilder.UpdateVertex(VC,PVC,Precision::Confusion());
      Vertices.Append(VC);
    }
    if (StartOnEdge) {
      Standard_Boolean Start = 1;
      Trim.AddOrConfuse(Start, E[0], E[1], Params);
      if (Params.Length() == Vertices.Length()) 
        Vertices.SetValue(1,VS);
      
      else
        // the point was not found by IntersectWith
        Vertices.Prepend(VS);
    }
    if (EndOnEdge) {	  
      Standard_Boolean Start = 0;
      Trim.AddOrConfuse(Start, E[0], E[1], Params);
      if (Params.Length() == Vertices.Length()) 
        Vertices.SetValue(Params.Length(),VE);
      
      else
        // the point was not found by IntersectWith
        Vertices.Append(VE);
    }

    //------------------------------------------------------------
    // Update Detromp.
    // Detromp allows to remove vertices on the offset 
    // corresponding to tangency zones
    // Detromp ranks the vertices that limit
    // the parts of the bissectrices located between the spine and the 
    // offset.
    //------------------------------------------------------------
    if (!Detromp.IsBound(S[0])) Detromp.Bind(S[0],EmptyList);
    if (!Detromp.IsBound(S[1])) Detromp.Bind(S[1],EmptyList);

    
    UpdateDetromp (Detromp, S[0], S[1], Vertices, Params, 
      Bisec, StartOnEdge, EndOnEdge, Trim);
    //----------------------------------------------
    // Storage of vertices on parallel edges.
    // fill MapBis and MapVerPar.
    //----------------------------------------------
    if (!Vertices.IsEmpty()) {
      for (k = 0; k <= 1; k++) {
        if (!MapBis.IsBound(E[k])) {
          MapBis   .Bind(E[k],EmptySeq);
          MapVerPar.Bind(E[k],EmptySeqOfReal);
        } 
        for (Standard_Integer ii = 1; ii <= Vertices.Length(); ii++) {
          MapBis (E[k]).Append(Vertices.Value(ii));
          if (k == 0) MapVerPar (E[k]).Append(Params.Value(ii).Y());
          else        MapVerPar (E[k]).Append(Params.Value(ii).Z());
        }
      }
    }
    else {
      //------------------------------------------------------------
      // FOR COMPLETE CIRCLES. the parallel line can be contained
      // in the zone without intersection with the border
      // no intersection 
      // if myoffset is < distance of nodes the parallel can be valid.
      //-------------------------------------------------------------
      for (k = 0; k <= 1; k++) {
        if (!MapBis.IsBound(E[k])) {
          if (Node1->Distance() > myOffset && Node2->Distance() > myOffset) {
            MapBis   .Bind(E[k],EmptySeq); 
            MapVerPar.Bind(E[k],EmptySeqOfReal);
          }
        }
      }
    }
  }

#ifdef OCCT_DEBUG
#ifdef DRAW
  if (AffichEdge) {
    cout << " End Construction of vertices on offsets"<<endl;
  }
#endif
#endif

  //----------------------------------
  // Construction of parallel edges.
  //----------------------------------
  TopTools_IndexedDataMapOfShapeShape MapVV;

  TopoDS_Shape CurrentSpine;

  //BRepFill_DataMapIteratorOfDataMapOfOrientedShapeListOfShape ite1;  

  for (j = 1; j <= myMap.Extent(); j++) {
    CurrentSpine = myMap.FindKey(j);
    CurrentEdge  = TopoDS::Edge(myMap(j).First());

    myMap(j).Clear();
    if (MapBis.IsBound(CurrentEdge)) {
      TopTools_SequenceOfShape aSeqOfShape;
      if (!MapBis(CurrentEdge).IsEmpty()) {
        Standard_Integer IndOfE = 0;
        if (myIsOpenResult)
        {
          if (j == 1)
            IndOfE = 1;
          else if (j == myMap.Extent())
            IndOfE = -1;
        }
        TrimEdge (CurrentEdge,
          CurrentSpine,
          mySpine,
          Detromp  (CurrentSpine),
          MapBis   (CurrentEdge) ,  
          MapVerPar(CurrentEdge) ,
          aSeqOfShape, MapVV, IndOfE);
        for ( k = 1; k <= aSeqOfShape.Length(); k++) {
          myMap(j).Append(aSeqOfShape.Value(k));
        }
      }
      else {
        //-----------------
        // Complete circles
        //-----------------
        myMap(j).Append(CurrentEdge);
      }
    }
  }

  Standard_Integer ind;
  for (ind = 1; ind <= MapVV.Extent(); ind++)
  {
    TopoDS_Vertex OldV = TopoDS::Vertex(MapVV.FindKey(ind));
    TopoDS_Vertex NewV = TopoDS::Vertex(MapVV(ind));
    gp_Pnt P1 = BRep_Tool::Pnt(OldV);
    gp_Pnt P2 = BRep_Tool::Pnt(NewV);
    myBuilder.UpdateVertex(NewV, P1.Distance(P2));
    TopTools_ListOfShape LV;
    LV.Append( NewV.Oriented(TopAbs_FORWARD) );
    BRepTools_Substitution aSubst;
    aSubst.Substitute( OldV, LV );
    for (j = 1; j <= myMap.Extent(); j++)
    {
      TopTools_ListIteratorOfListOfShape itl(myMap(j));
      for (; itl.More(); itl.Next())
      {
        aSubst.Build(itl.Value());
        if (aSubst.IsCopied(itl.Value()))
        {
          const TopTools_ListOfShape& listSh = aSubst.Copy(itl.Value());
          TopAbs_Orientation SaveOr = itl.Value().Orientation();
          itl.Value() = listSh.First();
          itl.Value().Orientation(SaveOr);
        }
      }
    }
  }
      
  //----------------------------------
  // Construction of offset wires.
  //----------------------------------
  MakeWires ();

  // Update vertices ( Constructed in the plane Z = 0) !!!
  TopTools_MapOfShape MapVertex;
  for ( TopExp_Explorer exp(myShape,TopAbs_VERTEX); exp.More(); exp.Next()) {
    TopoDS_Vertex V = TopoDS::Vertex(exp.Current());
    if ( MapVertex.Add(V)) {
      gp_Pnt        P = BRep_Tool::Pnt(V);
      P = RefPlane->Value(P.X(),P.Y());
      myBuilder.UpdateVertex(V,P, Precision::Confusion());
    }
  }

  // Construction of curves 3d.
  BRepLib::BuildCurves3d(myShape);
  MapVertex.Clear();
  TopExp_Explorer Explo( myShape, TopAbs_EDGE );
  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Edge anEdge = TopoDS::Edge( Explo.Current() );
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anEdge, V1, V2 );
    Handle(BRep_TVertex)& TV1 = *((Handle(BRep_TVertex)*) &(V1).TShape());
    Handle(BRep_TVertex)& TV2 = *((Handle(BRep_TVertex)*) &(V2).TShape());

    TopLoc_Location loc;
    Standard_Real f, l;
    Handle( Geom_Curve ) theCurve = BRep_Tool::Curve(anEdge, loc, f, l );
    theCurve = Handle( Geom_Curve )::DownCast( theCurve->Copy() );
    theCurve->Transform( loc.Transformation() );
    gp_Pnt f3d = theCurve->Value( f );
    gp_Pnt l3d = theCurve->Value( l );

    Standard_Real dist1, dist2;
    dist1 = f3d.Distance( TV1->Pnt() );
    dist2 = l3d.Distance( TV2->Pnt() );
    if (! MapVertex.Contains( V1 ))
    {
      
      TV1->Pnt( f3d );
      MapVertex.Add( V1 );
    }
    else
      TV1->UpdateTolerance( 1.5*dist1 );
    if (! MapVertex.Contains( V2 ))
    {
      TV2->Pnt( l3d );
      MapVertex.Add( V2 );
    }
    else
      TV2->UpdateTolerance( 1.5*dist2 );
  }

  if (!myIsOpenResult)
    FixHoles();

  myIsDone = Standard_True;
}


//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

BRepFill_IndexedDataMapOfOrientedShapeListOfShape& 
  BRepFill_OffsetWire::Generated() 
{
  return myMap;
}


//=======================================================================
//function : PrepareSpine
//purpose  : 
//=======================================================================

void BRepFill_OffsetWire::PrepareSpine()
{
  BRep_Builder      B;
  TopTools_ListOfShape Cuts;
  TopTools_ListIteratorOfListOfShape IteCuts;
  TopoDS_Vertex V1,V2;

  myWorkSpine.Nullify();
  myMapSpine.Clear();

  TopLoc_Location L;
  const Handle(Geom_Surface)& S    = BRep_Tool::Surface  (mySpine,L);
  Standard_Real               TolF = BRep_Tool::Tolerance(mySpine);
  B.MakeFace(myWorkSpine,S,L,TolF);
  
  for (TopoDS_Iterator IteF(mySpine) ; IteF.More(); IteF.Next()) {

    TopoDS_Wire NW;
    B.MakeWire (NW);

    //  Modified by Sergey KHROMOV - Thu Nov 16 17:29:55 2000 Begin
    Standard_Integer ForcedCut = 0;
    Standard_Integer nbResEdges = -1;
    TopTools_IndexedMapOfShape EdgeMap;

    TopExp::MapShapes(IteF.Value(), TopAbs_EDGE, EdgeMap);
    Standard_Integer nbEdges = EdgeMap.Extent();
    
    if (nbEdges == 1 && !myIsOpenResult) //in case of open wire there's no need to do it
      ForcedCut = 2;
    //  Modified by Sergey KHROMOV - Thu Nov 16 17:29:48 2000 End

    for (TopoDS_Iterator IteW(IteF.Value()); IteW.More(); IteW.Next()) {
      
      const TopoDS_Edge& E = TopoDS::Edge(IteW.Value());
      EdgeVertices(E,V1,V2);
      myMapSpine.Bind(V1,V1);
      myMapSpine.Bind(V2,V2);
      Cuts.Clear();

      // Cut
      TopoDS_Shape aLocalShape = E.Oriented(TopAbs_FORWARD);
      //  Modified by Sergey KHROMOV - Thu Nov 16 17:29:29 2000 Begin
      if (nbEdges == 2 && nbResEdges == 0)
        ForcedCut = 1;
      //  Modified by Sergey KHROMOV - Thu Nov 16 17:29:33 2000 End
      nbResEdges = CutEdge (TopoDS::Edge(aLocalShape), mySpine, ForcedCut, Cuts);
      
      if (Cuts.IsEmpty()) {
        B.Add(NW,E);
        myMapSpine.Bind(E,E);
      }
      else {	
        for (IteCuts.Initialize(Cuts); IteCuts.More(); IteCuts.Next()) {
          TopoDS_Edge NE = TopoDS::Edge(IteCuts.Value());
          NE.Orientation(E.Orientation());
          B.Add(NW,NE);
          myMapSpine.Bind(NE,E);
          EdgeVertices(NE,V1,V2);
          if (!myMapSpine.IsBound(V1)) myMapSpine.Bind(V1,E);
          if (!myMapSpine.IsBound(V2)) myMapSpine.Bind(V2,E);
        }
      }
    }
    //  Modified by Sergey KHROMOV - Thu Mar  7 09:17:41 2002 Begin
    TopoDS_Vertex aV1;
    TopoDS_Vertex aV2;

    TopExp::Vertices(NW, aV1, aV2);

    NW.Closed(aV1.IsSame(aV2));

    //  Modified by Sergey KHROMOV - Thu Mar  7 09:17:43 2002 End
    B.Add(myWorkSpine, NW);
  }

#ifdef OCCT_DEBUG
#ifdef DRAW
  if ( AffichEdge) {
    DBRep::Set("WS",myWorkSpine);
    DBRep::Set("MS",mySpine);
    BRepTools::Write(myWorkSpine, "WS");
    BRepTools::Write(mySpine, "MS");
  }
#endif
#endif

}

//=======================================================================
//function : UpdateDetromp
//purpose  : For each interval on bissectrice defined by parameters
//           test if the medium point is at a distance > offset	
//           in this case vertices corresponding to the extremities of the interval
//           are ranked in the proofing.
//           => If the same vertex appears in the proofing, the 
//           border of the zone of proximity is tangent to the offset .
//=======================================================================

void BRepFill_OffsetWire::UpdateDetromp (BRepFill_DataMapOfOrientedShapeListOfShape& Detromp,
                                         const TopoDS_Shape& Shape1,
                                         const TopoDS_Shape& Shape2,
                                         const TopTools_SequenceOfShape& Vertices, 
                                         const TColgp_SequenceOfPnt&     Params, 
                                         const Bisector_Bisec&           Bisec,
                                         const Standard_Boolean          SOnE,
                                         const Standard_Boolean          EOnE,
                                         const BRepFill_TrimEdgeTool&    Trim) const
{
  Standard_Integer ii = 1;

  if (myJoinType == GeomAbs_Intersection)
  {
    for (; ii <= Vertices.Length(); ii++)
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(Vertices.Value(ii));
      Detromp(Shape1).Append(aVertex);
      Detromp(Shape2).Append(aVertex);
    }
  }
  else //myJoinType == GeomAbs_Arc
  {
    Standard_Real    U1,U2;
    TopoDS_Vertex    V1,V2;
    
    const Handle(Geom2d_Curve)& Bis = Bisec.Value();
    Standard_Boolean ForceAdd = Standard_False;
    Handle(Geom2d_TrimmedCurve) aTC = Handle(Geom2d_TrimmedCurve)::DownCast(Bis);
    if(!aTC.IsNull() && aTC->BasisCurve()->IsPeriodic())
    {
      gp_Pnt2d Pf = Bis->Value(Bis->FirstParameter());
      gp_Pnt2d Pl = Bis->Value(Bis->LastParameter());
      ForceAdd = Pf.Distance(Pl) <= Precision::Confusion();
    }

    U1 = Bis->FirstParameter();
    
    if (SOnE) { 
      // the first point of the bissectrice is on the offset
      V1 = TopoDS::Vertex(Vertices.Value(ii));
      ii++; 
    }
    
    while (ii <= Vertices.Length()) {
      U2 = Params.Value(ii).X();
      V2 = TopoDS::Vertex(Vertices.Value(ii));
      
      gp_Pnt2d P = Bis->Value((U2 + U1)*0.5);
      if (!Trim.IsInside(P) || ForceAdd) {
        if (!V1.IsNull()) {
          Detromp(Shape1).Append(V1);
          Detromp(Shape2).Append(V1);
        }
        Detromp(Shape1).Append(V2);
        Detromp(Shape2).Append(V2);
      }
      U1 = U2;
      V1 = V2;
      ii ++;
    }
    
    // test medium point between the last parameter and the end of the bissectrice.
    U2 = Bis->LastParameter();
    if (!EOnE) {
      if (!Precision::IsInfinite(U2)) {
        gp_Pnt2d P = Bis->Value((U2 + U1)*0.5);
        if (!Trim.IsInside(P) || ForceAdd) {
          if (!V1.IsNull()) {
            Detromp(Shape1).Append(V1);
            Detromp(Shape2).Append(V1);
          }
        }
      }
      else {
        if (!V1.IsNull()) {
          Detromp(Shape1).Append(V1);
          Detromp(Shape2).Append(V1);
        }
      }
    }    
    //else if(myJoinType != GeomAbs_Arc)
    //{
    //  if (!V1.IsNull()) {
    //    Detromp(Shape1).Append(V1);
    //    Detromp(Shape2).Append(V1);
    //  }
    //}
  } //end of else (myJoinType==GeomAbs_Arc)
}

//=======================================================================
//function : MakeWires
//purpose  : 
//=======================================================================

void BRepFill_OffsetWire::MakeWires()
{
  //--------------------------------------------------------
  // creation of a single list of created parallel edges.
  //--------------------------------------------------------
  TopTools_SequenceOfShape TheEdges;
  TopTools_ListOfShape     TheWires;
  TopTools_ListIteratorOfListOfShape                          itl;
  //BRepFill_DataMapIteratorOfDataMapOfOrientedShapeListOfShape ite;  
  TopTools_IndexedDataMapOfShapeListOfShape                   MVE;
  //TopTools_DataMapIteratorOfDataMapOfShapeListOfShape         MVEit;
  TopoDS_Vertex V1,V2,VF,CV;
  Standard_Integer i;

  for (i = 1; i <= myMap.Extent(); i++) {    
    for (itl.Initialize(myMap(i)); itl.More(); itl.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(itl.Value());
      TopExp::Vertices (E,V1,V2);
      if (V1.IsSame(V2) && IsSmallClosedEdge(E, V1))
        continue; //remove small closed edges
      if (!CheckSmallParamOnEdge(E))
        continue;
      if (!MVE.Contains(V1)) {
        TopTools_ListOfShape empty;
        MVE.Add(V1,empty);
      }
      MVE.ChangeFromKey(V1).Append(E);
      if (!MVE.Contains(V2)) {
        TopTools_ListOfShape empty;
        MVE.Add(V2,empty);
      }
      MVE.ChangeFromKey(V2).Append(E);
    }
  }

  //--------------------------------------
  // Creation of parallel wires.
  //--------------------------------------
  BRep_Builder B;


  //  Standard_Integer NbEdges;
  //  Standard_Boolean NewWire  = Standard_True;
  //  Standard_Boolean AddEdge  = Standard_False;

  TopoDS_Wire      NW;
  Standard_Boolean End;
  TopoDS_Edge CE;

  while (!MVE.IsEmpty()) {
    B.MakeWire(NW);

    //MVEit.Initialize(MVE);
    for (i = 1; i <= MVE.Extent(); i++)
      if(MVE(i).Extent() == 1)
        break;

    //if(!MVEit.More()) MVEit.Initialize(MVE);
    if (i > MVE.Extent())
      i = 1;
    
    CV  = VF = TopoDS::Vertex(MVE.FindKey(i));
    CE  = TopoDS::Edge(MVE(i).First());
    End = Standard_False;
    MVE.ChangeFromKey(CV).RemoveFirst();

    if (myIsOpenResult && MVE.FindFromKey(CV).IsEmpty())
    {
      MVE.RemoveKey(CV);
    }
      

    //  Modified by Sergey KHROMOV - Thu Mar 14 11:29:59 2002 Begin
    Standard_Boolean isClosed = Standard_False;

    //  Modified by Sergey KHROMOV - Thu Mar 14 11:30:00 2002 End

    while(!End) {      
      //-------------------------------
      // Construction of a wire.
      //-------------------------------
      TopExp::Vertices(CE,V1,V2);
      if (!CV.IsSame(V1)) CV = V1; else CV = V2;

      B.Add (NW,CE);

      if (VF.IsSame(CV) || !MVE.Contains(CV)) {
        //  Modified by Sergey KHROMOV - Thu Mar 14 11:30:14 2002 Begin
        isClosed = VF.IsSame(CV);
        //  Modified by Sergey KHROMOV - Thu Mar 14 11:30:15 2002 End
        End = Standard_True;
        MVE.RemoveKey(VF);
      }

      if (!End) {
        if (MVE.FindFromKey(CV).Extent() > 2) {
          //cout <<"vertex on more that 2 edges in a face."<<endl;
        }
        for ( itl.Initialize(MVE.FindFromKey(CV)); itl.More(); itl.Next()) {
          if (itl.Value().IsSame(CE)) {
            MVE.ChangeFromKey(CV).Remove(itl);
            break;
          }
        }
        if (!MVE.FindFromKey(CV).IsEmpty()) {
          CE = TopoDS::Edge(MVE.FindFromKey(CV).First());
          MVE.ChangeFromKey(CV).RemoveFirst();
        }
        else if (myIsOpenResult)//CV was a vertex with one edge
          End = Standard_True;
        
        if (MVE.FindFromKey(CV).IsEmpty())
        {
          MVE.RemoveKey(CV);
        }
      }
    }
    //  Modified by Sergey KHROMOV - Thu Mar 14 11:29:31 2002 Begin
    //     NW.Closed(Standard_True);
    NW.Closed(isClosed);
    //  Modified by Sergey KHROMOV - Thu Mar 14 11:29:37 2002 End
    TheWires.Append(NW);
  }

  // update myShape :
  //      -- if only one wire : myShape is a Wire
  //      -- if several wires : myShape is a Compound.
  if ( TheWires.Extent() == 1) {
    myShape = TheWires.First();
  }
  else {
    TopoDS_Compound R;
    B.MakeCompound(R);
    TopTools_ListIteratorOfListOfShape it(TheWires);
    for ( ; it.More(); it.Next()) {
      B.Add(R, it.Value());
    }
    myShape = R;
  }
  
}

//=======================================================================
//function : FixHoles
//purpose  : 
//=======================================================================

void BRepFill_OffsetWire::FixHoles()
{
  TopTools_SequenceOfShape ClosedWires, UnclosedWires, IsolatedWires;

  Standard_Real MaxTol = 0.;
  BRep_Builder BB;

  TopExp_Explorer Explo( mySpine, TopAbs_VERTEX );
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Vertex& aVertex = TopoDS::Vertex( Explo.Current() );
    Standard_Real Tol = BRep_Tool::Tolerance(aVertex);
    if (Tol > MaxTol)
      MaxTol = Tol;
  }
  MaxTol *= 100.;

  Explo.Init( myShape, TopAbs_WIRE );
  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Shape aWire = Explo.Current();
    // Remove duplicated edges
    TopTools_DataMapOfShapeListOfShape EEmap;
    TopoDS_Iterator it( aWire );
    for (; it.More(); it.Next())
    {
      const TopoDS_Shape& anEdge = it.Value();
      if (! EEmap.IsBound( anEdge ))
      {
        TopTools_ListOfShape LE;
        EEmap.Bind( anEdge, LE );
      }
      else
        EEmap(anEdge).Append( anEdge );
    }
    aWire.Free( Standard_True );
    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape mapit( EEmap );
    for (; mapit.More(); mapit.Next())
    {
      const TopTools_ListOfShape& LE = mapit.Value();
      TopTools_ListIteratorOfListOfShape itl( LE );
      for (; itl.More(); itl.Next())
        BB.Remove( aWire, itl.Value() );
    }
    // Sorting
    if (aWire.Closed())
      ClosedWires.Append( aWire );
    else
      UnclosedWires.Append( aWire );
  }

  while (!UnclosedWires.IsEmpty())
  {
    TopoDS_Wire& Base = TopoDS::Wire( UnclosedWires(1) );
    TopoDS_Vertex Vf, Vl;
    TopExp::Vertices( Base, Vf, Vl );
    if(Vf.IsNull() || Vl.IsNull())
    {
      throw Standard_Failure("BRepFill_OffsetWire::FixHoles(): Wrong wire.");
    }
    gp_Pnt Pf, Pl;
    Pf = BRep_Tool::Pnt(Vf);
    Pl = BRep_Tool::Pnt(Vl);
    Standard_Real DistF = RealLast(), DistL = RealLast();
    Standard_Integer IndexF = 0, IndexL = 0;
    Standard_Boolean IsFirstF = Standard_False, IsFirstL = Standard_False;
    for (Standard_Integer i = 2; i <= UnclosedWires.Length(); i++)
    {
      TopoDS_Wire aWire = TopoDS::Wire( UnclosedWires(i) );
      TopoDS_Vertex V1, V2;
      TopExp::Vertices( aWire, V1, V2 );

      if(V1.IsNull() || V2.IsNull())
      {
        throw Standard_Failure("BRepFill_OffsetWire::FixHoles(): Wrong wire.");
      }

      gp_Pnt P1, P2;
      P1 = BRep_Tool::Pnt(V1);
      P2 = BRep_Tool::Pnt(V2);
      Standard_Real dist = Pf.Distance( P1 );
      if (dist < DistF)
      {
        DistF = dist;
        IndexF = i;
        IsFirstF = Standard_True;
      }
      dist = Pf.Distance( P2 );
      if (dist < DistF)
      {
        DistF = dist;
        IndexF = i;
        IsFirstF = Standard_False;
      }
      dist = Pl.Distance( P1 );
      if (dist < DistL)
      {
        DistL = dist;
        IndexL = i;
        IsFirstL = Standard_True;
      }
      dist = Pl.Distance( P2 );
      if (dist < DistL)
      {
        DistL = dist;
        IndexL = i;
        IsFirstL = Standard_False;
      }
    }
    if (DistF > MaxTol)
      IndexF = 0;
    if (DistL > MaxTol)
      IndexL = 0;
    TopoDS_Wire theWire;
    TopoDS_Edge theEdge;
    TopoDS_Vertex theVertex;
    Standard_Real CommonTol;
    Standard_Boolean TryToClose = Standard_True;
    if (DistF <= MaxTol && DistL <= MaxTol && IndexF == IndexL && IsFirstF == IsFirstL)
    {
      if (DistF < DistL)
      {
        DistL = RealLast();
        IndexL++;
      }
      else
      {
        DistF = RealLast();
        IndexF++;
      }
      TryToClose = Standard_False;
    }
    if (DistF <= MaxTol)
    {
      theWire = TopoDS::Wire( UnclosedWires(IndexF) );
      TopoDS_Vertex V1, V2;
      TopExp::Vertices( theWire, V1, V2 );
      TopTools_IndexedDataMapOfShapeListOfShape VEmap;
      TopExp::MapShapesAndAncestors( theWire, TopAbs_VERTEX, TopAbs_EDGE, VEmap );
      theEdge = (IsFirstF)? TopoDS::Edge(VEmap.FindFromKey( V1 ).First()) :
        TopoDS::Edge(VEmap.FindFromKey( V2 ).First());
      TopoDS_Iterator it( theWire );
      for (; it.More(); it.Next())
      {
        TopoDS_Edge anEdge = TopoDS::Edge( it.Value() );
        if (IsFirstF) anEdge.Reverse();
        if (!anEdge.IsSame( theEdge ))
          BB.Add( Base, anEdge );
      }
      theVertex = (IsFirstF)? V1 : V2;
      CommonTol = Max( BRep_Tool::Tolerance(Vf), BRep_Tool::Tolerance(theVertex) );
      if (DistF <= CommonTol)
      {
        theEdge.Free( Standard_True );
        Vf.Orientation( theVertex.Orientation() );
        BB.Remove( theEdge, theVertex );
        BB.Add( theEdge, Vf );
        BB.UpdateVertex( Vf, CommonTol );
        if (IsFirstF) theEdge.Reverse();
        BB.Add( Base, theEdge );
      }
      else
      {
        if (IsFirstF) theEdge.Reverse();
        BB.Add( Base, theEdge );
        // Creating new edge from theVertex to Vf
        TopoDS_Edge NewEdge = BRepLib_MakeEdge( theVertex, Vf );
        BB.Add( Base, NewEdge );
      }
    }
    if (DistL <= MaxTol && IndexL != IndexF)
    {
      theWire = TopoDS::Wire( UnclosedWires(IndexL) );
      TopoDS_Vertex V1, V2;
      TopExp::Vertices( theWire, V1, V2 );
      TopTools_IndexedDataMapOfShapeListOfShape VEmap;
      TopExp::MapShapesAndAncestors( theWire, TopAbs_VERTEX, TopAbs_EDGE, VEmap );
      theEdge = (IsFirstL)? TopoDS::Edge(VEmap.FindFromKey( V1 ).First()) :
        TopoDS::Edge(VEmap.FindFromKey( V2 ).First());
      TopoDS_Iterator it( theWire );
      for (; it.More(); it.Next())
      {
        TopoDS_Edge anEdge = TopoDS::Edge( it.Value() );
        if (!IsFirstL) anEdge.Reverse();
        if (!anEdge.IsSame( theEdge ))
          BB.Add( Base, anEdge );
      }
      theVertex = (IsFirstL)? V1 : V2;
      CommonTol = Max( BRep_Tool::Tolerance(Vl), BRep_Tool::Tolerance(theVertex) );
      if (DistL <= CommonTol)
      {
        theEdge.Free( Standard_True );
        Vl.Orientation( theVertex.Orientation() );
        BB.Remove( theEdge, theVertex );
        BB.Add( theEdge, Vl );
        BB.UpdateVertex( Vl, CommonTol );
        if (!IsFirstL) theEdge.Reverse();
        BB.Add( Base, theEdge );
      }
      else
      {
        if (!IsFirstL) theEdge.Reverse();
        BB.Add( Base, theEdge );
        // Creating new edge from Vl to theVertex
        TopoDS_Edge NewEdge = BRepLib_MakeEdge( Vl, theVertex );
        BB.Add( Base, NewEdge );
      }
    }
    // Check if it is possible to close resulting wire
    if (TryToClose)
    {
      TopExp::Vertices( Base, Vf, Vl );
      CommonTol = Max( BRep_Tool::Tolerance(Vf), BRep_Tool::Tolerance(Vl) );
      TopTools_IndexedDataMapOfShapeListOfShape VEmap;
      TopExp::MapShapesAndAncestors( Base, TopAbs_VERTEX, TopAbs_EDGE, VEmap );
      TopoDS_Edge Efirst, Elast;
      Efirst = TopoDS::Edge(VEmap.FindFromKey( Vf ).First());
      Elast  = TopoDS::Edge(VEmap.FindFromKey( Vl ).First());
      Pf = BRep_Tool::Pnt(Vf);
      Pl = BRep_Tool::Pnt(Vl);
      Standard_Real Dist = Pf.Distance(Pl);
      if (Dist <= CommonTol)
      {
        Elast.Free( Standard_True );
        Vf.Orientation( Vl.Orientation() );
        BB.Remove( Elast, Vl );
        BB.Add( Elast, Vf );
        BB.UpdateVertex( Vf, CommonTol );
        Base.Closed( Standard_True );
      }
      else if (Dist <= MaxTol)
      {
        // Creating new edge from Vl to Vf
        TopoDS_Edge NewEdge = BRepLib_MakeEdge( Vf, Vl );
        BB.Add( Base, NewEdge );
        Base.Closed( Standard_True );
      }
    }
    // Updating sequences ClosedWires and UnclosedWires
    if (DistF <= MaxTol)
      UnclosedWires.Remove( IndexF );
    if (DistL <= MaxTol && IndexL != IndexF)
    {
      if (DistF <= MaxTol && IndexL > IndexF)
        IndexL--;
      UnclosedWires.Remove( IndexL );
    }
    if (Base.Closed())
    {
      ClosedWires.Append( Base );
      UnclosedWires.Remove( 1 );
    }
    else if (DistF > MaxTol && DistL > MaxTol)
    {
      IsolatedWires.Append( Base );
      UnclosedWires.Remove( 1 );
    }
  }

  // Updating myShape
  if (ClosedWires.Length() + IsolatedWires.Length() == 1)
  {
    if (!ClosedWires.IsEmpty())
      myShape = ClosedWires.First();
    else
      myShape = IsolatedWires.First();
  }
  else
  {
    TopoDS_Compound R;
    BB.MakeCompound( R );
    Standard_Integer i;
    for (i = 1; i <= ClosedWires.Length(); i++)
      BB.Add( R, ClosedWires(i) );
    for (i = 1; i <= IsolatedWires.Length(); i++)
      BB.Add( R, IsolatedWires(i) );
    myShape = R;
  }
}

//=======================================================================
//function : CutEdge
//purpose  : Cut edge at the extrema of curvatures and points of inflexion.
//           So, closed circles are cut in two.
//           If <Cuts> is empty, the edge is not modified.
//           The first and the last vertex of the initial edge 
//           belong to the first and the last parts respectively.
//=======================================================================
Standard_Integer CutEdge (const TopoDS_Edge& E, 
  const TopoDS_Face& F,
  Standard_Integer ForceCut,
  TopTools_ListOfShape& Cuts)
{
  Cuts.Clear();
  MAT2d_CutCurve              Cuter;
  TColGeom2d_SequenceOfCurve  theCurves;
  Standard_Real               f,l;
  Handle(Geom2d_Curve)        C2d;
  Handle(Geom2d_TrimmedCurve) CT2d;
  //  Modified by Sergey KHROMOV - Wed Mar  6 17:36:25 2002 Begin
  Standard_Real               aTol = BRep_Tool::Tolerance(E);
  Handle(Geom_Curve)          aC;
  //  Modified by Sergey KHROMOV - Wed Mar  6 17:36:25 2002 End
  
  TopoDS_Vertex V1,V2,VF,VL;
  TopExp::Vertices (E,V1,V2);
  BRep_Builder B;
  
  C2d  = BRep_Tool::CurveOnSurface (E,F,f,l);
  //  Modified by Sergey KHROMOV - Wed Mar  6 17:36:54 2002 Begin
  aC   = BRep_Tool::Curve(E,f,l);
  //  Modified by Sergey KHROMOV - Wed Mar  6 17:36:54 2002 End
  CT2d = new Geom2d_TrimmedCurve(C2d,f,l);
  //if (E.Orientation() == TopAbs_REVERSED) CT2d->Reverse();

  if (CT2d->BasisCurve()->IsKind(STANDARD_TYPE(Geom2d_Circle)) &&
    ( Abs(f-l) >= M_PI) ) {
      return 0;
  }

  //-------------------------
  // Cut curve.
  //-------------------------
  Cuter.Perform(CT2d);

  //  Modified by Sergey KHROMOV - Thu Nov 16 17:28:29 2000 Begin
  if (ForceCut == 0) {
    if (Cuter.UnModified()) {
      //-----------------------------
      // edge not modified => return.
      //-----------------------------
      return 0;
    } else {
      for (Standard_Integer k = 1; k <= Cuter.NbCurves(); k++)
        theCurves.Append(Cuter.Value(k));
    }
  } else if (ForceCut == 1) {
    if (Cuter.UnModified()) {
      CutCurve (CT2d, 2, theCurves);
    } else {
      for (Standard_Integer k = 1; k <= Cuter.NbCurves(); k++)

       theCurves.Append(Cuter.Value(k));
    }
  } else if (ForceCut == 2) {
    if (Cuter.UnModified()) {
      CutCurve (CT2d, 3, theCurves);
    } else {
      if (Cuter.NbCurves() == 2) {

        Handle(Geom2d_TrimmedCurve)CC = Cuter.Value(1);

        if (CC->LastParameter() > (l+f)/2.) {
          CutCurve (CC, 2, theCurves);
          theCurves.Append(Cuter.Value(2));
        } else {
          theCurves.Append(CC);
          CutCurve (Cuter.Value(2), 2, theCurves);
        }
      } else {
        for (Standard_Integer k = 1; k <= Cuter.NbCurves(); k++)
          theCurves.Append(Cuter.Value(k));
      }
    }
  }

  //  Modified by Sergey KHROMOV - Thu Nov 16 17:28:37 2000 End

  //--------------------------------------
  // Creation of cut edges.
  //--------------------------------------
  VF = V1;

  for (Standard_Integer k = 1; k <= theCurves.Length(); k++) {

    Handle(Geom2d_TrimmedCurve)CC = Handle(Geom2d_TrimmedCurve)::DownCast(theCurves.Value(k));

    if (k == theCurves.Length()) {VL = V2;}
    else { 
      //  Modified by Sergey KHROMOV - Wed Mar  6 17:38:02 2002 Begin
      gp_Pnt        P = aC->Value(CC->LastParameter());

      VL = BRepLib_MakeVertex(P);
      B.UpdateVertex(VL, aTol);
      //  Modified by Sergey KHROMOV - Wed Mar  6 17:38:05 2002 End
    }
    TopoDS_Shape aLocalShape = E.EmptyCopied();
    TopoDS_Edge NE = TopoDS::Edge(aLocalShape);
    //      TopoDS_Edge NE = TopoDS::Edge(E.EmptyCopied());
    NE.Orientation(TopAbs_FORWARD);
    B.Add  (NE,VF.Oriented(TopAbs_FORWARD));
    B.Add  (NE,VL.Oriented(TopAbs_REVERSED));      
    B.Range(NE,CC->FirstParameter(),CC->LastParameter());
    Cuts.Append(NE.Oriented(E.Orientation()));
    VF = VL;
  }

  return theCurves.Length();
}

//  Modified by Sergey KHROMOV - Thu Nov 16 17:27:56 2000 Begin
//=======================================================================
//function : CutCurve
//purpose  : 
//=======================================================================

void CutCurve (const Handle(Geom2d_TrimmedCurve)& C,
  const Standard_Integer nbParts,
  TColGeom2d_SequenceOfCurve& theCurves)
{
  Handle(Geom2d_TrimmedCurve) TrimC;
  Standard_Real               UF,UL,UC;
  Standard_Real               Step;
  gp_Pnt2d                    PF,PL,PC;
  Standard_Real               PTol  = Precision::PConfusion()*10;
  Standard_Real               Tol   = Precision::Confusion()*10;
  Standard_Boolean            YaCut = Standard_False;

  UF = C->FirstParameter();
  UL = C->LastParameter ();
  PF = C->Value(UF);
  PL = C->Value(UL);

  Step = (UL - UF)/nbParts;

  for (Standard_Integer i = 1; i < nbParts; i++) {

    UC = UF + i*Step;
    PC = C->Value(UC);

    if (UC - UF > PTol && PC.Distance(PF) > Tol) {
      if ( UL - UC < PTol || PL.Distance(PC) < Tol)
        continue;

      TrimC = new Geom2d_TrimmedCurve(C,UF,UC);
      theCurves.Append(TrimC);
      UF = UC;
      PF = PC;
      YaCut = Standard_True;
    }
  }
  if (YaCut) {
    TrimC = new Geom2d_TrimmedCurve(C,UF,UL);
    theCurves.Append(TrimC);
  } else
    theCurves.Append(C);
}
//  Modified by Sergey KHROMOV - Thu Nov 16 17:28:13 2000 End

//=======================================================================
//function : MakeCircle
//purpose  : 
//=======================================================================

void MakeCircle (const TopoDS_Edge&          E,
  const TopoDS_Vertex&        V,
  const TopoDS_Face&          F,
  const Standard_Real         Offset, 
  BRepFill_IndexedDataMapOfOrientedShapeListOfShape& Map,
  const Handle(Geom_Plane)&   RefPlane)
{
  // evaluate the Axis of the Circle.
  Standard_Real f,l;
  Handle(Geom2d_Curve) GC = BRep_Tool::CurveOnSurface(E,F,f,l);
  gp_Vec2d DX;
  gp_Pnt2d P;

  if (E.Orientation() == TopAbs_FORWARD) {
    GC->D1(l,P,DX);
    DX.Reverse();
  }
  else GC->D1(f,P,DX);

  gp_Ax2d Axis(P,gp_Dir2d(DX));
  Handle(Geom2d_Circle) Circ 
    = new  Geom2d_Circle(Axis, Abs(Offset), Offset < 0.);

  // Bind the edges in my Map.
  TopoDS_Edge OE = BRepLib_MakeEdge(Circ, RefPlane);
  TopTools_ListOfShape LL;

  LL.Append(OE);
  Map.Add(V,LL);

#ifdef OCCT_DEBUG
#ifdef DRAW
  if ( AffichGeom && !OE.IsNull()) {
    char name[256];
    sprintf(name,"OFFSET_%d",++NbOFFSET);
    DBRep::Set(name,OE);
  }
#endif
#endif
}

//=======================================================================
//function : MakeOffset
//purpose  : 
//=======================================================================

void MakeOffset (const TopoDS_Edge&        E, 
  const TopoDS_Face&        F,
  const Standard_Real       Offset, 
  BRepFill_IndexedDataMapOfOrientedShapeListOfShape& Map,
  const Handle(Geom_Plane)& RefPlane,
  const Standard_Boolean    IsOpenResult,
                 const GeomAbs_JoinType    theJoinType,
  const TopoDS_Vertex *     Ends)
{
  Standard_Real f,l;
  Standard_Real anOffset = Offset;

  if (E.Orientation() == TopAbs_REVERSED) anOffset *= -1;

  Handle(Geom2d_Curve) G2d = BRep_Tool::CurveOnSurface(E,F,f,l);
  Handle(Geom2d_Curve) G2dOC;

  Standard_Boolean ToExtendFirstPar = Standard_True;
  Standard_Boolean ToExtendLastPar  = Standard_True;
  if (IsOpenResult)
  {
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(E, V1, V2);
    if (V1.IsSame(Ends[0]) ||
      V1.IsSame(Ends[1]))
      ToExtendFirstPar = Standard_False;
    if (V2.IsSame(Ends[0]) ||
      V2.IsSame(Ends[1]))
      ToExtendLastPar  = Standard_False;
  }

  Geom2dAdaptor_Curve  AC(G2d,f,l);
  if ( AC.GetType() == GeomAbs_Circle) {
    // if the offset is greater otr equal to the radius and the side of the  
    // concavity of the circle => edge null.
    gp_Circ2d C1(AC.Circle());
    gp_Ax22d axes( C1.Axis());
    gp_Dir2d Xd = axes.XDirection();
    gp_Dir2d Yd = axes.YDirection();
    Standard_Real Crossed = Xd.X()*Yd.Y()-Xd.Y()*Yd.X();
    Standard_Real Signe = ( Crossed > 0.) ? 1. : -1.;

    if (anOffset*Signe < AC.Circle().Radius() - Precision::Confusion()) {

      Handle(Geom2dAdaptor_HCurve) AHC = 
        new Geom2dAdaptor_HCurve(G2d);
      Adaptor2d_OffsetCurve   Off(AHC,-anOffset);
      Handle(Geom2d_Circle) CC = new Geom2d_Circle(Off.Circle());      

      Standard_Real Delta = 2*M_PI - l + f;
      if (theJoinType == GeomAbs_Arc)
      {
      if (ToExtendFirstPar)
        f -= 0.2*Delta;
      if (ToExtendLastPar)
        l += 0.2*Delta;
      }
      else //GeomAbs_Intersection
      {
        if (ToExtendFirstPar && ToExtendLastPar)
        {
          Standard_Real old_l = l;
          f = old_l + Delta/2.;
          l = f + 2*M_PI;
        }
        else if (ToExtendFirstPar)
        {
          f = l;
          l = f + 2*M_PI;
        }
        else if (ToExtendLastPar)
        {
          l = f + 2*M_PI;
        }
      }
      G2dOC = new Geom2d_TrimmedCurve(CC,f,l);
    }
  }
  else if (AC.GetType() == GeomAbs_Line) {
    Handle(Geom2dAdaptor_HCurve) AHC = 
      new Geom2dAdaptor_HCurve(G2d);
    Adaptor2d_OffsetCurve Off(AHC,anOffset);
    Handle(Geom2d_Line)       CC = new Geom2d_Line(Off.Line());
    Standard_Real Delta = (l - f);
    if (ToExtendFirstPar)
    {
      if (theJoinType == GeomAbs_Arc)
      f -= Delta;
      else //GeomAbs_Intersection
        f = -Precision::Infinite();
    }
    if (ToExtendLastPar)
    {
      if (theJoinType == GeomAbs_Arc)
      l += Delta;
      else //GeomAbs_Intersection
        l = Precision::Infinite();
    }
    G2dOC = new Geom2d_TrimmedCurve(CC,f,l);
  }
  else {

    anOffset = -anOffset;
    Handle(Geom2d_TrimmedCurve) G2dT = new Geom2d_TrimmedCurve(G2d,f,l);
    G2dOC = new Geom2d_OffsetCurve( G2dT, anOffset);

  }

  // Bind the edges in my Map.
  if (!G2dOC.IsNull()) {
    TopoDS_Edge OE = BRepLib_MakeEdge(G2dOC, RefPlane);
    OE.Orientation(E.Orientation());
    TopTools_ListOfShape LL;
    LL.Append(OE);
    Map.Add(E,LL);

#ifdef OCCT_DEBUG
#ifdef DRAW  
    if (AffichGeom && !OE.IsNull()) {
      char name[256];
      sprintf(name,"OFFSET_%d",++NbOFFSET);
      DBRep::Set(name,OE);
      //Standard_Real ii = 0;
    }
#endif
#endif

  }
}  


//=======================================================================
//function : VertexFromNode
//purpose  : 
//=======================================================================

Standard_Boolean VertexFromNode (const Handle(MAT_Node)&      aNode, 
  const Standard_Real          Offset,
  gp_Pnt2d&                   PN,
  BRepFill_DataMapOfNodeShape& MapNodeVertex,
  TopoDS_Vertex&               VN)
{  
  Standard_Boolean Status;
  Standard_Real    Tol = Precision::Confusion();
  BRep_Builder     B;

  if (!aNode->Infinite() && Abs(aNode->Distance()-Offset) < Tol) {
    //------------------------------------------------
    // the Node gives a vertex on the offset
    //------------------------------------------------
    if (MapNodeVertex.IsBound(aNode)) {
      VN = TopoDS::Vertex(MapNodeVertex(aNode));
    }
    else { 
      gp_Pnt P(PN.X(),PN.Y(),0.);
      B.MakeVertex (VN);
      B.UpdateVertex(VN,P, Precision::Confusion());
      MapNodeVertex.Bind(aNode,VN);
    }
    Status = Standard_True;
  }
  else Status = Standard_False;

  return Status;
}


//=======================================================================
//function : StoreInMap
//purpose  : 
//=======================================================================

void StoreInMap (const TopoDS_Shape& V1,
  const TopoDS_Shape& V2,
  TopTools_IndexedDataMapOfShapeShape& MapVV)
{
  TopoDS_Shape OldV = V1, NewV = V2;
  Standard_Integer i;

  if (MapVV.Contains(V2))
    NewV = MapVV.FindFromKey(V2);

  if (MapVV.Contains(V1))
    MapVV.ChangeFromKey(V1) = NewV;

  for (i = 1; i <= MapVV.Extent(); i++)
    if (MapVV(i).IsSame(V1))
      MapVV(i) = NewV;

  MapVV.Add(V1, NewV);
}

//=======================================================================
//function : TrimEdge
//purpose  : 
//=======================================================================

void TrimEdge (const TopoDS_Edge&              E,
               const TopoDS_Shape&             ProE,
               const TopoDS_Face&              AllSpine,
	       const TopTools_ListOfShape&     Detromp,
               TopTools_SequenceOfShape& TheVer,
               TColStd_SequenceOfReal&   ThePar,
               TopTools_SequenceOfShape& S,
               TopTools_IndexedDataMapOfShapeShape& MapVV,
               const Standard_Integer IndOfE)
{
  Standard_Boolean         Change = Standard_True;
  BRep_Builder             TheBuilder;
  S.Clear();

  //-----------------------------------------------------------
  // Parse two sequences depending on the parameter on the edge.
  //-----------------------------------------------------------
  while (Change) {
    Change = Standard_False;
    for (Standard_Integer i = 1; i < ThePar.Length(); i++) {
      if (ThePar.Value(i) > ThePar.Value(i+1)) {
        ThePar.Exchange(i,i+1);
        TheVer.Exchange(i,i+1);
        Change = Standard_True;
      }
    }
  }

  //----------------------------------------------------------
  // If a vertex is not in the proofing, it is eliminated.
  //----------------------------------------------------------
  if (!BRep_Tool::Degenerated(E)) {
    for (Standard_Integer k = 1; k <= TheVer.Length(); k ++) {
      if ( DoubleOrNotInside (Detromp,
        TopoDS::Vertex(TheVer.Value(k)))) {
          TheVer.Remove(k);
          ThePar.Remove(k);
          k--;
      }
    }
  }

  //----------------------------------------------------------
  // If a vertex_double appears twice in the proofing 
  // the vertex is removed.
  // otherwise preserve only one of its representations.
  //----------------------------------------------------------
  if (!BRep_Tool::Degenerated(E)) {
    Standard_Real aParTol = 2.0 * Precision::PConfusion();
    for (Standard_Integer k = 1; k < TheVer.Length(); k ++) {
      if (TheVer.Value(k).IsSame(TheVer.Value(k+1)) || 
          Abs(ThePar.Value(k)-ThePar.Value(k+1)) <= aParTol) {

          if(k+1 == TheVer.Length()) {
            StoreInMap(TheVer(k), TheVer(k+1), MapVV);
            TheVer.Remove(k);
            ThePar.Remove(k);
          }
          else {
            StoreInMap(TheVer(k+1), TheVer(k), MapVV);
            TheVer.Remove(k+1);
            ThePar.Remove(k+1);
          }
          /*
          if ( DoubleOrNotInside (Detromp,
          TopoDS::Vertex(TheVer.Value(k)))) {
          TheVer.Remove(k);
          ThePar.Remove(k);
          k--;
          }
          */
          k--;
      }
    }
  }
  //-----------------------------------------------------------
  // Creation of edges.
  // the number of vertices should be even. The created edges  
  // go from a vertex with uneven index i to vertex i+1;
  //-----------------------------------------------------------
  if (IndOfE == 1 || IndOfE == -1) //open result and extreme edges of result
  {
    TopoDS_Shape aLocalShape = E.EmptyCopied();
    TopoDS_Edge NewEdge = TopoDS::Edge(aLocalShape);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(E, V1, V2);
    Standard_Real fpar, lpar;
    Handle(Geom_Surface) aPlane;
    TopLoc_Location aLoc;
    Handle(Geom2d_Curve) PCurve;
    BRep_Tool::CurveOnSurface(E, PCurve, aPlane, aLoc, fpar, lpar);
    //BRep_Tool::Range(E, fpar, lpar);

    Standard_Real TrPar1, TrPar2;
    Standard_Boolean ToTrimAsOrigin = IsInnerEdge(ProE, AllSpine, TrPar1, TrPar2);
    
    if (IndOfE == 1) //first edge of open wire
    {
      if (NewEdge.Orientation() == TopAbs_FORWARD)
      {
        if (ToTrimAsOrigin)
        {
          fpar = TrPar1;
          gp_Pnt2d TrPnt2d = PCurve->Value(fpar);
          gp_Pnt TrPnt(TrPnt2d.X(), TrPnt2d.Y(), 0.);
          V1 = BRepLib_MakeVertex(TrPnt);
        }
        TheBuilder.Add(NewEdge, V1.Oriented(TopAbs_FORWARD));
        TheBuilder.Add(NewEdge, TheVer.First().Oriented(TopAbs_REVERSED));
        TheBuilder.Range(NewEdge, fpar, ThePar.First());
      }
      else
      {
        if (ToTrimAsOrigin)
        {
          lpar = TrPar2;
          gp_Pnt2d TrPnt2d = PCurve->Value(lpar);
          gp_Pnt TrPnt(TrPnt2d.X(), TrPnt2d.Y(), 0.);
          V2 = BRepLib_MakeVertex(TrPnt);
        }
        TheBuilder.Add(NewEdge, TheVer.First().Oriented(TopAbs_REVERSED));
        TheBuilder.Add(NewEdge, V2.Oriented(TopAbs_FORWARD));
        TheBuilder.Range(NewEdge, ThePar.First(), lpar);
      }
    }
    else //last edge of open wire
    {
      if (NewEdge.Orientation() == TopAbs_FORWARD)
      {
        if (ToTrimAsOrigin)
        {
          lpar = TrPar2;
          gp_Pnt2d TrPnt2d = PCurve->Value(lpar);
          gp_Pnt TrPnt(TrPnt2d.X(), TrPnt2d.Y(), 0.);
          V2 = BRepLib_MakeVertex(TrPnt);
        }
        TheBuilder.Add(NewEdge, TheVer.First().Oriented(TopAbs_FORWARD));
        TheBuilder.Add(NewEdge, V2.Oriented(TopAbs_REVERSED));
        TheBuilder.Range(NewEdge, ThePar.First(), lpar);
      }
      else
      {
        if (ToTrimAsOrigin)
        {
          fpar = TrPar1;
          gp_Pnt2d TrPnt2d = PCurve->Value(fpar);
          gp_Pnt TrPnt(TrPnt2d.X(), TrPnt2d.Y(), 0.);
          V1 = BRepLib_MakeVertex(TrPnt);
        }
        TheBuilder.Add(NewEdge, V1.Oriented(TopAbs_REVERSED));
        TheBuilder.Add(NewEdge, TheVer.First().Oriented(TopAbs_FORWARD));
        TheBuilder.Range(NewEdge, fpar, ThePar.First());
      }
    }
    S.Append(NewEdge);
  }
  else
  {
    for (Standard_Integer k = 1; k < TheVer.Length(); k = k+2) {
      TopoDS_Shape aLocalShape = E.EmptyCopied();
      TopoDS_Edge NewEdge = TopoDS::Edge(aLocalShape);
      //    TopoDS_Edge NewEdge = TopoDS::Edge(E.EmptyCopied());
      
      if (NewEdge.Orientation() == TopAbs_REVERSED) {
        TheBuilder.Add  (NewEdge,TheVer.Value(k)  .Oriented(TopAbs_REVERSED));
        TheBuilder.Add  (NewEdge,TheVer.Value(k+1).Oriented(TopAbs_FORWARD));
      }
      else {      
        TheBuilder.Add  (NewEdge,TheVer.Value(k)  .Oriented(TopAbs_FORWARD));
        TheBuilder.Add  (NewEdge,TheVer.Value(k+1).Oriented(TopAbs_REVERSED));
      }
      
      
      TheBuilder.Range(NewEdge,ThePar.Value(k),ThePar.Value(k+1));
      
#ifdef OCCT_DEBUG
#ifdef DRAW
      if ( AffichEdge) {
        char name[256];
        sprintf(name,"TRIMEDGE_%d",NbTRIMEDGES);
        DBRep::Set(name,NewEdge);  
      }
      if (Affich2d) {
        TopLoc_Location L;
        Standard_Real f,l;
        Handle(Geom_Surface) Surf;  
        Handle(Geom2d_Curve) C;
        BRep_Tool::CurveOnSurface(NewEdge,C,Surf,L,f,l);
        char name[256];
        sprintf(name,"OFFSET2d_%d",NbTRIMEDGES++);
        Handle(Geom2d_TrimmedCurve) C2d = new Geom2d_TrimmedCurve(C,f,l);
        Handle(DrawTrSurf_Curve2d) dr =
          new DrawTrSurf_Curve2d(C2d,Standard_False);
        dr->SetColor(Draw_bleu);
        Draw::Set(name,dr);
      }
#endif
#endif
      S.Append(NewEdge);
    }
  }
}

//=======================================================================
//function : IsInnerEdge
//purpose  :
//=======================================================================

static Standard_Boolean IsInnerEdge(const TopoDS_Shape& ProE,
                                    const TopoDS_Face&  AllSpine,
                                    Standard_Real& TrPar1,
                                    Standard_Real& TrPar2)
{
  if (ProE.ShapeType() != TopAbs_EDGE)
    return Standard_False;

  TopoDS_Edge anEdge = TopoDS::Edge(ProE);

  TopTools_IndexedDataMapOfShapeListOfShape VEmap;
  TopExp::MapShapesAndAncestors(AllSpine, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
  for (Standard_Integer i = 1; i <= VEmap.Extent(); i++)
  {
    const TopTools_ListOfShape& LE = VEmap(i);
    if (LE.Extent() == 1 && anEdge.IsSame(LE.First()))
      return Standard_False;
  }

  BRep_Tool::Range(anEdge, TrPar1, TrPar2);
  return Standard_True;
}



//=======================================================================
//function : DoubleOrNotInside
//purpose  : return True if V appears twice in LV or is not inside.
//=======================================================================

Standard_Boolean DoubleOrNotInside (const TopTools_ListOfShape& LV,
  const TopoDS_Vertex&        V)
{  
  Standard_Boolean Vu = Standard_False;
  TopTools_ListIteratorOfListOfShape it(LV);

  for ( ; it.More(); it.Next()) {
    if (V.IsSame(it.Value())) {
      if  (Vu) return Standard_True;
      else       Vu = Standard_True;
    }
  }
  if (Vu) return Standard_False;
  else    return Standard_True;   
}

Standard_Boolean IsSmallClosedEdge(const TopoDS_Edge& anEdge,
  const TopoDS_Vertex& aVertex)
{
  gp_Pnt PV = BRep_Tool::Pnt(aVertex);
  gp_Pnt2d PV2d, Pfirst, Plast, Pmid;
  PV2d.SetCoord( PV.X(), PV.Y() );

  Handle(Geom2d_Curve) PCurve;
  Handle( BRep_CurveRepresentation ) CurveRep =
    ((Handle(BRep_TEdge)::DownCast(anEdge.TShape()))->Curves()).First();
  PCurve = CurveRep->PCurve();

  Standard_Real fpar = (Handle(BRep_GCurve)::DownCast(CurveRep))->First();
  Standard_Real lpar = (Handle(BRep_GCurve)::DownCast(CurveRep))->Last();
  Pfirst = PCurve->Value(fpar);
  Plast  = PCurve->Value(lpar);
  Pmid   = PCurve->Value((fpar + lpar)*0.5);

  Standard_Real theTol = BRep_Tool::Tolerance(aVertex);
  theTol *= 1.5;

  Standard_Real dist1 = Pfirst.Distance(PV2d);
  Standard_Real dist2 = Plast.Distance(PV2d);
  Standard_Real dist3 = Pmid.Distance(PV2d);

  if (dist1 <= theTol && dist2 <= theTol && dist3 <= theTol)
    return Standard_True;

  return Standard_False;
}

static void CheckBadEdges(const TopoDS_Face& Spine, const Standard_Real Offset,
  const BRepMAT2d_BisectingLocus& Locus, 
  const BRepMAT2d_LinkTopoBilo&   Link,
  TopTools_ListOfShape& BadEdges)
{

  TopoDS_Face F = TopoDS::Face(Spine.Oriented(TopAbs_FORWARD));
  Standard_Real eps = Precision::Confusion(); 
  Standard_Real LimCurv = 1./Offset;

  TopTools_MapOfShape aMap;

  for (Standard_Integer ic = 1; ic <= Locus.NumberOfContours(); ic++) {
    for (Standard_Integer ie = 1; ie <= Locus.NumberOfElts(ic); ie++) {
      const TopoDS_Shape& SE = Link.GeneratingShape(Locus.BasicElt(ic,ie));
      if (SE.ShapeType() == TopAbs_EDGE) {


        if (aMap.Contains(SE)) {
          //cout << "Edge is treated second time" << endl;
          continue;
        }


        TopoDS_Edge E = TopoDS::Edge(SE);


        Standard_Real f,l;


        Handle(Geom2d_Curve) G2d = BRep_Tool::CurveOnSurface(E,F,f,l);


        Geom2dAdaptor_Curve  AC(G2d,f,l);
        GeomAbs_CurveType aCType = AC.GetType();


        if(aCType != GeomAbs_Line && aCType != GeomAbs_Circle) {


          Standard_Boolean reverse = Standard_False;
          if (E.Orientation() == TopAbs_FORWARD) reverse = Standard_True;


          gp_Pnt2d P, Pc;
          gp_Dir2d N;


          Geom2dLProp_CLProps2d aCLProps(G2d, 2, eps);


          aCLProps.SetParameter(f);
          if(!aCLProps.IsTangentDefined()) {
            BadEdges.Append(SE);
            aMap.Add(SE);
            continue;
          }


          P = aCLProps.Value();
          Standard_Real Crv = aCLProps.Curvature();


          if(Crv >= eps) {
            aCLProps.Tangent(N);
            Standard_Real x = N.Y(), y = -N.X();
            N.SetCoord(x, y);
            if (reverse) N.Reverse();
            aCLProps.CentreOfCurvature(Pc);
            gp_Vec2d Dir( P, Pc );
            if (N.Dot(Dir) > 0.) {
              if (LimCurv <= Crv + eps) {
                BadEdges.Append(SE);
                aMap.Add(SE);
                continue;
              }
            }
          }  


          aCLProps.SetParameter(l);
          if(!aCLProps.IsTangentDefined()) {
            BadEdges.Append(SE);
            aMap.Add(SE);
            continue;
          }


          P = aCLProps.Value();
          Crv = aCLProps.Curvature();


          if(Crv >= eps) {
            aCLProps.Tangent(N);
            Standard_Real x = N.Y(), y = -N.X();
            N.SetCoord(x, y);
            if (reverse) N.Reverse();
            aCLProps.CentreOfCurvature(Pc);
            gp_Vec2d Dir( P, Pc );
            if (N.Dot(Dir) > 0.) {
              if (LimCurv <= Crv + eps) {
                BadEdges.Append(SE);
                aMap.Add(SE);
                continue;
              }
            }
          }  
        }
      }
    }
  }
}


//=======================================================================
//function : PerformCurve
//purpose  : 
//=======================================================================

static Standard_Boolean PerformCurve (TColStd_SequenceOfReal& Parameters,

  TColgp_SequenceOfPnt&   Points,
  const Adaptor3d_Curve& C, 
  const Standard_Real Deflection,
  const Standard_Real U1,
  const Standard_Real U2,
  const Standard_Real EPSILON,
  const Standard_Integer Nbmin)
{
  Standard_Real UU1 = Min(U1, U2);
  Standard_Real UU2 = Max(U1, U2);

  gp_Pnt Pdeb, Pfin;
  gp_Vec Ddeb,Dfin;
  C.D1(UU1,Pdeb,Ddeb);
  Parameters.Append(UU1);
  Points.Append(Pdeb);

  C.D1(UU2,Pfin,Dfin);

  const Standard_Real aDelta = UU2 - UU1;
  const Standard_Real aDist = Pdeb.Distance(Pfin);

  if((aDelta/aDist) > 5.0e-14)
  {
    QuasiFleche(C,Deflection*Deflection,

      UU1,Pdeb,
      Ddeb,
      UU2,Pfin,
      Dfin,
      Nbmin,
      EPSILON*EPSILON,
      Parameters,Points);
  }

  return Standard_True;
}
//=======================================================================
//function : QuasiFleche
//purpose  : 
//=======================================================================

static void QuasiFleche(const Adaptor3d_Curve& C,

  const Standard_Real Deflection2, 
  const Standard_Real Udeb,
  const gp_Pnt& Pdeb,
  const gp_Vec& Vdeb,
  const Standard_Real Ufin,
  const gp_Pnt& Pfin,
  const gp_Vec& Vfin,
  const Standard_Integer Nbmin,
  const Standard_Real Eps,
  TColStd_SequenceOfReal& Parameters,
  TColgp_SequenceOfPnt& Points)
{
  Standard_Integer Ptslength = Points.Length();
  Standard_Real Udelta = Ufin-Udeb;
  gp_Pnt Pdelta;
  gp_Vec Vdelta;
  if (Nbmin > 2) {
    Udelta /=(Nbmin-1);
    C.D1(Udeb+Udelta,Pdelta,Vdelta);
  }
  else {
    Pdelta = Pfin;
    Vdelta = Vfin;
  }


  Standard_Real Norme = gp_Vec(Pdeb,Pdelta).SquareMagnitude();
  Standard_Real theFleche=0;
  Standard_Boolean flecheok = Standard_False;
  if (Norme > Eps) { 
    // Evaluation of the arrow by interpolation. See IntWalk_IWalking_5.gxx
    Standard_Real N1 = Vdeb.SquareMagnitude();
    Standard_Real N2 = Vdelta.SquareMagnitude();
    if (N1 > Eps && N2 > Eps) {
      Standard_Real Normediff = 

        (Vdeb.Normalized().XYZ()-Vdelta.Normalized().XYZ()).SquareModulus();
      if (Normediff > Eps) {

        theFleche = Normediff*Norme/64.;
        flecheok = Standard_True;
      }
    }
  }
  if (!flecheok) {
    gp_Pnt Pmid((Pdeb.XYZ()+Pdelta.XYZ())/2.);
    gp_Pnt Pverif(C.Value(Udeb+Udelta/2.));
    theFleche = Pmid.SquareDistance(Pverif);
  }

  if (theFleche < Deflection2) {
    Parameters.Append(Udeb+Udelta);
    Points.Append(Pdelta);
  }
  else {
    QuasiFleche(C,Deflection2,Udeb,Pdeb,

      Vdeb,
      Udeb+Udelta,Pdelta,
      Vdelta,
      3,
      Eps,
      Parameters,Points);

  }

  if (Nbmin > 2) {
    QuasiFleche(C,Deflection2,Udeb+Udelta,Pdelta,

      Vdelta,
      Ufin,Pfin,
      Vfin,
      Nbmin-(Points.Length()-Ptslength),
      Eps,
      Parameters,Points);
  }
}

Standard_Boolean CheckSmallParamOnEdge(const TopoDS_Edge& anEdge)
{  
  const BRep_ListOfCurveRepresentation& aList = ((Handle(BRep_TEdge)::DownCast(anEdge.TShape()))->Curves());
  if (!aList.IsEmpty())
  {
    Handle( BRep_CurveRepresentation ) CRep = ((Handle(BRep_TEdge)::DownCast(anEdge.TShape()))->Curves()).First();
    Standard_Real f = (Handle(BRep_GCurve)::DownCast(CRep))->First();
    Standard_Real l = (Handle(BRep_GCurve)::DownCast(CRep))->Last();
    if (Abs (l - f) < Precision::PConfusion())
      return Standard_False;
  }
  return Standard_True;
}


