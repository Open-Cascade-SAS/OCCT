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

#ifndef _BRepGraph_LayerParametric_HeaderFile
#define _BRepGraph_LayerParametric_HeaderFile

#include <BRepGraph_Layer.hxx>

#include <cstdint>
#include <limits>

class BRepGraph_LayerLock;

//! @brief Base layer for graph-owned parametric generators.
//!
//! The class defines the common instance identity, generation flags, mesh
//! quality controls, and graph access helpers shared by higher-level parametric
//! layers.
//! Concrete layers such as BRepGraphPrim box, plane, or loft generators build
//! their own parameter schema and manifest storage on top of this base.
class BRepGraph_LayerParametric : public BRepGraph_Layer
{
public:
  //! Controls which graph artifacts should be created or refreshed.
  enum class GenerationFlag : uint32_t
  {
    Topology = 0x01, //!< Create or preserve topological nodes and references.
    Geometry = 0x02, //!< Bind analytic reps; absence keeps generated topology mesh-only.
    Mesh     = 0x04  //!< Create or preserve triangulation representations.
  };

  //! High-level mesh quality hint shared by parametric generators.
  enum class MeshQuality : uint8_t
  {
    VeryCoarse, //!< Minimal preview-oriented detail.
    Coarse,     //!< Low detail for fast authoring feedback.
    Medium,     //!< Balanced default-quality detail.
    Fine,       //!< High-quality detail for closer inspection.
    VeryFine    //!< Maximum detail requested by the caller.
  };

  //! Result of adding a new parametric instance to a graph.
  struct AddResult
  {
    uint32_t         Instance = THE_INVALID_INSTANCE; //!< Created instance identifier.
    BRepGraph_NodeId Root; //!< Root topology node of the created subtree.
  };

  //! Reserved sentinel used when an operation does not create an instance.
  static constexpr uint32_t THE_INVALID_INSTANCE = std::numeric_limits<uint32_t>::max();

  //! Default generation mode builds topology and analytic geometry only.
  static constexpr uint32_t THE_DEFAULT_GENERATION_FLAGS =
    static_cast<uint32_t>(GenerationFlag::Topology)
    | static_cast<uint32_t>(GenerationFlag::Geometry);

  //! Convert one generation flag into its bit-mask value.
  //! @param[in] theFlag generation flag to convert
  //! @return bit-mask value for the requested generation flag
  [[nodiscard]] static constexpr uint32_t GenerationMask(const GenerationFlag theFlag)
  {
    return static_cast<uint32_t>(theFlag);
  }

  //! Return true when the flag mask contains the requested generation flag.
  //! @param[in] theFlags generation mask built from GenerationFlag bits
  //! @param[in] theFlag generation flag to test
  //! @return true when the flag is present in the mask
  [[nodiscard]] static constexpr bool HasGenerationFlag(const uint32_t       theFlags,
                                                        const GenerationFlag theFlag)
  {
    return (theFlags & GenerationMask(theFlag)) != 0;
  }

  //! Select one integer value from a mesh-quality ladder.
  //! @param[in] theQuality requested shared mesh quality
  //! @param[in] theVeryCoarse value for MeshQuality::VeryCoarse
  //! @param[in] theCoarse value for MeshQuality::Coarse
  //! @param[in] theMedium value for MeshQuality::Medium
  //! @param[in] theFine value for MeshQuality::Fine
  //! @param[in] theVeryFine value for MeshQuality::VeryFine
  //! @return selected value for the requested quality
  [[nodiscard]] static constexpr uint32_t MeshQualityValue(const MeshQuality theQuality,
                                                           const uint32_t    theVeryCoarse,
                                                           const uint32_t    theCoarse,
                                                           const uint32_t    theMedium,
                                                           const uint32_t    theFine,
                                                           const uint32_t    theVeryFine)
  {
    switch (theQuality)
    {
      case MeshQuality::VeryCoarse:
        return theVeryCoarse;
      case MeshQuality::Coarse:
        return theCoarse;
      case MeshQuality::Medium:
        return theMedium;
      case MeshQuality::Fine:
        return theFine;
      case MeshQuality::VeryFine:
        return theVeryFine;
    }
    return theMedium;
  }

protected:
  //! Return the attached graph and raise if the layer is detached.
  //! @return attached graph for mutation operations
  [[nodiscard]] Standard_EXPORT BRepGraph* graphForMutation() const;

  //! Return the ownership layer used by parametric generators.
  //! @param[in] theGraph graph whose ownership layer should be returned
  //! @return ownership layer handle
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_LayerLock> lockLayer(
    BRepGraph& theGraph) const;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_LayerParametric, BRepGraph_Layer)
};

#endif // _BRepGraph_LayerParametric_HeaderFile
