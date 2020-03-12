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

#include <StepKinematics_KinematicLinkRepresentation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_KinematicLinkRepresentation, StepRepr_Representation)

//=======================================================================
//function : StepKinematics_KinematicLinkRepresentation
//purpose  :
//=======================================================================
StepKinematics_KinematicLinkRepresentation::StepKinematics_KinematicLinkRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_KinematicLinkRepresentation::Init (const Handle(TCollection_HAsciiString)& theRepresentation_Name,
                                                       const Handle(StepRepr_HArray1OfRepresentationItem)& theRepresentation_Items,
                                                       const Handle(StepRepr_RepresentationContext)& theRepresentation_ContextOfItems,
                                                       const Handle(StepKinematics_KinematicLink)& theRepresentedLink)
{
  StepRepr_Representation::Init(theRepresentation_Name,
                                theRepresentation_Items,
                                theRepresentation_ContextOfItems);

  myRepresentedLink = theRepresentedLink;
}

//=======================================================================
//function : RepresentedLink
//purpose  :
//=======================================================================
Handle(StepKinematics_KinematicLink) StepKinematics_KinematicLinkRepresentation::RepresentedLink () const
{
  return myRepresentedLink;
}

//=======================================================================
//function : SetRepresentedLink
//purpose  :
//=======================================================================
void StepKinematics_KinematicLinkRepresentation::SetRepresentedLink (const Handle(StepKinematics_KinematicLink)& theRepresentedLink)
{
  myRepresentedLink = theRepresentedLink;
}
