// Created by: Ilya SEVRIKOV
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _AIS_TrihedronOwner_HeaderFile
#define _AIS_TrihedronOwner_HeaderFile

#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <Prs3d_DatumParts.hxx>

//! Entity owner for selection management of AIS_Trihedron object.
class AIS_TrihedronOwner : public SelectMgr_EntityOwner
{
  DEFINE_STANDARD_RTTIEXT(AIS_TrihedronOwner, SelectMgr_EntityOwner)
public:
  //! Creates an owner of AIS_Trihedron object.
  Standard_EXPORT AIS_TrihedronOwner(const occ::handle<SelectMgr_SelectableObject>& theSelObject,
                                     const Prs3d_DatumParts                         theDatumPart,
                                     const int                                      thePriority);

  //! Returns the datum part identifier.
  Prs3d_DatumParts DatumPart() const { return myDatumPart; }

  //! Highlights selectable object's presentation.
  Standard_EXPORT void HilightWithColor(
    const occ::handle<PrsMgr_PresentationManager>& thePM,
    const occ::handle<Prs3d_Drawer>&               theStyle,
    const int                                      theMode) override;

  //! Returns true if the presentation manager thePM
  //! highlights selections corresponding to the selection mode aMode.
  Standard_EXPORT bool IsHilighted(const occ::handle<PrsMgr_PresentationManager>& thePM,
                                   const int theMode) const override;

  //! Removes highlighting from the owner of a detected
  //! selectable object in the presentation manager thePM.
  Standard_EXPORT void Unhilight(const occ::handle<PrsMgr_PresentationManager>& thePM,
                                         const int theMode) override;

protected:
  Prs3d_DatumParts myDatumPart; //!< part of datum selected
};

#endif // _AIS_TrihedronOwner_HeaderFile
