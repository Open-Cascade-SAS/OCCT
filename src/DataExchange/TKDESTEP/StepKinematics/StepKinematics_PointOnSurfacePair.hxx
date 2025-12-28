// Created on : Sat May 02 12:41:15 2020
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#ifndef _StepKinematics_PointOnSurfacePair_HeaderFile_
#define _StepKinematics_PointOnSurfacePair_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_HighOrderKinematicPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepGeom_Surface.hxx>

//! Representation of STEP entity PointOnSurfacePair
class StepKinematics_PointOnSurfacePair : public StepKinematics_HighOrderKinematicPair
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_PointOnSurfacePair();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
    const bool                       hasItemDefinedTransformation_Description,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
    const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
    const occ::handle<StepGeom_Surface>&              thePairSurface);

  //! Returns field PairSurface
  Standard_EXPORT occ::handle<StepGeom_Surface> PairSurface() const;
  //! Sets field PairSurface
  Standard_EXPORT void SetPairSurface(const occ::handle<StepGeom_Surface>& thePairSurface);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_PointOnSurfacePair, StepKinematics_HighOrderKinematicPair)

private:
  occ::handle<StepGeom_Surface> myPairSurface;
};
#endif // _StepKinematics_PointOnSurfacePair_HeaderFile_
