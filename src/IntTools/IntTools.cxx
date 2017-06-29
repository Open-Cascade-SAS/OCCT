// Created on: 2000-08-01
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepGProp.hxx>
#include <gce_ErrorType.hxx>
#include <gce_MakeCirc.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom_Curve.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <GProp_GProps.hxx>
#include <IntTools.hxx>
#include <IntTools_Array1OfRoots.hxx>
#include <IntTools_CArray1OfReal.hxx>
#include <IntTools_Root.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <TColStd_ListOfReal.hxx>
#include <TopoDS_Edge.hxx>

#include <algorithm>
//=======================================================================
//function : IntTools::GetRadius
//purpose  : 
//=======================================================================
  Standard_Integer IntTools::GetRadius(const BRepAdaptor_Curve& C,
				       const Standard_Real t1,
				       const Standard_Real t3,
				       Standard_Real& aR)
{
  GeomAbs_CurveType aType=C.GetType();
  if (aType==GeomAbs_Line) {
    return 1;
  }

  if (aType==GeomAbs_Circle) {
    gp_Circ aCrc=C.Circle();
    aR=aCrc.Radius();
    return 0;
  }

  Standard_Real t2;
  gp_Pnt P1, P2, P3;

  t2=0.5*(t1+t3);
  
  P1=C.Value(t1);
  P2=C.Value(t2);
  P3=C.Value(t3);
  //
  //
  gce_MakeCirc aMakeCirc(P1, P2, P3);
  gce_ErrorType anErrorType;
  
  anErrorType=aMakeCirc.Status();

  if (!aMakeCirc.IsDone()) {
    
    if (anErrorType==gce_ConfusedPoints ||
	anErrorType==gce_IntersectionError ||
	anErrorType==gce_ColinearPoints) {//modified by NIZNHY-PKV Fri Sep 24 09:54:05 2004ft
      return 2;
    }
    return -1;
  }
  //
  //
  gp_Circ aCirc=aMakeCirc.Value();
  aR=aCirc.Radius();
  
  return 0;
}

//=======================================================================
//function : PrepareArgs
//purpose  : 
//=======================================================================
  Standard_Integer IntTools::PrepareArgs(BRepAdaptor_Curve& C,
					 const Standard_Real Tmax,
					 const Standard_Real Tmin,
					 const Standard_Integer Discret,
					 const Standard_Real Deflection,
					 IntTools_CArray1OfReal& anArgs)
{
  
  TColStd_ListOfReal aPars;
  Standard_Real dt, tCurrent, tNext, aR, anAbsDeflection;
  Standard_Integer ip, i, j, aNbDeflectionPoints, aDiscretBis;
  Standard_Boolean aRFlag; 
  
  GeomAbs_CurveType aCurveType;
  aCurveType=C.GetType();
  
  dt=(Tmax-Tmin)/Discret;
  aRFlag=(dt > 1.e-5); 
  for (i=1; i<=Discret; i++) {
    tCurrent=Tmin+(i-1)*dt;
    aPars.Append(tCurrent);
    tNext=tCurrent+dt;
    if (i==Discret)
      tNext=Tmax;
    ///////////////////////////////////////////////////
    if (!aRFlag) {
      continue;
    }
    if (aCurveType==GeomAbs_BSplineCurve||
	aCurveType==GeomAbs_BezierCurve ||
	aCurveType==GeomAbs_OffsetCurve ||
	aCurveType==GeomAbs_Ellipse ||
	aCurveType==GeomAbs_OtherCurve) { //modified by NIZNHY-PKV Fri Sep 24 09:52:42 2004ft
      continue;
    }
    //
    ip=IntTools::GetRadius (C, tCurrent, tNext, aR);  
    if (ip<0) {
      return 1;
    }
    //
    if (!ip) {
      anAbsDeflection=Deflection*aR;
      GCPnts_QuasiUniformDeflection anUD;
      anUD.Initialize (C, anAbsDeflection, tCurrent, tNext);
      if (!anUD.IsDone()) {
	return 2;
      }
      
      aNbDeflectionPoints=anUD.NbPoints();
      if (aNbDeflectionPoints > 2) {
	aNbDeflectionPoints--;
	for (j=2; j<=aNbDeflectionPoints; j++) {
	  tCurrent=anUD.Parameter(j);
	  aPars.Append(tCurrent);
	}
      }
    }
  }

  aPars.Append(Tmax);
  aDiscretBis=aPars.Extent();
  anArgs.Resize(aDiscretBis);
  TColStd_ListIteratorOfListOfReal anIt(aPars);
  for (i=0; anIt.More(); anIt.Next(), i++) {
    anArgs(i)=anIt.Value();
  }
  return 0;
}

//=======================================================================
//function : IntTools::Length
//purpose  : 
//=======================================================================
  Standard_Real IntTools::Length (const TopoDS_Edge& anEdge)
{
  Standard_Real aLength=0;

  if (!BRep_Tool::Degenerated(anEdge) &&
      BRep_Tool::IsGeometric(anEdge)) {

    GProp_GProps Temp;
    BRepGProp::LinearProperties(anEdge, Temp);
    aLength = Temp.Mass();
  }
  return aLength;
}
  
