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


#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <BRep_ListOfPointRepresentation.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeAnalysis_TransferParametersProj.hxx>
#include <ShapeBuild_Edge.hxx>
#include <Standard_Type.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeAnalysis_TransferParametersProj,ShapeAnalysis_TransferParameters)

//=======================================================================
//function : ShapeAnalysis_TransferParametersProj
//purpose  : 
//=======================================================================
ShapeAnalysis_TransferParametersProj::ShapeAnalysis_TransferParametersProj()
{
  myMaxTolerance = 1; //Precision::Infinite(); ?? pdn
  myForceProj = Standard_False;
  myInitOK = Standard_False;
}


//=======================================================================
//function : ShapeAnalysis_TransferParametersProj
//purpose  : 
//=======================================================================

ShapeAnalysis_TransferParametersProj::ShapeAnalysis_TransferParametersProj(const TopoDS_Edge& E,
                                                                           const TopoDS_Face& F)
{
  myMaxTolerance = 1; //Precision::Infinite(); ?? pdn
  myForceProj = Standard_False;
  Init(E,F);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeAnalysis_TransferParametersProj::Init(const TopoDS_Edge& E,
                                                const TopoDS_Face& F)
{
  myInitOK = Standard_False;
  ShapeAnalysis_TransferParameters::Init(E,F);
  myEdge = E;
  myPrecision = BRep_Tool::Tolerance(E); // it is better - skl OCC2851
  //myPrecision = Precision::Confusion();

  myCurve = BRep_Tool::Curve (E, myFirst, myLast);
  if ( myCurve.IsNull() ) { myFirst = 0.; myLast = 1.; return;}
    
  if ( F.IsNull() ) return;
    
  Standard_Real f2d, l2d;
  ShapeAnalysis_Edge sae;
  if(sae.PCurve (E, F, myCurve2d, f2d, l2d, Standard_False)) {
      
    Handle(Geom2dAdaptor_HCurve) AC2d  = new Geom2dAdaptor_HCurve(myCurve2d,f2d,l2d);
    Handle(Geom_Surface) aSurface = BRep_Tool::Surface(F,myLocation);
    Handle(GeomAdaptor_HSurface) AdS = new GeomAdaptor_HSurface(aSurface);
    
    Adaptor3d_CurveOnSurface Ad1(AC2d,AdS);
    myAC3d = Ad1;//new Adaptor3d_CurveOnSurface(AC2d,AdS);
    myInitOK = Standard_True;
  }
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfReal) ShapeAnalysis_TransferParametersProj::Perform
          (const Handle(TColStd_HSequenceOfReal)& Knots,
           const Standard_Boolean To2d) 
{
  //pdn
  if( !myInitOK || 
     (! myForceProj && myPrecision < myMaxTolerance && BRep_Tool::SameParameter(myEdge)))
    return ShapeAnalysis_TransferParameters::Perform(Knots,To2d);
  
  Handle(TColStd_HSequenceOfReal) resKnots = new TColStd_HSequenceOfReal;

  Standard_Integer len = Knots->Length();
  Standard_Real preci = 2*Precision::PConfusion();

  Standard_Real first = (To2d ? myAC3d.FirstParameter() : myFirst);
  Standard_Real last  = (To2d ? myAC3d.LastParameter() : myLast);
  Standard_Real maxPar = first;
  Standard_Real lastPar = last;
  Standard_Real prevPar = maxPar;

  Standard_Integer j; // svv Jan 10 2000 : porting on DEC
  for(j = 1; j <= len; j++) {
    Standard_Real par = PreformSegment(Knots->Value(j),To2d,prevPar,lastPar);
    prevPar = par;
    if(prevPar > lastPar)
      prevPar -= preci;
    resKnots->Append(par);
    if(par > maxPar)
      maxPar = par;
  }
  
  //pdn correcting on periodic
  if(myCurve->IsClosed()) {
    for(j = len; j >=1; j--) 
      if(resKnots->Value(j) < maxPar) 
        resKnots->SetValue(j,(To2d ? myAC3d.LastParameter() : myCurve->LastParameter())-(len-j)*preci);
      else
        break;
  }
  //pdn correction on range
  for ( j=1; j <= len; j++ ) {
    if ( resKnots->Value (j) < first ) resKnots->SetValue ( j, first );
    if ( resKnots->Value (j) > last  ) resKnots->SetValue ( j, last );
  }
  
  return resKnots;
}


//=======================================================================
//function : PreformSegment
//purpose  : 
//=======================================================================

Standard_Real ShapeAnalysis_TransferParametersProj::PreformSegment(const Standard_Real Param,
                                                                   const Standard_Boolean To2d,
                                                                   const Standard_Real First,
                                                                   const Standard_Real Last)
{
  Standard_Real linPar = ShapeAnalysis_TransferParameters::Perform(Param, To2d);
  if( !myInitOK || 
     (! myForceProj && myPrecision < myMaxTolerance && BRep_Tool::SameParameter(myEdge)))
    return linPar;
  
  Standard_Real linDev, projDev;
  
  ShapeAnalysis_Curve sac;
  gp_Pnt pproj;
  Standard_Real ppar;
  if(To2d) {
    gp_Pnt p1 = myCurve->Value(Param).Transformed(myLocation.Inverted());
    Handle(Adaptor3d_HSurface) AdS = myAC3d.GetSurface();
    Handle(Geom2dAdaptor_HCurve) AC2d  = new Geom2dAdaptor_HCurve(myCurve2d,First,Last);
    Adaptor3d_CurveOnSurface Ad1(AC2d,AdS);
    projDev = sac.Project(Ad1,p1,myPrecision,pproj,ppar);//pdn
    linDev = p1.Distance(Ad1.Value(linPar));
  }
  else {
    gp_Pnt p1 = myAC3d.Value(Param).Transformed(myLocation);
    projDev = sac.Project(myCurve,p1,myPrecision,pproj,ppar,First,Last,Standard_False);
    linDev = p1.Distance(myCurve->Value(linPar));
  }
  
  if ( linDev <= projDev ||  (linDev < myPrecision && linDev <= 2 * projDev ) ) 
    ppar = linPar;
  return ppar;
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Real ShapeAnalysis_TransferParametersProj::Perform(const Standard_Real Knot,
                                                            const Standard_Boolean To2d) 
{
  if( !myInitOK || 
     (! myForceProj && myPrecision < myMaxTolerance && BRep_Tool::SameParameter(myEdge)))
    return ShapeAnalysis_TransferParameters::Perform(Knot, To2d);
  
  Standard_Real res;
  if(To2d) 
    res = PreformSegment(Knot,To2d,myAC3d.FirstParameter(),myAC3d.LastParameter());
  else 
    res = PreformSegment(Knot,To2d,myFirst,myLast);

  //pdn correction on range
  Standard_Real first = (To2d ? myAC3d.FirstParameter() : myFirst);
  Standard_Real last  = (To2d ? myAC3d.LastParameter() : myLast);
  if ( res < first ) res = first;
  if ( res > last  ) res = last;
  return res;
}


//=======================================================================
//function : CorrectParameter
//purpose  : auxilary
//=======================================================================
static Standard_Real CorrectParameter(const Handle(Geom2d_Curve) crv,
				      const Standard_Real param)
{
  if(crv->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) {
    Handle(Geom2d_TrimmedCurve) tmp = Handle(Geom2d_TrimmedCurve)::DownCast (crv);
    return CorrectParameter(tmp->BasisCurve(),param);
  } 
  else if(crv->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve))) {
    Handle(Geom2d_OffsetCurve) tmp = Handle(Geom2d_OffsetCurve)::DownCast (crv);
    return CorrectParameter(tmp->BasisCurve(),param);
  } 
  else if(crv->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
    Handle(Geom2d_BSplineCurve) bspline = Handle(Geom2d_BSplineCurve)::DownCast (crv);
    for(Standard_Integer j = bspline->FirstUKnotIndex(); j <= bspline->LastUKnotIndex(); j++) {
      Standard_Real valknot = bspline->Knot(j);
      if( Abs(valknot-param)<Precision::PConfusion() )
	return valknot;
    }
  }
  return param;
}
    
  
//=======================================================================
//function : TransferRange
//purpose  : 
//=======================================================================

