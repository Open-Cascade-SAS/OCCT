// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_Shell.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_ManifoldSolid, IGESData_IGESEntity)

IGESSolid_ManifoldSolid::IGESSolid_ManifoldSolid() {}

void IGESSolid_ManifoldSolid::Init(
  const occ::handle<IGESSolid_Shell>&                                   aShell,
  const bool                                                            Shellflag,
  const occ::handle<NCollection_HArray1<occ::handle<IGESSolid_Shell>>>& VoidShells,
  const occ::handle<NCollection_HArray1<int>>&                          VoidShellFlags)
{
  if (!VoidShells.IsNull())
    if (VoidShells->Lower() != 1 || VoidShellFlags->Lower() != 1
        || VoidShells->Length() != VoidShellFlags->Length())
      throw Standard_DimensionError("IGESSolid_ManifoldSolid : Init");

  theShell           = aShell;
  theOrientationFlag = Shellflag;
  theVoidShells      = VoidShells;
  theOrientFlags     = VoidShellFlags;
  InitTypeAndForm(186, 0);
}

occ::handle<IGESSolid_Shell> IGESSolid_ManifoldSolid::Shell() const
{
  return theShell;
}

bool IGESSolid_ManifoldSolid::OrientationFlag() const
{
  return theOrientationFlag;
}

int IGESSolid_ManifoldSolid::NbVoidShells() const
{
  return (theVoidShells.IsNull() ? 0 : theVoidShells->Length());
}

occ::handle<IGESSolid_Shell> IGESSolid_ManifoldSolid::VoidShell(const int index) const
{
  occ::handle<IGESSolid_Shell> ashell; // par defaut sera Null
  if (!theVoidShells.IsNull())
    ashell = theVoidShells->Value(index);
  return ashell;
}

bool IGESSolid_ManifoldSolid::VoidOrientationFlag(const int index) const
{
  if (!theOrientFlags.IsNull())
    return (theOrientFlags->Value(index) != 0);
  else
    return false; // pour retourner qqchose ...
}
