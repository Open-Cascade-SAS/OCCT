// Created on: 1998-01-14
// Created by: Philippe MANGIN
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


#include <BRepFill_DraftLaw.hxx>
#include <GeomFill_HArray1OfLocationLaw.hxx>
#include <GeomFill_LocationDraft.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <gp_Mat.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Wire.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepFill_DraftLaw,BRepFill_Edge3DLaw)

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
