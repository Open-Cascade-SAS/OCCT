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

//    rln 22.03.99: syntax correction in CombineVertex
//szv#4 S4163
#include <ShapeBuild_Vertex.ixx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <BRep_Builder.hxx>

//=======================================================================
//function : CombineVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ShapeBuild_Vertex::CombineVertex (const TopoDS_Vertex& V1,
						const TopoDS_Vertex& V2,
						const Standard_Real tolFactor) const
{
  return CombineVertex ( BRep_Tool::Pnt ( V1 ), BRep_Tool::Pnt ( V2 ), 
			 BRep_Tool::Tolerance ( V1 ), BRep_Tool::Tolerance ( V2 ), 
			 tolFactor );
}

//=======================================================================
//function : CombineVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ShapeBuild_Vertex::CombineVertex (const gp_Pnt& pnt1, 
						const gp_Pnt& pnt2,
						const Standard_Real tol1, 
						const Standard_Real tol2,
						const Standard_Real tolFactor) const
{
  gp_Pnt pos;
  Standard_Real tol;
  
  gp_Vec v = pnt2.XYZ() - pnt1.XYZ();
  Standard_Real dist = v.Magnitude();

  //#47 rln 09.12.98 S4054 PRO14323 entity 2844
  if ( dist + tol2 <= tol1 ) {
    pos = pnt1;
    tol = tol1;
  }
  else if ( dist + tol1 <= tol2 ) {
    pos = pnt2;
    tol = tol2;
  }
  else {
    tol = 0.5 * ( dist + tol1 + tol2 );
    Standard_Real s = ( dist > 0. )? ( tol2 - tol1 ) / dist : 0.; //szv#4:S4163:12Mar99 anti-exception
    pos = 0.5 * ( ( 1 - s ) * pnt1.XYZ() + ( 1 + s ) * pnt2.XYZ() );
  }
  
  TopoDS_Vertex V;
  BRep_Builder B;
  B.MakeVertex ( V, pos, tolFactor * tol );
  return V;
}

