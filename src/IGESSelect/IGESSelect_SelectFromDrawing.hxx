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

#ifndef _IGESSelect_SelectFromDrawing_HeaderFile
#define _IGESSelect_SelectFromDrawing_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectDeduct.hxx>
class Interface_InterfaceError;
class Interface_EntityIterator;
class Interface_Graph;
class TCollection_AsciiString;


class IGESSelect_SelectFromDrawing;
DEFINE_STANDARD_HANDLE(IGESSelect_SelectFromDrawing, IFSelect_SelectDeduct)

//! This selection gets in all the model, the entities which are
//! attached to the drawing(s) given as input. This includes :
//! - Drawing Frame (Annotations directky referenced by Drawings)
//! - Entities attached to the single Views referenced by Drawings
class IGESSelect_SelectFromDrawing : public IFSelect_SelectDeduct
{

public:

  
  //! Creates a SelectFromDrawing
  Standard_EXPORT IGESSelect_SelectFromDrawing();
  
  //! Selects the Entities which are attached to the Drawing(s)
  //! present in the Input
  Standard_EXPORT Interface_EntityIterator RootResult (const Interface_Graph& G) const Standard_OVERRIDE;
  
  //! Returns the label, with is "Entities attached to Drawing"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SelectFromDrawing,IFSelect_SelectDeduct)

protected:




private:




};







#endif // _IGESSelect_SelectFromDrawing_HeaderFile
