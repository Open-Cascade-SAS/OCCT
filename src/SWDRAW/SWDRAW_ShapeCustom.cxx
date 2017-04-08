// Created on: 1999-03-09
// Created by: Roman LYGIN
// Copyright (c) 1999-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <ShapeCustom.hxx>
#include <ShapeCustom_BSplineRestriction.hxx>
#include <ShapeCustom_RestrictionParameters.hxx>
#include <ShapeFix.hxx>
#include <SWDRAW.hxx>
#include <SWDRAW_ShapeCustom.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfInteger.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

static Standard_Integer ContToInteger( const GeomAbs_Shape Cont)
{
  Standard_Integer result =0;
  switch(Cont) {
    case GeomAbs_C0:
    case GeomAbs_G1: result = 0; break;
    case GeomAbs_C1:
    case GeomAbs_G2: result = 1; break;
    case GeomAbs_C2: result = 2; break;  
    case GeomAbs_C3: result = 3; break;
    default : result = 4; break;
  }
  return result;
}

static Standard_Integer directfaces(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) { di<<"Donner un nom de SHAPE + un nom de RESULTAT\n"; return 1 /* Error */; }
  Standard_CString arg1 = argv[1];
  Standard_CString arg2 = argv[2];
  TopoDS_Shape Shape = DBRep::Get(arg2);
  if (Shape.IsNull()) { di<<"Shape unknown : "<<arg2<<"\n"; return 1 /* Error */; }

  TopoDS_Shape result = ShapeCustom::DirectFaces (Shape);
  if (result.IsNull()) { di<<"NO RESULT\n"; return 1; }
  else if (result == Shape) { di<<"No modif\n"; return 0; }
  di<<"DirectFaces -> Result : "<<arg1<<"\n";
  DBRep::Set (arg1,result);
  return 0; // Done
}

static Standard_Integer ckeckKnots(const TColStd_Array1OfReal& theKnots,
                                   Standard_Real theFirst,
                                   Standard_Real theLast)
{
  Standard_Integer i =1, nb = theKnots.Length();
  Standard_Integer aNum =0;
  for( ; i< nb; i++)
  {
    if(!aNum)
    {
      Standard_Real aDF = theKnots(i) - theFirst;
      if(fabs(aDF) <= Precision::PConfusion() ||
      aDF > Precision::PConfusion())
      {
        aNum++;
        continue;
      }
    }
    else 
    {
      Standard_Real aDL = theKnots(i) - theLast;
      if(fabs(aDL) <= Precision::PConfusion() ||
        aDL > Precision::PConfusion())
        break;
      aNum++;

    }

  }
  return aNum;
}

static void expcurv2d(const Handle(Geom2d_Curve)& aCurve, TColStd_Array2OfInteger& Nb, const Standard_Integer Degree,
                      const Standard_Integer MaxSeg,
                      const Standard_Integer theCont,
                      Standard_Real theFirst,
                      Standard_Real theLast)   
{
  if(aCurve.IsNull()) return;
  
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) {
    Handle(Geom2d_TrimmedCurve) tmp = Handle(Geom2d_TrimmedCurve)::DownCast (aCurve);
    Handle(Geom2d_Curve) BasCurve = tmp->BasisCurve();
    expcurv2d(BasCurve,Nb,Degree,MaxSeg,theCont,theFirst,theLast);
    return;
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve))) {
    Handle(Geom2d_OffsetCurve) tmp = Handle(Geom2d_OffsetCurve)::DownCast (aCurve);
    Handle(Geom2d_Curve) BasCurve = tmp->BasisCurve();
    expcurv2d(BasCurve,Nb,Degree,MaxSeg,theCont,theFirst,theLast);
    return;
  }
  Standard_Integer aCont = ContToInteger(aCurve->Continuity());
  if(aCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
    Handle(Geom2d_BSplineCurve) Bs = Handle(Geom2d_BSplineCurve)::DownCast(aCurve);
    if(Bs->Degree() > Degree) Nb.ChangeValue(1,1)++; 
    if(Bs->NbKnots() -1 > MaxSeg)  Nb.ChangeValue(1,2)++;
    if(Bs->IsRational())  Nb.ChangeValue(1,3)++;
    if(aCont < theCont && Bs->NbKnots() > 2)
    {
      TColStd_Array1OfReal aKnots(1,Bs->NbKnots());
      Bs->Knots(aKnots);
      Standard_Integer nbInt = ckeckKnots(aKnots,theFirst,theLast);
      if(nbInt >1)
        Nb.ChangeValue(1,4)++;
    }
    return;  
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))) {
    Handle(Geom2d_BezierCurve) Bs = Handle(Geom2d_BezierCurve)::DownCast(aCurve); 
    if(Bs->Degree() > Degree) Nb.ChangeValue(2,1)++;
    if(Bs->IsRational())  Nb.ChangeValue(2,3)++;
    if(aCont < theCont)
    {

      Nb.ChangeValue(2,4)++;
    }
    return;
  }
  else
    Nb.ChangeValue(1,5)++;
  return;
}

