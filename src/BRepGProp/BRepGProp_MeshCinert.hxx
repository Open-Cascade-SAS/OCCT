// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _BRepGProp_MeshCinert_HeaderFile
#define _BRepGProp_MeshCinert_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <GProp_GProps.hxx>
class gp_Pnt;
class TopoDS_Edge;

//! Computes the  global properties of
//! of polylines  represented by set of points.
//! This class is used for computation of global
//! properties of edge, which has no exact geometry
//! (3d or 2d curve), but has any of allowed
//! polygons.
//!
class BRepGProp_MeshCinert : public GProp_GProps
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepGProp_MeshCinert();

  Standard_EXPORT void SetLocation(const gp_Pnt& CLocation);

  //! Computes the  global properties of
  //! of polylines  represented by set of points.
  Standard_EXPORT void Perform(const TColgp_Array1OfPnt& theNodes);

  //! Prepare set of 3d points on base of any available edge polygons:
  //! 3D polygon, polygon on triangulation, 2d polygon on surface
  //! If edge has no polygons, array thePolyg is left unchanged
  Standard_EXPORT static void PreparePolygon(const TopoDS_Edge&           theE,
                                             Handle(TColgp_HArray1OfPnt)& thePolyg);

protected:
private:
};

#endif // _BRepGProp_MeshCinert_HeaderFile
