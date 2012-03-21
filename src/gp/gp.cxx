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

#include <gp.ixx>

//=======================================================================
//function : Origin
//purpose  : 
//=======================================================================

const gp_Pnt&  gp::Origin()
{
  static gp_Pnt gp_Origin(0,0,0);
  return gp_Origin;
}

//=======================================================================
//function : DX
//purpose  : 
//=======================================================================

const gp_Dir&  gp::DX()
{
  static gp_Dir gp_DX(1,0,0);
  return gp_DX;
}

//=======================================================================
//function : DY
//purpose  : 
//=======================================================================

const gp_Dir&  gp::DY()
{
  static gp_Dir gp_DY(0,1,0);
  return gp_DY;
}

//=======================================================================
//function : DZ
//purpose  : 
//=======================================================================

const gp_Dir&  gp::DZ()
{
  static gp_Dir gp_DZ(0,0,1);
  return gp_DZ;
}

//=======================================================================
//function : OX
//purpose  : 
//=======================================================================

const gp_Ax1&  gp::OX()
{
  static gp_Ax1 gp_OX(gp_Pnt(0,0,0),gp_Dir(1,0,0));
  return gp_OX;
}

//=======================================================================
//function : OY
//purpose  : 
//=======================================================================

const gp_Ax1&  gp::OY()
{
  static gp_Ax1 gp_OY(gp_Pnt(0,0,0),gp_Dir(0,1,0));
  return gp_OY;
}

//=======================================================================
//function : OZ
//purpose  : 
//=======================================================================

const gp_Ax1&  gp::OZ()
{
  static gp_Ax1 gp_OZ(gp_Pnt(0,0,0),gp_Dir(0,0,1));
  return gp_OZ;
}

//=======================================================================
//function : XOY
//purpose  : 
//=======================================================================

const gp_Ax2&  gp::XOY()
{
  static gp_Ax2 gp_XOY(gp_Pnt(0,0,0),gp_Dir(0,0,1),gp_Dir(1,0,0));
  return gp_XOY;
}

//=======================================================================
//function : ZOX
//purpose  : 
//=======================================================================

const gp_Ax2&  gp::ZOX()
{
  static gp_Ax2 gp_ZOX(gp_Pnt(0,0,0),gp_Dir(0,1,0),gp_Dir(0,0,1));
  return gp_ZOX;
}

//=======================================================================
//function : YOZ
//purpose  : 
//=======================================================================

const gp_Ax2&  gp::YOZ()
{
  static gp_Ax2 gp_YOZ(gp_Pnt(0,0,0),gp_Dir(1,0,0),gp_Dir(0,1,0));
  return gp_YOZ;
}

//=======================================================================
//function : Origin2d
//purpose  : 
//=======================================================================

const gp_Pnt2d&  gp::Origin2d()
{
  static gp_Pnt2d gp_Origin2d(0,0);
  return gp_Origin2d;
}

//=======================================================================
//function : DX2d
//purpose  : 
//=======================================================================

const gp_Dir2d&  gp::DX2d()
{
  static gp_Dir2d gp_DX2d(1,0);
  return gp_DX2d;
}

//=======================================================================
//function : DY2d
//purpose  : 
//=======================================================================

const gp_Dir2d&  gp::DY2d()
{
  static gp_Dir2d gp_DY2d(0,1);
  return gp_DY2d;
}

//=======================================================================
//function : OX2d
//purpose  : 
//=======================================================================

const gp_Ax2d&  gp::OX2d()
{
  static gp_Ax2d gp_OX2d(gp_Pnt2d(0,0),gp_Dir2d(1,0));
  return gp_OX2d;
}

//=======================================================================
//function : OY2d
//purpose  : 
//=======================================================================

const gp_Ax2d&  gp::OY2d()
{
  static gp_Ax2d gp_OY2d(gp_Pnt2d(0,0),gp_Dir2d(0,1));
  return gp_OY2d;
}

