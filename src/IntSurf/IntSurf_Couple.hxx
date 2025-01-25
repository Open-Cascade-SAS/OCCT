// Created on: 1992-03-25
// Created by: Isabelle GRIGNON
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntSurf_Couple_HeaderFile
#define _IntSurf_Couple_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>

//! creation d 'un couple de 2 entiers
class IntSurf_Couple
{
public:
  DEFINE_STANDARD_ALLOC

  IntSurf_Couple();

  IntSurf_Couple(const Standard_Integer Index1, const Standard_Integer Index2);

  //! returns the first element
  Standard_Integer First() const;

  //! returns the Second element
  Standard_Integer Second() const;

protected:
private:
  Standard_Integer firstInteger;
  Standard_Integer secondInteger;
};

#include <IntSurf_Couple.lxx>

#endif // _IntSurf_Couple_HeaderFile
