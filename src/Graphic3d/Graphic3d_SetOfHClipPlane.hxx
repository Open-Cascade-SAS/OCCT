// Created on: 2013-07-15
// Created by: Anton POLETAEV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 65 (the "License") You may not use the content of this file
// except in compliance with the License Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file
//
// The Initial Developer of the Original Code is Open CASCADE SAS, having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement Please see the License for the specific terms
// and conditions governing the rights and limitations under the License

#ifndef _Graphic3d_SetOfHClipPlane_HeaderFile
#define _Graphic3d_SetOfHClipPlane_HeaderFile

#include <NCollection_Set.hxx>
#include <Graphic3d_ClipPlane.hxx>

// CDL-header shortcut for set of graphical clipping planes. This is a compact
// way (compared to list) to store clippings, with mandatory uniqueness check.
typedef NCollection_Set<Handle(Graphic3d_ClipPlane)> Graphic3d_SetOfHClipPlane;

#endif
