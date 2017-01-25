// Created on: 2016-11-29
// Created by: Irina KRYLOVA
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

#ifndef _XCAFDoc_ClippingPlaneTool_HeaderFile
#define _XCAFDoc_ClippingPlaneTool_HeaderFile

#include <gp_Pln.hxx>
#include <TDF_LabelSequence.hxx>

class XCAFDoc_ClippingPlaneTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_ClippingPlaneTool, TDF_Attribute)

//! Provide tool for management of ClippingPlane section of document.
//! Provide tool to store, retrieve, remove and modify clipping planes.
//! Each clipping plane consists of gp_Pln and its name.
class XCAFDoc_ClippingPlaneTool : public TDF_Attribute
{

public:

  
  Standard_EXPORT XCAFDoc_ClippingPlaneTool();
  
  //! Creates (if not exist) ClippingPlaneTool.
  Standard_EXPORT static Handle(XCAFDoc_ClippingPlaneTool) Set (const TDF_Label& theLabel);
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! returns the label under which ClippingPlanes are stored
  Standard_EXPORT TDF_Label BaseLabel() const;
  
  //! Returns True if label belongs to a ClippingPlane table and
  //! is a ClippingPlane definition
  Standard_EXPORT Standard_Boolean IsClippingPlane (const TDF_Label& theLabel) const;
  
  //! Returns ClippingPlane defined by label lab
  //! Returns False if the label is not in ClippingPlane table
  //! or does not define a ClippingPlane
  Standard_EXPORT Standard_Boolean GetClippingPlane(const TDF_Label& theLabel, gp_Pln& thePlane, TCollection_ExtendedString& theName, Standard_Boolean &theCapping) const;

  //! Returns ClippingPlane defined by label lab
  //! Returns False if the label is not in ClippingPlane table
  //! or does not define a ClippingPlane
  Standard_EXPORT Standard_Boolean GetClippingPlane(const TDF_Label& theLabel, gp_Pln& thePlane, Handle(TCollection_HAsciiString)& theName, Standard_Boolean &theCapping) const;
  
  //! Adds a clipping plane definition to a ClippingPlane table and returns
  //! its label (returns existing label if the same clipping plane
  //! is already defined)
  Standard_EXPORT TDF_Label AddClippingPlane(const gp_Pln thePlane, const TCollection_ExtendedString theName, const Standard_Boolean theCapping) const;

  //! Adds a clipping plane definition to a ClippingPlane table and returns
  //! its label (returns existing label if the same clipping plane
  //! is already defined)
  Standard_EXPORT TDF_Label AddClippingPlane(const gp_Pln thePlane, const Handle(TCollection_HAsciiString)& theName, const Standard_Boolean theCapping) const;

  //! Adds a clipping plane definition to a ClippingPlane table and returns
  //! its label (returns existing label if the same clipping plane
  //! is already defined)
  Standard_EXPORT TDF_Label AddClippingPlane(const gp_Pln thePlane, const TCollection_ExtendedString theName) const;

  //! Adds a clipping plane definition to a ClippingPlane table and returns
  //! its label (returns existing label if the same clipping plane
  //! is already defined)
  Standard_EXPORT TDF_Label AddClippingPlane(const gp_Pln thePlane, const Handle(TCollection_HAsciiString)& theName) const;
  
  //! Removes clipping plane from the ClippingPlane table
  //! Return false and do nothing if clipping plane is referenced in at least one View
  Standard_EXPORT Standard_Boolean RemoveClippingPlane(const TDF_Label& theLabel) const;
  
  //! Returns a sequence of clipping planes currently stored
  //! in the ClippingPlane table
  Standard_EXPORT void GetClippingPlanes(TDF_LabelSequence& Labels) const;
  
  //! Sets new value of plane and name to the given clipping plane label
  //! or do nothing, if the given label is not a clipping plane label
  Standard_EXPORT void UpdateClippingPlane(const TDF_Label& theLabelL, const gp_Pln thePlane, const TCollection_ExtendedString theName) const;

  //! Set new value of capping for given clipping plane label
  Standard_EXPORT void SetCapping(const TDF_Label& theClippingPlaneL, const Standard_Boolean theCapping);

  //! Get capping value for given clipping plane label
  //! Return capping value
  Standard_EXPORT Standard_Boolean GetCapping(const TDF_Label& theClippingPlaneL) const;

  //! Get capping value for given clipping plane label
  //! Return true if Label is valid abd capping is exist.
  Standard_EXPORT Standard_Boolean GetCapping(const TDF_Label& theClippingPlaneL, Standard_Boolean &theCapping) const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XCAFDoc_ClippingPlaneTool, TDF_Attribute)

};
#endif // _XCAFDoc_ClippingPlaneTool_HeaderFile
