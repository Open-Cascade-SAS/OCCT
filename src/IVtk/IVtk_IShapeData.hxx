// Created on: 2011-10-12 
// Created by: Roman KOZLOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS 
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

#ifndef __IVTK_ISHAPEDATA_H__
#define __IVTK_ISHAPEDATA_H__

#include <gp_Pnt.hxx>
#include <IVtk_Interface.hxx>
#include <IVtk_Types.hxx>
#include <NCollection_Vec3.hxx>

DEFINE_STANDARD_HANDLE( IVtk_IShapeData, IVtk_Interface )

//! @class IVtk_IShapeData
//! @brief Interface for working with triangulated data.
class IVtk_IShapeData : public IVtk_Interface
{
public:
  typedef Handle(IVtk_IShapeData) Handle;

  virtual ~IVtk_IShapeData() { }

  DEFINE_STANDARD_RTTIEXT(IVtk_IShapeData,IVtk_Interface)

  //! Insert a coordinate
  //! @param [in] thePnt  point position
  //! @param [in] theNorm point normal
  //! @return id of added point
  virtual IVtk_PointId InsertPoint (const gp_Pnt& thePnt,
                                    const NCollection_Vec3<float>& theNorm) = 0;

  //! Insert a vertex.
  //! @param [in] theShapeID id of the sub-shape to which the vertex belongs.
  //! @param [in] thePointId id of the point that defines the coordinates of the vertex
  //! @param [in] theMeshType mesh type of the sub-shape (MT_Undefined by default)
  virtual void InsertVertex (const IVtk_IdType   theShapeID,
                             const IVtk_PointId  thePointId,
                             const IVtk_MeshType theMeshType = MT_Undefined) = 0;

  //! Insert a line.
  //! @param [in] theShapeID id of the subshape to which the line belongs.
  //! @param [in] thePointId1 id of the first point
  //! @param [in] thePointId2 id of the second point
  //! @param [in] theMeshType mesh type of the subshape (MT_Undefined by default)
  virtual void InsertLine (const IVtk_IdType   theShapeID,
                           const IVtk_PointId  thePointId1,
                           const IVtk_PointId  thePointId2,
                           const IVtk_MeshType theMeshType = MT_Undefined) = 0;

  //! Insert a poly-line.
  //! @param [in] shapeID id of the subshape to which the polyline belongs.
  //! @param [in] pointIds vector of point ids
  //! @param [in] meshType mesh type of the subshape (MT_Undefined by default)
  virtual void InsertLine (const IVtk_IdType       theShapeID,
                           const IVtk_PointIdList* thePointIds,
                           const IVtk_MeshType     theMeshType = MT_Undefined) = 0;

  //! Insert a triangle
  //! @param [in] theShapeID id of the subshape to which the triangle belongs.
  //! @param [in] thePointId1 id of the first point
  //! @param [in] thePointId2 id of the second point
  //! @param [in] thePointId3 id of the third point
  //! @param [in] theMeshType mesh type of the subshape (MT_Undefined by default)
  virtual void InsertTriangle (const IVtk_IdType   theShapeID,
                               const IVtk_PointId  thePointId1,
                               const IVtk_PointId  thePointId2,
                               const IVtk_PointId  thePointId3,
                               const IVtk_MeshType theMeshType = MT_Undefined) = 0;

public:

  //! Insert a coordinate
  //! @param [in] theX X coordinate
  //! @param [in] theY Y coordinate
  //! @param [in] theZ Z coordinate
  //! @return id of added point
  virtual IVtk_PointId InsertCoordinate (double theX, double theY, double theZ)
  {
    return InsertCoordinate (gp_Pnt (theX, theY, theZ));
  }

  //! Insert a coordinate
  //! @param [in] thePnt point position
  //! @return id of added point
  IVtk_PointId InsertCoordinate (const gp_Pnt& thePnt)
  {
    return InsertPoint (thePnt, NCollection_Vec3<float>(0.0f, 0.0f, 1.0f));
  }

};

#endif // __IVTK_ISHAPEDATA_H__
