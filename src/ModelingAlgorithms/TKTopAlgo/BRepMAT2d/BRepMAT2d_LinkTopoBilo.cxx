// Created on: 1994-10-07
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRepMAT2d_BisectingLocus.hxx>
#include <BRepMAT2d_Explorer.hxx>
#include <BRepMAT2d_LinkTopoBilo.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Geometry.hxx>
#include <MAT_BasicElt.hxx>
#include <MAT_Graph.hxx>
#include <MAT_BasicElt.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>

//=================================================================================================

BRepMAT2d_LinkTopoBilo::BRepMAT2d_LinkTopoBilo()
    : current(0),
      isEmpty(true)
{
}

//=================================================================================================

BRepMAT2d_LinkTopoBilo::BRepMAT2d_LinkTopoBilo(const BRepMAT2d_Explorer&       Explo,
                                               const BRepMAT2d_BisectingLocus& BiLo)
{
  Perform(Explo, BiLo);
}

//=================================================================================================

void BRepMAT2d_LinkTopoBilo::Perform(const BRepMAT2d_Explorer&       Explo,
                                     const BRepMAT2d_BisectingLocus& BiLo)
{
  myMap.Clear();
  myBEShape.Clear();

  TopoDS_Shape     S          = Explo.Shape();
  int IndContour = 1;

  if (S.ShapeType() == TopAbs_FACE)
  {
    TopExp_Explorer Exp(S, TopAbs_WIRE);

    while (Exp.More())
    {
      LinkToWire(TopoDS::Wire(Exp.Current()), Explo, IndContour, BiLo);
      Exp.Next();
      IndContour++;
    }
  }
  else
  {
    throw Standard_ConstructionError("BRepMAT2d_LinkTopoBilo::Perform");
  }
}

//=================================================================================================

void BRepMAT2d_LinkTopoBilo::Init(const TopoDS_Shape& S)
{
  isEmpty = false;
  current = 1;
  if (myMap.IsBound(S))
    myKey = S;
  else
    isEmpty = true;
}

//=================================================================================================

bool BRepMAT2d_LinkTopoBilo::More()
{
  if (isEmpty)
    return false;
  return (current <= myMap(myKey).Length());
}

//=================================================================================================

void BRepMAT2d_LinkTopoBilo::Next()
{
  current++;
}

//=================================================================================================

occ::handle<MAT_BasicElt> BRepMAT2d_LinkTopoBilo::Value() const
{
  return myMap(myKey).Value(current);
}

//=================================================================================================

TopoDS_Shape BRepMAT2d_LinkTopoBilo::GeneratingShape(const occ::handle<MAT_BasicElt>& BE) const
{
  return myBEShape(BE);
}

static void LinkToContour(const BRepMAT2d_Explorer&        Explo,
                          const int           IndC,
                          const BRepMAT2d_BisectingLocus&  BiLo,
                          NCollection_DataMap<int, int>& Link);

//=================================================================================================

void BRepMAT2d_LinkTopoBilo::LinkToWire(const TopoDS_Wire&              W,
                                        const BRepMAT2d_Explorer&       Explo,
                                        const int          IndC,
                                        const BRepMAT2d_BisectingLocus& BiLo)
{
  BRepTools_WireExplorer   TheExp(W);
  int         KC;
  TopoDS_Vertex            VF, VL;
  TopoDS_Shape             S;
  occ::handle<MAT_BasicElt>     BE;
  occ::handle<Standard_Type>    Type;
  NCollection_Sequence<TopoDS_Shape> TopoSeq;
  NCollection_Sequence<occ::handle<MAT_BasicElt>>   EmptySeq;

  NCollection_DataMap<int, int>::Iterator Ite;
  NCollection_DataMap<int, int>                  LinkBECont;

  for (; TheExp.More(); TheExp.Next())
  {
    TopoSeq.Append(TheExp.Current());
  }

  //-----------------------------------------------------
  // Construction Links BasicElt => Curve of contour IndC.
  //-----------------------------------------------------
  LinkToContour(Explo, IndC, BiLo, LinkBECont);

  //---------------------------------------------------------------
  // Iteration on BasicElts. The associated index is the same for
  // the curves of the contour and the edges.               .
  //---------------------------------------------------------------
  for (Ite.Initialize(LinkBECont); Ite.More(); Ite.Next())
  {
    BE   = BiLo.Graph()->BasicElt(Ite.Key());
    Type = BiLo.GeomElt(BE)->DynamicType();
    KC   = Ite.Value();
    S    = TopoSeq.Value(std::abs(KC));

    if (Type == STANDARD_TYPE(Geom2d_CartesianPoint))
    {
      if (S.Orientation() == TopAbs_REVERSED)
      {
        TopExp::Vertices(TopoDS::Edge(S), VL, VF);
      }
      else
      {
        TopExp::Vertices(TopoDS::Edge(S), VF, VL);
      }
      if (KC > 0)
        S = VL;
      else
        S = VF;
    }
    if (!myMap.IsBound(S))
    {
      myMap.Bind(S, EmptySeq);
    }
    myMap(S).Append(BE);

    if (KC < 0)
      myBEShape.Bind(BE, S.Oriented(TopAbs::Reverse(S.Orientation())));
    else
      myBEShape.Bind(BE, S);
  }
}

