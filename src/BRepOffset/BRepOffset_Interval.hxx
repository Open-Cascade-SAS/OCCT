// Created on: 1995-10-20
// Created by: Yves FRICAUD
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

#ifndef _BRepOffset_Interval_HeaderFile
#define _BRepOffset_Interval_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <ChFiDS_TypeOfConcavity.hxx>



class BRepOffset_Interval 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepOffset_Interval();
  
  Standard_EXPORT BRepOffset_Interval(const Standard_Real U1,
                                      const Standard_Real U2,
                                      const ChFiDS_TypeOfConcavity Type);
  
    void First (const Standard_Real U);
  
    void Last (const Standard_Real U);
  
    void Type (const ChFiDS_TypeOfConcavity T);
  
    Standard_Real First() const;
  
    Standard_Real Last() const;
  
    ChFiDS_TypeOfConcavity Type() const;




protected:





private:



  Standard_Real f;
  Standard_Real l;
  ChFiDS_TypeOfConcavity type;


};


#include <BRepOffset_Interval.lxx>





#endif // _BRepOffset_Interval_HeaderFile
