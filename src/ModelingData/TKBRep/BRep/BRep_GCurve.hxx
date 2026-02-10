// Created on: 1995-03-09
// Created by: Laurent PAINNOT
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

#ifndef _BRep_GCurve_HeaderFile
#define _BRep_GCurve_HeaderFile

#include <Standard.hxx>

#include <Standard_Real.hxx>
#include <BRep_CurveRepresentation.hxx>
class TopLoc_Location;
class gp_Pnt;

//! Root class for the geometric curves
//! representation. Contains a range.
//! Contains a first and a last parameter.
class BRep_GCurve : public BRep_CurveRepresentation
{

public:
  void SetRange(const double First, const double Last);

  void Range(double& First, double& Last) const;

  double First() const;

  double Last() const;

  void First(const double F);

  void Last(const double L);

  //! Computes the point at parameter U.
  Standard_EXPORT virtual void D0(const double U, gp_Pnt& P) const = 0;

  //! Recomputes any derived data after a modification.
  //! This is called when the range is modified.
  Standard_EXPORT virtual void Update();

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(BRep_GCurve, BRep_CurveRepresentation)

protected:
  Standard_EXPORT BRep_GCurve(const TopLoc_Location&  L,
                              const double            First,
                              const double            Last,
                              const BRep_CurveRepKind theKind);

private:
  double myFirst;
  double myLast;
};

#include <BRep_GCurve.lxx>

#endif // _BRep_GCurve_HeaderFile
