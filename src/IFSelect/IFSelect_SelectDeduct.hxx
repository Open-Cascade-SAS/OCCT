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

#ifndef _IFSelect_SelectDeduct_HeaderFile
#define _IFSelect_SelectDeduct_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Selection.hxx>
class IFSelect_SelectPointed;
class Interface_EntityIterator;
class Interface_Graph;
class IFSelect_SelectionIterator;


class IFSelect_SelectDeduct;
DEFINE_STANDARD_HANDLE(IFSelect_SelectDeduct, IFSelect_Selection)

//! A SelectDeduct determines a list of Entities from an Input
//! Selection, by a computation : Output list is not obliged to be
//! a sub-list of Input list
//! (for more specific, see SelectExtract for filtered sub-lists,
//! and SelectExplore for recurcive exploration)
//!
//! A SelectDeduct may use an alternate input for one shot
//! This allows to use an already existing definition, by
//! overloading the input selection by an alternate list,
//! already defined, for one use :
//! If this alternate list is set, InputResult queries it instead
//! of calling the input selection, then clears it immediately
class IFSelect_SelectDeduct : public IFSelect_Selection
{

public:

  
  //! Defines or Changes the Input Selection
  Standard_EXPORT void SetInput (const Handle(IFSelect_Selection)& sel);
  
  //! Returns the Input Selection
  Standard_EXPORT Handle(IFSelect_Selection) Input() const;
  
  //! Returns True if the Input Selection is defined, False else
  Standard_EXPORT Standard_Boolean HasInput() const;
  
  //! Tells if an Alternate List has been set, i.e. : the Alternate
  //! Definition is present and set
  Standard_EXPORT Standard_Boolean HasAlternate() const;
  
  //! Returns the Alternate Definition
  //! It is returned modifiable, hence an already defined
  //! SelectPointed can be used
  //! But if it was not yet defined, it is created the first time
  //!
  //! It is exploited by InputResult
  Standard_EXPORT Handle(IFSelect_SelectPointed)& Alternate();
  
  //! Returns the Result determined by Input Selection, as Unique
  //! if Input Selection is not defined, returns an empty list.
  //!
  //! If Alternate is set, InputResult takes its definition instead
  //! of calling the Input Selection, then clears it
  Standard_EXPORT Interface_EntityIterator InputResult (const Interface_Graph& G) const;
  
  //! Puts in an Iterator the Selections from which "me" depends
  //! This list contains one Selection : the InputSelection
  Standard_EXPORT void FillIterator (IFSelect_SelectionIterator& iter) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectDeduct,IFSelect_Selection)

protected:




private:


  Handle(IFSelect_Selection) thesel;
  Handle(IFSelect_SelectPointed) thealt;


};







#endif // _IFSelect_SelectDeduct_HeaderFile
