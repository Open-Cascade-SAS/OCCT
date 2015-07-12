// Created by: Peter KURNEV
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

#ifndef _BOPDS_Tools_HeaderFile
#define _BOPDS_Tools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Boolean.hxx>



//! The class BOPDS_Tools contains
//! a set auxiliary static functions
//! of the package BOPDS
class BOPDS_Tools 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Converts the conmbination of two types
  //! of shape <theT1>,<theT2>
  //! to the one integer value, that is returned
    static Standard_Integer TypeToInteger (const TopAbs_ShapeEnum theT1, const TopAbs_ShapeEnum theT2);
  

  //! Converts the type of shape <theT>,
  //! to integer value, that is returned
    static Standard_Integer TypeToInteger (const TopAbs_ShapeEnum theT);
  

  //! Returns true if the type  <theT> correspond
  //! to a shape having boundary representation
    static Standard_Boolean HasBRep (const TopAbs_ShapeEnum theT);
  

  //! Returns true if the type <theT> can be participant of
  //! an interference
    static Standard_Boolean IsInterfering (const TopAbs_ShapeEnum theT);




protected:





private:





};


#include <BOPDS_Tools.lxx>





#endif // _BOPDS_Tools_HeaderFile
