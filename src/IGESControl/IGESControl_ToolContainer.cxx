// File:	IGESControl_ToolContainer.cxx
// Created:	Tue Feb  8 09:30:22 2000
// Author:	data exchange team
//		<det@kinox>


#include <IGESControl_ToolContainer.ixx>
#include <IGESControl_IGESBoundary.hxx>

//=======================================================================
//function : IGESControl_ToolContainer
//purpose  : 
//=======================================================================

IGESControl_ToolContainer::IGESControl_ToolContainer():IGESToBRep_ToolContainer()
{
}

//=======================================================================
//function : IGESBoundary
//purpose  : 
//=======================================================================

Handle(IGESToBRep_IGESBoundary) IGESControl_ToolContainer::IGESBoundary() const
{
  return new IGESControl_IGESBoundary;
}
