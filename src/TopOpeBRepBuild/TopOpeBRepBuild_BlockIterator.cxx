// Created on: 1993-03-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Standard_NoMoreObject.hxx>
#include <TopOpeBRepBuild_BlockIterator.hxx>

//=======================================================================
//function : TopOpeBRepBuild_BlockIterator
//purpose  : 
//=======================================================================
TopOpeBRepBuild_BlockIterator::TopOpeBRepBuild_BlockIterator() :
myLower(0),
myUpper(0),
myValue(1)
{
}

//=======================================================================
//function : TopOpeBRepBuild_BlockIterator
//purpose  : 
//=======================================================================

TopOpeBRepBuild_BlockIterator::TopOpeBRepBuild_BlockIterator
(const Standard_Integer Lower, const Standard_Integer Upper) :
myLower(Lower),
myUpper(Upper),
myValue(Lower)
{
}

