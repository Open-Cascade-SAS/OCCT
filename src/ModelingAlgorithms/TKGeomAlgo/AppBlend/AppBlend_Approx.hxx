// Created on: 1996-08-27
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AppBlend_Approx_HeaderFile
#define _AppBlend_Approx_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>

//! Bspline approximation of a surface.
class AppBlend_Approx
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT virtual bool IsDone() const = 0;

  Standard_EXPORT virtual void SurfShape(int& UDegree,
                                         int& VDegree,
                                         int& NbUPoles,
                                         int& NbVPoles,
                                         int& NbUKnots,
                                         int& NbVKnots) const = 0;

  Standard_EXPORT virtual void Surface(NCollection_Array2<gp_Pnt>&      TPoles,
                                       NCollection_Array2<double>&    TWeights,
                                       NCollection_Array1<double>&    TUKnots,
                                       NCollection_Array1<double>&    TVKnots,
                                       NCollection_Array1<int>& TUMults,
                                       NCollection_Array1<int>& TVMults) const = 0;

  Standard_EXPORT virtual int UDegree() const = 0;

  Standard_EXPORT virtual int VDegree() const = 0;

  Standard_EXPORT virtual const NCollection_Array2<gp_Pnt>& SurfPoles() const = 0;

  Standard_EXPORT virtual const NCollection_Array2<double>& SurfWeights() const = 0;

  Standard_EXPORT virtual const NCollection_Array1<double>& SurfUKnots() const = 0;

  Standard_EXPORT virtual const NCollection_Array1<double>& SurfVKnots() const = 0;

  Standard_EXPORT virtual const NCollection_Array1<int>& SurfUMults() const = 0;

  Standard_EXPORT virtual const NCollection_Array1<int>& SurfVMults() const = 0;

  Standard_EXPORT virtual int NbCurves2d() const = 0;

  Standard_EXPORT virtual void Curves2dShape(int& Degree,
                                             int& NbPoles,
                                             int& NbKnots) const = 0;

  Standard_EXPORT virtual void Curve2d(const int   Index,
                                       NCollection_Array1<gp_Pnt2d>&    TPoles,
                                       NCollection_Array1<double>&    TKnots,
                                       NCollection_Array1<int>& TMults) const = 0;

  Standard_EXPORT virtual int Curves2dDegree() const = 0;

  Standard_EXPORT virtual const NCollection_Array1<gp_Pnt2d>& Curve2dPoles(
    const int Index) const = 0;

  Standard_EXPORT virtual const NCollection_Array1<double>& Curves2dKnots() const = 0;

  Standard_EXPORT virtual const NCollection_Array1<int>& Curves2dMults() const = 0;

  Standard_EXPORT virtual void TolReached(double& Tol3d, double& Tol2d) const = 0;

  Standard_EXPORT virtual double TolCurveOnSurf(const int Index) const = 0;
  Standard_EXPORT virtual ~AppBlend_Approx();

};

#endif // _AppBlend_Approx_HeaderFile
