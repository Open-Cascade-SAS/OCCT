// Created on: 1992-11-10
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntSurf_PathPoint_HeaderFile
#define _IntSurf_PathPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir2d.hxx>
#include <gp_XY.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Integer.hxx>

class IntSurf_PathPoint
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntSurf_PathPoint();

  Standard_EXPORT IntSurf_PathPoint(const gp_Pnt& P, const double U, const double V);

  Standard_EXPORT void SetValue(const gp_Pnt& P, const double U, const double V);

  void AddUV(const double U, const double V);

  void SetDirections(const gp_Vec& V, const gp_Dir2d& D);

  void SetTangency(const bool Tang);

  void SetPassing(const bool Pass);

  const gp_Pnt& Value() const;

  void Value2d(double& U, double& V) const;

  bool IsPassingPnt() const;

  bool IsTangent() const;

  const gp_Vec& Direction3d() const;

  const gp_Dir2d& Direction2d() const;

  int Multiplicity() const;

  void Parameters(const int Index, double& U, double& V) const;

private:
  gp_Pnt                       pt;
  bool             ispass;
  bool             istgt;
  gp_Vec                       vectg;
  gp_Dir2d                     dirtg;
  occ::handle<NCollection_HSequence<gp_XY>> sequv;
};

#include <IntSurf_PathPoint.lxx>

#endif // _IntSurf_PathPoint_HeaderFile
