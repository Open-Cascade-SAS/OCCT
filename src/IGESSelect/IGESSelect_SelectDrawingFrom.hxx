// Created on: 1994-06-01
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

#ifndef _IGESSelect_SelectDrawingFrom_HeaderFile
#define _IGESSelect_SelectDrawingFrom_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectDeduct.hxx>
#include <Standard_Boolean.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class TCollection_AsciiString;


class IGESSelect_SelectDrawingFrom;
DEFINE_STANDARD_HANDLE(IGESSelect_SelectDrawingFrom, IFSelect_SelectDeduct)

//! This selection gets the Drawings attached to its input IGES
//! entities. They are read through thr Single Views, referenced
//! in Directory Parts of the entities
class IGESSelect_SelectDrawingFrom : public IFSelect_SelectDeduct
{

public:

  
  //! Creates a SelectDrawingFrom
  Standard_EXPORT IGESSelect_SelectDrawingFrom();
  
  //! Selects the Drawings attached (through Single Views in
  //! Directory Part) to input entities
  Standard_EXPORT Interface_EntityIterator RootResult (const Interface_Graph& G) const Standard_OVERRIDE;
  
  //! Returns the label, with is "Drawings attached"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SelectDrawingFrom,IFSelect_SelectDeduct)

protected:

  
  //! Returns True, because selection works with a ViewSorter which
  //! gives a unique result
  Standard_EXPORT virtual Standard_Boolean HasUniqueResult() const Standard_OVERRIDE;



private:




};







#endif // _IGESSelect_SelectDrawingFrom_HeaderFile
