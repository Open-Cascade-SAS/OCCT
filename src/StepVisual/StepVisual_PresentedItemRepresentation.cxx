// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepVisual_PresentedItemRepresentation.ixx>

StepVisual_PresentedItemRepresentation::StepVisual_PresentedItemRepresentation  ()    {  }

void  StepVisual_PresentedItemRepresentation::Init
  (const StepVisual_PresentationRepresentationSelect& aPresentation,
   const Handle(StepVisual_PresentedItem)& aItem)
{  thePresentation = aPresentation;  theItem = aItem;  }

void  StepVisual_PresentedItemRepresentation::SetPresentation (const StepVisual_PresentationRepresentationSelect& aPresentation)
{  thePresentation = aPresentation;  }

StepVisual_PresentationRepresentationSelect  StepVisual_PresentedItemRepresentation::Presentation () const
{  return thePresentation;  }

void  StepVisual_PresentedItemRepresentation::SetItem (const Handle(StepVisual_PresentedItem)& aItem)
{  theItem = aItem;  }

Handle(StepVisual_PresentedItem)  StepVisual_PresentedItemRepresentation::Item () const
{  return theItem;  }
