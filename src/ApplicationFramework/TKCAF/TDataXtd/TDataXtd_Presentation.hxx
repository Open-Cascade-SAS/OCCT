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
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>

class TDF_Label;
class TDF_RelocationTable;

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
  Standard_EXPORT static occ::handle<TDataXtd_Presentation> Set(const TDF_Label&     theLabel,
                                                           const Standard_GUID& theDriverId);

  //! Remove attribute of this type from the label
  Standard_EXPORT static void Unset(const TDF_Label& theLabel);

  //! Returns the ID of the attribute.
  Standard_EXPORT const Standard_GUID& ID() const override;

  //! Returns the ID of the attribute.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Restores the contents from <anAttribute> into this
  //! one. It is used when aborting a transaction.
  Standard_EXPORT virtual void Restore(const occ::handle<TDF_Attribute>& anAttribute) override;

  //! Returns an new empty attribute from the good end
  //! type. It is used by the copy algorithm.
  Standard_EXPORT virtual occ::handle<TDF_Attribute> NewEmpty() const override;

  //! This method is different from the "Copy" one,
  //! because it is used when copying an attribute from
  //! a source structure into a target structure. This
  //! method pastes the current attribute to the label
  //! corresponding to the insertor. The pasted
  //! attribute may be a brand new one or a new version
  //! of the previous one.
  Standard_EXPORT virtual void Paste(const occ::handle<TDF_Attribute>&       intoAttribute,
                                     const occ::handle<TDF_RelocationTable>& aRelocTationable) const
    override;

  Standard_EXPORT occ::handle<TDF_Attribute> BackupCopy() const override;

  DEFINE_STANDARD_RTTIEXT(TDataXtd_Presentation, TDF_Attribute)

public:
  //!@name Access to data

  //! Returns the GUID of the driver managing display of associated AIS object
  Standard_EXPORT Standard_GUID GetDriverGUID() const;

  //! Sets the GUID of the driver managing display of associated AIS object
  Standard_EXPORT void SetDriverGUID(const Standard_GUID& theGUID);

  Standard_EXPORT bool IsDisplayed() const;

  Standard_EXPORT bool HasOwnMaterial() const;

  Standard_EXPORT bool HasOwnTransparency() const;

  Standard_EXPORT bool HasOwnColor() const;

  Standard_EXPORT bool HasOwnWidth() const;

  Standard_EXPORT bool HasOwnMode() const;

  Standard_EXPORT bool HasOwnSelectionMode() const;

  Standard_EXPORT void SetDisplayed(const bool theIsDisplayed);

  Standard_EXPORT void SetMaterialIndex(const int theMaterialIndex);

  Standard_EXPORT void SetTransparency(const double theValue);

  Standard_EXPORT void SetColor(const Quantity_NameOfColor theColor);

  Standard_EXPORT void SetWidth(const double theWidth);

  Standard_EXPORT void SetMode(const int theMode);

  //! Returns the number of selection modes of the attribute.
  //! It starts with 1 .. GetNbSelectionModes().
  Standard_EXPORT int GetNbSelectionModes() const;

  //! Sets selection mode.
  //! If "theTransaction" flag is OFF, modification of the attribute doesn't influence the
  //! transaction mechanism (the attribute doesn't participate in undo/redo because of this
  //! modification). Certainly, if any other data of the attribute is modified (display mode, color,
  //! ...), the attribute will be included into undo/redo.
  Standard_EXPORT void SetSelectionMode(const int theSelectionMode,
                                        const bool theTransaction = true);
  Standard_EXPORT void AddSelectionMode(const int theSelectionMode,
                                        const bool theTransaction = true);

  Standard_EXPORT int MaterialIndex() const;

  Standard_EXPORT double Transparency() const;

  Standard_EXPORT Quantity_NameOfColor Color() const;

  Standard_EXPORT double Width() const;

  Standard_EXPORT int Mode() const;

  Standard_EXPORT int SelectionMode(const int index = 1) const;

  Standard_EXPORT void UnsetMaterial();

  Standard_EXPORT void UnsetTransparency();

  Standard_EXPORT void UnsetColor();

  Standard_EXPORT void UnsetWidth();

  Standard_EXPORT void UnsetMode();

  Standard_EXPORT void UnsetSelectionMode();

public:
  //! Convert values of old Quantity_NameOfColor to new enumeration for reading old documents
  //! after #0030969 (Coding Rules - refactor Quantity_Color.cxx color table definition).
  Standard_EXPORT static Quantity_NameOfColor getColorNameFromOldEnum(int theOld);

  //! Convert Quantity_NameOfColor to old enumeration value for writing documents in compatible
  //! format.
  Standard_EXPORT static int getOldColorNameFromNewEnum(Quantity_NameOfColor theNew);

private:
  Standard_GUID         myDriverGUID;
  Quantity_NameOfColor  myColor;
  int      myMaterialIndex;
  int      myMode;
  NCollection_List<int> mySelectionModes;
  double         myTransparency;
  double         myWidth;
  bool      myIsDisplayed;
  bool      myHasOwnColor;
  bool      myHasOwnMaterial;
  bool      myHasOwnTransparency;
  bool      myHasOwnWidth;
  bool      myHasOwnMode;
  bool      myHasOwnSelectionMode;

  //! Checks a list of selection modes.
  bool HasSelectionMode(const int theSelectionMode) const;
};

#endif // _TDataXtd_Presentation_HeaderFile
