// Created on: 1998-05-20
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

#ifndef _MoniTool_SignText_HeaderFile
#define _MoniTool_SignText_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
class TCollection_AsciiString;
class Standard_Transient;


class MoniTool_SignText;
DEFINE_STANDARD_HANDLE(MoniTool_SignText, Standard_Transient)

//! Provides the basic service to get a text which identifies
//! an object in a context
//! It can be used for other classes (general signatures ...)
//! It can also be used to build a message in which an object
//! is to be identified
class MoniTool_SignText : public Standard_Transient
{

public:

  
  //! Returns an identification of the Signature (a word), given at
  //! initialization time
  Standard_EXPORT virtual Standard_CString Name() const = 0;
  
  //! Gives a text as a signature for a transient object alone, i.e.
  //! without defined context.
  //! By default, calls Text with undefined context (Null Handle) and
  //! if empty, then returns DynamicType
  Standard_EXPORT virtual TCollection_AsciiString TextAlone (const Handle(Standard_Transient)& ent) const;
  
  //! Gives a text as a signature for a transient object in a context
  //! If the context is senseless, it can be given as Null Handle
  //! empty result if nothing to give (at least the DynamicType could
  //! be sent ?)
  Standard_EXPORT virtual TCollection_AsciiString Text (const Handle(Standard_Transient)& ent, const Handle(Standard_Transient)& context) const = 0;




  DEFINE_STANDARD_RTTIEXT(MoniTool_SignText,Standard_Transient)

protected:




private:




};







#endif // _MoniTool_SignText_HeaderFile
