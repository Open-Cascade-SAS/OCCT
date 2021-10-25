// Created on: 1994-05-31
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

#ifndef _IGESSelect_SelectFromSingleView_HeaderFile
#define _IGESSelect_SelectFromSingleView_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectDeduct.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class TCollection_AsciiString;


class IGESSelect_SelectFromSingleView;
DEFINE_STANDARD_HANDLE(IGESSelect_SelectFromSingleView, IFSelect_SelectDeduct)

//! This selection gets in all the model, the entities which are
//! attached to the views given as input. Only Single Views are
//! considered. This information is kept from Directory Part
//! (View Item).
class IGESSelect_SelectFromSingleView : public IFSelect_SelectDeduct
{

public:

  
  //! Creates a SelectFromSingleView
  Standard_EXPORT IGESSelect_SelectFromSingleView();
  
  //! Selects the Entities which are attached to the Single View(s)
  //! present in the Input
  Standard_EXPORT Interface_EntityIterator RootResult (const Interface_Graph& G) const Standard_OVERRIDE;
  
  //! Returns the label, with is "Entities attached to single View"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SelectFromSingleView,IFSelect_SelectDeduct)

protected:




private:




};







#endif // _IGESSelect_SelectFromSingleView_HeaderFile
