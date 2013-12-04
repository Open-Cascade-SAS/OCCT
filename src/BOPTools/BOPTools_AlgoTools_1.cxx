// Created by: Peter KURNEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BOPTools_AlgoTools.ixx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>

#include <TopLoc_Location.hxx>

#include <BRep_TVertex.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_PointRepresentation.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>

#include <Geom2d_Curve.hxx>

#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom2dAdaptor.hxx>
#include <GeomProjLib.hxx>

#include <ProjLib_ProjectedCurve.hxx>
#include <Extrema_LocateExtPC.hxx>

#include <gp_Pnt.hxx>

#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
//
#include <BRepAdaptor_Surface.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <gp_Pnt2d.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <IntRes2d_Domain.hxx>
#include <Geom2dInt_GInter.hxx>
#include <IntRes2d_IntersectionPoint.hxx>

static 
  void CheckEdge (const TopoDS_Edge& E,
                  const Standard_Real aMaxTol);
static 
  void CorrectEdgeTolerance (const TopoDS_Edge& myShape,
                             const TopoDS_Face& S,
                             const Standard_Real aMaxTol);
static 
  Standard_Boolean Validate(const Adaptor3d_Curve& CRef,
                            const Adaptor3d_Curve& Other,
                            const Standard_Real Tol,
                            const Standard_Boolean SameParameter,
                            Standard_Real& aNewTolerance);

static
  void CorrectVertexTolerance(const TopoDS_Edge& aE);

static
  void CorrectWires(const TopoDS_Face& aF);

static 
  Standard_Real IntersectCurves2d(const gp_Pnt& aPV,
                                  const TopoDS_Face& aF,
                                  const TopoDS_Edge& aE1,
                                  const TopoDS_Edge& aE2);

static
  void UpdateEdges(const TopoDS_Face& aF);

static
  void UpdateVertices(const TopoDS_Edge& aE);

//=======================================================================
// Function : CorrectTolerances
// purpose : 
//=======================================================================
  void BOPTools_AlgoTools::CorrectTolerances(const TopoDS_Shape& aShape,
                                             const Standard_Real aMaxTol)
{
  BOPTools_AlgoTools::CorrectPointOnCurve(aShape, aMaxTol);
  BOPTools_AlgoTools::CorrectCurveOnSurface(aShape, aMaxTol);
}

//=======================================================================
// Function : CorrectPointOnCurve
// purpose : 
//=======================================================================
  void BOPTools_AlgoTools::CorrectPointOnCurve(const TopoDS_Shape& S,
                                               const Standard_Real aMaxTol)
{
  Standard_Integer i, aNb;
  TopTools_IndexedMapOfShape Edges;
  TopExp::MapShapes (S, TopAbs_EDGE, Edges);
  aNb=Edges.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Edge& E= TopoDS::Edge(Edges(i));
    CheckEdge(E, aMaxTol);
  }     
}

