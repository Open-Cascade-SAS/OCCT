// Created on: 2001-04-05
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <BOPTools_Tools3D.ixx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <gp_Vec2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Line.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>

#include <GeomAdaptor_Surface.hxx>

#include <IntTools_Tools.hxx>

#include <BOPTools_Tools2D.hxx>

#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>



static
  void PointNearE (const TopoDS_Edge& aE,
		   const TopoDS_Face& aF,
		   const Standard_Real aT,
		   gp_Pnt& aPInFace,
		   const Standard_Boolean aSt);

static Standard_Boolean CheckKeepArguments(const TopoDS_Face& F1,
					   const TopoDS_Face& F2,
					   const TopoDS_Face& F3);

//=======================================================================
//function : RemoveSims
//purpose  : 
//=======================================================================
  void BOPTools_Tools3D::RemoveSims (const TopoDS_Shape& aS,
				     const Handle(IntTools_Context)& aContext)
{
  TopExp_Explorer anExp(aS, TopAbs_FACE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Face& aF=TopoDS::Face(anExp.Current());
    //
    BOPTools_Tools3D::RemoveSims (aF, aContext);
  }
}

//=======================================================================
//function : RemoveSims
//purpose  : 
//=======================================================================
  void BOPTools_Tools3D::RemoveSims (const TopoDS_Face& aFF,
				     const Handle(IntTools_Context)& aContext)
{
  Standard_Boolean anIsClosed, anIsPointInFace1, anIsPointInFace2;
  Standard_Real aT1, aT2, aT, aX, aY, dt=1.e-7, aTol;
  BRep_Builder BB;
  gp_Vec2d aV2D, aV2Dx;
  gp_Pnt2d aP2D, aP2Dx;
  TopAbs_Orientation anOri;
  Handle(Geom2d_Curve) aC2Dx;
  //
  TopoDS_Face aF=aFF;
  aF.Orientation(TopAbs_FORWARD);
  //
  TopExp_Explorer anExpW (aF, TopAbs_WIRE);
  for (; anExpW.More(); anExpW.Next()) {

    TopTools_MapOfShape aMap, aMapToAdd, aMapToRemove;
    const TopoDS_Shape& aW=anExpW.Current();

    TopExp_Explorer anExp(aW, TopAbs_EDGE);
    for (; anExp.More(); anExp.Next())   {
      const TopoDS_Edge& aE=TopoDS::Edge(anExp.Current());
      //
      if (BRep_Tool::Degenerated(aE)){
	continue;
      }
      //
      anIsClosed=BRep_Tool::IsClosed(aE, aF); 
      if (anIsClosed) {
	if (aMap.Contains(aE)) {
	  continue;
	}
	aMap.Add(aE);
	aTol=BRep_Tool::Tolerance(aE);
	
	Handle(Geom2d_Curve) aC2D=BRep_Tool::CurveOnSurface(aE, aF, aT1, aT2);
	aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
	aC2D-> D1(aT, aP2D, aV2D);
	
	anOri=aE.Orientation();
	if (anOri==TopAbs_REVERSED) {
	  aV2D.Reverse();
	}
	
	aV2D.Normalize();
	aX=aV2D.X();
	aY=aV2D.Y();
	aV2Dx.SetCoord(-aY, aX);
	
	aP2Dx.SetX(aP2D.X()+dt*aV2Dx.X());
	aP2Dx.SetY(aP2D.Y()+dt*aV2Dx.Y());
	//
	anIsPointInFace1=aContext->IsPointInFace(aF, aP2Dx);
	//
	aP2Dx.SetX(aP2D.X()-dt*aV2Dx.X());
	aP2Dx.SetY(aP2D.Y()-dt*aV2Dx.Y());
	//
	anIsPointInFace2=aContext->IsPointInFace(aF, aP2Dx);
	//
	
	if (anIsPointInFace1 && anIsPointInFace2) {
	  continue;
	}
	//
	TopoDS_Edge aEx=aE;
	
	aEx.EmptyCopy();
	TopExp_Explorer anExpV(aE, TopAbs_VERTEX);
	for (; anExpV.More(); anExpV.Next()) {
	  const TopoDS_Shape& aVx=anExpV.Current();
	  BB.Add (aEx, aVx);
	}
	
	BB.UpdateEdge(aEx, aTol);
	//
	if (anIsPointInFace1) {
	  if (anOri==TopAbs_REVERSED) {
	    // Remove Forward
	    BB.UpdateEdge(aEx, aC2Dx, aF, aTol);
	    BB.UpdateEdge(aEx, aC2D , aF, aTol);
	    aEx.Orientation(TopAbs_REVERSED);
	  }
	  else {
	    // Remove Reversed
	    BB.UpdateEdge(aEx, aC2Dx, aF, aTol);
	    BB.UpdateEdge(aEx, aC2D , aF, aTol);
	    aEx.Orientation(TopAbs_FORWARD);
	  }
	  aMapToAdd.Add(aEx);
	  aMapToRemove.Add(aE);
	}
      }
    }// next Edge
    //
    TopoDS_Shape* pW= (TopoDS_Shape*)&aW;
    pW->Free(Standard_True);

    TopTools_MapIteratorOfMapOfShape anIt(aMapToRemove);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aSR=anIt.Key();
      BB.Remove(*pW, aSR);
    }
    anIt.Initialize(aMapToAdd);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aSA=anIt.Key();
      BB.Add(*pW, aSA);
    }
    
  }// next Wire
}

//=======================================================================
//function : SubShapesAmount
//purpose  : 
//=======================================================================
  Standard_Integer BOPTools_Tools3D::SubShapesAmount (const TopoDS_Shape& aS,
						      const TopAbs_ShapeEnum aType)
{
  Standard_Integer aNb;
  TopTools_IndexedMapOfShape aM;
  TopExp::MapShapes(aS, aType, aM);
  aNb=aM.Extent();
  return aNb;
}

//=======================================================================
//function : IsConvexWire
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::IsConvexWire (const TopoDS_Wire& aW)
{
  Standard_Boolean aFlag;
  Standard_Integer aNbV, aNbE;

  aNbV=BOPTools_Tools3D::SubShapesAmount(aW, TopAbs_VERTEX);
  aNbE=BOPTools_Tools3D::SubShapesAmount(aW, TopAbs_EDGE);
  aFlag=aNbV <= aNbE;
  return aFlag;
}

