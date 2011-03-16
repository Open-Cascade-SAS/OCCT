// File:      NIS_SelectFilter.cxx
// Created:   20.08.07 14:04
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#include <NIS_SelectFilter.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_SelectFilter, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT (NIS_SelectFilter, Standard_Transient)

//=======================================================================
//function : ActsOn
//purpose  : 
//=======================================================================

Standard_Boolean NIS_SelectFilter::ActsOn (const Handle_Standard_Type&) const
{
  return Standard_True;
}


