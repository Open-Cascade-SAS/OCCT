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

//! Base class for kinematic pair objects that represent mechanical joints or constraints
//! between two rigid bodies in a kinematic system.
//!
//! This class provides common properties and methods for all types of kinematic pairs,
//! including identification, transformations, and limit management. Specific joint types
//! are implemented in derived classes:
//! - XCAFKinObjects_LowOrderPairObject: For standard joints with simple degrees of freedom
//! - XCAFKinObjects_LowOrderPairObjectWithCoupling: For joints with coupled motion
//! - XCAFKinObjects_HighOrderPairObject: For advanced joints involving curves and surfaces
//!
//! Each kinematic pair defines the relative position and permissible motion between
//! two linked components through their transformation coordinate systems.
class XCAFKinObjects_PairObject : public Standard_Transient
{

public:
  //! Default constructor.
  //! Creates an empty kinematic pair object with default transformation systems.
  Standard_EXPORT XCAFKinObjects_PairObject();

  //! Copy constructor.
  //! @param theObj Source kinematic pair object to copy from
  Standard_EXPORT XCAFKinObjects_PairObject(const Handle(XCAFKinObjects_PairObject)& theObj);

  //! Sets the name of the kinematic pair.
  //! @param theName Name to assign to this kinematic pair
  void SetName(const TCollection_AsciiString& theName) { myName = theName; }

  //! Returns the name of the kinematic pair.
  //! @return Current name of the kinematic pair
  TCollection_AsciiString Name() const { return myName; }

  //! Sets the type of the kinematic pair.
  //! This method is virtual and may be overridden in derived classes
  //! to provide additional type-specific initialization.
  //! @param theType Type of kinematic pair to set
  void virtual SetType(const XCAFKinObjects_PairType theType) { myType = theType; }

  //! Returns the type of the kinematic pair.
  //! @return Type of kinematic pair as an enumeration value
  XCAFKinObjects_PairType Type() const { return myType; }

  //! Sets the coordinate system of the first connected component.
  //! This coordinate system defines the position and orientation of the first component
  //! at the joint location, and serves as a reference for motion constraints.
  //! @param theTrsf Coordinate system to set
  void SetFirstTransformation(const gp_Ax3& theTrsf) { myTrsf1 = theTrsf; }

  //! Returns the coordinate system of the first connected component.
  //! @return Coordinate system of the first component
  gp_Ax3 FirstTransformation() const { return myTrsf1; }

  //! Sets the coordinate system of the second connected component.
  //! This coordinate system defines the position and orientation of the second component
  //! at the joint location, and serves as a reference for motion constraints.
  //! @param theTrsf Coordinate system to set
  void SetSecondTransformation(const gp_Ax3& theTrsf) { myTrsf2 = theTrsf; }

  //! Returns the coordinate system of the second connected component.
  //! @return Coordinate system of the second component
  gp_Ax3 SecondTransformation() const { return myTrsf2; }

  //! Sets all limit values for the kinematic pair from an array.
  //! This is a virtual method that must be implemented in derived classes
  //! to handle pair-specific limit structures.
  //! @param theLimits Array containing limit values
  Standard_EXPORT virtual void SetAllLimits(const Handle(TColStd_HArray1OfReal)& /*theLimits*/) {};

  //! Checks if the kinematic pair has defined limit values.
  //! This is a virtual method that should be implemented in derived classes
  //! to handle pair-specific limit checking.
  //! @return true if limits are defined, false otherwise
  Standard_EXPORT virtual Standard_Boolean HasLimits() const { return Standard_False; };

  //! Returns all limit values for the kinematic pair.
  //! This is a virtual method that should be implemented in derived classes
  //! to return their specific limit values.
  //! @return Array containing all limit values or NULL if no limits defined
  virtual Handle(TColStd_HArray1OfReal) GetAllLimits() const { return NULL; }

  DEFINE_STANDARD_RTTIEXT(XCAFKinObjects_PairObject, Standard_Transient)

private:
  TCollection_AsciiString myName;  //!< Name of the kinematic pair for identification
  XCAFKinObjects_PairType myType;  //!< Type of the kinematic pair defining its behavior
  gp_Ax3                  myTrsf1; //!< Coordinate system of the first connected component
  gp_Ax3                  myTrsf2; //!< Coordinate system of the second connected component
};

#endif // _XCAFKinObjects_PairObject_HeaderFile
