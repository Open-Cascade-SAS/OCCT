// Created on: 1998-04-02
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _IGESSelect_SignLevelNumber_HeaderFile
#define _IGESSelect_SignLevelNumber_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <IFSelect_Signature.hxx>
#include <Standard_CString.hxx>
class Standard_Transient;
class Interface_InterfaceModel;


class IGESSelect_SignLevelNumber;
DEFINE_STANDARD_HANDLE(IGESSelect_SignLevelNumber, IFSelect_Signature)

//! Gives D.E. Level Number under two possible forms :
//! * for counter : "LEVEL nnnnnnn", " NO LEVEL", " LEVEL LIST"
//! * for selection : "/nnn/", "/0/", "/1/2/nnn/"
//!
//! For matching, giving /nn/ gets any entity attached to level nn
//! whatever simple or in a level list
class IGESSelect_SignLevelNumber : public IFSelect_Signature
{

public:

  
  //! Creates a SignLevelNumber
  //! <countmode> True : values are naturally displayed
  //! <countmode> False: values are separated by slashes
  //! in order to allow selection by signature by Draw or C++
  Standard_EXPORT IGESSelect_SignLevelNumber(const Standard_Boolean countmode);
  
  //! Returns the value (see above)
  Standard_EXPORT Standard_CString Value (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SignLevelNumber,IFSelect_Signature)

protected:




private:


  Standard_Boolean thecountmode;


};







#endif // _IGESSelect_SignLevelNumber_HeaderFile
