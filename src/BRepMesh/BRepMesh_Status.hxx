// Created on: 2011-05-17
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
