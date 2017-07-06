// Created on: 1996-01-11
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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


#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepTools.hxx>
#include <Extrema_ExtCC.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <LocOpe.hxx>
#include <LocOpe_WiresOnShape.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <Extrema_ExtCC2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <ShapeAnalysis_Edge.hxx>

#include <TopTools_SequenceOfShape.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_SeqOfBox.hxx>
#include <NCollection_Handle.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <Extrema_ExtPS.hxx>

#include <BRepTopAdaptor_FClass2d.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>
#include <ShapeAnalysis.hxx>

IMPLEMENT_STANDARD_RTTIEXT(LocOpe_WiresOnShape,Standard_Transient)

static Standard_Boolean Project(const TopoDS_Vertex&,
                                const gp_Pnt2d&,
				const TopoDS_Face&,
				TopoDS_Edge&,
				Standard_Real&);

static Standard_Real Project(const TopoDS_Vertex&,
			     const TopoDS_Edge&);

static Standard_Real Project(const TopoDS_Vertex&,
                             const gp_Pnt2d&,
			     const TopoDS_Edge&,
                             const TopoDS_Face&);


static void PutPCurve(const TopoDS_Edge&,
		      const TopoDS_Face&);


static void PutPCurves(const TopoDS_Edge&,
		       const TopoDS_Edge&,
		       const TopoDS_Shape&);

static void FindInternalIntersections(const TopoDS_Edge&,
                                      const TopoDS_Face&,
                                      TopTools_IndexedDataMapOfShapeListOfShape&,
                                      Standard_Boolean&);

//=======================================================================
//function : LocOpe_WiresOnShape
//purpose  : 
//=======================================================================

LocOpe_WiresOnShape::LocOpe_WiresOnShape(const TopoDS_Shape& S):
  myShape(S),myCheckInterior(Standard_True),myDone(Standard_False)
{}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void LocOpe_WiresOnShape::Init(const TopoDS_Shape& S)
{
   myShape = S;
   myCheckInterior = Standard_True;
   myDone = Standard_False;
   myMap.Clear();
   myMapEF.Clear();
}

//=======================================================================
//function : Bind
//purpose  : 
//=======================================================================

void LocOpe_WiresOnShape::Bind(const TopoDS_Wire& W,
			       const TopoDS_Face& F)
{
  for (TopExp_Explorer exp(W, TopAbs_EDGE); exp.More(); exp.Next()) {
    Bind(TopoDS::Edge(exp.Current()),F);
  }
}

//=======================================================================
//function : Bind
//purpose  : 
//=======================================================================

void LocOpe_WiresOnShape::Bind(const TopoDS_Compound& Comp,
			       const TopoDS_Face& F)
{
  for (TopExp_Explorer exp(Comp, TopAbs_EDGE); exp.More(); exp.Next()) {
    Bind(TopoDS::Edge(exp.Current()),F);
  }
  myFacesWithSection.Add(F);
}

//=======================================================================
//function : Bind
//purpose  : 
//=======================================================================

void LocOpe_WiresOnShape::Bind(const TopoDS_Edge& E,
			       const TopoDS_Face& F)
{
//  if (!myMapEF.IsBound(E)) {
  if (!myMapEF.Contains(E)) {
//    for (TopExp_Explorer exp(F,TopAbs_EDGE);exp.More();exp.Next()) {
    TopExp_Explorer exp(F,TopAbs_EDGE) ;
    for ( ;exp.More();exp.Next()) {
      if (exp.Current().IsSame(E)) {
	break;
      }
    }
    if (!exp.More()) {
//      myMapEF.Bind(E,F);
      myMapEF.Add(E,F);
    }
  }
  else {
    throw Standard_ConstructionError();
  }
}


//=======================================================================
//function : Bind
//purpose  : 
//=======================================================================

void LocOpe_WiresOnShape::Bind(const TopoDS_Edge& Ewir,
			       const TopoDS_Edge& Efac)
{
  myMap.Bind(Ewir,Efac);
}


//=======================================================================
//function : BindAll
//purpose  : 
//=======================================================================

