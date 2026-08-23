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

#ifndef BRepFont_Regularizer_HeaderFile
#define BRepFont_Regularizer_HeaderFile

#include <BRepFont_PlanarRegion.hxx>
#include <NCollection_FlatMap.hxx>

#include <cstdint>
#include <optional>

//! Converts a font outline into regularized planar topology.
class BRepFont_Regularizer
{
public:
  enum class Status : uint8_t
  {
    Success,
    Empty,
    InvalidOutline,
    OpenContour,
    IntersectingContours,
    //! A marked outline contains a self-intersecting contour.
    SelfIntersectingContour
  };

  struct Options
  {
    //! Tolerance in the same design-unit coordinate system as the input outline.
    double Tolerance = 1.0e-7;
  };

  class Result
  {
  public:
    [[nodiscard]] Status StatusValue() const noexcept { return myStatus; }

    [[nodiscard]] bool IsDone() const noexcept
    {
      return myStatus == Status::Success || myStatus == Status::Empty;
    }

    [[nodiscard]] const std::optional<BRepFont_PlanarRegion>& PlanarRegion() const noexcept
    {
      return myRegion;
    }

    [[nodiscard]] std::optional<BRepFont_PlanarRegion>& ChangePlanarRegion() noexcept
    {
      return myRegion;
    }

  private:
    friend class BRepFont_Regularizer;

    Status                               myStatus = Status::InvalidOutline;
    std::optional<BRepFont_PlanarRegion> myRegion;
  };

  //! Build a planar region. Unmarked intersections return a failure status without a region.
  //! Marked intersections between loops are retained as separate output regions.
  //! SelfIntersectingContour identifies an intersection within one loop.
  [[nodiscard]] Standard_EXPORT Result Build(const Font_GlyphOutline& theOutline) const;
  [[nodiscard]] Standard_EXPORT Result Build(const Font_GlyphOutline& theOutline,
                                             const Options&           theOptions) const;

private:
  static bool appendOutline(const Font_GlyphOutline& theOutline,
                            double                   theTolerance,
                            BRepFont_PlanarRegion&   theRegion,
                            Status&                  theStatus);
  static bool classifyLoops(const Font_GlyphOutline&             theOutline,
                            BRepFont_PlanarRegion&               theRegion,
                            double                               theTolerance,
                            const NCollection_FlatMap<uint64_t>& theIntersectingLoopPairs);
};

#endif // BRepFont_Regularizer_HeaderFile
