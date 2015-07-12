// Created on: 2001-05-03
// Created by: Igor FEOKTISTOV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _QANewModTopOpe_HeaderFile
#define _QANewModTopOpe_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
class TopoDS_Shape;
class TopoDS_Compound;
class QANewModTopOpe_Tools;
class QANewModTopOpe_Limitation;
class QANewModTopOpe_Intersection;
class QANewModTopOpe_Glue;
class QANewModTopOpe_ReShaper;


//! QANewModTopOpe  package  provides  classes  for  limitation, gluing
//! and removing "floating" shapes.
class QANewModTopOpe 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! to remove  "floating" objects from compound.
  //! "floating" objects are wires, edges, vertices that do not belong
  //! solids, shells or faces.
  //! to check if TheS is valid or not.
  //! in difference with BRepCheck_Analizer, this method allows
  //! some  kind of  non-manifold shapes
  Standard_EXPORT static Standard_Boolean IsValid (const TopoDS_Shape& TheS, const Standard_Boolean GeomControls = Standard_True);
  
  //! to check if TheS is manifol or not.
  //! manifold shape is valid SOLID, SHELL, WIRE, EDGE, VERTEX without internal
  //! subshapes - faces, wires, edges and vertices that have INTERNAL orientation
  //! For SHELL there are additional conditions: orientations of faces must
  //! corresponds each other, each edge must be shared not more then two faces.
  //! COMPSOLID is non manifold by default.
  //! COMPOUND conciders to  be manifold if:
  //! 1) all shapes in compound are manifold (see above mentioned definitions)
  //! 2) all shapes are fully disconnected - there are any shapes in  compound that
  //! share common subshapes.
  Standard_EXPORT static Standard_Boolean IsManifold (const TopoDS_Shape& TheS);
  
  //! to check if TheS is manifol or not.
  //! compound conciders to  be manifold if:
  //! 1) all shapes in compound are manifold (see comments for method IsManifold)
  //! 2) all shapes are fully disconnected - there are any shapes in  compound that
  //! share common subshapes.
  Standard_EXPORT static Standard_Boolean IsCompoundManifold (const TopoDS_Compound& TheS);
  
  //! to define if COMPOUND is homogeneous
  //! (consists of shapes of the same type) and return this shape type.
  //! If COMPOUND is mixed, method returns TopAbs_COMPOUND.
  //! If TheS is single shape (not COMPOUND), method returns its type.
  //! If COMPOUND contains nested compounds, it concideres to be homogeneous
  //! if all compounds consist of shapes of the same type.
  Standard_EXPORT static TopAbs_ShapeEnum TypeOfShape (const TopoDS_Shape& TheS);
  
  //! to check if all subshapes in TheS, when TheS is COMPOUND, COMPSOLID, SHELL or WIRE,
  //! are linked through common faces, edges or  vertices.
  //! SOLID, FACE, EDGE, VERTEX concider to be connected by default.
  Standard_EXPORT static Standard_Boolean IsConnected (const TopoDS_Shape& TheS);




protected:





private:




friend class QANewModTopOpe_Tools;
friend class QANewModTopOpe_Limitation;
friend class QANewModTopOpe_Intersection;
friend class QANewModTopOpe_Glue;
friend class QANewModTopOpe_ReShaper;

};







#endif // _QANewModTopOpe_HeaderFile
