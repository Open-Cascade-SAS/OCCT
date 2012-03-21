// Created on: 2007-05-14
// Created by: data exchange team
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
