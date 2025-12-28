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
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Select3D_EntitySequence.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
class SelectMgr_SelectableObject;
class TopoDS_Face;

//! Tool to create specific selections (sets of primitives)
//! for Shapes from Topology.
//! These Selections may be used in dynamic selection
//! Mechanism
//! Given a Shape and a mode of selection
//! (selection of vertices,
//! edges,faces ...) , This Tool Computes corresponding sensitive primitives,
//! puts them in an entity called Selection (see package SelectMgr) and returns it.
//!
//! A Priority for the decomposed pickable objects can be given ;
//! by default There is A Preset Hierarchy:
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
//! occ::down_cast<StdSelect_BRepOwner>(ENTITY_OWNER) -> Shape()
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
  Standard_EXPORT static void Load(const occ::handle<SelectMgr_Selection>& aSelection,
                                   const TopoDS_Shape&                     aShape,
                                   const TopAbs_ShapeEnum                  aType,
                                   const double                            theDeflection,
                                   const double                            theDeviationAngle,
                                   const bool                              AutoTriangulation = true,
                                   const int                               aPriority         = -1,
                                   const int                               NbPOnEdge         = 9,
                                   const double                            MaximalParameter  = 500);

  //! Same functionalities. The only
  //! difference is that the selectable object from which the
  //! selection comes is stored in each Sensitive EntityOwner;
  //! decomposition of <aShape> into sensitive entities following
  //! a mode of decomposition <aType>. These entities are stored in <aSelection>
  //! The Major difference is that the known users are first inserted in the
  //! BRepOwners. the original shape is the last user...
  //! (see EntityOwner from SelectBasics and BrepOwner)...
  Standard_EXPORT static void Load(const occ::handle<SelectMgr_Selection>&        aSelection,
                                   const occ::handle<SelectMgr_SelectableObject>& Origin,
                                   const TopoDS_Shape&                            aShape,
                                   const TopAbs_ShapeEnum                         aType,
                                   const double                                   theDeflection,
                                   const double                                   theDeviationAngle,
                                   const bool   AutoTriangulation = true,
                                   const int    aPriority         = -1,
                                   const int    NbPOnEdge         = 9,
                                   const double MaximalParameter  = 500);

  //! Returns the standard priority of the shape aShap having the type aType.
  //! This priority is passed to a StdSelect_BRepOwner object.
  //! You can use the function Load to modify the
  //! selection priority of an owner to make one entity
  //! more selectable than another one.
  static int GetStandardPriority(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
  {
    switch (theType)
    {
      case TopAbs_VERTEX:
        return 8;
      case TopAbs_EDGE:
        return 7;
      case TopAbs_WIRE:
        return 6;
      case TopAbs_FACE:
        return 5;
      case TopAbs_SHAPE:
      default:
        break;
    }

    switch (theShape.ShapeType())
    {
      case TopAbs_VERTEX:
        return 9;
      case TopAbs_EDGE:
        return 8;
      case TopAbs_WIRE:
        return 7;
      case TopAbs_FACE:
        return 6;
      case TopAbs_SHELL:
        return 5;
      case TopAbs_COMPOUND:
      case TopAbs_COMPSOLID:
      case TopAbs_SOLID:
      case TopAbs_SHAPE:
      default:
        return 4;
    }
  }

  //! Computes the sensitive primitives, stores them in the SelectMgr_Selection object, and returns
  //! this object.
  //! @param[in] theShape        shape to compute sensitive entities
  //! @param[in] theOwner        selectable owner object
  //! @param[in] theSelection    selection to append new sensitive entities
  //! @param[in] theDeflection   linear deflection
  //! @param[in] theDeflAngle    angular deflection
  //! @param[in] theNbPOnEdge    sensitivity parameters for edges and wires
  //! @param[in] theMaxiParam    sensitivity parameters for infinite objects (the default value is
  //! 500)
  //! @param[in] theAutoTriang   flag to compute triangulation for the faces which have none
  Standard_EXPORT static void ComputeSensitive(const TopoDS_Shape&                       theShape,
                                               const occ::handle<SelectMgr_EntityOwner>& theOwner,
                                               const occ::handle<SelectMgr_Selection>& theSelection,
                                               const double theDeflection,
                                               const double theDeflAngle,
                                               const int    theNbPOnEdge,
                                               const double theMaxiParam,
                                               const bool   theAutoTriang = true);

  //! Creates the 3D sensitive entities for Face selection.
  //! @param[in]  theFace         face to compute sensitive entities
  //! @param[in]  theOwner        selectable owner object
  //! @param[out] theOutList     output result list to append created entities
  //! @param[in]  theAutoTriang   obsolete flag (has no effect)
  //! @param[in]  theNbPOnEdge    sensitivity parameters
  //! @param[in]  theMaxiParam    sensitivity parameters
  //! @param[in]  theInteriorFlag flag indicating that face interior (TRUE) or face boundary (FALSE)
  //! should be selectable
  Standard_EXPORT static bool GetSensitiveForFace(
    const TopoDS_Face&                                           theFace,
    const occ::handle<SelectMgr_EntityOwner>&                    theOwner,
    NCollection_Sequence<occ::handle<Select3D_SensitiveEntity>>& theOutList,
    const bool                                                   theAutoTriang   = true,
    const int                                                    theNbPOnEdge    = 9,
    const double                                                 theMaxiParam    = 500,
    const bool                                                   theInteriorFlag = true);

  //! Creates a sensitive cylinder.
  //! @param[in] theSubfacesMap map of cylinder faces
  //! @param[in] theOwner       selectable owner object
  //! @param[in] theSelection   selection to append new sensitive entities
  Standard_EXPORT static bool GetSensitiveForCylinder(
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theSubfacesMap,
    const occ::handle<SelectMgr_EntityOwner>&                            theOwner,
    const occ::handle<SelectMgr_Selection>&                              theSelection);

  //! Create a sensitive edge or sensitive wire.
  //! @param[in]  theShape          either TopoDS_Edge or TopoDS_Wire to compute sensitive entities
  //! @param[in]  theOwner          selectable owner object
  //! @param[in]  theSelection      selection to append new sensitive entities
  //! @param[in]  theDeflection     linear deflection
  //! @param[in]  theDeviationAngle angular deflection
  //! @param[in]  theNbPOnEdge      sensitivity parameters
  //! @param[out] theMaxiParam      sensitivity parameters
  Standard_EXPORT static void GetEdgeSensitive(const TopoDS_Shape&                       theShape,
                                               const occ::handle<SelectMgr_EntityOwner>& theOwner,
                                               const occ::handle<SelectMgr_Selection>& theSelection,
                                               const double theDeflection,
                                               const double theDeviationAngle,
                                               const int    theNbPOnEdge,
                                               const double theMaxiParam,
                                               occ::handle<Select3D_SensitiveEntity>& theSensitive);

  //! Traverses the selection given and pre-builds BVH trees for heavyweight
  //! sensitive entities containing more than BVH_PRIMITIVE_LIMIT (defined in .cxx file)
  //! sub-elements.
  Standard_EXPORT static void PreBuildBVH(const occ::handle<SelectMgr_Selection>& theSelection);
};

#endif // _StdSelect_BRepSelectionTool_HeaderFile
