// Created on: 1993-08-06
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

#ifndef _StepToTopoDS_PointPairHasher_HeaderFile
#define _StepToTopoDS_PointPairHasher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class StepToTopoDS_PointPair;



class StepToTopoDS_PointPairHasher 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Computes a hash code for the point pair, in the range [1, theUpperBound]
  //! @param thePointPair the point pair which hash code is to be computed
  //! @param theUpperBound the upper bound of the range a computing hash code must be within
  //! @return a computed hash code, in the range [1, theUpperBound]
  Standard_EXPORT static Standard_Integer HashCode (const StepToTopoDS_PointPair& thePointPair, Standard_Integer theUpperBound);
  
  //! Returns True  when the two  PointPair are the same
  Standard_EXPORT static Standard_Boolean IsEqual (const StepToTopoDS_PointPair& K1, const StepToTopoDS_PointPair& K2);




protected:





private:





};







#endif // _StepToTopoDS_PointPairHasher_HeaderFile
