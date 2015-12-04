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
#include <MoniTool_Element.hxx>
#include <MoniTool_TransientElem.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TColStd_MapTransientHasher.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MoniTool_TransientElem,MoniTool_Element)

MoniTool_TransientElem::MoniTool_TransientElem (const Handle(Standard_Transient)& akey)
    : theval (akey)
{  SetHashCode ( TColStd_MapTransientHasher::HashCode (akey, IntegerLast() ) );  }


    const Handle(Standard_Transient)&  MoniTool_TransientElem::Value () const
      {  return theval;  }

    Standard_Boolean  MoniTool_TransientElem::Equates
  (const Handle(MoniTool_Element)& other) const
{
  if (other.IsNull()) return Standard_False;
  if (GetHashCode() != other->GetHashCode()) return Standard_False;
  if (other->DynamicType() != DynamicType()) return Standard_False;
  Handle(MoniTool_TransientElem) another = Handle(MoniTool_TransientElem)::DownCast(other);
//  return (theval == another->Value());
  return  TColStd_MapTransientHasher::IsEqual (theval,another->Value());
}

    Handle(Standard_Type)  MoniTool_TransientElem::ValueType () const
      {  return MoniTool_DataInfo::Type(theval);  }

    Standard_CString  MoniTool_TransientElem::ValueTypeName () const
      {  return MoniTool_DataInfo::TypeName(theval);  }
