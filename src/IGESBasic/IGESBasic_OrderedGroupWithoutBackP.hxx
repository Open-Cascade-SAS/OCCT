// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
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

#ifndef _IGESBasic_OrderedGroupWithoutBackP_HeaderFile
#define _IGESBasic_OrderedGroupWithoutBackP_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESBasic_Group.hxx>
class Standard_OutOfRange;


class IGESBasic_OrderedGroupWithoutBackP;
DEFINE_STANDARD_HANDLE(IGESBasic_OrderedGroupWithoutBackP, IGESBasic_Group)

//! defines OrderedGroupWithoutBackP, Type <402> Form <15>
//! in package IGESBasic
//! Allows a collection of a set of entities to be
//! maintained as a single entity, but the group is
//! ordered and there are no back pointers.
//! It inherits from Group
class IGESBasic_OrderedGroupWithoutBackP : public IGESBasic_Group
{

public:

  
  Standard_EXPORT IGESBasic_OrderedGroupWithoutBackP();




  DEFINE_STANDARD_RTTIEXT(IGESBasic_OrderedGroupWithoutBackP,IGESBasic_Group)

protected:




private:




};







#endif // _IGESBasic_OrderedGroupWithoutBackP_HeaderFile
