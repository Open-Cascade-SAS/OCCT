// Created on: 1998-11-25
// Created by: Julia GERASIMOVA
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

#ifndef _GeomPlate_Aij_HeaderFile
#define _GeomPlate_Aij_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <gp_Vec.hxx>


//! A structure containing indexes of two normals and its cross product
class GeomPlate_Aij 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomPlate_Aij();
  
  Standard_EXPORT GeomPlate_Aij(const Standard_Integer anInd1, const Standard_Integer anInd2, const gp_Vec& aVec);


friend class GeomPlate_BuildAveragePlane;


protected:





private:



  Standard_Integer Ind1;
  Standard_Integer Ind2;
  gp_Vec Vec;


};







#endif // _GeomPlate_Aij_HeaderFile