//=======================================================================
//function : DoSplitSEAMOnFace
//purpose  : 
//=======================================================================
  void BOPTools_Tools3D::DoSplitSEAMOnFace (const TopoDS_Edge& aSplit,
					    const TopoDS_Face& aF)
{
  Standard_Boolean bIsUPeriodic, bIsLeft;
  Standard_Real aTol, a, b, anUPeriod, aT, anU, dU=1.e-7, anU1;
  Standard_Real aScPr;
  gp_Pnt2d aP2D;
  gp_Vec2d aVec2D;
  Handle(Geom2d_Curve) aTmpC1, aTmpC2;
  Handle(Geom2d_Curve) C2D1;
  Handle(Geom2d_Line) aLD1;
  Handle(Geom_Surface) aS;
  BRep_Builder BB;
  TopoDS_Edge aSp;
  //
  aSp=aSplit;
  aSp.Orientation(TopAbs_FORWARD);
  
  aTol=BRep_Tool::Tolerance(aSp);

  aS=BRep_Tool::Surface(aF);
  bIsUPeriodic=aS->IsUPeriodic();
  
  anUPeriod=0.;
  if (bIsUPeriodic) {
    anUPeriod=aS->UPeriod();
  }
  else {
    //modified by NIZNHY-PKV Fri Jul 11 09:54:43 2008f
    Standard_Boolean bIsUClosed;
    Standard_Real aUmin, aUmax, aVmin, aVmax;
    Handle(Geom_BSplineSurface) aBS;
    Handle(Geom_BezierSurface) aBZ;
    //
    bIsUClosed=Standard_False;
    aBS=Handle(Geom_BSplineSurface)::DownCast(aS);
    aBZ=Handle(Geom_BezierSurface) ::DownCast(aS);
    //
    if (!aBS.IsNull()) {
      bIsUClosed=aBS->IsUClosed();
      aBS->Bounds(aUmin, aUmax, aVmin, aVmax);
    }
    else if (!aBZ.IsNull()) {
      bIsUClosed=aBZ->IsUClosed();
      aBZ->Bounds(aUmin, aUmax, aVmin, aVmax);
    }
    if (!bIsUClosed) {
      return;
    }
    //
    anUPeriod=aUmax-aUmin;
    //modified by NIZNHY-PKV Fri Jul 11 09:54:49 2008t
  }
  //
  C2D1=BRep_Tool::CurveOnSurface(aSp, aF, a, b);
  //modified by NIZNHY-PKV Fri Jul 11 09:55:10 2008f
  /*
  aLD1=Handle(Geom2d_Line)::DownCast(C2D1);
  if (aLD1.IsNull()) {
    return;
  }
  */
  //modified by NIZNHY-PKV Fri Jul 11 09:55:14 2008t
  //
  aT=BOPTools_Tools2D::IntermediatePoint(a, b);
  C2D1->D1(aT, aP2D, aVec2D);
  gp_Dir2d aDir2D1(aVec2D);
  
  //
  gp_Dir2d aDOY(0.,1.);
  aScPr=aDir2D1*aDOY;
  //
  //
  anU=aP2D.X();
  if (fabs (anU) < dU) {
    bIsLeft=Standard_True;
    anU1=anU+anUPeriod;
  }
  else if (fabs (anU-anUPeriod) < dU) {
    bIsLeft=Standard_False;
    anU1=anU-anUPeriod;
  }
  else {
    return;
  }
  //
  
  
  aTmpC1=Handle(Geom2d_Curve)::DownCast(C2D1->Copy());
  Handle(Geom2d_TrimmedCurve) aC1 = new Geom2d_TrimmedCurve(aTmpC1, a, b);

  aTmpC2=Handle(Geom2d_Curve)::DownCast(C2D1->Copy());
  Handle(Geom2d_TrimmedCurve) aC2 = new Geom2d_TrimmedCurve(aTmpC2, a, b);
  gp_Vec2d aTrV(anU1-anU, 0.);
  aC2->Translate(aTrV);
  //
  if (!bIsLeft) {
    if (aScPr<0.) {
      BB.UpdateEdge(aSp, aC2, aC1, aF, aTol);
    }
    else {
      BB.UpdateEdge(aSp, aC1, aC2, aF, aTol);
    }
  }
  else {
    if (aScPr<0.) {
      BB.UpdateEdge(aSp, aC1, aC2, aF, aTol);
    }
    else {
      BB.UpdateEdge(aSp, aC2, aC1, aF, aTol);
    }
  }
  //
}
//=======================================================================
//function : DoSplitSEAMOnFace
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_Tools3D::DoSplitSEAMOnFace(const TopoDS_Edge& theSplit,
						     const TopoDS_Edge& theSeam,
						     const TopoDS_Face& theFace,
						     Standard_Boolean&  IsReversed) 
{
  Standard_Real aTol, f, l, a, b, aPeriod, aT, aParTolerance = 1.e-07;
  Standard_Boolean bIsUPeriodic = Standard_False;
  Standard_Boolean bIsVPeriodic = Standard_False;
  BRep_Builder BB;
  gp_Pnt2d aP2D;
  gp_Vec2d aVec2D;


  TopoDS_Edge aSp = theSplit;
  aSp.Orientation(TopAbs_FORWARD);
  TopoDS_Edge aSeamF = theSeam;
  aSeamF.Orientation(TopAbs_FORWARD);
  TopoDS_Edge aSeamR = theSeam;
  aSeamR.Orientation(TopAbs_REVERSED);

  aTol=BRep_Tool::Tolerance(aSp);

  Handle(Geom2d_Curve) C2D1 = BRep_Tool::CurveOnSurface(aSp, theFace, f, l);
  Handle(Geom2d_Curve) C2D2 = BRep_Tool::CurveOnSurface(aSeamF, theFace, a, b);
  Handle(Geom2d_Curve) C2D3 = BRep_Tool::CurveOnSurface(aSeamR, theFace, a, b);
  //
  Handle(Geom2d_TrimmedCurve) atr = Handle(Geom2d_TrimmedCurve)::DownCast(C2D1);
  Handle(Geom2d_Line) aLD1;

  if(!atr.IsNull()) {
    aLD1 = Handle(Geom2d_Line)::DownCast(atr->BasisCurve());
  }
  else {
    aLD1 = Handle(Geom2d_Line)::DownCast(C2D1);
  }

  if (aLD1.IsNull()) {
    return Standard_False;
  }
  aT = BOPTools_Tools2D::IntermediatePoint(f, l);
  C2D1->D1(aT, aP2D, aVec2D);
  gp_Dir2d aDir2D(aVec2D);

  gp_Vec2d aVec2D1, aVec2D2;
  gp_Pnt2d aP2D1, aP2D2;
  C2D2->D1(((a+b) * 0.5), aP2D1, aVec2D1);
  C2D3->D1(((a+b) * 0.5), aP2D2, aVec2D2);

  Handle(Geom_Surface) aS=BRep_Tool::Surface(theFace);
  bIsUPeriodic = aS->IsUPeriodic();
  bIsVPeriodic = aS->IsVPeriodic();

  for(Standard_Integer i = 0; i < 2; i++) {
    Standard_Boolean bIsPeriodic = (i == 0) ? bIsUPeriodic : bIsVPeriodic;

    if(!bIsPeriodic)
      continue;

    aPeriod = (i == 0) ? aS->UPeriod() : aS->VPeriod();

    Standard_Real aParameter = (i == 0) ? aP2D.X() : aP2D.Y();
    Standard_Real aParameter1 = (i == 0) ? aP2D1.X() : aP2D1.Y();
    Standard_Real aParameter2 = (i == 0) ? aP2D2.X() : aP2D2.Y();

    Standard_Boolean bIsLower = Standard_False, found = Standard_False;
    Standard_Boolean bIsFirst = Standard_False;
    Standard_Real aScPr = 0.;

    if (fabs (aParameter - aParameter1) < aParTolerance) {
      bIsLower = (aParameter < aParameter2);
      bIsFirst = Standard_True;
      aScPr = aDir2D * aVec2D1;
    }
    else if (fabs (aParameter - aParameter2) < aParTolerance) {
      bIsLower = (aParameter < aParameter1);
      bIsFirst = Standard_False;
      aScPr = aDir2D * aVec2D2;
    }
    found = (fabs(aScPr) > aParTolerance);

    if(found) {
      Handle(Geom2d_Curve) aTmpC1, aTmpC2;
  
      aTmpC1 = Handle(Geom2d_Curve)::DownCast(C2D1->Copy());
      Handle(Geom2d_TrimmedCurve) aC1 = new Geom2d_TrimmedCurve(aTmpC1, f, l);

      aTmpC2 = Handle(Geom2d_Curve)::DownCast(C2D1->Copy());
      Handle(Geom2d_TrimmedCurve) aC2 = new Geom2d_TrimmedCurve(aTmpC2, f, l);
      gp_Vec2d aTrV;

      if(i == 0) {
	if(bIsLower)
	  aTrV.SetX(aPeriod);
	else
	  aTrV.SetX(-aPeriod);
	aTrV.SetY(0.);
      }
      else {
	if(bIsLower)
	  aTrV.SetY(aPeriod);
	else
	  aTrV.SetY(-aPeriod);
	aTrV.SetX(0.);
      }
      aC2->Translate(aTrV);
      //
      IsReversed = (aScPr < 0.);
      Standard_Boolean bIsReverseOrder = (!IsReversed) ? !bIsFirst : bIsFirst;

      if(bIsReverseOrder) {
	BB.UpdateEdge(aSp, aC2, aC1, theFace, aTol);
      }
      else {
	BB.UpdateEdge(aSp, aC1, aC2, theFace, aTol);
      }
      return Standard_True;
    }
  }
  return Standard_False;
}
//=======================================================================
//function :IsSplitToReverse1
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::IsSplitToReverse1 (const TopoDS_Edge& aEF1,
							const TopoDS_Edge& aEF2,
							const Handle(IntTools_Context)& aContext)
{
  Standard_Boolean aFlag;
  Standard_Real aT1, aT2, aScPr, a, b;
  gp_Vec aV1, aV2;
  gp_Pnt aP;
  
  
  Handle(Geom_Curve)aC1=BRep_Tool::Curve(aEF1, a, b);
  aT1=BOPTools_Tools2D::IntermediatePoint(a, b);
  aC1->D0(aT1, aP);
  aFlag=BOPTools_Tools2D::EdgeTangent(aEF1, aT1, aV1);

  if(!aFlag) {
    return Standard_False;
  }

  gp_Dir aDT1(aV1);
  //
  aFlag=aContext->ProjectPointOnEdge(aP, aEF2, aT2);
  //
  aFlag=BOPTools_Tools2D::EdgeTangent(aEF2, aT2, aV2);
  if(!aFlag) {
    return Standard_False;
  }

  gp_Dir aDT2(aV2);

  aScPr=aDT1*aDT2;

  return (aScPr<0.);
}