void LocOpe_WiresOnShape::BindAll()
{
  if (myDone) {
    return;
  }
  TopTools_MapOfShape theMap;

  // Detection des vertex a projeter ou a "binder" avec des vertex existants
  TopTools_DataMapOfShapeShape mapV;
  TopTools_DataMapIteratorOfDataMapOfShapeShape ite(myMap);
  TopExp_Explorer exp,exp2;
  for (; ite.More(); ite.Next()) {
    const TopoDS_Edge& eref = TopoDS::Edge(ite.Key());
    const TopoDS_Edge& eimg = TopoDS::Edge(ite.Value());

    PutPCurves(eref,eimg,myShape);

    for (exp.Init(eref,TopAbs_VERTEX); exp.More(); exp.Next()) {
      const TopoDS_Vertex& vtx = TopoDS::Vertex(exp.Current());
      if (!theMap.Contains(vtx)) { // pas deja traite
	for (exp2.Init(eimg,TopAbs_VERTEX); exp2.More(); exp2.Next()) {
	  const TopoDS_Vertex& vtx2 = TopoDS::Vertex(exp2.Current());
	  if (vtx2.IsSame(vtx)) {
	    break;
	  }
	  else if (BRepTools::Compare(vtx,vtx2)) {
	    mapV.Bind(vtx,vtx2);
	    break;
	  }
	}
	if (!exp2.More()) {
	  mapV.Bind(vtx,eimg);
	}
	theMap.Add(vtx);
      }
    }
  }
  
  for (ite.Initialize(mapV); ite.More(); ite.Next()) {
    myMap.Bind(ite.Key(),ite.Value());
  }

  TopTools_IndexedDataMapOfShapeListOfShape Splits;
  Standard_Integer Ind;
  TopTools_MapOfShape anOverlappedEdges;
  for (Ind = 1; Ind <= myMapEF.Extent(); Ind++)
  {
    const TopoDS_Edge& edg = TopoDS::Edge(myMapEF.FindKey(Ind));
    const TopoDS_Face& fac = TopoDS::Face(myMapEF(Ind));
    
    PutPCurve(edg,fac);
    Standard_Real pf, pl;
    Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(edg, fac, pf, pl);
    if (aPCurve.IsNull())
      continue;

    if (myCheckInterior)
    {
      Standard_Boolean isOverlapped = Standard_False;
      FindInternalIntersections(edg, fac, Splits, isOverlapped);
      if(isOverlapped)
        anOverlappedEdges.Add(edg);
    }
  }

  for (Ind = 1; Ind <= Splits.Extent(); Ind++)
  {
    TopoDS_Shape anEdge = Splits.FindKey(Ind);
    if(anOverlappedEdges.Contains(anEdge))
      continue;
    TopoDS_Shape aFace  = myMapEF.FindFromKey(anEdge);
    //Remove "anEdge" from "myMapEF"
    myMapEF.RemoveKey(anEdge);
    TopTools_ListIteratorOfListOfShape itl(Splits(Ind));
    for (; itl.More(); itl.Next())
      myMapEF.Add(itl.Value(), aFace);
  }

  TopTools_DataMapOfShapeReal  aVertParam;

  for (Ind = 1; Ind <= myMapEF.Extent(); Ind++) {
    const TopoDS_Edge& edg = TopoDS::Edge(myMapEF.FindKey(Ind));
    const TopoDS_Face& fac = TopoDS::Face(myMapEF(Ind));
    // JAG 02.02.96 : On verifie les pcurves...

    //PutPCurve(edg,fac);

    for (exp.Init(edg,TopAbs_VERTEX); exp.More(); exp.Next()) {
      const TopoDS_Vertex& vtx = TopoDS::Vertex(exp.Current());
      if (theMap.Contains(vtx)) {
	continue;
      }
      
      Standard_Real vtx_param = BRep_Tool::Parameter(vtx, edg);
      BRepAdaptor_Curve2d BAcurve2d(edg, fac);
            
      gp_Pnt2d p2d = (!BAcurve2d.Curve().IsNull() ? 
        BAcurve2d.Value(vtx_param) : gp_Pnt2d(Precision::Infinite(), Precision::Infinite()));

      TopoDS_Edge Epro;
      Standard_Real prm = Precision::Infinite();
      Standard_Boolean isProjected = myMap.IsBound(vtx);

      //if vertex was already projected on the current edge on the previous face
      //it is necessary to check tolerance of the vertex in the 2D space on the current
      //face without projection and update tolerance of vertex if it is necessary
      if (isProjected)
      {
        TopoDS_Shape aSh = myMap.Find(vtx);
        if (aSh.ShapeType() != TopAbs_EDGE)
          continue;
        Epro = TopoDS::Edge(myMap.Find(vtx));
        if (aVertParam.IsBound(vtx))
          prm = aVertParam.Find(vtx);
      }
      Standard_Boolean ok = Project(vtx, p2d, fac, Epro, prm);
      if (ok && !isProjected) {

        for (exp2.Init(Epro, TopAbs_VERTEX); exp2.More(); exp2.Next()) {
          const TopoDS_Vertex& vtx2 = TopoDS::Vertex(exp2.Current());
          if (vtx2.IsSame(vtx)) {
            myMap.Bind(vtx, vtx2);
            theMap.Add(vtx);
            break;
          }
          else if (BRepTools::Compare(vtx, vtx2)) {
            Standard_Real aF1, aL1;
            BRep_Tool::Range(Epro, fac, aF1, aL1);
            if (!BRep_Tool::Degenerated(Epro) && (
              Abs(prm - aF1) <= Precision::PConfusion() ||
              Abs(prm - aL1) <= Precision::PConfusion()))
            {
              myMap.Bind(vtx, vtx2);
              theMap.Add(vtx);
              break;
            }
          }
        }
	if (!exp2.More()) {
	  myMap.Bind(vtx,Epro);
          aVertParam.Bind(vtx, prm);
        }
      }
    }
  }

//  Modified by Sergey KHROMOV - Mon Feb 12 16:26:50 2001 Begin
  for (ite.Initialize(myMap); ite.More(); ite.Next())
    if ((ite.Key()).ShapeType() == TopAbs_EDGE)
      myMapEF.Add(ite.Key(),ite.Value());
//  Modified by Sergey KHROMOV - Mon Feb 12 16:26:52 2001 End


  myDone = Standard_True;

}


//=======================================================================
//function : InitEdgeIterator
//purpose  : 
//=======================================================================

void LocOpe_WiresOnShape::InitEdgeIterator()
{
  BindAll();
//  myIt.Initialize(myMapEF);
  myIndex = 1;
}


//=======================================================================
//function : MoreEdge
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe_WiresOnShape::MoreEdge()
{
//  return myIt.More();
  return (myIndex <= myMapEF.Extent());
}


//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

TopoDS_Edge LocOpe_WiresOnShape::Edge()
{
//  return TopoDS::Edge(myIt.Key());
  return TopoDS::Edge(myMapEF.FindKey(myIndex));
}


//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

TopoDS_Face LocOpe_WiresOnShape::OnFace()
{
//  return TopoDS::Face(myIt.Value());
  return TopoDS::Face(myMapEF(myIndex));
}


//=======================================================================
//function : OnEdge
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe_WiresOnShape::OnEdge(TopoDS_Edge& E)
{
//  if (myMap.IsBound(myIt.Key())) {
  if (myMap.IsBound(myMapEF.FindKey(myIndex))) {
//    E = TopoDS::Edge(myMap(myIt.Key()));
    E = TopoDS::Edge(myMap(myMapEF.FindKey(myIndex)));
    return Standard_True;
  }
  return Standard_False;
}




//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================

void LocOpe_WiresOnShape::NextEdge()
{
//  myIt.Next();
  myIndex++;
}



//=======================================================================
//function : OnVertex
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe_WiresOnShape::OnVertex(const TopoDS_Vertex& Vw,
					       TopoDS_Vertex& Vs)
{
  if (myMap.IsBound(Vw)) {
    if (myMap(Vw).ShapeType() == TopAbs_VERTEX) {
      Vs = TopoDS::Vertex(myMap(Vw));
      return Standard_True;
    }
    return Standard_False;
  }
  return Standard_False;
}


//=======================================================================
//function : OnEdge
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe_WiresOnShape::OnEdge(const TopoDS_Vertex& V,
					  TopoDS_Edge& Ed,
					  Standard_Real& prm)
{
  if (!myMap.IsBound(V) ||
      myMap(V).ShapeType() == TopAbs_VERTEX) {
    return Standard_False;
  }
  
  Ed = TopoDS::Edge(myMap(V));
  prm = Project(V,Ed);
  return Standard_True;
}

