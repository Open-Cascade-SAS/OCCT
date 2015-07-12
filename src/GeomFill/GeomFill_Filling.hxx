// Created on: 1993-09-28
// Created by: Bruno DUMORTIER
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

#ifndef _GeomFill_Filling_HeaderFile
#define _GeomFill_Filling_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
class Standard_NoSuchObject;


//! Root class for Filling;
class GeomFill_Filling 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomFill_Filling();
  
  Standard_EXPORT Standard_Integer NbUPoles() const;
  
  Standard_EXPORT Standard_Integer NbVPoles() const;
  
  Standard_EXPORT void Poles (TColgp_Array2OfPnt& Poles) const;
  
  Standard_EXPORT Standard_Boolean isRational() const;
  
  Standard_EXPORT void Weights (TColStd_Array2OfReal& Weights) const;




protected:



  Standard_Boolean IsRational;
  Handle(TColgp_HArray2OfPnt) myPoles;
  Handle(TColStd_HArray2OfReal) myWeights;


private:





};







#endif // _GeomFill_Filling_HeaderFile
