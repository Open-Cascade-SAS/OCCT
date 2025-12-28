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

#include <StepShape_OrientedOpenShell.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_OrientedOpenShell, StepShape_OpenShell)

StepShape_OrientedOpenShell::StepShape_OrientedOpenShell() {}

void StepShape_OrientedOpenShell::Init(const occ::handle<TCollection_HAsciiString>& aName,
                                       const occ::handle<StepShape_OpenShell>&      aOpenShellElement,
                                       const bool                  aOrientation)
{
  // --- classe own fields ---
  openShellElement = aOpenShellElement;
  orientation      = aOrientation;
  // --- classe inherited fields ---
  occ::handle<NCollection_HArray1<occ::handle<StepShape_Face>>> aCfsFaces;
  aCfsFaces.Nullify();
  StepShape_ConnectedFaceSet::Init(aName, aCfsFaces);
}

void StepShape_OrientedOpenShell::SetOpenShellElement(
  const occ::handle<StepShape_OpenShell>& aOpenShellElement)
{
  openShellElement = aOpenShellElement;
}

occ::handle<StepShape_OpenShell> StepShape_OrientedOpenShell::OpenShellElement() const
{
  return openShellElement;
}

void StepShape_OrientedOpenShell::SetOrientation(const bool aOrientation)
{
  orientation = aOrientation;
}

bool StepShape_OrientedOpenShell::Orientation() const
{
  return orientation;
}

void StepShape_OrientedOpenShell::SetCfsFaces(const occ::handle<NCollection_HArray1<occ::handle<StepShape_Face>>>& /*aCfsFaces*/)
{
  // WARNING : the field is redefined.
  // field set up forbidden.
  std::cout << "Field is redefined, SetUp Forbidden" << std::endl;
}

occ::handle<NCollection_HArray1<occ::handle<StepShape_Face>>> StepShape_OrientedOpenShell::CfsFaces() const
{
  // WARNING : the field is redefined.
  // method body is not yet automatically wrote
  // Attention, cette modif. est juste pour la compilation
  return openShellElement->CfsFaces();
}

occ::handle<StepShape_Face> StepShape_OrientedOpenShell::CfsFacesValue(const int num) const
{
  // WARNING : the field is redefined.
  // method body is not yet automatically wrote
  // Attention, cette modif. est juste pour la compilation
  return openShellElement->CfsFacesValue(num);
}

int StepShape_OrientedOpenShell::NbCfsFaces() const
{
  // WARNING : the field is redefined.
  // method body is not yet automatically wrote
  // Attention, cette modif. est juste pour la compilation
  return openShellElement->NbCfsFaces();
}
