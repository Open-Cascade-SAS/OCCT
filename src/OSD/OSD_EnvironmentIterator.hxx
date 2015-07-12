// Created on: 1992-09-11
// Created by: Stephan GARNAUD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _OSD_EnvironmentIterator_HeaderFile
#define _OSD_EnvironmentIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <Standard_Integer.hxx>
#include <OSD_Error.hxx>
#include <Standard_Boolean.hxx>
class OSD_OSDError;
class OSD_Environment;


//! This allows consultation of every environment variable.
//! There is no specific order of results.
class OSD_EnvironmentIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Instantiates Object as Iterator;
  Standard_EXPORT OSD_EnvironmentIterator();
  
  Standard_EXPORT void Destroy();
~OSD_EnvironmentIterator()
{
  Destroy();
}
  
  //! Returns TRUE if there are other environment variables.
  Standard_EXPORT Standard_Boolean More();
  
  //! Sets the iterator to the next item.
  //! Returns the item value corresponding to the current
  //! position of the iterator.
  Standard_EXPORT void Next();
  
  //! Returns the next environment variable found.
  Standard_EXPORT OSD_Environment Values();
  
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



  Standard_Address myEnv;
  Standard_Integer myCount;
  OSD_Error myError;


};







#endif // _OSD_EnvironmentIterator_HeaderFile
