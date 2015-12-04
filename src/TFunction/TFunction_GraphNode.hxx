// Created on: 2008-06-21
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

#ifndef _TFunction_GraphNode_HeaderFile
#define _TFunction_GraphNode_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_MapOfInteger.hxx>
#include <TFunction_ExecutionStatus.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class Standard_GUID;
class TDF_Attribute;
class TDF_RelocationTable;
class TDF_DataSet;


class TFunction_GraphNode;
DEFINE_STANDARD_HANDLE(TFunction_GraphNode, TDF_Attribute)

//! Provides links between functions.
class TFunction_GraphNode : public TDF_Attribute
{

public:

  
  //! Static methods
  //! ==============
  //! Finds or Creates a graph node attribute at the label <L>.
  //! Returns the attribute.
  Standard_EXPORT static Handle(TFunction_GraphNode) Set (const TDF_Label& L);
  
  //! Returns the GUID for GraphNode attribute.
  //! Instant methods
  //! ===============
  //! Constructor (empty).
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT TFunction_GraphNode();
  
  //! Defines a reference to the function as a previous one.
  Standard_EXPORT Standard_Boolean AddPrevious (const Standard_Integer funcID);
  
  //! Defines a reference to the function as a previous one.
  Standard_EXPORT Standard_Boolean AddPrevious (const TDF_Label& func);
  
  //! Removes a reference to the function as a previous one.
  Standard_EXPORT Standard_Boolean RemovePrevious (const Standard_Integer funcID);
  
  //! Removes a reference to the function as a previous one.
  Standard_EXPORT Standard_Boolean RemovePrevious (const TDF_Label& func);
  
  //! Returns a map of previous functions.
  Standard_EXPORT const TColStd_MapOfInteger& GetPrevious() const;
  
  //! Clears a map of previous functions.
  Standard_EXPORT void RemoveAllPrevious();
  
  //! Defines a reference to the function as a next one.
  Standard_EXPORT Standard_Boolean AddNext (const Standard_Integer funcID);
  
  //! Defines a reference to the function as a next one.
  Standard_EXPORT Standard_Boolean AddNext (const TDF_Label& func);
  
  //! Removes a reference to the function as a next one.
  Standard_EXPORT Standard_Boolean RemoveNext (const Standard_Integer funcID);
  
  //! Removes a reference to the function as a next one.
  Standard_EXPORT Standard_Boolean RemoveNext (const TDF_Label& func);
  
  //! Returns a map of next functions.
  Standard_EXPORT const TColStd_MapOfInteger& GetNext() const;
  
  //! Clears a map of next functions.
  Standard_EXPORT void RemoveAllNext();
  
  //! Returns the execution status of the function.
  Standard_EXPORT TFunction_ExecutionStatus GetStatus() const;
  
  //! Defines an execution status for a function.
  //! Implementation of Attribute methods
  //! ===================================
  Standard_EXPORT void SetStatus (const TFunction_ExecutionStatus status);
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void References (const Handle(TDF_DataSet)& aDataSet) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TFunction_GraphNode,TDF_Attribute)

protected:




private:


  TColStd_MapOfInteger myPrevious;
  TColStd_MapOfInteger myNext;
  TFunction_ExecutionStatus myStatus;


};







#endif // _TFunction_GraphNode_HeaderFile
