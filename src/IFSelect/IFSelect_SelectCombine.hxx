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

#ifndef _IFSelect_SelectCombine_HeaderFile
#define _IFSelect_SelectCombine_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_TSeqOfSelection.hxx>
#include <IFSelect_Selection.hxx>
#include <Standard_Integer.hxx>
class IFSelect_SelectionIterator;


class IFSelect_SelectCombine;
DEFINE_STANDARD_HANDLE(IFSelect_SelectCombine, IFSelect_Selection)

//! A SelectCombine type Selection defines algebraic operations
//! between results of several Selections
//! It is a deferred class : sub-classes will have to define
//! precise what operator is to be applied
class IFSelect_SelectCombine : public IFSelect_Selection
{

public:

  
  //! Returns the count of Input Selections
  Standard_EXPORT Standard_Integer NbInputs() const;
  
  //! Returns an Input Selection, given its rank in the list
  Standard_EXPORT Handle(IFSelect_Selection) Input (const Standard_Integer num) const;
  
  //! Returns the rank of an input Selection, 0 if not in the list.
  //! Most generally, its value is meaningless, except for testing
  //! the presence of an input Selection :
  //! - == 0  if <sel> is not an input for <me>
  //! - >  0  if <sel> is an input for <me>
  Standard_EXPORT Standard_Integer InputRank (const Handle(IFSelect_Selection)& sel) const;
  
  //! Adds a Selection to the filling list
  //! By default, adds it to the end of the list
  //! A Positive rank less then NbInputs gives an insertion rank
  //! (InsertBefore : the new <atnum>th item of the list is <sel>)
  Standard_EXPORT void Add (const Handle(IFSelect_Selection)& sel, const Standard_Integer atnum = 0);
  
  //! Removes an input Selection.
  //! Returns True if Done, False, if <sel> is not an input for <me>
  Standard_EXPORT Standard_Boolean Remove (const Handle(IFSelect_Selection)& sel);
  
  //! Removes an input Selection, given its rank in the list
  //! Returns True if Done, False if <num> is out of range
  Standard_EXPORT Standard_Boolean Remove (const Standard_Integer num);
  
  //! Puts in an Iterator the Selections from which "me" depends
  //! That is to say, the list of Input Selections
  Standard_EXPORT void FillIterator (IFSelect_SelectionIterator& iter) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectCombine,IFSelect_Selection)

protected:

  
  //! Defines an empty SelectCombine
  Standard_EXPORT IFSelect_SelectCombine();
  
  //! Returns always True, because RootResult gives a Unique list
  Standard_EXPORT virtual Standard_Boolean HasUniqueResult() const Standard_OVERRIDE;



private:


  IFSelect_TSeqOfSelection thelist;


};







#endif // _IFSelect_SelectCombine_HeaderFile
