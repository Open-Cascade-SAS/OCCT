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

class BRepApprox_Approx
{
private:
  struct Approx_Data
  {
    Approx_Data()
        : myBezierApprox(true),
          Xo(0.0),
          Yo(0.0),
          Zo(0.0),
          U1o(0.0),
          V1o(0.0),
          U2o(0.0),
          V2o(0.0),
          ApproxXYZ(true),
          ApproxU1V1(true),
          ApproxU2V2(true),
          indicemin(0),
          indicemax(0),
          myNbPntMax(30),
          parametrization(Approx_ChordLength)
    {
    }

    bool                       myBezierApprox;
    double                     Xo, Yo, Zo, U1o, V1o, U2o, V2o;
    bool                       ApproxXYZ, ApproxU1V1, ApproxU2V2;
    int                        indicemin, indicemax, myNbPntMax;
    Approx_ParametrizationType parametrization;
  };

public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepApprox_Approx();

  Standard_EXPORT void Perform(const BRepAdaptor_Surface&                Surf1,
                               const BRepAdaptor_Surface&                Surf2,
                               const occ::handle<BRepApprox_ApproxLine>& aLine,
                               const bool                                ApproxXYZ  = true,
                               const bool                                ApproxU1V1 = true,
                               const bool                                ApproxU2V2 = true,
                               const int                                 indicemin  = 0,
                               const int                                 indicemax  = 0);

  Standard_EXPORT void Perform(const occ::handle<BRepApprox_ApproxLine>& aLine,
                               const bool                                ApproxXYZ  = true,
                               const bool                                ApproxU1V1 = true,
                               const bool                                ApproxU2V2 = true,
                               const int                                 indicemin  = 0,
                               const int                                 indicemax  = 0);

  Standard_EXPORT void SetParameters(
    const double                     Tol3d,
    const double                     Tol2d,
    const int                        DegMin,
    const int                        DegMax,
    const int                        NbIterMax,
    const int                        NbPntMax           = 30,
    const bool                       ApproxWithTangency = true,
    const Approx_ParametrizationType Parametrization    = Approx_ChordLength);

  Standard_EXPORT void Perform();

  Standard_EXPORT double TolReached3d() const;

  Standard_EXPORT double TolReached2d() const;

  Standard_EXPORT bool IsDone() const;

  Standard_EXPORT int NbMultiCurves() const;

  Standard_EXPORT const AppParCurves_MultiBSpCurve& Value(const int Index) const;

  Standard_EXPORT static void Parameters(const BRepApprox_TheMultiLineOfApprox& Line,
                                         const int                              firstP,
                                         const int                              lastP,
                                         const Approx_ParametrizationType       Par,
                                         math_Vector&                           TheParameters);

private:
  Standard_EXPORT void Perform(const IntSurf_Quadric&                    Surf1,
                               const BRepAdaptor_Surface&                Surf2,
                               const occ::handle<BRepApprox_ApproxLine>& aLine,
                               const bool                                ApproxXYZ,
                               const bool                                ApproxU1V1,
                               const bool                                ApproxU2V2,
                               const int                                 indicemin,
                               const int                                 indicemax,
                               const bool                                isTheQuadFirst);

  Standard_EXPORT void UpdateTolReached();

  //! Fill data structure for intersection approximation.
  Standard_EXPORT void fillData(const occ::handle<BRepApprox_ApproxLine>& theLine);

  //! Prepare data structure for further computations.
  Standard_EXPORT void prepareDS(const bool theApproxXYZ,
                                 const bool theApproxU1V1,
                                 const bool theApproxU2V2,
                                 const int  indicemin,
                                 const int  indicemax);

  //! Build knot sequence.
  Standard_EXPORT void buildKnots(const occ::handle<BRepApprox_ApproxLine>& theline,
                                  void* const                               thePtrSVSurf);

  //! Build curve.
  Standard_EXPORT void buildCurve(const occ::handle<BRepApprox_ApproxLine>& theline,
                                  void* const                               thePtrSVSurf);

  BRepApprox_TheComputeLineOfApprox       myComputeLine;
  BRepApprox_TheComputeLineBezierOfApprox myComputeLineBezier;
  Approx_MCurvesToBSpCurve                myBezToBSpl;
  bool                                    myWithTangency;
  double                                  myTol3d;
  double                                  myTol2d;
  int                                     myDegMin;
  int                                     myDegMax;
  int                                     myNbIterMax;
  double                                  myTolReached3d;
  double                                  myTolReached2d;
  Approx_Data                             myData;
  NCollection_Vector<int>                 myKnots;
};

#endif // _BRepApprox_Approx_HeaderFile
