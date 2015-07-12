// Created on: 1995-03-21
// Created by: Laurent BOURESCHE
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

#ifndef _ChFiDS_Regul_HeaderFile
#define _ChFiDS_Regul_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>


//! Storage of  a curve  and its 2 faces or surfaces of  support.
class ChFiDS_Regul 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ChFiDS_Regul();
  
  Standard_EXPORT void SetCurve (const Standard_Integer IC);
  
  Standard_EXPORT void SetS1 (const Standard_Integer IS1, const Standard_Boolean IsFace = Standard_True);
  
  Standard_EXPORT void SetS2 (const Standard_Integer IS2, const Standard_Boolean IsFace = Standard_True);
  
  Standard_EXPORT Standard_Boolean IsSurface1() const;
  
  Standard_EXPORT Standard_Boolean IsSurface2() const;
  
  Standard_EXPORT Standard_Integer Curve() const;
  
  Standard_EXPORT Standard_Integer S1() const;
  
  Standard_EXPORT Standard_Integer S2() const;




protected:





private:



  Standard_Integer icurv;
  Standard_Integer is1;
  Standard_Integer is2;


};







#endif // _ChFiDS_Regul_HeaderFile
