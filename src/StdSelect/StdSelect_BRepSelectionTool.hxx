// Created on: 1995-03-08
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _StdSelect_BRepSelectionTool_HeaderFile
#define _StdSelect_BRepSelectionTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <SelectMgr_Selection.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Select3D_EntitySequence.hxx>
class TopoDS_Shape;
class SelectMgr_SelectableObject;
class StdSelect_BRepOwner;
class TopoDS_Face;


//! Tool to create specific selections (sets of primitives)
//! for Shapes from Topology.
//! These Selections may be used  in dynamic selection
//! Mechanism
//! Given a Shape and a mode of selection
//! (selection of vertices,
//! edges,faces ...) , This Tool Computes corresponding sensitive primitives,
//! puts them in an entity called Selection (see package SelectMgr) and returns it.
//!
//! A Priority for the decomposed pickable objects can be given ;
//! by default There is A Preset Hierachy:
//! Vertex             priority : 5
//! Edge               priority : 4
//! Wire               priority : 3
//! Face               priority : 2
//! Shell,solid,shape  priority : 1
//! the default priority in the following methods has no sense - it's only taken in account
//! when the user gives a value between 0 and 10.
//! IMPORTANT : This decomposition creates BRepEntityOwner instances (from StdSelect).
//! which are stored in the Sensitive Entities coming from The Decomposition.
//!
//! the result of picking in a ViewerSelector return EntityOwner from SelectMgr;
//! to know what kind of object was picked :
//!
//! ENTITY_OWNER -> Selectable() gives the selectableobject which
//! was decomposed into pickable elements.
//! Handle(StdSelect_BRepOwner)::DownCast(ENTITY_OWNER) -> Shape()
//! gives the real picked shape (edge,vertex,shape...)
class StdSelect_BRepSelectionTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Decomposition of <aShape> into sensitive entities following
  //! a mode of decomposition <aType>. These entities are stored in <aSelection>.
  //! BrepOwners are created to store the identity of the picked shapes
  //! during the selection process.
  //! In those BRepOwners is also stored the original shape.
  //! But One can't get the selectable object which was decomposed to give
  //! the sensitive entities.
  //! maximal parameter is used for infinite objects, to limit the sensitive Domain....
  //! If AutoTriangulation = True, a Triangulation will be
  //! computed for faces which have no existing one.
  //! if AutoTriangulation = False the old algorithm will be
  //! called to compute sensitive entities on faces.
  Standard_EXPORT static void Load (const Handle(SelectMgr_Selection)& aSelection, const TopoDS_Shape& aShape, const TopAbs_ShapeEnum aType, const Standard_Real theDeflection, const Standard_Real theDeviationAngle, const Standard_Boolean AutoTriangulation = Standard_True, const Standard_Integer aPriority = -1, const Standard_Integer NbPOnEdge = 9, const Standard_Real MaximalParameter = 500);
  
  //! Same  functionnalities  ; the  only
  //! difference is that  the selectable object from which the
  //! selection comes is stored in each Sensitive EntityOwner;
  //! decomposition of <aShape> into sensitive entities following
  //! a mode of decomposition <aType>. These entities are stored in <aSelection>
  //! The Major difference is that the known users are first inserted in the
  //! BRepOwners. the original shape is the last user...
  //! (see EntityOwner from SelectBasics and BrepOwner)...
  Standard_EXPORT static void Load (const Handle(SelectMgr_Selection)& aSelection, const Handle(SelectMgr_SelectableObject)& Origin, const TopoDS_Shape& aShape, const TopAbs_ShapeEnum aType, const Standard_Real theDeflection, const Standard_Real theDeviationAngle, const Standard_Boolean AutoTriangulation = Standard_True, const Standard_Integer aPriority = -1, const Standard_Integer NbPOnEdge = 9, const Standard_Real MaximalParameter = 500);
  
  //! Returns the standard priority of the shape aShap having the type aType.
  //! This priority is passed to a StdSelect_BRepOwner object.
  //! You can use the function Load to modify the
  //! selection priority of an owner to make one entity
  //! more selectable than another one.
  Standard_EXPORT static Standard_Integer GetStandardPriority (const TopoDS_Shape& aShap, const TopAbs_ShapeEnum aType);
  
  //! Computes the sensitive primitives corresponding to
  //! the subshape aShape with the owner anOwner.
  //! Stores them in the SelectMgr_Selection object, and returns this object.
  //! The number NbPOnEdge is used to define edges and wires.
  //! In the case of infinite objects, the value
  //! MaximalParameter serves to limit computation time
  //! by limiting the sensitive area. The default value is 500.
  //! If AutoTriangulation is true, triangulation will be
  //! computed for the faces which have none. If it is false,
  //! sensitive entities on these faces will be calculated.
  Standard_EXPORT static void ComputeSensitive (const TopoDS_Shape& aShape, const Handle(StdSelect_BRepOwner)& anOwner, const Handle(SelectMgr_Selection)& aSelection, const Standard_Real theDeflection, const Standard_Real theDeviationAngle, const Standard_Integer NbPOnEdge, const Standard_Real MaximalParameter, const Standard_Boolean AutoTriangulation = Standard_True);
  
  //! appends   to <OutList>   the  3D
  //! sensitive entities  created  for  selection of <aFace>
  //! if<InteriorFlag> =  False  the face  will  be sensitive only on its boundary
  Standard_EXPORT static Standard_Boolean GetSensitiveForFace (const TopoDS_Face& aFace, const Handle(StdSelect_BRepOwner)& anOwner, Select3D_EntitySequence& OutList, const Standard_Boolean AutoTriangulation = Standard_True, const Standard_Integer NbPOnEdge = 9, const Standard_Real MaxiParam = 500, const Standard_Boolean InteriorFlag = Standard_True);




protected:





private:

  
  //! Traverses the selection given and pre-builds BVH trees for heavyweight
  //! sensitive entities containing more than BVH_PRIMITIVE_LIMIT (defined in .cxx file)
  //! sub-elements
  Standard_EXPORT static void preBuildBVH (const Handle(SelectMgr_Selection)& theSelection);
  
  Standard_EXPORT static void GetEdgeSensitive (const TopoDS_Shape& aShape, const Handle(StdSelect_BRepOwner)& anOwner, const Handle(SelectMgr_Selection)& aSelection, const Standard_Real theDeflection, const Standard_Real theDeflectionAngle, const Standard_Integer NbPOnEdge, const Standard_Real MaximalParameter, Handle(Select3D_SensitiveEntity)& aSensitive);




};







#endif // _StdSelect_BRepSelectionTool_HeaderFile
