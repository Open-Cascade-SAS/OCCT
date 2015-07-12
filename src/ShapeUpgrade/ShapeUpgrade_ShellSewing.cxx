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

//szv#4 S4163

#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeUpgrade_ShellSewing.hxx>
#include <ShapeUpgrade_WireDivide.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

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
