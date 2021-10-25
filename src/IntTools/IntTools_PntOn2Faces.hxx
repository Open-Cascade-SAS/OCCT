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

#ifndef _IntTools_PntOn2Faces_HeaderFile
#define _IntTools_PntOn2Faces_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <IntTools_PntOnFace.hxx>


//! Contains two points PntOnFace from IntTools and a flag
class IntTools_PntOn2Faces 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Empty constructor
  Standard_EXPORT IntTools_PntOn2Faces();
  

  //! Initializes me by two points aP1 and aP2
  Standard_EXPORT IntTools_PntOn2Faces(const IntTools_PntOnFace& aP1, const IntTools_PntOnFace& aP2);
  

  //! Modifier
  Standard_EXPORT void SetP1 (const IntTools_PntOnFace& aP1);
  

  //! Modifier
  Standard_EXPORT void SetP2 (const IntTools_PntOnFace& aP2);
  

  //! Modifier
  Standard_EXPORT void SetValid (const Standard_Boolean bF);
  

  //! Selector
  Standard_EXPORT const IntTools_PntOnFace& P1() const;
  

  //! Selector
  Standard_EXPORT const IntTools_PntOnFace& P2() const;
  

  //! Selector
  Standard_EXPORT Standard_Boolean IsValid() const;




protected:





private:



  Standard_Boolean myIsValid;
  IntTools_PntOnFace myPnt1;
  IntTools_PntOnFace myPnt2;


};







#endif // _IntTools_PntOn2Faces_HeaderFile
