// File:        BRepMesh_Status.hxx
// Created:     May 17 11:59:53 2011
// Author:      Oleg AGASHIN
// Copyright:   Open CASCADE SAS 2011

#ifndef _BRepMesh_Status_HeaderFile
#define _BRepMesh_Status_HeaderFile

//! Discribes the wires discretisation. <br>
enum BRepMesh_Status {
BRepMesh_NoError              = 0x0,
BRepMesh_OpenWire             = 0x1,
BRepMesh_SelfIntersectingWire = 0x2,
BRepMesh_Failure              = 0x4,
BRepMesh_ReMesh               = 0x8
};

#endif
