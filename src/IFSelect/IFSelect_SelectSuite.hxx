// Created on: 1998-10-19
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

#ifndef _IFSelect_SelectSuite_HeaderFile
#define _IFSelect_SelectSuite_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <IFSelect_SelectDeduct.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
class IFSelect_Selection;
class Interface_EntityIterator;
class Interface_Graph;


class IFSelect_SelectSuite;
DEFINE_STANDARD_HANDLE(IFSelect_SelectSuite, IFSelect_SelectDeduct)

//! A SelectSuite can describe a suite of SelectDeduct as a unique
//! one : in other words, it can be seen as a "macro selection"
//!
//! It works by applying each of its items (which is a
//! SelectDeduct) on the result computed by the previous one
//! (by using Alternate Input)
//!
//! But each of these Selections used as items may be used
//! independently, it will then give its own result
//!
//! Hence, SelectSuite gives a way of defining a new Selection
//! from existing ones, without having to do copies or saves
class IFSelect_SelectSuite : public IFSelect_SelectDeduct
{

public:

  
  //! Creates an empty SelectSuite
  Standard_EXPORT IFSelect_SelectSuite();
  
  //! Adds an input selection. I.E. :
  //! If <item> is a SelectDeduct, adds it as Previous, not as Input
  //! Else, sets it as Input
  //! Returns True when done
  //! Returns False and refuses to work if Input is already defined
  Standard_EXPORT Standard_Boolean AddInput (const Handle(IFSelect_Selection)& item);
  
  //! Adds a new first item (prepends to the list). The Input is not
  //! touched
  //! If <item> is null, does nothing
  Standard_EXPORT void AddPrevious (const Handle(IFSelect_SelectDeduct)& item);
  
  //! Adds a new last item (prepends to the list)
  //! If <item> is null, does nothing
  Standard_EXPORT void AddNext (const Handle(IFSelect_SelectDeduct)& item);
  
  //! Returns the count of Items
  Standard_EXPORT Standard_Integer NbItems() const;
  
  //! Returns an item from its rank in the list
  //! (the Input is always apart)
  Standard_EXPORT Handle(IFSelect_SelectDeduct) Item (const Standard_Integer num) const;
  
  //! Sets a value for the Label
  Standard_EXPORT void SetLabel (const Standard_CString lab);
  
  //! Returns the list of selected entities
  //! To do this, once InputResult has been taken (if Input or
  //! Alternate has been defined, else the first Item gives it) :
  //! this result is set as alternate input for the first item,
  //! which computes its result : this result is set as alternate
  //! input for the second item, etc...
  Standard_EXPORT Interface_EntityIterator RootResult (const Interface_Graph& G) const Standard_OVERRIDE;
  
  //! Returns the Label
  //! Either it has been defined by SetLabel, or it will give
  //! "Suite of nn Selections"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectSuite,IFSelect_SelectDeduct)

protected:




private:


  TCollection_AsciiString thelab;
  TColStd_SequenceOfTransient thesel;


};







#endif // _IFSelect_SelectSuite_HeaderFile
