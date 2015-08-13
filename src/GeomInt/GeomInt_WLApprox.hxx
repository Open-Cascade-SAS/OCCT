// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
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

#ifndef _GeomInt_WLApprox_HeaderFile
#define _GeomInt_WLApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomInt_TheComputeLineOfWLApprox.hxx>
#include <GeomInt_TheComputeLineBezierOfWLApprox.hxx>
#include <Approx_MCurvesToBSpCurve.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Approx_ParametrizationType.hxx>
class Standard_OutOfRange;
class StdFail_NotDone;
class Adaptor3d_HSurface;
class Adaptor3d_HSurfaceTool;
class IntSurf_Quadric;
class IntSurf_QuadricTool;
class IntPatch_WLine;
class GeomInt_ThePrmPrmSvSurfacesOfWLApprox;
class GeomInt_TheInt2SOfThePrmPrmSvSurfacesOfWLApprox;
class GeomInt_TheImpPrmSvSurfacesOfWLApprox;
class GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox;
class GeomInt_TheMultiLineOfWLApprox;
class GeomInt_TheMultiLineToolOfWLApprox;
class GeomInt_TheComputeLineOfWLApprox;
class GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox;
class GeomInt_MyGradientbisOfTheComputeLineOfWLApprox;
class GeomInt_TheComputeLineBezierOfWLApprox;
class GeomInt_MyGradientOfTheComputeLineBezierOfWLApprox;
class AppParCurves_MultiBSpCurve;



class GeomInt_WLApprox 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomInt_WLApprox();
  
  Standard_EXPORT void Perform (const Handle(Adaptor3d_HSurface)& Surf1, const Handle(Adaptor3d_HSurface)& Surf2, const Handle(IntPatch_WLine)& aLine, const Standard_Boolean ApproxXYZ = Standard_True, const Standard_Boolean ApproxU1V1 = Standard_True, const Standard_Boolean ApproxU2V2 = Standard_True, const Standard_Integer indicemin = 0, const Standard_Integer indicemax = 0);
  
  Standard_EXPORT void Perform (const Handle(IntPatch_WLine)& aLine, const Standard_Boolean ApproxXYZ = Standard_True, const Standard_Boolean ApproxU1V1 = Standard_True, const Standard_Boolean ApproxU2V2 = Standard_True, const Standard_Integer indicemin = 0, const Standard_Integer indicemax = 0);
  
  Standard_EXPORT void SetParameters (const Standard_Real Tol3d, const Standard_Real Tol2d, const Standard_Integer DegMin, const Standard_Integer DegMax, const Standard_Integer NbIterMax, const Standard_Boolean ApproxWithTangency = Standard_True, const Approx_ParametrizationType Parametrization = Approx_ChordLength);
  
  Standard_EXPORT void SetParameters (const Standard_Real Tol3d, const Standard_Real Tol2d, const Standard_Boolean RelativeTol, const Standard_Integer DegMin, const Standard_Integer DegMax, const Standard_Integer NbIterMax, const Standard_Integer NbPntMax, const Standard_Boolean ApproxWithTangency = Standard_True, const Approx_ParametrizationType Parametrization = Approx_ChordLength);
  
  Standard_EXPORT void Perform();
  
  Standard_EXPORT Standard_Real TolReached3d() const;
  
  Standard_EXPORT Standard_Real TolReached2d() const;
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Integer NbMultiCurves() const;
  
  Standard_EXPORT const AppParCurves_MultiBSpCurve& Value (const Standard_Integer Index) const;




protected:





private:

  Standard_EXPORT Standard_Integer CorrectFinishIdx(const Standard_Integer theMinIdx,
                                                    const Standard_Integer theMaxIdx,
                                                    const Handle(IntPatch_WLine)& theline);

  Standard_EXPORT void Perform (const Handle(Adaptor3d_HSurface)& Surf1, const IntSurf_Quadric& Surf2, const Handle(IntPatch_WLine)& aLine, const Standard_Boolean ApproxXYZ, const Standard_Boolean ApproxU1V1, const Standard_Boolean ApproxU2V2, const Standard_Integer indicemin, const Standard_Integer indicemax);
  
  Standard_EXPORT void Perform (const IntSurf_Quadric& Surf1, const Handle(Adaptor3d_HSurface)& Surf2, const Handle(IntPatch_WLine)& aLine, const Standard_Boolean ApproxXYZ, const Standard_Boolean ApproxU1V1, const Standard_Boolean ApproxU2V2, const Standard_Integer indicemin, const Standard_Integer indicemax);
  
  Standard_EXPORT void UpdateTolReached();


  GeomInt_TheComputeLineOfWLApprox myComputeLine;
  GeomInt_TheComputeLineBezierOfWLApprox myComputeLineBezier;
  Approx_MCurvesToBSpCurve myBezToBSpl;
  Standard_Boolean myTolReached;
  Standard_Boolean myApproxBez;
  Standard_Boolean myWithTangency;
  Standard_Real myTol3d;
  Standard_Real myTol2d;
  Standard_Boolean myRelativeTol;
  Standard_Integer myDegMin;
  Standard_Integer myDegMax;
  Standard_Integer myNbPntMax;
  Standard_Integer myNbIterMax;
  Standard_Real myMinFactorXYZ;
  Standard_Real myMinFactorUV;
  Standard_Real myTolReached3d;
  Standard_Real myTolReached2d;


};







#endif // _GeomInt_WLApprox_HeaderFile
