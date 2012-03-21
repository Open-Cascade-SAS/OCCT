// Created on: 1999-11-22
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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


// Taken from Interface_Macros to avoid cyclic dependency from Shape Healing

#ifndef MoniTool_Macros_HeaderFile
#define MoniTool_Macros_HeaderFile

// Interface General purpose Macros
// Their use is not required, but it gets some program parts easier :
// DownCasting, with or without Declaration

//  DownCasting to a "Handle" already declared
#define GetCasted(atype,start)  Handle(atype)::DownCast(start)

//  DownCasting with Declaration :
//  - Declares the variable result
//  - then performs DownCasting
//  - after it, result can be used as a new variable
#define DeclareAndCast(atype,result,start) \
Handle(atype) result = Handle(atype)::DownCast(start)

#define FastCast(atype,result,start) \
 Handle(atype) result;  result = (*(Handle(atype)*))& start


#endif