static void expcurv(const Handle(Geom_Curve)& aCurve, TColStd_Array2OfInteger& Nb, const Standard_Integer Degree,
                    const Standard_Integer MaxSeg,
                    const Standard_Integer theCont,
                    Standard_Real theFirst,
                    Standard_Real theLast)   
{
  if(aCurve.IsNull()) return;
  if (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    Handle(Geom_TrimmedCurve) tmp = Handle(Geom_TrimmedCurve)::DownCast (aCurve);
    Handle(Geom_Curve) BasCurve = tmp->BasisCurve();
    expcurv(BasCurve,Nb,Degree,MaxSeg,theCont,theFirst,theLast);
    return;
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
    Handle(Geom_OffsetCurve) tmp = Handle(Geom_OffsetCurve)::DownCast (aCurve);
    Handle(Geom_Curve) BasCurve = tmp->BasisCurve();
    expcurv(BasCurve,Nb,Degree,MaxSeg,theCont,theFirst,theLast);
    return;
  }

  Standard_Integer aCont = ContToInteger(aCurve->Continuity()); 
  if(aCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    Handle(Geom_BSplineCurve) Bs = Handle(Geom_BSplineCurve)::DownCast(aCurve);
    if(Bs->Degree() > Degree) Nb.ChangeValue(1,1)++; 
    if(Bs->NbKnots() -1 > MaxSeg)  Nb.ChangeValue(1,2)++;
    if(Bs->IsRational())  Nb.ChangeValue(1,3)++;
    if(aCont < theCont && Bs->NbKnots() > 2) 
    {
      TColStd_Array1OfReal aKnots(1,Bs->NbKnots());
      Bs->Knots(aKnots);
      Standard_Integer nbInt = ckeckKnots(aKnots,theFirst,theLast);
      if(nbInt >1)
        Nb.ChangeValue(1,4)++;
    }
    return;  
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom_BezierCurve))) {
    Handle(Geom_BezierCurve) Bs = Handle(Geom_BezierCurve)::DownCast(aCurve); 
    if(Bs->Degree() > Degree) Nb.ChangeValue(2,1)++;
    if(Bs->IsRational())  Nb.ChangeValue(2,3)++;
    if(aCont < theCont) 
      Nb.ChangeValue(2,4)++;
    return;
  }
  else
    Nb.ChangeValue(1,5)++;
  return;
}
static void expsurf(const Handle(Geom_Surface)& aSurface, TColStd_Array2OfInteger& NbSurf, const Standard_Integer Degree,
                    const Standard_Integer MaxSeg,
                    const Standard_Integer theCont)   
{
  if (aSurface.IsNull()) return;
  if (aSurface->IsKind(STANDARD_TYPE(Geom_SweptSurface))) {      
    Handle(Geom_SweptSurface) aSurf = Handle(Geom_SweptSurface)::DownCast(aSurface);
    Handle(Geom_Curve) BasCurve = aSurf->BasisCurve();
    expcurv(BasCurve,NbSurf,Degree,MaxSeg,theCont,BasCurve->FirstParameter(),
      BasCurve->LastParameter()); 
    return;  
  }
  if (aSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) aSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
    Handle(Geom_Surface) theSurf = aSurf->BasisSurface();
    expsurf(theSurf,NbSurf,Degree,MaxSeg,theCont);
    return;
  }
  if(aSurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) { 
    Handle(Geom_OffsetSurface) aSurf = Handle(Geom_OffsetSurface)::DownCast(aSurface);
    Handle(Geom_Surface) theSurf = aSurf->BasisSurface();
    expsurf(theSurf,NbSurf,Degree,MaxSeg,theCont);
    return;

  }
  Standard_Integer aCont = ContToInteger(aSurface->Continuity());

  if (aSurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) {
    Handle(Geom_BSplineSurface) BS = Handle(Geom_BSplineSurface)::DownCast(aSurface); 
    if(BS->UDegree() > Degree || BS->VDegree() > Degree) NbSurf.ChangeValue(1,1)++;
    if(((BS->NbUKnots()-1) * (BS->NbVKnots()-1)) >MaxSeg) NbSurf.ChangeValue(1,2)++;
    if(BS->IsURational() || BS->IsVRational()) NbSurf.ChangeValue(1,3)++;
    if(aCont < theCont)
      NbSurf.ChangeValue(1,4)++;
    return;
  }
  if (aSurface->IsKind(STANDARD_TYPE(Geom_BezierSurface))) { 
    Handle(Geom_BezierSurface) BS = Handle(Geom_BezierSurface)::DownCast(aSurface);
    if(BS->UDegree() > Degree || BS->VDegree() > Degree) NbSurf.ChangeValue(2,1)++;
    if(BS->IsURational() || BS->IsVRational()) NbSurf.ChangeValue(2,3)++;
    if(aCont < theCont)
      NbSurf.ChangeValue(2,4)++;
    return;
  }
  if(aSurface->IsKind(STANDARD_TYPE(Geom_Plane)))
  {
    NbSurf.ChangeValue(1,5)++;
  }
  else
    NbSurf.ChangeValue(2,5)++;
}

