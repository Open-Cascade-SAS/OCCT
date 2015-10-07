// Created on: 1997-03-10
// Created by: Stagiaire Francois DUMONT
// Copyright (c) 1997-1999 Matra Datavision
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
#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgo.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <ElCLib.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomLProp.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <ShapeFix_Shape.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColGeom_Array1OfBSplineCurve.hxx>
#include <TColGeom_HArray1OfBSplineCurve.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_SequenceOfBoolean.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_SequenceOfShape.hxx>

//Temporary
#//include <DrawTrSurf.hxx>


//=======================================================================
//function : ConcatenateWire
//purpose  : 
//=======================================================================

TopoDS_Wire  BRepAlgo::ConcatenateWire(const TopoDS_Wire& W,
  const GeomAbs_Shape Option,
  const Standard_Real TolAngular) 
{


  Standard_Integer        nb_curve,                         //number of curves in the Wire
    index;
  BRepTools_WireExplorer  WExp(W) ;
  TopoDS_Edge             edge;
  TopLoc_Location         L ;
  Standard_Real           First=0.,Last=0.,                       //extremal values for the curve
    First0 = 0.,
    toler = 0.,
    tolleft,tolright;                 //Vertex tolerances
  TopoDS_Vertex           Vfirst,Vlast;                     //Vertex of the Wire
  gp_Pnt                  Pfirst,Plast;            //, Pint;  corresponding points

  BRepLib_MakeWire        MakeResult;                       
  Standard_Real           closed_tolerance =0.0;
  Standard_Boolean        closed_flag = Standard_False ;

  nb_curve = 0;

  while ( WExp.More()){                                     //computation of the curve number
    nb_curve++ ;
    WExp.Next();
  }

  if (nb_curve > 1) {
    TColGeom_Array1OfBSplineCurve tab(0,nb_curve-1);          //array of the wire's curve
    TColStd_Array1OfReal tabtolvertex(0,nb_curve-2);          //array of the tolerance's vertex

    WExp.Init(W);

    for (index=0 ;index<nb_curve; index++){                   //main loop
      edge = WExp.Current() ;
      const Handle(Geom_Curve)& aCurve = BRep_Tool::Curve(edge, L, First, Last);
      Handle(Geom_TrimmedCurve) aTrCurve = new Geom_TrimmedCurve(aCurve, First, Last);
      tab(index) = GeomConvert::CurveToBSplineCurve(aTrCurve); //storage in a array 
      tab(index)->Transform(L.Transformation());
      GeomConvert::C0BSplineToC1BSplineCurve(tab(index),Precision::Confusion());

      if (index >= 1){                                         //continuity test loop
        if (edge.Orientation()==TopAbs_REVERSED)
          tab(index)->Reverse();
        tolleft=BRep_Tool::Tolerance(TopExp::LastVertex(edge));
        tolright=BRep_Tool::Tolerance(TopExp::FirstVertex(edge));
        tabtolvertex(index-1)=Max(tolleft,tolright);
      }

      if(index==0){                                           //storage of the first edge features
        First0=First;
        if(edge.Orientation()==TopAbs_REVERSED){             //(usefull for the closed wire) 
          Vfirst=TopExp::LastVertex(edge);
          tab(index)->Reverse();
        }
        else
          Vfirst=TopExp::FirstVertex(edge);
      }

      if(index==nb_curve-1){                                  //storage of the last edge features
        if(edge.Orientation()==TopAbs_REVERSED)
          Vlast=TopExp::FirstVertex(edge);
        else
          Vlast=TopExp::LastVertex(edge);
      }
      WExp.Next() ; 
    }

    if (BRep_Tool::Tolerance(Vfirst)>BRep_Tool::Tolerance(Vlast)) //computation of the closing tolerance
      toler=BRep_Tool::Tolerance(Vfirst);
    else
      toler=BRep_Tool::Tolerance(Vlast);

    Pfirst=BRep_Tool::Pnt(Vfirst);
    Plast=BRep_Tool::Pnt(Vlast); 

    if ((Pfirst.Distance(Plast)<=toler)&&                   //C0 continuity test at the closing point
      (GeomLProp::Continuity(tab(nb_curve-1),tab(0),Last,First0,
      Standard_True,Standard_True, 
      toler, TolAngular)>=GeomAbs_G1)) 
    {
      closed_tolerance =toler;                                        //if ClosedG1!=0 it will be True and
      closed_flag = Standard_True ;
    }                                                        //with the toler value
    Handle(TColGeom_HArray1OfBSplineCurve)  concatcurve;     //array of the concatenated curves
    Handle(TColStd_HArray1OfInteger)        ArrayOfIndices;  //array of the remining Vertex
    if (Option==GeomAbs_G1)
      GeomConvert::ConcatG1(tab,
      tabtolvertex,
      concatcurve,
      closed_flag,
      closed_tolerance) ;    //G1 concatenation
    else
      GeomConvert::ConcatC1(tab,
      tabtolvertex,
      ArrayOfIndices,
      concatcurve,
      closed_flag,
      closed_tolerance);   //C1 concatenation

    for (index=0;index<=(concatcurve->Length()-1);index++){    //building of the resulting Wire
      BRepLib_MakeEdge EdgeBuilder(concatcurve->Value(index));
      edge = EdgeBuilder.Edge();
      MakeResult.Add(edge);
    } 

  }
  else {

    WExp.Init(W);

    edge = WExp.Current() ;
    const Handle(Geom_Curve)& aC = BRep_Tool::Curve(edge,L,First,Last);
    Handle(Geom_BSplineCurve) aBS  = GeomConvert::CurveToBSplineCurve(new Geom_TrimmedCurve(aC,First,Last));
    aBS->Transform(L.Transformation());
    GeomConvert::C0BSplineToC1BSplineCurve(aBS, Precision::Confusion());
    if (edge.Orientation()==TopAbs_REVERSED)
    {
      aBS->Reverse();
    }

    BRepLib_MakeEdge EdgeBuilder(aBS);
    edge = EdgeBuilder.Edge();
    MakeResult.Add(edge);
  }
  return MakeResult.Wire() ;  

}

