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

#ifndef _TransferBRep_ShapeMapper_HeaderFile
#define _TransferBRep_ShapeMapper_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Transfer_Finder.hxx>
class TopoDS_Shape;
class TopTools_ShapeMapHasher;
class TransferBRep_ShapeInfo;
class Transfer_Finder;

class TransferBRep_ShapeMapper;
DEFINE_STANDARD_HANDLE(TransferBRep_ShapeMapper, Transfer_Finder)

class TransferBRep_ShapeMapper : public Transfer_Finder
{

public:
  //! Creates a Mapper with a Value. This Value can then not be
  //! changed. It is used by the Hasher to compute the HashCode,
  //! which will then be stored for an immediate reading.
  Standard_EXPORT TransferBRep_ShapeMapper(const TopoDS_Shape& akey);

  //! Returns the contained value
  Standard_EXPORT const TopoDS_Shape& Value() const;

  //! Specific testof equality : defined as False if <other> has
  //! not the same true Type, else contents are compared (by
  //! C++ operator ==)
  Standard_EXPORT Standard_Boolean
    Equates(const Handle(Transfer_Finder)& other) const Standard_OVERRIDE;

  //! Returns the Type of the Value. By default, returns the
  //! DynamicType of <me>, but can be redefined
  Standard_EXPORT virtual Handle(Standard_Type) ValueType() const Standard_OVERRIDE;

  //! Returns the name of the Type of the Value. Default is name
  //! of ValueType, unless it is for a non-handled object
  Standard_EXPORT virtual Standard_CString ValueTypeName() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTI_INLINE(TransferBRep_ShapeMapper, Transfer_Finder)

protected:
private:
  TopoDS_Shape theval;
};

#endif // _TransferBRep_ShapeMapper_HeaderFile
