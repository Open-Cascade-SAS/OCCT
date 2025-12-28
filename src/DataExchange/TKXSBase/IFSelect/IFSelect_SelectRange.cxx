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

#include <IFSelect_IntParam.hxx>
#include <IFSelect_SelectRange.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectRange, IFSelect_SelectExtract)

IFSelect_SelectRange::IFSelect_SelectRange() {}

void IFSelect_SelectRange::SetRange(const occ::handle<IFSelect_IntParam>& rankfrom,
                                    const occ::handle<IFSelect_IntParam>& rankto)
{
  thelower = rankfrom;
  theupper = rankto;
}

void IFSelect_SelectRange::SetOne(const occ::handle<IFSelect_IntParam>& rank)
{
  thelower = theupper = rank;
}

void IFSelect_SelectRange::SetFrom(const occ::handle<IFSelect_IntParam>& rankfrom)
{
  thelower = rankfrom;
  theupper.Nullify();
}

void IFSelect_SelectRange::SetUntil(const occ::handle<IFSelect_IntParam>& rankto)
{
  thelower.Nullify();
  theupper = rankto;
}

bool IFSelect_SelectRange::HasLower() const
{
  return (!thelower.IsNull());
}

occ::handle<IFSelect_IntParam> IFSelect_SelectRange::Lower() const
{
  return thelower;
}

int IFSelect_SelectRange::LowerValue() const
{
  if (thelower.IsNull())
    return 0;
  return thelower->Value();
}

bool IFSelect_SelectRange::HasUpper() const
{
  return (!theupper.IsNull());
}

occ::handle<IFSelect_IntParam> IFSelect_SelectRange::Upper() const
{
  return theupper;
}

int IFSelect_SelectRange::UpperValue() const
{
  if (theupper.IsNull())
    return 0;
  return theupper->Value();
}

bool IFSelect_SelectRange::Sort(const int rank,
                                const occ::handle<Standard_Transient>&,
                                const occ::handle<Interface_InterfaceModel>&) const
{
  int rankfrom = 0;
  if (!thelower.IsNull())
    rankfrom = thelower->Value();
  int rankto = 0;
  if (!theupper.IsNull())
    rankto = theupper->Value();
  return (rank >= rankfrom && (rankto == 0 || rankto >= rank));
}

TCollection_AsciiString IFSelect_SelectRange::ExtractLabel() const
{
  char lab[30];
  int  rankfrom = 0;
  if (!thelower.IsNull())
    rankfrom = thelower->Value();
  int rankto = 0;
  if (!theupper.IsNull())
    rankto = theupper->Value();
  if (rankfrom == rankto)
    Sprintf(lab, "Rank no %d", rankfrom);
  else if (rankfrom == 0)
    Sprintf(lab, "Until no %d", rankto);
  else if (rankto == 0)
    Sprintf(lab, "From no %d", rankto);
  else
    Sprintf(lab, "From %d Until %d", rankfrom, rankto);

  return TCollection_AsciiString(lab);
}
