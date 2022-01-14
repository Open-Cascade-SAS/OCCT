// Created on: 1993-04-08
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

#ifndef _Interface_CopyControl_HeaderFile
#define _Interface_CopyControl_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>


class Interface_CopyControl;
DEFINE_STANDARD_HANDLE(Interface_CopyControl, Standard_Transient)

//! This deferred class describes the services required by
//! CopyTool to work. They are very simple and correspond
//! basically to the management of an indexed map.
//! But they can be provided by various classes which can
//! control a Transfer. Each Starting Entity have at most
//! one Result (Mapping one-one)
class Interface_CopyControl : public Standard_Transient
{

public:

  
  //! Clears List of Copy Results. Gets Ready to begin another Copy
  //! Process.
  Standard_EXPORT virtual void Clear() = 0;
  
  //! Bind a Result to a Starting Entity identified by its Number
  Standard_EXPORT virtual void Bind (const Handle(Standard_Transient)& ent, const Handle(Standard_Transient)& res) = 0;
  
  //! Searches for the Result bound to a Startingf Entity identified
  //! by its Number.
  //! If Found, returns True and fills <res>
  //! Else, returns False and nullifies <res>
  Standard_EXPORT virtual Standard_Boolean Search (const Handle(Standard_Transient)& ent, Handle(Standard_Transient)& res) const = 0;




  DEFINE_STANDARD_RTTIEXT(Interface_CopyControl,Standard_Transient)

protected:




private:




};







#endif // _Interface_CopyControl_HeaderFile
