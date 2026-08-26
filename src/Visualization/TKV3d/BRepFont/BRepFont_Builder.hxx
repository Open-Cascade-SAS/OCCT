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

#ifndef BRepFont_Builder_HeaderFile
#define BRepFont_Builder_HeaderFile

#include <BRepFont_PlanarRegion.hxx>
#include <BRepGraph_NodeId.hxx>
#include <NCollection_LinearVector.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax3.hxx>
#include <gp_XY.hxx>

#include <cstddef>
#include <memory>
#include <optional>

class BRepGraph;

//! Builds glyph and text topology in TopoDS or caller-owned BRepGraph storage.
class BRepFont_Builder
{
public:
  struct GlyphOptions
  {
    //! Requested glyph height.
    double Size = 1.0;
    //! Tolerance assigned to generated topology.
    double Tolerance = 1.0e-7;
    //! Concatenate each contour into one C0 B-spline edge for TopoDS output.
    bool ToConcatenateContours = false;
  };

  //! Text layout with unique planar glyph regions.
  class TextPlan
  {
  public:
    //! One positioned use of a planar glyph region.
    class Item
    {
    public:
      Item(const size_t theRegion, const gp_XY& thePosition)
          : myRegion(theRegion),
            myPosition(thePosition)
      {
      }

      [[nodiscard]] size_t Region() const noexcept { return myRegion; }

      [[nodiscard]] const gp_XY& Position() const noexcept { return myPosition; }

    private:
      size_t myRegion;
      gp_XY  myPosition;
    };

    [[nodiscard]] double Size() const noexcept { return mySize; }

    [[nodiscard]] double Tolerance() const noexcept { return myTolerance; }

    [[nodiscard]] size_t NbRegions() const noexcept { return myRegions.Size(); }

    [[nodiscard]] const BRepFont_PlanarRegion& Region(const size_t theIndex) const
    {
      return *myRegions.Value(theIndex);
    }

    //! Return the shared planar region stored at the specified index.
    [[nodiscard]] const std::shared_ptr<const BRepFont_PlanarRegion>& RegionHandle(
      const size_t theIndex) const
    {
      return myRegions.Value(theIndex);
    }

    //! Return the lower-left corner of the formatted text bounds in model units.
    [[nodiscard]] const gp_XY& LowerLeft() const noexcept { return myLowerLeft; }

    //! Return the upper-right corner of the formatted text bounds in model units.
    [[nodiscard]] const gp_XY& UpperRight() const noexcept { return myUpperRight; }

    //! Return the formatted text width in model units.
    [[nodiscard]] double Width() const noexcept { return myUpperRight.X() - myLowerLeft.X(); }

    //! Return the formatted text height in model units.
    [[nodiscard]] double Height() const noexcept { return myUpperRight.Y() - myLowerLeft.Y(); }

    [[nodiscard]] const NCollection_LinearVector<Item>& Items() const noexcept { return myItems; }

  private:
    friend class BRepFont_Builder;

    TextPlan(const double                                                             theSize,
             const double                                                             theTolerance,
             const gp_XY&                                                             theLowerLeft,
             const gp_XY&                                                             theUpperRight,
             NCollection_LinearVector<std::shared_ptr<const BRepFont_PlanarRegion>>&& theRegions,
             NCollection_LinearVector<Item>&&                                         theItems);

    double                                                                 mySize;
    double                                                                 myTolerance;
    gp_XY                                                                  myLowerLeft;
    gp_XY                                                                  myUpperRight;
    NCollection_LinearVector<std::shared_ptr<const BRepFont_PlanarRegion>> myRegions;
    NCollection_LinearVector<Item>                                         myItems;
  };

  struct TextOptions
  {
    //! Position and orientation of the text baseline.
    gp_Ax3 Pen;
    //! Concatenate each contour into one C0 B-spline edge for TopoDS output.
    bool ToConcatenateContours = false;
  };

