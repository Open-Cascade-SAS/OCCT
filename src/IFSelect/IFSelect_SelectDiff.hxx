// Created on: 1992-11-18
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

#ifndef _IFSelect_SelectDiff_HeaderFile
#define _IFSelect_SelectDiff_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectControl.hxx>
#include <Standard_Boolean.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class TCollection_AsciiString;


class IFSelect_SelectDiff;
DEFINE_STANDARD_HANDLE(IFSelect_SelectDiff, IFSelect_SelectControl)

//! A SelectDiff keeps the entities from a Selection, the Main
//! Input, which are not listed by the Second Input
class IFSelect_SelectDiff : public IFSelect_SelectControl
{

public:

  
  //! Creates an empty SelectDiff
  Standard_EXPORT IFSelect_SelectDiff();
  
  //! Returns the list of selected entities : they are the Entities
  //! gotten from the Main Input but not from the Diff Input
  Standard_EXPORT Interface_EntityIterator RootResult (const Interface_Graph& G) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium : "Difference"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectDiff,IFSelect_SelectControl)

protected:

  
  //! Returns always True, because RootResult gives a Unique list
  Standard_EXPORT virtual Standard_Boolean HasUniqueResult() const Standard_OVERRIDE;



private:




};







#endif // _IFSelect_SelectDiff_HeaderFile
