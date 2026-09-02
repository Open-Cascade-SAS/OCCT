// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in
// OCCT distribution for complete details on the license and disclaimer of any
// warranty.
//
// Alternatively, this file may be used under the terms of the Open CASCADE
// commercial license or contractual agreement.

#ifndef _BRepGraph_CacheChart_HeaderFile
#define _BRepGraph_CacheChart_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_Cache.hxx>
#include <BRepGraphInc_ParityOrientation.hxx>
#include <BRepGraph_NodeId.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Pnt2d.hxx>

#include <cstddef>
#include <cstdint>
#include <shared_mutex>

class Geom_Surface;

//! @brief Periodic face chart service and graph-local chart cache.
//!
//! Persistent graph topology contains real edges only. This service derives
//! periodic UV navigators and finite chart views on demand, and caches those
//! views against the graph generation. All chart records are nested in the
//! cache service so the public ownership model is explicit.
class BRepGraph_CacheChart : public BRepGraph_Cache
{
public:
  //! @brief Direction of a periodic chart cut.
  enum class Direction : uint8_t
  {
    U,
    V
  };

  //! @brief Kind of a boundary occurrence in a transient chart.
  enum class BoundaryKind : uint8_t
  {
    RealCoEdge,
    VirtualCut,
    ParametricBoundary,
    SingularBoundary
  };

  //! @brief Result status returned by chart derivation.
  enum class Status : uint8_t
  {
    Ready,
    NotPeriodic,
    MissingSurface,
    MissingGeometry,
    AmbiguousWinding,
    MissingBoundary,
    UnboundedDomain,
    SingularUnsupported,
    Unsupported,
    InvalidBoundary
  };

  //! @brief Tolerances used by different chart-space comparisons.
  struct Tolerance
  {
    double Tolerance3d             = 1.0e-7;
    double ToleranceU              = 1.0e-9;
    double ToleranceV              = 1.0e-9;
    double CurveParameterTolerance = 1.0e-9;
    double SingularityTolerance    = 1.0e-7;
  };

  //! @brief Request policy for transient chart calculation.
  struct Policy
  {
    enum class CutSelection : uint8_t
    {
      Canonical,
      MaxClearance,
      MinimizeSplits,
      Explicit
    };

    enum class CacheMode : uint8_t
    {
      Default,
      Provisional
    };

    CutSelection Selection          = CutSelection::Canonical;
    Direction    PreferredDirection = Direction::U;
    double       ExplicitU          = 0.0;
    double       ExplicitV          = 0.0;
    Tolerance    Tolerances;
    CacheMode    Caching = CacheMode::Default;
  };

  //! @brief Requirements imposed by a chart consumer.
  struct Requirements
  {
    bool RequireFiniteUVDomain         = true;
    bool RequireClosedUVLoops          = true;
    bool RequireMaterializableTopology = false;
  };

  //! @brief Integer tile translation in the UV covering space.
  struct UVLift
  {
    int32_t UPeriodIndex = 0;
    int32_t VPeriodIndex = 0;
  };

  //! @brief A periodic transition between two chart-local sides.
  struct Transition
  {
    Direction PeriodicDirection = Direction::U;
    double    Period            = 0.0;
    double    CutParameter      = 0.0;
    gp_Pnt2d  Shift;
    int32_t   PositiveSide = -1;
    int32_t   NegativeSide = -1;
  };

  //! @brief A selected transient coordinate-aligned cut.
  struct Cut
  {
    Direction PeriodicDirection = Direction::U;
    double    Parameter         = 0.0;
    double    Period            = 0.0;
    double    OtherFirst        = 0.0;
    double    OtherLast         = 0.0;
    int32_t   PairIndex         = -1;
  };

  //! @brief One ordered real or virtual occurrence in a chart loop.
  struct BoundaryUse
  {
    BoundaryKind                    Kind = BoundaryKind::RealCoEdge;
    BRepGraph_CoEdgeId              SourceCoEdge;
    uint32_t                        VirtualCutIndex = 0;
    UVLift                          Lift;
    BRepGraphInc::ParityOrientation Orientation;
    int32_t                         PairIndex  = -1;
    double                          ParamFirst = 0.0;
    double                          ParamLast  = 0.0;
    gp_Pnt2d                        UVFirst;
    gp_Pnt2d                        UVLast;
  };

  //! @brief One intrinsic boundary component viewed in a chart.
  struct Loop
  {
    BRepGraph_WireId                      SourceWire;
    NCollection_LinearVector<BoundaryUse> Boundary;
    bool                                  IsClosed = false;
  };

  //! @brief Immutable transient chart result.
  class Result : public Standard_Transient
  {
  public:
    [[nodiscard]] BRepGraph_CacheChart::Status GetStatus() const noexcept { return Status; }

    [[nodiscard]] BRepGraph_FaceId GetFace() const noexcept { return Face; }

    [[nodiscard]] bool IsUPeriodic() const noexcept { return UPeriodic; }

    [[nodiscard]] bool IsVPeriodic() const noexcept { return VPeriodic; }

    [[nodiscard]] double GetUPeriod() const noexcept { return UPeriod; }

    [[nodiscard]] double GetVPeriod() const noexcept { return VPeriod; }

