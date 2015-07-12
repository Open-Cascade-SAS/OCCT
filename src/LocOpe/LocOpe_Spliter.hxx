// Created on: 1996-01-12
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

#ifndef _LocOpe_Spliter_HeaderFile
#define _LocOpe_Spliter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
class StdFail_NotDone;
class Standard_NullObject;
class Standard_NoSuchObject;
class TopoDS_Shape;
class LocOpe_WiresOnShape;



class LocOpe_Spliter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
    LocOpe_Spliter();
  
  //! Creates the algorithm on the shape <S>.
    LocOpe_Spliter(const TopoDS_Shape& S);
  
  //! Initializes the algorithm on the shape <S>.
    void Init (const TopoDS_Shape& S);
  
  Standard_EXPORT void Perform (const Handle(LocOpe_WiresOnShape)& PW);
  
    Standard_Boolean IsDone() const;
  
  //! Returns the new shape
    const TopoDS_Shape& ResultingShape() const;
  
  //! Returns the initial shape
    const TopoDS_Shape& Shape() const;
  
  //! Returns  the faces   which  are the  left of   the
  //! projected wires and which are
  Standard_EXPORT const TopTools_ListOfShape& DirectLeft() const;
  
  //! Returns the faces of the "left" part on the shape.
  //! (It  is build   from  DirectLeft,  with  the faces
  //! connected to this set, and so on...).
  Standard_EXPORT const TopTools_ListOfShape& Left() const;
  
  //! Returns the list of descendant shapes of <S>.
  Standard_EXPORT const TopTools_ListOfShape& DescendantShapes (const TopoDS_Shape& S);




protected:





private:



  TopoDS_Shape myShape;
  Standard_Boolean myDone;
  TopoDS_Shape myRes;
  TopTools_DataMapOfShapeListOfShape myMap;
  TopTools_ListOfShape myDLeft;
  TopTools_ListOfShape myLeft;


};


#include <LocOpe_Spliter.lxx>





#endif // _LocOpe_Spliter_HeaderFile
