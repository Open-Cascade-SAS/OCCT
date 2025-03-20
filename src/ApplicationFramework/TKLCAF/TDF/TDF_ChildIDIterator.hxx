// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TDF_ChildIDIterator_HeaderFile
#define _TDF_ChildIDIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_GUID.hxx>
#include <TDF_ChildIterator.hxx>
class TDF_Attribute;
class TDF_Label;

//! Iterates on the children of a label, to find
//! attributes having ID as Attribute ID.
//!
//! Level option works as TDF_ChildIterator.
class TDF_ChildIDIterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty iterator.
  Standard_EXPORT TDF_ChildIDIterator();

  //! Iterates on the children of the given label. If
  //! <allLevels> option is set to true, it explores not
  //! only the first, but all the sub label levels.
  Standard_EXPORT TDF_ChildIDIterator(const TDF_Label&       aLabel,
                                      const Standard_GUID&   anID,
                                      const Standard_Boolean allLevels = Standard_False);

  //! Initializes the iteration on the children of the
  //! given label. If <allLevels> option is set to true,
  //! it explores not only the first, but all the sub
  //! label levels.
  Standard_EXPORT void Initialize(const TDF_Label&       aLabel,
                                  const Standard_GUID&   anID,
                                  const Standard_Boolean allLevels = Standard_False);

  //! Returns True if there is a current Item in the
  //! iteration.
  Standard_Boolean More() const;

  //! Move to the next Item
  Standard_EXPORT void Next();

  //! Move to the next Brother. If there is none, go up
  //! etc. This method is interesting only with
  //! "allLevels" behavior, because it avoids to explore
  //! the current label children.
  Standard_EXPORT void NextBrother();

  //! Returns the current item; a null handle if there is none.
  Handle(TDF_Attribute) Value() const;

protected:
private:
  Standard_GUID         myID;
  TDF_ChildIterator     myItr;
  Handle(TDF_Attribute) myAtt;
};

#include <TDF_ChildIDIterator.lxx>

#endif // _TDF_ChildIDIterator_HeaderFile
