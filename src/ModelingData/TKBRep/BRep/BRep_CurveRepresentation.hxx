// Created on: 1993-07-05
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

#ifndef _BRep_CurveRepresentation_HeaderFile
#define _BRep_CurveRepresentation_HeaderFile

#include <Standard.hxx>

#include <TopLoc_Location.hxx>
#include <Standard_Transient.hxx>
#include <GeomAbs_Shape.hxx>
class Geom_Surface;
class Poly_Triangulation;
class Geom_Curve;
class Geom2d_Curve;
class Poly_Polygon3D;
class Poly_Polygon2D;
class Poly_PolygonOnTriangulation;

//! Discriminant tag identifying the concrete type of a curve representation.
enum class BRep_CurveRepKind : uint8_t
{
  Curve3D,
  CurveOnSurface,
  CurveOnClosedSurface,
  CurveOn2Surfaces,
  Polygon3D,
  PolygonOnSurface,
  PolygonOnClosedSurface,
  PolygonOnTriangulation,
  PolygonOnClosedTriangulation
};

//! Root class for the curve representations. Contains
//! a location.
class BRep_CurveRepresentation : public Standard_Transient
{

public:
  //! Returns the concrete representation kind tag.
  BRep_CurveRepKind RepresentationKind() const { return myKind; }

  //! A 3D curve representation.
  bool IsCurve3D() const { return myKind == BRep_CurveRepKind::Curve3D; }

  //! A curve in the parametric space of a surface.
  bool IsCurveOnSurface() const
  {
    return myKind == BRep_CurveRepKind::CurveOnSurface
           || myKind == BRep_CurveRepKind::CurveOnClosedSurface;
  }

  //! A continuity between two surfaces.
  bool IsRegularity() const
  {
    return myKind == BRep_CurveRepKind::CurveOnClosedSurface
           || myKind == BRep_CurveRepKind::CurveOn2Surfaces;
  }

  //! A curve with two parametric curves on the same surface.
  bool IsCurveOnClosedSurface() const { return myKind == BRep_CurveRepKind::CurveOnClosedSurface; }

  //! Is it a curve in the parametric space of <S> with location <L>.
  Standard_EXPORT virtual bool IsCurveOnSurface(const occ::handle<Geom_Surface>& S,
                                                const TopLoc_Location&           L) const;

  //! Is it a regularity between <S1> and <S2> with location <L1> and <L2>.
  Standard_EXPORT virtual bool IsRegularity(const occ::handle<Geom_Surface>& S1,
                                            const occ::handle<Geom_Surface>& S2,
                                            const TopLoc_Location&           L1,
                                            const TopLoc_Location&           L2) const;

  //! A 3D polygon representation.
  bool IsPolygon3D() const { return myKind == BRep_CurveRepKind::Polygon3D; }

  //! A representation by an array of nodes on a triangulation.
  bool IsPolygonOnTriangulation() const
  {
    return myKind == BRep_CurveRepKind::PolygonOnTriangulation
           || myKind == BRep_CurveRepKind::PolygonOnClosedTriangulation;
  }

  //! Is it a polygon in the definition of <T> with location <L>.
  Standard_EXPORT virtual bool IsPolygonOnTriangulation(const occ::handle<Poly_Triangulation>& T,
                                                        const TopLoc_Location& L) const;

  //! A representation by two arrays of nodes on a triangulation.
  bool IsPolygonOnClosedTriangulation() const
  {
    return myKind == BRep_CurveRepKind::PolygonOnClosedTriangulation;
  }

  //! A polygon in the parametric space of a surface.
  bool IsPolygonOnSurface() const
  {
    return myKind == BRep_CurveRepKind::PolygonOnSurface
           || myKind == BRep_CurveRepKind::PolygonOnClosedSurface;
  }

  //! Is it a polygon in the parametric space of <S> with location <L>.
  Standard_EXPORT virtual bool IsPolygonOnSurface(const occ::handle<Geom_Surface>& S,
                                                  const TopLoc_Location&           L) const;

  //! Two 2D polygon representations in the parametric space of a surface.
  bool IsPolygonOnClosedSurface() const
  {
    return myKind == BRep_CurveRepKind::PolygonOnClosedSurface;
  }

  const TopLoc_Location& Location() const;

  void Location(const TopLoc_Location& L);

  Standard_EXPORT virtual const occ::handle<Geom_Curve>& Curve3D() const;

  Standard_EXPORT virtual void Curve3D(const occ::handle<Geom_Curve>& C);

  Standard_EXPORT virtual const occ::handle<Geom_Surface>& Surface() const;

  Standard_EXPORT virtual const occ::handle<Geom2d_Curve>& PCurve() const;

  Standard_EXPORT virtual void PCurve(const occ::handle<Geom2d_Curve>& C);

  Standard_EXPORT virtual const occ::handle<Geom2d_Curve>& PCurve2() const;

  Standard_EXPORT virtual void PCurve2(const occ::handle<Geom2d_Curve>& C);

  Standard_EXPORT virtual const occ::handle<Poly_Polygon3D>& Polygon3D() const;

  Standard_EXPORT virtual void Polygon3D(const occ::handle<Poly_Polygon3D>& P);

  Standard_EXPORT virtual const occ::handle<Poly_Polygon2D>& Polygon() const;

  Standard_EXPORT virtual void Polygon(const occ::handle<Poly_Polygon2D>& P);

  Standard_EXPORT virtual const occ::handle<Poly_Polygon2D>& Polygon2() const;

  Standard_EXPORT virtual void Polygon2(const occ::handle<Poly_Polygon2D>& P);

  Standard_EXPORT virtual const occ::handle<Poly_Triangulation>& Triangulation() const;

  Standard_EXPORT virtual const occ::handle<Poly_PolygonOnTriangulation>& PolygonOnTriangulation()
    const;

  Standard_EXPORT virtual void PolygonOnTriangulation(
    const occ::handle<Poly_PolygonOnTriangulation>& P);

  Standard_EXPORT virtual const occ::handle<Poly_PolygonOnTriangulation>& PolygonOnTriangulation2()
    const;

  Standard_EXPORT virtual void PolygonOnTriangulation2(
    const occ::handle<Poly_PolygonOnTriangulation>& P2);

  Standard_EXPORT virtual const occ::handle<Geom_Surface>& Surface2() const;

  Standard_EXPORT virtual const TopLoc_Location& Location2() const;

  Standard_EXPORT virtual const GeomAbs_Shape& Continuity() const;

  Standard_EXPORT virtual void Continuity(const GeomAbs_Shape C);

  //! Return a copy of this representation.
  Standard_EXPORT virtual occ::handle<BRep_CurveRepresentation> Copy() const = 0;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  DEFINE_STANDARD_RTTIEXT(BRep_CurveRepresentation, Standard_Transient)

protected:
  BRep_CurveRepresentation(const TopLoc_Location& L, const BRep_CurveRepKind theKind)
      : myLocation(L),
        myKind(theKind)
  {
  }

  TopLoc_Location      myLocation;
  BRep_CurveRepKind    myKind;
};

#include <BRep_CurveRepresentation.lxx>

#endif // _BRep_CurveRepresentation_HeaderFile
