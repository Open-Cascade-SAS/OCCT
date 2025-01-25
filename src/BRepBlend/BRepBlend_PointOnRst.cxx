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

#include <BRepBlend_PointOnRst.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <IntSurf_Transition.hxx>

BRepBlend_PointOnRst::BRepBlend_PointOnRst()
    : prm(0.0)
{
}

BRepBlend_PointOnRst::BRepBlend_PointOnRst(const Handle(Adaptor2d_Curve2d)& A,
                                           const Standard_Real              Param,
                                           const IntSurf_Transition&        TLine,
                                           const IntSurf_Transition&        TArc)
    :

      arc(A),
      traline(TLine),
      traarc(TArc),
      prm(Param)
{
}

void BRepBlend_PointOnRst::SetArc(const Handle(Adaptor2d_Curve2d)& A,
                                  const Standard_Real              Param,
                                  const IntSurf_Transition&        TLine,
                                  const IntSurf_Transition&        TArc)
{
  arc     = A;
  prm     = Param;
  traline = TLine;
  traarc  = TArc;
}
