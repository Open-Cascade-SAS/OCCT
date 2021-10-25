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

#ifndef _IGESBasic_OrderedGroup_HeaderFile
#define _IGESBasic_OrderedGroup_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESBasic_Group.hxx>


class IGESBasic_OrderedGroup;
DEFINE_STANDARD_HANDLE(IGESBasic_OrderedGroup, IGESBasic_Group)

//! defines OrderedGroup, Type <402> Form <14>
//! in package IGESBasic
//! this class defines an Ordered Group with back pointers
//! Allows a collection of a set of entities to be
//! maintained as a single entity, but the group is
//! ordered.
//! It inherits from Group
class IGESBasic_OrderedGroup : public IGESBasic_Group
{

public:

  
  Standard_EXPORT IGESBasic_OrderedGroup();




  DEFINE_STANDARD_RTTIEXT(IGESBasic_OrderedGroup,IGESBasic_Group)

protected:




private:




};







#endif // _IGESBasic_OrderedGroup_HeaderFile
