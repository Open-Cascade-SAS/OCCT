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

#include <BRepLib_ToolTriangulatedShape.hxx>
#include <TColgp_Array1OfDir.hxx>

class TopoDS_Shape;
class Prs3d_Drawer;

class StdPrs_ToolTriangulatedShape: public BRepLib_ToolTriangulatedShape
{
public:

  //! Similar to BRepTools::Triangulation() but without extra checks.
  //! @return true if all faces within shape are triangulated.
  Standard_EXPORT static Standard_Boolean IsTriangulated (const TopoDS_Shape& theShape);

  //! Checks back faces visibility for specified shape (to activate back-face culling). <br>
  //! @return true if shape is closed manifold Solid or compound of such Solids. <br>
  Standard_EXPORT static Standard_Boolean IsClosed (const TopoDS_Shape& theShape);

  //! Computes the absolute deflection value depending on the type of deflection in theDrawer:
  //! <ul>
  //! <li><b>Aspect_TOD_RELATIVE</b>: the absolute deflection is computed using the relative
  //! deviation coefficient from theDrawer and the shape's bounding box;</li>
  //! <li><b>Aspect_TOD_ABSOLUTE</b>: the maximal chordial deviation from theDrawer is returned.</li>
  //! </ul>
  //! In case of the type of deflection in theDrawer computed relative deflection for shape is stored as absolute deflection.
  //! It is necessary to use it later on for sub-shapes.
  //! This function should always be used to compute the deflection value for building
  //! discrete representations of the shape (triangulation, wireframe) to avoid inconsistencies
  //! between different representations of the shape and undesirable visual artifacts.
  Standard_EXPORT static Standard_Real GetDeflection (const TopoDS_Shape& theShape,
                                                      const Handle(Prs3d_Drawer)& theDrawer);

  //! Checks whether the shape is properly triangulated for a given display settings.
  //! @param theShape [in] the shape.
  //! @param theDrawer [in] the display settings.
  Standard_EXPORT static Standard_Boolean IsTessellated (const TopoDS_Shape& theShape,
                                                         const Handle(Prs3d_Drawer)& theDrawer);

  //! Validates triangulation within the shape and performs tessellation if necessary.
  //! @param theShape [in] the shape.
  //! @param theDrawer [in] the display settings.
  //! @return true if tessellation was recomputed and false otherwise.
  Standard_EXPORT static Standard_Boolean Tessellate (const TopoDS_Shape& theShape,
                                                      const Handle(Prs3d_Drawer)& theDrawer);

  //! If presentation has own deviation coefficient and IsAutoTriangulation() is true,
  //! function will compare actual coefficients with previous values and will clear triangulation on their change
  //! (regardless actual tessellation quality).
  //! Function is placed here for compatibility reasons - new code should avoid using IsAutoTriangulation().
  //! @param theShape  [in] the shape
  //! @param theDrawer [in] the display settings
  //! @param theToResetCoeff [in] updates coefficients in theDrawer to actual state to avoid redundant recomputations
  Standard_EXPORT static void ClearOnOwnDeflectionChange (const TopoDS_Shape& theShape,
                                                          const Handle(Prs3d_Drawer)& theDrawer,
                                                          const Standard_Boolean theToResetCoeff);

};

#endif
