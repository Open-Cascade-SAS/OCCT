// Created on: 1994-02-16
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IFSelect_SelectControl_HeaderFile
#define _IFSelect_SelectControl_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Selection.hxx>
#include <Standard_Boolean.hxx>
class IFSelect_SelectionIterator;


class IFSelect_SelectControl;
DEFINE_STANDARD_HANDLE(IFSelect_SelectControl, IFSelect_Selection)

//! A SelectControl kind Selection works with two input Selections
//! in a dissymmetric way : the Main Input which gives an input
//! list of Entities, to be processed, and the Second Input which
//! gives another list, to be used to filter the main input.
//!
//! e.g. : SelectDiff retains the items of the Main Input which
//! are not in the Control Input (which acts as Diff Input)
//! or a specific selection which retains Entities from the Main
//! Input if and only if they are concerned by an entity from
//! the Control Input (such as Views in IGES, etc...)
//!
//! The way RootResult and Label are produced are at charge of
//! each sub-class
class IFSelect_SelectControl : public IFSelect_Selection
{

public:

  
  //! Returns the Main Input Selection
  Standard_EXPORT Handle(IFSelect_Selection) MainInput() const;
  
  //! Returns True if a Control Input is defined
  //! Thus, Result can be computed differently if there is a
  //! Control Input or if there is none
  Standard_EXPORT Standard_Boolean HasSecondInput() const;
  
  //! Returns the Control Input Selection, or a Null Handle
  Standard_EXPORT Handle(IFSelect_Selection) SecondInput() const;
  
  //! Sets a Selection to be the Main Input
  Standard_EXPORT void SetMainInput (const Handle(IFSelect_Selection)& sel);
  
  //! Sets a Selection to be the Control Input
  Standard_EXPORT void SetSecondInput (const Handle(IFSelect_Selection)& sel);
  
  //! Puts in an Iterator the Selections from which "me" depends
  //! That is to say, the list of Input Selections
  Standard_EXPORT void FillIterator (IFSelect_SelectionIterator& iter) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectControl,IFSelect_Selection)

protected:




private:


  Handle(IFSelect_Selection) themain;
  Handle(IFSelect_Selection) thesecond;


};







#endif // _IFSelect_SelectControl_HeaderFile