    [[nodiscard]] double GetUFirst() const noexcept { return UFirst; }

    [[nodiscard]] double GetULast() const noexcept { return ULast; }

    [[nodiscard]] double GetVFirst() const noexcept { return VFirst; }

    [[nodiscard]] double GetVLast() const noexcept { return VLast; }

    [[nodiscard]] const NCollection_LinearVector<Cut>& GetCuts() const noexcept { return Cuts; }

    [[nodiscard]] const NCollection_LinearVector<Transition>& GetTransitions() const noexcept
    {
      return Transitions;
    }

    [[nodiscard]] const NCollection_LinearVector<Loop>& GetLoops() const noexcept { return Loops; }

    [[nodiscard]] const TCollection_AsciiString& GetDiagnostic() const noexcept
    {
      return Diagnostic;
    }

    [[nodiscard]] bool IsUsable() const noexcept
    {
      return Status == BRepGraph_CacheChart::Status::Ready
             || Status == BRepGraph_CacheChart::Status::NotPeriodic;
    }

    [[nodiscard]] bool IsMaterializable() const noexcept { return IsUsable() && !Loops.IsEmpty(); }

    DEFINE_STANDARD_RTTIEXT(Result, Standard_Transient)

  protected:
    BRepGraph_CacheChart::Status         Status = BRepGraph_CacheChart::Status::Unsupported;
    BRepGraph_FaceId                     Face;
    occ::handle<Geom_Surface>            Surface;
    bool                                 UPeriodic = false;
    bool                                 VPeriodic = false;
    double                               UPeriod   = 0.0;
    double                               VPeriod   = 0.0;
    double                               UFirst    = 0.0;
    double                               ULast     = 0.0;
    double                               VFirst    = 0.0;
    double                               VLast     = 0.0;
    NCollection_LinearVector<Cut>        Cuts;
    NCollection_LinearVector<Transition> Transitions;
    NCollection_LinearVector<Loop>       Loops;
    TCollection_AsciiString              Diagnostic;
  };

public:
  Standard_EXPORT BRepGraph_CacheChart();

  BRepGraph_CacheChart(const BRepGraph_CacheChart&)            = delete;
  BRepGraph_CacheChart& operator=(const BRepGraph_CacheChart&) = delete;

  //! Build a chart directly without using a cache.
  Standard_EXPORT static occ::handle<Result> Build(const BRepGraph&       theGraph,
                                                   const BRepGraph_FaceId theFace);

  //! Build a chart directly with an explicit policy and requirements.
  Standard_EXPORT static occ::handle<Result> Build(const BRepGraph&       theGraph,
                                                   const BRepGraph_FaceId theFace,
                                                   const Policy&          thePolicy,
                                                   const Requirements&    theRequirements);

  //! Return a cached chart, rebuilding it when the face subtree is stale.
  [[nodiscard]] Standard_EXPORT occ::handle<Result> Get(const BRepGraph_FaceId theFace);

  [[nodiscard]] Standard_EXPORT occ::handle<Result> Get(const BRepGraph_FaceId theFace,
                                                        const Policy&          thePolicy);

  [[nodiscard]] Standard_EXPORT occ::handle<Result> Get(const BRepGraph_FaceId theFace,
                                                        const Policy&          thePolicy,
                                                        const Requirements&    theRequirements);

  //! Stable graph-cache service identity.
  [[nodiscard]] static Standard_EXPORT const Standard_GUID& GetID();

  [[nodiscard]] Standard_EXPORT const Standard_GUID& ID() const override;

  [[nodiscard]] Standard_EXPORT const TCollection_AsciiString& Name() const override;

  //! Drop all derived chart views.
  Standard_EXPORT void Clear() noexcept override;

  //! Chart views contain graph-local source IDs and are intentionally not copied.
  Standard_EXPORT void CopyFreshTo(const BRepGraph_CopyRemap& theCopy) const override;

  //! Number of currently cached entries. Intended for diagnostics and tests.
  [[nodiscard]] Standard_EXPORT uint32_t NbEntries() const;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_CacheChart, BRepGraph_Cache)

private:
  struct Entry : public NodeEntry
  {
    Policy              PolicyValue;
    Requirements        RequirementsValue;
    occ::handle<Result> Chart;
  };

  using FaceEntries = NCollection_LinearVector<Entry>;

  [[nodiscard]] const Entry* findFreshLocked(const BRepGraph_FaceId theFace,
                                             const Policy&          thePolicy,
                                             const Requirements&    theRequirements) const;

  void eraseExplicitLocked(const BRepGraph_FaceId theFace,
                           const Policy&          thePolicy,
                           const Requirements&    theRequirements);

  void eraseIdentityLocked(const BRepGraph_FaceId theFace,
                           const Policy&          thePolicy,
                           const Requirements&    theRequirements);

  void trimFaceEntriesLocked(FaceEntries& theEntries);

  //! Limit policy variants per face while allowing the cache to cover all faces.
  static constexpr size_t THE_MAX_ENTRIES_PER_FACE = 8;

  mutable std::shared_mutex                          myMutex;
  NCollection_DataMap<BRepGraph_FaceId, FaceEntries> myEntriesByFace;
};

#endif // _BRepGraph_CacheChart_HeaderFile
