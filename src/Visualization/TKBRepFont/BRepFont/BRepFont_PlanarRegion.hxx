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

#ifndef BRepFont_PlanarRegion_HeaderFile
#define BRepFont_PlanarRegion_HeaderFile

#include <Bnd_Box2d.hxx>
#include <Font_GlyphOutline.hxx>
#include <NCollection_LinearVector.hxx>
#include <gp_XY.hxx>

#include <array>
#include <cstdint>

//! Regularized planar glyph topology.
class BRepFont_PlanarRegion
{
public:
  enum class Entity : uint8_t
  {
    Vertex,
    Curve,
    Loop
  };

  //! Strongly typed index into an entity pool.
  template <Entity TheEntity>
  class Id
  {
  public:
    explicit Id(const uint32_t theValue)
        : myValue(theValue)
    {
    }

    [[nodiscard]] uint32_t Value() const noexcept { return myValue; }

    friend bool operator==(const Id& theLeft, const Id& theRight) noexcept
    {
      return theLeft.myValue == theRight.myValue;
    }

    friend bool operator!=(const Id& theLeft, const Id& theRight) noexcept
    {
      return !(theLeft == theRight);
    }

  private:
    uint32_t myValue;
  };

  using VertexId = Id<Entity::Vertex>;
  using CurveId  = Id<Entity::Curve>;
  using LoopId   = Id<Entity::Loop>;

  //! Planar Bezier segment.
  class Curve
  {
  public:
    explicit Curve(const Font_GlyphOutline::Segment& theSegment);

    [[nodiscard]] Font_GlyphOutline::Segment::Kind Type() const noexcept { return myKind; }

    [[nodiscard]] const gp_XY& Start() const noexcept { return myStart; }

    [[nodiscard]] const gp_XY& Control1() const noexcept { return myControl1; }

    [[nodiscard]] const gp_XY& Control2() const noexcept { return myControl2; }

    [[nodiscard]] const gp_XY& End() const noexcept { return myEnd; }

    [[nodiscard]] Standard_EXPORT gp_XY     Value(double theParameter) const;
    [[nodiscard]] Standard_EXPORT gp_XY     D1(double theParameter) const;
    [[nodiscard]] Standard_EXPORT double    SignedArea() const;
    [[nodiscard]] Standard_EXPORT Bnd_Box2d Bounds() const;

    //! Return a copy with replaced endpoints.
    [[nodiscard]] Curve WithEndpoints(const gp_XY& theStart, const gp_XY& theEnd) const noexcept
    {
      Curve aResult(*this);
      aResult.myStart = theStart;
      aResult.myEnd   = theEnd;
      return aResult;
    }

  private:
    struct PowerCoefficients
    {
      std::array<double, 4> X{};
      std::array<double, 4> Y{};
      int                   Degree = 0;
    };

    [[nodiscard]] PowerCoefficients powerCoefficients() const;

    Font_GlyphOutline::Segment::Kind myKind = Font_GlyphOutline::Segment::Kind::Line;
    gp_XY                            myStart;
    gp_XY                            myControl1;
    gp_XY                            myControl2;
    gp_XY                            myEnd;
  };

  //! Oriented use of a curve by one ordered loop.
  class Use
  {
  public:
    //! Construct an oriented use of a normalized curve interval.
    //! Parameters remain increasing in the curve's [0, 1] domain; orientation is represented
    //! independently so adapters can reuse the same curve geometry.
    //! @param[in] theCurve referenced curve
    //! @param[in] theFirstVertex first vertex in use orientation
    //! @param[in] theLastVertex last vertex in use orientation
    //! @param[in] theFirstParameter first normalized curve parameter
    //! @param[in] theLastParameter last normalized curve parameter
    //! @param[in] theIsReversed true when use orientation opposes increasing curve parameters
    Use(const CurveId  theCurve,
        const VertexId theFirstVertex,
        const VertexId theLastVertex,
        const double   theFirstParameter = 0.0,
        const double   theLastParameter  = 1.0,
        const bool     theIsReversed     = false)
        : myCurve(theCurve),
          myFirstVertex(theFirstVertex),
          myLastVertex(theLastVertex),
          myFirstParameter(theFirstParameter),
          myLastParameter(theLastParameter),
          myIsReversed(theIsReversed)
    {
    }

