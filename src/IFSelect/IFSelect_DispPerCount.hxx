// Created on: 1992-11-17
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFSelect_DispPerCount_HeaderFile
#define _IFSelect_DispPerCount_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Dispatch.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class IFSelect_IntParam;
class Interface_InterfaceError;
class TCollection_AsciiString;
class Interface_Graph;
class IFGraph_SubPartsIterator;


class IFSelect_DispPerCount;
DEFINE_STANDARD_HANDLE(IFSelect_DispPerCount, IFSelect_Dispatch)

//! A DispPerCount gathers all the input Entities into one or
//! several Packets, each containing a defined count of Entity
//! This count is a Parameter of the DispPerCount, given as an
//! IntParam, thus allowing external control of its Value
class IFSelect_DispPerCount : public IFSelect_Dispatch
{

public:

  
  //! Creates a DispPerCount with no Count (default value 1)
  Standard_EXPORT IFSelect_DispPerCount();
  
  //! Returns the Count Parameter used for splitting
  Standard_EXPORT Handle(IFSelect_IntParam) Count() const;
  
  //! Sets a new Parameter for Count
  Standard_EXPORT void SetCount (const Handle(IFSelect_IntParam)& count);
  
  //! Returns the effective value of the count parameter
  //! (if Count Parameter not Set or value not positive, returns 1)
  Standard_EXPORT Standard_Integer CountValue() const;
  
  //! Returns as Label, "One File per <count> Input Entities"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;
  
  //! Returns True, maximum count is given as <nbent>
  Standard_EXPORT virtual Standard_Boolean LimitedMax (const Standard_Integer nbent, Standard_Integer& max) const Standard_OVERRIDE;

  //! Computes the list of produced Packets. It defines Packets in
  //! order to have at most <Count> Entities per Packet, Entities
  //! are given by RootResult from the Final Selection.
  Standard_EXPORT void Packets (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_DispPerCount,IFSelect_Dispatch)

protected:




private:


  Handle(IFSelect_IntParam) thecount;


};







#endif // _IFSelect_DispPerCount_HeaderFile
