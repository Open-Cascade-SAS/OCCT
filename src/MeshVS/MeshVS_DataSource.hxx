// Created on: 2003-10-10
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

#ifndef _MeshVS_DataSource_HeaderFile
#define _MeshVS_DataSource_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <MeshVS_EntityType.hxx>
#include <MeshVS_HArray1OfSequenceOfInteger.hxx>
#include <Standard_Address.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <Standard_Real.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
class Bnd_Box;
class MeshVS_Mesh;
class TColStd_HPackedMapOfInteger;
class Bnd_Box2d;


class MeshVS_DataSource;
DEFINE_STANDARD_HANDLE(MeshVS_DataSource, Standard_Transient)

//! The deferred class using for the following tasks:
//! 1) Receiving geometry data about single element of node by its number;
//! 2) Receiving type of element or node by its number;
//! 3) Receiving topological information about links between element and nodes it consist of;
//! 4) Receiving information about what element cover this node;
//! 5) Receiving information about all nodes and elements the object consist of
//! 6) Activation of advanced mesh selection. In the advanced mesh selection mode there is created:
//! - one owner for the whole mesh and for all selection modes
//! - one sensitive entity for the whole mesh and for each selection mode
//! Receiving of IDs of detected entities (nodes and elements) in a viewer is achieved by
//! implementation of a group of methods GetDetectedEntities.
class MeshVS_DataSource : public Standard_Transient
{

public:

  
  //! Returns geometry information about node or element
  //! ID is the numerical identificator of node or element
  //! IsElement indicates this ID describe node ( if Standard_False ) or element ( if Standard_True )
  //! Coords is an array of coordinates of node(s).
  //! For node it is only 3 numbers: X, Y, Z in the strict order
  //! For element it is 3*n numbers, where n is number of this element vertices
  //! The order is strict also: X1, Y1, Z1, X2,...., where Xi, Yi, Zi are coordinates of vertices
  //! NbNodes is number of nodes. It is recommended this parameter to be set to 1 for node.
  //! Type is type of node or element (from enumeration). It is recommended this parameter to be set to
  //! MeshVS_ET_Node for node.
  Standard_EXPORT virtual Standard_Boolean GetGeom (const Standard_Integer ID, const Standard_Boolean IsElement, TColStd_Array1OfReal& Coords, Standard_Integer& NbNodes, MeshVS_EntityType& Type) const = 0;
  
  //! This method is similar to GetGeom, but returns only element or node type.
  Standard_EXPORT virtual Standard_Boolean GetGeomType (const Standard_Integer ID, const Standard_Boolean IsElement, MeshVS_EntityType& Type) const = 0;
  
  //! This method returns topology information about 3D-element
  //! Returns false if element with ID isn't 3D or because other troubles
  Standard_EXPORT virtual Standard_Boolean Get3DGeom (const Standard_Integer ID, Standard_Integer& NbNodes, Handle(MeshVS_HArray1OfSequenceOfInteger)& Data) const;
  
  //! This method returns pointer which represents element or node data structure.
  //! This address will be saved in MeshVS_MeshEntityOwner, so that you can access to data structure fast
  //! by the method Owner(). In the redefined method you can return NULL.
  //! ID is the numerical identificator of node or element
  //! IsElement indicates this ID describe node ( if Standard_False ) or element ( if Standard_True )
  Standard_EXPORT virtual Standard_Address GetAddr (const Standard_Integer ID, const Standard_Boolean IsElement) const = 0;
  
  //! This method returns information about nodes this element consist of.
  //! ID is the numerical identificator of element.
  //! NodeIDs is the output array of nodes IDs in correct order,
  //! the same as coordinates returned by GetGeom().
  //! NbNodes is number of nodes (number of items set in NodeIDs).
  //! Returns False if element does not exist
  Standard_EXPORT virtual Standard_Boolean GetNodesByElement (const Standard_Integer ID, TColStd_Array1OfInteger& NodeIDs, Standard_Integer& NbNodes) const = 0;
  
  //! This method returns map of all nodes the object consist of.
  Standard_EXPORT virtual const TColStd_PackedMapOfInteger& GetAllNodes() const = 0;
  
  //! This method returns map of all elements the object consist of.
  Standard_EXPORT virtual const TColStd_PackedMapOfInteger& GetAllElements() const = 0;
  
  //! This method calculates normal of face, which is using for correct reflection presentation.
  //! There is default method, for advance reflection this method can be redefined.
  //! Id is the numerical identificator of only element!
  //! Max is maximal number of nodes an element can consist of
  //! nx, ny, nz  are values whose represent coordinates of normal (will be returned)
  //! In the redefined method you can return normal with length more then 1, but in this case
  //! the appearance of element will be more bright than usual. For ordinary brightness you must return
  //! normal with length 1
  Standard_EXPORT virtual Standard_Boolean GetNormal (const Standard_Integer Id, const Standard_Integer Max, Standard_Real& nx, Standard_Real& ny, Standard_Real& nz) const;
  
  //! This method return normal of node ranknode of face Id,
  //! which is using for smooth shading presentation.
  //! Returns false if normal isn't defined.
  Standard_EXPORT virtual Standard_Boolean GetNodeNormal (const Standard_Integer ranknode, const Standard_Integer ElementId, Standard_Real& nx, Standard_Real& ny, Standard_Real& nz) const;
  
