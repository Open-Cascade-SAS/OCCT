// Created on: 1994-10-03
// Created by: Christian CAILLET
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

#ifndef _TransferBRep_OrientedShapeMapper_HeaderFile
#define _TransferBRep_OrientedShapeMapper_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Transfer_Finder.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Type.hxx>
#include <Standard_CString.hxx>
class TopoDS_Shape;
class TopTools_OrientedShapeMapHasher;
class TransferBRep_ShapeInfo;
class Transfer_Finder;


class TransferBRep_OrientedShapeMapper;
DEFINE_STANDARD_HANDLE(TransferBRep_OrientedShapeMapper, Transfer_Finder)


class TransferBRep_OrientedShapeMapper : public Transfer_Finder
{

public:

  
  Standard_EXPORT TransferBRep_OrientedShapeMapper(const TopoDS_Shape& akey);
  
  Standard_EXPORT const TopoDS_Shape& Value() const;
  
  Standard_EXPORT Standard_Boolean Equates (const Handle(Transfer_Finder)& other) const;
  
  Standard_EXPORT virtual Handle(Standard_Type) ValueType() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_CString ValueTypeName() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTI(TransferBRep_OrientedShapeMapper,Transfer_Finder)

protected:




private:


  TopoDS_Shape theval;


};







#endif // _TransferBRep_OrientedShapeMapper_HeaderFile
