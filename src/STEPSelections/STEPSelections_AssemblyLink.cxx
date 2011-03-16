// File:	STEPSelections_AssemblyLink.cxx
// Created:	Wed Mar 24 16:17:01 1999
// Author:	data exchange team
//		<det@friendox.nnov.matra-dtv.fr>


#include <STEPSelections_AssemblyLink.ixx>

STEPSelections_AssemblyLink::STEPSelections_AssemblyLink()
{
}

STEPSelections_AssemblyLink::STEPSelections_AssemblyLink(const Handle(StepRepr_NextAssemblyUsageOccurrence)& nauo,
						       const Handle(Standard_Transient)& item,
						       const Handle(STEPSelections_AssemblyComponent)& part)
{
  myNAUO = nauo;
  myItem = item;
  myComponent = part;
}
