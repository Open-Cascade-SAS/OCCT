// Created on: 1998-10-15
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _IFSelect_GraphCounter_HeaderFile
#define _IFSelect_GraphCounter_HeaderFile

#include <Standard.hxx>

#include <IFSelect_SignCounter.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
class IFSelect_SelectDeduct;
class Interface_Graph;


class IFSelect_GraphCounter;
DEFINE_STANDARD_HANDLE(IFSelect_GraphCounter, IFSelect_SignCounter)

//! A GraphCounter computes values to be sorted with the help of
//! a Graph. I.E. not from a Signature
//!
//! The default GraphCounter works with an Applied Selection (a
//! SelectDeduct), the value is the count of selected entities
//! from each input entities)
class IFSelect_GraphCounter : public IFSelect_SignCounter
{

public:

  
  //! Creates a GraphCounter, without applied selection
  Standard_EXPORT IFSelect_GraphCounter(const Standard_Boolean withmap = Standard_True, const Standard_Boolean withlist = Standard_False);
  
  //! Returns the applied selection
  Standard_EXPORT Handle(IFSelect_SelectDeduct) Applied() const;
  
  //! Sets a new applied selection
  Standard_EXPORT void SetApplied (const Handle(IFSelect_SelectDeduct)& sel);
  
  //! Adds a list of entities in the context given by the graph
  //! Default takes the count of entities selected by the applied
  //! selection, when it is given each entity of the list
  //! Can be redefined
  Standard_EXPORT virtual void AddWithGraph (const Handle(TColStd_HSequenceOfTransient)& list, const Interface_Graph& graph) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_GraphCounter,IFSelect_SignCounter)

protected:




private:


  Handle(IFSelect_SelectDeduct) theapplied;


};







#endif // _IFSelect_GraphCounter_HeaderFile