//=======================================================================
//function : OnEdge
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe_WiresOnShape::OnEdge(const TopoDS_Vertex& V,
                                             const TopoDS_Edge& EdgeFrom,
                                             TopoDS_Edge& Ed,
                                             Standard_Real& prm)
{
  if (!myMap.IsBound(V) ||
      myMap(V).ShapeType() == TopAbs_VERTEX) {
    return Standard_False;
  }
  
  Ed = TopoDS::Edge(myMap(V));
  if(!myMapEF.Contains(EdgeFrom))
    return Standard_False;

  TopoDS_Shape aShape = myMapEF.FindFromKey(EdgeFrom);
  Standard_Real aF, aL;
  Handle(Geom_Curve) aC = BRep_Tool::Curve(Ed, aF, aL);
  if (aC.IsNull() && aShape.ShapeType() == TopAbs_FACE)
  {

    TopoDS_Face aFace = TopoDS::Face(aShape);
    Standard_Real vtx_param = BRep_Tool::Parameter(V, EdgeFrom);
    BRepAdaptor_Curve2d BAcurve2d(EdgeFrom, aFace);
    gp_Pnt2d p2d = BAcurve2d.Value(vtx_param);


    prm = Project(V, p2d, Ed, aFace);
  }
  else
    prm = Project( V, TopoDS::Edge(Ed));
	
  return Standard_True;
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

Standard_Boolean Project(const TopoDS_Vertex& V,
                         const gp_Pnt2d& p2d,
			 const TopoDS_Face& F,
			 TopoDS_Edge& theEdge,
			 Standard_Real& param)
{
  Standard_Real aTolV = BRep_Tool::Tolerance(V);
  Standard_Real dmin = (theEdge.IsNull() ? RealLast() : aTolV * aTolV);
 
  Standard_Boolean valret = Standard_False;
  
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(F);
 
  if (theEdge.IsNull())
  {
    gp_Pnt toproj(BRep_Tool::Pnt(V));
    for (TopExp_Explorer exp(F.Oriented(TopAbs_FORWARD), TopAbs_EDGE);
      exp.More(); exp.Next()) {
      const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
      Standard_Real f, l;
      Handle(Geom_Curve) C = BRep_Tool::Curve(edg, f, l);
      Standard_Real aCurDist = Precision::Infinite();
      Standard_Real aCurPar = Precision::Infinite();
      if (!C.IsNull())
      {
        aCurPar = Project(V, edg);
        if (Precision::IsInfinite(aCurPar))
          continue;
        gp_Pnt aCurPBound;
        C->D0(aCurPar, aCurPBound);
        aCurDist = aCurPBound.SquareDistance(toproj);
      }

      else if(!Precision::IsInfinite(p2d.X()))
      {
        //Geom2dAPI_ProjectPointOnCurve proj;
        Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(edg,F , f, l);
        if(aC2d.IsNull())
          continue;

        aCurPar = Project(V, p2d, edg, F);
        if (Precision::IsInfinite(aCurPar))
          continue;
        Handle(Geom2d_Curve) PC = BRep_Tool::CurveOnSurface(edg, F, f, l);
        gp_Pnt2d aPProj;
        PC->D0(aCurPar, aPProj);
        gp_Pnt aCurPBound;
        aSurf->D0(aPProj.X(), aPProj.Y(), aCurPBound);
        aCurDist = aCurPBound.SquareDistance(toproj);

      }

      if (aCurDist < dmin) {
        theEdge = edg;
        theEdge.Orientation(edg.Orientation());
        dmin = aCurDist;
        param = aCurPar;

      }

    }
    if (theEdge.IsNull())
      return Standard_False;
  }
  else if (Precision::IsInfinite(param))
  {
    Standard_Real f, l;
    Handle(Geom_Curve) C = BRep_Tool::Curve(theEdge, f, l);
    param = (!C.IsNull() ? Project(V, theEdge) : Project(V, p2d, theEdge, F));

  }

  Standard_Real ttol = aTolV + BRep_Tool::Tolerance(theEdge);
  if (dmin <= ttol* ttol) {
    valret = Standard_True;
    GeomAdaptor_Surface adSurf(aSurf);

    Standard_Real anUResolution = adSurf.UResolution(1.);
    Standard_Real aVResolution = adSurf.UResolution(1.);

    Standard_Real f, l;
    Handle(Geom2d_Curve) aCrvBound = BRep_Tool::CurveOnSurface(theEdge, F, f, l);
    if (!aCrvBound.IsNull())
    {
      gp_Pnt2d aPBound2d;
      aCrvBound->D0(param, aPBound2d);

      //distance in 2D space recomputed in the 3D space in order to tolerance of vertex
      //cover gap in 2D space. For consistency with check of the validity in the  BRepCheck_Wire
      Standard_Real dumax = 0.01 * (adSurf.LastUParameter() - adSurf.FirstUParameter());
      Standard_Real dvmax = 0.01 * (adSurf.LastVParameter() - adSurf.FirstVParameter());
      
      gp_Pnt2d aPcur = p2d;
      Standard_Real dumin = Abs(aPcur.X() - aPBound2d.X());
      Standard_Real dvmin = Abs(aPcur.Y() - aPBound2d.Y());
      if (dumin > dumax && adSurf.IsUPeriodic())
      {
        Standard_Real aX1 = aPBound2d.X();
        Standard_Real aShift = ShapeAnalysis::AdjustToPeriod(aX1, adSurf.FirstUParameter(), adSurf.LastUParameter());
        aX1 += aShift;
        aPBound2d.SetX(aX1);
        Standard_Real aX2 = p2d.X();
        aShift = ShapeAnalysis::AdjustToPeriod(aX2, adSurf.FirstUParameter(), adSurf.LastUParameter());
        aX2 += aShift;
        dumin = Abs(aX2 - aX1);
        if (dumin > dumax &&  (Abs(dumin - adSurf.UPeriod()) < Precision::PConfusion()) )
        {
          aX2 = aX1;
          dumin = 0.;
        }
        aPcur.SetX(aX2);

      }

      if (dvmin > dvmax && adSurf.IsVPeriodic())
      {
        Standard_Real aY1 = aPBound2d.Y();
        Standard_Real aShift = ShapeAnalysis::AdjustToPeriod(aY1, adSurf.FirstVParameter(), adSurf.LastVParameter());
        aY1 += aShift;
        aPBound2d.SetY(aY1);
        Standard_Real aY2 = p2d.Y();
        aShift = ShapeAnalysis::AdjustToPeriod(aY2, adSurf.FirstVParameter(), adSurf.LastVParameter());
        aY2 += aShift;
        dvmin = Abs(aY1 - aY2);
        if (dvmin > dvmax && ( Abs(dvmin - adSurf.VPeriod()) < Precision::Confusion()) )
        {
          aY2 = aY1;
          dvmin = 0.;
        }
        aPcur.SetY(aY2);
      }
      Standard_Real aDist3d = aTolV;
      if ((dumin > dumax) || (dvmin > dvmax))
      {

        dumax = adSurf.UResolution(aTolV);
        dvmax = adSurf.VResolution(aTolV);
        Standard_Real aTol2d = 2. * Max(dumax, dvmax);
        Standard_Real aDist2d =  Max(dumin, dvmin);

        if (aDist2d > aTol2d)
        {
          Standard_Real aDist3d1 = aDist2d / Max(anUResolution, aVResolution);
          if( aDist3d1 > aDist3d)
            aDist3d =  aDist3d1;
        }
      }

      //added check by 3D the same as in the BRepCheck_Wire::SelfIntersect
      gp_Pnt aPBound;
      aSurf->D0(aPBound2d.X(), aPBound2d.Y(), aPBound);
      gp_Pnt aPV2d;
      aSurf->D0(p2d.X(), p2d.Y(), aPV2d);
      Standard_Real aDistPoints_3D = aPV2d.SquareDistance(aPBound);
	    Standard_Real aMaxDist = Max(aDistPoints_3D, aDist3d * aDist3d);
 
      BRep_Builder B;
      if (aTolV * aTolV < aMaxDist)
      {
        Standard_Real aNewTol = sqrt(aMaxDist);
        B.UpdateVertex(V, aNewTol);
      }
    }
  }
#ifdef OCCT_DEBUG_MESH
  else {
    cout <<"LocOpe_WiresOnShape::Project --> le vertex projete est a une "; 
    cout <<"distance / la face = "<<dmin <<" superieure a la tolerance = "<<ttol<<endl;
  }
#endif
  return valret;
}

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

Standard_Real Project(const TopoDS_Vertex& V,
		      const TopoDS_Edge& theEdge)
{
  Handle(Geom_Curve) C;
  TopLoc_Location Loc;
  Standard_Real f,l;

  gp_Pnt toproj(BRep_Tool::Pnt(V));
  GeomAPI_ProjectPointOnCurve proj;

  C = BRep_Tool::Curve(theEdge,Loc,f,l);
  if (!Loc.IsIdentity()) {
    Handle(Geom_Geometry) GG = C->Transformed(Loc.Transformation());
    C = Handle(Geom_Curve)::DownCast (GG);
  }
  proj.Init(toproj,C,f,l);
  


  return (proj.NbPoints() > 0 ? proj.LowerDistanceParameter() : Precision::Infinite());
}

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

Standard_Real Project(const TopoDS_Vertex&,
                      const gp_Pnt2d&      p2d,
		      const TopoDS_Edge& theEdge,
                      const TopoDS_Face& theFace)
{

  
  Handle(Geom2d_Curve) PC;
 
  Standard_Real f, l;

  Geom2dAPI_ProjectPointOnCurve proj;

  PC = BRep_Tool::CurveOnSurface(theEdge, theFace, f, l);
  
  proj.Init(p2d, PC, f, l);

  return proj.NbPoints() > 0 ? proj.LowerDistanceParameter() : Precision::Infinite();
}


//=======================================================================
//function : PutPCurve
//purpose  : 
//=======================================================================

void PutPCurve(const TopoDS_Edge& Edg,
	       const TopoDS_Face& Fac)
{
  BRep_Builder B;
  TopLoc_Location LocFac;

  Handle(Geom_Surface) S = BRep_Tool::Surface(Fac, LocFac);
  Handle(Standard_Type) styp = S->DynamicType();

  if (styp == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    S = Handle(Geom_RectangularTrimmedSurface)::DownCast (S)->BasisSurface();
    styp = S->DynamicType();
  }

  if (styp == STANDARD_TYPE(Geom_Plane)) {
    return;
  }

  Standard_Real Umin,Umax,Vmin,Vmax;
  BRepTools::UVBounds(Fac,Umin,Umax,Vmin,Vmax);

  Standard_Real f,l;

  Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(Edg,Fac,f,l);
  if ( !aC2d.IsNull() ) {
    gp_Pnt2d p2d;
    aC2d->D0((f + l) *0.5, p2d);
    Standard_Boolean IsIn = Standard_True;
    if ( ( p2d.X() < Umin-Precision::PConfusion() ) || 
         ( p2d.X() > Umax+Precision::PConfusion() ) )
      IsIn = Standard_False;
    if ( ( p2d.Y() < Vmin-Precision::PConfusion() ) || 
         ( p2d.Y() > Vmax+Precision::PConfusion() ) )
      IsIn = Standard_False;
 
    if (IsIn)
      return;
  }

  TopLoc_Location Loc;
  Handle(Geom_Curve) C = BRep_Tool::Curve(Edg,Loc,f,l);
  if (!Loc.IsIdentity()) {
    Handle(Geom_Geometry) GG = C->Transformed(Loc.Transformation());
    C = Handle(Geom_Curve)::DownCast (GG);
  }

  if (C->DynamicType() != STANDARD_TYPE(Geom_TrimmedCurve)) {
    C = new Geom_TrimmedCurve(C,f,l);
  }

  S = BRep_Tool::Surface(Fac);

  Standard_Real TolFirst = -1, TolLast = -1;
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(Edg, V1, V2);
  if (!V1.IsNull())
    TolFirst = BRep_Tool::Tolerance(V1);
  if (!V2.IsNull())
    TolLast = BRep_Tool::Tolerance(V2);
  
  Standard_Real tol2d = Precision::Confusion();
  Handle(Geom2d_Curve) C2d;
  ShapeConstruct_ProjectCurveOnSurface aToolProj;
  aToolProj.Init(S, tol2d);

  aToolProj.Perform(C,f,l,C2d,TolFirst,TolLast);
  if (C2d.IsNull())
  {
    return;
  }
 
  gp_Pnt2d pf(C2d->Value(f));
  gp_Pnt2d pl(C2d->Value(l));
  gp_Pnt PF,PL;
  S->D0(pf.X(),pf.Y(),PF);
  S->D0(pl.X(),pl.Y(),PL);
  if (Edg.Orientation() == TopAbs_REVERSED) {
    V1 = TopExp::LastVertex(Edg);
    V1.Reverse();
  }
  else {
    V1 = TopExp::FirstVertex (Edg);
  }
  if (Edg.Orientation() == TopAbs_REVERSED) {
    V2 = TopExp::FirstVertex(Edg);
    V2.Reverse();
  }
  else {
    V2 = TopExp::LastVertex (Edg);
  }

  if(!V1.IsNull() && V2.IsNull()) {
    //Handling of internal vertices
    Standard_Real old1 = BRep_Tool::Tolerance (V1);
    Standard_Real old2 = BRep_Tool::Tolerance (V2);
    gp_Pnt pnt1 = BRep_Tool::Pnt (V1);
    gp_Pnt pnt2 = BRep_Tool::Pnt (V2);
    Standard_Real tol1 = pnt1.Distance(PF);
    Standard_Real tol2 = pnt2.Distance(PL);
    B.UpdateVertex(V1,Max(old1,tol1));
    B.UpdateVertex(V2,Max(old2,tol2));
  }

  if (S->IsUPeriodic()) {
    Standard_Real up   = S->UPeriod();
    Standard_Real tolu = Precision::PConfusion();// Epsilon(up);
    Standard_Integer nbtra = 0;
    Standard_Real theUmin = Min(pf.X(),pl.X());
    Standard_Real theUmax = Max(pf.X(),pl.X());

    if (theUmin < Umin-tolu) {
      while (theUmin < Umin-tolu) {
	theUmin += up;
	nbtra++;
      }
    }
    else if (theUmax > Umax+tolu) {
      while (theUmax > Umax+tolu) {
	theUmax -= up;
	nbtra--;
      }
    }

    if (nbtra !=0) {
      C2d->Translate(gp_Vec2d(nbtra*up,0.));
    }
  }    

  if (S->IsVPeriodic()) {
    Standard_Real vp   = S->VPeriod();
    Standard_Real tolv = Precision::PConfusion();// Epsilon(vp);
    Standard_Integer nbtra = 0;
    Standard_Real theVmin = Min(pf.Y(),pl.Y());
    Standard_Real theVmax = Max(pf.Y(),pl.Y());

    if (theVmin < Vmin-tolv) {
      while (theVmin < Vmin-tolv) {
	theVmin += vp; theVmax += vp;
	nbtra++;
      }
    }
    else if (theVmax > Vmax+tolv) {
      while (theVmax > Vmax+tolv) {
	theVmax -= vp; theVmin -= vp;
	nbtra--;
      }
    }

    if (nbtra !=0) {
      C2d->Translate(gp_Vec2d(0.,nbtra*vp));
    }
  }
  B.UpdateEdge(Edg,C2d,Fac,BRep_Tool::Tolerance(Edg));
  
  B.SameParameter(Edg,Standard_False);
  BRepLib::SameParameter(Edg,tol2d); 
}


//=======================================================================
//function : PutPCurves
//purpose  : 
//=======================================================================

void PutPCurves(const TopoDS_Edge& Efrom,
		const TopoDS_Edge& Eto,
		const TopoDS_Shape& myShape)
{

  TopTools_ListOfShape Lfaces;
  TopExp_Explorer exp,exp2;

  for (exp.Init(myShape,TopAbs_FACE); exp.More(); exp.Next()) {
    for (exp2.Init(exp.Current(), TopAbs_EDGE); exp2.More();exp2.Next()) {
      if (exp2.Current().IsSame(Eto)) {
	Lfaces.Append(exp.Current());
      }
    }
  }

  if (Lfaces.Extent() != 1 && Lfaces.Extent() !=2) {
    throw Standard_ConstructionError();
  }

  // soit bord libre, soit connexite entre 2 faces, eventuellement edge closed

  if (Lfaces.Extent() ==1) {
    return; // sera fait par PutPCurve.... on l`espere
  }

  BRep_Builder B;
  Handle(Geom_Surface) S;
  Handle(Standard_Type) styp;
  Handle(Geom_Curve) C;
  Standard_Real Umin,Umax,Vmin,Vmax;
  Standard_Real f,l;
  TopLoc_Location Loc, LocFac;

  if (!Lfaces.First().IsSame(Lfaces.Last())) {
    TopTools_ListIteratorOfListOfShape itl(Lfaces);
    for (; itl.More(); itl.Next()) {
      const TopoDS_Face& Fac = TopoDS::Face(itl.Value());

      if (!BRep_Tool::CurveOnSurface(Efrom,Fac,f,l).IsNull()) {
	continue;
      }
      S = BRep_Tool::Surface(Fac, LocFac);
      styp = S->DynamicType();
      if (styp == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
	S = Handle(Geom_RectangularTrimmedSurface)::DownCast (S)->BasisSurface();
	styp = S->DynamicType();
      }
      if (styp == STANDARD_TYPE(Geom_Plane)) {
	continue;
      }

            
      BRepTools::UVBounds(Fac,Umin,Umax,Vmin,Vmax);
      C = BRep_Tool::Curve(Efrom,Loc,f,l);
      if (!Loc.IsIdentity()) {
	Handle(Geom_Geometry) GG = C->Transformed(Loc.Transformation());
	C = Handle(Geom_Curve)::DownCast (GG);
      }
      
      if (C->DynamicType() != STANDARD_TYPE(Geom_TrimmedCurve)) {
	C = new Geom_TrimmedCurve(C,f,l);
      }
    
      S = BRep_Tool::Surface(Fac);

      // Compute the tol2d
      Standard_Real tol3d = Max(BRep_Tool::Tolerance(Efrom),
				BRep_Tool::Tolerance(Fac));
      GeomAdaptor_Surface Gas(S,Umin,Umax,Vmin,Vmax);
      Standard_Real TolU  = Gas.UResolution(tol3d);
      Standard_Real TolV  = Gas.VResolution(tol3d);
      Standard_Real tol2d = Max(TolU,TolV);

      Handle(Geom2d_Curve) C2d = 
	GeomProjLib::Curve2d(C,S,Umin,Umax,Vmin,Vmax,tol2d);
     if(C2d.IsNull())
       return;

      gp_Pnt2d pf(C2d->Value(f));
      gp_Pnt2d pl(C2d->Value(l));
      
      if (S->IsUPeriodic()) {
	Standard_Real up   = S->UPeriod();
	Standard_Real tolu = Precision::PConfusion();// Epsilon(up);
	Standard_Integer nbtra = 0;
	Standard_Real theUmin = Min(pf.X(),pl.X());
	Standard_Real theUmax = Max(pf.X(),pl.X());

	if (theUmin < Umin-tolu) {
	  while (theUmin < Umin-tolu) {
	    theUmin += up; theUmax += up;
	    nbtra++;
	  }
	}
	else if (theUmax > Umax+tolu) {
	  while (theUmax > Umax+tolu) {
	    theUmax -= up; theUmin -= up;
	    nbtra--;
	  }
	}
/*
	if (theUmin > Umax+tolu) {
	  while (theUmin > Umax+tolu) {
	    theUmin -= up;
	    nbtra--;
	  }
	}
	else if (theUmax < Umin-tolu) {
 	  while (theUmax < Umin-tolu) {
	    theUmax += up;
	    nbtra++;
	  }
	}
*/
	if (nbtra !=0) {
	  C2d->Translate(gp_Vec2d(nbtra*up,0.));
	}
      }    
      
      if (S->IsVPeriodic()) {
	Standard_Real vp   = S->VPeriod();
	Standard_Real tolv = Precision::PConfusion();// Epsilon(vp);
	Standard_Integer nbtra = 0;
	Standard_Real theVmin = Min(pf.Y(),pl.Y());
	Standard_Real theVmax = Max(pf.Y(),pl.Y());

	if (theVmin < Vmin-tolv) {
	  while (theVmin < Vmin-tolv) {
	    theVmin += vp; theVmax += vp;
	    nbtra++;
	  }
	}
	else if (theVmax > Vmax+tolv) {
	  while (theVmax > Vmax+tolv) {
	    theVmax -= vp; theVmin -= vp;
	    nbtra--;
	  }
	}
/*
	if (theVmin > Vmax+tolv) {
	  while (theVmin > Vmax+tolv) {
	    theVmin -= vp;
	    nbtra--;
	  }
	}
	else if (theVmax < Vmin-tolv) {
	  while (theVmax < Vmin-tolv) {
	    theVmax += vp;
	    nbtra++;
	  }
	}
*/
	if (nbtra !=0) {
	  C2d->Translate(gp_Vec2d(0.,nbtra*vp));
	}
      }
      B.UpdateEdge(Efrom,C2d,Fac,BRep_Tool::Tolerance(Efrom));
    }
  }

  else {
    const TopoDS_Face& Fac = TopoDS::Face(Lfaces.First());
    if (!BRep_Tool::IsClosed(Eto,Fac)) {
      throw Standard_ConstructionError();
    }

    TopoDS_Shape aLocalE = Efrom.Oriented(TopAbs_FORWARD);
    TopoDS_Shape aLocalF = Fac.Oriented(TopAbs_FORWARD);
    Handle(Geom2d_Curve) c2dff = 
      BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalE),
				TopoDS::Face(aLocalF),
				f,l);

//    Handle(Geom2d_Curve) c2dff = 
//      BRep_Tool::CurveOnSurface(TopoDS::Edge(Efrom.Oriented(TopAbs_FORWARD)),
//				TopoDS::Face(Fac.Oriented(TopAbs_FORWARD)),
//				f,l);
    
    aLocalE = Efrom.Oriented(TopAbs_REVERSED);
    aLocalF = Fac.Oriented(TopAbs_FORWARD);
    Handle(Geom2d_Curve) c2dfr = 
      BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalE),
				TopoDS::Face(aLocalF),
				f,l);
