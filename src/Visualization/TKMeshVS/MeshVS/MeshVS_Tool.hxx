// Created on: 2003-12-17
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

#ifndef _MeshVS_Tool_HeaderFile
#define _MeshVS_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_Array1.hxx>
class Graphic3d_AspectFillArea3d;
class MeshVS_Drawer;
class Graphic3d_MaterialAspect;
class Graphic3d_AspectLine3d;
class Graphic3d_AspectMarker3d;
class Graphic3d_AspectText3d;
class gp_Vec;

//! This class provides auxiliary methods to create different aspects
class MeshVS_Tool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates fill area aspect with values from Drawer according to keys from DrawerAttribute
  Standard_EXPORT static occ::handle<Graphic3d_AspectFillArea3d> CreateAspectFillArea3d(
    const occ::handle<MeshVS_Drawer>& theDr,
    const bool       UseDefaults = true);

  //! Creates fill aspect with values from Drawer according to keys from DrawerAttribute
  //! and specific material aspect
  Standard_EXPORT static occ::handle<Graphic3d_AspectFillArea3d> CreateAspectFillArea3d(
    const occ::handle<MeshVS_Drawer>&    theDr,
    const Graphic3d_MaterialAspect& Mat,
    const bool          UseDefaults = true);

  //! Creates line aspect with values from Drawer according to keys from DrawerAttribute
  Standard_EXPORT static occ::handle<Graphic3d_AspectLine3d> CreateAspectLine3d(
    const occ::handle<MeshVS_Drawer>& theDr,
    const bool       UseDefaults = true);

  //! Creates marker aspect with values from Drawer according to keys from DrawerAttribute
  Standard_EXPORT static occ::handle<Graphic3d_AspectMarker3d> CreateAspectMarker3d(
    const occ::handle<MeshVS_Drawer>& theDr,
    const bool       UseDefaults = true);

  //! Creates text aspect with values from Drawer according to keys from DrawerAttribute
  Standard_EXPORT static occ::handle<Graphic3d_AspectText3d> CreateAspectText3d(
    const occ::handle<MeshVS_Drawer>& theDr,
    const bool       UseDefaults = true);

  //! Get one of normals to polygon described by these points.
  //! If the polygon isn't planar, function returns false
  Standard_EXPORT static bool GetNormal(const NCollection_Array1<double>& Nodes,
                                                    gp_Vec&                     Norm);

  //! Get an average of normals to non-planar polygon described by these points or compute
  //! normal of planar polygon. If the polygon isn't planar, function returns false
  Standard_EXPORT static bool GetAverageNormal(const NCollection_Array1<double>& Nodes,
                                                           gp_Vec&                     Norm);

};

#endif // _MeshVS_Tool_HeaderFile
