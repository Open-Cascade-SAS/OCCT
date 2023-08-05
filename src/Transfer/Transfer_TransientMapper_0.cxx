// Created on: 1992-02-03
// Created by: Christian CAILLET
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

#include <Transfer_TransientMapper.hxx>

#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <Transfer_DataInfo.hxx>
#include <Transfer_Finder.hxx>

//=======================================================================
//function : Transfer_TransientMapper
//purpose  : 
//=======================================================================
Transfer_TransientMapper::Transfer_TransientMapper(const Handle(Standard_Transient)& akey)
  : theval(akey)
{
  SetHashCode(std::hash<Handle(Standard_Transient)>{}(akey));
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
const Handle(Standard_Transient)& Transfer_TransientMapper::Value() const
{
  return theval;
}

//=======================================================================
//function : Equates
//purpose  : 
//=======================================================================
Standard_Boolean  Transfer_TransientMapper::Equates
(const Handle(Transfer_Finder)& other) const
{
  if (other.IsNull()) return Standard_False;
  if (GetHashCode() != other->GetHashCode()) return Standard_False;
  if (other->DynamicType() != DynamicType()) return Standard_False;
  Handle(Transfer_TransientMapper) another = Handle(Transfer_TransientMapper)::DownCast(other);
  return theval == another->Value();
}

//=======================================================================
//function : ValueType
//purpose  : 
//=======================================================================
Handle(Standard_Type)  Transfer_TransientMapper::ValueType() const
{
  return Transfer_DataInfo::Type(theval);
}

//=======================================================================
//function : ValueTypeName
//purpose  : 
//=======================================================================
Standard_CString  Transfer_TransientMapper::ValueTypeName() const
{
  return Transfer_DataInfo::TypeName(theval);
}
