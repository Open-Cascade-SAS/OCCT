// Created on: 1999-04-08
// Created by: Fabrice SERVANT
// Copyright (c) 1999 Matra Datavision
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

#ifndef _IntPolyh_Couple_HeaderFile
#define _IntPolyh_Couple_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>


//! couple of triangles
class IntPolyh_Couple 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntPolyh_Couple();
  
  Standard_EXPORT IntPolyh_Couple(const Standard_Integer i1, const Standard_Integer i2);
  
  Standard_EXPORT Standard_Integer FirstValue() const;
  
  Standard_EXPORT Standard_Integer SecondValue() const;
  
  Standard_EXPORT Standard_Integer AnalyseFlagValue() const;
  
  Standard_EXPORT Standard_Real AngleValue() const;
  
  Standard_EXPORT void SetCoupleValue (const Standard_Integer v, const Standard_Integer w);
  
  Standard_EXPORT void SetAnalyseFlag (const Standard_Integer v);
  
  Standard_EXPORT void SetAngleValue (const Standard_Real ang);
  
  Standard_EXPORT void Dump (const Standard_Integer v) const;




protected:





private:



  Standard_Integer t1;
  Standard_Integer t2;
  Standard_Integer ia;
  Standard_Real angle;


};







#endif // _IntPolyh_Couple_HeaderFile
