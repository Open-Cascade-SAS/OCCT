// Created on: 1993-08-10
// Created by: Remi LEQUETTE
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

#ifndef _BRep_PointRepresentation_HeaderFile
#define _BRep_PointRepresentation_HeaderFile

#include <Standard.hxx>

#include <TopLoc_Location.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
class Geom_Curve;
class Geom2d_Curve;
class Geom_Surface;

//! Discriminant tag identifying the concrete type of a point representation.
enum class BRep_PointRepKind : uint8_t
{
  PointOnCurve,
  PointOnSurface,
  PointOnCurveOnSurface
};

//! Root class for the points representations.
//! Contains a location and a parameter.
class BRep_PointRepresentation : public Standard_Transient
{

public:
  //! Returns the concrete representation kind tag.
  BRep_PointRepKind PointRepresentationKind() const { return myPointKind; }

  //! A point on a 3d curve.
  bool IsPointOnCurve() const { return myPointKind == BRep_PointRepKind::PointOnCurve; }

  //! A point on a 2d curve on a surface.
  bool IsPointOnCurveOnSurface() const
  {
    return myPointKind == BRep_PointRepKind::PointOnCurveOnSurface;
  }

  //! A point on a surface.
  bool IsPointOnSurface() const { return myPointKind == BRep_PointRepKind::PointOnSurface; }

  //! A point on the curve <C>.
  Standard_EXPORT virtual bool IsPointOnCurve(const occ::handle<Geom_Curve>& C,
                                              const TopLoc_Location&         L) const;

  //! A point on the 2d curve <PC> on the surface <S>.
  Standard_EXPORT virtual bool IsPointOnCurveOnSurface(const occ::handle<Geom2d_Curve>& PC,
                                                       const occ::handle<Geom_Surface>& S,
                                                       const TopLoc_Location&           L) const;

  //! A point on the surface <S>.
  Standard_EXPORT virtual bool IsPointOnSurface(const occ::handle<Geom_Surface>& S,
                                                const TopLoc_Location&           L) const;

  const TopLoc_Location& Location() const;

  void Location(const TopLoc_Location& L);

  double Parameter() const;

  void Parameter(const double P);

  Standard_EXPORT virtual double Parameter2() const;

  Standard_EXPORT virtual void Parameter2(const double P);

  Standard_EXPORT virtual const occ::handle<Geom_Curve>& Curve() const;

  Standard_EXPORT virtual void Curve(const occ::handle<Geom_Curve>& C);

  Standard_EXPORT virtual const occ::handle<Geom2d_Curve>& PCurve() const;

  Standard_EXPORT virtual void PCurve(const occ::handle<Geom2d_Curve>& C);

  Standard_EXPORT virtual const occ::handle<Geom_Surface>& Surface() const;

  Standard_EXPORT virtual void Surface(const occ::handle<Geom_Surface>& S);

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  DEFINE_STANDARD_RTTIEXT(BRep_PointRepresentation, Standard_Transient)

protected:
  BRep_PointRepresentation(const double P, const TopLoc_Location& L, const BRep_PointRepKind theKind)
      : myPointKind(theKind),
        myLocation(L),
        myParameter(P)
  {
  }

  BRep_PointRepKind myPointKind;

private:
  TopLoc_Location myLocation;
  double          myParameter;
};

#include <BRep_PointRepresentation.lxx>

#endif // _BRep_PointRepresentation_HeaderFile
