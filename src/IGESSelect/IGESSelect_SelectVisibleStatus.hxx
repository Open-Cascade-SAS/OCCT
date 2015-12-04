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

#ifndef _IGESSelect_SelectVisibleStatus_HeaderFile
#define _IGESSelect_SelectVisibleStatus_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExtract.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;


class IGESSelect_SelectVisibleStatus;
DEFINE_STANDARD_HANDLE(IGESSelect_SelectVisibleStatus, IFSelect_SelectExtract)

//! This selection looks at Blank Status of IGES Entities
//! Direct  selection keeps Visible Entities (Blank = 0),
//! Reverse selection keeps Blanked Entities (Blank = 1)
class IGESSelect_SelectVisibleStatus : public IFSelect_SelectExtract
{

public:

  
  //! Creates a SelectVisibleStatus
  Standard_EXPORT IGESSelect_SelectVisibleStatus();
  
  //! Returns True if <ent> is an IGES Entity with Blank Status = 0
  Standard_EXPORT Standard_Boolean Sort (const Standard_Integer rank, const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Returns the Selection criterium : "IGES Entity, Status Visible"
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SelectVisibleStatus,IFSelect_SelectExtract)

protected:




private:




};







#endif // _IGESSelect_SelectVisibleStatus_HeaderFile
