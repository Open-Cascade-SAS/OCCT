// File:	AIS_NListTransient.hxx
// Created:	Sun May 04 18:13:54 2003
// Author:      Alexander Grigoriev (a-grigoriev@opencascade.com)

#ifndef _AIS_NListTransient_HeaderFile
#define _AIS_NListTransient_HeaderFile

#include <Standard_Transient.hxx>
#include <NCollection_StdBase.hxx>
#include <NCollection_DefineList.hxx>

DEFINE_LIST   (AIS_NListTransient,
               NCollection_BaseCollTransient,
               Handle(Standard_Transient))

#endif
