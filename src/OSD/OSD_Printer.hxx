// Created on: 2018-03-15
// Created by: Stephan GARNAUD (ARM)
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

#ifndef _OSD_Printer_HeaderFile
#define _OSD_Printer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_AsciiString.hxx>
#include <OSD_Error.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Standard_ConstructionError;
class Standard_NullObject;
class OSD_OSDError;
class TCollection_AsciiString;


//! Selects a printer (used by File).
class OSD_Printer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes printer to use with its name.
  //! The string must contain only ASCII characters
  //! between ' ' and '~'; this means no control character
  //! and no extended ASCII code. If it is not the case the
  //! exception ConstructionError is raised.
  Standard_EXPORT OSD_Printer(const TCollection_AsciiString& Name);
  
  //! Changes name of printer to use.
  Standard_EXPORT void SetName (const TCollection_AsciiString& Name);
  
  //! Returns name of current printer
  Standard_EXPORT void Name (TCollection_AsciiString& Name) const;
  
  //! Returns TRUE if an error occurs
  Standard_EXPORT Standard_Boolean Failed() const;
  
  //! Resets error counter to zero
  Standard_EXPORT void Reset();
  
  //! Raises OSD_Error
  Standard_EXPORT void Perror();
  
  //! Returns error number if 'Failed' is TRUE.
  Standard_EXPORT Standard_Integer Error() const;




protected:





private:



  TCollection_AsciiString myName;
  OSD_Error myError;


};







#endif // _OSD_Printer_HeaderFile
