// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef GeomBndLib_Line2d_HeaderFile
#define GeomBndLib_Line2d_HeaderFile

#include <Bnd_Box2d.hxx>
#include <GeomBndLib_InfiniteHelpers.pxx>
#include <ElCLib.hxx>
#include <Geom2d_Line.hxx>
#include <gp_Lin2d.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Failure.hxx>

//! Computes bounding box for a 2D line (Geom2d_Line).
//! Handles infinite parameters by opening the box in appropriate directions.
//!
//! Static method accepting gp_Lin2d can be used directly without
//! constructing a Geom2d_Line handle.
class GeomBndLib_Line2d
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Line2d(const occ::handle<Geom2d_Line>& theLine)
      : myGeom(theLine)
  {
  }

  GeomBndLib_Line2d(const GeomBndLib_Line2d&)            = delete;
  GeomBndLib_Line2d& operator=(const GeomBndLib_Line2d&) = delete;
  GeomBndLib_Line2d(GeomBndLib_Line2d&&)                 = delete;
  GeomBndLib_Line2d& operator=(GeomBndLib_Line2d&&)      = delete;

  const occ::handle<Geom2d_Line>& Geometry() const { return myGeom; }

  //! Compute bounding box for line segment [theU1, theU2].
  [[nodiscard]] Bnd_Box2d Box(double theU1, double theU2, double theTol) const
  {
    return Box(myGeom->Lin2d(), theU1, theU2, theTol);
  }

  //! Compute bounding box for full curve.
  [[nodiscard]] Bnd_Box2d Box(double theTol) const
  {
    return Box(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
  }

  //! For analytical curves, BoxOptimal is same as Box.
  [[nodiscard]] Bnd_Box2d BoxOptimal(double theU1, double theU2, double theTol) const
  {
    return Box(theU1, theU2, theTol);
  }

  //! Compute bounding box for a 2D line segment [theU1, theU2] defined by gp_Lin2d.
  [[nodiscard]] static Bnd_Box2d Box(const gp_Lin2d& theLin,
                                     double          theU1,
                                     double          theU2,
                                     double          theTol)
  {
    Bnd_Box2d aBox;
    if (Precision::IsNegativeInfinite(theU1))
    {
      if (Precision::IsNegativeInfinite(theU2))
      {
        throw Standard_Failure("GeomBndLib_Line2d::Box - bad parameter");
      }
      else if (Precision::IsPositiveInfinite(theU2))
      {
        GeomBndLib_InfiniteHelpers::OpenMinMax(theLin.Direction(), aBox);
        aBox.Add(ElCLib::Value(0., theLin));
      }
      else
      {
        GeomBndLib_InfiniteHelpers::OpenMin(theLin.Direction(), aBox);
        aBox.Add(ElCLib::Value(theU2, theLin));
      }
    }
    else if (Precision::IsPositiveInfinite(theU1))
    {
      if (Precision::IsNegativeInfinite(theU2))
      {
        GeomBndLib_InfiniteHelpers::OpenMinMax(theLin.Direction(), aBox);
        aBox.Add(ElCLib::Value(0., theLin));
      }
      else if (Precision::IsPositiveInfinite(theU2))
      {
        throw Standard_Failure("GeomBndLib_Line2d::Box - bad parameter");
      }
      else
      {
        GeomBndLib_InfiniteHelpers::OpenMax(theLin.Direction(), aBox);
        aBox.Add(ElCLib::Value(theU2, theLin));
      }
    }
    else
    {
      aBox.Add(ElCLib::Value(theU1, theLin));
      if (Precision::IsNegativeInfinite(theU2))
      {
        GeomBndLib_InfiniteHelpers::OpenMin(theLin.Direction(), aBox);
      }
      else if (Precision::IsPositiveInfinite(theU2))
      {
        GeomBndLib_InfiniteHelpers::OpenMax(theLin.Direction(), aBox);
      }
      else
      {
        aBox.Add(ElCLib::Value(theU2, theLin));
      }
    }
    aBox.Enlarge(theTol);
    return aBox;
  }

private:
  occ::handle<Geom2d_Line> myGeom;
};

#endif // GeomBndLib_Line2d_HeaderFile
