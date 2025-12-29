// Created on: 2003-11-12
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _MeshVS_NodalColorPrsBuilder_HeaderFile
#define _MeshVS_NodalColorPrsBuilder_HeaderFile

#include <Standard_Integer.hxx>
#include <Quantity_Color.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Sequence.hxx>
#include <MeshVS_PrsBuilder.hxx>
#include <MeshVS_DisplayModeFlags.hxx>
#include <MeshVS_BuilderPriority.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

class MeshVS_Mesh;
class MeshVS_DataSource;
class Graphic3d_Texture2D;
class Graphic3d_ArrayOfPrimitives;

//! This class provides methods to create presentation of nodes with assigned color.
//! There are two ways of presentation building
//! 1. Without using texture.
//! In this case colors of nodes are specified with DataMapOfIntegerColor and presentation
//! is built with gradient fill between these nodes (default behaviour)
//! 2. Using texture.
//! In this case presentation is built with spectrum filling between nodes. For example, if
//! one node has blue color and second one has violet color, parameters of this class may be
//! set to fill presentation between nodes with solar spectrum.
//! Methods:
//! UseTexture - activates/deactivates this way
//! SetColorMap - sets colors used for generation of texture
//! SetColorindices - specifies correspondence between node IDs and indices of colors from color map
class MeshVS_NodalColorPrsBuilder : public MeshVS_PrsBuilder
{

public:
  Standard_EXPORT MeshVS_NodalColorPrsBuilder(
    const occ::handle<MeshVS_Mesh>&       Parent,
    const MeshVS_DisplayModeFlags&        Flags    = MeshVS_DMF_NodalColorDataPrs,
    const occ::handle<MeshVS_DataSource>& DS       = nullptr,
    const int                             Id       = -1,
    const MeshVS_BuilderPriority&         Priority = MeshVS_BP_NodalColor);

  //! Builds presentation of nodes with assigned color.
  Standard_EXPORT void Build(const occ::handle<Prs3d_Presentation>& Prs,
                             const TColStd_PackedMapOfInteger&      IDs,
                             TColStd_PackedMapOfInteger&            IDsToExclude,
                             const bool                             IsElement,
                             const int                              DisplayMode) const override;

  //! Returns map of colors assigned to nodes.
  Standard_EXPORT const NCollection_DataMap<int, Quantity_Color>& GetColors() const;

  //! Sets map of colors assigned to nodes.
  Standard_EXPORT void SetColors(const NCollection_DataMap<int, Quantity_Color>& Map);

  //! Returns true, if map isn't empty
  Standard_EXPORT bool HasColors() const;

  //! Returns color assigned to single node
  Standard_EXPORT bool GetColor(const int ID, Quantity_Color& theColor) const;

  //! Sets color assigned to single node
  Standard_EXPORT void SetColor(const int ID, const Quantity_Color& theColor);

  //! Specify whether texture must be used to build presentation
  Standard_EXPORT void UseTexture(const bool theToUse);

  //! Verify whether texture is used to build presentation
  Standard_EXPORT bool IsUseTexture() const;

  //! Set colors to be used for texrture presentation
  //! theColors - colors for valid coordinates (laying in range [0, 1])
  Standard_EXPORT void SetColorMap(const NCollection_Sequence<Quantity_Color>& theColors);

  //! Return colors used for texrture presentation
  Standard_EXPORT const NCollection_Sequence<Quantity_Color>& GetColorMap() const;

  //! Set color representing invalid texture coordinate
  //! (laying outside range [0, 1])
  Standard_EXPORT void SetInvalidColor(const Quantity_Color& theInvalidColor);

  //! Return color representing invalid texture coordinate
  //! (laying outside range [0, 1])
  Standard_EXPORT Quantity_Color GetInvalidColor() const;

  //! Specify correspondence between node IDs and texture coordinates (range [0, 1])
  Standard_EXPORT void SetTextureCoords(const NCollection_DataMap<int, double>& theMap);

  //! Get correspondence between node IDs and texture coordinates (range [0, 1])
  Standard_EXPORT const NCollection_DataMap<int, double>& GetTextureCoords() const;

  //! Specify correspondence between node ID and texture coordinate (range [0, 1])
  Standard_EXPORT void SetTextureCoord(const int theID, const double theCoord);

  //! Return correspondence between node IDs and texture coordinate (range [0, 1])
  Standard_EXPORT double GetTextureCoord(const int theID);

  //! Add to array polygons or polylines representing volume
  Standard_EXPORT void AddVolumePrs(
    const occ::handle<NCollection_HArray1<NCollection_Sequence<int>>>& theTopo,
    const NCollection_Array1<int>&                                     theNodes,
    const NCollection_Array1<double>&                                  theCoords,
    const occ::handle<Graphic3d_ArrayOfPrimitives>&                    theArray,
    const bool                                                         theIsShaded,
    const int                                                          theNbColors,
    const int                                                          theNbTexColors,
    const double                                                       theColorRatio) const;

  DEFINE_STANDARD_RTTIEXT(MeshVS_NodalColorPrsBuilder, MeshVS_PrsBuilder)

private:
  //! Create texture in accordance with myTextureColorMap
  Standard_EXPORT occ::handle<Graphic3d_Texture2D> CreateTexture() const;

  NCollection_DataMap<int, Quantity_Color> myNodeColorMap;
  bool                                     myUseTexture;
  NCollection_Sequence<Quantity_Color>     myTextureColorMap;
  NCollection_DataMap<int, double>         myTextureCoords;
  Quantity_Color                           myInvalidColor;
};

#endif // _MeshVS_NodalColorPrsBuilder_HeaderFile
