// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _StdPrs_ToolTriangulatedShape_HeaderFile
#define _StdPrs_ToolTriangulatedShape_HeaderFile

#include <Poly_Triangulation.hxx>
#include <Prs3d_Drawer.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <TColgp_Array1OfDir.hxx>

class TopoDS_Face;
class TopLoc_Location;
class TopoDS_Shape;
class Prs3d_Drawer;
class Poly_Triangulation;
class Poly_Connect;

class StdPrs_ToolTriangulatedShape
{
public:

  //! Similar to BRepTools::Triangulation() but without extra checks.
  //! @return true if all faces within shape are triangulated.
  Standard_EXPORT static Standard_Boolean IsTriangulated (const TopoDS_Shape& theShape);

  //! Checks back faces visibility for specified shape (to activate back-face culling). <br>
  //! @return true if shape is closed manifold Solid or compound of such Solids. <br>
  Standard_EXPORT static Standard_Boolean IsClosed (const TopoDS_Shape& theShape);

  //! Evaluate normals for a triangle of a face.
  //! @param theFace [in] the face.
  //! @param thePolyConnect [in] the definition of a face triangulation.
  //! @param theNormal [out] the array of normals for each triangle.
  Standard_EXPORT static void Normal (const TopoDS_Face& theFace,
                                      Poly_Connect& thePolyConnect,
                                      TColgp_Array1OfDir& theNormals);

  //! Checks whether the shape is properly triangulated for a given display settings.
  //! @param theShape [in] the shape.
  //! @param theDrawer [in] the display settings.
  Standard_EXPORT static Standard_Boolean IsTessellated (const TopoDS_Shape& theShape,
                                                         const Handle(Prs3d_Drawer)& theDrawer);

  //! Validates triangulation within the shape and performs tessellation if necessary.
  //! @param theShape [in] the shape.
  //! @param theDrawer [in] the display settings.
  //! @return true if tesselation was recomputed and false otherwise.
  Standard_EXPORT static Standard_Boolean Tessellate (const TopoDS_Shape& theShape,
                                                      const Handle(Prs3d_Drawer)& theDrawer);
};

#endif
