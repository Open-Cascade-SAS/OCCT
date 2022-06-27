// Created on: 2003-06-04
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _StepDimTol_CylindricityTolerance_HeaderFile
#define _StepDimTol_CylindricityTolerance_HeaderFile

#include <Standard.hxx>

#include <StepDimTol_GeometricTolerance.hxx>


class StepDimTol_CylindricityTolerance;
DEFINE_STANDARD_HANDLE(StepDimTol_CylindricityTolerance, StepDimTol_GeometricTolerance)

//! Representation of STEP entity CylindricityTolerance
class StepDimTol_CylindricityTolerance : public StepDimTol_GeometricTolerance
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepDimTol_CylindricityTolerance();




  DEFINE_STANDARD_RTTIEXT(StepDimTol_CylindricityTolerance,StepDimTol_GeometricTolerance)

protected:




private:




};







#endif // _StepDimTol_CylindricityTolerance_HeaderFile
