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

#ifndef Font_GlyphOutline_HeaderFile
#define Font_GlyphOutline_HeaderFile

#include <NCollection_LinearVector.hxx>
#include <Standard_HashUtils.hxx>
#include <gp_XY.hxx>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

//! Glyph outline in font design units.
class Font_GlyphOutline
{
public:
  //! Face-local glyph identity. Face() returns an index used by the owning Font_FTFont.
  class Glyph
  {
  public:
    //! Per-font identity retained by resolved glyph values.
    class Identity
    {
    public:
      Identity() = default;
    };

    [[nodiscard]] const std::shared_ptr<const Identity>& Owner() const noexcept { return myOwner; }

    [[nodiscard]] uint64_t Revision() const noexcept { return myRevision; }

    [[nodiscard]] uint32_t Face() const noexcept { return myFace; }

    [[nodiscard]] uint32_t Index() const noexcept { return myIndex; }

    class Hasher
    {
    public:
      [[nodiscard]] size_t operator()(const Glyph& theGlyph) const noexcept
      {
        const std::array<size_t, 4> aHashes = {opencascade::hash(theGlyph.Owner().get()),
                                               opencascade::hash(theGlyph.Revision()),
                                               opencascade::hash(theGlyph.Face()),
                                               opencascade::hash(theGlyph.Index())};
        return opencascade::hashBytes(aHashes.data(), sizeof(aHashes));
      }

      [[nodiscard]] bool operator()(const Glyph& theLeft, const Glyph& theRight) const noexcept
      {
        return theLeft.Owner() == theRight.Owner() && theLeft.Revision() == theRight.Revision()
               && theLeft.Face() == theRight.Face() && theLeft.Index() == theRight.Index();
      }
    };

    friend bool operator==(const Glyph& theLeft, const Glyph& theRight) noexcept
    {
      return theLeft.myOwner == theRight.myOwner && theLeft.myRevision == theRight.myRevision
             && theLeft.myFace == theRight.myFace && theLeft.myIndex == theRight.myIndex;
    }

    friend bool operator!=(const Glyph& theLeft, const Glyph& theRight) noexcept
    {
      return !(theLeft == theRight);
    }

  private:
    friend class Font_FTFont;

    Glyph(const std::shared_ptr<const Identity>& theOwner,
          const uint64_t                         theRevision,
          const uint32_t                         theFace,
          const uint32_t                         theIndex)
        : myOwner(theOwner),
          myRevision(theRevision),
          myFace(theFace),
          myIndex(theIndex)
    {
    }

    std::shared_ptr<const Identity> myOwner;
    uint64_t                        myRevision = 0;
    uint32_t                        myFace     = 0;
    uint32_t                        myIndex    = 0;
  };

  enum class FillRule : uint8_t
  {
    NonZero,
    EvenOdd
  };

  //! 2D outline segment.
  class Segment
  {
  public:
    enum class Kind : uint8_t
    {
      Line,
      QuadraticBezier,
      CubicBezier
    };

    Segment() = default;

    [[nodiscard]] static Segment Line(const gp_XY& theStart, const gp_XY& theEnd) noexcept
    {
      return Segment(Kind::Line, theStart, theStart, theEnd, theEnd);
    }

    [[nodiscard]] static Segment Quadratic(const gp_XY& theStart,
                                           const gp_XY& theControl,
                                           const gp_XY& theEnd) noexcept
    {
      return Segment(Kind::QuadraticBezier, theStart, theControl, theControl, theEnd);
    }

    [[nodiscard]] static Segment Cubic(const gp_XY& theStart,
                                       const gp_XY& theControl1,
                                       const gp_XY& theControl2,
                                       const gp_XY& theEnd) noexcept
    {
      return Segment(Kind::CubicBezier, theStart, theControl1, theControl2, theEnd);
    }

    [[nodiscard]] Kind Type() const noexcept { return myKind; }

    [[nodiscard]] const gp_XY& Start() const noexcept { return myStart; }

    [[nodiscard]] const gp_XY& Control1() const noexcept { return myControl1; }

    [[nodiscard]] const gp_XY& Control2() const noexcept { return myControl2; }

    [[nodiscard]] const gp_XY& End() const noexcept { return myEnd; }

  private:
    Segment(const Kind   theKind,
            const gp_XY& theStart,
            const gp_XY& theControl1,
            const gp_XY& theControl2,
            const gp_XY& theEnd) noexcept
        : myKind(theKind),
          myStart(theStart),
          myControl1(theControl1),
          myControl2(theControl2),
          myEnd(theEnd)
    {
    }

    Kind  myKind = Kind::Line;
    gp_XY myStart;
    gp_XY myControl1;
    gp_XY myControl2;
    gp_XY myEnd;
  };

  //! Ordered segment range describing one contour in Segments().
  class Contour
  {
  public:
    Contour() = default;

    Contour(const uint32_t theFirstSegment, const uint32_t theNbSegments) noexcept
        : myFirstSegment(theFirstSegment),
          myNbSegments(theNbSegments)
    {
    }

    [[nodiscard]] uint32_t FirstSegment() const noexcept { return myFirstSegment; }

    [[nodiscard]] uint32_t NbSegments() const noexcept { return myNbSegments; }

  private:
    uint32_t myFirstSegment = 0;
    uint32_t myNbSegments   = 0;
  };

public:
  //! Construct outline geometry without a resolved font glyph.
  //! This entry point supports other font providers and generated modeling inputs.
  //! @param[in] theUnitsPerEm design units per em
  //! @param[in] theFillRule contour filling rule
  //! @param[in] theHasPossibleOverlaps source metadata indicating possible contour overlaps
  //! @param[in] theIsSingleStroke interpret contours as independent strokes
  //! @param[in] theSegments packed outline segments
  //! @param[in] theContours packed ranges into theSegments
  //! @return outline; structural validation is performed by the regularizer
  [[nodiscard]] Standard_EXPORT static Font_GlyphOutline Create(
    double                              theUnitsPerEm,
    FillRule                            theFillRule,
    bool                                theHasPossibleOverlaps,
    bool                                theIsSingleStroke,
    NCollection_LinearVector<Segment>&& theSegments,
    NCollection_LinearVector<Contour>&& theContours);

  [[nodiscard]] double UnitsPerEm() const noexcept { return myUnitsPerEm; }

  [[nodiscard]] FillRule Rule() const noexcept { return myFillRule; }

  [[nodiscard]] bool HasPossibleOverlaps() const noexcept { return myHasPossibleOverlaps; }

  [[nodiscard]] bool IsSingleStroke() const noexcept { return myIsSingleStroke; }

  [[nodiscard]] bool IsEmpty() const noexcept { return mySegments.IsEmpty(); }

  [[nodiscard]] const NCollection_LinearVector<Segment>& Segments() const noexcept
  {
    return mySegments;
  }

  [[nodiscard]] const NCollection_LinearVector<Contour>& Contours() const noexcept
  {
    return myContours;
  }

private:
  friend class Font_FTFont;

  double                            myUnitsPerEm          = 0.0;
  FillRule                          myFillRule            = FillRule::NonZero;
  bool                              myHasPossibleOverlaps = false;
  bool                              myIsSingleStroke      = false;
  NCollection_LinearVector<Segment> mySegments;
  NCollection_LinearVector<Contour> myContours;
};

#endif // Font_GlyphOutline_HeaderFile
