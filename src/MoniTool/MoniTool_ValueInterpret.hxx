// Created on: 2000-02-28
// Created by: data exchange team
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#ifndef MoniTool_ValueInterpret_HeaderFile
#define MoniTool_ValueInterpret_HeaderFile

#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Handle_MoniTool_TypedValue.hxx>

typedef Handle(TCollection_HAsciiString)  (*MoniTool_ValueInterpret) (const Handle(MoniTool_TypedValue)& typval,
								      const Handle(TCollection_HAsciiString)& val,
								      const Standard_Boolean native);

Standard_EXPORT Handle(Standard_Type)& STANDARD_TYPE(MoniTool_ValueInterpret);

#endif
