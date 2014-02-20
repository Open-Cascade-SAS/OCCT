// Created on: 2007-05-14
// Created by: data exchange team
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef Units_Operators_HeaderFile
#define Units_Operators_HeaderFile

#include <Units_Token.hxx>
#include <Units_Unit.hxx>
#include <Units_Quantity.hxx>

Standard_EXPORT Standard_Boolean operator ==(const Handle(Units_Quantity)&,const Standard_CString);
Standard_EXPORT Standard_Boolean operator ==(const Handle(Units_Token)&,const Standard_CString);
Standard_EXPORT Standard_Boolean operator ==(const Handle(Units_Unit)&,const Standard_CString);
     
Standard_EXPORT Handle(Units_Dimensions) operator *(const Handle(Units_Dimensions)&,const Handle(Units_Dimensions)&);
Standard_EXPORT Handle(Units_Dimensions) operator /(const Handle(Units_Dimensions)&,const Handle(Units_Dimensions)&);
     
Standard_EXPORT Handle(Units_Dimensions) pow(const Handle(Units_Dimensions)&,const Standard_Real);
Standard_EXPORT Handle(Units_Token) pow(const Handle(Units_Token)&,const Handle(Units_Token)&);
Standard_EXPORT Handle(Units_Token) pow(const Handle(Units_Token)&,const Standard_Real);
     
Standard_EXPORT Handle(Units_Token) operator +(const Handle(Units_Token)&,const Standard_Integer);
Standard_EXPORT Handle(Units_Token) operator +(const Handle(Units_Token)&,const Handle(Units_Token)&);
Standard_EXPORT Handle(Units_Token) operator -(const Handle(Units_Token)&,const Handle(Units_Token)&);
Standard_EXPORT Handle(Units_Token) operator *(const Handle(Units_Token)&,const Handle(Units_Token)&);
Standard_EXPORT Handle(Units_Token) operator /(const Handle(Units_Token)&,const Handle(Units_Token)&);

Standard_EXPORT Standard_Boolean operator !=(const Handle(Units_Token)&,const Standard_CString);
Standard_EXPORT Standard_Boolean operator <=(const Handle(Units_Token)&,const Standard_CString);
Standard_EXPORT Standard_Boolean operator >(const Handle(Units_Token)&,const Standard_CString);
Standard_EXPORT Standard_Boolean operator >(const Handle(Units_Token)&,const Handle(Units_Token)&);
Standard_EXPORT Standard_Boolean operator >=(const Handle(Units_Token)&,const Handle(Units_Token)&);
     
#endif
