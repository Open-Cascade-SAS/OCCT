// Created on: 2018/03/21
// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _BRepTest_Objects_HeaderFile
#define _BRepTest_Objects_HeaderFile

#include <BRepTools_History.hxx>

//! Provides the access to the useful tools common for the algorithms.
class BRepTest_Objects
{
public:
  //! Sets the given history into the session.
  Standard_EXPORT static void SetHistory(const Handle(BRepTools_History)& theHistory);

  //! Adds the given history to the history in the session.
  Standard_EXPORT static void AddHistory(const Handle(BRepTools_History)& theHistory);

  //! Sets the history of the given algorithm into the session.
  template <class TheAlgo>
  static void SetHistory(const TopTools_ListOfShape& theArguments, TheAlgo& theAlgo)
  {
    SetHistory(new BRepTools_History(theArguments, theAlgo));
  }

  //! Adds the history of the given algorithm into the session.
  template <class TheAlgo>
  static void AddHistory(const TopTools_ListOfShape& theArguments, TheAlgo& theAlgo)
  {
    AddHistory(new BRepTools_History(theArguments, theAlgo));
  }

  //! Returns the history from the session.
  Standard_EXPORT static Handle(BRepTools_History) History();

  //! Enables/Disables the history saving
  Standard_EXPORT static void SetToFillHistory(const Standard_Boolean theFillHist);

  //! Returns the flag controlling the history collection
  Standard_EXPORT static Standard_Boolean IsHistoryNeeded();
};

#endif
