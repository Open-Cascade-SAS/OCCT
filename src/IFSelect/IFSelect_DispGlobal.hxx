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

#ifndef _IFSelect_DispGlobal_HeaderFile
#define _IFSelect_DispGlobal_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Dispatch.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class TCollection_AsciiString;
class Interface_Graph;
class IFGraph_SubPartsIterator;


class IFSelect_DispGlobal;
DEFINE_STANDARD_HANDLE(IFSelect_DispGlobal, IFSelect_Dispatch)

//! A DispGlobal gathers all the input Entities into only one
//! global Packet
class IFSelect_DispGlobal : public IFSelect_Dispatch
{

public:

  
  //! Creates a DispGlobal
  Standard_EXPORT IFSelect_DispGlobal();
  
  //! Returns as Label, "One File for all Input"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;
  
  //! Returns True : maximum equates 1
  Standard_EXPORT virtual Standard_Boolean LimitedMax (const Standard_Integer nbent, Standard_Integer& max) const Standard_OVERRIDE;

  //! Computes the list of produced Packets. It is made of only ONE
  //! Packet, which gets the RootResult from the Final Selection.
  //! Remark : the inherited exception raising is never activated.
  Standard_EXPORT void Packets (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_DispGlobal,IFSelect_Dispatch)

protected:




private:




};







#endif // _IFSelect_DispGlobal_HeaderFile
