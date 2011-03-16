// File:	BRepFill_SectionLaw.cxx
// Created:	Wed Jan  7 13:40:00 1998
// Author:	Philippe MANGIN
//		<pmn@sgi29>


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
//purpose  : Donne le nombre de loi elementaire (ou Geometrique)
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
//purpose  : Prepare le parcour d'un wire
//=======================================================================
 void BRepFill_SectionLaw::Init(const TopoDS_Wire& W)
{
  myIterator.Init(W);
}

//=======================================================================
//function : 
//purpose  : Parcourt d'un wire en sautant les Edges degenere
//=======================================================================
 TopoDS_Edge BRepFill_SectionLaw::CurrentEdge() 
{
  TopoDS_Edge E;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool B;
  Standard_Boolean Suivant = Standard_False;
  if (myIterator.More()) {
    E =  myIterator.Current();
//    Suivant = (B.Degenerated(E));
    Suivant = (BRep_Tool::Degenerated(E));
  }

  while (Suivant) {
     myIterator.Next();
     E = myIterator.Current();
//     Suivant = (B.Degenerated(E) && myIterator.More());
     Suivant = (BRep_Tool::Degenerated(E) && myIterator.More());
   }

  if (myIterator.More()) myIterator.Next();
  return E;
}

