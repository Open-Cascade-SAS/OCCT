// File:	Units_Operators.hxx
// Created:	Mon May 14 11:58:40 2007
// Author:	data exchange team
//		<det@kex.nnov.opencascade.com>


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
