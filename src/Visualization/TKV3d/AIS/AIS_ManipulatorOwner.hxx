// Created on: 2015-12-23
// Created by: Anastasia BORISOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _AIS_ManipulatorOwner_HeaderFile
#define _AIS_ManipulatorOwner_HeaderFile

#include <AIS_Manipulator.hxx>

#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SelectableObject.hxx>

//! Entity owner for selection management of AIS_Manipulator object.
class AIS_ManipulatorOwner : public SelectMgr_EntityOwner
{
public:
  DEFINE_STANDARD_RTTIEXT(AIS_ManipulatorOwner, SelectMgr_EntityOwner)

  Standard_EXPORT AIS_ManipulatorOwner(const occ::handle<SelectMgr_SelectableObject>& theSelObject,
                                       const int                                      theIndex,
                                       const AIS_ManipulatorMode                      theMode,
                                       const int thePriority = 0);

  Standard_EXPORT void HilightWithColor(const occ::handle<PrsMgr_PresentationManager>& thePM,
                                        const occ::handle<Prs3d_Drawer>&               theStyle,
                                        const int theMode) override;

  Standard_EXPORT bool IsHilighted(const occ::handle<PrsMgr_PresentationManager>& thePM,
                                   const int theMode) const override;

  Standard_EXPORT void Unhilight(const occ::handle<PrsMgr_PresentationManager>& thePM,
                                 const int                                      theMode) override;

  AIS_ManipulatorMode Mode() const { return myMode; }

  //! @return index of manipulator axis.
  int Index() const { return myIndex; }

protected:
  int                 myIndex; //!< index of manipulator axis.
  AIS_ManipulatorMode myMode;  //!< manipulation (highlight) mode.
};

#endif // _AIS_ManipulatorOwner_HeaderFile
