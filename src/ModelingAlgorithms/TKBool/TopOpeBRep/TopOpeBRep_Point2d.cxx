// Created on: 1998-10-29
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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

#ifdef DRAW
  #include <TopOpeBRepTool_DRAW.hxx>
#endif

#include <BRep_Tool.hxx>
#include <TopOpeBRep_EdgesIntersector.hxx>
#include <TopOpeBRep_Point2d.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Transition.hxx>

//=================================================================================================

TopOpeBRep_Point2d::TopOpeBRep_Point2d()
    : myhaspint(Standard_False),
      myisvertex1(Standard_False),
      myparameter1(0.),
      myisvertex2(Standard_False),
      myparameter2(0.),
      myispointofsegment(Standard_False),
      myips1(0),
      myips2(0),
      myhasancestors(Standard_False),
      mystatus(TopOpeBRep_P2DUNK),
      myindex(0),
      mykeep(Standard_True),
      myedgesconfig(TopOpeBRepDS_UNSHGEOMETRY),
      mytolerance(0.)
{
}

//=================================================================================================

const TopoDS_Vertex& TopOpeBRep_Point2d::Vertex(const Standard_Integer Index) const
{
  if (!IsVertex(Index))
    throw Standard_Failure("TopOpeBRep_Point2d::Vertex");
  if (Index == 1)
    return myvertex1;
  else if (Index == 2)
    return myvertex2;
  else
    throw Standard_Failure("TopOpeBRep_Point2d::Vertex");
}

//=================================================================================================

const TopOpeBRepDS_Transition& TopOpeBRep_Point2d::Transition(const Standard_Integer Index) const
{
  if (Index == 1)
    return mytransition1;
  else if (Index == 2)
    return mytransition2;
  else
    throw Standard_Failure("TopOpeBRep_Point2d::Transition");
}

//=================================================================================================

TopOpeBRepDS_Transition& TopOpeBRep_Point2d::ChangeTransition(const Standard_Integer Index)
{
  if (Index == 1)
    return mytransition1;
  else if (Index == 2)
    return mytransition2;
  else
    throw Standard_Failure("TopOpeBRep_Point2d::ChangeTransition");
}

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRep_Point2d::Dump(const Standard_Integer E1index, const Standard_Integer E2index) const
{
  Standard_Real par1 = Parameter(1);
  Standard_Real par2 = Parameter(2);

  Standard_Integer index = Index();
  Standard_Boolean keep  = Keep();
  Standard_Integer sts   = Status();
  Standard_Boolean pos   = IsPointOfSegment();

  Standard_Boolean isvertex1 = IsVertex(1);
  TopoDS_Vertex    V1;
  if (isvertex1)
    V1 = Vertex(1);
  Standard_Boolean isvertex2 = IsVertex(2);
  TopoDS_Vertex    V2;
  if (isvertex2)
    V2 = Vertex(2);

  Standard_Integer ia1, ia2;
  SegmentAncestors(ia1, ia2);
  std::cout << std::endl << "p2d " << index << "  k=" << keep << " pos=" << pos;
  switch (sts)
  {
    case TopOpeBRep_P2DUNK:
      std::cout << " sts=u";
      break;
    case TopOpeBRep_P2DSGF:
      std::cout << " sts=f";
      break;
    case TopOpeBRep_P2DSGL:
      std::cout << " sts=l";
      break;
    case TopOpeBRep_P2DNEW:
      std::cout << " sts=n";
      std::cout << " anc=" << ia1 << "," << ia2;
      break;
    case TopOpeBRep_P2DINT:
      std::cout << " sts=i";
      break;
  } // switch
  std::cout << " cfg=";
  TopOpeBRepDS::Print(myedgesconfig, std::cout);
  std::cout << std::endl;

  gp_Pnt P3D = Value();
  #ifdef DRAW
  std::cout << FUN_tool_PRODINP() << "P" << Index() << " " << P3D.X() << " " << P3D.Y() << " "
            << P3D.Z() << "; # tol = " << tol << std::endl;
  #endif
  std::cout << "     on (1) :";
  std::cout << " vertex(1) : ";
  std::cout << (isvertex1 ? 1 : 0);
  std::cout << "  T " << E1index << "(1)";
  std::cout << " par(1) = " << par1;
  if (isvertex1)
  {
    P3D = BRep_Tool::Pnt(V1);
    std::cout << " PV(1) : " << P3D.X() << " " << P3D.Y() << " " << P3D.Z();
  }
  std::cout << std::endl;

  std::cout << "     on (2) :";
  std::cout << " vertex(2) : ";
  std::cout << (isvertex2 ? 1 : 0);
  std::cout << "  T " << E2index << "(2)";
  std::cout << " par(2) = " << par2;
  if (isvertex2)
  {
    P3D = BRep_Tool::Pnt(V2);
    std::cout << " PV(2) : " << P3D.X() << " " << P3D.Y() << " " << P3D.Z();
  }
  std::cout << std::endl;
}
#else
void TopOpeBRep_Point2d::Dump(const Standard_Integer, const Standard_Integer) const {}
#endif