//=======================================================================
//function : EdgeOrientation
//purpose  : 
//=======================================================================
  TopAbs_Orientation BOPTools_Tools3D::EdgeOrientation (const TopoDS_Edge& aE,
							const TopoDS_Face& aF1,
							const TopoDS_Face& aF2)
{
  Standard_Real aScPr;
  gp_Dir aDTE, aDNF1, aDNF2, aDTN;

  BOPTools_Tools3D::GetNormalToFaceOnEdge(aE, aF1, aDNF1);
  BOPTools_Tools3D::GetNormalToFaceOnEdge(aE, aF2, aDNF2);
  
  aDTN=aDNF1^aDNF2;
  
  BOPTools_Tools2D::TangentOnEdge(aE, aDTE);
  
  aScPr=aDTN*aDTE;

  TopAbs_Orientation anOr;

  anOr=TopAbs_FORWARD;
  if (aScPr<0.) {
    anOr=TopAbs_REVERSED;
  }
  return anOr;
}
//=======================================================================
//function : IsTouchCase
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::IsTouchCase(const TopoDS_Edge& aE,
						 const TopoDS_Face& aF1,
						 const TopoDS_Face& aF2)
{
  gp_Dir aDNF1, aDNF2;
  

  BOPTools_Tools3D::GetNormalToFaceOnEdge(aE, aF1, aDNF1);
  BOPTools_Tools3D::GetNormalToFaceOnEdge(aE, aF2, aDNF2);

  Standard_Boolean bIsDirsCoinside;
  bIsDirsCoinside=IntTools_Tools::IsDirsCoinside(aDNF1, aDNF2);
  return bIsDirsCoinside;
}
//=======================================================================
//function : GetNormalToFaceOnEdge
//purpose  : 
//=======================================================================
  void BOPTools_Tools3D::GetNormalToFaceOnEdge (const TopoDS_Edge& aE,
						const TopoDS_Face& aF,
						gp_Dir& aDNF)
{
  Standard_Real aT, aT1, aT2;
  
  BRep_Tool::CurveOnSurface(aE, aF, aT1, aT2);
  aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);

  BOPTools_Tools3D::GetNormalToFaceOnEdge (aE, aF, aT, aDNF);

  if (aF.Orientation()==TopAbs_REVERSED){
    aDNF.Reverse();
  }
}

