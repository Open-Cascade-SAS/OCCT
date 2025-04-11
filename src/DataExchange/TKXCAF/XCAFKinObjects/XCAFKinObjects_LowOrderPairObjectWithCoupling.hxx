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

#ifndef _XCAFKinObjects_LowOrderPairObjectWithCoupling_HeaderFile
#define _XCAFKinObjects_LowOrderPairObjectWithCoupling_HeaderFile

#include <XCAFKinObjects_PairObject.hxx>
#include <TColStd_HArray1OfReal.hxx>

DEFINE_STANDARD_HANDLE(XCAFKinObjects_LowOrderPairObjectWithCoupling, XCAFKinObjects_PairObject)

//! \class XCAFKinObjects_LowOrderPairObjectWithCoupling
//! \brief Object for low order kinematic pairs with motion coupling:
//! - XCAFKinObjects_PairType_Screw
//! - XCAFKinObjects_PairType_RackAndPinion
//! - XCAFKinObjects_PairType_Gear
//! - XCAFKinObjects_PairType_LinearFlexibleAndPinion
class XCAFKinObjects_LowOrderPairObjectWithCoupling : public XCAFKinObjects_PairObject
{

public:
  Standard_EXPORT XCAFKinObjects_LowOrderPairObjectWithCoupling();

  Standard_EXPORT XCAFKinObjects_LowOrderPairObjectWithCoupling(
    const Handle(XCAFKinObjects_LowOrderPairObjectWithCoupling)& theObj);

  Standard_EXPORT void SetType(const XCAFKinObjects_PairType theType) Standard_OVERRIDE;

  void SetLowLimit(const Standard_Real theLimit)
  {
    myIsRanged = Standard_True;
    myLowLimit = theLimit;
  }

  Standard_Real LowLimit() const { return myLowLimit; }

  void SetUpperLimit(const Standard_Real theLimit)
  {
    myIsRanged   = Standard_True;
    myUpperLimit = theLimit;
  }

  Standard_Real UpperLimit() const { return myUpperLimit; }

  //! Creates array with all limits
  //! \return created array
  Standard_EXPORT Handle(TColStd_HArray1OfReal) GetAllLimits() const Standard_OVERRIDE;

  //! Sets pitch parameter (only for Screw)
  //! \param[in] thePitch parameter value
  Standard_EXPORT void SetPitch(const Standard_Real thePitch);

  //! Gets pitch parameter (only for Screw)
  //! \return parameter value
  Standard_EXPORT Standard_Real Pitch() const;

  //! Sets pinion radius parameter (only for RackAndPinion)
  //! \param[in] theRadius parameter value
  Standard_EXPORT void SetPinionRadius(const Standard_Real theRadius);

  //! Gets pinion radius parameter (only for RackAndPinion)
  //! \return parameter value
  Standard_EXPORT Standard_Real PinionRadius() const;

  //! Sets first link radius parameter (only for Gear)
  //! \param[in] theRadius parameter value
  Standard_EXPORT void SetRadiusFirstLink(const Standard_Real theRadius);

  //! Gets first link radius parameter (only for Gear)
  //! \return parameter value
  Standard_EXPORT Standard_Real RadiusFirstLink() const;

  //! Sets second link radius parameter (only for Gear)
  //! \param[in] theRadius parameter value
  Standard_EXPORT void SetRadiusSecondLink(const Standard_Real theRadius);

  //! Gets second link radius parameter (only for Gear)
  //! \return parameter value
  Standard_EXPORT Standard_Real RadiusSecondLink() const;

  //! Sets bevel parameter (only for Gear)
  //! \param[in] theBevel parameter value
  Standard_EXPORT void SetBevel(const Standard_Real theBevel);

  //! Gets bevel parameter (only for Gear)
  //! \return parameter value
  Standard_EXPORT Standard_Real Bevel() const;

  //! Sets helical angle parameter (only for Gear)
  //! \param[in] theAngle parameter value
  Standard_EXPORT void SetHelicalAngle(const Standard_Real theAngle);

  //! Gets helical angle parameter (only for Gear)
  //! \return parameter value
  Standard_EXPORT Standard_Real HelicalAngle() const;

  //! Sets gear ratio parameter (only for Gear)
  //! \param[in] theGearRatio parameter value
  Standard_EXPORT void SetGearRatio(const Standard_Real theGearRatio);

  //! Gets gear ratio parameter (only for Gear)
  //! \return parameter value
  Standard_EXPORT Standard_Real GearRatio() const;

  //! Sets all parameters as an array
  //! \param[in] theParams array of parameters
  Standard_EXPORT void SetAllParams(const Handle(TColStd_HArray1OfReal)& theParams);

  Handle(TColStd_HArray1OfReal) GetAllParams() const { return myParams; }

  Standard_EXPORT Standard_Boolean HasLimits() const Standard_OVERRIDE;

  Standard_EXPORT void SetAllLimits(const Handle(TColStd_HArray1OfReal)& theLimits)
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(XCAFKinObjects_LowOrderPairObjectWithCoupling, XCAFKinObjects_PairObject)

private:
  Standard_Real                 myLowLimit;   //!< low limit of motion range
  Standard_Real                 myUpperLimit; //!< upper limit of motion range
  Handle(TColStd_HArray1OfReal) myParams;     //!< additional parameters of kinematic pair
  Standard_Boolean              myIsRanged;   //!< flag "is limits defined"
};

#endif // _XCAFKinObjects_LowOrderPairObjectWithCoupling_HeaderFile
