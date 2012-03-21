// Created on: 1993-07-02
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#include <StepToGeom_MakeBSplineSurface.ixx>
#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <StepGeom_HArray2OfCartesianPoint.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepToGeom_MakeCartesianPoint.hxx>
#include <Geom_CartesianPoint.hxx>
#include <gp_Pnt.hxx>

//=============================================================================
// Creation d' une BSplineSurface de Geom a partir d' une
// BSplineSurface de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeBSplineSurface::Convert
    (const Handle(StepGeom_BSplineSurface)& SS,
     Handle(Geom_BSplineSurface)& CS)
{
  Standard_Integer                    i, j;
  Handle(StepGeom_BSplineSurfaceWithKnots) BS;
  Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface) BSR;

  if (SS->
      IsKind(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface))) {
    BSR = 
      Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)
	::DownCast(SS);
    BS = 
      Handle(StepGeom_BSplineSurfaceWithKnots)
	::DownCast(BSR->BSplineSurfaceWithKnots());
  }
  else
    BS = Handle(StepGeom_BSplineSurfaceWithKnots)::DownCast(SS);

  const Standard_Integer UDeg = BS->UDegree();
  const Standard_Integer VDeg = BS->VDegree();
  const Standard_Integer NUPoles = BS->NbControlPointsListI();
  const Standard_Integer NVPoles = BS->NbControlPointsListJ();
  const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList = BS->ControlPointsList();
  Handle(Geom_CartesianPoint) P;
  TColgp_Array2OfPnt Poles(1,NUPoles,1,NVPoles);
  for (i=1; i<=NUPoles; i++) {
    for (j=1; j<=NVPoles; j++) {
      if (StepToGeom_MakeCartesianPoint::Convert(aControlPointsList->Value(i,j),P))
        Poles.SetValue(i,j,P->Pnt());
	  else
        return Standard_False;
    }
  }
  const Standard_Integer NUKnots = BS->NbUMultiplicities();
  const Handle(TColStd_HArray1OfInteger)& aUMultiplicities = BS->UMultiplicities();
  TColStd_Array1OfInteger UMult(1,NUKnots);
  for (i=1; i<=NUKnots; i++) {
    UMult.SetValue(i,aUMultiplicities->Value(i));
  }
  const Standard_Integer NVKnots = BS->NbVMultiplicities();
  const Handle(TColStd_HArray1OfInteger)& aVMultiplicities = BS->VMultiplicities();
  TColStd_Array1OfInteger VMult(1,NVKnots);
  for (i=1; i<=NVKnots; i++) {
    VMult.SetValue(i,aVMultiplicities->Value(i));
  }
  const Handle(TColStd_HArray1OfReal)& aUKnots = BS->UKnots();
  TColStd_Array1OfReal KUn(1,NUKnots);
  for (i=1; i<=NUKnots; i++) {
    KUn.SetValue(i,aUKnots->Value(i));
  }
  const Handle(TColStd_HArray1OfReal)& aVKnots = BS->VKnots();
  TColStd_Array1OfReal KVn(1,NVKnots);
  for (i=1; i<=NVKnots; i++) {
    KVn.SetValue(i,aVKnots->Value(i));
  }
  
  // --- Does the Surface Descriptor LOOKS like a U and/or V Periodic ---
  // --- Descriptor ? ---
  
  // --- U Periodic ? ---
  
  Standard_Integer SumMult = 0;
  for (i=1; i<=NUKnots; i++) {
    SumMult += aUMultiplicities->Value(i);
  }
  
  Standard_Boolean shouldBeUPeriodic = Standard_False;
  if (SumMult == (NUPoles + UDeg + 1)) {
    //shouldBeUPeriodic = Standard_False;
  }
  else if ((aUMultiplicities->Value(1) == 
	    aUMultiplicities->Value(NUKnots)) &&
	   ((SumMult - aUMultiplicities->Value(1))== NUPoles)) {
    shouldBeUPeriodic = Standard_True;
  }
  /*else {  // --- What is that ??? ---
    shouldBeUPeriodic = Standard_False;
#ifdef DEBUG
    cout << "Strange BSpline Surface Descriptor" << endl;
#endif
  }*/
  
  // --- V Periodic ? ---
  
  SumMult = 0;
  for (i=1; i<=NVKnots; i++) {
    SumMult += aVMultiplicities->Value(i);
  }
  
  Standard_Boolean shouldBeVPeriodic = Standard_False;
  if (SumMult == (NVPoles + VDeg + 1)) {
    //shouldBeVPeriodic = Standard_False;
  }
  else if ((aVMultiplicities->Value(1) == 
	    aVMultiplicities->Value(NVKnots)) &&
	   ((SumMult - aVMultiplicities->Value(1)) == NVPoles)) {
    shouldBeVPeriodic = Standard_True;
  }
  /*else {  // --- What is that ??? ---
    shouldBeVPeriodic = Standard_False;
#ifdef DEBUG
    cout << "Strange BSpline Surface Descriptor" << endl;
#endif
  }*/
  
  if (SS->IsKind(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface))) {
    const Handle(TColStd_HArray2OfReal)& aWeight = BSR->WeightsData();
    TColStd_Array2OfReal W(1,NUPoles,1,NVPoles);
    for (i=1; i<=NUPoles; i++) {
      for (j=1; j<=NVPoles; j++) {
        W.SetValue(i,j,aWeight->Value(i,j));
      }
    }
    CS = new Geom_BSplineSurface(Poles, W, KUn, KVn, UMult,
                                 VMult, UDeg, VDeg,
                                 shouldBeUPeriodic,
                                 shouldBeVPeriodic);
  }
  else
    CS = new Geom_BSplineSurface(Poles, KUn, KVn, UMult,
                                 VMult, UDeg, VDeg,
                                 shouldBeUPeriodic,
                                 shouldBeVPeriodic);
  return Standard_True;
}
