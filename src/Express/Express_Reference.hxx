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

#ifndef _Express_Reference_HeaderFile
#define _Express_Reference_HeaderFile

#include <Express_Item.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>

class Express_HSequenceOfItem;

//! Implements REFERENCE FROM (list of types used from other schema)
//! item of the EXPRESS schema, with interface for Item class.
class Express_Reference : public Express_Item
{

public:
  //! Create Reference item and initialize it
  Standard_EXPORT Express_Reference(const Standard_CString                         theName,
                                    const Handle(TColStd_HSequenceOfHAsciiString)& theTypes);

  //! Returns list of types referenced
  const Handle(TColStd_HSequenceOfHAsciiString)& Types() const { return myTypes; }

  //! Returns handle to sequence of items corresponding to
  //! listed types
  const Handle(Express_HSequenceOfItem)& Items() const { return myItems; }

  //! Redefined to empty (in order to be able to instantiate)
  //! Return False
  Standard_EXPORT virtual Standard_Boolean GenerateClass() const Standard_OVERRIDE;

  //! Propagates the calls of Use function
  Standard_EXPORT virtual void PropagateUse() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Express_Reference, Express_Item)

protected:
private:
  Handle(TColStd_HSequenceOfHAsciiString) myTypes;
  Handle(Express_HSequenceOfItem)         myItems;
};

#endif // _Express_Reference_HeaderFile
