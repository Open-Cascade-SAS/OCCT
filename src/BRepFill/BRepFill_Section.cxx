// Created on: 1998-07-22
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



#include <BRepFill_Section.ixx>

#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Edge.hxx>


BRepFill_Section::BRepFill_Section() :islaw(0), contact(0), correction(0)
{
}


BRepFill_Section::BRepFill_Section(const TopoDS_Shape& Profile,
				   const TopoDS_Vertex& V,
				   const Standard_Boolean WithContact,
				   const Standard_Boolean WithCorrection) 
                                  : vertex(V),
				    islaw(0),contact(WithContact),
				    correction(WithCorrection)
{
  if (Profile.ShapeType() == TopAbs_WIRE)
    wire = TopoDS::Wire(Profile);
  else if (Profile.ShapeType() == TopAbs_VERTEX)
    {
      TopoDS_Vertex aVertex = TopoDS::Vertex(Profile);
      BRep_Builder BB;
      
      TopoDS_Edge DegEdge;
      BB.MakeEdge( DegEdge );
      BB.Add( DegEdge, aVertex.Oriented(TopAbs_FORWARD) );
      BB.Add( DegEdge, aVertex.Oriented(TopAbs_REVERSED) );
      BB.Degenerated( DegEdge, Standard_True );
      DegEdge.Closed( Standard_True );
      
      BB.MakeWire( wire );
      BB.Add( wire, DegEdge );
      wire.Closed( Standard_True );
    }
  else
    Standard_Failure::Raise("BRepFill_Section: bad shape type of section");
}

void BRepFill_Section::Set(const Standard_Boolean IsLaw)
{
  islaw = IsLaw;
}
