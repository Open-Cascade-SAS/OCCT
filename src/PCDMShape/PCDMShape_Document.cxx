// Created on: 1998-01-08
// Created by: Isabelle GRIGNON
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



#include <PCDMShape_Document.ixx>


//=======================================================================
//function : PCDMShape_Document
//purpose  : 
//=======================================================================

PCDMShape_Document::PCDMShape_Document()
{}


//=======================================================================
//function : PCDMShape_Document
//purpose  : 
//=======================================================================

PCDMShape_Document::PCDMShape_Document(const PTopoDS_Shape1& T)
: myShape(T)
{}


//=======================================================================
//function : TShape
//purpose  : 
//=======================================================================

const PTopoDS_Shape1 PCDMShape_Document::Shape()const 
{ return myShape; }


//=======================================================================
//function : TShape
//purpose  : 
//=======================================================================

void  PCDMShape_Document::Shape(const PTopoDS_Shape1& T)
{ myShape = T; }
