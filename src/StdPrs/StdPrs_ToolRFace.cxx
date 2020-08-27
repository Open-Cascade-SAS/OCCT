// Copyright (c) 1995-1999 Matra Datavision
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

#include <StdPrs_ToolRFace.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <TopoDS.hxx>

//=======================================================================
//function : StdPrs_ToolRFace
//purpose  :
//=======================================================================
StdPrs_ToolRFace::StdPrs_ToolRFace()
: myHasNullCurves (Standard_False)
{
}

//=======================================================================
//function : StdPrs_ToolRFace
//purpose  :
//=======================================================================
StdPrs_ToolRFace::StdPrs_ToolRFace (const Handle(BRepAdaptor_HSurface)& theSurface)
: myFace (theSurface->ChangeSurface().Face()),
  myHasNullCurves (Standard_False)
{
  myFace.Orientation(TopAbs_FORWARD);
}

//=======================================================================
//function : Edge
//purpose  :
//=======================================================================
const TopoDS_Edge& StdPrs_ToolRFace::Edge() const
{
  return TopoDS::Edge (myExplorer.Current());
}

//=======================================================================
//function : next
//purpose  :
//=======================================================================
void StdPrs_ToolRFace::next()
{
  Standard_Real aParamU1, aParamU2;
  for (; myExplorer.More(); myExplorer.Next())
  {
    // skip INTERNAL and EXTERNAL edges
    if (myExplorer.Current().Orientation() != TopAbs_FORWARD
     && myExplorer.Current().Orientation() != TopAbs_REVERSED)
    {
      continue;
    }

    if (Handle(Geom2d_Curve) aCurve = BRep_Tool::CurveOnSurface (TopoDS::Edge (myExplorer.Current()), myFace, aParamU1, aParamU2))
    {
      myCurve.Load (aCurve, aParamU1, aParamU2);
      return;
    }
    else
    {
      myHasNullCurves = Standard_True;
    }
  }

  myCurve.Reset();
}