//    Handle(Geom2d_Curve) c2dfr = 
//      BRep_Tool::CurveOnSurface(TopoDS::Edge(Efrom.Oriented(TopAbs_REVERSED)),
//				TopoDS::Face(Fac.Oriented(TopAbs_FORWARD)),
//				f,l);
    
    aLocalE = Eto.Oriented(TopAbs_FORWARD);
    aLocalF = Fac.Oriented(TopAbs_FORWARD);
    Handle(Geom2d_Curve) c2dtf = 
      BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalE),
				TopoDS::Face(aLocalF),
				f,l);

//    Handle(Geom2d_Curve) c2dtf = 
//      BRep_Tool::CurveOnSurface(TopoDS::Edge(Eto.Oriented(TopAbs_FORWARD)),
//				TopoDS::Face(Fac.Oriented(TopAbs_FORWARD)),
//				f,l);
    aLocalE = Eto.Oriented(TopAbs_REVERSED);
    aLocalF = Fac.Oriented(TopAbs_FORWARD);
    Handle(Geom2d_Curve) c2dtr = 
      BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalE),
				TopoDS::Face(aLocalF),
				f,l);
//    Handle(Geom2d_Curve) c2dtr = 
//      BRep_Tool::CurveOnSurface(TopoDS::Edge(Eto.Oriented(TopAbs_REVERSED)),
//				TopoDS::Face(Fac.Oriented(TopAbs_FORWARD)),
//				f,l);

    gp_Pnt2d ptf(c2dtf->Value(f)); // sur courbe frw
    gp_Pnt2d ptr(c2dtr->Value(f)); // sur courbe rev

    Standard_Boolean isoU = (Abs(ptf.Y()-ptr.Y())<Epsilon(ptf.X())); // meme V

    // Efrom et Eto dans le meme sens???

    C = BRep_Tool::Curve(Efrom,Loc,f,l);
    if (!Loc.IsIdentity()) {
      Handle(Geom_Geometry) GG = C->Transformed(Loc.Transformation());
      C = Handle(Geom_Curve)::DownCast (GG);
    }

    gp_Pnt pt;
    gp_Vec d1f,d1t;

    C->D1(f,pt,d1f);


    TopoDS_Vertex FirstVertex = TopExp::FirstVertex(Efrom);
    Standard_Real vtx_param = BRep_Tool::Parameter(FirstVertex, Efrom);
    BRepAdaptor_Curve2d BAcurve2d(Efrom, Fac);
    gp_Pnt2d p2d = BAcurve2d.Value(vtx_param);

    Standard_Real prmproj = Project(TopExp::FirstVertex(Efrom),p2d,Eto,Fac);
    
    C = BRep_Tool::Curve(Eto,Loc,f,l);
    if (!Loc.IsIdentity()) {
      Handle(Geom_Geometry) GG = C->Transformed(Loc.Transformation());
      C = Handle(Geom_Curve)::DownCast (GG);
    }
    
    C->D1(prmproj,pt,d1t);

    Standard_Real SameOri = (d1t.Dot(d1f)>0.); 


    if (c2dff.IsNull() && c2dfr.IsNull()) {
      S = BRep_Tool::Surface(Fac);
      styp = S->DynamicType();
      if (styp == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
	S = Handle(Geom_RectangularTrimmedSurface)::DownCast (S)->BasisSurface();
	styp = S->DynamicType();
      }
      
      C = BRep_Tool::Curve(Efrom,Loc,f,l);
      if (!Loc.IsIdentity()) {
	Handle(Geom_Geometry) GG = C->Transformed(Loc.Transformation());
	C = Handle(Geom_Curve)::DownCast (GG);
      }
      
      if (C->DynamicType() != STANDARD_TYPE(Geom_TrimmedCurve)) {
	C = new Geom_TrimmedCurve(C,f,l);
      }

      // Compute the tol2d
      BRepTools::UVBounds(Fac,Umin,Umax,Vmin,Vmax);

      Standard_Real tol3d = Max(BRep_Tool::Tolerance(Efrom),
				BRep_Tool::Tolerance(Fac));
      GeomAdaptor_Surface Gas(S,Umin,Umax,Vmin,Vmax);
      Standard_Real TolU  = Gas.UResolution(tol3d);
      Standard_Real TolV  = Gas.VResolution(tol3d);
      Standard_Real tol2d = Max(TolU,TolV);
    
      Handle(Geom2d_Curve) C2d = 
	GeomProjLib::Curve2d(C,S,Umin,Umax,Vmin,Vmax,tol2d);
      c2dff = C2d;
      c2dfr = C2d;
    }
    else if (c2dfr.IsNull()) {
      c2dfr = c2dff;

    }
    else if (c2dff.IsNull()) {
      c2dff = c2dfr;
    }

    BRep_Tool::Range(Efrom,f,l);

    gp_Pnt2d p2f = c2dff->Value(f);
    gp_Pnt2d p2r = c2dfr->Value(f);

    if (isoU) {
      if (SameOri) {
	if (Abs(ptf.X()-p2f.X()) > Epsilon(ptf.X())) {
	  c2dff = Handle(Geom2d_Curve)::DownCast
	    (c2dff->Translated(gp_Vec2d(ptf.X()-p2f.X(),0.)));
	}
	if (Abs(ptr.X()-p2r.X()) > Epsilon(ptr.X())) {
	  c2dfr = Handle(Geom2d_Curve)::DownCast
	    (c2dfr->Translated(gp_Vec2d(ptr.X()-p2r.X(),0.)));
	}
      }
      else {
	if (Abs(ptr.X()-p2f.X()) > Epsilon(ptr.X())) {
	  c2dff = Handle(Geom2d_Curve)::DownCast
	    (c2dff->Translated(gp_Vec2d(ptr.X()-p2f.X(),0.)));
	}
	
	if (Abs(ptf.X()-p2r.X()) > Epsilon(ptf.X())) {
	  c2dfr = Handle(Geom2d_Curve)::DownCast
	    (c2dfr->Translated(gp_Vec2d(ptf.X()-p2r.X(),0.)));
	}
      }

      // on est bien en U, recalage si periodique en V a faire



    }
    
    else { // !isoU soit isoV
      if (SameOri) {
	if (Abs(ptf.Y()-p2f.Y()) > Epsilon(ptf.Y())) {
	  c2dff = Handle(Geom2d_Curve)::DownCast
	    (c2dff->Translated(gp_Vec2d(0.,ptf.Y()-p2f.Y())));
	}
	if (Abs(ptr.Y()-p2r.Y()) > Epsilon(ptr.Y())) {
	  c2dfr = Handle(Geom2d_Curve)::DownCast
	    (c2dfr->Translated(gp_Vec2d(0.,ptr.Y()-p2r.Y())));
	}
      }
      else {
	if (Abs(ptr.Y()-p2f.Y()) > Epsilon(ptr.Y())) {
	  c2dff = Handle(Geom2d_Curve)::DownCast
	    (c2dff->Translated(gp_Vec2d(0.,ptr.Y()-p2f.Y())));
	}
	if (Abs(ptf.Y()-p2r.Y()) > Epsilon(ptf.Y())) {
	  c2dfr = Handle(Geom2d_Curve)::DownCast
	    (c2dfr->Translated(gp_Vec2d(0.,ptf.Y()-p2r.Y())));
	}
      }
      // on est bien en V, recalage si periodique en U a faire

    }
    B.UpdateEdge(Efrom,c2dff,c2dfr,Fac,BRep_Tool::Tolerance(Efrom));
  }
}

