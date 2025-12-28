// Created on: 1993-06-21
// Created by: Martine LANGLOIS
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

#include <Geom2d_Circle.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>
#include <GeomToStep_MakeCircle.hxx>
#include <GeomToStep_MakeConic.hxx>
#include <GeomToStep_MakeEllipse.hxx>
#include <GeomToStep_MakeHyperbola.hxx>
#include <GeomToStep_MakeParabola.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepGeom_Circle.hxx>
#include <StepGeom_Conic.hxx>
#include <StepGeom_Ellipse.hxx>
#include <StepGeom_Hyperbola.hxx>
#include <StepGeom_Parabola.hxx>

//=============================================================================
// Creation d' une Conic de prostep a partir d' une Conic de Geom
//=============================================================================
GeomToStep_MakeConic::GeomToStep_MakeConic(const occ::handle<Geom_Conic>& C,
                                           const StepData_Factors&        theLocalFactors)
{
  done = true;
  if (C->IsKind(STANDARD_TYPE(Geom_Circle)))
  {
    occ::handle<Geom_Circle> Cer = occ::down_cast<Geom_Circle>(C);
    GeomToStep_MakeCircle    MkCircle(Cer, theLocalFactors);
    theConic = MkCircle.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_Ellipse)))
  {
    occ::handle<Geom_Ellipse> Ell = occ::down_cast<Geom_Ellipse>(C);
    GeomToStep_MakeEllipse    MkEllipse(Ell, theLocalFactors);
    theConic = MkEllipse.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_Hyperbola)))
  {
    occ::handle<Geom_Hyperbola> Hyp = occ::down_cast<Geom_Hyperbola>(C);
    GeomToStep_MakeHyperbola    MkHyperbola(Hyp, theLocalFactors);
    theConic = MkHyperbola.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_Parabola)))
  {
    occ::handle<Geom_Parabola> Par = occ::down_cast<Geom_Parabola>(C);
    GeomToStep_MakeParabola    MkParabola(Par, theLocalFactors);
    theConic = MkParabola.Value();
  }
  else
  {
#ifdef OCCT_DEBUG
    std::cout << "3D Curve Type   : " << C->DynamicType() << std::endl;
#endif
    done = false;
  }
}

//=============================================================================
// Creation d' une Conic2d de prostep a partir d' une Conic de Geom2d
//=============================================================================

GeomToStep_MakeConic::GeomToStep_MakeConic(const occ::handle<Geom2d_Conic>& C,
                                           const StepData_Factors&          theLocalFactors)
{
  done = true;
  if (C->IsKind(STANDARD_TYPE(Geom2d_Circle)))
  {
    occ::handle<Geom2d_Circle> Cer = occ::down_cast<Geom2d_Circle>(C);
    GeomToStep_MakeCircle      MkCircle(Cer, theLocalFactors);
    theConic = MkCircle.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_Ellipse)))
  {
    occ::handle<Geom2d_Ellipse> Ell = occ::down_cast<Geom2d_Ellipse>(C);
    GeomToStep_MakeEllipse      MkEllipse(Ell, theLocalFactors);
    theConic = MkEllipse.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_Hyperbola)))
  {
    occ::handle<Geom2d_Hyperbola> Hyp = occ::down_cast<Geom2d_Hyperbola>(C);
    GeomToStep_MakeHyperbola      MkHyperbola(Hyp, theLocalFactors);
    theConic = MkHyperbola.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_Parabola)))
  {
    occ::handle<Geom2d_Parabola> Par = occ::down_cast<Geom2d_Parabola>(C);
    GeomToStep_MakeParabola      MkParabola(Par, theLocalFactors);
    theConic = MkParabola.Value();
  }
  else
  {
    // Attention : Other 2d conics shall be implemented ...
    //             To be performed later !
#ifdef OCCT_DEBUG
    std::cout << "2D conic not yet implemented" << std::endl;
#endif
    done = false;
  }
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const occ::handle<StepGeom_Conic>& GeomToStep_MakeConic::Value() const
{
  StdFail_NotDone_Raise_if(!done, "GeomToStep_MakeConic::Value() - no result");
  return theConic;
}
