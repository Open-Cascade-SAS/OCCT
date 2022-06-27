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

#ifndef _StepKinematics_KinematicLinkRepresentation_HeaderFile_
#define _StepKinematics_KinematicLinkRepresentation_HeaderFile_

#include <Standard.hxx>
#include <StepRepr_Representation.hxx>

#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepKinematics_KinematicLink.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_KinematicLinkRepresentation, StepRepr_Representation)

//! Representation of STEP entity KinematicLinkRepresentation
class StepKinematics_KinematicLinkRepresentation : public StepRepr_Representation
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_KinematicLinkRepresentation();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentation_Name,
                           const Handle(StepRepr_HArray1OfRepresentationItem)& theRepresentation_Items,
                           const Handle(StepRepr_RepresentationContext)& theRepresentation_ContextOfItems,
                           const Handle(StepKinematics_KinematicLink)& theRepresentedLink);

  //! Returns field RepresentedLink
  Standard_EXPORT Handle(StepKinematics_KinematicLink) RepresentedLink() const;
  //! Sets field RepresentedLink
  Standard_EXPORT void SetRepresentedLink (const Handle(StepKinematics_KinematicLink)& theRepresentedLink);

DEFINE_STANDARD_RTTIEXT(StepKinematics_KinematicLinkRepresentation, StepRepr_Representation)

private:
  Handle(StepKinematics_KinematicLink) myRepresentedLink;

};
#endif // _StepKinematics_KinematicLinkRepresentation_HeaderFile_
