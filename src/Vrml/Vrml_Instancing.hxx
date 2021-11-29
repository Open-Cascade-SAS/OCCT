// Created on: 1997-02-05
// Created by: Alexander BRIVIN
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

#ifndef _Vrml_Instancing_HeaderFile
#define _Vrml_Instancing_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_OStream.hxx>


//! defines  "instancing" - using  the  same  instance  of  a  node
//! multiple  times.
//! It  is  accomplished  by  using  the  "DEF"  and  "USE"  keywords.
//! The  DEF  keyword  both  defines  a  named  node,  and  creates  a  single
//! instance  of  it.
//! The  USE  keyword  indicates  that  the  most  recently  defined  instance
//! should  be  used  again.
//! If  several  nades  were  given  the  same  name,  then  the  last  DEF
//! encountered  during  parsing  "wins".
//! DEF/USE  is  limited  to  a  single  file.
class Vrml_Instancing 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds "DEF  <myName>" in  anOStream  (VRML  file).
  Standard_EXPORT Vrml_Instancing(const TCollection_AsciiString& aString);
  
  //! Adds "USE  <myName>" in  anOStream (VRML  file).
  Standard_EXPORT Standard_OStream& DEF (Standard_OStream& anOStream) const;
  
  Standard_EXPORT Standard_OStream& USE (Standard_OStream& anOStream) const;




protected:





private:



  TCollection_AsciiString myName;


};







#endif // _Vrml_Instancing_HeaderFile
