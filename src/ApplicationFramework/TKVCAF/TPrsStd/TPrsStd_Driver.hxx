// Created on: 1997-08-01
// Created by: SMO
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TPrsStd_Driver_HeaderFile
#define _TPrsStd_Driver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TDF_Label;
class AIS_InteractiveObject;

class TPrsStd_Driver;
DEFINE_STANDARD_HANDLE(TPrsStd_Driver, Standard_Transient)

//! Driver for AIS
//! ==============
//! An abstract class, which - in classes inheriting
//! from it - allows you to update an
//! AIS_InteractiveObject or create one if one does
//! not already exist.
//! For both creation and update, the interactive
//! object is filled with information contained in
//! attributes. These attributes are those found on
//! the label given as an argument in the method Update.
//! true is returned if the interactive object was modified by the update.
//! This class  provide  an algorithm  to  Build with its  default
//! values (if Null) or Update (if !Null) an AIS_InteractiveObject
//! .   Resources are found  in  attributes associated to  a given
//! label.
class TPrsStd_Driver : public Standard_Transient
{

public:
  //! Updates the interactive object ais with
  //! information found on the attributes associated with the label L.
  Standard_EXPORT virtual Standard_Boolean Update(const TDF_Label&               L,
                                                  Handle(AIS_InteractiveObject)& ais) = 0;

  DEFINE_STANDARD_RTTIEXT(TPrsStd_Driver, Standard_Transient)

protected:
  Standard_EXPORT TPrsStd_Driver();

private:
};

#endif // _TPrsStd_Driver_HeaderFile
