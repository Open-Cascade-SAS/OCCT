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


#ifndef _TopOpeBRepDS_reDEB_HeaderFile
#define _TopOpeBRepDS_reDEB_HeaderFile

#ifdef DEB
#include <TopOpeBRepDS_define.hxx>
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettracePEI();
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettracePI();
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSPSX(const Standard_Integer);
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceEDPR();
Standard_EXPORT Standard_Boolean DSREDUEDGETRCE(const Standard_Integer);
Standard_EXPORT void debreducerE(const Standard_Integer);
Standard_EXPORT void debreducerEP(const Standard_Integer);
Standard_EXPORT void debreducerEV(const Standard_Integer,const Standard_Integer);
Standard_EXPORT void debreducer3d(const Standard_Integer);
Standard_EXPORT void debredpvg(const Standard_Integer);
#endif

#endif
