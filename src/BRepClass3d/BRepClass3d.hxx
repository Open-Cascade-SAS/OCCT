// Created on: 1994-04-18
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepClass3d_HeaderFile
#define _BRepClass3d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class TopoDS_Shell;
class TopoDS_Solid;
class BRepClass3d_Intersector3d;
class BRepClass3d_SolidExplorer;
class BRepClass3d_SolidPassiveClassifier;
class BRepClass3d_SClassifier;
class BRepClass3d_SolidClassifier;



class BRepClass3d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the outer most shell of <S>. Returns a Null
  //! shell if <S> has no outer shell.
  //! If <S> has only one shell, then it will return, without checking orientation.
  Standard_EXPORT static TopoDS_Shell OuterShell (const TopoDS_Solid& S);




protected:





private:




friend class BRepClass3d_Intersector3d;
friend class BRepClass3d_SolidExplorer;
friend class BRepClass3d_SolidPassiveClassifier;
friend class BRepClass3d_SClassifier;
friend class BRepClass3d_SolidClassifier;

};







#endif // _BRepClass3d_HeaderFile
