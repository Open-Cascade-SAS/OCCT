// Created on: 1992-11-27
// Created by: Isabelle GRIGNON
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRepGProp_Domain_HeaderFile
#define _BRepGProp_Domain_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopExp_Explorer.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Face;
class TopoDS_Edge;


//! Arc iterator. Returns only Forward and Reversed edges from
//! the face in an undigested order.
class BRepGProp_Domain 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
    BRepGProp_Domain();
  
  //! Constructor. Initializes the domain with the face.
    BRepGProp_Domain(const TopoDS_Face& F);
  
  //! Initializes the domain with the face.
    void Init (const TopoDS_Face& F);
  

  //! Returns True if there is another arc of curve in the list.
    Standard_Boolean More();
  
  //! Initializes the exploration with the face already set.
    void Init();
  
  //! Returns the current edge.
    const TopoDS_Edge& Value();
  

  //! Sets the index of the arc iterator to the next arc of
  //! curve.
  Standard_EXPORT void Next();




protected:





private:



  TopExp_Explorer myExplorer;


};


#include <BRepGProp_Domain.lxx>





#endif // _BRepGProp_Domain_HeaderFile
