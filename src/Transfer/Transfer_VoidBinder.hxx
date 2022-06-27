// Created on: 1994-06-27
// Created by: Design
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

#ifndef _Transfer_VoidBinder_HeaderFile
#define _Transfer_VoidBinder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Transfer_Binder.hxx>


class Transfer_VoidBinder;
DEFINE_STANDARD_HANDLE(Transfer_VoidBinder, Transfer_Binder)

//! a VoidBinder is used to bind a starting item with a status,
//! error or warning messages, but no result
//! It is interpreted by TransferProcess, which admits a
//! VoidBinder to be over-written, and copies its check to the
//! new Binder
class Transfer_VoidBinder : public Transfer_Binder
{

public:

  
  //! a VoidBinder is not Multiple (Remark : it is not Simple too)
  //! But it can bring next results ...
  Standard_EXPORT Transfer_VoidBinder();
  
  //! while a VoidBinder admits no Result, its ResultType returns
  //! the type of <me>
  Standard_EXPORT Handle(Standard_Type) ResultType() const Standard_OVERRIDE;
  
  //! Returns "(void)"
  Standard_EXPORT Standard_CString ResultTypeName() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Transfer_VoidBinder,Transfer_Binder)

protected:




private:




};







#endif // _Transfer_VoidBinder_HeaderFile
