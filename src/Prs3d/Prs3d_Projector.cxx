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

#include <Prs3d_Projector.ixx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax3.hxx>


Prs3d_Projector::Prs3d_Projector (const HLRAlgo_Projector& HLPr): MyProjector(HLPr)
{}



Prs3d_Projector::Prs3d_Projector (const Standard_Boolean Pers,
				const Quantity_Length Focus,
			        const Quantity_Length DX,
			        const Quantity_Length DY,
			        const Quantity_Length DZ,
			        const Quantity_Length XAt,
			        const Quantity_Length YAt,
			        const Quantity_Length ZAt,
			        const Quantity_Length XUp,
			        const Quantity_Length YUp,
			        const Quantity_Length ZUp) 
{
  gp_Pnt At (XAt,YAt,ZAt);
  gp_Dir Zpers (DX,DY,DZ);
  gp_Dir Ypers (XUp,YUp,ZUp);
  gp_Dir Xpers = Ypers.Crossed(Zpers);
  gp_Ax3 Axe (At, Zpers, Xpers);
  gp_Trsf T;
  T.SetTransformation(Axe);
  MyProjector = HLRAlgo_Projector(T,Pers,Focus);
}


HLRAlgo_Projector Prs3d_Projector::Projector () const 
{
  return MyProjector;
}