//=======================================================================
// Function : CorrectCurveOnSurface
// purpose : 
//=======================================================================
  void BOPTools_AlgoTools::CorrectCurveOnSurface(const TopoDS_Shape& S,
                                                 const Standard_Real aMaxTol)
{
  Standard_Integer i, aNbFaces, j, aNbEdges;
  TopTools_IndexedMapOfShape Faces;
  TopExp::MapShapes (S, TopAbs_FACE, Faces);
  
  aNbFaces=Faces.Extent();
  for (i=1; i<=aNbFaces; i++) {
    const TopoDS_Face& F= TopoDS::Face(Faces(i));
    //
    CorrectWires(F);
    //
    TopTools_IndexedMapOfShape Edges;
    TopExp::MapShapes (F, TopAbs_EDGE, Edges);
    aNbEdges=Edges.Extent();
    for (j=1; j<=aNbEdges; j++) {
      const TopoDS_Edge& E= TopoDS::Edge(Edges(j));
      CorrectEdgeTolerance (E, F, aMaxTol);
    }
  }
}
//=======================================================================
// Function : CorrectWires
// purpose : 
//=======================================================================
void CorrectWires(const TopoDS_Face& aFx)
{
  Standard_Integer i, aNbV;
  Standard_Real aTol, aTol2, aD2, aD2max, aT1, aT2, aT, aTol2d; 
  gp_Pnt aP, aPV;
  gp_Pnt2d aP2D;
  TopoDS_Face aF;
  BRep_Builder aBB;
  TopTools_IndexedDataMapOfShapeListOfShape aMVE;
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  //
  aF=aFx;
  aF.Orientation(TopAbs_FORWARD);
  aTol2d = Precision::Confusion();
  const Handle(Geom_Surface)& aS=BRep_Tool::Surface(aFx);
  //
  TopExp::MapShapesAndAncestors(aF, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
  aNbV=aMVE.Extent();
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Vertex& aV=TopoDS::Vertex(aMVE.FindKey(i));
    aPV=BRep_Tool::Pnt(aV);
    aTol=BRep_Tool::Tolerance(aV);
    aTol2=aTol*aTol;
    //
    aD2max=-1.;
    const TopTools_ListOfShape& aLE=aMVE.FindFromIndex(i);
    aIt.Initialize(aLE);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Edge& aE=*(TopoDS_Edge*)(&aIt.Value());
      aT=BRep_Tool::Parameter(aV, aE);
      const Handle(Geom2d_Curve)& aC2D=BRep_Tool::CurveOnSurface(aE, aF, aT1, aT2);
      aC2D->D0(aT, aP2D);
      aS->D0(aP2D.X(), aP2D.Y(), aP);
      aD2=aPV.SquareDistance(aP);
      if (aD2>aD2max) {
        aD2max=aD2;
      }
      //check self interference
      if (aNbV == 2) {
        continue;
      }
      aIt1 = aIt;
      aIt1.Next();
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Edge& aE1=*(TopoDS_Edge*)(&aIt1.Value());
        //do not perform check for edges that have two common vertices
        {
          TopoDS_Vertex aV11, aV12, aV21, aV22;
          TopExp::Vertices(aE, aV11, aV12);
          TopExp::Vertices(aE1, aV21, aV22);
          if ((aV11.IsSame(aV21) && aV12.IsSame(aV22)) ||
              (aV12.IsSame(aV21) && aV11.IsSame(aV22))) {
            continue;
          }
        }
        aD2 = IntersectCurves2d(aPV, aF, aE, aE1);
        if (aD2>aD2max) {
          aD2max=aD2;
        }
      }
    }
    if (aD2max>aTol2) {
      aTol=sqrt(aD2max);
      aBB.UpdateVertex(aV, aTol);
    }
  }
}
//=======================================================================
// Function : IntersectCurves2d
// purpose  : Intersect 2d curves of edges
//=======================================================================
Standard_Real IntersectCurves2d(const gp_Pnt& aPV,
                                const TopoDS_Face& aF,
                                const TopoDS_Edge& aE1,
                                const TopoDS_Edge& aE2)
{
  const Handle(Geom_Surface)& aS=BRep_Tool::Surface(aF);
  GeomAdaptor_Surface aGAS (aS);
  if (aGAS.IsUPeriodic() || aGAS.IsVPeriodic()) {
    return 0;
  }
  //
  Standard_Real aDist, aD, aT11, aT12, aT21, aT22, aTol2d;
  Standard_Integer j, aNbPnt;
  Geom2dInt_GInter aInter;
  gp_Pnt aP;
  gp_Pnt2d aP2D;
  //
  aDist = 0.;
  aTol2d = Precision::Confusion();
  //
  const Handle(Geom2d_Curve)& aC2D1=BRep_Tool::CurveOnSurface(aE1, aF, aT11, aT12);
  const Handle(Geom2d_Curve)& aC2D2=BRep_Tool::CurveOnSurface(aE2, aF, aT21, aT22);
  //
  Geom2dAdaptor_Curve aGAC1(aC2D1), aGAC2(aC2D2);
  IntRes2d_Domain aDom1(aC2D1->Value(aT11), aT11, aTol2d, aC2D1->Value(aT12), aT12, aTol2d),
                  aDom2(aC2D2->Value(aT21), aT21, aTol2d, aC2D2->Value(aT22), aT22, aTol2d);
  //
  aInter.Perform(aGAC1, aDom1, aGAC2, aDom2, aTol2d, aTol2d);
  if (aInter.IsDone()) {
    if (aInter.NbSegments()) {
      return aDist;
    }
    aNbPnt = aInter.NbPoints();
    if (aNbPnt) {
      aDist = Precision::Infinite();
      for (j = 1; j <= aNbPnt; ++j) {
        aP2D = aInter.Point(j).Value();
        aS->D0(aP2D.X(), aP2D.Y(), aP);
        aD=aPV.SquareDistance(aP);
        if (aD < aDist) {
          aDist = aD;
        }
      }
    }
  }
  return aDist;
}

