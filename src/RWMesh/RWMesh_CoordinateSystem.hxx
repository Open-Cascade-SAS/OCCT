// Author: Kirill Gavrilov
// Copyright: Open CASCADE 2015-2019
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

#ifndef _RWMesh_CoordinateSystem_HeaderFile
#define _RWMesh_CoordinateSystem_HeaderFile

//! Standard coordinate system definition.
//! Open CASCADE does not force application using specific coordinate system,
//! although Draw Harness and samples define +Z-up +Y-forward coordinate system for camera view manipulation.
//! This enumeration defines two commonly used conventions - Z-up and Y-up..
enum RWMesh_CoordinateSystem
{
  RWMesh_CoordinateSystem_Undefined      = -1, //!< undefined
  RWMesh_CoordinateSystem_posYfwd_posZup =  0, //!< +YForward+Zup+Xright
  RWMesh_CoordinateSystem_negZfwd_posYup,      //!< -ZForward+Yup+Xright

  RWMesh_CoordinateSystem_Blender = RWMesh_CoordinateSystem_posYfwd_posZup, //!< coordinate system used by Blender (+YForward+Zup+Xright)
  RWMesh_CoordinateSystem_glTF    = RWMesh_CoordinateSystem_negZfwd_posYup, //!< coordinate system used by glTF    (-ZForward+Yup+Xright)
  RWMesh_CoordinateSystem_Zup     = RWMesh_CoordinateSystem_Blender,        //!< Z-up coordinate system (+YForward+Zup+Xright)
  RWMesh_CoordinateSystem_Yup     = RWMesh_CoordinateSystem_glTF,           //!< Y-up coordinate system (-ZForward+Yup+Xright)
};

#endif // _RWMesh_CoordinateSystem_HeaderFile
