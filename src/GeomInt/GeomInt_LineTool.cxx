// Created on: 1995-02-08
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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


#include <GeomInt_LineTool.ixx>

#include <IntPatch_WLine.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_GLine.hxx>
#include <Precision.hxx>


//=======================================================================
//function : NbVertex
//purpose  : 
//=======================================================================
Standard_Integer GeomInt_LineTool::NbVertex(const Handle(IntPatch_Line)& L)
{
  switch (L->ArcType())
  {
    case IntPatch_Analytic:    return Handle(IntPatch_ALine)::DownCast(L)->NbVertex();
    case IntPatch_Restriction: return Handle(IntPatch_RLine)::DownCast(L)->NbVertex();
    case IntPatch_Walking:     return Handle(IntPatch_WLine)::DownCast(L)->NbVertex();
    default: break;
  }
  return Handle(IntPatch_GLine)::DownCast(L)->NbVertex();
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================
const IntPatch_Point & GeomInt_LineTool::Vertex(const Handle(IntPatch_Line)& L,
                                                const Standard_Integer I)
{
  switch (L->ArcType())
  {
    case IntPatch_Analytic: return Handle(IntPatch_ALine)::DownCast(L)->Vertex(I);
    case IntPatch_Restriction: return Handle(IntPatch_RLine)::DownCast(L)->Vertex(I);
    case IntPatch_Walking: return Handle(IntPatch_WLine)::DownCast(L)->Vertex(I);
    default: break;
  }
  return Handle(IntPatch_GLine)::DownCast(L)->Vertex(I);
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================
Standard_Real GeomInt_LineTool::FirstParameter (const Handle(IntPatch_Line)& L)
{
  const IntPatch_IType typl = L->ArcType();
  switch (typl)
  {
    case IntPatch_Analytic:
    {
      Handle(IntPatch_ALine) alin = Handle(IntPatch_ALine)::DownCast(L);
      if (alin->HasFirstPoint())
        return alin->FirstPoint().ParameterOnLine();
      Standard_Boolean included;
      Standard_Real firstp = alin->FirstParameter(included);
      if (!included)
        firstp += Epsilon(firstp);
      return firstp;
    }

    case IntPatch_Restriction:
    {
      Handle(IntPatch_RLine) rlin = Handle(IntPatch_RLine)::DownCast(L);
	  return (rlin->HasFirstPoint()? rlin->FirstPoint().ParameterOnLine() : -Precision::Infinite()); // a voir selon le type de la ligne 2d
    }

    case IntPatch_Walking:
    {
      Handle(IntPatch_WLine) wlin = Handle(IntPatch_WLine)::DownCast(L);
	  return (wlin->HasFirstPoint()? wlin->FirstPoint().ParameterOnLine() : 1.);
    }

    default:
    {
      Handle(IntPatch_GLine) glin = Handle(IntPatch_GLine)::DownCast(L);
      if (glin->HasFirstPoint())
        return glin->FirstPoint().ParameterOnLine();
      switch (typl)
      {
        case IntPatch_Lin:
        case IntPatch_Parabola:
        case IntPatch_Hyperbola:
          return -Precision::Infinite();
        default: break;
      }
    }
  }
  return 0.0;
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================
Standard_Real GeomInt_LineTool::LastParameter (const Handle(IntPatch_Line)& L)
{
  const IntPatch_IType typl = L->ArcType();
  switch (typl)
  {
    case IntPatch_Analytic:
    {
      Handle(IntPatch_ALine) alin = Handle(IntPatch_ALine)::DownCast(L);
      if (alin->HasLastPoint())
        return alin->LastPoint().ParameterOnLine();
      Standard_Boolean included;
      Standard_Real lastp = alin->LastParameter(included);
      if (!included)
        lastp -=Epsilon(lastp);
      return lastp;
    }

    case IntPatch_Restriction:
    {
      Handle(IntPatch_RLine) rlin = Handle(IntPatch_RLine)::DownCast(L);
	  return (rlin->HasLastPoint()? rlin->LastPoint().ParameterOnLine() : Precision::Infinite()); // a voir selon le type de la ligne 2d
    }

    case IntPatch_Walking:
    {
      Handle(IntPatch_WLine) wlin = Handle(IntPatch_WLine)::DownCast(L);
	  return (wlin->HasLastPoint()? wlin->LastPoint().ParameterOnLine() : wlin->NbPnts());
    }

    default:
    {
      Handle(IntPatch_GLine) glin = Handle(IntPatch_GLine)::DownCast(L);
      if (glin->HasLastPoint())
        return glin->LastPoint().ParameterOnLine();
      switch (typl)
      {
        case IntPatch_Lin:
        case IntPatch_Parabola:
        case IntPatch_Hyperbola:
          return Precision::Infinite();
        case IntPatch_Circle:
        case IntPatch_Ellipse:
          return 2.*M_PI;
        default: break;
      }
    }
  }
  return 0.0;
}
