// Created on: 2008-01-22
// Created by: Vladislav ROMASHKO
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _TFunction_Iterator_HeaderFile
#define _TFunction_Iterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_LabelList.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_LabelMap.hxx>
#include <Standard_Integer.hxx>
#include <TFunction_ExecutionStatus.hxx>
#include <Standard_OStream.hxx>
class TFunction_Scope;
class TDF_Label;


//! Iterator of the graph of functions
class TFunction_Iterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! An empty constructor.
  Standard_EXPORT TFunction_Iterator();
  
  //! A constructor.
  //! Initializes the iterator.
  Standard_EXPORT TFunction_Iterator(const TDF_Label& Access);
  
  //! Initializes the Iterator.
  Standard_EXPORT virtual void Init (const TDF_Label& Access);
  
  //! Defines the mode of iteration - usage or not of the execution status.
  //! If the iterator takes into account the execution status,
  //! the method ::Current() returns only "not executed" functions
  //! while their status is not changed.
  //! If the iterator ignores the execution status,
  //! the method ::Current() returns the functions
  //! following their dependencies and ignoring the execution status.
  Standard_EXPORT void SetUsageOfExecutionStatus (const Standard_Boolean usage);
  
  //! Returns usage of execution status by the iterator.
  Standard_EXPORT Standard_Boolean GetUsageOfExecutionStatus() const;
  
  //! Analyses the graph of dependencies and returns
  //! maximum number of threads may be used to calculate the model.
  Standard_EXPORT virtual Standard_Integer GetMaxNbThreads() const;
  
  //! Returns the current list of functions.
  //! If the iterator uses the execution status,
  //! the returned list contains only the functions
  //! with "not executed" status.
  Standard_EXPORT virtual const TDF_LabelList& Current() const;
  
  //! Returns false if the graph of functions is fully iterated.
  Standard_EXPORT virtual Standard_Boolean More() const;
  
  //! Switches the iterator to the next list of current functions.
  Standard_EXPORT virtual void Next();
  
  //! A help-function aimed to help the user to check the status of retrurned function.
  //! It calls TFunction_GraphNode::GetStatus() inside.
  Standard_EXPORT TFunction_ExecutionStatus GetStatus (const TDF_Label& func) const;
  
  //! A help-function aimed to help the user to change the execution status of a function.
  //! It calls TFunction_GraphNode::SetStatus() inside.
  Standard_EXPORT void SetStatus (const TDF_Label& func, const TFunction_ExecutionStatus status) const;
  
  Standard_EXPORT Standard_OStream& Dump (Standard_OStream& OS) const;




protected:





private:



  TDF_LabelList myCurrent;
  Standard_Boolean myUsageOfExecutionStatus;
  TDF_LabelMap myPassedFunctions;
  Handle(TFunction_Scope) myScope;


};







#endif // _TFunction_Iterator_HeaderFile
