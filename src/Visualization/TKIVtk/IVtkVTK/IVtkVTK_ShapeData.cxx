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

#include <IVtkVTK_ShapeData.hxx>

// prevent disabling some MSVC warning messages by VTK headers
#include <Standard_WarningsDisable.hxx>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkIdList.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <Standard_WarningsRestore.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IVtkVTK_ShapeData, IVtk_IShapeData)

//=================================================================================================

IVtkVTK_ShapeData::IVtkVTK_ShapeData()
{
  myPolyData = vtkSmartPointer<vtkPolyData>::New();
  myPolyData->Allocate();
  myPolyData->SetPoints(vtkSmartPointer<vtkPoints>::New());

  myNormals = vtkSmartPointer<vtkFloatArray>::New();
  myNormals->SetName("Normals");
  myNormals->SetNumberOfComponents(3);
  myPolyData->GetPointData()->SetNormals(myNormals);

  mySubShapeIDs = vtkSmartPointer<vtkIdTypeArray>::New();
  mySubShapeIDs->SetName(IVtkVTK_ShapeData::ARRNAME_SUBSHAPE_IDS());
  mySubShapeIDs->SetNumberOfComponents(1);
  myPolyData->GetCellData()->AddArray(mySubShapeIDs);

  myMeshTypes = vtkSmartPointer<vtkIdTypeArray>::New();
  myMeshTypes->SetName(IVtkVTK_ShapeData::ARRNAME_MESH_TYPES());
  myMeshTypes->SetNumberOfComponents(1);
  myPolyData->GetCellData()->AddArray(myMeshTypes);
}

//=================================================================================================

IVtkVTK_ShapeData::~IVtkVTK_ShapeData() {}

//=================================================================================================

IVtk_PointId IVtkVTK_ShapeData::InsertPoint(const gp_Pnt&                  thePnt,
                                            const NCollection_Vec3<float>& theNorm)
{
  IVtk_PointId aPointId =
    myPolyData->GetPoints()->InsertNextPoint(thePnt.X(), thePnt.Y(), thePnt.Z());
  if (myNormals.GetPointer() != NULL)
  {
    myNormals->InsertNextTuple(theNorm.GetData());
  }
  return aPointId;
}

//=================================================================================================

void IVtkVTK_ShapeData::InsertVertex(const IVtk_IdType   theShapeID,
                                     const IVtk_PointId  thePointId,
                                     const IVtk_MeshType theMeshType)
{
  vtkIdType aPointIdVTK = thePointId;
  myPolyData->InsertNextCell(VTK_VERTEX, 1, &aPointIdVTK);
  insertNextSubShapeId(theShapeID, theMeshType);
}

//=================================================================================================

void IVtkVTK_ShapeData::InsertLine(const IVtk_IdType   theShapeID,
                                   const IVtk_PointId  thePointId1,
                                   const IVtk_PointId  thePointId2,
                                   const IVtk_MeshType theMeshType)
{
  vtkIdType aPoints[2] = {thePointId1, thePointId2};
  myPolyData->InsertNextCell(VTK_LINE, 2, aPoints);
  insertNextSubShapeId(theShapeID, theMeshType);
}

//=================================================================================================

void IVtkVTK_ShapeData::InsertLine(const IVtk_IdType       theShapeID,
                                   const IVtk_PointIdList* thePointIds,
                                   const IVtk_MeshType     theMeshType)
{
  if (!thePointIds->IsEmpty())
  {
    vtkSmartPointer<vtkIdList> anIdList = vtkSmartPointer<vtkIdList>::New();
    // Fill the vtk id list by ids from IVtk_PointIdList.
    IVtk_PointIdList::Iterator anIterOfIds = IVtk_PointIdList::Iterator(*thePointIds);
    anIdList->Allocate(thePointIds->Extent());
    for (; anIterOfIds.More(); anIterOfIds.Next())
    {
      anIdList->InsertNextId(anIterOfIds.Value());
    }

    myPolyData->InsertNextCell(VTK_POLY_LINE, anIdList);
    insertNextSubShapeId(theShapeID, theMeshType);
  }
}

//=================================================================================================

void IVtkVTK_ShapeData::InsertTriangle(const IVtk_IdType   theShapeID,
                                       const IVtk_PointId  thePointId1,
                                       const IVtk_PointId  thePointId2,
                                       const IVtk_PointId  thePointId3,
                                       const IVtk_MeshType theMeshType)
{
  vtkIdType aPoints[3] = {thePointId1, thePointId2, thePointId3};
  myPolyData->InsertNextCell(VTK_TRIANGLE, 3, aPoints);
  insertNextSubShapeId(theShapeID, theMeshType);
}
