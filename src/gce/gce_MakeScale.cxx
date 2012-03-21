// Created on: 1992-09-04
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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


#include <gce_MakeScale.ixx>

//=========================================================================
//   Creation d un homothetie de gp de centre Point et de rapport Scale.  +
//=========================================================================

gce_MakeScale::
  gce_MakeScale(const gp_Pnt&       Point ,
		const Standard_Real Scale ) {
   TheScale.SetScale(Point,Scale);
 }

const gp_Trsf& gce_MakeScale::Value() const
{ 
  return TheScale; 
}

const gp_Trsf& gce_MakeScale::Operator() const 
{
  return TheScale;
}

gce_MakeScale::operator gp_Trsf() const
{
  return TheScale;
}
