// Created on: 2020-03-16
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

#ifndef _XCAFKinObjects_PairObject_HeaderFile
#define _XCAFKinObjects_PairObject_HeaderFile

#include <gp_Ax3.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <XCAFKinObjects_PairType.hxx>

DEFINE_STANDARD_HANDLE(XCAFKinObjects_PairObject, Standard_Transient)

//! \class XCAFKinObjects_PairObject
//! \brief Main parent object for kinematic pairs
class XCAFKinObjects_PairObject : public Standard_Transient
{

public:
  Standard_EXPORT XCAFKinObjects_PairObject();

  Standard_EXPORT XCAFKinObjects_PairObject(const Handle(XCAFKinObjects_PairObject)& theObj);

  void SetName(const TCollection_AsciiString& theName) { myName = theName; }

  TCollection_AsciiString Name() const { return myName; }

  void virtual SetType(const XCAFKinObjects_PairType theType) { myType = theType; }

  XCAFKinObjects_PairType Type() const { return myType; }

  void SetFirstTransformation(const gp_Ax3& theTrsf) { myTrsf1 = theTrsf; }

  gp_Ax3 FirstTransformation() const { return myTrsf1; }

  void SetSecondTransformation(const gp_Ax3& theTrsf) { myTrsf2 = theTrsf; }

  gp_Ax3 SecondTransformation() const { return myTrsf2; }

  Standard_EXPORT virtual void SetAllLimits(const Handle(TColStd_HArray1OfReal)& /*theLimits*/) {};

  Standard_EXPORT virtual Standard_Boolean HasLimits() const { return Standard_False; };

  virtual Handle(TColStd_HArray1OfReal) GetAllLimits() const { return NULL; }

  DEFINE_STANDARD_RTTIEXT(XCAFKinObjects_PairObject, Standard_Transient)

private:
  TCollection_AsciiString myName;  //!< name of kinematic pair
  XCAFKinObjects_PairType     myType;  //!< type of kinematic pair
  gp_Ax3                  myTrsf1; //!< first transformation element
  gp_Ax3                  myTrsf2; //!< second transformation element
};

#endif // _XCAFKinObjects_PairObject_HeaderFile
