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

#include <Adaptor3d_Curve.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <GeomBndLib_Curve.hxx>

//=================================================================================================

void BndLib_Add3dCurve::Add(const Adaptor3d_Curve& C, const double Tol, Bnd_Box& B)
{
  GeomBndLib_Curve(C).Add(Tol, B);
}

//=================================================================================================

void BndLib_Add3dCurve::Add(const Adaptor3d_Curve& C,
                            const double           U1,
                            const double           U2,
                            const double           Tol,
                            Bnd_Box&               B)
{
  GeomBndLib_Curve(C).Add(U1, U2, Tol, B);
}

//=================================================================================================

void BndLib_Add3dCurve::AddOptimal(const Adaptor3d_Curve& C, const double Tol, Bnd_Box& B)
{
  GeomBndLib_Curve(C).AddOptimal(Tol, B);
}

//=================================================================================================

void BndLib_Add3dCurve::AddOptimal(const Adaptor3d_Curve& C,
                                   const double           U1,
                                   const double           U2,
                                   const double           Tol,
                                   Bnd_Box&               B)
{
  GeomBndLib_Curve(C).AddOptimal(U1, U2, Tol, B);
}
