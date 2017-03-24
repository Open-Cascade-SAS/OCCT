// Created by: Peter KURNEV
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


#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_Range.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>

static
  void TreatCompound(const TopoDS_Shape& theC1, 
                     BOPCol_ListOfShape& theLSX);

//=======================================================================
// function: UpdateVertex
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::UpdateVertex
  (const TopoDS_Vertex& aVF,
   const TopoDS_Vertex& aNewVertex)
{
  Standard_Real aTolVF, aTolNewVertex, aDist, aDTol=1.e-12, aNewTol;
  //
  gp_Pnt aPVF=BRep_Tool::Pnt(aVF);
  gp_Pnt aPNewVertex=BRep_Tool::Pnt(aNewVertex);
  aTolVF=BRep_Tool::Tolerance(aVF);
  aTolNewVertex=BRep_Tool::Tolerance(aNewVertex);

  aDist=aPVF.Distance(aPNewVertex);
  aNewTol=aDist+aTolNewVertex;

  if (aNewTol>aTolVF) {
    BRep_Builder BB;
    BB.UpdateVertex (aVF, aNewTol+aDTol);
  }
}

//=======================================================================
// function: UpdateVertex
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::UpdateVertex (const TopoDS_Edge& aE,
                                       const Standard_Real  aT,
                                       const TopoDS_Vertex& aV)
{
  Standard_Real aTolV, aDist, aDTol=1.e-12, aFirst, aLast;
  gp_Pnt  aPc; 

  gp_Pnt aPv=BRep_Tool::Pnt(aV);
  aTolV=BRep_Tool::Tolerance(aV);

  GeomAdaptor_Curve aCA( BRep_Tool::Curve(aE, aFirst, aLast) );
  aCA.D0(aT, aPc);
  aDist=aPv.Distance(aPc);
  if (aDist>aTolV) {
    BRep_Builder BB;
    BB.UpdateVertex (aV, aDist+aDTol);
  }
}
//
//=======================================================================
// function: UpdateVertex
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::UpdateVertex (const IntTools_Curve& aC,
                                       const Standard_Real  aT,
                                       const TopoDS_Vertex& aV)
{
  Standard_Real aTolV, aDist, aDTol=1.e-12;
  gp_Pnt  aPc; 

  gp_Pnt aPv=BRep_Tool::Pnt(aV);
  aTolV=BRep_Tool::Tolerance(aV);

  GeomAdaptor_Curve aCA( aC.Curve() );
  aCA.D0(aT, aPc);
  aDist=aPv.Distance(aPc);
  if (aDist>aTolV) {
    BRep_Builder BB;
    BB.UpdateVertex (aV, aDist+aDTol);
  }
}
//=======================================================================
// function: MakeSectEdge
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::MakeSectEdge(const IntTools_Curve& aIC,
                                      const TopoDS_Vertex& aV1,
                                      const Standard_Real  aP1,
                                      const TopoDS_Vertex& aV2,
                                      const Standard_Real  aP2,
                                      TopoDS_Edge& aNewEdge)
{
  Handle(Geom_Curve) aC=aIC.Curve ();
  
  BRepBuilderAPI_MakeEdge aMakeEdge(aC, aV1, aV2, aP1, aP2);
  
  const TopoDS_Edge& aE=TopoDS::Edge(aMakeEdge.Shape());
  //
  // Range must be as it was !
  BRep_Builder aBB;
  aBB.Range (aE, aP1, aP2);
  //
  aNewEdge=aE;
  
}

//=======================================================================
// function: MakeSplitEdge
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::MakeSplitEdge(const TopoDS_Edge&   aE,
                                       const TopoDS_Vertex& aV1,
                                       const Standard_Real  aP1,
                                       const TopoDS_Vertex& aV2,
                                       const Standard_Real  aP2,
                                       TopoDS_Edge& aNewEdge)
{
  Standard_Real aTol;//f, l, 
  aTol=BRep_Tool::Tolerance(aE);
  //
  TopoDS_Edge E = TopoDS::Edge(aE.Oriented(TopAbs_FORWARD));
  E.EmptyCopy();
  //
  BRep_Builder BB;
  if (!aV1.IsNull()) {
    BB.Add  (E, aV1);
  }
  if (!aV2.IsNull()) {
    BB.Add  (E, aV2);
  }
  BB.Range(E, aP1, aP2);
  BB.UpdateEdge(E, aTol);
  aNewEdge=E;
  aNewEdge.Orientation(aE.Orientation());
}

