// Created on: 1992-08-18
// Created by: Arnaud BOUZY
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

#ifndef _ExprIntrp_GenFct_HeaderFile
#define _ExprIntrp_GenFct_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <ExprIntrp_Generator.hxx>
class TCollection_AsciiString;


class ExprIntrp_GenFct;
DEFINE_STANDARD_HANDLE(ExprIntrp_GenFct, ExprIntrp_Generator)

//! Implements an interpreter for defining functions.
//! All its functionalities can be found in class
//! GenExp.
class ExprIntrp_GenFct : public ExprIntrp_Generator
{

public:

  
  Standard_EXPORT static Handle(ExprIntrp_GenFct) Create();
  
  Standard_EXPORT void Process (const TCollection_AsciiString& str);
  
  Standard_EXPORT Standard_Boolean IsDone() const;




  DEFINE_STANDARD_RTTIEXT(ExprIntrp_GenFct,ExprIntrp_Generator)

protected:




private:

  
  Standard_EXPORT ExprIntrp_GenFct();

  Standard_Boolean done;


};







#endif // _ExprIntrp_GenFct_HeaderFile
