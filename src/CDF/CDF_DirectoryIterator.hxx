// Created on: 1997-08-13
// Created by: Jean-Louis Frenkel
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

#ifndef _CDF_DirectoryIterator_HeaderFile
#define _CDF_DirectoryIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <CDM_ListIteratorOfListOfDocument.hxx>
class CDF_Directory;
class CDM_Document;



class CDF_DirectoryIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates an Iterator with the directory
  //! of the current CDF.
  Standard_EXPORT CDF_DirectoryIterator();
  
  Standard_EXPORT CDF_DirectoryIterator(const Handle(CDF_Directory)& aDirectory);
  
  //! Returns True if there are more entries to return
  Standard_EXPORT Standard_Boolean MoreDocument();
  
  //! Go to the next entry
  //! (if there is not, Value will raise an exception)
  Standard_EXPORT void NextDocument();
  
  //! Returns item value of current entry
  Standard_EXPORT Handle(CDM_Document) Document();




protected:





private:



  CDM_ListIteratorOfListOfDocument myIterator;


};







#endif // _CDF_DirectoryIterator_HeaderFile
