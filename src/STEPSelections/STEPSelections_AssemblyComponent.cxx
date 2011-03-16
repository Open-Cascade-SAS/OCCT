// File:	STEPSelections_AssemblyComponent.cxx
// Created:	Wed Mar 24 16:03:32 1999
// Author:	data exchange team
//		<det@friendox.nnov.matra-dtv.fr>


#include <STEPSelections_AssemblyComponent.ixx>

STEPSelections_AssemblyComponent::STEPSelections_AssemblyComponent() 
{
}

STEPSelections_AssemblyComponent::STEPSelections_AssemblyComponent(const Handle(StepShape_ShapeDefinitionRepresentation)& sdr,
								 const Handle(STEPSelections_HSequenceOfAssemblyLink)& list)
{
  mySDR = sdr;
  myList = list;
}
