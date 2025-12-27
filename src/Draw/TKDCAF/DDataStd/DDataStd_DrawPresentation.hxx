// Created on: 1998-09-23
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _DDataStd_DrawPresentation_HeaderFile
#define _DDataStd_DrawPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
class Draw_Drawable3D;
class TDF_Label;
class Standard_GUID;
class TDF_RelocationTable;
class TDF_AttributeDelta;

//! draw presentation of a label of a document
class DDataStd_DrawPresentation : public TDF_Attribute
{

public:
  //! api methods on draw presentation
  //! ================================
  Standard_EXPORT static bool HasPresentation(const TDF_Label& L);

  Standard_EXPORT static bool IsDisplayed(const TDF_Label& L);

  Standard_EXPORT static void Display(const TDF_Label& L);

  Standard_EXPORT static void Erase(const TDF_Label& L);

  //! attribute implementation
  //! ========================
  Standard_EXPORT static void Update(const TDF_Label& L);

  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT DDataStd_DrawPresentation();

  Standard_EXPORT void SetDisplayed(const bool status);

  Standard_EXPORT bool IsDisplayed() const;

  Standard_EXPORT void SetDrawable(const occ::handle<Draw_Drawable3D>& D);

  Standard_EXPORT occ::handle<Draw_Drawable3D> GetDrawable() const;

  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& with) override;

  //! call backs for viewer updating
  //! ==============================
  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT virtual void AfterAddition() override;

  Standard_EXPORT virtual void BeforeRemoval() override;

  Standard_EXPORT virtual void BeforeForget() override;

  Standard_EXPORT virtual void AfterResume() override;

  Standard_EXPORT virtual bool BeforeUndo(
    const occ::handle<TDF_AttributeDelta>& anAttDelta,
    const bool            forceIt = false) override;

  //! update draw viewer according to delta
  //! private methods
  //! ===============
  Standard_EXPORT virtual bool AfterUndo(
    const occ::handle<TDF_AttributeDelta>& anAttDelta,
    const bool            forceIt = false) override;

  DEFINE_STANDARD_RTTIEXT(DDataStd_DrawPresentation, TDF_Attribute)

private:
  Standard_EXPORT void DrawBuild();

  Standard_EXPORT static void DrawDisplay(const TDF_Label&                         L,
                                          const occ::handle<DDataStd_DrawPresentation>& P);

  Standard_EXPORT static void DrawErase(const TDF_Label&                         L,
                                        const occ::handle<DDataStd_DrawPresentation>& P);

  bool        isDisplayed;
  occ::handle<Draw_Drawable3D> myDrawable;
};

#endif // _DDataStd_DrawPresentation_HeaderFile
