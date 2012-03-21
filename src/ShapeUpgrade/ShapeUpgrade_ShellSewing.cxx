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

//szv#4 S4163
#include <ShapeUpgrade_ShellSewing.ixx>

#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>

#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <ShapeAnalysis_ShapeTolerance.hxx>

//=======================================================================
//function : ShapeUpgrade_ShellSewing
//purpose  : 
//=======================================================================

ShapeUpgrade_ShellSewing::ShapeUpgrade_ShellSewing()
{
  myReShape = new ShapeBuild_ReShape;
}

//=======================================================================
//function : ApplySewing
//purpose  : 
//=======================================================================

void ShapeUpgrade_ShellSewing::Init (const TopoDS_Shape& shape) 
{
  if (shape.IsNull()) return;
  if (shape.ShapeType() == TopAbs_SHELL) myShells.Add (shape);
  else {
    for (TopExp_Explorer exs (shape,TopAbs_SHELL); exs.More(); exs.Next()) {
      myShells.Add (exs.Current());
    }
  }
}

//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================

Standard_Integer ShapeUpgrade_ShellSewing::Prepare (const Standard_Real tol) 
{
  Standard_Integer nb = myShells.Extent(), ns = 0;
  for ( Standard_Integer i = 1; i <= nb; i ++) {
    TopoDS_Shell sl = TopoDS::Shell ( myShells.FindKey (i) );
    BRepBuilderAPI_Sewing ss ( tol );
    TopExp_Explorer exp(sl,TopAbs_FACE);
    for (; exp.More(); exp.Next()) ss.Add(exp.Current());
    ss.Perform();
    TopoDS_Shape newsh = ss.SewedShape();
    if (!newsh.IsNull()) { myReShape->Replace (sl,newsh); ns ++; }
  }
  return ns;
}

//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

TopoDS_Shape ShapeUpgrade_ShellSewing::Apply (const TopoDS_Shape& shape, 
					      const Standard_Real tol) 
{
  if ( shape.IsNull() || myShells.Extent() == 0 ) return shape;

  TopoDS_Shape res = myReShape->Apply ( shape, TopAbs_FACE, 2 );

  //  A present orienter les solides correctement
  myReShape->Clear(); 
  Standard_Integer ns = 0;
  for (TopExp_Explorer exd (shape,TopAbs_SOLID); exd.More(); exd.Next()) {
    TopoDS_Solid sd = TopoDS::Solid ( exd.Current() );
    BRepClass3d_SolidClassifier bsc3d (sd);
    bsc3d.PerformInfinitePoint ( tol );
    if (bsc3d.State() == TopAbs_IN) { myReShape->Replace (sd,sd.Reversed()); ns++; }
  }

  //szv#4:S4163:12Mar99 optimized
  if (ns != 0) res = myReShape->Apply( res, TopAbs_SHELL, 2 );

  return res;
}

//=======================================================================
//function : ApplySewing
//purpose  : 
//=======================================================================

TopoDS_Shape ShapeUpgrade_ShellSewing::ApplySewing (const TopoDS_Shape& shape,
						    const Standard_Real tol) 
{
  if (shape.IsNull()) return shape;

  Standard_Real t = tol;
  if (t <= 0.) {
    ShapeAnalysis_ShapeTolerance stu;
    t = stu.Tolerance (shape,0);    // tolerance moyenne
  }

  Init ( shape );
  if ( Prepare ( t ) ) return Apply ( shape, t );

  return TopoDS_Shape();
}