//=======================================================================
// Function : CorrectEdgeTolerance
// purpose :  Correct tolerances for Edge 
//=======================================================================
void CorrectEdgeTolerance (const TopoDS_Edge& myShape, 
                           const TopoDS_Face& S,
                           const Standard_Real aMaxTol)
{
  // 
  // 1. Minimum of conditions to Perform
  Handle (BRep_CurveRepresentation) myCref;
  Handle (Adaptor3d_HCurve) myHCurve;

  myCref.Nullify();

  Handle(BRep_TEdge)& TEx = *((Handle(BRep_TEdge)*)&myShape.TShape());
  BRep_ListIteratorOfListOfCurveRepresentation itcrx(TEx->Curves());
  Standard_Boolean Degenerated, SameParameterx, SameRangex;

  Standard_Integer unique = 0;

  Degenerated    = TEx->Degenerated();
  SameParameterx = TEx->SameParameter();
  SameRangex     = TEx->SameRange();
  
  if (!SameRangex && SameParameterx) {
    return;
  }

  Handle(Geom_Curve) C3d;
  while (itcrx.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcrx.Value();
    if (cr->IsCurve3D()) {
      unique++;
      if (myCref.IsNull() && !cr->Curve3D().IsNull()) {
        myCref = cr;
      }
    }
    itcrx.Next();
  }
  
  if (unique==0) {
    return;//...No3DCurve
  }
  if (unique>1) {
    return;//...Multiple3DCurve;
  }

  if (myCref.IsNull() && !Degenerated) {
    itcrx.Initialize(TEx->Curves());
    while (itcrx.More()) {
      const Handle(BRep_CurveRepresentation)& cr = itcrx.Value();
      if (cr->IsCurveOnSurface()) {
        myCref = cr;
        break;
      }
      itcrx.Next();
    }
  }
  
  else if (!myCref.IsNull() && Degenerated){
    return ;//...InvalidDegeneratedFlag;
  }
  
  if (!myCref.IsNull()) {
    const Handle(BRep_GCurve)& GCref = *((Handle(BRep_GCurve)*)&myCref);
    Standard_Real First,Last;
    GCref->Range(First,Last);
    if (Last<=First) {
      myCref.Nullify();
      return ;//InvalidRange;
    }
    
    else {
      if (myCref->IsCurve3D()) {
        Handle(Geom_Curve) C3dx = Handle(Geom_Curve)::DownCast
          (myCref->Curve3D()->Transformed (myCref->Location().Transformation()));
        GeomAdaptor_Curve GAC3d(C3dx, First, Last);
        myHCurve = new GeomAdaptor_HCurve(GAC3d);
      }
      else { // curve on surface
        Handle(Geom_Surface) Sref = myCref->Surface();
        Sref = Handle(Geom_Surface)::DownCast(Sref->Transformed(myCref->Location().Transformation()));
        const  Handle(Geom2d_Curve)& PCref = myCref->PCurve();
        Handle(GeomAdaptor_HSurface) GAHSref = new GeomAdaptor_HSurface(Sref);
        Handle(Geom2dAdaptor_HCurve) GHPCref = new Geom2dAdaptor_HCurve(PCref, First, Last);
        Adaptor3d_CurveOnSurface ACSref(GHPCref,GAHSref);
        myHCurve = new Adaptor3d_HCurveOnSurface(ACSref);
      }
    }
  }

  //=============================================== 
  // 2. Tolerances in InContext
  {
    if (myCref.IsNull()) 
      return;
    Standard_Boolean ok=Standard_True;;

    Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&myShape.TShape());
    Standard_Real Tol = BRep_Tool::Tolerance(TopoDS::Edge(myShape));
    Standard_Real aNewTol=Tol;

    Standard_Boolean SameParameter = TE->SameParameter();
    Standard_Boolean SameRange = TE->SameRange();
    Standard_Real First = myHCurve->FirstParameter();
    Standard_Real Last  = myHCurve->LastParameter();
    Standard_Real Delta =1.e-12;

    Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*) &S.TShape());
    const TopLoc_Location& Floc = S.Location();
    const TopLoc_Location& TFloc = TF->Location();
    const Handle(Geom_Surface)& Su = TF->Surface();
    TopLoc_Location L = (Floc * TFloc).Predivided(myShape.Location());
    Standard_Boolean pcurvefound = Standard_False;

    BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
    while (itcr.More()) {
      const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
      if (cr != myCref && cr->IsCurveOnSurface(Su,L)) {
        pcurvefound = Standard_True;
        const Handle(BRep_GCurve)& GC = *((Handle(BRep_GCurve)*)&cr);
        Standard_Real f,l;
        GC->Range(f,l);
        if (SameRange && (f != First || l != Last)) {
          return ;//BRepCheck_InvalidSameRangeFlag;
        }
	
        Handle(Geom_Surface) Sb = cr->Surface();
        Sb = Handle(Geom_Surface)::DownCast (Su->Transformed(L.Transformation()));
        Handle(Geom2d_Curve) PC = cr->PCurve();
        Handle(GeomAdaptor_HSurface) GAHS = new GeomAdaptor_HSurface(Sb);
        Handle(Geom2dAdaptor_HCurve) GHPC = new Geom2dAdaptor_HCurve(PC,f,l);
        Adaptor3d_CurveOnSurface ACS(GHPC,GAHS);
        ok = Validate(myHCurve->Curve(), ACS, Tol, SameParameter, aNewTol);
        if (ok) {
          if (cr->IsCurveOnClosedSurface()) {
            //return ;// BRepCheck::Add(lst,BRepCheck_InvalidCurveOnClosedSurface);
          }
          else {
            //return;//BRepCheck::Add(lst,BRepCheck_InvalidCurveOnSurface);
          }
          if (SameParameter) {
            //return;//BRepCheck::Add(lst,BRepCheck_InvalidSameParameterFlag);
          }
          //
          if (aNewTol<aMaxTol) {
            TE->UpdateTolerance(aNewTol+Delta); 
            //
            CorrectVertexTolerance(myShape);
          }
        }
        
        if (cr->IsCurveOnClosedSurface()) {
          //checkclosed = Standard_True;
          GHPC->ChangeCurve2d().Load(cr->PCurve2(),f,l); // same bounds
          ACS.Load(GAHS); // sans doute inutile
          ACS.Load(GHPC); // meme remarque...
          ok = Validate(myHCurve->Curve(),ACS,Tol,SameParameter, aNewTol);
          if (ok) {
            //return;//BRepCheck::Add(lst,BRepCheck_InvalidCurveOnClosedSurface);
            if (SameParameter) {
              //return;//BRepCheck::Add(lst,BRepCheck_InvalidSameParameterFlag);
            }
            if (aNewTol<aMaxTol) {
              TE->UpdateTolerance(aNewTol+Delta);
              CorrectVertexTolerance(myShape);
            } 
          }
        }
      }
      itcr.Next();
    }
    
    if (!pcurvefound) {
      Handle(Geom_Plane) P;
      Handle(Standard_Type) styp = Su->DynamicType();
      if (styp == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
        P = Handle(Geom_Plane)::DownCast(Handle(Geom_RectangularTrimmedSurface)::
                                         DownCast(Su)->BasisSurface());
      }
      else {
        P = Handle(Geom_Plane)::DownCast(Su);
      }
      if (P.IsNull()) { // not a plane
        return;//BRepCheck::Add(lst,BRepCheck_NoCurveOnSurface);
      }
      
      else {// on fait la projection a la volee, comme BRep_Tool
        P = Handle(Geom_Plane)::DownCast(P->Transformed(L.Transformation()));
        //on projette Cref sur ce plan
        Handle(GeomAdaptor_HSurface) GAHS = new GeomAdaptor_HSurface(P);
        
        // Dub - Normalement myHCurve est une GeomAdaptor_HCurve
        GeomAdaptor_Curve& Gac = Handle(GeomAdaptor_HCurve)::DownCast(myHCurve)->ChangeCurve();
        Handle(Geom_Curve) C3dx = Gac.Curve();
        Handle(Geom_Curve) ProjOnPlane = GeomProjLib::ProjectOnPlane
          (new Geom_TrimmedCurve(C3dx,First,Last), P, P->Position().Direction(), Standard_True);
        
        Handle(GeomAdaptor_HCurve) aHCurve = new GeomAdaptor_HCurve(ProjOnPlane);
        
        ProjLib_ProjectedCurve proj(GAHS,aHCurve);
        Handle(Geom2d_Curve) PC = Geom2dAdaptor::MakeCurve(proj);
        Handle(Geom2dAdaptor_HCurve) GHPC = 
          new Geom2dAdaptor_HCurve(PC, myHCurve->FirstParameter(), myHCurve->LastParameter());
        
        Adaptor3d_CurveOnSurface ACS(GHPC,GAHS);
        
        Standard_Boolean okx = Validate(myHCurve->Curve(),ACS,
                                        Tol,Standard_True, aNewTol); // voir dub...
        if (okx) {
          //return;//BRepCheck::Add(lst,BRepCheck_InvalidCurveOnSurface);
          if (aNewTol<aMaxTol) {
            TE->UpdateTolerance(aNewTol+Delta);
            CorrectVertexTolerance(myShape);
          }
        }
      }
      
    }//end of if (!pcurvefound) {
  } // end of  2. Tolerances in InContext

}