static Standard_Integer expshape(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc <4) {
    di << "Incorrect number of arguments. Must be 3\n";
    return 1 /* Error */;
  }
  Standard_CString arg2 = argv[1];
  TopoDS_Shape Shape = DBRep::Get(arg2);
  Standard_Integer Degree = Draw::Atoi(argv[2]);
  Standard_Integer MaxSeg = Draw::Atoi(argv[3]);
  GeomAbs_Shape aCont3 = GeomAbs_C0;
  Standard_Integer k = 4;
  if( argc > k)
  {
    if(strcmp(argv[k],"C0") == 0)  aCont3 = GeomAbs_C0;
    else if (strcmp(argv[k],"C1") == 0)  aCont3 = GeomAbs_C1;
    else if (strcmp(argv[k],"C2") == 0)  aCont3 = GeomAbs_C2;
    else if (strcmp(argv[k],"C3") == 0)  aCont3 = GeomAbs_C3;
    else if (strcmp(argv[k],"CN") == 0)  aCont3 = GeomAbs_CN;
    else if (strcmp(argv[k],"G1") == 0)  aCont3 = GeomAbs_C0;
    else if (strcmp(argv[k],"G2") == 0)  aCont3 = GeomAbs_C1;
    else {
      di<< "Invalid argument Cont3e\n"; return 1; }


  }

  Standard_Integer aCont = ContToInteger(aCont3);
  TColStd_Array2OfInteger NbSurf(1,2,1,5);
  TColStd_Array2OfInteger NbCurv(1,2,1,5);
  TColStd_Array2OfInteger NbCurv2d(1,2,1,5);
  Standard_Integer nbSeam =0;
  NbSurf.Init(0);
  NbCurv.Init(0);
  NbCurv2d.Init(0);
  if (Shape.IsNull()) { di << "Shape unknown: " << arg2 << "\n"; return 1 /* Error */; }
  TopExp_Explorer Ex;
  
  Standard_Integer nbF =1;
  for (Ex.Init(Shape,TopAbs_FACE); Ex.More(); Ex.Next(),nbF++) {
    TopoDS_Face F = TopoDS::Face(Ex.Current());
    TopLoc_Location L;
    Handle(Geom_Surface) aSurface = BRep_Tool::Surface(F,L);
    expsurf(aSurface,NbSurf,Degree,MaxSeg,aCont);
    TopExp_Explorer exp;

    Standard_Integer nbE = 1;
    for(exp.Init(F,TopAbs_EDGE); exp.More(); exp.Next(),nbE++) {
      TopoDS_Edge E = TopoDS::Edge(exp.Current());
      if(BRep_Tool::IsClosed(E,F))
        nbSeam++;
      Standard_Real First,Last;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(E,L,First, Last);
      expcurv(aCurve,NbCurv,Degree,MaxSeg,aCont,First, Last);
      Standard_Real First2d,Last2d;
      Handle(Geom2d_Curve) aCurve2d = BRep_Tool::CurveOnSurface(E,F,First2d,Last2d);
      expcurv2d(aCurve2d,NbCurv2d,Degree,MaxSeg,aCont,First2d,Last2d);
    }
  }
  di<<"Number of seam edges - "<<nbSeam<<"\n";
  //if(NbSurf.Value(1,1) !=0)
  di<< "Number of BSpline surfaces with degree more then "<< Degree << " - " <<NbSurf.Value(1,1)<<"\n";
  //if(NbSurf.Value(1,2) !=0)
  di<< "Number of BSpline surfaces with number of spans more then "<< MaxSeg << " - " <<NbSurf.Value(1,2)<<"\n";
  //if(NbSurf.Value(1,3) !=0)
  di<< "Number of Rational BSpline surfaces " << NbSurf.Value(1,3)<<"\n";
  //if(NbSurf.Value(1,4) !=0)
  di<< "Number of BSpline surfaces with continuity less than specified - " << NbSurf.Value(1,4)<<"\n";
  //if(NbSurf.Value(2,1) !=0)
  di<< "Number of Bezier surfaces with degree more then "<< Degree << " - " <<NbSurf.Value(2,1)<<"\n";
  //if(NbSurf.Value(2,3) !=0)
  di<< "Number of Rational Bezier surfaces  - " <<NbSurf.Value(2,3)<<"\n";
  //if(NbSurf.Value(2,4) !=0)
  di<< "Number of Bezier surfaces with continuity less than specified - " << NbSurf.Value(2,4)<<"\n";

  //if(NbSurf.Value(1,5) !=0)
  di<< "Number of Planes - " << NbSurf.Value(1,5)<<"\n";
  //if(NbSurf.Value(2,5) !=0)
  di<< "Number of other surfaces - " << NbSurf.Value(2,5)<<"\n";

  //if(NbCurv.Value(1,1) !=0)
  di<< "Number of BSpline curves with degree more then "<< Degree << " - " <<NbCurv.Value(1,1)<<"\n";
  //if(NbCurv.Value(1,2) !=0)
  di<< "Number of BSpline curves with number of spans more then - "<< MaxSeg << " - " <<NbCurv.Value(1,2)<<"\n";
  //if(NbCurv.Value(1,3) !=0)
  di<< "Number of Rational BSpline curves " << NbCurv.Value(1,3)<<"\n";
  //if(NbCurv.Value(1,4) !=0)
  di<< "Number of  BSpline curves with less continuity - " << NbCurv.Value(1,4)<<"\n";
  //if(NbCurv.Value(2,1) !=0)
  di<< "Number of Bezier curves with degree more then - "<< Degree << " - " <<NbCurv.Value(2,1)<<"\n";
  //if(NbCurv.Value(2,3) !=0)
  di<< "Number of Rational Bezier curves  - " <<NbCurv.Value(2,3)<<"\n"; 
  //if(NbCurv.Value(2,4) !=0)
  di<< "Number of  Bezier curves with less continuity - " << NbCurv.Value(2,4)<<"\n";

  //if(NbCurv.Value(1,5) !=0)
  di<< "Number of  other curves - " << NbCurv.Value(1,5)<<"\n";

  //if(NbCurv2d.Value(1,1) !=0)
  di<< "Number of BSpline pcurves with degree more then - "<< Degree << " - " <<NbCurv2d.Value(1,1)<<"\n";
  //if(NbCurv2d.Value(1,2) !=0)
  di<< "Number of BSpline pcurves with number of spans more then "<< MaxSeg << " - " <<NbCurv2d.Value(1,2)<<"\n";
  //if(NbCurv2d.Value(1,3) !=0)
  di<< "Number of Rational BSpline pcurves - " << NbCurv2d.Value(1,3)<<"\n";
  // if(NbCurv2d.Value(1,4) !=0)
  di<< "Number of  BSpline pcurves with less continuity - " << NbCurv2d.Value(1,4)<<"\n";
  //if(NbCurv2d.Value(2,1) !=0)
  di<< "Number of Bezier pcurves with degree more then "<< Degree <<" -  - " <<NbCurv2d.Value(2,1)<<"\n";
  //if(NbCurv2d.Value(2,3) !=0)
  di<< "Number of Rational Bezier pcurves  - " <<NbCurv2d.Value(2,3)<<"\n";
  //if(NbCurv2d.Value(2,4) !=0)
  di<< "Number of  Bezier pcurves with less continuity - " << NbCurv2d.Value(2,4)<<"\n";

  //if(NbCurv2d.Value(1,5) !=0)
  di<< "Number of  other pcurves - " << NbCurv2d.Value(1,5)<<"\n";
  return 0;
}

