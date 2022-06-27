// Created on: 1998-09-30
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

#ifndef _TPrsStd_AISPresentation_HeaderFile
#define _TPrsStd_AISPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Quantity_NameOfColor.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Standard_Integer.hxx>
#include <TDF_Attribute.hxx>
#include <TDataXtd_Presentation.hxx>
#include <AIS_InteractiveContext.hxx>

class AIS_InteractiveObject;
class Standard_GUID;
class TDF_Label;
class TDF_RelocationTable;
class TDF_AttributeDelta;

class TPrsStd_AISPresentation;
DEFINE_STANDARD_HANDLE(TPrsStd_AISPresentation, TDF_Attribute)

//! An attribute to associate an
//! AIS_InteractiveObject to a label in an AIS viewer.
//! This attribute works in collaboration with TPrsStd_AISViewer.
//! Note that all the Set... and Unset... attribute
//! methods as well as the query methods for
//! visualization attributes and the HasOwn... test
//! methods are shortcuts to the respective
//! AIS_InteractiveObject settings.
class TPrsStd_AISPresentation : public TDF_Attribute
{

public:

  
  //! Returns the GUID for TPrsStd_AISPresentation attributes.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Creates or retrieves the presentation attribute on
  //! the label L, and sets the GUID driver.
  Standard_EXPORT static Handle(TPrsStd_AISPresentation) Set (const TDF_Label& L, const Standard_GUID& driver);
  
  //! Delete (if exist) the presentation attribute associated to the label <L>.
  Standard_EXPORT static void Unset (const TDF_Label& L);
  
  //! Creates or retrieves the AISPresentation
  //! attribute attached to master.
  //! The GUID of the driver will be the GUID of master.
  //! master is the attribute you want to display.
  Standard_EXPORT static Handle(TPrsStd_AISPresentation) Set (const Handle(TDF_Attribute)& master);
  
  Standard_EXPORT TPrsStd_AISPresentation();
  
  Standard_EXPORT void SetDisplayed (const Standard_Boolean B);
  
  //! Display presentation of object in AIS viewer.
  //! If <update> = True then AISObject is recomputed and all
  //! the visualization settings are applied
  Standard_EXPORT void Display (const Standard_Boolean update = Standard_False);
  
  //! Removes the presentation of this AIS
  //! presentation attribute from the TPrsStd_AISViewer.
  //! If remove is true, this AIS presentation attribute
  //! is removed from the interactive context.
  Standard_EXPORT void Erase (const Standard_Boolean remove = Standard_False);
  
  //! Recompute presentation of object and apply the visualization settings
  Standard_EXPORT void Update();
  
  Standard_EXPORT Standard_GUID GetDriverGUID() const;
  
  Standard_EXPORT void SetDriverGUID (const Standard_GUID& guid);
  

  //! Returns true if this AIS presentation attribute is displayed.
  Standard_EXPORT Standard_Boolean IsDisplayed() const;
  
  //! Returns AIS_InteractiveObject stored in the presentation attribute
  Standard_EXPORT Handle(AIS_InteractiveObject) GetAIS() const;
  

  //! Returns the material setting for this presentation attribute.
  Standard_EXPORT Graphic3d_NameOfMaterial Material() const;
  
  //! Sets the material aName for this presentation  attribute.
  Standard_EXPORT void SetMaterial (const Graphic3d_NameOfMaterial aName);
  
  //! Returns true if this presentation attribute already has a material setting.
  Standard_EXPORT Standard_Boolean HasOwnMaterial() const;
  
  //! Removes the material setting from this presentation attribute.
  Standard_EXPORT void UnsetMaterial();
  

  //! Sets the transparency value aValue for this
  //! presentation attribute.
  //! This value is 0.6 by default.
  Standard_EXPORT void SetTransparency (const Standard_Real aValue = 0.6);
  
  Standard_EXPORT Standard_Real Transparency() const;
  
  //! Returns true if this presentation attribute already has a transparency setting.
  Standard_EXPORT Standard_Boolean HasOwnTransparency() const;
  
