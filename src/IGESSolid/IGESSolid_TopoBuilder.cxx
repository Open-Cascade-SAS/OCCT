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


#include <gp_XYZ.hxx>
#include <IGESBasic_HArray1OfHArray1OfIGESEntity.hxx>
#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESSolid_HArray1OfFace.hxx>
#include <IGESSolid_HArray1OfLoop.hxx>
#include <IGESSolid_HArray1OfShell.hxx>
#include <IGESSolid_HArray1OfVertexList.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_Shell.hxx>
#include <IGESSolid_TopoBuilder.hxx>
#include <IGESSolid_VertexList.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <TColStd_HArray1OfInteger.hxx>

IGESSolid_TopoBuilder::IGESSolid_TopoBuilder ()    {  Clear();  }

    void  IGESSolid_TopoBuilder::Clear ()
{
  thesolid = new IGESSolid_ManifoldSolid;
  thevoids = new TColStd_HSequenceOfTransient();
  thevflag = new TColStd_HSequenceOfInteger();
  theshell = new IGESSolid_Shell;
  thefaces = new TColStd_HSequenceOfTransient();
  thefflag = new TColStd_HSequenceOfInteger();
  theface.Nullify();
  theinner = new TColStd_HSequenceOfTransient();
  theloop.Nullify();
  theetype = new TColStd_HSequenceOfInteger();
  thee3d   = new TColStd_HSequenceOfInteger();
  theeflag = new TColStd_HSequenceOfInteger();
  theeuv   = new TColStd_HSequenceOfTransient();
  theisol  = new TColStd_HSequenceOfInteger();
  thecuruv = new TColStd_HSequenceOfTransient();
  theiso   = new TColStd_HSequenceOfTransient();
  theedgel = new IGESSolid_EdgeList;
  thecur3d = new TColStd_HSequenceOfTransient();
  thevstar = new TColStd_HSequenceOfInteger();
  thevend  = new TColStd_HSequenceOfInteger();
  thevertl = new IGESSolid_VertexList;
  thepoint = new TColgp_HSequenceOfXYZ();
}

    void  IGESSolid_TopoBuilder::AddVertex (const gp_XYZ& val)
      {  thepoint->Append(val);  }

    Standard_Integer  IGESSolid_TopoBuilder::NbVertices () const
      {  return thepoint->Length();  }

    const gp_XYZ&  IGESSolid_TopoBuilder::Vertex
  (const Standard_Integer num) const
      {  return thepoint->Value(num);  }

    Handle(IGESSolid_VertexList)  IGESSolid_TopoBuilder::VertexList () const
      {  return thevertl;  }


    void  IGESSolid_TopoBuilder::AddEdge
  (const Handle(IGESData_IGESEntity)& curve,
   const Standard_Integer vstart,  const Standard_Integer vend)
{
  if (curve.IsNull() || vstart <= 0 || vend <= 0 ||
      vstart > thepoint->Length() || vend > thepoint->Length())
    throw Standard_DomainError("IGESSolid_TopoBuilder : AddEdge");
  thecur3d->Append (curve);
  thevstar->Append (vstart);
  thevend->Append  (vend);
}

    Standard_Integer  IGESSolid_TopoBuilder::NbEdges () const
      {  return thecur3d->Length();  }

    void  IGESSolid_TopoBuilder::Edge
  (const Standard_Integer num,
   Handle(IGESData_IGESEntity)& curve,
   Standard_Integer& vstart, Standard_Integer& vend) const
{
  if (num <= 0 || num > thecur3d->Length()) return;
  curve  = GetCasted(IGESData_IGESEntity,thecur3d->Value(num));
  vstart = thevstar->Value(num);
  vend   = thevend->Value(num);
}

    Handle(IGESSolid_EdgeList)  IGESSolid_TopoBuilder::EdgeList () const
      {  return theedgel;  }

    void  IGESSolid_TopoBuilder::EndLists ()
{
  Standard_Integer i,nb;
  Handle(TColgp_HArray1OfXYZ) vert;
  Handle(IGESData_HArray1OfIGESEntity)  curves;
  Handle(IGESSolid_HArray1OfVertexList) estart, eend;
  Handle(TColStd_HArray1OfInteger)      nstart, nend;

  nb = thepoint->Length();
  if (nb > 0) {
    vert   = new TColgp_HArray1OfXYZ (1,nb);
    for (i = 1; i <= nb; i ++) vert->SetValue (i, thepoint->Value(i));
  }
  thevertl->Init (vert);

  nb = thecur3d->Length();
  if (nb > 0) {
    curves = new IGESData_HArray1OfIGESEntity (1,nb);
    nstart = new TColStd_HArray1OfInteger     (1,nb); nstart->Init(0);
    nend   = new TColStd_HArray1OfInteger     (1,nb); nend->Init(0);
    estart = new IGESSolid_HArray1OfVertexList (1,nb);
    eend   = new IGESSolid_HArray1OfVertexList (1,nb);
    for (i = 1; i <= nb; i ++) {
      curves->SetValue
	(i,GetCasted(IGESData_IGESEntity,thecur3d->Value(i)));
      nstart->SetValue (i,thevstar->Value(i));
      nend->SetValue   (i,thevend->Value(i));
      estart->SetValue (i,thevertl);
      estart->SetValue (i,thevertl);
    }
  }
  theedgel->Init (curves,estart,nstart,eend,nend);
}

    void  IGESSolid_TopoBuilder::MakeLoop ()
{
  theloop = new IGESSolid_Loop;
  theetype->Clear();  thee3d->Clear();   theeflag->Clear();
  theeuv->Clear();    theisol->Clear();
}

    void  IGESSolid_TopoBuilder::MakeEdge
  (const Standard_Integer edgetype, const Standard_Integer edge3d,
   const Standard_Integer orientation)
{
  if (edge3d <= 0 || edge3d > thecur3d->Length())
    throw Standard_DomainError("IGESSolid_TopoBuilder : MakeEdge");
  theetype->Append (edgetype);
  thee3d->Append   (edge3d);
  theeflag->Append (orientation);
  thecuruv->Clear();  theisol->Clear();
}

    void  IGESSolid_TopoBuilder::AddCurveUV
  (const Handle(IGESData_IGESEntity)& curve, const Standard_Integer iso)
{
  if (curve.IsNull() || thee3d->IsEmpty())
    throw Standard_DomainError("IGESSolid_TopoBuilder : AddCurveUV");
  thecuruv->Append(curve);
  theisol->Append(iso);
}

    void  IGESSolid_TopoBuilder::EndEdge ()
{
//  transformer  thecuruv,theiso en array et le mettre dans theeuv
  Handle(IGESData_HArray1OfIGESEntity) curuv;
  Handle(TColStd_HArray1OfInteger) iso;
  if (!thecuruv->IsEmpty()) {
    Standard_Integer i, nb = thecuruv->Length();
    curuv = new IGESData_HArray1OfIGESEntity (1,nb);
    iso   = new TColStd_HArray1OfInteger     (1,nb); iso->Init(0);
    for (i = 1; i <= nb; i ++) {
      curuv->SetValue (i, GetCasted(IGESData_IGESEntity,thecuruv->Value(i)));
      iso->SetValue   (i, theisol->Value(i));
    }
  }
  theeuv->Append(curuv);
}

    void  IGESSolid_TopoBuilder::EndLoop ()
{
  Handle(TColStd_HArray1OfInteger) etypes, e3d, eflags, enbuv, eiso;
  Handle(IGESData_HArray1OfIGESEntity) edges, curves;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) isol;
  Handle(IGESBasic_HArray1OfHArray1OfIGESEntity) curvl;
  Standard_Integer i, nb; //szv#4:S4163:12Mar99 nbuv not needed
  nb = thee3d->Length();
  if (nb > 0) {
    etypes = new TColStd_HArray1OfInteger (1,nb);
    e3d    = new TColStd_HArray1OfInteger (1,nb);
    eflags = new TColStd_HArray1OfInteger (1,nb);
    enbuv  = new TColStd_HArray1OfInteger (1,nb);
    edges  = new IGESData_HArray1OfIGESEntity (1,nb);
    curvl  = new IGESBasic_HArray1OfHArray1OfIGESEntity (1,nb);
    isol   = new IGESBasic_HArray1OfHArray1OfInteger    (1,nb);

    for (i = 1; i <= nb; i ++) {
      etypes->SetValue (i, theetype->Value(i));
      e3d->SetValue    (i, thee3d->Value(i));
      eflags->SetValue (i, theeflag->Value(i));
      enbuv->SetValue  (i,0);
      edges->SetValue  (i,theedgel);
      curves = GetCasted(IGESData_HArray1OfIGESEntity,thecuruv->Value(i));
      if (!curves.IsNull()) {
	//nbuv = curves->Length(); //szv#4:S4163:12Mar99 not needed
	enbuv->SetValue (i,nb);
	curvl->SetValue (i,curves);
	isol->SetValue  (i,GetCasted(TColStd_HArray1OfInteger,theiso->Value(i)));
      }
    }
  }
  theloop->Init (etypes,edges,e3d,eflags,enbuv, isol,curvl);
}


    void  IGESSolid_TopoBuilder::MakeFace
  (const Handle(IGESData_IGESEntity)& surface)
{
  if (surface.IsNull())
    throw Standard_DomainError("IGESSolid_TopoBuilder : MakeFace");
  thesurf  = surface;
  theouter = Standard_False;
  theinner->Clear();
  theface  = new IGESSolid_Face;
}

    void  IGESSolid_TopoBuilder::SetOuter ()
{
  EndLoop();
  theouter = Standard_True;
  theinner->Append (theloop);
  theloop.Nullify();
}

    void  IGESSolid_TopoBuilder::AddInner ()
{
  EndLoop();
  theinner->Append (theloop);
  theloop.Nullify();
}

    void  IGESSolid_TopoBuilder::EndFace (const Standard_Integer orientation)
{
  Handle(IGESSolid_HArray1OfLoop) loops;
  Standard_Integer i, nb = theinner->Length();
  if (nb > 0) {
    loops = new IGESSolid_HArray1OfLoop (1,nb);
    for (i = 1; i <= nb; i ++) loops->SetValue
      (i, GetCasted(IGESSolid_Loop,theinner->Value(i)));
  }
  theface->Init (thesurf,theouter,loops);
  thefaces->Append(theface);
  thefflag->Append(orientation);
}


    void  IGESSolid_TopoBuilder::MakeShell ()
{
  theshell = new IGESSolid_Shell;
  thefaces->Clear();
  thefflag->Clear();
}

    void  IGESSolid_TopoBuilder::EndShell ()
{
  Handle(IGESSolid_HArray1OfFace)  faces;
  Handle(TColStd_HArray1OfInteger) flags;
  Standard_Integer i, nb = thefaces->Length();
  if (nb > 0) {
    faces = new IGESSolid_HArray1OfFace ( 1,nb);
    flags = new TColStd_HArray1OfInteger (1,nb); flags->Init(0);
    for (i = 1; i <= nb; i ++) {
      faces->SetValue (i,GetCasted(IGESSolid_Face,thefaces->Value(i)));
      flags->SetValue (i, thefflag->Value(i));
    }
  }
  theshell->Init (faces,flags);
}

    void  IGESSolid_TopoBuilder::EndSimpleShell ()
      {  EndShell();  EndLists();  }

    void  IGESSolid_TopoBuilder::SetMainShell
  (const Standard_Integer orientation)
{
  EndShell();
  themains = theshell;
  themflag = orientation != 0;
  theshell.Nullify();
}

    void  IGESSolid_TopoBuilder::AddVoidShell
  (const Standard_Integer orientation)
{
  EndShell();
  thevoids->Append (theshell);
  thevflag->Append (orientation);
  theshell.Nullify();
}


    void  IGESSolid_TopoBuilder::EndSolid ()
{
  EndLists();
  Handle(IGESSolid_HArray1OfShell) shells;
  Handle(TColStd_HArray1OfInteger) flags;
  Standard_Integer i, nb = thevoids->Length();
  if (nb > 0) {
    shells = new IGESSolid_HArray1OfShell (1,nb);
    flags  = new TColStd_HArray1OfInteger (1,nb); flags->Init(0);
    for (i = 1; i <= nb; i ++) {
      shells->SetValue (i,GetCasted(IGESSolid_Shell,thevoids->Value(i)));
      flags->SetValue  (i,thevflag->Value(i));
    }
  }
  thesolid->Init (themains,themflag, shells,flags);
}

    Handle(IGESSolid_Shell)  IGESSolid_TopoBuilder::Shell () const
      {  return theshell;  }

    Handle(IGESSolid_ManifoldSolid)  IGESSolid_TopoBuilder::Solid () const
      {  return thesolid;  }
