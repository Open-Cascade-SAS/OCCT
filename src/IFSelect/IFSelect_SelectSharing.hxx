// Created on: 1993-11-03
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IFSelect_SelectSharing_HeaderFile
#define _IFSelect_SelectSharing_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectDeduct.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class TCollection_AsciiString;


class IFSelect_SelectSharing;
DEFINE_STANDARD_HANDLE(IFSelect_SelectSharing, IFSelect_SelectDeduct)

//! A SelectSharing selects Entities which directly Share (Level
//! One) the Entities of the Input list
//! Remark : if an Entity of the Input List directly shares
//! another one, it is of course present in the Result List
class IFSelect_SelectSharing : public IFSelect_SelectDeduct
{

public:

  
  //! Creates a SelectSharing;
  Standard_EXPORT IFSelect_SelectSharing();
  
  //! Returns the list of selected entities (list of entities
  //! which share (level one) those of input list)
  Standard_EXPORT Interface_EntityIterator RootResult (const Interface_Graph& G) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium : "Sharing (one level)"
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectSharing,IFSelect_SelectDeduct)

protected:




private:




};







#endif // _IFSelect_SelectSharing_HeaderFile
