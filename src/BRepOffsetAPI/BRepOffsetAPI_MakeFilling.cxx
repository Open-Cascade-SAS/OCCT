// Created on: 1998-08-26
// Created by: Julia GERASIMOVA
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



#include <BRepOffsetAPI_MakeFilling.ixx>


//=======================================================================
//function : Constructor
//purpose  : 
//======================================================================
BRepOffsetAPI_MakeFilling::BRepOffsetAPI_MakeFilling( const Standard_Integer Degree,
					  const Standard_Integer NbPtsOnCur,
					  const Standard_Integer NbIter,
					  const Standard_Boolean Anisotropie,
					  const Standard_Real Tol2d,
					  const Standard_Real Tol3d,
					  const Standard_Real TolAng,
					  const Standard_Real TolCurv,
					  const Standard_Integer MaxDeg,
					  const Standard_Integer MaxSegments ) :
myFilling( Degree, NbPtsOnCur, NbIter, Anisotropie, Tol2d, Tol3d, TolAng, TolCurv, MaxDeg, MaxSegments )
{
}

//=======================================================================
//function : SetConstrParam
//purpose  : 
//======================================================================
void BRepOffsetAPI_MakeFilling::SetConstrParam( const Standard_Real Tol2d,
					  const Standard_Real Tol3d,
					  const Standard_Real TolAng,
					  const Standard_Real TolCurv ) 
{
  myFilling.SetConstrParam( Tol2d, Tol3d, TolAng, TolCurv );
}

//=======================================================================
//function : SetResolParam
//purpose  : 
//======================================================================
void BRepOffsetAPI_MakeFilling::SetResolParam( const Standard_Integer Degree,
					 const Standard_Integer NbPtsOnCur,
					 const Standard_Integer NbIter,
					 const Standard_Boolean Anisotropie ) 
{
  myFilling.SetResolParam( Degree, NbPtsOnCur, NbIter, Anisotropie );
}

//=======================================================================
//function : SetApproxParam
//purpose  : 
//======================================================================
void BRepOffsetAPI_MakeFilling::SetApproxParam( const Standard_Integer MaxDeg,
					  const Standard_Integer MaxSegments ) 
{
  myFilling.SetApproxParam( MaxDeg, MaxSegments );
}

//=======================================================================
//function : LoadInitSurface
//purpose  : 
//======================================================================
void BRepOffsetAPI_MakeFilling::LoadInitSurface( const TopoDS_Face& Surf ) 
{
  myFilling.LoadInitSurface( Surf );
}

//=======================================================================
//function : Add
//purpose  : adds an edge as a constraint
//======================================================================
Standard_Integer BRepOffsetAPI_MakeFilling::Add( const TopoDS_Edge& Constr,
			       const GeomAbs_Shape Order,
			       const Standard_Boolean IsBound ) 
{
  return  myFilling.Add( Constr, Order, IsBound );
}

//=======================================================================
//function : Add
//purpose  : adds an edge with supporting face as a constraint
//======================================================================
Standard_Integer BRepOffsetAPI_MakeFilling::Add( const TopoDS_Edge& Constr,
			       const TopoDS_Face& Support,
			       const GeomAbs_Shape Order,
			       const Standard_Boolean IsBound ) 
{
  return  myFilling.Add( Constr, Support, Order, IsBound );
}

//=======================================================================
//function : Add
//purpose  : adds a "free constraint": face without edge
//======================================================================
Standard_Integer BRepOffsetAPI_MakeFilling::Add( const TopoDS_Face& Support,
			       const GeomAbs_Shape Order ) 
{
  return myFilling.Add( Support, Order );
}

//=======================================================================
//function : Add
//purpose  : adds a point constraint
//======================================================================
Standard_Integer BRepOffsetAPI_MakeFilling::Add( const gp_Pnt& Point )
{
  return myFilling.Add( Point );
}

//=======================================================================
//function : Add
//purpose  : adds a point constraint on a face
//======================================================================
Standard_Integer BRepOffsetAPI_MakeFilling::Add( const Standard_Real U,
			       const Standard_Real V,
			       const TopoDS_Face& Support,
			       const GeomAbs_Shape Order )
{
  return myFilling.Add( U, V, Support, Order );
}


//=======================================================================
//function : Build
//purpose  : builds the resulting face
//======================================================================
void BRepOffsetAPI_MakeFilling::Build() 
{
  myFilling.Build();
  myShape = myFilling.Face();
}

//=======================================================================
//function : IsDone
//purpose  : 
//======================================================================
Standard_Boolean BRepOffsetAPI_MakeFilling::IsDone() const
{
  return myFilling.IsDone();
}

//=======================================================================
//function : Generated
//purpose  : returns the new edge (first in list) made from old edge "S"
//=======================================================================
const TopTools_ListOfShape& BRepOffsetAPI_MakeFilling::Generated(const TopoDS_Shape& S)
{
  return myFilling.Generated(S);
}

//==========================================================================
//function : G0Error
//purpose  : returns maximum distance from boundary to the resulting surface
//==========================================================================
Standard_Real BRepOffsetAPI_MakeFilling::G0Error() const
{
  return myFilling.G0Error();
}

//=======================================================================
//function : G1Error
//purpose  : returns maximum angle between the resulting surface
//           and constraint surfaces at boundaries
//======================================================================
Standard_Real BRepOffsetAPI_MakeFilling::G1Error() const
{
  return myFilling.G1Error();
}

//=======================================================================
//function : G2Error
//purpose  : returns maximum difference of curvature between
//           the resulting surface and constraint surfaces at boundaries
//======================================================================
Standard_Real BRepOffsetAPI_MakeFilling::G2Error() const
{
  return myFilling.G2Error();
}

//==========================================================================
//function : G0Error
//purpose  : returns maximum distance between the constraint number Index
//           and the resulting surface
//==========================================================================
Standard_Real BRepOffsetAPI_MakeFilling::G0Error( const Standard_Integer Index )
{
  return myFilling.G0Error( Index );
}

//==========================================================================
//function : G1Error
//purpose  : returns maximum angle between the constraint number Index
//           and the resulting surface
//==========================================================================
Standard_Real BRepOffsetAPI_MakeFilling::G1Error( const Standard_Integer Index )
{
  return myFilling.G1Error( Index );
}

//==========================================================================
//function : G2Error
//purpose  : returns maximum difference of curvature between
//           the constraint number Index and the resulting surface
//==========================================================================
Standard_Real BRepOffsetAPI_MakeFilling::G2Error( const Standard_Integer Index )
{
  return myFilling.G2Error( Index );
}
