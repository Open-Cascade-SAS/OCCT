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

#include <Interface_Check.hxx>
#include <Interface_ReportEntity.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_ReportEntity, Standard_Transient)

//=================================================================================================

Interface_ReportEntity::Interface_ReportEntity(const occ::handle<Standard_Transient>& unknown)
{
  theconcerned = unknown;
  thecontent   = unknown;
}

//=================================================================================================

Interface_ReportEntity::Interface_ReportEntity(const occ::handle<Interface_Check>&    acheck,
                                               const occ::handle<Standard_Transient>& concerned)
    : thecheck(acheck)
{
  theconcerned = concerned;
  thecheck->SetEntity(concerned);
}

//=================================================================================================

void Interface_ReportEntity::SetContent(const occ::handle<Standard_Transient>& content)
{
  thecontent = content;
}

//  ....                        CONSULTATION                        ....

//=================================================================================================

const occ::handle<Interface_Check>& Interface_ReportEntity::Check() const
{
  return thecheck;
}

//=================================================================================================

occ::handle<Interface_Check>& Interface_ReportEntity::CCheck()
{
  return thecheck;
}

//=================================================================================================

occ::handle<Standard_Transient> Interface_ReportEntity::Concerned() const
{
  return theconcerned;
}

//=================================================================================================

bool Interface_ReportEntity::HasContent() const
{
  return (!thecontent.IsNull());
}

//=================================================================================================

bool Interface_ReportEntity::HasNewContent() const
{
  return (!thecontent.IsNull() && thecontent != theconcerned);
}

//=================================================================================================

occ::handle<Standard_Transient> Interface_ReportEntity::Content() const
{
  return thecontent;
}

//=================================================================================================

bool Interface_ReportEntity::IsError() const
{
  return (thecheck->NbFails() > 0);
}

//=================================================================================================

bool Interface_ReportEntity::IsUnknown() const
{
  return ((thecheck->NbFails() == 0) && (thecheck->NbWarnings() == 0)
          && (theconcerned == thecontent));
}
