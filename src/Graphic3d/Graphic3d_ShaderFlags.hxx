// Created on: 2014-10-08
// Created by: Kirill Gavrilov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_ShaderFlags_HeaderFile
#define _Graphic3d_ShaderFlags_HeaderFile

//! Standard GLSL program combination bits.
enum Graphic3d_ShaderFlags
{
  Graphic3d_ShaderFlags_VertColor       = 0x0001, //!< per-vertex color
  Graphic3d_ShaderFlags_TextureRGB      = 0x0002, //!< handle RGB texturing
  Graphic3d_ShaderFlags_TextureEnv      = 0x0004, //!< handle environment map (obsolete, to be removed)
  Graphic3d_ShaderFlags_TextureNormal   = Graphic3d_ShaderFlags_TextureRGB|Graphic3d_ShaderFlags_TextureEnv, //!< extended texture set (with normal map)
  Graphic3d_ShaderFlags_PointSimple     = 0x0008, //!< point marker without sprite
  Graphic3d_ShaderFlags_PointSprite     = 0x0010, //!< point sprite with RGB image
  Graphic3d_ShaderFlags_PointSpriteA    = Graphic3d_ShaderFlags_PointSimple|Graphic3d_ShaderFlags_PointSprite, //!< point sprite with Alpha image
  Graphic3d_ShaderFlags_StippleLine     = 0x0020, //!< stipple line
  Graphic3d_ShaderFlags_ClipPlanes1     = 0x0040, //!< handle 1 clipping plane
  Graphic3d_ShaderFlags_ClipPlanes2     = 0x0080, //!< handle 2 clipping planes
  Graphic3d_ShaderFlags_ClipPlanesN     = Graphic3d_ShaderFlags_ClipPlanes1|Graphic3d_ShaderFlags_ClipPlanes2, //!< handle N clipping planes
  Graphic3d_ShaderFlags_ClipChains      = 0x0100, //!< handle chains of clipping planes
  Graphic3d_ShaderFlags_MeshEdges       = 0x0200, //!< draw mesh edges (wireframe)
  Graphic3d_ShaderFlags_AlphaTest       = 0x0400, //!< discard fragment by alpha test (defined by cutoff value)
  Graphic3d_ShaderFlags_WriteOit        = 0x0800, //!< write coverage buffer for Blended Order-Independent Transparency
  Graphic3d_ShaderFlags_OitDepthPeeling = 0x1000, //!< handle Depth Peeling OIT
  //
  Graphic3d_ShaderFlags_NB              = 0x2000, //!< overall number of combinations
  Graphic3d_ShaderFlags_IsPoint         = Graphic3d_ShaderFlags_PointSimple|Graphic3d_ShaderFlags_PointSprite|Graphic3d_ShaderFlags_PointSpriteA,
  Graphic3d_ShaderFlags_HasTextures     = Graphic3d_ShaderFlags_TextureRGB|Graphic3d_ShaderFlags_TextureEnv,
  Graphic3d_ShaderFlags_NeedsGeomShader = Graphic3d_ShaderFlags_MeshEdges,
};

#endif // _Graphic3d_ShaderFlags_HeaderFile
