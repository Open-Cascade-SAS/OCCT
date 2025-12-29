// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepShape_OrientedClosedShell_HeaderFile
#define _StepShape_OrientedClosedShell_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_Face.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepShape_Face;

class StepShape_OrientedClosedShell : public StepShape_ClosedShell
{

public:
  //! Returns a OrientedClosedShell
  Standard_EXPORT StepShape_OrientedClosedShell();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<StepShape_ClosedShell>&    aClosedShellElement,
                            const bool                                   aOrientation);

  Standard_EXPORT void SetClosedShellElement(
    const occ::handle<StepShape_ClosedShell>& aClosedShellElement);

  Standard_EXPORT occ::handle<StepShape_ClosedShell> ClosedShellElement() const;

  Standard_EXPORT void SetOrientation(const bool aOrientation);

  Standard_EXPORT bool Orientation() const;

  Standard_EXPORT void SetCfsFaces(
    const occ::handle<NCollection_HArray1<occ::handle<StepShape_Face>>>& aCfsFaces) override;

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepShape_Face>>> CfsFaces()
    const override;

  Standard_EXPORT occ::handle<StepShape_Face> CfsFacesValue(const int num) const override;

  Standard_EXPORT int NbCfsFaces() const override;

  DEFINE_STANDARD_RTTIEXT(StepShape_OrientedClosedShell, StepShape_ClosedShell)

private:
  occ::handle<StepShape_ClosedShell> closedShellElement;
  bool                               orientation;
};

#endif // _StepShape_OrientedClosedShell_HeaderFile
