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

class DDataStd_DrawPresentation;
DEFINE_STANDARD_HANDLE(DDataStd_DrawPresentation, TDF_Attribute)

//! draw presentation of a label of a document
class DDataStd_DrawPresentation : public TDF_Attribute
{

public:
  //! api methods on draw presentation
  //! ================================
  Standard_EXPORT static Standard_Boolean HasPresentation(const TDF_Label& L);

  Standard_EXPORT static Standard_Boolean IsDisplayed(const TDF_Label& L);

  Standard_EXPORT static void Display(const TDF_Label& L);

  Standard_EXPORT static void Erase(const TDF_Label& L);

  //! attribute implementation
  //! ========================
  Standard_EXPORT static void Update(const TDF_Label& L);

  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT DDataStd_DrawPresentation();

  Standard_EXPORT void SetDisplayed(const Standard_Boolean status);

  Standard_EXPORT Standard_Boolean IsDisplayed() const;

  Standard_EXPORT void SetDrawable(const Handle(Draw_Drawable3D)& D);

  Standard_EXPORT Handle(Draw_Drawable3D) GetDrawable() const;

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& with) Standard_OVERRIDE;

  //! call backs for viewer updating
  //! ==============================
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       into,
                             const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

  Standard_EXPORT virtual void AfterAddition() Standard_OVERRIDE;

  Standard_EXPORT virtual void BeforeRemoval() Standard_OVERRIDE;

  Standard_EXPORT virtual void BeforeForget() Standard_OVERRIDE;

  Standard_EXPORT virtual void AfterResume() Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean BeforeUndo(
    const Handle(TDF_AttributeDelta)& anAttDelta,
    const Standard_Boolean            forceIt = Standard_False) Standard_OVERRIDE;

  //! update draw viewer according to delta
  //! private methods
  //! ===============
  Standard_EXPORT virtual Standard_Boolean AfterUndo(
    const Handle(TDF_AttributeDelta)& anAttDelta,
    const Standard_Boolean            forceIt = Standard_False) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(DDataStd_DrawPresentation, TDF_Attribute)

protected:
private:
  Standard_EXPORT void DrawBuild();

  Standard_EXPORT static void DrawDisplay(const TDF_Label&                         L,
                                          const Handle(DDataStd_DrawPresentation)& P);

  Standard_EXPORT static void DrawErase(const TDF_Label&                         L,
                                        const Handle(DDataStd_DrawPresentation)& P);

  Standard_Boolean        isDisplayed;
  Handle(Draw_Drawable3D) myDrawable;
};

#endif // _DDataStd_DrawPresentation_HeaderFile