//=======================================================================
// Function : CorrectShapeTolerances
// purpose : 
//=======================================================================
  void BOPTools_AlgoTools::CorrectShapeTolerances(const TopoDS_Shape& aShape)
{
  TopExp_Explorer aExp;
  Standard_Integer aDim;
  //
  aDim=Dimension(aShape);
  if (aDim == 1) {
    aExp.Init(aShape, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Edge& aE = *(TopoDS_Edge*)&aExp.Current();
      UpdateVertices(aE);
    }
  } else {
    aExp.Init(aShape, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Face& aF = *(TopoDS_Face*)&aExp.Current();
      UpdateEdges(aF);
    }
  }
}

//=======================================================================
//function : CorrectVertexTolerance
//purpose  : 
//=======================================================================
void CorrectVertexTolerance(const TopoDS_Edge& aE)
{
  Standard_Integer k, aNbV;
  Standard_Real aTolE, aTolV;
  TopTools_IndexedMapOfShape aVMap;
  
  aTolE=BRep_Tool::Tolerance(aE);
  
  TopExp::MapShapes(aE, TopAbs_VERTEX, aVMap);
  aNbV=aVMap.Extent();
  for (k=1; k<=aNbV; ++k) {
    const TopoDS_Vertex& aV=TopoDS::Vertex(aVMap(k));
    aTolV=BRep_Tool::Tolerance(aV);
    if (aTolV<aTolE) {
      Handle(BRep_TVertex)& aTV = *((Handle(BRep_TVertex)*)&aV.TShape());
      aTV->UpdateTolerance(aTolE);
    }
  }
}

