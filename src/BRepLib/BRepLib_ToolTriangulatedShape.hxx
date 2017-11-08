// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _BrepLib_ToolTriangulatedShape_HeaderFile
#define _BrepLib_ToolTriangulatedShape_HeaderFile

#include <Poly_Connect.hxx>
#include <Poly_Triangulation.hxx>

class TopoDS_Face;
class Poly_Triangulation;

//! Provides methods for calculating normals to Poly_Triangulation of TopoDS_Face.
class BRepLib_ToolTriangulatedShape
{
public:

  //! Computes nodal normals for Poly_Triangulation structure using UV coordinates and surface.
  //! Does nothing if triangulation already defines normals.
  //! @param[in] theFace the face
  //! @param[in] theTris the definition of a face triangulation
  static void ComputeNormals (const TopoDS_Face& theFace,
                              const Handle(Poly_Triangulation)& theTris)
  {
    Poly_Connect aPolyConnect;
    ComputeNormals (theFace, theTris, aPolyConnect);
  }

  //! Computes nodal normals for Poly_Triangulation structure using UV coordinates and surface.
  //! Does nothing if triangulation already defines normals.
  //! @param[in] theFace the face
  //! @param[in] theTris the definition of a face triangulation
  //! @param[in,out] thePolyConnect optional, initialized tool for exploring triangulation
  Standard_EXPORT static void ComputeNormals (const TopoDS_Face& theFace,
                                              const Handle(Poly_Triangulation)& theTris,
                                              Poly_Connect& thePolyConnect);

};

#endif
