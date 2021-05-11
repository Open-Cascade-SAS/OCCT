// Created on: 1994-09-15
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _ProjLib_ProjectOnSurface_HeaderFile
#define _ProjLib_ProjectOnSurface_HeaderFile

#include <Adaptor3d_Surface.hxx>

class Geom_BSplineCurve;

//! Project a curve on a  surface.  The result ( a  3D
//! Curve)  will be an approximation
class ProjLib_ProjectOnSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Create an empty projector.
  Standard_EXPORT ProjLib_ProjectOnSurface();

  //! Create a projector normally to the surface <S>.
  Standard_EXPORT ProjLib_ProjectOnSurface(const Handle(Adaptor3d_Surface)& S);

  Standard_EXPORT virtual ~ProjLib_ProjectOnSurface();

  //! Set the Surface to <S>.
  //! To compute the projection, you have to Load the Curve.
  Standard_EXPORT void Load (const Handle(Adaptor3d_Surface)& S);

  //! Compute the projection of the curve <C> on the Surface.
  Standard_EXPORT void Load (const Handle(Adaptor3d_Curve)& C, const Standard_Real Tolerance);

  Standard_Boolean IsDone() const { return myIsDone; }

  Standard_EXPORT Handle(Geom_BSplineCurve) BSpline() const;

private:

  Handle(Adaptor3d_Curve) myCurve;
  Handle(Adaptor3d_Surface) mySurface;
  Standard_Real myTolerance;
  Standard_Boolean myIsDone;
  Handle(Geom_BSplineCurve) myResult;

};

#endif // _ProjLib_ProjectOnSurface_HeaderFile
