// Created on: 1995-10-19
// Created by: Andre LIEUTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Plate_PinpointConstraint_HeaderFile
#define _Plate_PinpointConstraint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_XYZ.hxx>
#include <gp_XY.hxx>


//! define a constraint on the Plate
class Plate_PinpointConstraint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Plate_PinpointConstraint();
  
  Standard_EXPORT Plate_PinpointConstraint(const gp_XY& point2d, const gp_XYZ& ImposedValue, const Standard_Integer iu = 0, const Standard_Integer iv = 0);
  
    const gp_XY& Pnt2d() const;
  
    const Standard_Integer& Idu() const;
  
    const Standard_Integer& Idv() const;
  
    const gp_XYZ& Value() const;




protected:





private:



  gp_XYZ value;
  gp_XY pnt2d;
  Standard_Integer idu;
  Standard_Integer idv;


};


#include <Plate_PinpointConstraint.lxx>





#endif // _Plate_PinpointConstraint_HeaderFile