static Standard_Integer scaleshape(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc !=4) {
    di << "Incorrect number of arguments. Must be 4\n";
    return 1 /* Error */;
  }
  Standard_CString arg2 = argv[2];
  TopoDS_Shape Shape = DBRep::Get(arg2);
  if (Shape.IsNull()) { di << "Shape unknown: " << arg2 << "\n"; return 1 /* Error */; }

  TopoDS_Shape result = ShapeCustom::ScaleShape (Shape, Draw::Atof(argv[3]));
  if (result.IsNull()) { di << "NO RESULT\n"; return 1; }
  else if (result == Shape) { di << "NO MODIFICATIONS\n"; return 0; }
  DBRep::Set (argv[1],result);
  return 0;
}
static Standard_Integer BSplRes(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 11) {
    di << "Incorrect number of arguments. Must be 10\n";
    return 1 /* Error */;
  }
  Standard_CString arg2 = argv[2];
  TopoDS_Shape Shape = DBRep::Get(arg2);
  if (Shape.IsNull()) { di << "Shape unknown: " << arg2 << "\n"; return 1 /* Error */; }
  GeomAbs_Shape aCont3;
  if(strcmp(argv[7],"C0") == 0)  aCont3 = GeomAbs_C0;
  else if (strcmp(argv[7],"C1") == 0)  aCont3 = GeomAbs_C1;
  else if (strcmp(argv[7],"C2") == 0)  aCont3 = GeomAbs_C2;
  else if (strcmp(argv[7],"C3") == 0)  aCont3 = GeomAbs_C3;
  else if (strcmp(argv[7],"CN") == 0)  aCont3 = GeomAbs_CN;
  else if (strcmp(argv[7],"G1") == 0)  aCont3 = GeomAbs_C0;
  else if (strcmp(argv[7],"G2") == 0)  aCont3 = GeomAbs_C1;
  else {
    di<< "Invalid argument Cont3e\n"; return 1; }
  GeomAbs_Shape aCont2;
  if(strcmp(argv[8],"C0") == 0)  aCont2 = GeomAbs_C0;
  else if (strcmp(argv[8],"C1") == 0)  aCont2 = GeomAbs_C1;
  else if (strcmp(argv[8],"C2") == 0)  aCont2 = GeomAbs_C2;
  else if (strcmp(argv[8],"C3") == 0)  aCont2 = GeomAbs_C3;
  else if (strcmp(argv[8],"CN") == 0)  aCont2 = GeomAbs_CN;
  else if (strcmp(argv[8],"G1") == 0)  aCont2 = GeomAbs_C0;
  else if (strcmp(argv[8],"G2") == 0)  aCont2 = GeomAbs_C1;
  else {
    di<< "Invalid argument Cont3e\n"; return 1; }

  Handle(ShapeCustom_RestrictionParameters) aParameters = new ShapeCustom_RestrictionParameters;
  TopoDS_Shape result = ShapeCustom::BSplineRestriction (Shape, Draw::Atof(argv[3]), Draw::Atof(argv[4]), Draw::Atoi(argv[5]),Draw::Atoi(argv[6]),aCont3,aCont2, Draw::Atoi(argv[9]) != 0, Draw::Atoi(argv[10]) != 0,aParameters);
  if (result.IsNull()) { di << "NO RESULT\n"; return 1; }
  else if (result == Shape) { 
    di << "NO MODIFICATIONS\n";
    DBRep::Set (argv[1],result); return 0; 
  }
  ShapeFix::SameParameter(result,Standard_False);

  DBRep::Set (argv[1],result);
  return 0;
}

