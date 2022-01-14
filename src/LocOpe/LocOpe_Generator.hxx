// Created on: 1996-01-09
// Created by: Jacques GOUSSARD
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

#ifndef _LocOpe_Generator_HeaderFile
#define _LocOpe_Generator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
class LocOpe_GeneratedShape;
class TopoDS_Face;



class LocOpe_Generator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
    LocOpe_Generator();
  
  //! Creates the algorithm on the shape <S>.
    LocOpe_Generator(const TopoDS_Shape& S);
  
  //! Initializes the algorithm on the shape <S>.
    void Init (const TopoDS_Shape& S);
  
  Standard_EXPORT void Perform (const Handle(LocOpe_GeneratedShape)& G);
  
    Standard_Boolean IsDone() const;
  
  //! Returns the new shape
    const TopoDS_Shape& ResultingShape() const;
  
  //! Returns the initial shape
    const TopoDS_Shape& Shape() const;
  
  //! Returns  the  descendant  face  of <F>.    <F> may
  //! belong to the original shape or to the "generated"
  //! shape.  The returned    face may be   a null shape
  //! (when <F> disappears).
  Standard_EXPORT const TopTools_ListOfShape& DescendantFace (const TopoDS_Face& F);




protected:





private:



  TopoDS_Shape myShape;
  Handle(LocOpe_GeneratedShape) myGen;
  Standard_Boolean myDone;
  TopoDS_Shape myRes;
  TopTools_DataMapOfShapeListOfShape myModShapes;


};


#include <LocOpe_Generator.lxx>





#endif // _LocOpe_Generator_HeaderFile
