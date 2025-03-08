// Created on: 2003-10-10
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

#ifndef _MeshVS_DrawerAttribute_HeaderFile
#define _MeshVS_DrawerAttribute_HeaderFile

//! Is it allowed to draw beam and face's edge overlapping with this beam.
//! Is mesh drawn with reflective material
//! Is colored mesh data representation drawn with reflective material
//! What part of face or link will be shown if shrink mode.
//! It is recommended this coeff to be between 0 and 1.
//! How many nodes is possible to be in face
//! If this parameter is true, the compute method CPU time will be displayed in console window
//! If this parameter is true, the compute selection method CPU time will be displayed in console
//! window If this parameter is false, the nodes won't be shown in viewer, otherwise will be.//! If
//! this parameter is true, the selectable nodes map will be updated automatically when hidden
//! elements change//! If this parameter is false, the face's edges are not shown Warning: in
//! wireframe mode this parameter is ignored Is mesh drawing in smooth shading mode Is back faces of
//! volume elements should be suppressed The integer keys for most useful constants attuning mesh
//! presentation appearance WARNING: DA_TextExpansionFactor, DA_TextSpace, DA_TextDisplayType have
//! no effect and might be removed in the future.
enum MeshVS_DrawerAttribute
{
  MeshVS_DA_InteriorStyle,
  MeshVS_DA_InteriorColor,
  MeshVS_DA_BackInteriorColor,
  MeshVS_DA_EdgeColor,
  MeshVS_DA_EdgeType,
  MeshVS_DA_EdgeWidth,
  MeshVS_DA_HatchStyle,
  MeshVS_DA_FrontMaterial,
  MeshVS_DA_BackMaterial,
  MeshVS_DA_BeamType,
  MeshVS_DA_BeamWidth,
  MeshVS_DA_BeamColor,
  MeshVS_DA_MarkerType,
  MeshVS_DA_MarkerColor,
  MeshVS_DA_MarkerScale,
  MeshVS_DA_TextColor,
  MeshVS_DA_TextHeight,
  MeshVS_DA_TextFont,
  MeshVS_DA_TextExpansionFactor,
  MeshVS_DA_TextSpace,
  MeshVS_DA_TextStyle,
  MeshVS_DA_TextDisplayType,
  MeshVS_DA_TextTexFont,
  MeshVS_DA_TextFontAspect,
  MeshVS_DA_VectorColor,
  MeshVS_DA_VectorMaxLength,
  MeshVS_DA_VectorArrowPart,
  MeshVS_DA_IsAllowOverlapped,
  MeshVS_DA_Reflection,
  MeshVS_DA_ColorReflection,
  MeshVS_DA_ShrinkCoeff,
  MeshVS_DA_MaxFaceNodes,
  MeshVS_DA_ComputeTime,
  MeshVS_DA_ComputeSelectionTime,
  MeshVS_DA_DisplayNodes,
  MeshVS_DA_SelectableAuto,
  MeshVS_DA_ShowEdges,
  MeshVS_DA_SmoothShading,
  MeshVS_DA_SupressBackFaces,
  MeshVS_DA_User
};

#endif // _MeshVS_DrawerAttribute_HeaderFile
