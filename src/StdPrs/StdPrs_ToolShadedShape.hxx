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

#ifndef _StdPrs_ToolShadedShape_HeaderFile
#define _StdPrs_ToolShadedShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Handle_Poly_Triangulation.hxx>

class TopoDS_Shape;
class Poly_Triangulation;
class TopoDS_Face;
class TopLoc_Location;
class Poly_Connect;
class TColgp_Array1OfDir;

class StdPrs_ToolShadedShape
{
public:

  DEFINE_STANDARD_ALLOC

  //! Similar to BRepTools::Triangulation() but without extra checks.
  //! @return true if all faces within shape are triangulated.
  Standard_EXPORT static Standard_Boolean IsTriangulated (const TopoDS_Shape& theShape);

  //! Checks back faces visibility for specified shape (to activate back-face culling). <br>
  //! @return true if shape is closed manifold Solid or compound of such Solids. <br>
  Standard_EXPORT static Standard_Boolean IsClosed(const TopoDS_Shape& theShape);
  
  Standard_EXPORT static Handle_Poly_Triangulation Triangulation(const TopoDS_Face& aFace,
                                                 TopLoc_Location& loc);
  
  Standard_EXPORT static void Normal(const TopoDS_Face& aFace,
                                     Poly_Connect& PC,
                                     TColgp_Array1OfDir& Nor);
};

#endif
