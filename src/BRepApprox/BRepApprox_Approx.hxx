// Created on: 1995-06-06
// Created by: Jean Yves LEBEY
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

#ifndef _BRepApprox_Approx_HeaderFile
#define _BRepApprox_Approx_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_Vector.hxx>
#include <BRepApprox_TheComputeLineOfApprox.hxx>
#include <BRepApprox_TheComputeLineBezierOfApprox.hxx>
#include <Approx_MCurvesToBSpCurve.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Approx_ParametrizationType.hxx>
class Standard_OutOfRange;
class StdFail_NotDone;
class BRepAdaptor_Surface;
class BRepApprox_SurfaceTool;
class IntSurf_Quadric;
class IntSurf_QuadricTool;
class BRepApprox_ApproxLine;
class BRepApprox_ThePrmPrmSvSurfacesOfApprox;
class BRepApprox_TheInt2SOfThePrmPrmSvSurfacesOfApprox;
class BRepApprox_TheImpPrmSvSurfacesOfApprox;
class BRepApprox_TheZerImpFuncOfTheImpPrmSvSurfacesOfApprox;
class BRepApprox_TheMultiLineOfApprox;
class BRepApprox_TheMultiLineToolOfApprox;
class BRepApprox_TheComputeLineOfApprox;
class BRepApprox_MyBSplGradientOfTheComputeLineOfApprox;
class BRepApprox_MyGradientbisOfTheComputeLineOfApprox;
class BRepApprox_TheComputeLineBezierOfApprox;
class BRepApprox_MyGradientOfTheComputeLineBezierOfApprox;
class AppParCurves_MultiBSpCurve;

struct Approx_Data 
{
  Approx_Data() : myBezierApprox(Standard_True),
                  Xo(0.0), Yo(0.0), Zo(0.0),
                  U1o(0.0), V1o(0.0), U2o(0.0), V2o(0.0),
                  ApproxXYZ(Standard_True),
                  ApproxU1V1(Standard_True),
                  ApproxU2V2(Standard_True),
                  indicemin(0), indicemax(0),
                  myNbPntMax(30), parametrization(Approx_ChordLength)
  {
  }

  Standard_Boolean myBezierApprox;
  Standard_Real  Xo, Yo, Zo, U1o, V1o, U2o, V2o;
  Standard_Boolean ApproxXYZ, ApproxU1V1, ApproxU2V2;
  Standard_Integer indicemin, indicemax, myNbPntMax;
  Approx_ParametrizationType parametrization;
};

class BRepApprox_Approx 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepApprox_Approx();
  
  Standard_EXPORT void Perform (const BRepAdaptor_Surface& Surf1, const BRepAdaptor_Surface& Surf2, const Handle(BRepApprox_ApproxLine)& aLine, const Standard_Boolean ApproxXYZ = Standard_True, const Standard_Boolean ApproxU1V1 = Standard_True, const Standard_Boolean ApproxU2V2 = Standard_True, const Standard_Integer indicemin = 0, const Standard_Integer indicemax = 0);
  
  Standard_EXPORT void Perform (const Handle(BRepApprox_ApproxLine)& aLine, const Standard_Boolean ApproxXYZ = Standard_True, const Standard_Boolean ApproxU1V1 = Standard_True, const Standard_Boolean ApproxU2V2 = Standard_True, const Standard_Integer indicemin = 0, const Standard_Integer indicemax = 0);
  
  Standard_EXPORT 
      void SetParameters (const Standard_Real Tol3d, const Standard_Real Tol2d,
                          const Standard_Integer DegMin, 
                          const Standard_Integer DegMax,
                          const Standard_Integer NbIterMax,
                          const Standard_Integer NbPntMax = 30,
                          const Standard_Boolean ApproxWithTangency = Standard_True,
                          const Approx_ParametrizationType
                                          Parametrization = Approx_ChordLength);

  Standard_EXPORT void Perform();
  
  Standard_EXPORT Standard_Real TolReached3d() const;
  
  Standard_EXPORT Standard_Real TolReached2d() const;
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Integer NbMultiCurves() const;
  
  Standard_EXPORT const AppParCurves_MultiBSpCurve& Value (const Standard_Integer Index) const;




protected:





private:
  Standard_EXPORT void Perform (const IntSurf_Quadric& Surf1, const BRepAdaptor_Surface& Surf2, const Handle(BRepApprox_ApproxLine)& aLine, const Standard_Boolean ApproxXYZ, const Standard_Boolean ApproxU1V1, const Standard_Boolean ApproxU2V2, const Standard_Integer indicemin, const Standard_Integer indicemax, const Standard_Boolean isTheQuadFirst);
  
  Standard_EXPORT void UpdateTolReached();

  //! Fill data structure for intersection approximation.
  Standard_EXPORT void fillData(const Handle(BRepApprox_ApproxLine)& theLine);

  //! Prepare data structure for further computations.
  Standard_EXPORT void prepareDS(const Standard_Boolean theApproxXYZ,
                                 const Standard_Boolean theApproxU1V1,
                                 const Standard_Boolean theApproxU2V2,
                                 const Standard_Integer indicemin,
                                 const Standard_Integer indicemax);

  //! Build knot sequence.
  Standard_EXPORT void buildKnots(const Handle(BRepApprox_ApproxLine)& theline,
                                  const Standard_Address thePtrSVSurf);

  //! Build curve.
  Standard_EXPORT void buildCurve(const Handle(BRepApprox_ApproxLine)& theline,
                                  const Standard_Address thePtrSVSurf);

  BRepApprox_TheComputeLineOfApprox myComputeLine;
  BRepApprox_TheComputeLineBezierOfApprox myComputeLineBezier;
  Approx_MCurvesToBSpCurve myBezToBSpl;
  Standard_Boolean myWithTangency;
  Standard_Real myTol3d;
  Standard_Real myTol2d;
  Standard_Integer myDegMin;
  Standard_Integer myDegMax;
  Standard_Integer myNbIterMax;
  Standard_Real myTolReached3d;
  Standard_Real myTolReached2d;
  Approx_Data myData;
  NCollection_Vector<Standard_Integer> myKnots;

};







#endif // _BRepApprox_Approx_HeaderFile
