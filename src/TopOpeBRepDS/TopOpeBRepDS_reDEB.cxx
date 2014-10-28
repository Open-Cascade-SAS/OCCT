// Created on: 1998-02-14
// Created by: Jean Yves LEBEY
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

#include <TopOpeBRepDS_reDEB.hxx>
#include <TopOpeBRepDS_define.hxx>

#ifdef OCCT_DEBUG
Standard_EXPORT Standard_Boolean DSREDUEDGETRCE(const Standard_Integer EIX) {
  Standard_Boolean b1 = TopOpeBRepDS_GettracePEI();
  Standard_Boolean b2 = TopOpeBRepDS_GettracePI();
  Standard_Boolean b3 = TopOpeBRepDS_GettraceSPSX(EIX);
  return (b1 || b2 || b3);
}
Standard_EXPORT void debreducerE(const Standard_Integer ie)
{cout<<"+++ debreducerE e"<<ie<<endl;}
Standard_EXPORT void debreducerEP(const Standard_Integer ie,const Standard_Integer ip)
{cout<<"+++ debreducerPEI e"<<ie<<" P"<<ip<<endl;}
Standard_EXPORT void debreducerEV(const Standard_Integer ie,const Standard_Integer iv)
{cout<<"+++ debreducerVEI e"<<ie<<" V"<<iv<<endl;}
Standard_EXPORT void debreducer3d(const Standard_Integer ie)
{cout<<"+++ debreducer3d e"<<ie<<endl;}
Standard_EXPORT void debredpvg(const Standard_Integer ie)
{cout<<"+++ debredpvg e"<<ie<<endl;}
#endif