//=======================================================================
//function : ConcatenateWireC0
//purpose  : 
//=======================================================================

TopoDS_Edge  BRepAlgo::ConcatenateWireC0(const TopoDS_Wire& aWire)
{
  Standard_Real LinTol = Precision::Confusion();
  Standard_Real AngTol = Precision::Angular();

  TopoDS_Edge ResEdge;

  TopoDS_Wire theWire = aWire;
  BRepLib::BuildCurves3d(theWire);
  Handle(ShapeFix_Shape) Fixer = new ShapeFix_Shape(theWire);
  Fixer->SetPrecision(LinTol);
  Fixer->SetMaxTolerance(LinTol);
  Fixer->Perform();
  theWire = TopoDS::Wire(Fixer->Shape());

  TColGeom_SequenceOfCurve CurveSeq;
  TColStd_SequenceOfReal FparSeq;
  TColStd_SequenceOfReal LparSeq;
  TColStd_SequenceOfReal TolSeq;
  TColStd_SequenceOfBoolean IsFwdSeq;
  GeomAbs_CurveType CurType = GeomAbs_OtherCurve;
  TopoDS_Vertex FirstVertex, LastVertex;

  BRepTools_WireExplorer wexp(theWire);

  for (; wexp.More(); wexp.Next()) {
    TopoDS_Edge anEdge = wexp.Current();
    Standard_Real fpar, lpar;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, fpar, lpar);

    if (aCurve.IsNull())
      continue;

    GeomAdaptor_Curve  aGACurve(aCurve);
    GeomAbs_CurveType  aType       = aGACurve.GetType();
    Handle(Geom_Curve) aBasisCurve = aGACurve.Curve();
    Standard_Boolean   isFwd       = (wexp.Orientation() != TopAbs_REVERSED);

    if (aBasisCurve->IsPeriodic()) {
      ElCLib::AdjustPeriodic
        (aBasisCurve->FirstParameter(), aBasisCurve->LastParameter(),
        Precision::PConfusion(), fpar, lpar);
    }

    if (CurveSeq.IsEmpty()) {
      CurveSeq.Append(aCurve);
      FparSeq.Append(fpar);
      LparSeq.Append(lpar);
      IsFwdSeq.Append(isFwd);
      CurType     = aType;
      FirstVertex = wexp.CurrentVertex();
    } else {
      Standard_Boolean isSameCurve = Standard_False;
      Standard_Real NewFpar = RealFirst(), NewLpar = RealLast();
      GeomAdaptor_Curve GAprevcurve(CurveSeq.Last());

      if (aCurve == CurveSeq.Last()) {
        NewFpar = fpar;
        NewLpar = lpar;
        isSameCurve = Standard_True;
      } else if (aType == CurType) {
          switch (aType) {
          case GeomAbs_Line:
            {
              gp_Lin aLine    = aGACurve.Line();
              gp_Lin PrevLine = GAprevcurve.Line(); 

              if (aLine.Contains(PrevLine.Location(), LinTol) &&
                aLine.Direction().IsParallel(PrevLine.Direction(), AngTol)) {
                  gp_Pnt P1 = ElCLib::Value(fpar, aLine);
                  gp_Pnt P2 = ElCLib::Value(lpar, aLine);

                  NewFpar = ElCLib::Parameter(PrevLine, P1);
                  NewLpar = ElCLib::Parameter(PrevLine, P2);
                  isSameCurve = Standard_True;
              }
              break;
            }
          case GeomAbs_Circle:
            {
              gp_Circ aCircle    = aGACurve.Circle();
              gp_Circ PrevCircle = GAprevcurve.Circle();

              if (aCircle.Location().Distance(PrevCircle.Location()) <= LinTol &&
                Abs(aCircle.Radius() - PrevCircle.Radius()) <= LinTol &&
                aCircle.Axis().IsParallel(PrevCircle.Axis(), AngTol)) {
                  gp_Pnt P1 = ElCLib::Value(fpar, aCircle);
                  gp_Pnt P2 = ElCLib::Value(lpar, aCircle);

                  NewFpar = ElCLib::Parameter(PrevCircle, P1);
                  NewLpar = ElCLib::Parameter(PrevCircle, P2);
                  isSameCurve = Standard_True;
              }
              break;
            }
          case GeomAbs_Ellipse:
            {
              gp_Elips anEllipse   = aGACurve.Ellipse();
              gp_Elips PrevEllipse = GAprevcurve.Ellipse();

              if (anEllipse.Focus1().Distance(PrevEllipse.Focus1()) <= LinTol &&
                anEllipse.Focus2().Distance(PrevEllipse.Focus2()) <= LinTol &&
                Abs(anEllipse.MajorRadius() - PrevEllipse.MajorRadius()) <= LinTol &&
                Abs(anEllipse.MinorRadius() - PrevEllipse.MinorRadius()) <= LinTol &&
                anEllipse.Axis().IsParallel(PrevEllipse.Axis(), AngTol)) {
                  gp_Pnt P1 = ElCLib::Value(fpar, anEllipse);
                  gp_Pnt P2 = ElCLib::Value(lpar, anEllipse);

                  NewFpar = ElCLib::Parameter(PrevEllipse, P1);
                  NewLpar = ElCLib::Parameter(PrevEllipse, P2);
                  isSameCurve = Standard_True;
              }
              break;
            }
          case GeomAbs_Hyperbola:
            {
              gp_Hypr aHypr    = aGACurve.Hyperbola();
              gp_Hypr PrevHypr = GAprevcurve.Hyperbola();

              if (aHypr.Focus1().Distance(PrevHypr.Focus1()) <= LinTol &&
                aHypr.Focus2().Distance(PrevHypr.Focus2()) <= LinTol &&
                Abs(aHypr.MajorRadius() - PrevHypr.MajorRadius()) <= LinTol &&
                Abs(aHypr.MinorRadius() - PrevHypr.MinorRadius()) <= LinTol &&
                aHypr.Axis().IsParallel(PrevHypr.Axis(), AngTol)) {
                  gp_Pnt P1 = ElCLib::Value(fpar, aHypr);
                  gp_Pnt P2 = ElCLib::Value(lpar, aHypr);

                  NewFpar = ElCLib::Parameter(PrevHypr, P1);
                  NewLpar = ElCLib::Parameter(PrevHypr, P2);
                  isSameCurve = Standard_True;
              }
              break;
            }
          case GeomAbs_Parabola:
            {
              gp_Parab aParab    = aGACurve.Parabola();
              gp_Parab PrevParab = GAprevcurve.Parabola();

              if (aParab.Location().Distance(PrevParab.Location()) <= LinTol &&
                aParab.Focus().Distance(PrevParab.Focus()) <= LinTol &&
                Abs(aParab.Focal() - PrevParab.Focal()) <= LinTol &&
                aParab.Axis().IsParallel(PrevParab.Axis(), AngTol)) {
                  gp_Pnt P1 = ElCLib::Value(fpar, aParab);
                  gp_Pnt P2 = ElCLib::Value(lpar, aParab);

                  NewFpar = ElCLib::Parameter(PrevParab, P1);
                  NewLpar = ElCLib::Parameter(PrevParab, P2);
                  isSameCurve = Standard_True;
              }
              break;
            }
          default:
            break;
          } //end of switch
      } //end of else

      if (isSameCurve) {
        const Standard_Boolean isSameDir = (isFwd == IsFwdSeq.Last());

        if (aBasisCurve->IsPeriodic()) {
          // Treat periodic curves.
          const Standard_Real aPeriod = aBasisCurve->Period();

          if (isSameDir) {
            // Check if first parameter is greater then the last one.
            while (NewFpar > NewLpar) {
              NewFpar -= aPeriod;
            }
          } else { // !isSameDir
            // Check if last parameter is greater then the first one.
            while (NewLpar > NewFpar) {
              NewLpar -= aPeriod;
            }

            // Change parameters
            const Standard_Real aTmpPar = NewLpar;

            NewLpar = NewFpar;
            NewFpar = aTmpPar;
          }

          // Udjust parameters on periodic curves.
          if (IsFwdSeq.Last()) {
            // The current curve should be after the previous one.
            ElCLib::AdjustPeriodic(LparSeq.Last(), LparSeq.Last() + aPeriod,
              Precision::PConfusion(), NewFpar, NewLpar);
          } else {
            // The current curve should be before the previous one.
            ElCLib::AdjustPeriodic(FparSeq.Last() - aPeriod, FparSeq.Last(),
              Precision::PConfusion(), NewFpar, NewLpar);
          }
        } else if (!isSameDir) {
          // Not periodic curves. Opposite dirs.
          const Standard_Real aTmpPar = NewLpar;

          NewLpar = NewFpar;
          NewFpar = aTmpPar;
        }

        if (IsFwdSeq.Last()) {
          // Update last parameter 
          LparSeq(LparSeq.Length()) = NewLpar;
        } else {
          // Update first parameter 
          FparSeq(FparSeq.Length()) = NewFpar;
        }
      } else {
        // Add new curve.
        CurveSeq.Append(aCurve);
        FparSeq.Append(fpar);
        LparSeq.Append(lpar);
        IsFwdSeq.Append(isFwd);
        TolSeq.Append(BRep_Tool::Tolerance(wexp.CurrentVertex()));
        CurType = aType;
      }
    }
  }

  LastVertex = wexp.CurrentVertex();
  TolSeq.Append(BRep_Tool::Tolerance(LastVertex));

  Standard_Boolean isReverse = Standard_False;

  if (!IsFwdSeq.IsEmpty()) {
    isReverse = !IsFwdSeq(1);
  }

  TopoDS_Vertex FirstVtx_final = (isReverse)? LastVertex : FirstVertex;
  FirstVtx_final.Orientation(TopAbs_FORWARD);
  TopoDS_Vertex LastVtx_final = (isReverse)? FirstVertex : LastVertex;
  LastVtx_final.Orientation(TopAbs_REVERSED);

  if (CurveSeq.IsEmpty())
    return ResEdge;

  Standard_Integer nb_curve = CurveSeq.Length();   //number of curves
  TColGeom_Array1OfBSplineCurve tab(0,nb_curve-1);                    //array of the curves
  TColStd_Array1OfReal tabtolvertex(0,nb_curve-1); //(0,nb_curve-2);  //array of the tolerances

  Standard_Integer i;

  if (nb_curve > 1)
  {
    for (i = 1; i <= nb_curve; i++)
    {
      if (CurveSeq(i)->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
        CurveSeq(i) = (*((Handle(Geom_TrimmedCurve)*)&(CurveSeq(i))))->BasisCurve();

      Handle(Geom_TrimmedCurve) aTrCurve = new Geom_TrimmedCurve(CurveSeq(i), FparSeq(i), LparSeq(i));
      tab(i-1) = GeomConvert::CurveToBSplineCurve(aTrCurve);
      GeomConvert::C0BSplineToC1BSplineCurve(tab(i-1), Precision::Confusion());

      if (!IsFwdSeq(i)) {
        tab(i-1)->Reverse();
      }

      //Temporary
      //char* name = new char[100];
      //sprintf(name, "c%d", i);
      //DrawTrSurf::Set(name, tab(i-1));

      if (i > 1)
        tabtolvertex(i-2) = TolSeq(i-1) * 5.;
    }
    tabtolvertex(nb_curve-1) = TolSeq(TolSeq.Length()) * 5.;

    Standard_Boolean closed_flag = Standard_False;
    Standard_Real closed_tolerance = 0.;
    if (FirstVertex.IsSame(LastVertex) &&
      GeomLProp::Continuity(tab(0), tab(nb_curve-1),
      tab(0)->FirstParameter(),
      tab(nb_curve-1)->LastParameter(),
      Standard_False, Standard_False, LinTol, AngTol) >= GeomAbs_G1)
    {
      closed_flag = Standard_True ;
      closed_tolerance = BRep_Tool::Tolerance(FirstVertex);
    }

    Handle(TColGeom_HArray1OfBSplineCurve)  concatcurve;     //array of the concatenated curves
    Handle(TColStd_HArray1OfInteger)        ArrayOfIndices;  //array of the remining Vertex
    GeomConvert::ConcatC1(tab,
      tabtolvertex,
      ArrayOfIndices,
      concatcurve,
      closed_flag,
      closed_tolerance);   //C1 concatenation

    if (concatcurve->Length() > 1)
    {
      Standard_Real MaxTolVer = LinTol;
      for (i = 1; i <= TolSeq.Length(); i++)
        if (TolSeq(i) > MaxTolVer)
          MaxTolVer = TolSeq(i);
      MaxTolVer *= 5.;

      GeomConvert_CompCurveToBSplineCurve Concat(concatcurve->Value(concatcurve->Lower()));

      for (i = concatcurve->Lower()+1; i <= concatcurve->Upper(); i++)
        Concat.Add( concatcurve->Value(i), MaxTolVer, Standard_True );

      concatcurve->SetValue(concatcurve->Lower(), Concat.BSplineCurve());
    }

    ResEdge = BRepLib_MakeEdge(concatcurve->Value(concatcurve->Lower()),
      FirstVtx_final, LastVtx_final,
      concatcurve->Value(concatcurve->Lower())->FirstParameter(),
      concatcurve->Value(concatcurve->Lower())->LastParameter());
  }
  else
  {
    if (CurveSeq(1)->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
      CurveSeq(1) = (*((Handle(Geom_TrimmedCurve)*)&(CurveSeq(1))))->BasisCurve();

    Handle(Geom_Curve) aCopyCurve =
      Handle(Geom_Curve)::DownCast(CurveSeq(1)->Copy());

    ResEdge = BRepLib_MakeEdge(aCopyCurve,
      FirstVtx_final, LastVtx_final,
      FparSeq(1), LparSeq(1));
  }

  if (isReverse)
    ResEdge.Reverse();

  return ResEdge;
}
