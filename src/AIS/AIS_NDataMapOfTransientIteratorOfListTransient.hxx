// Created on: 2003-05-04
// Created by: Alexander Grigoriev (a-grigoriev@opencascade.com)
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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


#ifndef _AIS_NDataMapOfTransientIteratorOfListTransient_HeaderFile
#define _AIS_NDataMapOfTransientIteratorOfListTransient_HeaderFile

#include <AIS_NListIteratorOfListTransient.hxx>
#include <NCollection_DefineDataMap.hxx>

inline Standard_Boolean IsEqual (const Handle_Standard_Transient& theH1,
				 const Handle_Standard_Transient& theH2)
{
  return (theH1 == theH2);
}

DEFINE_BASECOLLECTION(AIS_BaseCollItListTransient,
                      AIS_NListIteratorOfListTransient)
DEFINE_DATAMAP       (AIS_NDataMapOfTransientIteratorOfListTransient,
                      AIS_BaseCollItListTransient,
                      Handle(Standard_Transient),
                      AIS_NListIteratorOfListTransient)

#endif
