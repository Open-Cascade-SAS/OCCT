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

#ifndef _IFSelect_DispPerOne_HeaderFile
#define _IFSelect_DispPerOne_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Dispatch.hxx>
#include <Standard_Integer.hxx>
class TCollection_AsciiString;
class Interface_Graph;
class IFGraph_SubPartsIterator;

class IFSelect_DispPerOne;
DEFINE_STANDARD_HANDLE(IFSelect_DispPerOne, IFSelect_Dispatch)

//! A DispPerOne gathers all the input Entities into as many
//! Packets as there Root Entities from the Final Selection,
//! that is, one Packet per Entity
class IFSelect_DispPerOne : public IFSelect_Dispatch
{

public:
  //! Creates a DispPerOne
  Standard_EXPORT IFSelect_DispPerOne();

  //! Returns as Label, "One File per Input Entity"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  //! Returns True, maximum limit is given as <nbent>
  Standard_EXPORT virtual Standard_Boolean LimitedMax(const Standard_Integer nbent,
                                                      Standard_Integer&      max) const
    Standard_OVERRIDE;

  //! Returns the list of produced Packets. It defines one Packet
  //! per Entity given by RootResult from the Final Selection.
  Standard_EXPORT void Packets(const Interface_Graph&    G,
                               IFGraph_SubPartsIterator& packs) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_DispPerOne, IFSelect_Dispatch)

protected:
private:
};

#endif // _IFSelect_DispPerOne_HeaderFile
