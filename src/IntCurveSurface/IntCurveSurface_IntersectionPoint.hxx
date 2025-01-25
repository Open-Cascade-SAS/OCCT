// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IntCurveSurface_IntersectionPoint_HeaderFile
#define _IntCurveSurface_IntersectionPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <IntCurveSurface_TransitionOnCurve.hxx>

//! Definition of an interserction point between a
//! curve and a surface.
class IntCurveSurface_IntersectionPoint
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty Constructor.
  Standard_EXPORT IntCurveSurface_IntersectionPoint();

  //! Create an IntersectionPoint.
  Standard_EXPORT IntCurveSurface_IntersectionPoint(const gp_Pnt&                           P,
                                                    const Standard_Real                     USurf,
                                                    const Standard_Real                     VSurf,
                                                    const Standard_Real                     UCurv,
                                                    const IntCurveSurface_TransitionOnCurve TrCurv);

  //! Set the fields of the current IntersectionPoint.
  Standard_EXPORT void SetValues(const gp_Pnt&                           P,
                                 const Standard_Real                     USurf,
                                 const Standard_Real                     VSurf,
                                 const Standard_Real                     UCurv,
                                 const IntCurveSurface_TransitionOnCurve TrCurv);

  //! Get the fields of the current IntersectionPoint.
  Standard_EXPORT void Values(gp_Pnt&                            P,
                              Standard_Real&                     USurf,
                              Standard_Real&                     VSurf,
                              Standard_Real&                     UCurv,
                              IntCurveSurface_TransitionOnCurve& TrCurv) const;

  //! returns the geometric point.
  const gp_Pnt& Pnt() const;

  //! returns the U parameter on the surface.
  Standard_Real U() const;

  //! returns the V parameter on the surface.
  Standard_Real V() const;

  //! returns the parameter on the curve.
  Standard_Real W() const;

  //! returns the Transition of the point.
  IntCurveSurface_TransitionOnCurve Transition() const;

  //! Dump all the fields.
  Standard_EXPORT void Dump() const;

protected:
private:
  gp_Pnt                            myP;
  Standard_Real                     myUSurf;
  Standard_Real                     myVSurf;
  Standard_Real                     myUCurv;
  IntCurveSurface_TransitionOnCurve myTrOnCurv;
};

#include <IntCurveSurface_IntersectionPoint.lxx>

#endif // _IntCurveSurface_IntersectionPoint_HeaderFile
