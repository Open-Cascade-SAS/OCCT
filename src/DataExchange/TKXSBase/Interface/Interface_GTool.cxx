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

#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_GTool.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_SignType.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_GTool, Standard_Transient)

Interface_GTool::Interface_GTool() {}

Interface_GTool::Interface_GTool(const occ::handle<Interface_Protocol>& proto, const int nb)
    : theproto(proto),
      thelib(proto)
{
  if (nb > 0)
  {
    thentnum.ReSize(nb);
    thentmod.ReSize(nb);
  }
}

void Interface_GTool::SetSignType(const occ::handle<Interface_SignType>& sign)
{
  thesign = sign;
}

occ::handle<Interface_SignType> Interface_GTool::SignType() const
{
  return thesign;
}

const char* Interface_GTool::SignValue(const occ::handle<Standard_Transient>&       ent,
                                            const occ::handle<Interface_InterfaceModel>& model) const
{
  if (ent.IsNull())
    return "";
  if (thesign.IsNull())
    return Interface_SignType::ClassName(ent->DynamicType()->Name());
  return thesign->Value(ent, model);
}

const char* Interface_GTool::SignName() const
{
  if (thesign.IsNull())
    return "Class Name";
  return thesign->Name();
}

void Interface_GTool::SetProtocol(const occ::handle<Interface_Protocol>& proto,
                                  const bool            enforce)
{
  if (proto == theproto && !enforce)
    return;
  theproto = proto;
  thelib.Clear();
  thelib.AddProtocol(proto);
}

occ::handle<Interface_Protocol> Interface_GTool::Protocol() const
{
  return theproto;
}

Interface_GeneralLib& Interface_GTool::Lib()
{
  return thelib;
}

void Interface_GTool::Reservate(const int nb, const bool enforce)
{
  int n = thentnum.NbBuckets();
  if (n < nb && !enforce)
    return;
  thentnum.ReSize(nb);
  thentmod.ReSize(nb);
}

void Interface_GTool::ClearEntities()
{
  thentnum.Clear();
  thentmod.Clear();
}

//=================================================================================================

bool Interface_GTool::Select(const occ::handle<Standard_Transient>& ent,
                                         occ::handle<Interface_GeneralModule>&  gmod,
                                         int&                 CN,
                                         const bool            enforce)
{
  const occ::handle<Standard_Type>& aType = ent->DynamicType();
  int             num   = thentmod.FindIndex(aType); // (ent);
  if (num == 0 || enforce)
  {
    if (thelib.Select(ent, gmod, CN))
    {
      num = thentmod.Add(aType, gmod);
      thentnum.Bind(aType, CN);
      return true;
    }
    return false;
  }
  gmod = occ::down_cast<Interface_GeneralModule>(thentmod.FindFromKey(aType));
  CN   = thentnum.Find(aType);
  return true;
}