  //! This method puts components of normal vectors at each node of a mesh face (at each face of a mesh volume)
  //! into the output array.
  //! Returns false if some problem was detected during calculation of normals.
  //! Id is an identifier of the mesh element.
  //! IsNodal, when true, means that normals at mesh element nodes are needed. If nodal normals
  //! are not available, or IsNodal is false, or the mesh element is a volume, then the output array contents
  //! depend on the element type:
  //! face: a normal calculated by GetNormal() is duplicated for each node of the face;
  //! volume: normals to all faces of the volume are computed (not for each node!).
  //! MaxNodes is maximal number of nodes an element can consist of.
  //! Normals contains the result.
  Standard_EXPORT virtual Standard_Boolean GetNormalsByElement (const Standard_Integer Id, const Standard_Boolean IsNodal, const Standard_Integer MaxNodes, Handle(TColStd_HArray1OfReal)& Normals) const;
  
  //! This method returns map of all groups the object contains.
  Standard_EXPORT virtual void GetAllGroups (TColStd_PackedMapOfInteger& Ids) const;
  
  //! This method returns map of all group elements.
  Standard_EXPORT virtual Standard_Boolean GetGroup (const Standard_Integer Id, MeshVS_EntityType& Type, TColStd_PackedMapOfInteger& Ids) const;
  
  //! This method returns pointer which represents group data structure.
  //! This address will be saved in MeshVS_MeshOwner, so that you can access to data structure fast
  //! by the method Owner(). In the redefined method you can return NULL.
  //! ID is the numerical identificator of group
  Standard_EXPORT virtual Standard_Address GetGroupAddr (const Standard_Integer ID) const;
  
  //! Returns True if advanced mesh selection is enabled.
  //! Default implementation returns False.
  //! It should be redefined to return True for advanced
  //! mesh selection activation.
  Standard_EXPORT virtual Standard_Boolean IsAdvancedSelectionEnabled() const;
  
  //! Returns the bounding box of the whole mesh.
  //! It is used in advanced selection mode to define roughly
  //! the sensitive area of the mesh.
  //! It can be redefined to get access to a box computed in advance.
  Standard_EXPORT virtual Bnd_Box GetBoundingBox() const;
  
  //! Returns maps of entities (nodes and elements) detected
  //! by mouse click at the point (X,Y) on the current view plane,
  //! with the tolerance aTol.
  //! DMin - is out argument should return actual detection tolerance.
  //! Returns True if something is detected.
  //! It should be redefined if the advanced mesh selection is
  //! activated. Default implementation returns False.
  Standard_EXPORT virtual Standard_Boolean GetDetectedEntities (const Handle(MeshVS_Mesh)& Prs, const Standard_Real X, const Standard_Real Y, const Standard_Real aTol, Handle(TColStd_HPackedMapOfInteger)& Nodes, Handle(TColStd_HPackedMapOfInteger)& Elements, Standard_Real& DMin);
  
  //! Returns maps of entities (nodes and elements) detected
  //! by mouse selection with rectangular box (XMin, YMin, XMax, YMax)
  //! on the current view plane, with the tolerance aTol.
  //! Returns True if something is detected.
  //! It should be redefined if the advanced mesh selection is
  //! activated. Default implementation returns False.
  Standard_EXPORT virtual Standard_Boolean GetDetectedEntities (const Handle(MeshVS_Mesh)& Prs, const Standard_Real XMin, const Standard_Real YMin, const Standard_Real XMax, const Standard_Real YMax, const Standard_Real aTol, Handle(TColStd_HPackedMapOfInteger)& Nodes, Handle(TColStd_HPackedMapOfInteger)& Elements);
  
  //! Returns maps of entities (nodes and elements) detected
  //! by mouse selection with the polyline <Polyline>
  //! on the current view plane, with the tolerance aTol.
  //! Returns True if something is detected.
  //! It should be redefined if the advanced mesh selection is
  //! activated. Default implementation returns False.
  Standard_EXPORT virtual Standard_Boolean GetDetectedEntities (const Handle(MeshVS_Mesh)& Prs, const TColgp_Array1OfPnt2d& Polyline, const Bnd_Box2d& aBox, const Standard_Real aTol, Handle(TColStd_HPackedMapOfInteger)& Nodes, Handle(TColStd_HPackedMapOfInteger)& Elements);
  
  //! Filter out the maps of mesh entities so as to keep
  //! only the entities that are allowed to be selected
  //! according to the current context.
  //! Returns True if any of the maps has been changed.
  //! It should be redefined if the advanced mesh selection is
  //! activated. Default implementation returns False.
  Standard_EXPORT virtual Standard_Boolean GetDetectedEntities (const Handle(MeshVS_Mesh)& Prs, Handle(TColStd_HPackedMapOfInteger)& Nodes, Handle(TColStd_HPackedMapOfInteger)& Elements);

  DEFINE_STANDARD_RTTIEXT(MeshVS_DataSource,Standard_Transient)

};

#endif // _MeshVS_DataSource_HeaderFile
