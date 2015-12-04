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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MeshVS_DataMapOfIntegerColor.hxx>
#include <Standard_Boolean.hxx>
#include <Aspect_SequenceOfColor.hxx>
#include <TColStd_DataMapOfIntegerReal.hxx>
#include <Quantity_Color.hxx>
#include <MeshVS_PrsBuilder.hxx>
#include <MeshVS_DisplayModeFlags.hxx>
#include <Standard_Integer.hxx>
#include <MeshVS_BuilderPriority.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <Standard_Real.hxx>
#include <MeshVS_HArray1OfSequenceOfInteger.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
class MeshVS_Mesh;
class MeshVS_DataSource;
class Prs3d_Presentation;
class Quantity_Color;
class Graphic3d_Texture2D;
class Graphic3d_ArrayOfPrimitives;


class MeshVS_NodalColorPrsBuilder;
DEFINE_STANDARD_HANDLE(MeshVS_NodalColorPrsBuilder, MeshVS_PrsBuilder)

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

  
  Standard_EXPORT MeshVS_NodalColorPrsBuilder(const Handle(MeshVS_Mesh)& Parent, const MeshVS_DisplayModeFlags& Flags = MeshVS_DMF_NodalColorDataPrs, const Handle(MeshVS_DataSource)& DS = 0, const Standard_Integer Id = -1, const MeshVS_BuilderPriority& Priority = MeshVS_BP_NodalColor);
  
  //! Builds presentation of nodes with assigned color.
  Standard_EXPORT virtual void Build (const Handle(Prs3d_Presentation)& Prs, const TColStd_PackedMapOfInteger& IDs, TColStd_PackedMapOfInteger& IDsToExclude, const Standard_Boolean IsElement, const Standard_Integer DisplayMode) const Standard_OVERRIDE;
  
  //! Returns map of colors assigned to nodes.
  Standard_EXPORT const MeshVS_DataMapOfIntegerColor& GetColors() const;
  
  //! Sets map of colors assigned to nodes.
  Standard_EXPORT void SetColors (const MeshVS_DataMapOfIntegerColor& Map);
  
  //! Returns true, if map isn't empty
  Standard_EXPORT Standard_Boolean HasColors() const;
  
  //! Returns color assigned to single node
  Standard_EXPORT Standard_Boolean GetColor (const Standard_Integer ID, Quantity_Color& theColor) const;
  
  //! Sets color assigned to single node
  Standard_EXPORT void SetColor (const Standard_Integer ID, const Quantity_Color& theColor);
  
  //! Specify whether texture must be used to build presentation
  Standard_EXPORT void UseTexture (const Standard_Boolean theToUse);
  
  //! Verify whether texture is used to build presentation
  Standard_EXPORT Standard_Boolean IsUseTexture() const;
  
  //! Set colors to be used for texrture presentation
  //! theColors - colors for valid coordinates (laying in range [0, 1])
  Standard_EXPORT void SetColorMap (const Aspect_SequenceOfColor& theColors);
  
  //! Return colors used for texrture presentation
  Standard_EXPORT const Aspect_SequenceOfColor& GetColorMap() const;
  
  //! Set color representing invalid texture coordinate
  //! (laying outside range [0, 1])
  Standard_EXPORT void SetInvalidColor (const Quantity_Color& theInvalidColor);
  
  //! Return color representing invalid texture coordinate
  //! (laying outside range [0, 1])
  Standard_EXPORT Quantity_Color GetInvalidColor() const;
  
  //! Specify correspondence between node IDs and texture coordinates (range [0, 1])
  Standard_EXPORT void SetTextureCoords (const TColStd_DataMapOfIntegerReal& theMap);
  
  //! Get correspondence between node IDs and texture coordinates (range [0, 1])
  Standard_EXPORT const TColStd_DataMapOfIntegerReal& GetTextureCoords() const;
  
  //! Specify correspondence between node ID and texture coordinate (range [0, 1])
  Standard_EXPORT void SetTextureCoord (const Standard_Integer theID, const Standard_Real theCoord);
  
  //! Return correspondence between node IDs and texture coordinate (range [0, 1])
  Standard_EXPORT Standard_Real GetTextureCoord (const Standard_Integer theID);
  
  //! Add to array polygons or polylines representing volume
  Standard_EXPORT void AddVolumePrs (const Handle(MeshVS_HArray1OfSequenceOfInteger)& theTopo, const TColStd_Array1OfInteger& theNodes, const TColStd_Array1OfReal& theCoords, const Handle(Graphic3d_ArrayOfPrimitives)& theArray, const Standard_Boolean theIsShaded, const Standard_Integer theNbColors, const Standard_Integer theNbTexColors, const Standard_Real theColorRatio) const;




  DEFINE_STANDARD_RTTIEXT(MeshVS_NodalColorPrsBuilder,MeshVS_PrsBuilder)

protected:




private:

  
  //! Create texture in accordance with myTextureColorMap
  Standard_EXPORT Handle(Graphic3d_Texture2D) CreateTexture() const;

  MeshVS_DataMapOfIntegerColor myNodeColorMap;
  Standard_Boolean myUseTexture;
  Aspect_SequenceOfColor myTextureColorMap;
  TColStd_DataMapOfIntegerReal myTextureCoords;
  Quantity_Color myInvalidColor;


};







#endif // _MeshVS_NodalColorPrsBuilder_HeaderFile
