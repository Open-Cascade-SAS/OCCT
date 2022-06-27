// Created on: 1999-09-17
// Created by: Denis PASCAL
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TDocStd_PathParser_HeaderFile
#define _TDocStd_PathParser_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_ExtendedString.hxx>


//! parse an OS path
class TDocStd_PathParser 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TDocStd_PathParser(const TCollection_ExtendedString& path);
  
  Standard_EXPORT void Parse();
  
  Standard_EXPORT TCollection_ExtendedString Trek() const;
  
  Standard_EXPORT TCollection_ExtendedString Name() const;
  
  Standard_EXPORT TCollection_ExtendedString Extension() const;
  
  Standard_EXPORT TCollection_ExtendedString Path() const;
  
  Standard_EXPORT Standard_Integer Length() const;




protected:





private:



  TCollection_ExtendedString myPath;
  TCollection_ExtendedString myExtension;
  TCollection_ExtendedString myTrek;
  TCollection_ExtendedString myName;


};







#endif // _TDocStd_PathParser_HeaderFile
