// Created on: 1998-02-14
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#include <TopOpeBRepDS_reDEB.hxx>
#include <TopOpeBRepDS_define.hxx>

#ifdef DEB
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
