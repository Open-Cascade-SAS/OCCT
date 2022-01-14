// Created on: 1994-09-02
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

#ifndef _IFSelect_SelectIncorrectEntities_HeaderFile
#define _IFSelect_SelectIncorrectEntities_HeaderFile

#include <Standard.hxx>

#include <IFSelect_SelectFlag.hxx>


class IFSelect_SelectIncorrectEntities;
DEFINE_STANDARD_HANDLE(IFSelect_SelectIncorrectEntities, IFSelect_SelectFlag)

//! A SelectIncorrectEntities sorts the Entities which have been
//! noted as Incorrect in the Graph of the Session
//! (flag "Incorrect")
//! It can find a result only if ComputeCheck has formerly been
//! called on the WorkSession. Else, its result will be empty.
class IFSelect_SelectIncorrectEntities : public IFSelect_SelectFlag
{

public:

  
  //! Creates a SelectIncorrectEntities
  //! i.e. a SelectFlag("Incorrect")
  Standard_EXPORT IFSelect_SelectIncorrectEntities();




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectIncorrectEntities,IFSelect_SelectFlag)

protected:




private:




};







#endif // _IFSelect_SelectIncorrectEntities_HeaderFile
