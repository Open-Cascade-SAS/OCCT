// File:	Standard_MMgrRoot.cxx
// Created:	Wed Mar 23 09:55:03 2005
// Author:	
//		<pkv@ALEX>


#include <Standard_MMgrRoot.hxx>
#include <stdlib.h>

//=======================================================================
//function : ~Standard_MMgrRoot
//purpose  : 
//=======================================================================

Standard_MMgrRoot::~Standard_MMgrRoot()
{
}

//=======================================================================
//function : Purge
//purpose  : 
//=======================================================================

Standard_Integer Standard_MMgrRoot::Purge(Standard_Boolean)
{
  return 0;
}

//=======================================================================
//function : SetReentrant
//purpose  : 
//=======================================================================

void Standard_MMgrRoot::SetReentrant(Standard_Boolean)
{
}
