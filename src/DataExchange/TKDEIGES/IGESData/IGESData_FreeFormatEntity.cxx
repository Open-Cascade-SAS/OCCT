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

#include <IGESData_FreeFormatEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESWriter.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_UndefinedContent.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESData_FreeFormatEntity, IGESData_UndefinedEntity)

//  Provides simple access to the constitution of an UndefinedEntity :
//  Methods of UndefinedContent, + Type & Form, + AddEntities (little gadget)
IGESData_FreeFormatEntity::IGESData_FreeFormatEntity() = default;

void IGESData_FreeFormatEntity::SetTypeNumber(const int typenum)
{
  InitTypeAndForm(typenum, 0);
}

void IGESData_FreeFormatEntity::SetFormNumber(const int formnum)
{
  InitTypeAndForm(TypeNumber(), formnum);
}

int IGESData_FreeFormatEntity::NbParams() const
{
  return UndefinedContent()->NbParams();
}

bool IGESData_FreeFormatEntity::ParamData(const int                              num,
                                          Interface_ParamType&                   ptype,
                                          occ::handle<IGESData_IGESEntity>&      ent,
                                          occ::handle<TCollection_HAsciiString>& val) const
{
  occ::handle<Standard_Transient> anEnt = ent;
  return UndefinedContent()->ParamData(num, ptype, anEnt, val)
         && !(ent = occ::down_cast<IGESData_IGESEntity>(anEnt)).IsNull();
}

Interface_ParamType IGESData_FreeFormatEntity::ParamType(const int num) const
{
  return UndefinedContent()->ParamType(num);
}

bool IGESData_FreeFormatEntity::IsParamEntity(const int num) const
{
  return UndefinedContent()->IsParamEntity(num);
}

occ::handle<IGESData_IGESEntity> IGESData_FreeFormatEntity::ParamEntity(const int num) const
{
  return occ::down_cast<IGESData_IGESEntity>(UndefinedContent()->ParamEntity(num));
}

bool IGESData_FreeFormatEntity::IsNegativePointer(const int num) const
{
  if (thenegptrs.IsNull())
    return false;
  int nb = thenegptrs->Length();
  for (int i = 1; i <= nb; i++)
    if (thenegptrs->Value(i) == num)
      return true;
  return false;
}

occ::handle<TCollection_HAsciiString> IGESData_FreeFormatEntity::ParamValue(const int num) const
{
  return UndefinedContent()->ParamValue(num);
}

occ::handle<NCollection_HSequence<int>> IGESData_FreeFormatEntity::NegativePointers() const
{
  return thenegptrs;
}

void IGESData_FreeFormatEntity::AddLiteral(const Interface_ParamType                    ptype,
                                           const occ::handle<TCollection_HAsciiString>& val)
{
  UndefinedContent()->AddLiteral(ptype, val);
}

void IGESData_FreeFormatEntity::AddLiteral(const Interface_ParamType ptype, const char* const val)
{
  UndefinedContent()->AddLiteral(ptype, new TCollection_HAsciiString(val));
}

void IGESData_FreeFormatEntity::AddEntity(const Interface_ParamType               ptype,
                                          const occ::handle<IGESData_IGESEntity>& ent,
                                          const bool                              negative)
{
  UndefinedContent()->AddEntity(ptype, ent);
  if (!negative)
    return;
  if (thenegptrs.IsNull())
    thenegptrs = new NCollection_HSequence<int>();
  thenegptrs->Append(NbParams());
}

void IGESData_FreeFormatEntity::AddEntities(
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& ents)
{
  if (ents.IsNull())
  {
    AddLiteral(Interface_ParamInteger, new TCollection_HAsciiString("0"));
    return;
  }
  AddLiteral(Interface_ParamInteger, new TCollection_HAsciiString(ents->Length()));
  int iup = ents->Upper();
  for (int i = ents->Lower(); i <= iup; i++)
  {
    AddEntity(Interface_ParamIdent, ents->Value(i));
  }
}

void IGESData_FreeFormatEntity::AddNegativePointers(
  const occ::handle<NCollection_HSequence<int>>& list)
{
  if (thenegptrs.IsNull())
    thenegptrs = new NCollection_HSequence<int>();
  thenegptrs->Append(list);
}

void IGESData_FreeFormatEntity::ClearNegativePointers()
{
  thenegptrs.Nullify();
}

void IGESData_FreeFormatEntity::WriteOwnParams(IGESData_IGESWriter& IW) const
{
  //  Redefined from UndefinedEntity for : NegativePointers
  int neg  = 0;
  int fneg = 0;
  if (!thenegptrs.IsNull())
    if (!thenegptrs->IsEmpty())
    {
      neg  = thenegptrs->Value(1);
      fneg = 1;
    }

  int nb = UndefinedContent()->NbParams();
  for (int i = 1; i <= nb; i++)
  {
    Interface_ParamType ptyp = UndefinedContent()->ParamType(i);
    if (ptyp == Interface_ParamVoid)
      IW.SendVoid();
    else if (UndefinedContent()->IsParamEntity(i))
    {
      DeclareAndCast(IGESData_IGESEntity, anent, UndefinedContent()->ParamEntity(i));
      //  Send Entity : Redefini
      if (i == neg)
      {
        IW.Send(anent, true);
        if (fneg >= thenegptrs->Length())
          neg = 0;
        else
        {
          fneg++;
          neg = thenegptrs->Value(fneg);
        }
      }
      else
        IW.Send(anent, false);
    }
    else
      IW.SendString(UndefinedContent()->ParamValue(i));
  }
}