//=======================================================================
// function : LinkToContour
// purpose  : Association to each basicElt of the curre of the initial
//           contour from which it comes.
//=======================================================================

void LinkToContour(const BRepMAT2d_Explorer&        Explo,
                   const int           IndC,
                   const BRepMAT2d_BisectingLocus&  BiLo,
                   NCollection_DataMap<int, int>& Link)
{
  occ::handle<MAT_BasicElt>    BE;
  occ::handle<Geom2d_Geometry> GeomBE;
  occ::handle<Standard_Type>   Type;
  bool        DirectSense = true;
  bool        LastPoint   = false;
  int        NbSect, ISect;

  //---------------------------------------------------
  // NbSect : number of sections on the current curve.
  // ISect  : Counter on sections.
  //---------------------------------------------------

  const NCollection_Sequence<occ::handle<Geom2d_Curve>>& Cont = Explo.Contour(IndC);

  //------------------------------------------------------------------
  // Initialization of the explorer on the first curve of the contour.
  //------------------------------------------------------------------
  int IndOnCont     = 1;
  int PrecIndOnCont = -1;
  NbSect                         = BiLo.NumberOfSections(IndC, 1);
  ISect                          = 0;

  //------------------------------------------------------------------
  // Parsing of base elements associated to contour IndC.
  // Rq : the base elements are ordered.
  //------------------------------------------------------------------
  for (int i = 1; i <= BiLo.NumberOfElts(IndC); i++)
  {

    BE     = BiLo.BasicElt(IndC, i);
    GeomBE = BiLo.GeomElt(BE);
    Type   = GeomBE->DynamicType();

    if (Type != STANDARD_TYPE(Geom2d_CartesianPoint))
    {
      ISect++;
      //----------------------------------------------------------------
      // The base element is a curve associated with the current curve.
      //----------------------------------------------------------------
      if (DirectSense)
      {
        Link.Bind(BE->Index(), IndOnCont);
      }
      else
      {
        Link.Bind(BE->Index(), -IndOnCont);
      }
    }
    else
    {
      //-----------------------------------------------------------------
      // The base element is a point associated with the previous curve.
      //-----------------------------------------------------------------
      if (DirectSense || LastPoint)
      {
        Link.Bind(BE->Index(), PrecIndOnCont);
      }
      else
      {
        Link.Bind(BE->Index(), -PrecIndOnCont);
      }
    }

    PrecIndOnCont = IndOnCont;
    //----------------------------------------------------------------------
    // Passage to the next curve in Explo, when all parts
    // of curves corresponding to the initial curve have been parsed.
    //---------------------------------------------------------------------
    if (Type != STANDARD_TYPE(Geom2d_CartesianPoint) && ISect == NbSect)
    {
      if (IndOnCont < Cont.Length() && DirectSense)
      {
        IndOnCont++;
        NbSect = BiLo.NumberOfSections(IndC, IndOnCont);
        ISect  = 0;
      }
      else
      {
        //-----------------------------------------------------
        // For open lines restart in the other direction.
        //-----------------------------------------------------
        if (!DirectSense)
        {
          IndOnCont--;
          if (IndOnCont != 0)
            NbSect = BiLo.NumberOfSections(IndC, IndOnCont);
          LastPoint = false;
        }
        else
        {
          LastPoint = true;
        }
        ISect       = 0;
        DirectSense = false;
      }
    }
  }
}
