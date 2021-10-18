// Created on: 1995-03-22
// Created by: Christophe MARION
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

#ifndef _HLRTopoBRep_VData_HeaderFile
#define _HLRTopoBRep_VData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <TopoDS_Shape.hxx>



class HLRTopoBRep_VData 
{
public:

  DEFINE_STANDARD_ALLOC

  
    HLRTopoBRep_VData();
  
  Standard_EXPORT HLRTopoBRep_VData(const Standard_Real P, const TopoDS_Shape& V);
  
    Standard_Real Parameter() const;
  
    const TopoDS_Shape& Vertex() const;




protected:





private:



  Standard_Real myParameter;
  TopoDS_Shape myVertex;


};


#include <HLRTopoBRep_VData.lxx>





#endif // _HLRTopoBRep_VData_HeaderFile