static Standard_Integer convtorevol(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) { di<<"Convert surfaces to revolution: convtorevol result shape\n"; return 1; }

  Standard_CString arg1 = argv[1];
  Standard_CString arg2 = argv[2];
  TopoDS_Shape Shape = DBRep::Get(arg2);
  if (Shape.IsNull()) { di<<"Shape unknown : "<<arg2<<"\n"; return 1; }

  TopoDS_Shape result = ShapeCustom::ConvertToRevolution (Shape);
  if (result.IsNull()) { di<<"NO RESULT\n"; return 1; }
  else if (result == Shape) { di<<"No modif\n"; return 0; }
  di<<"ConvertToRevolution -> Result : "<<arg1<<"\n";
  DBRep::Set (arg1,result);
  return 0; // Done
}  

//======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================

void SWDRAW_ShapeCustom::InitCommands(Draw_Interpretor& theCommands) 
{
  static Standard_Integer initactor = 0;
  if (initactor)
  {
    return;
  }
  initactor = 1;

  Standard_CString g = SWDRAW::GroupName();

  theCommands.Add ("directfaces", "directfaces result shape", __FILE__, directfaces, g);
  theCommands.Add ("expshape", "expshape shape maxdegree maxseg [min_continuity]", __FILE__, expshape, g);
  theCommands.Add ("scaleshape", "scaleshape result shape scale", __FILE__, scaleshape, g);
  theCommands.Add ("bsplres", "BSplineRestriction result shape tol3d tol2d reqdegree reqnbsegments continuity3d continuity2d PriorDeg RationalConvert", __FILE__,BSplRes, g);
  theCommands.Add ("convtorevol", "convtorevol result shape", __FILE__, convtorevol, g);
}
