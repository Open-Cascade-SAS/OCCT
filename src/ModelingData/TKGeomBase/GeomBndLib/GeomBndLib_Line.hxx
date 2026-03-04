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

#ifndef GeomBndLib_Line_HeaderFile
#define GeomBndLib_Line_HeaderFile

#include <Bnd_Box.hxx>
#include <GeomBndLib_InfiniteHelpers.pxx>
#include <ElCLib.hxx>
#include <Geom_Line.hxx>
#include <gp_Lin.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Failure.hxx>

//! Computes bounding box for a 3D line (Geom_Line).
//! Handles infinite parameters by opening the box in appropriate directions.
//!
//! Static method accepting gp_Lin can be used directly without
//! constructing a Geom_Line handle.
class GeomBndLib_Line
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Line(const occ::handle<Geom_Line>& theLine)
      : myGeom(theLine)
  {
  }

  GeomBndLib_Line(const GeomBndLib_Line&)            = delete;
  GeomBndLib_Line& operator=(const GeomBndLib_Line&) = delete;
  GeomBndLib_Line(GeomBndLib_Line&&)                 = delete;
  GeomBndLib_Line& operator=(GeomBndLib_Line&&)      = delete;

  const occ::handle<Geom_Line>& Geometry() const { return myGeom; }

  //! Add bounding box for line segment [theU1, theU2].
  void Add(double theU1, double theU2, double theTol, Bnd_Box& theBox) const
  {
    Add(myGeom->Lin(), theU1, theU2, theTol, theBox);
  }

  //! Add bounding box for full curve.
  void Add(double theTol, Bnd_Box& theBox) const
  {
    Add(myGeom->FirstParameter(), myGeom->LastParameter(), theTol, theBox);
  }

  //! For analytical curves, AddOptimal is same as Add.
  void AddOptimal(double theU1, double theU2, double theTol, Bnd_Box& theBox) const
  {
    Add(theU1, theU2, theTol, theBox);
  }

  //! Add bounding box for a line segment [theU1, theU2] defined by gp_Lin.
  static void Add(const gp_Lin& theLin,
                  double        theU1,
                  double        theU2,
                  double        theTol,
                  Bnd_Box&      theBox)
  {
    if (Precision::IsNegativeInfinite(theU1))
    {
      if (Precision::IsNegativeInfinite(theU2))
      {
        throw Standard_Failure("GeomBndLib_Line::Add - bad parameter");
      }
      else if (Precision::IsPositiveInfinite(theU2))
      {
        GeomBndLib_InfiniteHelpers::OpenMinMax(theLin.Direction(), theBox);
        theBox.Add(ElCLib::Value(0., theLin));
      }
      else
      {
        GeomBndLib_InfiniteHelpers::OpenMin(theLin.Direction(), theBox);
        theBox.Add(ElCLib::Value(theU2, theLin));
      }
    }
    else if (Precision::IsPositiveInfinite(theU1))
    {
      if (Precision::IsNegativeInfinite(theU2))
      {
        GeomBndLib_InfiniteHelpers::OpenMinMax(theLin.Direction(), theBox);
        theBox.Add(ElCLib::Value(0., theLin));
      }
      else if (Precision::IsPositiveInfinite(theU2))
      {
        throw Standard_Failure("GeomBndLib_Line::Add - bad parameter");
      }
      else
      {
        GeomBndLib_InfiniteHelpers::OpenMax(theLin.Direction(), theBox);
        theBox.Add(ElCLib::Value(theU2, theLin));
      }
    }
    else
    {
      theBox.Add(ElCLib::Value(theU1, theLin));
      if (Precision::IsNegativeInfinite(theU2))
      {
        GeomBndLib_InfiniteHelpers::OpenMin(theLin.Direction(), theBox);
      }
      else if (Precision::IsPositiveInfinite(theU2))
      {
        GeomBndLib_InfiniteHelpers::OpenMax(theLin.Direction(), theBox);
      }
      else
      {
        theBox.Add(ElCLib::Value(theU2, theLin));
      }
    }
    theBox.Enlarge(theTol);
  }

private:
  occ::handle<Geom_Line> myGeom;
};

#endif // GeomBndLib_Line_HeaderFile