#define NCONTROL 23
//=======================================================================
//function : Validate
//purpose  : 
//=======================================================================
Standard_Boolean Validate(const Adaptor3d_Curve& CRef,
                          const Adaptor3d_Curve& Other,
                          const Standard_Real Tol,
                          const Standard_Boolean SameParameter,
                          Standard_Real& aNewTolerance)
{
  Standard_Real First, Last, MaxDistance, aD, Tol2;

  First = CRef.FirstParameter();
  Last  = CRef.LastParameter();
  MaxDistance = 0.;
  Tol2 = Tol*Tol;

  Standard_Integer i, aNC1=NCONTROL-1;

  Standard_Boolean aFlag=Standard_False;
  Standard_Boolean proj = (!SameParameter || 
                           First != Other.FirstParameter() ||
                           Last  != Other.LastParameter());
  //
  // 1. 
  if (!proj) {
    for (i = 0; i < NCONTROL; i++) {
      Standard_Real prm = ((aNC1-i)*First + i*Last)/aNC1;
      gp_Pnt pref   = CRef.Value(prm);
      gp_Pnt pother = Other.Value(prm);
      
      aD=pref.SquareDistance(pother);

      if (aD > Tol2) {
        if (aD>MaxDistance) {
          MaxDistance=aD;
        }
        aFlag=Standard_True;
      }
    }

    if (aFlag) {
      aNewTolerance=sqrt(MaxDistance);
    }
    return aFlag;
  }
  
  //
  // 2.
  else {
    Extrema_LocateExtPC refd,otherd;
    Standard_Real OFirst, OLast;
    OFirst = Other.FirstParameter();
    OLast  = Other.LastParameter();
    
    gp_Pnt pd  = CRef.Value(First);
    gp_Pnt pdo = Other.Value(OFirst);
    
    aD = pd.SquareDistance(pdo);
    if (aD > Tol2) {
      if (aD>MaxDistance) {
        MaxDistance=aD;
      }
      aFlag=Standard_True;
    }

    pd  = CRef.Value(Last);
    pdo = Other.Value(OLast);
    aD = pd.SquareDistance(pdo);
    if (aD > Tol2 && aD > MaxDistance) {
      MaxDistance=aD;
      aFlag=Standard_True;
    }

    refd.Initialize(CRef, First, Last, CRef.Resolution(Tol));
    otherd.Initialize(Other, OFirst, OLast, Other.Resolution(Tol));
    
    for (i = 2; i< aNC1; i++) {
      Standard_Real rprm = ((aNC1-i)*First + i*Last)/aNC1;
      gp_Pnt pref = CRef.Value(rprm);

      Standard_Real oprm = ((aNC1-i)*OFirst + i*OLast)/aNC1;
      gp_Pnt pother = Other.Value(oprm);

      refd.Perform(pother,rprm);
      if (!refd.IsDone() || refd.SquareDistance() > Tol2) {
        if (refd.IsDone()) {
          aD=refd.SquareDistance();
          if (aD > Tol2 && aD>MaxDistance) {
            aFlag=Standard_True;
            MaxDistance=aD;
          }
        }
      }

      otherd.Perform(pref,oprm);
      if (!otherd.IsDone() || otherd.SquareDistance() > Tol2) {
        
        if (otherd.IsDone()) {
          aD=otherd.SquareDistance();
          if (aD > Tol2 && aD>MaxDistance) {
            aFlag=Standard_True;
            MaxDistance=aD;
          }
        }
      }
    }
  }
  
  aD=sqrt (MaxDistance);
  aNewTolerance=aD;

  return aFlag;
  
}

