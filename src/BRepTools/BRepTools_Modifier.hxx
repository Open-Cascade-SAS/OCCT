// Created on: 1994-08-25
// Created by: Jacques GOUSSARD
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

#ifndef _BRepTools_Modifier_HeaderFile
#define _BRepTools_Modifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>

#include <Message_ProgressIndicator.hxx>

class Standard_NullObject;
class Standard_NoSuchObject;
class TopoDS_Shape;
class BRepTools_Modification;
class Message_ProgressIndicator;


//! Performs geometric modifications on a shape.
class BRepTools_Modifier 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty Modifier.
  Standard_EXPORT BRepTools_Modifier();
  
  //! Creates a modifier on the shape <S>.
  Standard_EXPORT BRepTools_Modifier(const TopoDS_Shape& S);
  
  //! Creates a modifier on  the shape <S>, and performs
  //! the modifications described by <M>.
  Standard_EXPORT BRepTools_Modifier(const TopoDS_Shape& S, const Handle(BRepTools_Modification)& M);
  
  //! Initializes the modifier with the shape <S>.
  Standard_EXPORT void Init (const TopoDS_Shape& S);
  
  //! Performs the modifications described by <M>.
  Standard_EXPORT void Perform (const Handle(BRepTools_Modification)& M, const Handle(Message_ProgressIndicator)& aProgress = NULL);
  
  //! Returns Standard_True if the modification has
  //! been computed successfully.
    Standard_Boolean IsDone() const;
  
  //! Returns the modified shape corresponding to <S>.
    const TopoDS_Shape& ModifiedShape (const TopoDS_Shape& S) const;




protected:





private:

  
  Standard_EXPORT void Put (const TopoDS_Shape& S);
  
  Standard_EXPORT Standard_Boolean Rebuild (const TopoDS_Shape& S, const Handle(BRepTools_Modification)& M, const Handle(Message_ProgressIndicator)& aProgress = NULL);


  TopTools_DataMapOfShapeShape myMap;
  TopoDS_Shape myShape;
  Standard_Boolean myDone;


};


#include <BRepTools_Modifier.lxx>





#endif // _BRepTools_Modifier_HeaderFile
