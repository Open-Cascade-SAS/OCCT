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

#include <BRepMeshData_Model.hxx>

#include <BRepMeshData_Face.hxx>
#include <BRepMeshData_Edge.hxx>
#include <NCollection_IncAllocator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMeshData_Model, IMeshData_Model)

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMeshData_Model::BRepMeshData_Model (const TopoDS_Shape& theShape)
  : IMeshData_Model (theShape),
    myMaxSize (0.),
    myAllocator (new NCollection_IncAllocator (IMeshData::MEMORY_BLOCK_SIZE_HUGE)),
    myDFaces (256, myAllocator),
    myDEdges (256, myAllocator)
{
  myAllocator->SetThreadSafe(true);
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMeshData_Model::~BRepMeshData_Model ()
{
}

//=======================================================================
// Function: FacesNb
// Purpose : 
//=======================================================================
Standard_Integer BRepMeshData_Model::FacesNb () const
{
  return myDFaces.Size ();
}

//=======================================================================
// Function: AddFace
// Purpose : 
//=======================================================================
const IMeshData::IFaceHandle& BRepMeshData_Model::AddFace (const TopoDS_Face& theFace)
{
  IMeshData::IFaceHandle aFace (new (myAllocator) BRepMeshData_Face (theFace, myAllocator));
  return myDFaces.Append (aFace);
}

//=======================================================================
// Function: GetFace
// Purpose : 
//=======================================================================
const IMeshData::IFaceHandle& BRepMeshData_Model::GetFace (const Standard_Integer theIndex) const
{
  return myDFaces (theIndex);
}

//=======================================================================
// Function: EdgesNb
// Purpose : 
//=======================================================================
Standard_Integer BRepMeshData_Model::EdgesNb () const
{
  return myDEdges.Size ();
}

//=======================================================================
// Function: AddEdge
// Purpose : 
//=======================================================================
const IMeshData::IEdgeHandle& BRepMeshData_Model::AddEdge (const TopoDS_Edge& theEdge)
{
  IMeshData::IEdgeHandle aEdge (new (myAllocator) BRepMeshData_Edge (theEdge, myAllocator));
  return myDEdges.Append (aEdge);
}

//=======================================================================
// Function: GetEdge
// Purpose : 
//=======================================================================
const IMeshData::IEdgeHandle& BRepMeshData_Model::GetEdge (const Standard_Integer theIndex) const
{
  return myDEdges (theIndex);
}
