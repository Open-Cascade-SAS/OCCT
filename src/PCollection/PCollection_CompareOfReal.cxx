// Created on: 1992-08-27
// Created by: Mireille MERCIEN
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Do not delete this line. Used by sccs.

#include <PCollection_CompareOfReal.ixx>

// -----------
// Create :
// -----------
PCollection_CompareOfReal::PCollection_CompareOfReal()
{
}

// -----------
// IsLower :
// -----------
Standard_Boolean PCollection_CompareOfReal::IsLower (
       const Standard_Real &Left,const Standard_Real &Right) const
{
   return (Left < Right) ;
}

// -----------
// IsGreater :
// -----------
Standard_Boolean PCollection_CompareOfReal::IsGreater (
       const Standard_Real &Left,const Standard_Real &Right) const
{
   return (Left > Right) ;
}						





