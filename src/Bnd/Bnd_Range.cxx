// Created on: 2016-06-07
// Created by: Nikolai BUKHALOV
// Copyright (c) 2016 OPEN CASCADE SAS
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


#include <Bnd_Range.hxx>

//=======================================================================
//function : Common
//purpose  : 
//=======================================================================
void Bnd_Range::Common(const Bnd_Range& theOther)
{
  if(theOther.IsVoid())
  {
    SetVoid();
  }

  if(IsVoid())
  {
    return;
  }

  myFirst = Max(myFirst, theOther.myFirst);
  myLast = Min(myLast, theOther.myLast);
}
