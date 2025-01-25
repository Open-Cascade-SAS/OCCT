// Created on: 2001-12-13
// Created by: Peter KURNEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <IntTools_PntOn2Faces.hxx>
#include <IntTools_PntOnFace.hxx>

//=================================================================================================

IntTools_PntOn2Faces::IntTools_PntOn2Faces()
    : myIsValid(Standard_False)
{
}

//=================================================================================================

IntTools_PntOn2Faces::IntTools_PntOn2Faces(const IntTools_PntOnFace& aP1,
                                           const IntTools_PntOnFace& aP2)
    : myIsValid(Standard_False)
{
  myPnt1 = aP1;
  myPnt2 = aP2;
}

//=================================================================================================

void IntTools_PntOn2Faces::SetP1(const IntTools_PntOnFace& aP)
{
  myPnt1 = aP;
}

//=================================================================================================

void IntTools_PntOn2Faces::SetP2(const IntTools_PntOnFace& aP)
{
  myPnt2 = aP;
}

//=================================================================================================

const IntTools_PntOnFace& IntTools_PntOn2Faces::P1() const
{
  return myPnt1;
}

//=================================================================================================

const IntTools_PntOnFace& IntTools_PntOn2Faces::P2() const
{
  return myPnt2;
}

//=================================================================================================

void IntTools_PntOn2Faces::SetValid(const Standard_Boolean bF)
{
  myIsValid = bF;
}

//=================================================================================================

Standard_Boolean IntTools_PntOn2Faces::IsValid() const
{
  return myIsValid;
}
