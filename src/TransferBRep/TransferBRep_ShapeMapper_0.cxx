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

#include <TransferBRep_ShapeMapper.hxx>

#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TransferBRep_ShapeInfo.hxx>
#include <Transfer_Finder.hxx>

//=================================================================================================

TransferBRep_ShapeMapper::TransferBRep_ShapeMapper(const TopoDS_Shape& akey)
    : theval(akey)
{
  SetHashCode(TopTools_ShapeMapHasher{}(akey));
}

//=================================================================================================

const TopoDS_Shape& TransferBRep_ShapeMapper::Value() const
{
  return theval;
}

//=================================================================================================

Standard_Boolean TransferBRep_ShapeMapper::Equates(const Handle(Transfer_Finder)& other) const
{
  if (other.IsNull())
    return Standard_False;
  if (GetHashCode() != other->GetHashCode())
    return Standard_False;
  if (other->DynamicType() != DynamicType())
    return Standard_False;
  Handle(TransferBRep_ShapeMapper) another = Handle(TransferBRep_ShapeMapper)::DownCast(other);
  return TopTools_ShapeMapHasher{}(theval, another->Value());
}

//=================================================================================================

Handle(Standard_Type) TransferBRep_ShapeMapper::ValueType() const
{
  return TransferBRep_ShapeInfo::Type(theval);
}

//=================================================================================================

Standard_CString TransferBRep_ShapeMapper::ValueTypeName() const
{
  return TransferBRep_ShapeInfo::TypeName(theval);
}
