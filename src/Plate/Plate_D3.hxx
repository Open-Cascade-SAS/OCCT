// Created on: 1998-10-14
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

#ifndef _Plate_D3_HeaderFile
#define _Plate_D3_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_XYZ.hxx>


//! define an order 3 derivatives of a 3d valued
//! function of a 2d variable
class Plate_D3 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Plate_D3(const gp_XYZ& duuu, const gp_XYZ& duuv, const gp_XYZ& duvv, const gp_XYZ& dvvv);
  
  Standard_EXPORT Plate_D3(const Plate_D3& ref);


friend class Plate_GtoCConstraint;
friend class Plate_FreeGtoCConstraint;


protected:





private:



  gp_XYZ Duuu;
  gp_XYZ Duuv;
  gp_XYZ Duvv;
  gp_XYZ Dvvv;


};







#endif // _Plate_D3_HeaderFile
