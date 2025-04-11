// Created on: 2020-03-30
// Created by: Irina KRYLOVA
// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _XCAFKinObjects_KinematicPairValue_HeaderFile
#define _XCAFKinObjects_KinematicPairValue_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;
class XCAFKinObjects_PairValueObject;

// resolve name collisions with WinAPI headers
#ifdef GetObject
  #undef GetObject
#endif

class XCAFKinObjects_KinematicPairValue;
DEFINE_STANDARD_HANDLE(XCAFKinObjects_KinematicPairValue, TDF_Attribute)

//! Attribute to store kinematic pair value.
//! This parameter is an additional attribute for kinematic pair attribute,
//! which characterizes the pair for one of the mechanism states.
//! It is prohibited to store inconsistent data:
//! - to store this attribute not of the same type as the type of linked the pair.
//!
//! The attribute itsels does not contain any data only a way to store/collect
//! necessary standard attributes from the label, so the attribute
//! does not stored to OCAF document only a UAttribute with special GUID.
//! Use static IsValidLabel() method to check if the label contains this attribute
//! instead of common Label.FindAttribute().
class XCAFKinObjects_KinematicPairValue : public TDF_Attribute
{

public:
  Standard_EXPORT XCAFKinObjects_KinematicPairValue();

  Standard_EXPORT static const Standard_GUID& GetID();

  //! Checks if the given label has UAttribute with the necessary GUID.
  //! param[in] theLabel - label to check.
  //! \return result of check.
  Standard_EXPORT static Standard_Boolean IsValidLabel(const TDF_Label& theLabel);

  Standard_EXPORT static Handle(XCAFKinObjects_KinematicPairValue) Set(const TDF_Label& theValue,
                                                                   const TDF_Label& theJoint);

  Standard_EXPORT static Handle(XCAFKinObjects_KinematicPairValue) Set(const TDF_Label& theValue);

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theWith) Standard_OVERRIDE;

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theInfo,
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;

  //! Updates parent's label and its sub-labels with data taken from thePairValueObject.
  //! Old data associated with the label will be lost.
  Standard_EXPORT void SetObject(const Handle(XCAFKinObjects_PairValueObject)& thePairValueObject);

  //! Returns kinematic pair value object data taken from the parent's label and its sub-labels.
  Standard_EXPORT Handle(XCAFKinObjects_PairValueObject) GetObject() const;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

private:
  //! Special GUID of UAttribute to indicate that this labeland its children contains
  //! XCAFKinObjects_KinematicPairValue attribute.
  Standard_EXPORT static const Standard_GUID& getUID();

  DEFINE_STANDARD_RTTIEXT(XCAFKinObjects_KinematicPairValue, TDF_Attribute)
};

#endif
