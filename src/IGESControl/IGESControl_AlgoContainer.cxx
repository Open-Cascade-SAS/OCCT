// File:	IGESControl_AlgoContainer.cxx
// Created:	Tue Feb  8 09:29:29 2000
// Author:	data exchange team
//		<det@kinox>


#include <IGESControl_AlgoContainer.ixx>
#include <IGESControl_ToolContainer.hxx>

//=======================================================================
//function : IGESControl_AlgoContainer
//purpose  : 
//=======================================================================

IGESControl_AlgoContainer::IGESControl_AlgoContainer():IGESToBRep_AlgoContainer()
{
  SetToolContainer (new IGESControl_ToolContainer);
}

