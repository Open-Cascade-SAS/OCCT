// Created on: 1998-01-07
// Created by: Philippe MANGIN
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



#include <BRepFill_SectionLaw.ixx>

#include <BRepTools_WireExplorer.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepLProp.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomFill_UniformSection.hxx>

#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>


#include <Precision.hxx>


//=======================================================================
//function : NbLaw
//purpose  : Gives the number of elementary (or Geometric) law
//=======================================================================
Standard_Integer BRepFill_SectionLaw::NbLaw() const
{
  return myLaws->Length();
}


//=======================================================================
//function : Law
//purpose  : 
//=======================================================================
 const Handle(GeomFill_SectionLaw)& 
 BRepFill_SectionLaw::Law(const Standard_Integer Index) const
{
  return myLaws->Value(Index);
}

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================
 Standard_Boolean BRepFill_SectionLaw::IsUClosed() const
{
  return uclosed;
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================
 Standard_Boolean BRepFill_SectionLaw::IsVClosed() const
{
  return vclosed;
}

//=======================================================================
//function : Init
//purpose  : Prepare the parsing of a wire
//=======================================================================
 void BRepFill_SectionLaw::Init(const TopoDS_Wire& W)
{
  myIterator.Init(W);
}

//=======================================================================
//function : 
//purpose  : Parses the wire omitting the degenerated Edges
//=======================================================================
 TopoDS_Edge BRepFill_SectionLaw::CurrentEdge() 
{
  TopoDS_Edge E;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool B;
  Standard_Boolean Suivant = Standard_False;
  if (myIterator.More()) {
    E =  myIterator.Current();
//    Next = (B.Degenerated(E));
    Suivant = (BRep_Tool::Degenerated(E));
  }

  while (Suivant) {
     myIterator.Next();
     E = myIterator.Current();
//    Next = (B.Degenerated(E) && myIterator.More());
     Suivant = (BRep_Tool::Degenerated(E) && myIterator.More());
   }

  if (myIterator.More()) myIterator.Next();
  return E;
}

