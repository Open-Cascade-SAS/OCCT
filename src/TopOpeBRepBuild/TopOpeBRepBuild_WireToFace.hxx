// Created on: 1996-01-29
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _TopOpeBRepBuild_WireToFace_HeaderFile
#define _TopOpeBRepBuild_WireToFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopTools_ListOfShape.hxx>
class TopoDS_Wire;
class TopoDS_Face;



//! This class builds faces from a set of wires  SW and a face F.
//! The face must have and underlying surface, say S.
//! All of the edges of all of the wires must have a 2d representation
//! on surface S (except if S is planar)
class TopOpeBRepBuild_WireToFace 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_WireToFace();
  
  Standard_EXPORT void Init();
  
  Standard_EXPORT void AddWire (const TopoDS_Wire& W);
  
  Standard_EXPORT void MakeFaces (const TopoDS_Face& F, TopTools_ListOfShape& LF);




protected:





private:



  TopTools_ListOfShape myLW;


};







#endif // _TopOpeBRepBuild_WireToFace_HeaderFile