    [[nodiscard]] const CurveId& CurveIndex() const noexcept { return myCurve; }

    [[nodiscard]] const VertexId& FirstVertex() const noexcept { return myFirstVertex; }

    [[nodiscard]] const VertexId& LastVertex() const noexcept { return myLastVertex; }

    [[nodiscard]] double FirstParameter() const noexcept { return myFirstParameter; }

    [[nodiscard]] double LastParameter() const noexcept { return myLastParameter; }

    [[nodiscard]] bool IsReversed() const noexcept { return myIsReversed; }

  private:
    CurveId  myCurve;
    VertexId myFirstVertex;
    VertexId myLastVertex;
    double   myFirstParameter = 0.0;
    double   myLastParameter  = 1.0;
    bool     myIsReversed     = false;
  };

  //! Ordered closed-loop range in Uses().
  class Loop
  {
  public:
    Loop(const uint32_t   theFirstUse,
         const uint32_t   theNbUses,
         const double     theSignedArea,
         const Bnd_Box2d& theBounds)
        : myFirstUse(theFirstUse),
          myNbUses(theNbUses),
          mySignedArea(theSignedArea),
          myBounds(theBounds)
    {
    }

    [[nodiscard]] uint32_t FirstUse() const noexcept { return myFirstUse; }

    [[nodiscard]] uint32_t NbUses() const noexcept { return myNbUses; }

    [[nodiscard]] double SignedArea() const noexcept { return mySignedArea; }

    [[nodiscard]] const Bnd_Box2d& Bounds() const noexcept { return myBounds; }

  private:
    uint32_t  myFirstUse   = 0;
    uint32_t  myNbUses     = 0;
    double    mySignedArea = 0.0;
    Bnd_Box2d myBounds;
  };

  //! One material region: an outer loop and a packed range of hole-loop IDs.
  class Region
  {
  public:
    Region(const LoopId theOuterLoop, const uint32_t theFirstHole, const uint32_t theNbHoles)
        : myOuterLoop(theOuterLoop),
          myFirstHole(theFirstHole),
          myNbHoles(theNbHoles)
    {
    }

    [[nodiscard]] const LoopId& OuterLoop() const noexcept { return myOuterLoop; }

    [[nodiscard]] uint32_t FirstHole() const noexcept { return myFirstHole; }

    [[nodiscard]] uint32_t NbHoles() const noexcept { return myNbHoles; }

  private:
    LoopId   myOuterLoop;
    uint32_t myFirstHole = 0;
    uint32_t myNbHoles   = 0;
  };

public:
  [[nodiscard]] bool IsEmpty() const noexcept { return myLoops.IsEmpty(); }

  [[nodiscard]] bool IsSingleStroke() const noexcept { return myIsSingleStroke; }

  [[nodiscard]] double UnitsPerEm() const noexcept { return myUnitsPerEm; }

  [[nodiscard]] double Tolerance() const noexcept { return myTolerance; }

  [[nodiscard]] const NCollection_LinearVector<gp_XY>& Vertices() const noexcept
  {
    return myVertices;
  }

  [[nodiscard]] const NCollection_LinearVector<Curve>& Curves() const noexcept { return myCurves; }

  [[nodiscard]] const NCollection_LinearVector<Use>& Uses() const noexcept { return myUses; }

  [[nodiscard]] const NCollection_LinearVector<Loop>& Loops() const noexcept { return myLoops; }

  [[nodiscard]] const NCollection_LinearVector<LoopId>& HoleLoops() const noexcept
  {
    return myHoleLoops;
  }

  [[nodiscard]] const NCollection_LinearVector<Region>& Regions() const noexcept
  {
    return myRegions;
  }

private:
  friend class BRepFont_Regularizer;

  double                           myUnitsPerEm     = 0.0;
  double                           myTolerance      = 0.0;
  bool                             myIsSingleStroke = false;
  NCollection_LinearVector<gp_XY>  myVertices;
  NCollection_LinearVector<Curve>  myCurves;
  NCollection_LinearVector<Use>    myUses;
  NCollection_LinearVector<Loop>   myLoops;
  NCollection_LinearVector<LoopId> myHoleLoops;
  NCollection_LinearVector<Region> myRegions;
};

#endif // BRepFont_PlanarRegion_HeaderFile
