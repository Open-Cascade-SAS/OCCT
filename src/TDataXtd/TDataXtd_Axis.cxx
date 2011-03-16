// File:	TDataXtd_Axis.cxx
// Created:	Mon Apr  6 17:33:17 2009
//		<sergey.zaritchny@opencascade.com>
//Copyright:    Open CasCade SA 2009


#include <TDataXtd_Axis.ixx>
#include <TDataStd.hxx>
#include <TDataXtd.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <TDF_Label.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopAbs.hxx>
#include <gp_Lin.hxx>
#include <BRep_Tool.hxx>
#include <TopLoc_Location.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <GeomAbs_CurveType.hxx>

#define OCC2932

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataXtd_Axis::GetID () 
{
  static Standard_GUID TDataXtd_AxisID("2a96b601-ec8b-11d0-bee7-080009dc3333");
  return TDataXtd_AxisID;
}



//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataXtd_Axis) TDataXtd_Axis::Set (const TDF_Label& L)
{ 
  Handle(TDataXtd_Axis) A; 
  if (!L.FindAttribute(TDataXtd_Axis::GetID(),A)) {
    A = new TDataXtd_Axis ();
    L.AddAttribute(A);
  }  
  return A;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataXtd_Axis) TDataXtd_Axis::Set (const TDF_Label& L, const gp_Lin& line)
{ 
  Handle(TDataXtd_Axis) A = Set (L);

#ifdef OCC2932
  Handle(TNaming_NamedShape) aNS;
  if(L.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
    if(!aNS->Get().IsNull())
       if(aNS->Get().ShapeType() == TopAbs_EDGE) {
	 TopoDS_Edge anEdge = TopoDS::Edge(aNS->Get());
	 BRepAdaptor_Curve anAdaptor(anEdge);
	 if(anAdaptor.GetType() == GeomAbs_Line) {
	   gp_Lin anOldLine = anAdaptor.Line();
	   if(anOldLine.Direction().X() == line.Direction().X() &&
	      anOldLine.Direction().Y() == line.Direction().Y() &&
	      anOldLine.Direction().Z() == line.Direction().Z() &&
	      anOldLine.Location().X() == line.Location().X() &&
	      anOldLine.Location().Y() == line.Location().Y() &&
	      anOldLine.Location().Z() == line.Location().Z()
	      )
	     return A;
	 }
       }
  }
#endif
  TNaming_Builder B (L);
  B.Generated (BRepBuilderAPI_MakeEdge(line));
  return A;
}


//=======================================================================
//function : TDataXtd_Axis
//purpose  : 
//=======================================================================

TDataXtd_Axis::TDataXtd_Axis () { }




//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataXtd_Axis::ID() const {  return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataXtd_Axis::NewEmpty () const
{  
  return new TDataXtd_Axis(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataXtd_Axis::Restore (const Handle(TDF_Attribute)& With) { }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataXtd_Axis::Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const { }


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataXtd_Axis::Dump (Standard_OStream& anOS) const
{  
  anOS << "Axis";
  return anOS;
}
