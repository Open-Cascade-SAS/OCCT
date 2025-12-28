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

#include <IFSelect_SignType.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SignType, IFSelect_Signature)

static const char* nulsign = "";

IFSelect_SignType::IFSelect_SignType(const bool nopk)
    : IFSelect_Signature((const char*)(nopk ? "Class Type" : "Dynamic Type")),
      thenopk(nopk)
{
}

const char* IFSelect_SignType::Value(const occ::handle<Standard_Transient>& ent,
                                          const occ::handle<Interface_InterfaceModel>& /*model*/) const
{
  if (ent.IsNull())
    return nulsign;
  DeclareAndCast(Standard_Type, atype, ent);
  if (atype.IsNull())
    atype = ent->DynamicType();
  const char* tn = atype->Name();
  if (!thenopk)
    return tn;
  for (int i = 0; tn[i] != '\0'; i++)
  {
    if (tn[i] == '_')
      return &tn[i + 1];
  }
  return tn;
}
