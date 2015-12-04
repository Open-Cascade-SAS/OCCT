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

#ifndef _TPrsStd_NamedShapeDriver_HeaderFile
#define _TPrsStd_NamedShapeDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TPrsStd_Driver.hxx>
#include <Standard_Boolean.hxx>
class TDF_Label;
class AIS_InteractiveObject;


class TPrsStd_NamedShapeDriver;
DEFINE_STANDARD_HANDLE(TPrsStd_NamedShapeDriver, TPrsStd_Driver)

//! An implementation of TPrsStd_Driver for named shapes.
class TPrsStd_NamedShapeDriver : public TPrsStd_Driver
{

public:

  
  //! Constructs an empty named shape driver.
  Standard_EXPORT TPrsStd_NamedShapeDriver();
  
  //! Build the AISObject (if null) or update it.
  //! No compute is done.
  //! Returns <True> if informations was found
  //! and AISObject updated.
  Standard_EXPORT virtual Standard_Boolean Update (const TDF_Label& aLabel, Handle(AIS_InteractiveObject)& anAISObject) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TPrsStd_NamedShapeDriver,TPrsStd_Driver)

protected:




private:




};







#endif // _TPrsStd_NamedShapeDriver_HeaderFile
