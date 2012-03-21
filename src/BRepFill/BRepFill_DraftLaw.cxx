// Created on: 1998-01-14
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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



#include <BRepFill_DraftLaw.ixx>

#include <GeomFill_LocationDraft.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_HArray1OfLocationLaw.hxx>

#include <gp_Vec.hxx>
#include <gp_Mat.hxx>
#include <gp_XYZ.hxx>
#include <gp_Trsf.hxx>


//=======================================================================
//function : ToG0
//purpose  : Cacul une tranformation T tq T.M2 = M1
//=======================================================================

static void ToG0(const gp_Mat& M1, const gp_Mat& M2, gp_Mat& T) {
  T =  M2.Inverted();
  T *= M1;
}


 BRepFill_DraftLaw::BRepFill_DraftLaw(const TopoDS_Wire & Path,
				     const Handle(GeomFill_LocationDraft) & Law)
                                    :BRepFill_Edge3DLaw(Path, Law)
{
}

 void BRepFill_DraftLaw::CleanLaw(const Standard_Real TolAngular) 
{
  Standard_Real First, Last;//, Angle;
  Standard_Integer ipath;
  gp_Mat Trsf, M1, M2;
  gp_Vec V, T1, T2, N1, N2;
//  gp_Dir D;

  myLaws->Value(1)->GetDomain(First, Last);
// D = Handle(GeomFill_LocationDraft)::DownCast(myLaws->Value(1))->Direction();
//  gp_Vec Vd(D);

  for (ipath=2; ipath<=myLaws->Length(); ipath++) {
    myLaws->Value(ipath-1)->D0(Last, M1, V);
    myLaws->Value(ipath)->GetDomain(First, Last);
    myLaws->Value(ipath)->D0(First, M2, V);
    T1.SetXYZ(M1.Column(3));
    T2.SetXYZ(M2.Column(3));
    N1.SetXYZ(M1.Column(1));
    N2.SetXYZ(M2.Column(1));
    if (N1.IsParallel(N2, TolAngular)) { // Correction G0 des normales...
      ToG0(M1, M2, Trsf);
      myLaws->Value(ipath)->SetTrsf(Trsf);
    }
  } 
}