void ShapeAnalysis_TransferParametersProj::TransferRange(TopoDS_Edge& newEdge,
                                                         const Standard_Real prevPar,
                                                         const Standard_Real currPar,
                                                         const Standard_Boolean Is2d) 
{
  if( !myInitOK || 
     (! myForceProj && myPrecision < myMaxTolerance && BRep_Tool::SameParameter(myEdge))) {
    ShapeAnalysis_TransferParameters::TransferRange(newEdge,prevPar,currPar,Is2d);
    return;
  }

  BRep_Builder B;
  Standard_Boolean samerange = Standard_True;
  ShapeBuild_Edge sbe;
  sbe.CopyRanges(newEdge,myEdge);
  gp_Pnt p1;
  gp_Pnt p2;
  Standard_Real alpha = 0, beta = 1;
  Standard_Real preci = Precision::PConfusion();
  Standard_Real firstPar, lastPar;
  if(prevPar < currPar) {
    firstPar = prevPar;
    lastPar  = currPar;
  }
  else {
    firstPar = currPar;
    lastPar  = prevPar;
  }
  if(Is2d) {
    p1 = myAC3d.Value(firstPar).Transformed(myLocation);
    if(Precision::IsInfinite(p1.X()) || Precision::IsInfinite(p1.Y()) ||
       Precision::IsInfinite(p1.Z()))
    {
      B.SameRange(newEdge, Standard_False);
      return;
    }
    p2 = myAC3d.Value(lastPar).Transformed(myLocation);
    if(Precision::IsInfinite(p2.X()) || Precision::IsInfinite(p2.Y()) ||
       Precision::IsInfinite(p2.Z()))
    {
      B.SameRange(newEdge, Standard_False);
      return;
    }
    Standard_Real fact = myAC3d.LastParameter() - myAC3d.FirstParameter();
    if( fact > Epsilon(myAC3d.LastParameter()) ) {
      alpha = ( firstPar - myAC3d.FirstParameter() ) / fact;
      beta  = ( lastPar - myAC3d.FirstParameter() ) / fact;
    }
  }
  else {
    p1 = myCurve->Value(firstPar);
    if(Precision::IsInfinite(p1.X()) || Precision::IsInfinite(p1.Y()) ||
       Precision::IsInfinite(p1.Z()))
    {
      B.SameRange(newEdge, Standard_False);
      return;
    }
    p2 = myCurve->Value(lastPar);
    if(Precision::IsInfinite(p2.X()) || Precision::IsInfinite(p2.Y()) ||
       Precision::IsInfinite(p2.Z()))
    {
      B.SameRange(newEdge, Standard_False);
      return;
    }
    Standard_Real fact = myLast - myFirst;
    if( fact > Epsilon(myLast) ) {
      alpha = ( firstPar - myFirst ) / fact;
      beta  = ( lastPar - myFirst ) / fact;
    }
  }
  const Standard_Boolean useLinearFirst = (alpha < preci);
  const Standard_Boolean useLinearLast = (1-beta < preci);
  TopLoc_Location EdgeLoc = myEdge.Location();
  ShapeAnalysis_Curve sac;
  gp_Pnt pproj;
  Standard_Real ppar1,ppar2;
  BRep_ListOfCurveRepresentation& tolist = (*((Handle(BRep_TEdge)*)&newEdge.TShape()))->ChangeCurves();
  Handle(BRep_GCurve) toGC;
  for (BRep_ListIteratorOfListOfCurveRepresentation toitcr (tolist); toitcr.More(); toitcr.Next()) {
    toGC = Handle(BRep_GCurve)::DownCast(toitcr.Value());
    if ( toGC.IsNull() ) continue;
    TopLoc_Location loc = ( EdgeLoc * toGC->Location() ).Inverted();
    if ( toGC->IsCurve3D() ) {
      if (!Is2d) {
	ppar1 = firstPar;
	ppar2 = lastPar;
      } 
      else {
	Handle(Geom_Curve) C3d = toGC->Curve3D();
	if (C3d.IsNull()) continue;
	Standard_Real first = toGC->First();
	Standard_Real last  = toGC->Last();
	Standard_Real len = last -first;
	gp_Pnt ploc1 = p1.Transformed(loc);
	gp_Pnt ploc2 = p2.Transformed(loc); 
	GeomAdaptor_Curve GAC(C3d,first,last);
	// CATIA bplseitli.model FAC1155 - Copy: protection for degenerated edges(3d case for symmetry)
	Standard_Real linFirst = first+alpha*len;
	Standard_Real linLast  = first+beta*len;
	Standard_Real dist1 = sac.NextProject(linFirst,GAC,ploc1,myPrecision,pproj,ppar1);
	Standard_Real dist2 = sac.NextProject(linLast,GAC,ploc2,myPrecision,pproj,ppar2);
	Standard_Boolean useLinear = Abs(ppar1-ppar2) < preci;
	
	gp_Pnt pos1 = C3d->Value ( linFirst );
	gp_Pnt pos2 = C3d->Value ( linLast );
	Standard_Real d01 = pos1.Distance ( ploc1 );
	Standard_Real d02 = pos2.Distance ( ploc2 );
	if ( useLinearFirst || useLinear || d01 <= dist1 || ( d01 < myPrecision && d01 <= 2 * dist1 ) ) 
	  ppar1 = linFirst;
	if ( useLinearLast || useLinear || d02 <= dist2 || ( d02 < myPrecision && d02 <= 2 * dist2 ) ) 
	  ppar2 = linLast;
      }
      if(ppar1 > ppar2) {
	Standard_Real tmpP = ppar2; ppar2 = ppar1; ppar1 = tmpP;
      }
      if(ppar2-ppar1 < preci) {
	if(ppar1-toGC->First() < preci)
	  ppar2+=2*preci;
	else if(toGC->Last()-ppar2 < preci)
	  ppar1-=2*preci;
	else {
	  ppar1 -= preci;
	  ppar2 += preci;
	}
      }
      toGC->SetRange ( ppar1, ppar2);
      //if(fabs(ppar1- firstPar) > Precision::PConfusion() || 
	//     fabs(ppar2 - lastPar) >Precision::PConfusion()) // by LSS
      if(ppar1!=firstPar || ppar2!=lastPar)
        samerange = Standard_False;
      
    }
    else if (toGC->IsCurveOnSurface()) { //continue;  || 

      Standard_Boolean localLinearFirst = useLinearFirst;
      Standard_Boolean localLinearLast  = useLinearLast;
      Handle(Geom2d_Curve) C2d = toGC->PCurve();
      Standard_Real first = toGC->First();
      Standard_Real last  = toGC->Last();
      Standard_Real len = last -first;
      Handle(Geom2dAdaptor_HCurve) AC2d  = new Geom2dAdaptor_HCurve(toGC->PCurve(),first,last);
      Handle(GeomAdaptor_HSurface) AdS = new GeomAdaptor_HSurface( toGC->Surface());
      Adaptor3d_CurveOnSurface Ad1(AC2d,AdS);
      ShapeAnalysis_Curve sac1;
      
      //gp_Pnt p1 = Ad1.Value(prevPar);
      //gp_Pnt p2 = Ad1.Value(currPar);
      gp_Pnt ploc1 = p1.Transformed(loc);
      gp_Pnt ploc2 = p2.Transformed(loc);
      // CATIA bplseitli.model FAC1155 - Copy: protection for degenerated edges
      Standard_Real linFirst = first+alpha*len;
      Standard_Real linLast  = first+beta*len;
      Standard_Real dist1 = sac1.NextProject(linFirst, Ad1, ploc1, myPrecision,pproj,ppar1);
      Standard_Real dist2 = sac1.NextProject(linLast, Ad1, ploc2, myPrecision,pproj,ppar2);

      Standard_Boolean isFirstOnEnd = (ppar1-first)/len < Precision::PConfusion();
      Standard_Boolean isLastOnEnd = (last-ppar2)/len < Precision::PConfusion();
      Standard_Boolean useLinear = Abs(ppar1-ppar2) < Precision::PConfusion();
      if(isFirstOnEnd && ! localLinearFirst)
	localLinearFirst = Standard_True;
      if(isLastOnEnd && ! localLinearLast)
	localLinearLast  = Standard_True;

      gp_Pnt pos1 = Ad1.Value ( linFirst );
      gp_Pnt pos2 = Ad1.Value ( linLast );
      Standard_Real d01 = pos1.Distance ( ploc1 );
      Standard_Real d02 = pos2.Distance ( ploc2 );
      if ( localLinearFirst || useLinear || d01 <= dist1 || ( d01 < myPrecision && d01 <= 2 * dist1 ) ) 
	ppar1 = linFirst;
      if ( localLinearLast  || useLinear || d02 <= dist2 || ( d02 < myPrecision && d02 <= 2 * dist2 ) ) 
	ppar2 = linLast;

      if(ppar1 > ppar2) {
	Standard_Real tmpP = ppar2; ppar2 = ppar1; ppar1 = tmpP;
      }
      ppar1 = CorrectParameter(C2d,ppar1);
      ppar2 = CorrectParameter(C2d,ppar2);
      if(ppar2-ppar1 < preci) {
	if(ppar1-toGC->First() < preci)
	  ppar2+=2*preci;
	else if(toGC->Last()-ppar2 < preci)
	  ppar1-=2*preci;
	else {
	  ppar1 -= preci;
	  ppar2 += preci;
	}
      }
      toGC->SetRange ( ppar1, ppar2);
      //if(fabs(ppar1 - firstPar) > Precision::PConfusion() || 
	//    fabs(ppar2 -lastPar) > Precision::PConfusion())// by LSS
      if(ppar1 != firstPar || ppar2 != lastPar)
	    samerange = Standard_False;
    }
  }
  B.SameRange(newEdge, samerange);
}


