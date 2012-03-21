// Created on: 2008-05-29
// Created by: Sergey ZARITCHNY
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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


#include <TDataXtd.ixx>
#include <TDataXtd.ixx>
#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_PatternStd.hxx>
#include <TDataXtd_Placement.hxx>
#include <TDataXtd_Point.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataXtd_Position.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDF_IDList.hxx>

//=======================================================================
//function : IDList
//purpose  : 
//=======================================================================

void TDataXtd::IDList(TDF_IDList& anIDList)
{  
  anIDList.Append(TDataXtd_Axis::GetID());
  anIDList.Append(TDataXtd_Constraint::GetID());
  anIDList.Append(TDataXtd_Geometry::GetID());
  anIDList.Append(TDataXtd_PatternStd::GetID());
  anIDList.Append(TDataXtd_Placement::GetID());
  anIDList.Append(TDataXtd_Point::GetID());
  anIDList.Append(TDataXtd_Plane::GetID());
  anIDList.Append(TDataXtd_Position::GetID());
  anIDList.Append(TDataXtd_Shape::GetID());
  
}

//=======================================================================
//function : 
//purpose  : print the name of the constraint
//=======================================================================

Standard_OStream& TDataXtd::Print(const TDataXtd_ConstraintEnum C,  Standard_OStream& s)
{
  switch (C) {
    
  //2d planar constraints
 
  case TDataXtd_RADIUS         : { s << "RADIUS"; break;}
  case TDataXtd_DIAMETER       : { s << "DIAMETER"; break;}
  case TDataXtd_MINOR_RADIUS   : { s << "MINOR_RADIUS"; break;}
  case TDataXtd_MAJOR_RADIUS   : { s << "MAJOR_RADIUS"; break;}
  case TDataXtd_TANGENT        : { s << "TANGENT"; break;}
  case TDataXtd_PARALLEL       : { s << "PARALLEL"; break;}
  case TDataXtd_PERPENDICULAR  : { s << "PERPENDICULAR"; break;}
  case TDataXtd_CONCENTRIC     : { s << "CONCENTRIC"; break;}
  case TDataXtd_COINCIDENT     : { s << "COINCIDENT"; break;}
  case TDataXtd_DISTANCE       : { s << "DISTANCE"; break;}
  case TDataXtd_ANGLE          : { s << "ANGLE"; break;}
  case TDataXtd_EQUAL_RADIUS   : { s << "EQUAL_RADIUS"; break;}
  case TDataXtd_SYMMETRY       : { s << "SYMMETRY"; break;}
  case TDataXtd_MIDPOINT       : { s << "MIDPOINT"; break;} 
  case TDataXtd_EQUAL_DISTANCE : { s << "EQUAL_DISTANCE"; break;} 
  case TDataXtd_FIX            : { s << "FIX"; break;}
  case TDataXtd_RIGID          : { s << "RIGID"; break;}

  // placement constraint

  case TDataXtd_FROM           : { s << "FROM"; break;}  
  case TDataXtd_AXIS           : { s << "AXIS"; break;}  
  case TDataXtd_MATE           : { s << "MATE"; break;}
  case TDataXtd_ALIGN_FACES    : { s << "ALIGN_FACES"; break;}
  case TDataXtd_ALIGN_AXES     : { s << "ALIGN_AXES"; break;}
  case TDataXtd_AXES_ANGLE     : { s << "AXES_ANGLE"; break;}
  case TDataXtd_FACES_ANGLE    : { s << "FACES_ANGLE"; break;}
  case TDataXtd_ROUND          : { s << "ROUND"; break;}
  case TDataXtd_OFFSET         : { s << "OFFSET"; break;} 
 
    
    default :
      {
	s << "UNKNOWN"; break;
      }
  }
  return s;
}

//=======================================================================
//function : 
//purpose  : print the name of the real dimension
//=======================================================================

Standard_OStream& TDataXtd::Print(const TDataXtd_GeometryEnum G,  Standard_OStream& s)
{
  switch (G) {  
  case TDataXtd_ANY_GEOM :
    { 
      s << "ANY_GEOM"; break;
    }
  case TDataXtd_POINT :
    {
      s << "POINT";  break;
    }
  case  TDataXtd_LINE :
    {  
      s << "LINE"; break;
    }  
  case TDataXtd_CIRCLE :
    { 
      s << "CIRCLE"; break;
    }  
  case TDataXtd_ELLIPSE :
    { 
      s << "ELLIPSE"; break;
    }  
  case TDataXtd_SPLINE :
    { 
      s << "SPLINE"; break;
    }  
  case TDataXtd_PLANE :
    { 
      s << "PLANE"; break;
    }  
  case TDataXtd_CYLINDER :
    { 
      s << "CYLINDER"; break;
    }
    default :
      {
	s << "UNKNOWN"; break;
      }
  }
  return s;
}
