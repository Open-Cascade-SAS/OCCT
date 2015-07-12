// Created on: 1997-10-02
// Created by: Xuan Trang PHAM PHU
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TopOpeBRepBuild_ShellToSolid_HeaderFile
#define _TopOpeBRepBuild_ShellToSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_ListOfShape.hxx>
class TopoDS_Shell;
class TopoDS_Solid;



//! This class builds solids from a set of shells SSh and a solid F.
class TopOpeBRepBuild_ShellToSolid 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_ShellToSolid();
  
  Standard_EXPORT void Init();
  
  Standard_EXPORT void AddShell (const TopoDS_Shell& Sh);
  
  Standard_EXPORT void MakeSolids (const TopoDS_Solid& So, TopTools_ListOfShape& LSo);




protected:





private:



  TopTools_ListOfShape myLSh;


};







#endif // _TopOpeBRepBuild_ShellToSolid_HeaderFile