//=======================================================================
//function : FindInternalIntersections
//purpose  : 
//=======================================================================

void FindInternalIntersections(const TopoDS_Edge& theEdge,
                               const TopoDS_Face& theFace,
                               TopTools_IndexedDataMapOfShapeListOfShape& Splits,
                               Standard_Boolean& isOverlapped)
{
  Standard_Real TolExt = Precision::PConfusion();
  Standard_Integer i, j;

  BRepAdaptor_Surface anAdSurf(theFace, Standard_False);
  TColStd_SequenceOfReal SplitPars;
  
  TopoDS_Vertex theVertices [2];
  TopExp::Vertices(theEdge, theVertices[0], theVertices[1]);
  gp_Pnt thePnt [2];
  thePnt[0] = BRep_Tool::Pnt(theVertices[0]);
  thePnt[1] = BRep_Tool::Pnt(theVertices[1]);
  Standard_Real aTolV[2];
  aTolV[0] =BRep_Tool::Tolerance(theVertices[0]);
  aTolV[1] =BRep_Tool::Tolerance(theVertices[1]);

  BRepAdaptor_Curve2d thePCurve(theEdge, theFace);
  Bnd_Box2d theBox;
  BndLib_Add2dCurve::Add(thePCurve, BRep_Tool::Tolerance(theEdge), theBox);

  Standard_Real thePar [2];
  Standard_Real  aFpar, aLpar;
  const Handle(Geom_Curve)& theCurve = BRep_Tool::Curve(theEdge, thePar[0], thePar[1]);
  GeomAdaptor_Curve theGAcurve(theCurve, thePar[0], thePar[1]);
  Standard_Real aDistMax = Precision::Confusion() * Precision::Confusion();
  TopExp_Explorer Explo(theFace, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
    BRepAdaptor_Curve2d aPCurve(anEdge, theFace);
    Bnd_Box2d aBox;
    BndLib_Add2dCurve::Add(aPCurve, BRep_Tool::Tolerance(anEdge), aBox);
    if (theBox.IsOut(aBox))
      continue;

    const Handle(Geom_Curve)& aCurve   = BRep_Tool::Curve(anEdge, aFpar, aLpar);
    GeomAdaptor_Curve aGAcurve(aCurve, aFpar, aLpar);
    Extrema_ExtCC anExtrema(theGAcurve, aGAcurve, TolExt, TolExt);

    if (!anExtrema.IsDone() || !anExtrema.NbExt())
      continue;
   
    Standard_Integer aNbExt = anExtrema.NbExt();
    Standard_Real MaxTol = BRep_Tool::Tolerance(anEdge);
    Standard_Real aMaxTol2 =  MaxTol *  MaxTol;
    if (anExtrema.IsParallel() && anExtrema.SquareDistance(1) <= aMaxTol2)
    {
      isOverlapped = Standard_True;
      return;
    }
    for (i = 1; i <= aNbExt; i++)
    {
      Standard_Real aDist = anExtrema.SquareDistance(i);
      if (aDist > aMaxTol2)
        continue;

      Extrema_POnCurv aPOnC1, aPOnC2;
      anExtrema.Points(i, aPOnC1, aPOnC2);
      Standard_Real theIntPar = aPOnC1.Parameter();
      Standard_Real anIntPar = aPOnC2.Parameter();
      for (j = 0; j < 2; j++) //try to find intersection on an extremity of "theEdge"
      {
        if (Abs(theIntPar - thePar[j]) <= Precision::PConfusion())
          break;
      }
      //intersection found in the middle of the edge
      if (j >= 2) //intersection is inside "theEdge" => split
      {
        gp_Pnt aPoint = aCurve->Value(anIntPar);
        gp_Pnt aPointInt = theCurve->Value(theIntPar);
    
        if (aPointInt.SquareDistance(thePnt[0]) > aTolV[0] * aTolV[0] &&
          aPointInt.SquareDistance(thePnt[1]) > aTolV[1] * aTolV[1] &&
          aPoint.SquareDistance(thePnt[0]) > aTolV[0] * aTolV[0] &&
          aPoint.SquareDistance(thePnt[1]) > aTolV[1] * aTolV[1])
        {
          SplitPars.Append(theIntPar);
          if( aDist > aDistMax)
            aDistMax = aDist;
        }
      }
    }
  }

  if (SplitPars.IsEmpty())
    return;
  
  //Sort
  for (i = 1; i < SplitPars.Length(); i++)
    for (j = i+1; j <= SplitPars.Length(); j++)
      if (SplitPars(i) > SplitPars(j))
      {
        Standard_Real Tmp = SplitPars(i);
        SplitPars(i) = SplitPars(j);
        SplitPars(j) = Tmp;
      }
  //Remove repeating points
  i = 1;
  while (i < SplitPars.Length())
  {
    gp_Pnt Pnt1 = theCurve->Value(SplitPars(i));
    gp_Pnt Pnt2 = theCurve->Value(SplitPars(i+1));
    if (Pnt1.SquareDistance(Pnt2) <= Precision::Confusion()* Precision::Confusion())
      SplitPars.Remove(i+1);
    else
      i++;
  }

  //Split
  TopTools_ListOfShape NewEdges;
  BRep_Builder BB;
  
  TopoDS_Vertex FirstVertex = theVertices[0], LastVertex;
  Standard_Real FirstPar = thePar[0], LastPar;
  for (i = 1; i <= SplitPars.Length()+1; i++)
  {
    FirstVertex.Orientation(TopAbs_FORWARD);
    if (i <= SplitPars.Length())
    {
      LastPar = SplitPars(i);
      gp_Pnt LastPoint = theCurve->Value(LastPar);
      LastVertex = BRepLib_MakeVertex(LastPoint);
      BRep_Builder aB;
      aB.UpdateVertex(LastVertex, sqrt(aDistMax));
    }
    else
    {
      LastPar = thePar[1];
      LastVertex = theVertices[1];
    }
    LastVertex.Orientation(TopAbs_REVERSED);
    
    TopoDS_Shape aLocalShape = theEdge.EmptyCopied();
    TopAbs_Orientation anOrient = aLocalShape.Orientation();
    aLocalShape.Orientation(TopAbs_FORWARD);
    TopoDS_Edge NewEdge = TopoDS::Edge(aLocalShape);
    BB.Range(NewEdge, FirstPar, LastPar);
    BB.Add(NewEdge, FirstVertex);
    BB.Add(NewEdge, LastVertex);
    NewEdge.Orientation(anOrient);
    ShapeAnalysis_Edge aSae;
    Standard_Real amaxdev =0.;
    if (aSae.CheckSameParameter(NewEdge, theFace, amaxdev))
    {
      BRep_Builder aB;
      aB.UpdateEdge(NewEdge, amaxdev);
    }

    if (anOrient == TopAbs_FORWARD)
      NewEdges.Append(NewEdge);
    else
      NewEdges.Prepend(NewEdge);
    FirstVertex = LastVertex;
    FirstPar = LastPar;
  }

  if (!NewEdges.IsEmpty())
    Splits.Add(theEdge, NewEdges);
}