//=======================================================================
// function: MakeNewVertex
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::MakeNewVertex(const TopoDS_Vertex& aV1,
                                       const TopoDS_Vertex& aV2,
                                       TopoDS_Vertex& aNewVertex)
{
  gp_Pnt aPnt1=BRep_Tool::Pnt(aV1);
  Standard_Real aTol1=BRep_Tool::Tolerance(aV1);
        
  gp_Pnt aPnt2=BRep_Tool::Pnt(aV2);
  Standard_Real aTol2=BRep_Tool::Tolerance(aV2);

  Standard_Real aMaxTol, aDist;
        
  aDist=aPnt1.Distance(aPnt2);
  aMaxTol=(aTol1>aTol2)? aTol1 : aTol2;
  aMaxTol=aMaxTol+0.5*aDist;
        
  const gp_XYZ& aXYZ1=aPnt1.XYZ();
  const gp_XYZ& aXYZ2=aPnt2.XYZ();
  gp_XYZ aNewXYZ=0.5*(aXYZ1+aXYZ2);
        
  gp_Pnt aNewPnt(aNewXYZ);
  BRep_Builder aBB;
  aBB.MakeVertex (aNewVertex, aNewPnt, aMaxTol);
}
 //=======================================================================
// function: MakeNewVertex
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::MakeNewVertex(const gp_Pnt& aP,
                                       const Standard_Real aTol,
                                       TopoDS_Vertex& aNewVertex)
{
  BRep_Builder aBB;
  aBB.MakeVertex (aNewVertex, aP, aTol);
}

//=======================================================================
// function: MakeNewVertex
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::MakeNewVertex(const TopoDS_Edge& aE1,
                                       const Standard_Real aParm1,
                                       const TopoDS_Edge& aE2,
                                       const Standard_Real aParm2,
                                       TopoDS_Vertex& aNewVertex)
{
  Standard_Real aTol1, aTol2, aMaxTol, aDist; 
  gp_Pnt aPnt1, aPnt2;

  PointOnEdge (aE1, aParm1, aPnt1);
  PointOnEdge (aE2, aParm2, aPnt2);

  aTol1=BRep_Tool::Tolerance(aE1);
  aTol2=BRep_Tool::Tolerance(aE2);
  
  aDist=aPnt1.Distance(aPnt2);
  aMaxTol=(aTol1>aTol2)? aTol1 : aTol2;
  aMaxTol=aMaxTol+0.5*aDist;

  const gp_XYZ& aXYZ1=aPnt1.XYZ();
  const gp_XYZ& aXYZ2=aPnt2.XYZ();
  gp_XYZ aNewXYZ=0.5*(aXYZ1+aXYZ2);
        
  gp_Pnt aNewPnt(aNewXYZ);
  BRep_Builder aBB;
  aBB.MakeVertex (aNewVertex, aNewPnt, aMaxTol);
}
//=======================================================================
// function: MakeNewVertex
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::MakeNewVertex(const TopoDS_Edge& aE1,
                                       const Standard_Real aParm1,
                                       const TopoDS_Face& aF1,
                                       TopoDS_Vertex& aNewVertex)
{
  Standard_Real aTol1, aTol2, aMaxTol, delta=1.e-12; 
  gp_Pnt aPnt;

  PointOnEdge (aE1, aParm1, aPnt);

  aTol1=BRep_Tool::Tolerance(aE1);
  aTol2=BRep_Tool::Tolerance(aF1);
  //
  //aMaxTol=(aTol1>aTol2)? aTol1 : aTol2;
  aMaxTol=aTol1+aTol2+delta;
  //
  BRep_Builder aBB;
  aBB.MakeVertex (aNewVertex, aPnt, aMaxTol);
}

//=======================================================================
// function: PointOnEdge
// purpose: 
//=======================================================================
void BOPTools_AlgoTools::PointOnEdge(const TopoDS_Edge& aE,
                                     const Standard_Real aParm,
                                     gp_Pnt& aPnt)
{
  Standard_Real f, l;
  Handle(Geom_Curve) C1=BRep_Tool::Curve(aE, f, l);
  C1->D0(aParm, aPnt);
}

//=======================================================================
//function : CorrectRange
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools::CorrectRange(const TopoDS_Edge& aE1,
                                      const TopoDS_Edge& aE2,
                                      const IntTools_Range& aSR,
                                      IntTools_Range& aNewSR)
{
  Standard_Integer i;
  Standard_Real aRes, aTolE1, aTolE2, aTF, aTL, dT;
  BRepAdaptor_Curve aBC;
  GeomAbs_CurveType aCT;
  gp_Pnt aP;
  gp_Vec aDer;
  //
  aNewSR=aSR;
  //
  aBC.Initialize(aE1);
  aCT=aBC.GetType();
  if (aCT==GeomAbs_Line) {
    return;
  }
  //
  dT=Precision::PConfusion();
  aTF=aSR.First();
  aTL=aSR.Last();
  //
  aTolE1=BRep_Tool::Tolerance(aE1);
  aTolE2=BRep_Tool::Tolerance(aE2);
  //
  for(i=0; i<2; ++i) {
    aRes = 2.*(aTolE1 + aTolE2);
    //
    if (aCT==GeomAbs_BezierCurve ||
        aCT==GeomAbs_BSplineCurve||
        aCT==GeomAbs_OffsetCurve ||
        aCT==GeomAbs_OtherCurve) {
      
      if(!i){
        aBC.D1 (aTF, aP, aDer);
      }
      else {
        aBC.D1 (aTL, aP, aDer);
      }
      //
      Standard_Real aMgn = aDer.Magnitude();
      
      if(aMgn  > 1.e-12) {
        aRes = aRes/aMgn ;
      }
      else {
        aRes = aBC.Resolution(aRes);
      }
    } // if (aCT==GeomAbs_BezierCurve||...
    else {
      aRes = aBC.Resolution(aRes);
    }
    //
    if(!i) {
      aNewSR.SetFirst (aTF+aRes);
    }
    else {
      aNewSR.SetLast (aTL-aRes);
    }
    //
    if ((aNewSR.Last()-aNewSR.First()) < dT) {
      aNewSR=aSR;
    }
    //aNewSR=((aNewSR.Last()-aNewSR.First()) < dT) ? aSR : aNewSR;
  }
}
                                    
