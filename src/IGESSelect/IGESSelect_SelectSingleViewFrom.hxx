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

#ifndef _IGESSelect_SelectSingleViewFrom_HeaderFile
#define _IGESSelect_SelectSingleViewFrom_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectDeduct.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class TCollection_AsciiString;


class IGESSelect_SelectSingleViewFrom;
DEFINE_STANDARD_HANDLE(IGESSelect_SelectSingleViewFrom, IFSelect_SelectDeduct)

//! This selection gets the Single Views attached to its input
//! IGES entities. Single Views themselves or Drawings as passed
//! as such (Drawings, for their Annotations)
class IGESSelect_SelectSingleViewFrom : public IFSelect_SelectDeduct
{

public:

  
  //! Creates a SelectSingleViewFrom
  Standard_EXPORT IGESSelect_SelectSingleViewFrom();
  
  //! Selects the Single Views attached (in Directory Part) to
  //! input entities
  Standard_EXPORT Interface_EntityIterator RootResult (const Interface_Graph& G) const Standard_OVERRIDE;
  
  //! Returns the label, with is "Single Views attached"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SelectSingleViewFrom,IFSelect_SelectDeduct)

protected:

  
  //! Returns True, because selection works with a ViewSorter which
  //! gives a unique result
  Standard_EXPORT virtual Standard_Boolean HasUniqueResult() const Standard_OVERRIDE;



private:




};







#endif // _IGESSelect_SelectSingleViewFrom_HeaderFile
