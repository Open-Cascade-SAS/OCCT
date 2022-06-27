// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _TDataXtd_Presentation_HeaderFile
#define _TDataXtd_Presentation_HeaderFile

#include <Standard.hxx>
#include <Standard_GUID.hxx>

#include <gp_Pnt.hxx>
#include <TDF_Attribute.hxx>
#include <Quantity_NameOfColor.hxx>
#include <TColStd_ListOfInteger.hxx>

class TDF_Label;
class TDF_RelocationTable;


class TDataXtd_Presentation;
DEFINE_STANDARD_HANDLE(TDataXtd_Presentation, TDF_Attribute)

//! Attribute containing parameters of presentation of the shape,
//! e.g. the shape attached to the same label and displayed using 
//! TPrsStd tools (see TPrsStd_AISPresentation).
class TDataXtd_Presentation : public TDF_Attribute
{
public:
  //!@name Attribute mechanics
  
  //! Empty constructor
  Standard_EXPORT TDataXtd_Presentation();

  //! Create if not found the TDataXtd_Presentation attribute and set its driver GUID
  Standard_EXPORT static Handle(TDataXtd_Presentation) Set(const TDF_Label& theLabel, const Standard_GUID& theDriverId);
  
  //! Remove attribute of this type from the label
  Standard_EXPORT static void Unset(const TDF_Label& theLabel);
  
  //! Returns the ID of the attribute.
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  //! Returns the ID of the attribute.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Restores the contents from <anAttribute> into this
  //! one. It is used when aborting a transaction.
  Standard_EXPORT virtual void Restore (const Handle(TDF_Attribute)& anAttribute) Standard_OVERRIDE;
  
  //! Returns an new empty attribute from the good end
  //! type. It is used by the copy algorithm.
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  //! This method is different from the "Copy" one,
  //! because it is used when copying an attribute from
  //! a source structure into a target structure. This
  //! method pastes the current attribute to the label
  //! corresponding to the insertor. The pasted
  //! attribute may be a brand new one or a new version
  //! of the previous one.
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& intoAttribute, 
                                      const Handle(TDF_RelocationTable)& aRelocTationable) const Standard_OVERRIDE;

  Standard_EXPORT Handle(TDF_Attribute) BackupCopy() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(TDataXtd_Presentation,TDF_Attribute)

public:
  //!@name Access to data

  //! Returns the GUID of the driver managing display of associated AIS object
  Standard_EXPORT Standard_GUID GetDriverGUID() const;

  //! Sets the GUID of the driver managing display of associated AIS object
  Standard_EXPORT void SetDriverGUID(const Standard_GUID& theGUID);
  
  Standard_EXPORT Standard_Boolean IsDisplayed() const;

  Standard_EXPORT Standard_Boolean HasOwnMaterial() const;

  Standard_EXPORT Standard_Boolean HasOwnTransparency() const;

  Standard_EXPORT Standard_Boolean HasOwnColor() const;

  Standard_EXPORT Standard_Boolean HasOwnWidth() const;

  Standard_EXPORT Standard_Boolean HasOwnMode() const;

  Standard_EXPORT Standard_Boolean HasOwnSelectionMode() const;

  Standard_EXPORT void SetDisplayed(const Standard_Boolean theIsDisplayed);

  Standard_EXPORT void SetMaterialIndex(const Standard_Integer theMaterialIndex);

  Standard_EXPORT void SetTransparency(const Standard_Real theValue);

  Standard_EXPORT void SetColor(const Quantity_NameOfColor theColor);

  Standard_EXPORT void SetWidth(const Standard_Real theWidth);

  Standard_EXPORT void SetMode(const Standard_Integer theMode);

  //! Returns the number of selection modes of the attribute.
  //! It starts with 1 .. GetNbSelectionModes().
  Standard_EXPORT Standard_Integer GetNbSelectionModes() const;

  //! Sets selection mode.
  //! If "theTransaction" flag is OFF, modification of the attribute doesn't influence the transaction mechanism
  //! (the attribute doesn't participate in undo/redo because of this modification).
  //! Certainly, if any other data of the attribute is modified (display mode, color, ...),
  //! the attribute will be included into undo/redo.
  Standard_EXPORT void SetSelectionMode(const Standard_Integer theSelectionMode, const Standard_Boolean theTransaction = Standard_True);
  Standard_EXPORT void AddSelectionMode(const Standard_Integer theSelectionMode, const Standard_Boolean theTransaction = Standard_True);

  Standard_EXPORT Standard_Integer MaterialIndex() const;

  Standard_EXPORT Standard_Real Transparency() const;

  Standard_EXPORT Quantity_NameOfColor Color() const;

  Standard_EXPORT Standard_Real Width() const;

  Standard_EXPORT Standard_Integer Mode() const;

  Standard_EXPORT Standard_Integer SelectionMode(const int index = 1) const;

  Standard_EXPORT void UnsetMaterial();

  Standard_EXPORT void UnsetTransparency();

  Standard_EXPORT void UnsetColor();

  Standard_EXPORT void UnsetWidth();

  Standard_EXPORT void UnsetMode();

  Standard_EXPORT void UnsetSelectionMode();

public:
  //! Convert values of old Quantity_NameOfColor to new enumeration for reading old documents
  //! after #0030969 (Coding Rules - refactor Quantity_Color.cxx color table definition).
  Standard_EXPORT static Quantity_NameOfColor getColorNameFromOldEnum (Standard_Integer theOld);

  //! Convert Quantity_NameOfColor to old enumeration value for writing documents in compatible format.
  Standard_EXPORT static Standard_Integer getOldColorNameFromNewEnum (Quantity_NameOfColor theNew);

private:
  Standard_GUID  myDriverGUID;
  Quantity_NameOfColor myColor;
  Standard_Integer myMaterialIndex;
  Standard_Integer myMode;
  TColStd_ListOfInteger mySelectionModes;
  Standard_Real myTransparency;
  Standard_Real myWidth;
  Standard_Boolean myIsDisplayed;
  Standard_Boolean myHasOwnColor;
  Standard_Boolean myHasOwnMaterial;
  Standard_Boolean myHasOwnTransparency;
  Standard_Boolean myHasOwnWidth;
  Standard_Boolean myHasOwnMode;
  Standard_Boolean myHasOwnSelectionMode;

  //! Checks a list of selection modes.
  Standard_Boolean HasSelectionMode(const Standard_Integer theSelectionMode) const;
};

#endif // _TDataXtd_Presentation_HeaderFile
