// File:      TObj_HiddenPartition.cxx
// Created:   Tue Nov 23 11:39:12 2004
// Author:    Pavel TELKOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_HiddenPartition.hxx>

IMPLEMENT_STANDARD_HANDLE(TObj_HiddenPartition, TObj_Partition)
IMPLEMENT_STANDARD_RTTIEXT(TObj_HiddenPartition, TObj_Partition)
IMPLEMENT_TOBJOCAF_PERSISTENCE(TObj_HiddenPartition)
  
//=======================================================================
//function : TObj_HiddenPartition
//purpose  : constructor
//=======================================================================

Standard_EXPORT TObj_HiddenPartition::TObj_HiddenPartition
                                             (const TDF_Label& theLabel):
  TObj_Partition(theLabel)
{
}

//=======================================================================
//function : GetTypeFlags
//purpose  : 
//=======================================================================

Standard_Integer TObj_HiddenPartition::GetTypeFlags() const
{
  return TObj_Partition::GetTypeFlags() & (~Visible);
}
