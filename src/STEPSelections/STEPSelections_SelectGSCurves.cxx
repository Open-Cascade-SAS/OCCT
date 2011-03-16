// File:	StepSelect_STEPGSCurves.cxx
// Created:	Mon Mar 22 18:01:05 1999
// Author:	data exchange team
//		<det@friendox.nnov.matra-dtv.fr>


#include <STEPSelections_SelectGSCurves.ixx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_CompositeCurve.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepGeom_CompositeCurveSegment.hxx>

static Standard_Integer flag;

STEPSelections_SelectGSCurves::STEPSelections_SelectGSCurves():IFSelect_SelectExplore (-1){ flag = 1;}
     
Standard_Boolean STEPSelections_SelectGSCurves::Explore(const Standard_Integer level,
						       const Handle(Standard_Transient)& start,
						       const Interface_Graph& G,
						       Interface_EntityIterator& explored) const
{
  if(start.IsNull()) return Standard_False;
  
  if (start->IsKind(STANDARD_TYPE(StepGeom_Curve))) {
    if(start->IsKind(STANDARD_TYPE(StepGeom_CompositeCurve))) {
      Interface_EntityIterator subs = G.Sharings(start);
      Standard_Boolean isInGeomSet = Standard_False;
      for (subs.Start(); subs.More()&&!isInGeomSet; subs.Next()) 
	if(subs.Value()->IsKind(STANDARD_TYPE(StepShape_GeometricSet))){
	  if(flag) {
	    explored.AddItem (subs.Value());
	    flag =0;
	  }
	  isInGeomSet = Standard_True; 
	}
      if(isInGeomSet) {
	Interface_EntityIterator subs = G.Shareds(start);
	subs.Start();
	Standard_Boolean isSome = subs.More();
	for (; subs.More(); subs.Next()) 
	  explored.AddItem (subs.Value());
	return isSome;
      } else
	return Standard_False;
    } else {
      Interface_EntityIterator subs = G.Sharings(start);
      for (subs.Start(); subs.More(); subs.Next()) {
	if(subs.Value()->IsKind(STANDARD_TYPE(StepShape_GeometricSet))||
	   subs.Value()->IsKind(STANDARD_TYPE(StepGeom_CompositeCurveSegment)))
	  return Standard_True;
      }
    }
  }
  
  Interface_EntityIterator subs = G.Shareds(start);
  subs.Start();
  Standard_Boolean isSome = subs.More();
  for (; subs.More(); subs.Next()) 
    explored.AddItem (subs.Value());
  
  return isSome;
}

TCollection_AsciiString STEPSelections_SelectGSCurves::ExploreLabel() const
{
  return TCollection_AsciiString ("Curves in GS");
}
