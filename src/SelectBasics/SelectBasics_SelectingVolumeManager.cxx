// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <SelectBasics_SelectingVolumeManager.hxx>

// =======================================================================
// function : SelectBasics_SelectingVolumeManager
// purpose  :
// =======================================================================
SelectBasics_SelectingVolumeManager::SelectBasics_SelectingVolumeManager()
{
  //
}

// =======================================================================
// function : ~SelectBasics_SelectingVolumeManager
// purpose  :
// =======================================================================
SelectBasics_SelectingVolumeManager::~SelectBasics_SelectingVolumeManager()
{
  //
}

// =======================================================================
// function : Overlaps
// purpose  :
// =======================================================================
Standard_Boolean SelectBasics_SelectingVolumeManager::Overlaps(
  const Handle(TColgp_HArray1OfPnt)& theArrayOfPts,
  Standard_Integer                   theSensType,
  SelectBasics_PickResult&           thePickResult) const
{
  return OverlapsPolygon(theArrayOfPts->Array1(), theSensType, thePickResult);
}
