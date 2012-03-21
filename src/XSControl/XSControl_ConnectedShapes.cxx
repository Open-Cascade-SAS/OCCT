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

#include <XSControl_ConnectedShapes.ixx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TransferBRep.hxx>


XSControl_ConnectedShapes::XSControl_ConnectedShapes ()
    : IFSelect_SelectExplore (1)    {  }

    XSControl_ConnectedShapes::XSControl_ConnectedShapes
  (const Handle(XSControl_TransferReader)& TR)
    : IFSelect_SelectExplore (1) , theTR (TR)    {  }

    void  XSControl_ConnectedShapes::SetReader
  (const Handle(XSControl_TransferReader)& TR)
    {  theTR = TR;  }

    Standard_Boolean  XSControl_ConnectedShapes::Explore
  (const Standard_Integer /*level*/, const Handle(Standard_Transient)& ent,
   const Interface_Graph& /*G*/,  Interface_EntityIterator& explored) const
{
  Handle(Transfer_TransientProcess) TP;
  if (!theTR.IsNull()) TP = theTR->TransientProcess();
  if (TP.IsNull()) return Standard_False;
  TopoDS_Shape Shape = TransferBRep::ShapeResult (TP,ent);
  if (Shape.IsNull()) return Standard_False;
  Handle(TColStd_HSequenceOfTransient) li = AdjacentEntities (Shape,TP,TopAbs_FACE);
  explored.AddList (li);
  return Standard_True;
}

    TCollection_AsciiString XSControl_ConnectedShapes::ExploreLabel () const
{
  TCollection_AsciiString lab("Connected Entities through produced Shapes");
  return lab;
}

    Handle(TColStd_HSequenceOfTransient)  XSControl_ConnectedShapes::AdjacentEntities
  (const TopoDS_Shape& ashape,
   const Handle(Transfer_TransientProcess)& TP,
   const TopAbs_ShapeEnum type)
{
  Handle(TColStd_HSequenceOfTransient) li = new TColStd_HSequenceOfTransient();
  Standard_Integer i, nb = TP->NbMapped();
//  TopTools_MapOfShape adj (nb);
  TopTools_MapOfShape vtx(20);

  for (TopExp_Explorer vert(ashape,TopAbs_VERTEX); vert.More(); vert.Next()) {
    vtx.Add (vert.Current());
  }

  for (i = 1; i <= nb; i ++) {
    Handle(Transfer_Binder) bnd = TP->MapItem(i);
    TopoDS_Shape sh = TransferBRep::ShapeResult (bnd);
    if (sh.IsNull()) continue;
    if (sh.ShapeType() != type) continue;
    for (TopExp_Explorer vsh(sh,TopAbs_VERTEX); vsh.More(); vsh.Next()) {
      TopoDS_Shape avtx = vsh.Current();
      if (vtx.Contains(avtx)) {
	li->Append (TP->Mapped(i));
	break;  // break de ce for interieur, entite suivante
      }
    }
  }

  return li;
}
