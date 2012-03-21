// Created on: 1999-02-11
// Created by: Pavel DURANDIN
// Copyright (c) 1999-1999 Matra Datavision
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



#include <STEPSelections_SelectFaces.ixx>
#include <StepShape_FaceSurface.hxx>
#include <StepGeom_Surface.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepGeom_Surface.hxx>


STEPSelections_SelectFaces::STEPSelections_SelectFaces():IFSelect_SelectExplore (-1){ }

Standard_Boolean STEPSelections_SelectFaces::Explore(const Standard_Integer level,
						    const Handle(Standard_Transient)& start,
						    const Interface_Graph& G,
						    Interface_EntityIterator& explored) const
{
  if(start.IsNull()) return Standard_False;
  
  if (start->IsKind(STANDARD_TYPE(StepShape_FaceSurface))) return Standard_True;
  
  Standard_Boolean isInFaceOfInSurface = Standard_False;
  if (start->IsKind(STANDARD_TYPE(StepGeom_Surface))) {
    Interface_EntityIterator subs = G.Sharings(start);
    for (subs.Start(); subs.More(); subs.Next()) {
      if(subs.Value()->IsKind(STANDARD_TYPE(StepShape_GeometricSet)))
	return Standard_True;
      if(subs.Value()->IsKind(STANDARD_TYPE(StepGeom_Surface)))
	isInFaceOfInSurface = Standard_True;
      if(subs.Value()->IsKind(STANDARD_TYPE(StepShape_FaceSurface)))
	isInFaceOfInSurface = Standard_True;
    }
    return !isInFaceOfInSurface;
  }
  Interface_EntityIterator subs = G.Shareds(start);
  subs.Start();
  Standard_Boolean isSome = subs.More();
  for (; subs.More(); subs.Next()) 
    explored.AddItem (subs.Value());
  
  return isSome;
}

TCollection_AsciiString STEPSelections_SelectFaces::ExploreLabel() const
{
  return TCollection_AsciiString ("Faces");
}
