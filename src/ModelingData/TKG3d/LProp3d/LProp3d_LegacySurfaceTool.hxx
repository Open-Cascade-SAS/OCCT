// Copyright (c) 2026 OPEN CASCADE SAS
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

// Copyright (c) 2002-2014 OPEN CASCADE SAS

#ifndef _LProp3d_LegacySurfaceTool_HeaderFile
#define _LProp3d_LegacySurfaceTool_HeaderFile

#include <Standard_DefineAlloc.hxx>

#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <algorithm>

class LProp3d_LegacySurfaceTool
{
public:
  DEFINE_STANDARD_ALLOC

  static void Value(const occ::handle<Adaptor3d_Surface>& S,
                    const double                          U,
                    const double                          V,
                    gp_Pnt&                               P)
  {
    P = S->Value(U, V);
  }

  static void D1(const occ::handle<Adaptor3d_Surface>& S,
                 const double                          U,
                 const double                          V,
                 gp_Pnt&                               P,
                 gp_Vec&                               D1U,
                 gp_Vec&                               D1V)
  {
    S->D1(U, V, P, D1U, D1V);
  }

  static void D2(const occ::handle<Adaptor3d_Surface>& S,
                 const double                          U,
                 const double                          V,
                 gp_Pnt&                               P,
                 gp_Vec&                               D1U,
                 gp_Vec&                               D1V,
                 gp_Vec&                               D2U,
                 gp_Vec&                               D2V,
                 gp_Vec&                               DUV)
  {
    S->D2(U, V, P, D1U, D1V, D2U, D2V, DUV);
  }

  static gp_Vec DN(const occ::handle<Adaptor3d_Surface>& S,
                   const double                          U,
                   const double                          V,
                   const int                             IU,
                   const int                             IV)
  {
    return S->DN(U, V, IU, IV);
  }

  static int Continuity(const occ::handle<Adaptor3d_Surface>& S)
  {
    const GeomAbs_Shape s = (GeomAbs_Shape)std::min(S->UContinuity(), S->VContinuity());
    switch (s)
    {
      case GeomAbs_C0:
        return 0;
      case GeomAbs_C1:
        return 1;
      case GeomAbs_C2:
        return 2;
      case GeomAbs_C3:
        return 3;
      case GeomAbs_G1:
      case GeomAbs_G2:
        return 0;
      case GeomAbs_CN:
        return 3;
    }
    return 0;
  }

  static void Bounds(const occ::handle<Adaptor3d_Surface>& S,
                     double&                               U1,
                     double&                               V1,
                     double&                               U2,
                     double&                               V2)
  {
    U1 = S->FirstUParameter();
    V1 = S->FirstVParameter();
    U2 = S->LastUParameter();
    V2 = S->LastVParameter();
  }
};

#endif
