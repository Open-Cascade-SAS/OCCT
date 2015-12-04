// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_LabelIterator.hxx>


IMPLEMENT_STANDARD_RTTIEXT(TObj_LabelIterator,TObj_ObjectIterator)

//=======================================================================
//function : TObj_LabelIterator
//purpose  : 
//=======================================================================

TObj_LabelIterator::TObj_LabelIterator()
{
}
     
//=======================================================================
//function : TObj_LabelIterator
//purpose  : 
//=======================================================================

TObj_LabelIterator::TObj_LabelIterator(const TDF_Label& theLabel,
                                               const Standard_Boolean isRecursive)
{
  Init(theLabel,isRecursive);
}
     
//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TObj_LabelIterator::Next()
{
  myObject.Nullify();
  myNode.Nullify();
  MakeStep();
}
