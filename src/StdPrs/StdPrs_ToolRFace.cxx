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


#include <Adaptor2d_Curve2d.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <TopoDS.hxx>

//=======================================================================
//function : StdPrs_ToolRFace
//purpose  : 
//=======================================================================
StdPrs_ToolRFace::StdPrs_ToolRFace()
{
}

//=======================================================================
//function : StdPrs_ToolRFace
//purpose  : 
//=======================================================================

StdPrs_ToolRFace::StdPrs_ToolRFace(const Handle(BRepAdaptor_HSurface)& aSurface) :
       myFace(((BRepAdaptor_Surface*)&(aSurface->Surface()))->Face())
{
  myFace.Orientation(TopAbs_FORWARD);
}

//=======================================================================
//function : IsOriented
//purpose  : 
//=======================================================================

Standard_Boolean StdPrs_ToolRFace::IsOriented () const {
  
  return Standard_True;

}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StdPrs_ToolRFace::Init() 
{
  myExplorer.Init(myFace,TopAbs_EDGE);
  if (myExplorer.More()) {
    Standard_Real U1,U2;
    const Handle(Geom2d_Curve)& C = 
      BRep_Tool::CurveOnSurface(TopoDS::Edge(myExplorer.Current()),
				myFace,
				U1,U2);
    DummyCurve.Load(C,U1,U2);
  }
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean StdPrs_ToolRFace::More() const
{
  return myExplorer.More();
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void StdPrs_ToolRFace::Next()
{
  myExplorer.Next();

  if (myExplorer.More()) {
    // skip INTERNAL and EXTERNAL edges
    while (myExplorer.More() && (myExplorer.Current().Orientation() == TopAbs_INTERNAL 
                              || myExplorer.Current().Orientation() == TopAbs_EXTERNAL)) 
	myExplorer.Next();
    if (myExplorer.More()) {
      Standard_Real U1,U2;
      const Handle(Geom2d_Curve)& C = 
	BRep_Tool::CurveOnSurface(TopoDS::Edge(myExplorer.Current()),
				  myFace,
				  U1,U2);
      if ( !C.IsNull() )
	DummyCurve.Load(C,U1,U2);
    }
  }
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Adaptor2d_Curve2dPtr StdPrs_ToolRFace::Value() const
{
  return (Adaptor2d_Curve2dPtr)&DummyCurve;
}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation StdPrs_ToolRFace::Orientation () const {
  return myExplorer.Current().Orientation();
}

