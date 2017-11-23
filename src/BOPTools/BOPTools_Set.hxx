// Created by: Peter KURNEV
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

#ifndef _BOPTools_Set_HeaderFile
#define _BOPTools_Set_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_BaseAllocator.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Shape;



class BOPTools_Set 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BOPTools_Set();
Standard_EXPORT virtual ~BOPTools_Set();
  
  Standard_EXPORT BOPTools_Set(const Handle(NCollection_BaseAllocator)& theAllocator);
  
  Standard_EXPORT BOPTools_Set& Assign (const BOPTools_Set& Other);
BOPTools_Set& operator = (const BOPTools_Set& Other)
{
  return Assign(Other);
}
  
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  Standard_EXPORT void Add (const TopoDS_Shape& theS, const TopAbs_ShapeEnum theType);
  
  Standard_EXPORT Standard_Integer NbShapes() const;
  
  Standard_EXPORT Standard_Boolean IsEqual (const BOPTools_Set& aOther) const;
  
  Standard_EXPORT Standard_Integer HashCode (const Standard_Integer Upper) const;




protected:

  
  Standard_EXPORT void Clear();


  Handle(NCollection_BaseAllocator) myAllocator;
  TopTools_ListOfShape myShapes;
  TopoDS_Shape myShape;
  Standard_Integer myNbShapes;
  Standard_Integer mySum;
  Standard_Integer myUpper;


private:





};







#endif // _BOPTools_Set_HeaderFile
