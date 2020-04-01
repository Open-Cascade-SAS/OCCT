// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#ifndef _Express_Select_HeaderFile
#define _Express_Select_HeaderFile

#include <Standard_Type.hxx>

#include <Express_Item.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

class Express_HSequenceOfItem;

//! Implements TYPE SELECT item of the EXPRESS
//! schema, with interface for deferred Item class.
class Express_Select : public Express_Item
{

public:

  //! Create SELECT item and initialize it
  Standard_EXPORT Express_Select (const Standard_CString theName,
                                  const Handle(TColStd_HSequenceOfHAsciiString)& theNames);

  //! Returns names of types included in this SELECT
  Standard_EXPORT const Handle(TColStd_HSequenceOfHAsciiString)& Names() const;

  //! Returns sequence of items corresponding to typenames
  Standard_EXPORT const Handle(Express_HSequenceOfItem)& Items() const;

  //! Create HXX/CXX files from item
  Standard_EXPORT virtual Standard_Boolean GenerateClass() const Standard_OVERRIDE;

  //! Propagates the calls of Use function
  Standard_EXPORT virtual void PropagateUse() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Express_Select, Express_Item)

protected:

private:

  Standard_EXPORT Standard_Boolean generateSelectMember (const Handle(TColStd_HSequenceOfInteger)& theSeqMember) const;

  Handle(TColStd_HSequenceOfHAsciiString) myNames;
  Handle(Express_HSequenceOfItem) myItems;

};

#endif // _Express_Select_HeaderFile