//=======================================================================
//function : RemoveIdenticalRoots 
//purpose  : 
//=======================================================================
  void IntTools::RemoveIdenticalRoots(IntTools_SequenceOfRoots& aSR,
				      const Standard_Real anEpsT)
{
  Standard_Integer aNbRoots, j, k;
  Standard_Real anEpsT2=0.5*anEpsT;
  aNbRoots=aSR.Length();
  for (j=1; j<=aNbRoots; j++) { 
    const IntTools_Root& aRj=aSR(j);
    for (k=j+1; k<=aNbRoots; k++) {
      const IntTools_Root& aRk=aSR(k);
      if (fabs (aRj.Root()-aRk.Root()) < anEpsT2) {
	aSR.Remove(k);
	aNbRoots=aSR.Length();
      }
    }
  }
}

//=======================================================================

namespace {
  // Auxiliary: comparator function for sorting roots
  bool IntTools_RootComparator (const IntTools_Root& theLeft, const IntTools_Root& theRight)
  {
    return theLeft.Root() < theRight.Root();
  }
}

//=======================================================================
//function : SortRoots
//purpose  : 
//=======================================================================
  void IntTools::SortRoots(IntTools_SequenceOfRoots& mySequenceOfRoots,
			   const Standard_Real /*myEpsT*/)
{
  Standard_Integer j, aNbRoots;

  aNbRoots=mySequenceOfRoots.Length();

  IntTools_Array1OfRoots anArray1OfRoots(1, aNbRoots);  
  for (j=1; j<=aNbRoots; j++) {
    anArray1OfRoots(j)=mySequenceOfRoots(j);
  }
  
  std::sort (anArray1OfRoots.begin(), anArray1OfRoots.end(), IntTools_RootComparator);
  
  mySequenceOfRoots.Clear();
  for (j=1; j<=aNbRoots; j++) {
    mySequenceOfRoots.Append(anArray1OfRoots(j));
  }
}
//=======================================================================
//function :FindRootStates
//purpose  : 
//=======================================================================
  void  IntTools::FindRootStates(IntTools_SequenceOfRoots& mySequenceOfRoots,
				 const Standard_Real myEpsNull)
{
  Standard_Integer aType, j, aNbRoots;
  Standard_Real t1, t2, f1, f2, absf2;

  aNbRoots=mySequenceOfRoots.Length();

  for (j=1; j<=aNbRoots; j++) {
    IntTools_Root& aR=mySequenceOfRoots.ChangeValue(j);
    
    aR.Interval (t1, t2, f1, f2);

    aType=aR.Type();
    switch (aType) {
    case 0: // Simple Root
      if (f1>0. && f2<0.) {
	aR.SetStateBefore(TopAbs_OUT);
	aR.SetStateAfter (TopAbs_IN);
      }
    else {
      aR.SetStateBefore(TopAbs_IN);
      aR.SetStateAfter (TopAbs_OUT);
    }
      break;
    
    case 1: // Complete 0;
      aR.SetStateBefore(TopAbs_ON);
      aR.SetStateAfter (TopAbs_ON);
      break;
      
    case 2: // Smart;
      absf2=fabs(f2);
      if (absf2 < myEpsNull) {
	aR.SetStateAfter (TopAbs_ON);
	if (f1>0.) {
	  aR.SetStateBefore(TopAbs_OUT);
	}
	else {
	  aR.SetStateBefore(TopAbs_IN);
	}
      }
      
      else {
	aR.SetStateBefore(TopAbs_ON);
	if (f2>0.) {
	  aR.SetStateAfter (TopAbs_OUT);
	}
	else {
	  aR.SetStateAfter (TopAbs_IN);
	}
      }
      
    default: break;
    } // switch (aType)  
  }
}

#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <ElCLib.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

//=======================================================================
//function :Parameter
//purpose  :
//=======================================================================
  Standard_Integer  IntTools::Parameter (const gp_Pnt& aP,
					 const Handle(Geom_Curve)& aCurve,
					 Standard_Real& aParameter)
{
  Standard_Real aFirst, aLast;
  GeomAbs_CurveType aCurveType;

  aFirst=aCurve->FirstParameter();
  aLast =aCurve->LastParameter ();

  GeomAdaptor_Curve aGAC;
  
  aGAC.Load (aCurve, aFirst, aLast);

  aCurveType=aGAC.GetType();
  
  switch (aCurveType){

  case GeomAbs_Line:
    {
      gp_Lin aLin=aGAC.Line();
      aParameter=ElCLib::Parameter (aLin, aP);
      return 0;
    }
  case GeomAbs_Circle:
    {
      gp_Circ aCircle=aGAC.Circle();
      aParameter=ElCLib::Parameter (aCircle, aP);
      return 0;
    }
  case GeomAbs_Ellipse: 
    {
      gp_Elips aElips=aGAC.Ellipse();
      aParameter=ElCLib::Parameter (aElips, aP);
      return 0;
    }
  case GeomAbs_Hyperbola: 
    {
      gp_Hypr aHypr=aGAC.Hyperbola();
      aParameter=ElCLib::Parameter (aHypr, aP);
      return 0;
    }
  case GeomAbs_Parabola: 
    {
      gp_Parab aParab=aGAC.Parabola();
      aParameter=ElCLib::Parameter (aParab, aP);
      return 0;
    }

  case GeomAbs_BezierCurve:
  case GeomAbs_BSplineCurve:
    {
      GeomAPI_ProjectPointOnCurve aProjector;
      
      aProjector.Init(aP, aCurve, aFirst, aLast);
      Standard_Integer aNbPoints=aProjector.NbPoints();
      if (aNbPoints) {
	aParameter=aProjector.LowerDistanceParameter();
	return 0;
      }
      else {
	return 2;
      }
    }
  default: 
    break;
  }
  return 1;
}
