// File:	StepSelect_SelectFaces.cxx
// Created:	Thu Feb 11 14:39:20 1999
// Author:	Pavel DURANDIN
//		<pdn@friendox.nnov.matra-dtv.fr>


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
