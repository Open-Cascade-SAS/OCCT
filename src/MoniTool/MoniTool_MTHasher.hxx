// Created on: 2001-12-19
// Created by: Sergey KUUL
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

#ifndef _MoniTool_MTHasher_HeaderFile
#define _MoniTool_MTHasher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>



//! The auxiliary class provides hash code for mapping objects
class MoniTool_MTHasher 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Returns hash code for the given string, in the range [1, theUpperBound]
  //! @param theString the string which hash code is to be computed
  //! @param theUpperBound the upper bound of the range a computing hash code must be within
  //! @return a computed hash code, in the range [1, theUpperBound]
  static Standard_Integer HashCode (Standard_CString theString, Standard_Integer theUpperBound);

  //! Returns True  when the two CString are the same. Two
  //! same strings must have the same hashcode, the
  //! contrary is not necessary.
  //! Default Str1 == Str2
    static Standard_Boolean IsEqual (const Standard_CString Str1, const Standard_CString Str2);




protected:





private:





};


#include <MoniTool_MTHasher.lxx>





#endif // _MoniTool_MTHasher_HeaderFile