//=======================================================================
//function : GetNormalToFaceOnEdge
//purpose  : 
//=======================================================================
  void BOPTools_Tools3D::GetNormalToFaceOnEdge (const TopoDS_Edge& aE,
						const TopoDS_Face& aF1,
						const Standard_Real aT, 
						gp_Dir& aDNF1)
{
  Standard_Real U, V, aTolPC;
  gp_Pnt2d aP2D;
  gp_Pnt aP;
  gp_Vec aD1U, aD1V;

  Handle(Geom_Surface) aS1=BRep_Tool::Surface(aF1);
  
  Handle(Geom2d_Curve)aC2D1;
  BOPTools_Tools2D::CurveOnSurface(aE, aF1, aC2D1, aTolPC, Standard_True);

  aC2D1->D0(aT, aP2D);
  U=aP2D.X();
  V=aP2D.Y();
  
  aS1->D1(U, V, aP, aD1U, aD1V);
  gp_Dir aDD1U(aD1U); 
  gp_Dir aDD1V(aD1V); 
  
  aDNF1=aDD1U^aDD1V; 
}

//=======================================================================
//function : GetBiNormal
//purpose  :
//=======================================================================
  void BOPTools_Tools3D::GetBiNormal(const TopoDS_Edge& aE,
				     const TopoDS_Face& aF,
				     const Standard_Real aT, 
				     gp_Dir& aDB)
{
  gp_Dir aDNF, aDT;
  //
  // Normal to the face aF at parameter aT
  BOPTools_Tools3D::GetNormalToFaceOnEdge (aE, aF, aT, aDNF);
  if (aF.Orientation()==TopAbs_REVERSED){
    aDNF.Reverse();
  }
  //
  // Split tangent on aF at parameter aT
  BOPTools_Tools3D::GetTangentToEdge(aE, aT, aDT);

  if (aF.Orientation()==TopAbs_REVERSED){
    aDT.Reverse();
  }
  // Binormal 
  aDB=aDNF^aDT;
}

//=======================================================================
//function : :GetBiNormal
//purpose  :
//=======================================================================
  void BOPTools_Tools3D::GetBiNormal(const TopoDS_Edge& aSp,
				     const TopoDS_Face& aF,
				     gp_Dir& aDB)
{
  gp_Dir aDNF, aDD1Sp;
  //
  // Normal to the face aF at parameter aT
  BOPTools_Tools3D::GetNormalToFaceOnEdge (aSp, aF, aDNF);
  //
  // Split tangent on aF at parameter aT
  BOPTools_Tools3D::GetTangentToEdge(aSp, aDD1Sp);

  if (aF.Orientation()==TopAbs_REVERSED){
    aDD1Sp.Reverse();
  }
  // Binormal 
  aDB=aDNF^aDD1Sp;
}
//=======================================================================
//function : GetTangentToEdge
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::GetTangentToEdge(const TopoDS_Edge& anEdge, 
						      gp_Dir& aDT)
{
  Standard_Boolean isdgE;
  
  Standard_Real aT1, aT2, aT;
  
  isdgE = BRep_Tool::Degenerated(anEdge); 
  if (isdgE) {
    return Standard_False;
  }
  
  Handle(Geom_Curve) aC=BRep_Tool::Curve(anEdge, aT1, aT2);
  aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);

  BOPTools_Tools3D::GetTangentToEdge(anEdge, aT, aDT);

  return Standard_True;
}
//=======================================================================
//function : GetTangentToEdge
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::GetTangentToEdge(const TopoDS_Edge& anEdge, 
						      const Standard_Real aT,
						      gp_Dir& aDT)
{
  Standard_Boolean isdgE;
  Standard_Real first, last;
  
  isdgE = BRep_Tool::Degenerated(anEdge); 
  if (isdgE) {
    return Standard_False;
  }

  Handle(Geom_Curve) aC=BRep_Tool::Curve(anEdge, first, last);
  gp_Pnt aP;
  gp_Vec aTau;
  aC->D1(aT, aP, aTau);
  gp_Dir aD(aTau);
  if (anEdge.Orientation() == TopAbs_REVERSED){
    aD.Reverse();
  }
  aDT=aD;

  return Standard_True;
}
//=======================================================================
//function : :IsSplitToReverse
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_Tools3D:: IsSplitToReverse(const TopoDS_Edge& aE,
						       const TopoDS_Edge& aSp)
{
  Standard_Real t1, t2, aT;
  
  Handle(Geom_Curve)aCE=BRep_Tool::Curve(aE, t1, t2);
  Handle(Geom_Curve)aCSp=BRep_Tool::Curve(aSp,t1, t2);
  
  aT=BOPTools_Tools2D::IntermediatePoint(t1, t2);
  
  gp_Vec aD1E, aD1Sp;
  gp_Pnt aP;
  
  aCE->D1 (aT, aP, aD1E);
  aCSp->D1(aT, aP, aD1Sp);
  
  gp_Dir aDD1E (aD1E); 
  gp_Dir aDD1Sp(aD1Sp); 
  
  if (aE.Orientation()==TopAbs_REVERSED) {
    aDD1E.Reverse();
  }
  if (aSp.Orientation()==TopAbs_REVERSED) {
    aDD1Sp.Reverse();
  }
  
  aT=aDD1E*aDD1Sp;
  
  return (aT<0.); 
}

