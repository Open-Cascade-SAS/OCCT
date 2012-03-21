// Created on: 2005-03-17
// Created by: OPEN CASCADE Support
// Copyright (c) 2005-2012 OPEN CASCADE SAS
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


#ifndef _Extrema_HUBTreeOfSphere_HeaderFile
#define _Extrema_HUBTreeOfSphere_HeaderFile

#include <NCollection_UBTree.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <NCollection_Handle.hxx>
#include <Bnd_Sphere.hxx>

typedef NCollection_UBTree<Standard_Integer,Bnd_Sphere> Extrema_UBTreeOfSphere;
typedef NCollection_UBTreeFiller<Standard_Integer,Bnd_Sphere> Extrema_UBTreeFillerOfSphere;
typedef NCollection_Handle<Extrema_UBTreeOfSphere> Extrema_HUBTreeOfSphere;

#endif //_Extrema_HUBTreeOfSphere_HeaderFile
