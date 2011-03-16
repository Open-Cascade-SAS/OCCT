// File:	IGESToBRep_ToolContainer.cxx
// Created:	Mon Feb  7 13:08:56 2000
// Author:	data exchange team
//		<det@kinox>


#include <IGESToBRep_ToolContainer.ixx>

//=======================================================================
//function : IGESToBRep_ToolContainer
//purpose  : 
//=======================================================================

IGESToBRep_ToolContainer::IGESToBRep_ToolContainer()
{
}

//=======================================================================
//function : IGESBoundary
//purpose  : 
//=======================================================================

Handle(IGESToBRep_IGESBoundary) IGESToBRep_ToolContainer::IGESBoundary() const
{
  return new IGESToBRep_IGESBoundary;
}
