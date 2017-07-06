// Created on: 1995-12-07
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepCheck_Result_HeaderFile
#define _BRepCheck_Result_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <BRepCheck_DataMapOfShapeListOfStatus.hxx>
#include <BRepCheck_DataMapIteratorOfDataMapOfShapeListOfStatus.hxx>
#include <Standard_Transient.hxx>
#include <BRepCheck_ListOfStatus.hxx>
class Standard_NoSuchObject;
class TopoDS_Shape;


class BRepCheck_Result;
DEFINE_STANDARD_HANDLE(BRepCheck_Result, Standard_Transient)


class BRepCheck_Result : public Standard_Transient
{

public:

  
  Standard_EXPORT void Init (const TopoDS_Shape& S);
  
  Standard_EXPORT virtual void InContext (const TopoDS_Shape& ContextShape) = 0;
  
  Standard_EXPORT virtual void Minimum() = 0;
  
  Standard_EXPORT virtual void Blind() = 0;
  
  Standard_EXPORT void SetFailStatus (const TopoDS_Shape& S);
  
    const BRepCheck_ListOfStatus& Status() const;
  
    Standard_Boolean IsMinimum() const;
  
    Standard_Boolean IsBlind() const;
  
  //! If  not  already   done,  performs the   InContext
  //! control and returns the list of status.
  Standard_EXPORT const BRepCheck_ListOfStatus& StatusOnShape (const TopoDS_Shape& S);
  
  Standard_EXPORT void InitContextIterator();
  
    Standard_Boolean MoreShapeInContext() const;
  
    const TopoDS_Shape& ContextualShape() const;
  
    const BRepCheck_ListOfStatus& StatusOnShape() const;
  
  Standard_EXPORT void NextShapeInContext();




  DEFINE_STANDARD_RTTIEXT(BRepCheck_Result,Standard_Transient)

protected:

  
  Standard_EXPORT BRepCheck_Result();

  TopoDS_Shape myShape;
  Standard_Boolean myMin;
  Standard_Boolean myBlind;
  BRepCheck_DataMapOfShapeListOfStatus myMap;


private:


  BRepCheck_DataMapIteratorOfDataMapOfShapeListOfStatus myIter;


};


#include <BRepCheck_Result.lxx>





#endif // _BRepCheck_Result_HeaderFile
