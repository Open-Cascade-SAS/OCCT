// Created on: 1998-10-14
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

#ifdef OCCT_DEBUG
#include <TopOpeBRepTool_KRO.hxx>
Standard_IMPORT TOPKRO KRO_DSFILLER_TOTAL;
Standard_IMPORT TOPKRO KRO_DSFILLER_INTFF;
Standard_IMPORT TOPKRO KRO_DSFILLER_INTEE;
Standard_IMPORT TOPKRO KRO_DSFILLER_INTFE;
void PrintKRO_PREPA() {
  KRO_DSFILLER_TOTAL.Print(cout); cout<<endl;
  KRO_DSFILLER_INTFF.Print(cout); cout<<endl;
  KRO_DSFILLER_INTEE.Print(cout); cout<<endl;
  KRO_DSFILLER_INTFE.Print(cout); cout<<endl;
//  KRO_CURVETOOL_APPRO.Print(cout); cout<<endl;
}
#endif
