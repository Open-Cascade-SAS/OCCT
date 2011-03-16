// File:	BRepFill_DraftLaw.cxx
// Created:	Wed Jan 14 14:41:23 1998
// Author:	Philippe MANGIN
//		<pmn@sgi29>


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