//=======================================================================
//function : GetAdjacentFace
//purpose  :
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::GetAdjacentFace(const TopoDS_Face& aFaceObj,
						     const TopoDS_Edge& anEObj,
		 const TopTools_IndexedDataMapOfShapeListOfShape& anEdgeFaceMap,
						     TopoDS_Face& anAdjF)
{
  const TopTools_ListOfShape& aListOfAdjFaces=anEdgeFaceMap.FindFromKey(anEObj);
  TopTools_ListIteratorOfListOfShape anIt(aListOfAdjFaces);
  TopoDS_Shape anAdjShape;
  for (; anIt.More(); anIt.Next()) {
    if (anIt.Value()!=aFaceObj) {
      anAdjShape=anIt.Value();
      break;
    }
  }

  if (!anAdjShape.IsNull()) {
    anAdjF=TopoDS::Face(anAdjShape);
    return Standard_True;
  }
  else {
    return Standard_False;
  }
}
//=======================================================================
//function :  IsKeepTwice
//purpose  :
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::IsKeepTwice(const TopoDS_Face& aF1,
						 const TopoDS_Face& aF2,
						 const TopoDS_Face& aF2Adj,
						 const TopoDS_Edge& aSpEF2)
{
  if( !CheckKeepArguments(aF1, aF2, aF2Adj) ) {
    return Standard_False;
  }

  Standard_Real aT1, aT2, aT, dt=1.e-7, A, B, C, D, d2, d2Adj;
  gp_Dir aDNF1, aDNF2, DBF2, aDNF2Adj, DBF2Adj;
  gp_Vec aD1Sp;
  gp_Pnt aP, aPF2, aPF2Adj;

  Handle(Geom_Curve) aC3D=BRep_Tool::Curve(aSpEF2, aT1, aT2);
  aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
  BOPTools_Tools3D::GetNormalToFaceOnEdge (aSpEF2, aF1, aT, aDNF1);
  
  //
  // Split tangent on F2
  aC3D->D1(aT, aP, aD1Sp);
  gp_Dir aDD1Sp(aD1Sp); 
  
  if (aSpEF2.Orientation()==TopAbs_REVERSED) {
    aDD1Sp.Reverse();
  }
  // Split Normal on F2
  BOPTools_Tools3D::GetNormalToFaceOnEdge (aSpEF2, aF2, aT, aDNF2);
  if (aF2.Orientation()==TopAbs_REVERSED) {
    aDNF2.Reverse();
  }
  // Binormal on F2
  DBF2=aDNF2^aDD1Sp;
  
  // Point near aP
  aPF2.SetCoord(aP.X()+dt*DBF2.X(), 
		aP.Y()+dt*DBF2.Y(),
		aP.Z()+dt*DBF2.Z());
		
  
  //
  // Split tangent on F2Adj
  aDD1Sp.Reverse();
  // Split Normal on F2Adj
  BOPTools_Tools3D::GetNormalToFaceOnEdge (aSpEF2, aF2Adj, aT, aDNF2Adj);
  if (aF2Adj.Orientation()==TopAbs_REVERSED) {
    aDNF2Adj.Reverse();
  }
  // Binormal on F2Adj
  DBF2Adj=aDNF2Adj^aDD1Sp;
  
  // Point near aP
  aPF2Adj.SetCoord(aP.X()+dt*DBF2Adj.X(), 
		   aP.Y()+dt*DBF2Adj.Y(),
		   aP.Z()+dt*DBF2Adj.Z());
  //
  // Tangent Plane on F1
  gp_Pln aPlnN1(aP, aDNF1);
  aPlnN1.Coefficients(A, B, C, D);
  //
  d2   = A*aPF2.X()    + B*aPF2.Y()   + C*aPF2.Z()    + D;
  d2Adj= A*aPF2Adj.X() + B*aPF2Adj.Y()+ C*aPF2Adj.Z() + D;
  //
  if (fabs(d2)<1.e-10) {
    d2=0.;
  }
  if (fabs(d2Adj)<1.e-10) {
    d2Adj=0.;
  }
  //
  aT=d2*d2Adj;
  //
  return  (aT >= 0.);
}

//=======================================================================
//function : SenseFlag
//purpose  :
//=======================================================================
  Standard_Integer BOPTools_Tools3D::SenseFlag (const gp_Dir& aDNF1,
						const gp_Dir& aDNF2)
{
  Standard_Boolean bIsDirsCoinside;
  bIsDirsCoinside=IntTools_Tools::IsDirsCoinside(aDNF1, aDNF2);
  if (!bIsDirsCoinside) {
    return 0;
  }
  
  Standard_Real aScPr;
  
  aScPr=aDNF1*aDNF2;
  if (aScPr<0.) {
    return -1;
  }
  else if (aScPr>0.) {
    return 1;
  }
  return -1;
}

//=======================================================================
//function : GetNormalToSurface
//purpose  :
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::GetNormalToSurface (const Handle(Geom_Surface)& aS,
							 const Standard_Real U,
							 const Standard_Real V,
							 gp_Dir& aDNS)
{
  Standard_Boolean bFlag;
  
  gp_Pnt aP;
  gp_Vec aD1U, aD1V;

  aS->D1(U, V, aP, aD1U, aD1V);
  
  gp_Dir aDD1U(aD1U); 
  gp_Dir aDD1V(aD1V); 
  
  bFlag=IntTools_Tools::IsDirsCoinside(aDD1U, aDD1U);
  if (!bFlag) {
    return bFlag;
  }
  
  aDNS=aDD1U^aDD1V;
  return bFlag;
}

//=======================================================================
//function : GetNormalToSurface
//purpose  : local modification
//=======================================================================

static void GetApproxNormalToFaceOnEdgeEx(const TopoDS_Edge& aE,
					  const TopoDS_Face& aF,
					  const Standard_Real aT,
					  const Standard_Real aDT,
					  gp_Pnt& aPNear,
					  gp_Dir& aDNF)
{
  Standard_Real aFirst, aLast;
  Handle(Geom2d_Curve) aC2D= BRep_Tool::CurveOnSurface (aE, aF, aFirst, aLast);
  
  if (aC2D.IsNull()) {
    return;
  }

  //gp_Pnt aPNear;
  gp_Pnt2d aPx2DNear;
  BOPTools_Tools3D::PointNearEdge (aE, aF, aT, aDT, aPx2DNear, aPNear);
  
  Handle(Geom_Surface) aS=BRep_Tool::Surface(aF);
  
  BOPTools_Tools3D::GetNormalToSurface (aS, aPx2DNear.X(), aPx2DNear.Y(), aDNF);
  
  if (aF.Orientation()==TopAbs_REVERSED){
    aDNF.Reverse();
  }
}

