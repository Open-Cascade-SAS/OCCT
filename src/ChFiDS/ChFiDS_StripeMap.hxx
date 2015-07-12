// Created on: 1993-11-10
// Created by: Laurent BOURESCHE
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

#ifndef _ChFiDS_StripeMap_HeaderFile
#define _ChFiDS_StripeMap_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <ChFiDS_IndexedDataMapOfVertexListOfStripe.hxx>
#include <Standard_Integer.hxx>
#include <ChFiDS_ListOfStripe.hxx>
class TopoDS_Vertex;
class ChFiDS_Stripe;


//! encapsulation of IndexedDataMapOfVertexListOfStripe
class ChFiDS_StripeMap 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ChFiDS_StripeMap();
  
  Standard_EXPORT void Add (const TopoDS_Vertex& V, const Handle(ChFiDS_Stripe)& F);
  
    Standard_Integer Extent() const;
  
  Standard_EXPORT const ChFiDS_ListOfStripe& FindFromKey (const TopoDS_Vertex& V) const;
const ChFiDS_ListOfStripe& operator() (const TopoDS_Vertex& V) const
{
  return FindFromKey(V);
}
  
  Standard_EXPORT const ChFiDS_ListOfStripe& FindFromIndex (const Standard_Integer I) const;
const ChFiDS_ListOfStripe& operator() (const Standard_Integer I) const
{
  return FindFromIndex(I);
}
  
    const TopoDS_Vertex& FindKey (const Standard_Integer I) const;
  
  Standard_EXPORT void Clear();




protected:





private:



  ChFiDS_IndexedDataMapOfVertexListOfStripe mymap;


};


#include <ChFiDS_StripeMap.lxx>





#endif // _ChFiDS_StripeMap_HeaderFile
