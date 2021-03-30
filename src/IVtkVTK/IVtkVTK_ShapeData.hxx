// Created on: 2011-10-14 
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

#ifndef __IVTKVTK_SHAPEDATA_H__
#define __IVTKVTK_SHAPEDATA_H__

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <IVtk_IShapeData.hxx>

// prevent disabling some MSVC warning messages by VTK headers 
#include <Standard_WarningsDisable.hxx>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkIdTypeArray.h>
#include <Standard_WarningsRestore.hxx>

class vtkIdTypeArray;
class vtkFloatArray;

class IVtkVTK_ShapeData;
DEFINE_STANDARD_HANDLE( IVtkVTK_ShapeData, IVtk_IShapeData )

//! @class IVtkVTK_ShapeData 
//! @brief IShapeData implementation for VTK.
//!
//! Contains the shape geometry information as vtkPolyData.
class IVtkVTK_ShapeData : public IVtk_IShapeData
{
public:

  static const char* ARRNAME_SUBSHAPE_IDS() { return "SUBSHAPE_IDS"; }
  static const char* ARRNAME_MESH_TYPES() { return "MESH_TYPES"; }

  typedef Handle(IVtkVTK_ShapeData) Handle;

  //! Constructor
  Standard_EXPORT IVtkVTK_ShapeData();
  //! Destructor
  Standard_EXPORT ~IVtkVTK_ShapeData();

  DEFINE_STANDARD_RTTIEXT(IVtkVTK_ShapeData,IVtk_IShapeData)

  //! Insert a coordinate
  //! @param [in] thePnt  point position
  //! @param [in] theNorm point normal
  //! @return id of added point
  Standard_EXPORT virtual IVtk_PointId InsertPoint (const gp_Pnt& thePnt,
                                                    const NCollection_Vec3<float>& theNorm) Standard_OVERRIDE;

  //! Insert a vertex.
  //! @param [in] theShapeID id of the subshape to which the vertex belongs.
  //! @param [in] thePointId id of the point that defines the coordinates of the vertex
  //! @param [in] theMeshType mesh type of the subshape (MT_Undefined by default)
  Standard_EXPORT virtual void InsertVertex (const IVtk_IdType   theShapeID,
                                             const IVtk_PointId  thePointId,
                                             const IVtk_MeshType theMeshType) Standard_OVERRIDE;

  //! Insert a line.
  //! @param [in] theShapeID id of the subshape to which the line belongs.
  //! @param [in] thePointId1 id of the first point
  //! @param [in] thePointId2 id of the second point
  //! @param [in] theMeshType mesh type of the subshape (MT_Undefined by default)
  Standard_EXPORT virtual void InsertLine (const IVtk_IdType   theShapeID,
                                           const IVtk_PointId  thePointId1,
                                           const IVtk_PointId  thePointId2,
                                           const IVtk_MeshType theMeshType) Standard_OVERRIDE;

  //! Insert a poly-line.
  //! @param [in] theShapeID id of the subshape to which the polyline belongs.
  //! @param [in] thePointIds vector of point ids
  //! @param [in] theMeshType mesh type of the subshape (MT_Undefined by default)
  Standard_EXPORT virtual void InsertLine (const IVtk_IdType       theShapeID, 
                                           const IVtk_PointIdList* thePointIds,
                                           const IVtk_MeshType     theMeshType) Standard_OVERRIDE;
  //! Insert a triangle
  //! @param [in] theShapeID id of the subshape to which the triangle belongs.
  //! @param [in] thePointId1 id of the first point
  //! @param [in] thePointId2 id of the second point
  //! @param [in] thePointId3 id of the third point
  //! @param [in] theMeshType mesh type of the subshape (MT_Undefined by default)
  Standard_EXPORT virtual void InsertTriangle (const IVtk_IdType   theShapeID,
                                               const IVtk_PointId  thePointId1,
                                               const IVtk_PointId  thePointId2,
                                               const IVtk_PointId  thePointId3,
                                               const IVtk_MeshType theMeshType) Standard_OVERRIDE;


public: //! @name Specific methods

  //! Get VTK PolyData.
  //! @return VTK PolyData
  vtkPolyData* getVtkPolyData() const
  { return myPolyData; }

private:

  //! Wrapper over vtkGenericDataArray::InsertNextTypedTuple().
  void insertNextSubShapeId (IVtk_IdType theShapeID,
                             IVtk_MeshType theMeshType)
  {
    const vtkIdType aShapeIDVTK = theShapeID;
    const vtkIdType aType = theMeshType;
  #if (VTK_MAJOR_VERSION > 7) || (VTK_MAJOR_VERSION == 7 && VTK_MINOR_VERSION >= 1)
    mySubShapeIDs->InsertNextTypedTuple (&aShapeIDVTK);
    myMeshTypes->InsertNextTypedTuple (&aType);
  #else
    mySubShapeIDs->InsertNextTupleValue (&aShapeIDVTK);
    myMeshTypes->InsertNextTupleValue (&aType);
  #endif
  }

private:
  vtkSmartPointer< vtkPolyData >    myPolyData;    //!< Shape geometry as vtkPolyData
  vtkSmartPointer< vtkFloatArray >  myNormals;     //!< vertex normals
  vtkSmartPointer< vtkIdTypeArray > mySubShapeIDs; //!< Array of sub-shapes ids
  vtkSmartPointer< vtkIdTypeArray > myMeshTypes;   //!< Array of type codes of mesh parts
};

#endif // __IVTKVTK_SHAPEDATA_H__