//=======================================================================
//function : GetPlanes
//purpose  :
//=======================================================================
  void BOPTools_Tools3D::GetPlanes (const TopoDS_Edge& aSpx,
				    const TopoDS_Edge& anEx,
				    const TopTools_IndexedDataMapOfShapeListOfShape& anEFMapx,
				    const TopoDS_Edge& anE1,
				    const TopoDS_Face& aF1,
				    TopAbs_State& aStPF1,
				    const Handle(IntTools_Context)& aContext)
{
  Standard_Boolean bIsAdjExists;

  Standard_Real aT, aT1, aT2;
  gp_Dir aDNFx1, aDNFx2; 
  gp_Pnt aPx, aPx1, aPx2, aPF1;
  TopoDS_Face aFx1, aFx2;
  //
  // Point
  Handle(Geom_Curve)aC3D =BRep_Tool::Curve(aSpx, aT1, aT2);
  aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
  
  aC3D->D0(aT, aPx);
  //
  // 1.1. Fx1, Fx2 and theirs normals
  TopAbs_Orientation anOrEx, anOr;
  anOrEx=anEx.Orientation();

  TopoDS_Edge aSpxSimm=anEx;
  if (anOrEx==TopAbs_FORWARD) {
    aSpxSimm.Orientation(TopAbs_REVERSED);
  }
  else if (anOrEx==TopAbs_REVERSED){
    aSpxSimm.Orientation(TopAbs_FORWARD);
  }
  //
  
  const TopTools_ListOfShape& aLF=anEFMapx.FindFromKey(anEx);
  TopTools_ListIteratorOfListOfShape anIt(aLF);
  
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aFE=anIt.Value();
    aFx1=TopoDS::Face(aFE);
    anOr=BOPTools_Tools3D::Orientation(anEx, aFx1);
    if (anOr==anOrEx){
      break;
    }
  }
  
  //-- EJG
  Standard_Boolean aMoreShift = Standard_False;
  Standard_Real aF2Tol = BRep_Tool::Tolerance(aFx1);
  Standard_Real aF1Tol = BRep_Tool::Tolerance(aF1);
  Standard_Real aETol = BRep_Tool::Tolerance(anEx);
  if( aETol > 1.e-5 && (aF2Tol > 1.e-5 && aF1Tol > 1.e-5) ) 
     aMoreShift = Standard_True;
  Standard_Real aDT = 9.1e-5;
  if( aMoreShift ) {
    GetApproxNormalToFaceOnEdgeEx(anEx, aFx1, aT, aDT, aPx1, aDNFx1);
  }
  else {
    //-- EJG
    BOPTools_Tools3D::GetApproxNormalToFaceOnEdge (anEx, aFx1, aT, aPx1, aDNFx1);
  }

  bIsAdjExists=BOPTools_Tools3D::GetAdjacentFace (aFx1, anEx, anEFMapx, aFx2);

  if (!bIsAdjExists) {
    //-- EJG
    if(  aMoreShift ) {
      GetApproxNormalToFaceOnEdgeEx(aSpxSimm, aFx1, aT, aDT, aPx2, aDNFx2);
    }
    else {
      //-- EJG
      BOPTools_Tools3D::GetApproxNormalToFaceOnEdge (aSpxSimm, aFx1, aT, aPx2, aDNFx2); 
    }

    aContext->ProjectPointOnEdge(aPx, anE1, aT1);
    PointNearE (anE1, aF1, aT1, aPF1, aMoreShift);
  }

  else {// if (bIsAdjExists)
    BOPTools_Tools3D::GetApproxNormalToFaceOnEdge (aSpxSimm, aFx2, aT, aPx2, aDNFx2);
    //  
    aContext->ProjectPointOnEdge(aPx, anE1, aT1);
    PointNearE (anE1, aF1, aT1, aPF1, aMoreShift);
  }
  
  {
    Standard_Real d12, d1, anAlfa12, anAlfa1, aTwoPI;
    
    aTwoPI = M_PI + M_PI;
    
    gp_Vec aVx1(aPx, aPx1);
    gp_Dir aDBx1 (aVx1);
    gp_Pln aPlnToCompare (aPx, aDNFx1);
    
    gp_Vec aVx2(aPx, aPx2);
    gp_Dir aDBx2 (aVx2);
    
    anAlfa12=aDBx1.Angle(aDBx2);
    d12=BOPTools_Tools3D::SignDistance(aPx2, aPlnToCompare);
    if (d12 < 0.) {
      anAlfa12=aTwoPI-anAlfa12;
    }
    
    gp_Vec aVF1(aPx, aPF1);
    gp_Dir aDBF1 (aVF1);
    anAlfa1=aDBx1.Angle(aDBF1);
    d1=BOPTools_Tools3D::SignDistance(aPF1, aPlnToCompare);
    if (d1 < 0.) {
      anAlfa1=aTwoPI-anAlfa1;
    }
    
    aStPF1=TopAbs_OUT;
    if (anAlfa1 > anAlfa12) {
      aStPF1=TopAbs_IN;
    }
  }
}
  
//=======================================================================
//function : Orientation
//purpose  :
//=======================================================================
  TopAbs_Orientation BOPTools_Tools3D::Orientation(const TopoDS_Edge& anE,
						   const TopoDS_Face& aF)
{
  TopAbs_Orientation anOr=TopAbs_INTERNAL;

  TopExp_Explorer anExp;
  anExp.Init(aF, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Edge& anEF1=TopoDS::Edge(anExp.Current());
    if (anEF1.IsEqual(anE)) {
      anOr=anEF1.Orientation();
      break;
    }
  }
  return anOr;
}

//=======================================================================
//function : SignDistance
//purpose  :
//=======================================================================
  Standard_Real BOPTools_Tools3D::SignDistance(const gp_Pnt& aP, 
					       const gp_Pln& aPln)
{
  Standard_Real A, B, C, D, d;
  aPln.Coefficients(A, B, C, D);
  //
  d   = A*aP.X() + B*aP.Y() + C*aP.Z() + D;
  
  return d;
}

//=======================================================================
// function: IsValidArea
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_Tools3D::IsValidArea (const TopoDS_Face& aF, 
						  Standard_Boolean& bNegativeFlag)
			       