//=======================================================================
//function : CorrectRange
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools::CorrectRange(const TopoDS_Edge& aE,
                                      const TopoDS_Face& aF,
                                      const IntTools_Range& aSR,
                                      IntTools_Range& aNewSR)
{
  Standard_Integer i;
  Standard_Real aRes, aTolF, aTF, aTL, dT;
  BRepAdaptor_Curve aBC;
  GeomAbs_CurveType aCT;
  gp_Pnt aP;
  gp_Vec aDer;
  //
  aNewSR=aSR;
  //
  dT=Precision::PConfusion();
  aTF=aSR.First();
  aTL=aSR.Last();
  //
  aBC.Initialize(aE);
  aCT=aBC.GetType();
  //
  aTolF=BRep_Tool::Tolerance(aF);
  //
  for(i=0; i<2; ++i) {
    aRes =aTolF;

    if (aCT==GeomAbs_BezierCurve ||
        aCT==GeomAbs_BSplineCurve||
        aCT==GeomAbs_OffsetCurve ||
        aCT==GeomAbs_OtherCurve) {
      
      if(!i){
        aBC.D1 (aTF, aP, aDer);
      }
      else {
        aBC.D1 (aTL, aP, aDer);
      }
      //
      Standard_Real aMgn = aDer.Magnitude();
      
      if(aMgn  > 1.e-12) {
        aRes = aRes/aMgn ;
      }
      else {
        aRes = aBC.Resolution(aRes);
      }
    } // if (aCT==GeomAbs_BezierCurve||...
    else {
      aRes = aBC.Resolution(aRes);
    }
    //
    if(!i) {
      aNewSR.SetFirst (aTF+aRes);
    }
    else {
      aNewSR.SetLast (aTL-aRes);
    }
    //
    if ((aNewSR.Last()-aNewSR.First()) < dT) {
      aNewSR=aSR;
    }
  }
}
//=======================================================================
//function : Dimension
//purpose  : 
//=======================================================================
Standard_Integer BOPTools_AlgoTools::Dimension(const TopoDS_Shape& theS)
{
  Standard_Integer i, iRet, iRx0 = 0, iRx = 0;
  TopAbs_ShapeEnum aTS;
  BOPCol_ListOfShape aLS;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  aTS=theS.ShapeType();
  if (aTS!=TopAbs_COMPOUND) {
    switch (aTS) {
      case TopAbs_EDGE:
      case TopAbs_WIRE:
        iRet=1;
        break;
      case TopAbs_FACE:
      case TopAbs_SHELL:
        iRet=2;
        break;
      case TopAbs_SOLID:
      case TopAbs_COMPSOLID:
        iRet=3;
        break;
      default:
        iRet=0;
    }
    return iRet;
  }
  //
  iRet=-1;
  TreatCompound(theS, aLS);
  if(aLS.IsEmpty()) {
    iRet = -2; //empty compound
    return iRet;
  }
  aIt.Initialize(aLS);
  for (i=0; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value(); 
    iRx=Dimension(aSx);
    if (!i) {
      iRx0=iRx;
      i=1;
      continue;
    }
    if (iRx!=iRx0) {
      return iRet;// -1
    }
  }
  return iRx;
}

//=======================================================================
//function : TreatCompound
//purpose  : 
//=======================================================================
void TreatCompound(const TopoDS_Shape& theC1, 
                   BOPCol_ListOfShape& theLSX)
{
  Standard_Integer aNbC1;
  TopAbs_ShapeEnum aType;
  BOPCol_ListOfShape aLC, aLC1;
  BOPCol_ListIteratorOfListOfShape aIt, aIt1;
  TopoDS_Iterator aItC;
  //
  aLC.Append (theC1);
  for(;;) {
    aLC1.Clear();
    aIt.Initialize(aLC);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aC=aIt.Value(); //C is compound
      //
      aItC.Initialize(aC);
      for (; aItC.More(); aItC.Next()) {
        const TopoDS_Shape& aS=aItC.Value();
        aType=aS.ShapeType();
        if (aType==TopAbs_COMPOUND) {
          aLC1.Append(aS);
        }
        else {
          theLSX.Append(aS);
        }
      }
    }
    //
    aNbC1=aLC1.Extent();
    if (!aNbC1) {
      break;
    }
    //
    aLC.Clear();
    aIt.Initialize(aLC1);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aSC=aIt.Value();
      aLC.Append(aSC);
    }
  }// while(1)
}
