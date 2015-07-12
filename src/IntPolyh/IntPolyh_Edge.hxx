// Created on: 1999-03-05
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

#ifndef _IntPolyh_Edge_HeaderFile
#define _IntPolyh_Edge_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>



class IntPolyh_Edge 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntPolyh_Edge();
  
  Standard_EXPORT IntPolyh_Edge(const Standard_Integer i1, const Standard_Integer i2, const Standard_Integer i3, const Standard_Integer i4);
  
  Standard_EXPORT Standard_Integer FirstPoint() const;
  
  Standard_EXPORT Standard_Integer SecondPoint() const;
  
  Standard_EXPORT Standard_Integer FirstTriangle() const;
  
  Standard_EXPORT Standard_Integer SecondTriangle() const;
  
  Standard_EXPORT Standard_Integer AnalysisFlag() const;
  
  Standard_EXPORT void SetFirstPoint (const Standard_Integer v);
  
  Standard_EXPORT void SetSecondPoint (const Standard_Integer v);
  
  Standard_EXPORT void SetFirstTriangle (const Standard_Integer v);
  
  Standard_EXPORT void SetSecondTriangle (const Standard_Integer v);
  
  Standard_EXPORT void SetAnalysisFlag (const Standard_Integer v);
  
  Standard_EXPORT void Dump (const Standard_Integer v) const;




protected:





private:



  Standard_Integer p1;
  Standard_Integer p2;
  Standard_Integer t1;
  Standard_Integer t2;
  Standard_Integer ia;


};







#endif // _IntPolyh_Edge_HeaderFile