{
  Standard_Boolean bFlag;
  Standard_Real aMass, dM=1.e-16;
  GProp_GProps G;
  BRepGProp::SurfaceProperties(aF,G);
  aMass=G.Mass();
  //
  bFlag=(fabs(aMass)-dM > 0.);
  bNegativeFlag=(aMass < dM);

  if( bNegativeFlag ) {
    Bnd_Box boxF;
    BRepBndLib::AddClose(aF, boxF);
    Standard_Real aXmin = 0, aYmin = 0., aZmin = 0., aXmax = 0., aYmax = 0., aZmax = 0.;
    boxF.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    Standard_Boolean bigX = (fabs(aXmax-aXmin) >= 1.e+10);
    Standard_Boolean bigY = (fabs(aYmax-aYmin) >= 1.e+10);
    Standard_Boolean bigZ = (fabs(aZmax-aZmin) >= 1.e+10);
    if( !bigX && (!bigY && !bigZ) ) {
      TopExp_Explorer anExp;
      Standard_Integer nbW = 0;
      for(anExp.Init(aF, TopAbs_WIRE); anExp.More(); anExp.Next()) {
	const TopoDS_Wire& aW = TopoDS::Wire(anExp.Current());
	nbW++;
      }
      if( nbW == 1 ) {
	TopTools_IndexedDataMapOfShapeListOfShape mapVE;
	mapVE.Clear();
	TopExp::MapShapesAndAncestors(aF,TopAbs_VERTEX,TopAbs_EDGE,mapVE);
	Standard_Integer nbKeys = mapVE.Extent(), iKey = 0;
	Standard_Boolean changeFlag = Standard_True;
	for( iKey = 1; iKey <= nbKeys; iKey++ ) {
	  const TopoDS_Vertex& iV = TopoDS::Vertex(mapVE.FindKey(iKey));
	  if( iV.IsNull() ) continue;
	  //Standard_Real TolV = BRep_Tool::Tolerance(iV);
	  const TopTools_ListOfShape& iLE = mapVE.FindFromIndex(iKey);
	  Standard_Integer nbE = iLE.Extent();
	  if( nbE != 2 ) {
	    changeFlag = Standard_False;
	    break;
	  }
	  const TopoDS_Edge& iE1 = TopoDS::Edge(iLE.First());
	  const TopoDS_Edge& iE2 = TopoDS::Edge(iLE.Last());
	  if(BRep_Tool::Degenerated(iE1) ||
	     BRep_Tool::Degenerated(iE2) ) continue;
	  Standard_Real iPE1 = BRep_Tool::Parameter(iV,iE1);
	  Standard_Real iPE2 = BRep_Tool::Parameter(iV,iE2);
	  Standard_Real pF1 = 0., pL1 = 0., pF2 = 0., pL2 = 0.;
	  Handle(Geom_Curve) aC3D1 = BRep_Tool::Curve(iE1,pF1,pL1);
	  Handle(Geom_Curve) aC3D2 = BRep_Tool::Curve(iE2,pF2,pL2);
	  if( aC3D1.IsNull() || aC3D2.IsNull() ) {
	    changeFlag = Standard_False;
	    break;
	  }
	  if( Abs(Abs(pL1-pF1)-Abs(pL2-pF2)) <= 1.e-10 ) {
	    changeFlag = Standard_False;
	    break;
	  }
	  gp_Pnt aPnt1 = aC3D1->Value(iPE1);
	  gp_Pnt aPnt2 = aC3D2->Value(iPE2);
	  Standard_Real dist = aPnt1.Distance(aPnt2);
	  Standard_Real TolE1 = BRep_Tool::Tolerance(iE1);
	  Standard_Real TolE2 = BRep_Tool::Tolerance(iE2);
	  if( dist > (/*TolV+*/TolE1+TolE2) ) {
	    changeFlag = Standard_False;
	    break;
	  }
	}
	if( changeFlag ) bNegativeFlag = 0;
      }
    }
  }

  //
  return bFlag;
}

//=======================================================================
// function: PointNearE
// purpose: 
//=======================================================================
void PointNearE (const TopoDS_Edge& aE,
		 const TopoDS_Face& aF,
		 const Standard_Real aT,
		 gp_Pnt& aPInFace,
		 const Standard_Boolean aSt) 
{
  Standard_Real aT1, aT2;
  //
  // 1. a Point on Edge aPOnEdge
  Handle(Geom_Curve)aC=BRep_Tool::Curve(aE, aT1, aT2);
  gp_Pnt aPOnEdge;
  gp_Pnt2d aPInFace2D;
  //
  aC->D0 (aT, aPOnEdge);
  //
  // 2. a Point inside Face near aPOnEdge aPInFace;
  TopoDS_Face aFF=aF;
  TopoDS_Edge aERight;
  aFF.Orientation(TopAbs_FORWARD);
  BOPTools_Tools3D::OrientEdgeOnFace (aE, aFF, aERight);
  //
  //
  Standard_Real aTolE, aDt2D;
  GeomAbs_SurfaceType aType;
  {
    aDt2D=BOPTools_Tools3D::MinStepIn2d();
    //
    Handle(Geom_Surface) aS=BRep_Tool::Surface(aFF);
    GeomAdaptor_Surface aGASF(aS);
    aType=aGASF.GetType();
    if (aType==GeomAbs_Plane) {
      aTolE=BRep_Tool::Tolerance(aE);
      if (aTolE>aDt2D) {
	aDt2D=aTolE;
      }
    }
    
  }

  //-- EJG
  if( aSt )
    if( aDt2D < 1.e-4)
      aDt2D *= 10.;
  //-- EJG

  //
  BOPTools_Tools3D::PointNearEdge (aERight, aFF, aT, aDt2D, aPInFace2D, aPInFace);
  //
  if (BRep_Tool::IsClosed(aE, aF)) {
    Standard_Real X, Y, UMin, UMax, VMin, VMax;
    X=aPInFace2D.X();
    Y=aPInFace2D.Y();
    BRepTools::UVBounds(aF, UMin, UMax, VMin, VMax);
    if (!(X >= UMin && X <= UMax && Y >= VMin && Y <= VMax)) {
      aERight.Reverse();
      BOPTools_Tools3D::PointNearEdge (aERight, aFF, aT, aPInFace2D, aPInFace);
    }
  }
  
}

