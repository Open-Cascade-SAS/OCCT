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

#include <MoniTool_DataInfo.hxx>
#include <MoniTool_TransientElem.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MoniTool_TransientElem, MoniTool_Element)

MoniTool_TransientElem::MoniTool_TransientElem(const occ::handle<Standard_Transient>& akey)
    : theval(akey)
{
  SetHashCode(std::hash<occ::handle<Standard_Transient>>{}(akey));
}

const occ::handle<Standard_Transient>& MoniTool_TransientElem::Value() const
{
  return theval;
}

bool MoniTool_TransientElem::Equates(const occ::handle<MoniTool_Element>& other) const
{
  if (other.IsNull())
    return false;
  if (GetHashCode() != other->GetHashCode())
    return false;
  if (other->DynamicType() != DynamicType())
    return false;
  occ::handle<MoniTool_TransientElem> another = occ::down_cast<MoniTool_TransientElem>(other);
  //  return (theval == another->Value());
  return theval == another->Value();
}

occ::handle<Standard_Type> MoniTool_TransientElem::ValueType() const
{
  return MoniTool_DataInfo::Type(theval);
}

const char* MoniTool_TransientElem::ValueTypeName() const
{
  return MoniTool_DataInfo::TypeName(theval);
}