  //! Construct a validated text plan from the specified arrays.
  //! @param[in] theSize output glyph size in model units
  //! @param[in] theTolerance output construction tolerance
  //! @param[in] theLowerLeft lower-left layout bound in model units
  //! @param[in] theUpperRight upper-right layout bound in model units
  //! @param[in] theRegions unique planar glyph regions
  //! @param[in] theItems positioned references into theRegions
  //! @return plan, or empty when an option or region reference is invalid
  [[nodiscard]] Standard_EXPORT static std::optional<TextPlan> CreateTextPlan(
    const double                                                             theSize,
    const double                                                             theTolerance,
    const gp_XY&                                                             theLowerLeft,
    const gp_XY&                                                             theUpperRight,
    NCollection_LinearVector<std::shared_ptr<const BRepFont_PlanarRegion>>&& theRegions,
    NCollection_LinearVector<TextPlan::Item>&&                               theItems);

  //! Build one TopoDS glyph shape.
  //! @param[in] theRegion regularized glyph region
  //! @param[in] theOptions glyph construction options
  //! @return resulting face, wire, or compound; null on failure or for an empty region
  [[nodiscard]] Standard_EXPORT static TopoDS_Shape BuildGlyph(
    const BRepFont_PlanarRegion& theRegion,
    const GlyphOptions&          theOptions);

  //! Add one glyph shape to an existing graph.
  //! @param[in, out] theGraph graph receiving new topology
  //! @param[in] theRegion regularized glyph region
  //! @param[in] theOptions glyph construction options
  //! @return root node of the added shape, or an invalid node on failure or for an empty region
  [[nodiscard]] Standard_EXPORT static BRepGraph_NodeId AddGlyph(
    BRepGraph&                   theGraph,
    const BRepFont_PlanarRegion& theRegion,
    const GlyphOptions&          theOptions);

  //! Build positioned text occurrences in a TopoDS compound.
  //! @param[in] thePlan text plan
  //! @return text compound, or a null shape on failure or for empty text
  [[nodiscard]] Standard_EXPORT static TopoDS_Shape BuildText(const TextPlan& thePlan);

  //! Build positioned text occurrences in a TopoDS compound.
  //! @param[in] thePlan text plan
  //! @param[in] theOptions text placement and topology options
  //! @return text compound, or a null shape on failure or for empty text
  [[nodiscard]] Standard_EXPORT static TopoDS_Shape BuildText(const TextPlan&    thePlan,
                                                              const TextOptions& theOptions);

  //! Assemble positioned occurrences from prebuilt glyph shapes.
  //! @param[in] thePlan text plan defining occurrence positions
  //! @param[in] theGlyphShapes one shape per unique plan region
  //! @param[in] theOptions text placement options
  //! @return text compound, or a null shape when input is invalid or text is empty
  [[nodiscard]] Standard_EXPORT static TopoDS_Shape BuildText(
    const TextPlan&                               thePlan,
    const NCollection_LinearVector<TopoDS_Shape>& theGlyphShapes,
    const TextOptions&                            theOptions);

  //! Add positioned text occurrences to an existing graph.
  //! @param[in, out] theGraph graph receiving glyph definitions and text occurrences
  //! @param[in] thePlan text plan
  //! @return root compound of the added text, or an invalid node on failure or for empty text
  [[nodiscard]] Standard_EXPORT static BRepGraph_NodeId AddText(BRepGraph&      theGraph,
                                                                const TextPlan& thePlan);

  //! Add positioned text occurrences to an existing graph.
  //! @param[in, out] theGraph graph receiving glyph definitions and text occurrences
  //! @param[in] thePlan text plan
  //! @param[in] theOptions text placement options
  //! @return root compound of the added text, or an invalid node on failure or for empty text
  [[nodiscard]] Standard_EXPORT static BRepGraph_NodeId AddText(BRepGraph&         theGraph,
                                                                const TextPlan&    thePlan,
                                                                const TextOptions& theOptions);

private:
  BRepFont_Builder() = delete;
};

#endif // BRepFont_Builder_HeaderFile
