// Created on: 1994-11-04
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _MoniTool_ElemHasher_HeaderFile
#define _MoniTool_ElemHasher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class MoniTool_Element;


//! ElemHasher defines HashCode for Element, which is : ask a
//! Element its HashCode !  Because this is the Element itself
//! which brings the HashCode for its Key
//!
//! This class complies to the template given in TCollection by
//! MapHasher itself
class MoniTool_ElemHasher 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Returns hash code for the given element, in the range [1, theUpperBound].
  //! Asks theElement its HashCode, then transforms it to be in the required range.
  //! @param theElement the element which hash code is to be computed
  //! @param theUpperBound the upper bound of the range a computing hash code must be within
  //! @return a computed hash code, in the range [1, theUpperBound]
  Standard_EXPORT static Standard_Integer HashCode (const Handle (MoniTool_Element) & theElement,
                                                    Standard_Integer                  theUpperBound);

  //! Returns True if two keys are the same.
  //! The test does not work on the Elements themselves but by
  //! calling their methods Equates
  Standard_EXPORT static Standard_Boolean IsEqual (const Handle(MoniTool_Element)& K1, const Handle(MoniTool_Element)& K2);




protected:





private:





};







#endif // _MoniTool_ElemHasher_HeaderFile
