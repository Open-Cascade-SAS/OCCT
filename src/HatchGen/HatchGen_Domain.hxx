// Created on: 1993-11-05
// Created by: Jean Marc LACHAUME
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

#ifndef _HatchGen_Domain_HeaderFile
#define _HatchGen_Domain_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <HatchGen_PointOnHatching.hxx>

class HatchGen_Domain
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an infinite domain.
  Standard_EXPORT HatchGen_Domain();

  //! Creates a domain for the curve associated to a hatching.
  Standard_EXPORT HatchGen_Domain(const HatchGen_PointOnHatching& P1,
                                  const HatchGen_PointOnHatching& P2);

  //! Creates a semi-infinite domain for the curve associated
  //! to a hatching. The `First' flag means that the given
  //! point is the first one.
  Standard_EXPORT HatchGen_Domain(const HatchGen_PointOnHatching& P, const Standard_Boolean First);

  //! Sets the first and the second points of the domain.
  void SetPoints(const HatchGen_PointOnHatching& P1, const HatchGen_PointOnHatching& P2);

  //! Sets the first and the second points of the domain
  //! as the infinite.
  void SetPoints();

  //! Sets the first point of the domain.
  void SetFirstPoint(const HatchGen_PointOnHatching& P);

  //! Sets the first point of the domain at the
  //! infinite.
  void SetFirstPoint();

  //! Sets the second point of the domain.
  void SetSecondPoint(const HatchGen_PointOnHatching& P);

  //! Sets the second point of the domain at the
  //! infinite.
  void SetSecondPoint();

  //! Returns True if the domain has a first point.
  Standard_Boolean HasFirstPoint() const;

  //! Returns the first point of the domain.
  //! The exception DomainError is raised if
  //! HasFirstPoint returns False.
  const HatchGen_PointOnHatching& FirstPoint() const;

  //! Returns True if the domain has a second point.
  Standard_Boolean HasSecondPoint() const;

  //! Returns the second point of the domain.
  //! The exception DomainError is raised if
  //! HasSecondPoint returns False.
  const HatchGen_PointOnHatching& SecondPoint() const;

  //! Dump of the domain.
  Standard_EXPORT void Dump(const Standard_Integer Index = 0) const;

protected:
private:
  Standard_Boolean         myHasFirstPoint;
  HatchGen_PointOnHatching myFirstPoint;
  Standard_Boolean         myHasSecondPoint;
  HatchGen_PointOnHatching mySecondPoint;
};

#include <HatchGen_Domain.lxx>

#endif // _HatchGen_Domain_HeaderFile
