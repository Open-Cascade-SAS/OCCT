// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef _TDataStd_Tick_HeaderFile
#define _TDataStd_Tick_HeaderFile

#include <TDataStd_GenericEmpty.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataStd_Tick;
DEFINE_STANDARD_HANDLE(TDataStd_Tick, TDataStd_GenericEmpty)

//! Defines a boolean attribute.
//! If it exists at a label - true,
//! Otherwise - false.
class TDataStd_Tick : public TDataStd_GenericEmpty
{

public:

  
  //! Static methods
  //! ==============
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Find, or create, a Tick attribute.
  //! Tick methods
  //! ============
  Standard_EXPORT static Handle(TDataStd_Tick) Set (const TDF_Label& label);
  
  Standard_EXPORT TDataStd_Tick();
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;


  DEFINE_DERIVED_ATTRIBUTE(TDataStd_Tick, TDataStd_GenericEmpty)

};







#endif // _TDataStd_Tick_HeaderFile
