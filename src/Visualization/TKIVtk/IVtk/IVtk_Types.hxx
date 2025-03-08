// Created on: 2011-10-11
// Created by: Roman KOZLOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef __IVTK_TYPES_H__
#define __IVTK_TYPES_H__

#include <gp_XY.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <vtkType.h>

#include <limits>

#ifdef VTK_USE_64BIT_IDS
  #if defined(_WIN32) && !defined(_WIN64)
    #error "64-bit VTK library can not be linked for 32-bit target platform"
  #endif
#else
  #ifdef _WIN64
    #error "32-bit VTK library can not be linked for 64-bit target platform"
  #endif
#endif

typedef vtkIdType   IVtk_IdType;
typedef IVtk_IdType IVtk_PointId;

typedef IVtk_IdType IVtk_FaceId;
typedef IVtk_IdType IVtk_EdgeId;

typedef NCollection_List<IVtk_IdType>  IVtk_ShapeIdList;
typedef NCollection_List<IVtk_PointId> IVtk_PointIdList;

typedef NCollection_DataMap<IVtk_IdType, IVtk_ShapeIdList> IVtk_SubShapeMap;
typedef NCollection_Map<IVtk_IdType>                       IVtk_IdTypeMap;

typedef NCollection_List<gp_XY> IVtk_Pnt2dList;

//! @enum IVtk_SelectionMode Selection modes for 3D shapes
//!
//! Enumeration that describes all supported selection modes for 3D shapes.
//! SM_None means that the shape should become non-selectable.
//! SM_Shape makes the shape selectable as a whole.
//! Other modes activate selection of sub-shapes of corresponding types.
typedef enum
{
  SM_None      = -1, //!< No selection
  SM_Shape     = 0,  //!< Shape selection
  SM_Vertex    = 1,  //!< Vertex selection
  SM_Edge      = 2,  //!< Edge selection
  SM_Wire      = 3,  //!< Wire selection
  SM_Face      = 4,  //!< Face selection
  SM_Shell     = 5,  //!< Shell selection
  SM_Solid     = 6,  //!< Solid selection
  SM_CompSolid = 7,  //!< CompSolid selection
  SM_Compound  = 8,  //!< Compound selection
} IVtk_SelectionMode;

typedef NCollection_List<IVtk_SelectionMode> IVtk_SelectionModeList;

//! @enum IVtk_MeshType Types of mesh parts for 3D shapes
//!
//! Enumeration that describes all supported types of mesh parts for 3D shapes.
typedef enum
{
  MT_Undefined     = -1, //!< Undefined
  MT_IsoLine       = 0,  //!< Isoline
  MT_FreeVertex    = 1,  //!< Free vertex
  MT_SharedVertex  = 2,  //!< Shared vertex
  MT_FreeEdge      = 3,  //!< Free edge
  MT_BoundaryEdge  = 4,  //!< Boundary edge (related to a single face)
  MT_SharedEdge    = 5,  //!< Shared edge (related to several faces)
  MT_WireFrameFace = 6,  //!< Wireframe face
  MT_ShadedFace    = 7,  //!< Shaded face
  MT_SeamEdge      = 8   //!< Seam edge between faces
} IVtk_MeshType;

//! @enum IVtk_DisplayMode Display modes for 3D shapes
//!
//! Enumeration that describes all supported display modes for 3D shapes.
typedef enum
{
  DM_Wireframe = 0, //!< Wireframe display mode
  DM_Shading   = 1  //!< Shaded display mode
} IVtk_DisplayMode;

#endif // __IVTK_TYPES_H__
