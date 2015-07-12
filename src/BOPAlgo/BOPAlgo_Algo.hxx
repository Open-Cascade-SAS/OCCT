// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef _BOPAlgo_Algo_HeaderFile
#define _BOPAlgo_Algo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPCol_BaseAllocator.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Message_ProgressIndicator;


//! provides the root interface for algorithms
class BOPAlgo_Algo 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Boolean GetParallelMode();
  
  Standard_EXPORT static void SetParallelMode (const Standard_Boolean theNewMode);
  
  Standard_EXPORT virtual void Perform() = 0;
  
  Standard_EXPORT Standard_Integer ErrorStatus() const;
  
  Standard_EXPORT Standard_Integer WarningStatus() const;
  
  Standard_EXPORT const BOPCol_BaseAllocator& Allocator() const;
  
  //! Set the flag of parallel processing
  //! if <theFlag> is true  the parallel processing is switched on
  //! if <theFlag> is false the parallel processing is switched off
  Standard_EXPORT void SetRunParallel (const Standard_Boolean theFlag);
  
  //! Returns the flag of parallel processing
  Standard_EXPORT Standard_Boolean RunParallel() const;
  
  //! Set the Progress Indicator object.
  Standard_EXPORT void SetProgressIndicator (const Handle(Message_ProgressIndicator)& theObj);




protected:

  
  Standard_EXPORT BOPAlgo_Algo();
Standard_EXPORT virtual ~BOPAlgo_Algo();
  
  Standard_EXPORT BOPAlgo_Algo(const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT virtual void CheckData();
  
  Standard_EXPORT virtual void CheckResult();
  
  //! Breaks the execution if the break signal
  //! is indicated by myProgressIndicator.
  Standard_EXPORT void UserBreak() const;


  BOPCol_BaseAllocator myAllocator;
  Standard_Integer myErrorStatus;
  Standard_Integer myWarningStatus;
  Standard_Boolean myRunParallel;
  Handle(Message_ProgressIndicator) myProgressIndicator;


private:





};







#endif // _BOPAlgo_Algo_HeaderFile