  //! Removes the transparency setting from this presentation attribute.
  Standard_EXPORT void UnsetTransparency();
  
  Standard_EXPORT Quantity_NameOfColor Color() const;
  
  //! Sets the color aColor for this presentation attribute.
  Standard_EXPORT void SetColor (const Quantity_NameOfColor aColor);
  
  //! Returns true if this presentation attribute already has a color setting.
  Standard_EXPORT Standard_Boolean HasOwnColor() const;
  
  //! Removes the color setting from this presentation attribute.
  Standard_EXPORT void UnsetColor();
  
  Standard_EXPORT Standard_Real Width() const;
  
  //! Sets the width aWidth for this presentation attribute.
  Standard_EXPORT void SetWidth (const Standard_Real aWidth);
  
  //! Returns true if this presentation attribute already has a width setting.
  Standard_EXPORT Standard_Boolean HasOwnWidth() const;
  
  //! Removes the width setting from this presentation attribute.
  Standard_EXPORT void UnsetWidth();
  
  Standard_EXPORT Standard_Integer Mode() const;
  
  Standard_EXPORT void SetMode (const Standard_Integer theMode);
  
  Standard_EXPORT Standard_Boolean HasOwnMode() const;
  
  Standard_EXPORT void UnsetMode();

  //! Returns selection mode(s) of the attribute.
  //! It starts with 1 .. GetNbSelectionModes().
  Standard_EXPORT Standard_Integer GetNbSelectionModes() const;
  Standard_EXPORT Standard_Integer SelectionMode(const int index = 1) const;

  //! Sets selection mode.
  //! If "theTransaction" flag is OFF, modification of the attribute doesn't influence the transaction mechanism
  //! (the attribute doesn't participate in undo/redo because of this modification).
  //! Certainly, if any other data of the attribute is modified (display mode, color, ...),
  //! the attribute will be included into undo/redo.
  Standard_EXPORT void SetSelectionMode(const Standard_Integer theSelectionMode, const Standard_Boolean theTransaction = Standard_True);
  Standard_EXPORT void AddSelectionMode(const Standard_Integer theSelectionMode, const Standard_Boolean theTransaction = Standard_True);

  Standard_EXPORT Standard_Boolean HasOwnSelectionMode() const;
  
  //! Clears all selection modes of the attribute.
  Standard_EXPORT void UnsetSelectionMode();
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(TDF_Attribute) BackupCopy() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void AfterAddition() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void BeforeRemoval() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void BeforeForget() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void AfterResume() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean BeforeUndo (const Handle(TDF_AttributeDelta)& anAttDelta, const Standard_Boolean forceIt = Standard_False) Standard_OVERRIDE;
  
  //! update AIS viewer according to delta
  Standard_EXPORT virtual Standard_Boolean AfterUndo (const Handle(TDF_AttributeDelta)& anAttDelta, const Standard_Boolean forceIt = Standard_False) Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(TPrsStd_AISPresentation,TDF_Attribute)

protected:

  //! Returns attribute storing presentation data
  Standard_EXPORT virtual Handle(TDataXtd_Presentation) getData () const;

private:

  Handle(AIS_InteractiveContext) getAISContext() const;

  //! Activates selection mode of the interactive object.
  //! It is called internally on change of selection mode and AISUpdate().
  void ActivateSelectionMode();
  
  //! Updates AIS_InteractiveObject stored in the attribute
  //! and applies the visualization settings
  Standard_EXPORT void AISUpdate();
  
  //! Displays AIS_InteractiveObject stored in the attribute
  Standard_EXPORT void AISDisplay();
  
  //! Erases AIS_InteractiveObject stored in the attribute in
  //! the viewer; If <remove> = True then AISObject is removed
  //! from AIS_InteractiveContext instead of simple erasing in the viewer
  Standard_EXPORT void AISErase (const Standard_Boolean remove = Standard_False);

private:
  Handle(AIS_InteractiveObject) myAIS;
};

#endif // _TPrsStd_AISPresentation_HeaderFile