//=======================================================================
//function : IsSameRange
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_TransferParametersProj::IsSameRange() const
{

  if( !myInitOK || 
     (! myForceProj && myPrecision < myMaxTolerance && BRep_Tool::SameParameter(myEdge)))
    return ShapeAnalysis_TransferParameters::IsSameRange();
  else
    return Standard_False;
}


//=======================================================================
//function : ForceProjection
//purpose  : 
//=======================================================================

Standard_Boolean& ShapeAnalysis_TransferParametersProj::ForceProjection() 
{
  return myForceProj;
}

//=======================================================================
//function : CopyNMVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ShapeAnalysis_TransferParametersProj::CopyNMVertex (const TopoDS_Vertex& theV,
				   const TopoDS_Edge& toedge, 
				   const TopoDS_Edge& fromedge)
{
  TopoDS_Vertex anewV;
  if(theV.Orientation() != TopAbs_INTERNAL &&
     theV.Orientation() != TopAbs_EXTERNAL)
    return anewV;
  
  TopLoc_Location fromLoc;
  Standard_Real f1,l1;
  const Handle(Geom_Curve)& C1 = BRep_Tool::Curve(fromedge,fromLoc,f1,l1);
  fromLoc = fromLoc.Predivided(theV.Location());
  
  Standard_Real f2,l2;
  Handle(Geom_Curve) C2 = BRep_Tool::Curve(toedge,f2,l2);
  
  anewV = TopoDS::Vertex(theV.EmptyCopied());
  gp_Pnt apv = BRep_Tool::Pnt(anewV);
  
   BRep_ListOfPointRepresentation& alistrep = 
    (*((Handle(BRep_TVertex)*)&anewV.TShape()))->ChangePoints();
  
  BRep_ListIteratorOfListOfPointRepresentation itpr
	  ((*((Handle(BRep_TVertex)*) &theV.TShape()))->Points());
  
  Standard_Real aOldPar = RealLast();
  Standard_Boolean hasRepr = Standard_False;
  for ( ;itpr.More(); itpr.Next()) {
    const Handle(BRep_PointRepresentation)& pr = itpr.Value();
    if(pr.IsNull())
      continue;
    if(pr->IsPointOnCurve(C1,fromLoc)) {
      aOldPar = pr->Parameter();
      hasRepr =Standard_True;
      continue;
    }
    else if(pr->IsPointOnSurface()) {
      Handle(BRep_PointOnSurface) aOld = Handle(BRep_PointOnSurface)::DownCast(pr);
      Handle(BRep_PointOnSurface) aPS = new BRep_PointOnSurface(aOld->Parameter(),
								  aOld->Parameter2(),
								  aOld->Surface(),
								  aOld->Location());
      alistrep.Append(aPS);
      continue;
    }
    else if(pr->IsPointOnCurveOnSurface()) {
      Standard_Boolean found = Standard_False;
      BRep_ListIteratorOfListOfCurveRepresentation fromitcr
	((*((Handle(BRep_TEdge)*)&fromedge.TShape()))->ChangeCurves());
      
      for( ;fromitcr.More() && !found; fromitcr.Next()) {
	Handle(BRep_GCurve) fromGC = Handle(BRep_GCurve)::DownCast(fromitcr.Value());
	if ( fromGC.IsNull() || !fromGC->IsCurveOnSurface()) continue;
	
	TopLoc_Location aL = fromGC->Location();
	aL.Predivided(theV.Location());
	Handle(Geom_Surface) surface1 = fromGC->Surface();
	Handle(Geom2d_Curve) ac2d1 = fromGC->PCurve();
	if (pr->IsPointOnCurveOnSurface(ac2d1,surface1,aL)) {
      found = Standard_True;
	  if(!hasRepr) {
	    aOldPar = pr->Parameter();
	  }
	}
      }
      if(found) continue;
    }
    if(pr->IsPointOnCurve()) {
      Handle(BRep_PointOnCurve) aPRep = new BRep_PointOnCurve(pr->Parameter(),pr->Curve(),pr->Location());
      alistrep.Append(aPRep);
    }
    else if(pr->IsPointOnCurveOnSurface() ) {
      Handle(BRep_PointOnCurveOnSurface) aPonCS = 
	new BRep_PointOnCurveOnSurface(pr->Parameter(),pr->PCurve(),pr->Surface(),pr->Location()); 
      alistrep.Append(aPonCS);
    }
  }
  Standard_Real apar = aOldPar;
  Standard_Real aTol = BRep_Tool::Tolerance(theV);
  if(!hasRepr || (fabs(f1-f2) > Precision::PConfusion() || fabs(l1-l2)> Precision::PConfusion())) {
    gp_Pnt projP;
    ShapeAnalysis_Curve sae;
    Standard_Real adist = sae.Project(C2,apv,Precision::Confusion(),projP,apar);
    if(aTol < adist)
      aTol = adist;
  }
  BRep_Builder aB;
  aB.UpdateVertex(anewV,apar,toedge,aTol);
  
  //update tolerance
  Standard_Boolean needUpdate = Standard_False;
  gp_Pnt aPV = (*((Handle(BRep_TVertex)*)&anewV.TShape()))->Pnt();
  TopLoc_Location toLoc = toedge.Location();
  BRep_ListIteratorOfListOfCurveRepresentation toitcr
	((*((Handle(BRep_TEdge)*)&toedge.TShape()))->ChangeCurves());
      
  for( ;toitcr.More() ; toitcr.Next()) {
	Handle(BRep_GCurve) toGC = Handle(BRep_GCurve)::DownCast(toitcr.Value());
	if ( toGC.IsNull() || !toGC->IsCurveOnSurface()) continue;
	
	TopLoc_Location aL = (toLoc*toGC->Location()).Predivided(theV.Location());
	//aL.Predivided(theV.Location());
	Handle(Geom_Surface) surface1 = toGC->Surface();
	Handle(Geom2d_Curve) ac2d1 = toGC->PCurve();
	gp_Pnt2d aP2d = ac2d1->Value(apar);
    gp_Pnt aP3d = surface1->Value(aP2d.X(),aP2d.Y());
    aP3d.Transform(aL.Transformation());
    Standard_Real adist = aPV.Distance(aP3d);
    if(adist > aTol) {
      aTol = adist;
      needUpdate = Standard_True;
    }

  } 
  if(needUpdate)
    aB.UpdateVertex(anewV,aTol);
  return anewV;
}

