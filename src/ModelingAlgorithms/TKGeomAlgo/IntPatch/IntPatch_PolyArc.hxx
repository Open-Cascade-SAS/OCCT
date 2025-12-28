// Created on: 1993-01-27
// Created by: Isabelle GRIGNON
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

#ifndef _IntPatch_PolyArc_HeaderFile
#define _IntPatch_PolyArc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <IntPatch_Polygo.hxx>
#include <Standard_Integer.hxx>
class Bnd_Box2d;
class gp_Pnt2d;

class IntPatch_PolyArc : public IntPatch_Polygo
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates the polygon of the arc A on the surface S.
  //! The arc is limited by the parameters Pfirst and Plast.
  //! None of these parameters can be infinite.
  Standard_EXPORT IntPatch_PolyArc(const occ::handle<Adaptor2d_Curve2d>& A,
                                   const int                             NbSample,
                                   const double                          Pfirst,
                                   const double                          Plast,
                                   const Bnd_Box2d&                      BoxOtherPolygon);

  Standard_EXPORT bool Closed() const override;

  Standard_EXPORT int NbPoints() const override;

  Standard_EXPORT gp_Pnt2d Point(const int Index) const override;

  Standard_EXPORT double Parameter(const int Index) const;

  Standard_EXPORT void SetOffset(const double OffsetX, const double OffsetY);

private:
  NCollection_Array1<gp_Pnt2d> brise;
  NCollection_Array1<double>   param;
  double                       offsetx;
  double                       offsety;
  bool                         ferme;
};

#endif // _IntPatch_PolyArc_HeaderFile
