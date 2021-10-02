// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _BRepMesh_ShapeVisitor_HeaderFile
#define _BRepMesh_ShapeVisitor_HeaderFile

#include <IMeshTools_ShapeVisitor.hxx>
#include <IMeshData_Model.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <IMeshTools_Parameters.hxx>
#include <IMeshData_Types.hxx>

class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Wire;

//! Builds discrete model of a shape by adding faces and free edges.
//! Computes deflection for corresponded shape and checks whether it
//! fits existing polygonal representation. If not, cleans shape from
//! outdated info.
class BRepMesh_ShapeVisitor : public IMeshTools_ShapeVisitor
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_ShapeVisitor (const Handle (IMeshData_Model)& theModel);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_ShapeVisitor ();

  //! Handles TopoDS_Face object.
  Standard_EXPORT virtual void Visit (const TopoDS_Face& theFace) Standard_OVERRIDE;

  //! Handles TopoDS_Edge object.
  Standard_EXPORT virtual void Visit (const TopoDS_Edge& theEdge) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRepMesh_ShapeVisitor, IMeshTools_ShapeVisitor)

private:

  //! Adds wire to face discrete model.
  Standard_Boolean addWire (
    const TopoDS_Wire&            theWire,
    const IMeshData::IFaceHandle& theDFace);

private:

  Handle (IMeshData_Model)      myModel;
  IMeshData::DMapOfShapeInteger myDEdgeMap;
};

#endif