//=======================================================================
// Function : CheckEdge
// purpose :  Correct tolerances for Vertices on Edge 
//=======================================================================
void CheckEdge (const TopoDS_Edge& Ed, const Standard_Real aMaxTol)
{
  TopoDS_Edge E=Ed;
  E.Orientation(TopAbs_FORWARD);

  gp_Pnt Controlp;
  
  TopExp_Explorer aVExp;
  aVExp.Init(E, TopAbs_VERTEX);
  for (; aVExp.More(); aVExp.Next()) {
    TopoDS_Vertex aVertex= TopoDS::Vertex(aVExp.Current());

    Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*) &aVertex.TShape());
    const gp_Pnt& prep = TV->Pnt();

    Standard_Real Tol, aD2, aNewTolerance, dd;

    Tol =BRep_Tool::Tolerance(aVertex);
    Tol = Max(Tol, BRep_Tool::Tolerance(E));
    dd=0.1*Tol;
    Tol*=Tol;

    const TopLoc_Location& Eloc = E.Location();
    BRep_ListIteratorOfListOfPointRepresentation itpr;
    
    Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&E.TShape());
    BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
    while (itcr.More()) {
      const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
      const TopLoc_Location& loc = cr->Location();
      TopLoc_Location L = (Eloc * loc).Predivided(aVertex.Location());
      
      if (cr->IsCurve3D()) {
        const Handle(Geom_Curve)& C = cr->Curve3D();
        if (!C.IsNull()) {
          itpr.Initialize(TV->Points());
          while (itpr.More()) {
            const Handle(BRep_PointRepresentation)& pr = itpr.Value();
            if (pr->IsPointOnCurve(C,L)) {
              Controlp = C->Value(pr->Parameter());
              Controlp.Transform(L.Transformation());
              aD2=prep.SquareDistance(Controlp);
              if (aD2 > Tol) {
                aNewTolerance=sqrt(aD2)+dd;
                if (aNewTolerance<aMaxTol)
                  TV->UpdateTolerance(aNewTolerance);
              }
            }
            itpr.Next();
          }
          
          TopAbs_Orientation orv = aVertex.Orientation();
          if (orv == TopAbs_FORWARD || orv == TopAbs_REVERSED) {
            const Handle(BRep_GCurve)& GC = *((Handle(BRep_GCurve)*)&cr);
            
            if (orv==TopAbs_FORWARD)
              Controlp = C->Value(GC->First());
            else 
              Controlp = C->Value(GC->Last());
            
            Controlp.Transform(L.Transformation());
            aD2=prep.SquareDistance(Controlp);
            
            if (aD2 > Tol) {
              aNewTolerance=sqrt(aD2)+dd;
              if (aNewTolerance<aMaxTol)
                TV->UpdateTolerance(aNewTolerance);
            }
          }
        }
      }
      itcr.Next();
    }
  }
}

