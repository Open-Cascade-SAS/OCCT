// Created on: 1993-08-30
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _StepToTopoDS_CartesianPointHasher_HeaderFile
#define _StepToTopoDS_CartesianPointHasher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class StepGeom_CartesianPoint;



class StepToTopoDS_CartesianPointHasher 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a HasCode value  for  the  CartesianPoint
  Standard_EXPORT static Standard_Integer HashCode (const Handle(StepGeom_CartesianPoint)& K, const Standard_Integer Upper);
  
  //! Returns True  when the two  CartesianPoint are the same
  Standard_EXPORT static Standard_Boolean IsEqual (const Handle(StepGeom_CartesianPoint)& K1, const Handle(StepGeom_CartesianPoint)& K2);




protected:





private:





};







#endif // _StepToTopoDS_CartesianPointHasher_HeaderFile