//=======================================================================
//function : CopyNMVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ShapeAnalysis_TransferParametersProj::CopyNMVertex (const TopoDS_Vertex& theV,
				   const TopoDS_Face& toFace, 
				   const TopoDS_Face& fromFace)
{
  TopoDS_Vertex anewV;
  if(theV.Orientation() != TopAbs_INTERNAL &&
     theV.Orientation() != TopAbs_EXTERNAL)
    return anewV;
  

  TopLoc_Location fromLoc;
  TopLoc_Location toLoc;
  Handle(Geom_Surface) fromSurf = BRep_Tool::Surface(fromFace,fromLoc);
  Handle(Geom_Surface) toSurf = BRep_Tool::Surface(toFace,toLoc);
  fromLoc = fromLoc.Predivided(theV.Location());
  
  anewV = TopoDS::Vertex(theV.EmptyCopied());
  gp_Pnt apv = BRep_Tool::Pnt(anewV);
  
  
  BRep_ListOfPointRepresentation& alistrep = 
    (*((Handle(BRep_TVertex)*)&anewV.TShape()))->ChangePoints();
  
  BRep_ListIteratorOfListOfPointRepresentation itpr
	  ((*((Handle(BRep_TVertex)*) &theV.TShape()))->Points());
  
  Standard_Boolean hasRepr = Standard_False;
  Standard_Real apar1=0., apar2=0.;
  for ( ;itpr.More(); itpr.Next()) {
    const Handle(BRep_PointRepresentation)& pr = itpr.Value();
    if(pr.IsNull())
      continue;
    TopLoc_Location aLoc = pr->Location();
    if( pr->IsPointOnCurveOnSurface()) {
      Handle(BRep_PointOnCurveOnSurface) aPonCS = 
	    new BRep_PointOnCurveOnSurface(pr->Parameter(),pr->PCurve(),pr->Surface(),aLoc); 
      alistrep.Append(aPonCS);
    }
    else if(pr->IsPointOnCurve()) {
      Handle(BRep_PointOnCurve) aPRep = new BRep_PointOnCurve(pr->Parameter(),pr->Curve(),aLoc);
      alistrep.Append(aPRep);
    }
    else if(pr->IsPointOnSurface()) {
      Handle(BRep_PointOnSurface) aOld = Handle(BRep_PointOnSurface)::DownCast(pr);
      
      if(pr->IsPointOnSurface(fromSurf,fromLoc)) {
	apar1= aOld->Parameter();
	apar2 = aOld->Parameter2();
	hasRepr = Standard_True;
      }
      else {
	Handle(BRep_PointOnSurface) aPS = new BRep_PointOnSurface(aOld->Parameter(),
								    aOld->Parameter2(),
								    aOld->Surface(),
								    aOld->Location());
	alistrep.Append(aPS);
      }
    }
    
  }
  Standard_Real aTol = BRep_Tool::Tolerance(anewV);
  if(!hasRepr || (fromSurf != toSurf || fromLoc != toLoc)) {
    Handle(Geom_Surface) aS = BRep_Tool::Surface(toFace);
    Handle(ShapeAnalysis_Surface) aSurfTool = new ShapeAnalysis_Surface(aS);
    gp_Pnt2d aP2d = aSurfTool->ValueOfUV(apv,Precision::Confusion());
    apar1 = aP2d.X();
    apar2 = aP2d.Y();
    
    if(aTol < aSurfTool->Gap())
      aTol = aSurfTool->Gap() + 0.1*Precision::Confusion();
    //Handle(BRep_PointOnSurface) aPS = new BRep_PointOnSurface(aP2d.X(),aP2d.Y(),toSurf,toLoc);
    //alistrep.Append(aPS);
  }
  
  BRep_Builder aB;
  aB.UpdateVertex(anewV,apar1,apar2,toFace,aTol);
  return anewV;
}