//=======================================================================
// Function : UpdateVertices
// purpose : 
//=======================================================================
  void UpdateVertices(const TopoDS_Edge& aE)
{
  Standard_Real aTolE, aTolV;
  TopoDS_Iterator aItE;
  BRep_Builder aBB;
  //
  aTolE = BRep_Tool::Tolerance(aE);
  aItE.Initialize(aE);
  for (; aItE.More(); aItE.Next()) {
    const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&aItE.Value();
    aTolV = BRep_Tool::Tolerance(aV);
    if (aTolV < aTolE) {
      aBB.UpdateVertex(aV, aTolE);
    }
  }
}
//=======================================================================
// Function : UpdateEdges
// purpose : 
//=======================================================================
  void UpdateEdges(const TopoDS_Face& aF)
{
  Standard_Real aTolF, aTolE, aTolV;
  TopoDS_Iterator aItF, aItW, aItE;
  BRep_Builder aBB;
  //
  aTolE = aTolF = BRep_Tool::Tolerance(aF);
  aItF.Initialize(aF);
  for (; aItF.More(); aItF.Next()) {
    const TopoDS_Shape& aS = aItF.Value();
    if (aS.ShapeType()==TopAbs_WIRE) {
      aItW.Initialize(aS);
      for (; aItW.More(); aItW.Next()) {
        const TopoDS_Edge& aE = *(TopoDS_Edge*)&aItW.Value();
        aTolE = BRep_Tool::Tolerance(aE);
        if (aTolE < aTolF) {
          aBB.UpdateEdge(aE, aTolF);
          aTolE = aTolF;
        }
        UpdateVertices(aE);
      }
    }
    else {
      const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&aItF.Value();
      aTolV = BRep_Tool::Tolerance(aV);
      if (aTolV < aTolE) {
        aBB.UpdateVertex(aV, aTolF);
      }
    }
  }
}
