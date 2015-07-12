// Created on: 1998-05-06
// Created by: Andre LIEUTIER
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

#ifndef _Plate_PlaneConstraint_HeaderFile
#define _Plate_PlaneConstraint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Plate_LinearScalarConstraint.hxx>
#include <Standard_Integer.hxx>
class gp_XY;
class gp_Pln;
class Plate_LinearScalarConstraint;


//! constraint a point to belong to a Plane
class Plate_PlaneConstraint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Plate_PlaneConstraint(const gp_XY& point2d, const gp_Pln& pln, const Standard_Integer iu = 0, const Standard_Integer iv = 0);
  
    const Plate_LinearScalarConstraint& LSC() const;




protected:





private:



  Plate_LinearScalarConstraint myLSC;


};


#include <Plate_PlaneConstraint.lxx>





#endif // _Plate_PlaneConstraint_HeaderFile
