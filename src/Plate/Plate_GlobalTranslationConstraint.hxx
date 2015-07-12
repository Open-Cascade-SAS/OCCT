// Created on: 1998-03-26
// Created by: # Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Plate_GlobalTranslationConstraint_HeaderFile
#define _Plate_GlobalTranslationConstraint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Plate_LinearXYZConstraint.hxx>
#include <TColgp_SequenceOfXY.hxx>
class Plate_LinearXYZConstraint;


//! force a set of UV points to translate without deformation
class Plate_GlobalTranslationConstraint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Plate_GlobalTranslationConstraint(const TColgp_SequenceOfXY& SOfXY);
  
    const Plate_LinearXYZConstraint& LXYZC() const;




protected:





private:



  Plate_LinearXYZConstraint myLXYZC;


};


#include <Plate_GlobalTranslationConstraint.lxx>





#endif // _Plate_GlobalTranslationConstraint_HeaderFile
