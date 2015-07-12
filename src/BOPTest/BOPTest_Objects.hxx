// Created by: Peter KURNEV
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

#ifndef _BOPTest_Objects_HeaderFile
#define _BOPTest_Objects_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPDS_PDS.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPAlgo_PBuilder.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class BOPAlgo_PaveFiller;
class BOPAlgo_Builder;
class BOPAlgo_BOP;
class BOPAlgo_Section;



class BOPTest_Objects 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static BOPAlgo_PaveFiller& PaveFiller();
  
  Standard_EXPORT static void Init();
  
  Standard_EXPORT static void Clear();
  
  Standard_EXPORT static BOPDS_PDS PDS();
  
  Standard_EXPORT static BOPAlgo_Builder& Builder();
  
  Standard_EXPORT static BOPAlgo_BOP& BOP();
  
  Standard_EXPORT static BOPAlgo_Section& Section();
  
  Standard_EXPORT static BOPCol_ListOfShape& Shapes();
  
  Standard_EXPORT static BOPCol_ListOfShape& Tools();
  
  Standard_EXPORT static void SetBuilder (const BOPAlgo_PBuilder& theBuilder);
  
  Standard_EXPORT static void SetBuilderDefault();
  
  Standard_EXPORT static void SetRunParallel (const Standard_Boolean theFlag);
  
  Standard_EXPORT static Standard_Boolean RunParallel();
  
  Standard_EXPORT static void SetFuzzyValue (const Standard_Real theValue);
  
  Standard_EXPORT static Standard_Real FuzzyValue();




protected:





private:





};







#endif // _BOPTest_Objects_HeaderFile
