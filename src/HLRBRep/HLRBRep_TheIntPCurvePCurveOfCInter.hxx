// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRBRep_TheIntPCurvePCurveOfCInter_HeaderFile
#define _HLRBRep_TheIntPCurvePCurveOfCInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntRes2d_Domain.hxx>
#include <IntRes2d_Intersection.hxx>
#include <Standard_Address.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class HLRBRep_CurveTool;
class HLRBRep_TheProjPCurOfCInter;
class HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter;
class HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter;
class HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter;
class IntRes2d_Domain;



class HLRBRep_TheIntPCurvePCurveOfCInter  : public IntRes2d_Intersection
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT HLRBRep_TheIntPCurvePCurveOfCInter();
  
  Standard_EXPORT void Perform (const Standard_Address& Curve1, const IntRes2d_Domain& Domain1, const Standard_Address& Curve2, const IntRes2d_Domain& Domain2, const Standard_Real TolConf, const Standard_Real Tol);
  
  Standard_EXPORT void Perform (const Standard_Address& Curve1, const IntRes2d_Domain& Domain1, const Standard_Real TolConf, const Standard_Real Tol);

  //! Set / get minimum number of points in polygon for intersection.
  Standard_EXPORT void SetMinNbSamples (const Standard_Integer theMinNbSamples);
  Standard_EXPORT Standard_Integer GetMinNbSamples () const;

protected:

  
  Standard_EXPORT void Perform (const Standard_Address& Curve1, const IntRes2d_Domain& Domain1, const Standard_Address& Curve2, const IntRes2d_Domain& Domain2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Integer NbIter, const Standard_Real DeltaU, const Standard_Real DeltaV);
  
  Standard_EXPORT void Perform (const Standard_Address& Curve1, const IntRes2d_Domain& Domain1, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Integer NbIter, const Standard_Real DeltaU, const Standard_Real DeltaV);




private:

  
  //! Method to find intersection between two curves
  //! :  returns false for case when some points of polygon
  //! : were replaced on line and exact point of intersection was not found
  //! : for case when point of intersection was found
  //! : during prelimanary search for line (case of bad paramerization of Bspline for example).
  Standard_EXPORT Standard_Boolean findIntersect (const Standard_Address& Curve1, const IntRes2d_Domain& Domain1, const Standard_Address& Curve2, const IntRes2d_Domain& Domain2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Integer NbIter, const Standard_Real DeltaU, const Standard_Real DeltaV, const HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter& thePoly1, const HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter& thePoly2, const Standard_Boolean isFullRepresentation);


  IntRes2d_Domain DomainOnCurve1;
  IntRes2d_Domain DomainOnCurve2;

  //! Minimal number of sample points
  Standard_Integer myMinPntNb;
};







#endif // _HLRBRep_TheIntPCurvePCurveOfCInter_HeaderFile