static Standard_Boolean PseudoSDFaces(const BRepAdaptor_Surface& BS1,
				      const BRepAdaptor_Surface& BS2)
{
  Standard_Real TolF1 = BS1.Tolerance();
  Standard_Real TolF2 = BS2.Tolerance();

  gp_Pln Pln1 = BS1.Plane();
  gp_Pln Pln2 = BS2.Plane();
  
  TopExp_Explorer anExpE;
  Standard_Real pF = 0., pL = 0.;


  const TopoDS_Face& aF1 = BS1.Face();
  Standard_Real maxTolE1 = 1.e-7;
  Standard_Integer nbE1 = 0, nbOnE1 = 0;
  for(anExpE.Init(aF1, TopAbs_EDGE); anExpE.More(); anExpE.Next()) {
    const TopoDS_Edge& aE = TopoDS::Edge(anExpE.Current());
    nbE1++;
    Standard_Real aTolE = BRep_Tool::Tolerance(aE);
    if( aTolE > maxTolE1 ) maxTolE1 = aTolE;
    Handle(Geom_Curve) aC3D = BRep_Tool::Curve(aE, pF, pL);
    if( !aC3D.IsNull() ) {
      Standard_Real pM = BOPTools_Tools2D::IntermediatePoint(pF, pL);
      gp_Pnt mPnt = aC3D->Value(pM);
      Standard_Real distMP = Pln2.Distance(mPnt);

      if( distMP <= aTolE )
	nbOnE1++;
      else {
	TopoDS_Vertex Vf, Vl;
	TopExp::Vertices(aE,Vf,Vl);
	if( !Vf.IsNull() && !Vl.IsNull() ) {
	  Standard_Real aTolVf = BRep_Tool::Tolerance(Vf);
	  Standard_Real aTolVl = BRep_Tool::Tolerance(Vl);
	  gp_Pnt aPntF = BRep_Tool::Pnt(Vf);
	  gp_Pnt aPntL = BRep_Tool::Pnt(Vl);
	  Standard_Real distF = Pln2.Distance(aPntF);
	  Standard_Real distL = Pln2.Distance(aPntL);
	  if( distF <= aTolVf && distL <= aTolVl )
	    nbOnE1++;
	}
	else if( !Vf.IsNull() && Vl.IsNull() ) {
	  Standard_Real aTolVf = BRep_Tool::Tolerance(Vf);
	  gp_Pnt aPntF = BRep_Tool::Pnt(Vf);
	  Standard_Real distF = Pln2.Distance(aPntF);
	  if( distF <= aTolVf )
	    nbOnE1++;
	}
	else if( Vf.IsNull() && !Vl.IsNull() ) {
	  Standard_Real aTolVl = BRep_Tool::Tolerance(Vl);
	  gp_Pnt aPntL = BRep_Tool::Pnt(Vl);
	  Standard_Real distL = Pln2.Distance(aPntL);
	  if( distL <= aTolVl )
	    nbOnE1++;
	}
	else
	  continue;
      }
    }
  }

  Standard_Boolean procF1 = ((maxTolE1/TolF1) >= 1000. ||
			     (TolF1/maxTolE1) >= 1000.) ? Standard_True : Standard_False;
  procF1 = (procF1 && (nbE1 > 1 && nbOnE1 > 1) );

  if( !procF1 )
    return Standard_False;

  const TopoDS_Face& aF2 = BS1.Face();
  Standard_Real maxTolE2 = 1.e-7;
  Standard_Integer nbE2 = 0, nbOnE2 = 0;
  for(anExpE.Init(aF2, TopAbs_EDGE); anExpE.More(); anExpE.Next()) {
    const TopoDS_Edge& aE = TopoDS::Edge(anExpE.Current());
    nbE2++;
    Standard_Real aTolE = BRep_Tool::Tolerance(aE);
    if( aTolE > maxTolE2 ) maxTolE2 = aTolE;
    Handle(Geom_Curve) aC3D = BRep_Tool::Curve(aE, pF, pL);
    if( !aC3D.IsNull() ) {
      Standard_Real pM = BOPTools_Tools2D::IntermediatePoint(pF, pL);
      gp_Pnt mPnt = aC3D->Value(pM);
      Standard_Real distMP = Pln1.Distance(mPnt);
      if( distMP <= aTolE )
	nbOnE2++;
      else {
	TopoDS_Vertex Vf, Vl;
	TopExp::Vertices(aE,Vf,Vl);
	if( !Vf.IsNull() && !Vl.IsNull() ) {
	  Standard_Real aTolVf = BRep_Tool::Tolerance(Vf);
	  Standard_Real aTolVl = BRep_Tool::Tolerance(Vl);
	  gp_Pnt aPntF = BRep_Tool::Pnt(Vf);
	  gp_Pnt aPntL = BRep_Tool::Pnt(Vl);
	  Standard_Real distF = Pln1.Distance(aPntF);
	  Standard_Real distL = Pln1.Distance(aPntL);
	  if( distF <= aTolVf && distL <= aTolVl )
	    nbOnE2++;
	}
	else if( !Vf.IsNull() && Vl.IsNull() ) {
	  Standard_Real aTolVf = BRep_Tool::Tolerance(Vf);
	  gp_Pnt aPntF = BRep_Tool::Pnt(Vf);
	  Standard_Real distF = Pln1.Distance(aPntF);
	  if( distF <= aTolVf )
	    nbOnE2++;
	}
	else if( Vf.IsNull() && !Vl.IsNull() ) {
	  Standard_Real aTolVl = BRep_Tool::Tolerance(Vl);
	  gp_Pnt aPntL = BRep_Tool::Pnt(Vl);
	  Standard_Real distL = Pln1.Distance(aPntL);
	  if( distL <= aTolVl )
	    nbOnE2++;
	}
	else
	  continue;
      }
    }
  }

  Standard_Boolean procF2 = ((maxTolE2/TolF2) >= 1000. ||
			     (TolF2/maxTolE2) >= 1000.) ? Standard_True : Standard_False;
  procF2 = (procF2 && (nbE2 > 1 && nbOnE2 > 1) );

  return (procF1 && procF2);

}

Standard_Boolean CheckKeepArguments(const TopoDS_Face& F1,
				    const TopoDS_Face& F2,
				    const TopoDS_Face& F3)
{
  BRepAdaptor_Surface aBS1(F1);
  BRepAdaptor_Surface aBS2(F2);
  BRepAdaptor_Surface aBS3(F3);
  
  GeomAbs_SurfaceType aT1 = aBS1.GetType();
  GeomAbs_SurfaceType aT2 = aBS2.GetType();
  GeomAbs_SurfaceType aT3 = aBS3.GetType();
  
  if(aT1 == GeomAbs_Cylinder ||
     aT1 == GeomAbs_Cone ||
     aT1 ==  GeomAbs_Sphere ||
     aT1 == GeomAbs_Torus ) return Standard_True;

  if(aT2 == GeomAbs_Cylinder ||
     aT2 == GeomAbs_Cone ||
     aT3 ==  GeomAbs_Sphere ||
     aT3 == GeomAbs_Torus ) return Standard_True;

  if(aT3 == GeomAbs_Cylinder ||
     aT3 == GeomAbs_Cone ||
     aT3 ==  GeomAbs_Sphere ||
     aT3 == GeomAbs_Torus ) return Standard_True;

  if( aT1 == GeomAbs_Plane && aT2 == GeomAbs_Plane ) {
    if( PseudoSDFaces(aBS1, aBS2) )
      return Standard_False;
  }
  else if( aT1 == GeomAbs_Plane && aT3 == GeomAbs_Plane ) {
    if( PseudoSDFaces(aBS1, aBS3) )
      return Standard_False;
  }
  else if( aT2 == GeomAbs_Plane && aT3 == GeomAbs_Plane ) {
    if( PseudoSDFaces(aBS2, aBS3) )
      return Standard_False;
  }
  else
    return Standard_True;
  return Standard_True;
}