//=======================================================================
//function : AddSplittingEdges
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe_WiresOnShape::Add(const TopTools_SequenceOfShape& theEdges)
{
  TopTools_SequenceOfShape anEdges;
  Bnd_SeqOfBox anEdgeBoxes;
  Standard_Integer i = 1, nb = theEdges.Length();
  for (; i <= nb; i++)
  {
    const TopoDS_Shape& aCurSplit = theEdges(i);
    TopExp_Explorer anExpE(aCurSplit, TopAbs_EDGE);
    for (; anExpE.More(); anExpE.Next())
    {
      const TopoDS_Shape& aCurE = anExpE.Current();

      Bnd_Box aBoxE;
      BRepBndLib::AddClose(aCurE, aBoxE);
      if (aBoxE.IsVoid())
        continue;
      Standard_Real aTolE = BRep_Tool::Tolerance(TopoDS::Edge(aCurE));
      aBoxE.SetGap(aTolE);
      anEdgeBoxes.Append(aBoxE);
      anEdges.Append(aCurE);

    }
  }
  TopExp_Explorer anExpFaces(myShape, TopAbs_FACE);
  Standard_Integer numF = 1;
  TColStd_PackedMapOfInteger anUsedEdges;
  for (; anExpFaces.More(); anExpFaces.Next(), numF++)
  {
    const TopoDS_Face& aCurF = TopoDS::Face(anExpFaces.Current());
    Bnd_Box aBoxF;
    BRepBndLib::Add(aCurF, aBoxF);
    if (aBoxF.IsVoid())
      continue;
    BRepAdaptor_Surface anAdF(aCurF, Standard_False);
    NCollection_Handle<BRepTopAdaptor_FClass2d> aCheckStateTool;

    i = 1;
    nb = anEdgeBoxes.Length();
    for (; i <= nb; i++)
    {
      if (anUsedEdges.Contains(i))
        continue;

      if (aBoxF.IsOut(anEdgeBoxes(i)))
        continue;

      const TopoDS_Edge& aCurE = TopoDS::Edge(anEdges(i));

      Standard_Real aF, aL;
      Handle(Geom_Curve) aC = BRep_Tool::Curve(aCurE, aF, aL);
      if (aC.IsNull())
      {
        anUsedEdges.Add(i);
        continue;
      }
      gp_Pnt aP = aC->Value((aF + aL)* 0.5);
      Extrema_ExtPS anExtr(aP, anAdF, Precision::Confusion(), Precision::Confusion());
     
      if (!anExtr.IsDone() || !anExtr.NbExt())
        continue;
      Standard_Real aTolE = BRep_Tool::Tolerance(TopoDS::Edge(aCurE));
      Standard_Real aTol2 = (aTolE + Precision::Confusion()) * (aTolE + Precision::Confusion());
      Standard_Integer n = 1;
      for (; n <= anExtr.NbExt(); n++)
      {
        Standard_Real aDist2 = anExtr.SquareDistance(n);
        if (aDist2 > aTol2)
          continue;
        const Extrema_POnSurf& aPS = anExtr.Point(n);
        Standard_Real aU, aV;
        aPS.Parameter(aU, aV);

        if (aCheckStateTool.IsNull())
        {
          aCheckStateTool = new BRepTopAdaptor_FClass2d(aCurF, Precision::PConfusion());
        }
        if (aCheckStateTool->Perform(gp_Pnt2d(aU, aV)) == TopAbs_IN)
        {
          Bind(aCurE, aCurF);
          anUsedEdges.Add(i);
        }
        
      }
    }

  }
  return !anUsedEdges.IsEmpty();
}
