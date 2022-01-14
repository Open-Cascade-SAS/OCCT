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

#ifndef _IGESSelect_SelectName_HeaderFile
#define _IGESSelect_SelectName_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExtract.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;


class IGESSelect_SelectName;
DEFINE_STANDARD_HANDLE(IGESSelect_SelectName, IFSelect_SelectExtract)

//! Selects Entities which have a given name.
//! Consider Property Name if present, else Short Label, but
//! not the Subscript Number
//! First version : keeps exact name
//! Later : regular expression
class IGESSelect_SelectName : public IFSelect_SelectExtract
{

public:

  
  //! Creates an empty SelectName : every entity is considered
  //! good (no filter active)
  Standard_EXPORT IGESSelect_SelectName();
  
  //! Returns True if Name of Entity complies with Name Filter
  Standard_EXPORT Standard_Boolean Sort (const Standard_Integer rank, const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Sets a Name as a criterium : IGES Entities which have this name
  //! are kept (without regular expression, there should be at most
  //! one). <name> can be regarded as a Text Parameter
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& name);
  
  //! Returns the Name used as Filter
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  //! Returns the Selection criterium : "IGES Entity, Name : <name>"
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SelectName,IFSelect_SelectExtract)

protected:




private:


  Handle(TCollection_HAsciiString) thename;


};







#endif // _IGESSelect_SelectName_HeaderFile
