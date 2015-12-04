// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <IGESData_LevelListEntity.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESData_LevelListEntity,IGESData_IGESEntity)

// LevelListEntity ne sert qu'au controle de type (pour le directory part)
Standard_Boolean  IGESData_LevelListEntity::HasLevelNumber
  (const Standard_Integer level) const
{
  Standard_Integer nb = NbLevelNumbers();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (LevelNumber(i) == level) return Standard_True;
  }
  return Standard_False;
}

