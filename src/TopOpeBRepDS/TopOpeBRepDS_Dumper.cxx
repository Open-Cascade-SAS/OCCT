// Created on: 1994-08-04
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <GeomTools_CurveSet.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Stream.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_PointExplorer.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>
#include <TopOpeBRepDS_SurfaceExplorer.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

#include <string.h>
//=======================================================================
//function : TopOpeBRepDS_Dumper::TopOpeBRepDS_Dumper
//purpose  : 
//=======================================================================
TopOpeBRepDS_Dumper::TopOpeBRepDS_Dumper(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  myHDS = HDS;
}

//=======================================================================
//function : PrintType
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::PrintType(const Handle(Geom_Curve)&
#ifdef OCCT_DEBUG
                                                                           C
#endif
                                                 ,Standard_OStream& OS)
{
#ifdef OCCT_DEBUG
  if (C.IsNull()) {
    OS<<"NULL CURVE";
    return OS;
  }
  Handle(Standard_Type) T = C->DynamicType();
  if      (T==STANDARD_TYPE(Geom_Circle))       OS<<"CIRCLE";
  else if (T==STANDARD_TYPE(Geom_Line))         OS<<"LINE";
  else if (T==STANDARD_TYPE(Geom_Ellipse))      OS<<"ELLIPSE";
  else if (T==STANDARD_TYPE(Geom_Parabola))     OS<<"ELLIPSE";
  else if (T==STANDARD_TYPE(Geom_Hyperbola))    OS<<"HYPERBOLA";
  else if (T==STANDARD_TYPE(Geom_BezierCurve))  OS<<"BEZIER";
  else if (T==STANDARD_TYPE(Geom_BSplineCurve)) 
    OS<<"BSPLINE("<<Handle(Geom_BSplineCurve)::DownCast (C)->Degree()<<")";
  else if (T==STANDARD_TYPE(Geom_TrimmedCurve))
    {OS<<"TRIMMED ";PrintType(Handle(Geom_TrimmedCurve)::DownCast(C)->BasisCurve(),OS);}
  else if (T==STANDARD_TYPE(Geom_OffsetCurve))  OS<<"OFFSETCURVE";
  else                                 OS<<"***UNKNOWN***";
  if (T!=STANDARD_TYPE(Geom_TrimmedCurve)) OS<<"3D";
#endif
  
  return OS;
}

//=======================================================================
//function : PrintType
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::PrintType(const Handle(Geom2d_Curve)&
#ifdef OCCT_DEBUG
                                                                             C
#endif
                                                 ,Standard_OStream& OS)
{
#ifdef OCCT_DEBUG
  if (C.IsNull()) {
    OS<<"NULL CURVE2d";
    return OS;
  }
  Handle(Standard_Type) T = C->DynamicType();
  if      (T==STANDARD_TYPE(Geom2d_Circle))       OS<<"CIRCLE";
  else if (T==STANDARD_TYPE(Geom2d_Line))         OS<<"LINE";
  else if (T==STANDARD_TYPE(Geom2d_Ellipse))      OS<<"ELLIPSE";
  else if (T==STANDARD_TYPE(Geom2d_Parabola))     OS<<"ELLIPSE";
  else if (T==STANDARD_TYPE(Geom2d_Hyperbola))    OS<<"HYPERBOLA";
  else if (T==STANDARD_TYPE(Geom2d_BezierCurve))  OS<<"BEZIER";
  else if (T==STANDARD_TYPE(Geom2d_BSplineCurve)) 
    OS<<"BSPLINE("<<Handle(Geom2d_BSplineCurve)::DownCast (C)->Degree()<<")";
  else if (T==STANDARD_TYPE(Geom2d_TrimmedCurve)) 
    {OS<<"TRIMMED ";PrintType(Handle(Geom2d_TrimmedCurve)::DownCast(C)->BasisCurve(),OS);}
  else if (T==STANDARD_TYPE(Geom2d_OffsetCurve))  OS<<"OFFSETCURVE";
  else                                   OS<<"***UNKNOWN***";
  if (T!=STANDARD_TYPE(Geom2d_TrimmedCurve)) OS<<"2D";
#endif
  
  return OS;
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::Print(const gp_Pnt&
#ifdef OCCT_DEBUG
                                                           P
#endif
                                             ,Standard_OStream& OS)
{
#ifdef OCCT_DEBUG
  OS<<P.X()<<" "<<P.Y()<<" "<<P.Z()<<" ";
#endif
  
  return OS;
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::Print(const gp_Pnt2d&
#ifdef OCCT_DEBUG
                                                             P
#endif
                                             ,Standard_OStream& OS)
{
#ifdef OCCT_DEBUG
  OS<<P.X()<<" "<<P.Y()<<" ";
#endif
  
  return OS;
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::Print(const Handle(Geom_BSplineCurve)&
#ifdef OCCT_DEBUG
                                                                            B
#endif
                                             ,Standard_OStream& OS
                                             ,const Standard_Boolean
#ifdef OCCT_DEBUG
                                                                     compact
#endif
                                             )
{
#ifdef OCCT_DEBUG
  Standard_Integer i,l,lb = 0,n;
  
  Standard_Integer degree   = B->Degree();
  Standard_Integer nbpoles  = B->NbPoles();
  Standard_Integer nbknots  = B->NbKnots();
  Standard_Boolean rational = B->IsRational();
  Standard_Boolean periodic = B->IsPeriodic();
  
  OS<<"BSplineCurve : "; 
  if (periodic) OS<<"periodic, ";
  OS<<"degree "<<degree<<", "<<nbpoles<<" poles, "<<nbknots<<" knots"<<endl;
  
  //l = strlen("poles : ") + 1
  for (i = 1,OS<<"poles : ",l = 9,n = nbpoles; 
       i<=n; 
       i++) {
    if (!compact)      { if (i == 1) lb = 0; else lb = l; }
    else if (i == 1)   lb = 0; 
    else if (i == 2)   lb = l;
    else if (i == n-1) {
      lb = l; 
      if(n > 4) OS << setw(lb) << "\t  ...\n";
    }
    else if (i == n)   lb = l;
    else continue;
    
    OS<<setw(lb)<<i<<" : ";
    TopOpeBRepDS_Dumper::Print(B->Pole(i),OS);
    if (rational) OS<<" "<<B->Weight(i);
    
    OS<<endl;
  }
  
  //l = strlen("knots : ") + 1
  for (i = 1,OS<<"knots : ",l = 9,n = nbknots;
       i<=n;
       i++) {
    if (!compact)      { if (i == 1) lb = 0; else lb = l; }
    else if (i == 1)   lb = 0;
    else if (i == 2)   lb = l;
    else if (i == n-1) {
      if(n > 4) OS << setw(lb) << "\t  ...\n";
      lb = l;
    }
    else if (i == n)   lb = l;
    else continue;
    
    OS<<setw(lb)<<i<<" : ";
    OS<<B->Knot(i)<<" "<<B->Multiplicity(i);
    
    OS<<endl;
  }
#endif
  
  return OS;
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::Print(const Handle(Geom2d_BSplineCurve)&
#ifdef OCCT_DEBUG
                                                                                B
#endif
                                             ,Standard_OStream& OS
                                             ,const Standard_Boolean
#ifdef OCCT_DEBUG
                                                                     compact
#endif
                                             )
{
#ifdef OCCT_DEBUG
  Standard_Integer i,l,lb=-1,n;
  
  Standard_Integer degree   = B->Degree();
  Standard_Integer nbpoles  = B->NbPoles();
  Standard_Integer nbknots  = B->NbKnots();
  Standard_Boolean rational = B->IsRational();
  Standard_Boolean periodic = B->IsPeriodic();
  
  OS<<"BSplineCurve : "; 
  if (periodic) OS<<"periodic, ";
  OS<<"degree "<<degree<<", "<<nbpoles<<" poles, "<<nbknots<<" knots"<<endl;
  
  //l = strlen("poles : ") + 1
  for (i = 1,OS<<"poles : ",l = 9,n = nbpoles; 
       i<=n; 
       i++) {
    if (!compact)      { if (i == 1) lb = 0; else lb = l; }
    else if (i == 1)   lb = 0; 
    else if (i == 2)   lb = l;
    else if (i == n-1) {
      lb = l;
      if(n > 4) OS << setw(lb) << "\t  ...\n";
    }
    else if (i == n)   lb = l;
    else continue;
    
    OS<<setw(lb)<<i<<" : ";
    TopOpeBRepDS_Dumper::Print(B->Pole(i),OS);
    if (rational) OS<<" "<<B->Weight(i);
    
    OS<<endl;
  }
  
  //l = strlen("knots : ") + 1
  for (i = 1,OS<<"knots : ",l = 9,n = nbknots; 
       i<=n; 
       i++) {
    if (!compact)      { if (i == 1) lb = 0; else lb = l; }
    else if (i == 1)   lb = 0;
    else if (i == 2)   lb = l;
    else if (i == n-1) {
      lb = l;
      if(n > 4) OS << setw(lb) << "\t  ...\n";
    }
    else if (i == n)   lb = l;
    else continue;
    
    OS<<setw(lb)<<i<<" : "; 
    OS<<B->Knot(i)<<" "<<B->Multiplicity(i);
    
    OS<<endl;
  }
#endif
  
  return OS;
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::Print(const Handle(Geom_Curve)&
#ifdef OCCT_DEBUG
                                                                       C
#endif
                                             ,Standard_OStream& OS
                                             ,const Standard_Boolean
#ifdef OCCT_DEBUG
                                                                      compact
#endif
                                             )
{
#ifdef OCCT_DEBUG
#define DBSP3D(tut) (*((Handle(Geom_BSplineCurve)*)&(tut)))
  if (!C.IsNull()) {
    if (C->DynamicType() == STANDARD_TYPE(Geom_BSplineCurve))
      TopOpeBRepDS_Dumper::Print(DBSP3D(C),OS,compact);
    else 
      GeomTools_CurveSet::PrintCurve(C,OS,Standard_False);
  }
#endif
  
  return OS;
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

#ifdef OCCT_DEBUG
Standard_OStream& TopOpeBRepDS_Dumper::Print(const Handle(Geom2d_Curve)& C,
				Standard_OStream& OS,const Standard_Boolean compact)
{
#define DBSP2D(tut) (*((Handle(Geom2d_BSplineCurve)*)&(tut)))
  if (!C.IsNull()) {
    if (C->DynamicType() == STANDARD_TYPE(Geom2d_BSplineCurve)) 
      TopOpeBRepDS_Dumper::Print(DBSP2D(C),OS,compact);
    else 
      GeomTools_Curve2dSet::PrintCurve2d(C,OS,Standard_False);
  }
  
  return OS;
}
#endif

// DUMP OF HDATASTRUCTURE

//-----------------------------------------------------------------------
#ifdef OCCT_DEBUG
//unreferenced function, commented
/*static void souligne(const TCollection_AsciiString& t,Standard_OStream& OS,const char c = '-') 
{ for(char i=0;i<t.Length();i++)OS<<c; }*/
//-----------------------------------------------------------------------
// print the name of a Kind on a AsciiString.
static TCollection_AsciiString& Print(const TopOpeBRepDS_Kind k,TCollection_AsciiString& s)
{
  switch(k) {
  case TopOpeBRepDS_SURFACE : s.AssignCat("SURFACE");break;
  case TopOpeBRepDS_CURVE   : s.AssignCat("CURVE");break;
  case TopOpeBRepDS_POINT   : s.AssignCat("POINT");break;
  case TopOpeBRepDS_SOLID   : s.AssignCat("SOLID");break;
  case TopOpeBRepDS_SHELL   : s.AssignCat("SHELL");break;
  case TopOpeBRepDS_FACE    : s.AssignCat("FACE");break;
  case TopOpeBRepDS_WIRE    : s.AssignCat("WIRE");break;
  case TopOpeBRepDS_EDGE    : s.AssignCat("EDGE");break;
  case TopOpeBRepDS_VERTEX  : s.AssignCat("VERTEX");break;
  default:
    break ;
  }
  return s;
}
#endif


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::Dump(Standard_OStream& OS
                                            ,const Standard_Boolean
#ifdef OCCT_DEBUG
                                                                    findkeep
#endif
                                            ,const Standard_Boolean
#ifdef OCCT_DEBUG
                                                                    compact
#endif
                                            ) const
{
#ifdef OCCT_DEBUG
  OS<<endl<<"==== HDS start filling resume"<<endl;
  DumpGeometry(OS,findkeep,compact);
  DumpTopology(OS);
  OS<<endl<<"==== HDS end filling resume"<<endl;
#endif
  return OS;
}


//=======================================================================
//function : DumpGeometry
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Dumper::DumpGeometry(Standard_OStream& OS
                                                    ,const Standard_Boolean
#ifdef OCCT_DEBUG
                                                                            findkeep
#endif
                                                    ,const Standard_Boolean
#ifdef OCCT_DEBUG
                                                                            compact
#endif
                                                    ) const
{
#ifdef OCCT_DEBUG
  DumpGeometry(TopOpeBRepDS_POINT,OS,findkeep,compact);
  DumpGeometry(TopOpeBRepDS_CURVE,OS,findkeep,compact);
  DumpGeometry(TopOpeBRepDS_SURFACE,OS,findkeep,compact);
#endif
  return OS;
}


//=======================================================================
//function : DumpGeometry
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::DumpGeometry
                                       (const TopOpeBRepDS_Kind
#ifdef OCCT_DEBUG
                                                                K
#endif
                                        ,Standard_OStream& OS
                                        ,const Standard_Boolean
#ifdef OCCT_DEBUG
                                                                findkeep
#endif
                                        ,const Standard_Boolean
#ifdef OCCT_DEBUG
                                                                compact
#endif
                                        ) const
{
#ifdef OCCT_DEBUG
  if ( ! TopOpeBRepDS::IsGeometry(K) ) return OS;
  Standard_Integer nt = myHDS->NbGeometry(K);
  if (!nt) return OS;
  
  Standard_Integer n=0;
  if     (K==TopOpeBRepDS_POINT){TopOpeBRepDS_PointExplorer ex(myHDS->DS(),findkeep);n=ex.NbPoint();}
  else if(K==TopOpeBRepDS_CURVE){TopOpeBRepDS_CurveExplorer ex(myHDS->DS());n=ex.NbCurve();}
  else if(K==TopOpeBRepDS_SURFACE){TopOpeBRepDS_SurfaceExplorer ex(myHDS->DS());n=ex.NbSurface();}
  if (!n) return OS;
  TCollection_AsciiString s = "---- "; ::Print(K,s);
  OS<<endl<<s<<" : "<<n<<endl;
  for(Standard_Integer i=1;i<=nt;i++) DumpGeometry(K,i,OS,findkeep,compact);
#endif
  return OS;
}

//=======================================================================
//function : DumpGeometry
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::DumpGeometry(const TopOpeBRepDS_Kind
#ifdef OCCT_DEBUG
                                                                            K
#endif
                                                    ,const Standard_Integer
#ifdef OCCT_DEBUG
                                                                            I
#endif
                                                    ,Standard_OStream& OS,
				                    const Standard_Boolean
#ifdef OCCT_DEBUG
                                                                           findkeep
#endif
                                                    ,const Standard_Boolean
#ifdef OCCT_DEBUG
                                                                            compact
#endif
                                                     ) const
{
#ifdef OCCT_DEBUG
  if ( ! TopOpeBRepDS::IsGeometry(K) ) return OS;
  
  if      ( K == TopOpeBRepDS_POINT ) {
    TopOpeBRepDS_PointExplorer pex(myHDS->DS());
    Standard_Boolean isp = pex.IsPoint(I);
    Standard_Boolean isk = pex.IsPointKeep(I);
    if(!findkeep || isk) {
      TCollection_AsciiString kstr;::Print(K,kstr);kstr=kstr+" "+I+" :";
      Standard_Integer lk = kstr.Length();
      OS<<kstr;
      if (isp) {
	const TopOpeBRepDS_Point& PDS = pex.Point(I);
	OS<<" (K:"<<((isk)?"1":"0") <<")"<<endl;
	OS<<"tol = "<<PDS.Tolerance()<<endl; 
	TCollection_AsciiString istr(lk,' ');
	const gp_Pnt& P = PDS.Point();
	OS<<istr; OS<<P.X()<<" "<<P.Y()<<" "<<P.Z()<<endl;
      }
      else OS<<"unbounded"<<endl;
    }
  }
  else if ( K == TopOpeBRepDS_CURVE ) {
    TopOpeBRepDS_CurveExplorer cex(myHDS->DS());
    Standard_Boolean isc = cex.IsCurve(I);
    Standard_Boolean isk = cex.IsCurveKeep(I);
    if(!findkeep || isk) {
      TCollection_AsciiString kstr;::Print(K,kstr);kstr=kstr+" "+I+" :";
      Standard_Integer lk = kstr.Length();
      OS<<kstr;
      if (isc) {
	const TopOpeBRepDS_Curve& CDS = cex.Curve(I);
	OS<<" (K:"<<((isk)?"1":"0")<<")";
	OS<<" (M:"<<CDS.Mother()<<")"<<endl;
	TCollection_AsciiString istr(lk+1,' ');
	OS<<istr;PrintType(CDS.Curve(),OS);
	OS<<" tol = "<<CDS.Tolerance()<<endl;
	Standard_Real first,last;
	if(CDS.Range(first,last))
	  OS<<istr<<"first = "<<first<<"  last = "<<last<<endl;
	TopOpeBRepDS_InterferenceIterator it(myHDS->DS().CurveInterferences(I));
	for (;it.More();it.Next()) it.Value()->Dump(OS,istr,"\n");
	{
	  const Handle(TopOpeBRepDS_Interference)& SCI1 = CDS.GetSCI1();
	  const Handle(TopOpeBRepDS_Interference)& SCI2 = CDS.GetSCI2();
	  Handle(Geom2d_Curve) PC1,PC2;
	  if ( !SCI1.IsNull())
	    PC1 = Handle(TopOpeBRepDS_SurfaceCurveInterference)::DownCast(SCI1)->PCurve();
	  if ( !SCI2.IsNull())
	    PC2 = Handle(TopOpeBRepDS_SurfaceCurveInterference)::DownCast(SCI2)->PCurve();
	  
	  OS<<istr;OS<<"PC(SCI1) : ";Print(PC1,cout,compact);
	  OS<<istr;OS<<"PC(SCI2) : ";Print(PC2,cout,compact);
	  OS<<endl;
	}
      }
      else OS<<"unbounded"<<endl;
    }
  }
  
  else if ( K == TopOpeBRepDS_SURFACE ) {
    TopOpeBRepDS_SurfaceExplorer sex(myHDS->DS());
    Standard_Boolean iss = sex.IsSurface(I);
    Standard_Boolean isk = sex.IsSurfaceKeep(I);
    if(!findkeep || isk) {
      TCollection_AsciiString kstr; ::Print(K,kstr); kstr=kstr+" "+I+" : ";
      Standard_Integer lk = kstr.Length();
      OS<<kstr;
      if (iss) {
	const TopOpeBRepDS_Surface& SDS = sex.Surface(I);
	OS<<" (K:"<<((isk)?"1":"0") <<")"<<endl;
	OS<<"tol = "<<SDS.Tolerance()<<endl; 
	{
	  TCollection_AsciiString istr(lk,' ');
	  TopOpeBRepDS_InterferenceIterator it(myHDS->SurfaceCurves(I));
	  for (;it.More();it.Next()) { 
	    OS<<istr; it.Value()->Dump(OS);OS<<endl; 
	  }
	}
      }
      else OS<<"unbounded"<<endl;
    }
  }
  else {
    return OS;
  }
  
#endif
  return OS;
}

// = = = = = = =  T O P O L O G Y

//=======================================================================
//function : DumpTopology
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Dumper::DumpTopology(Standard_OStream& OS) const
{
#ifdef OCCT_DEBUG
  DumpTopology(TopOpeBRepDS_VERTEX,OS);
  DumpTopology(TopOpeBRepDS_EDGE,OS);
  DumpTopology(TopOpeBRepDS_WIRE,OS);
  DumpTopology(TopOpeBRepDS_FACE,OS);
  DumpTopology(TopOpeBRepDS_SHELL,OS);
  DumpTopology(TopOpeBRepDS_SOLID,OS);
  DumpSectionEdge(TopOpeBRepDS_EDGE,OS);
#endif
  return OS;
}

//=======================================================================
//function : DumpTopology
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Dumper::DumpTopology(const TopOpeBRepDS_Kind
#ifdef OCCT_DEBUG
                                                                            K
#endif
                                                    ,Standard_OStream& OS) const
{
#ifdef OCCT_DEBUG
  if ( ! TopOpeBRepDS::IsTopology(K) ) return OS;
  Standard_Integer nk = myHDS->NbTopology(K);
  if ( ! nk ) return OS;
  
  OS<<endl;
  TCollection_AsciiString s = "---- "; ::Print(K,s); 
  Standard_Integer nt = myHDS->NbTopology();
  OS<<s<<" : "<<nk<<"/"<<nt<<" shapes"<<endl;
  for(Standard_Integer i = 1; i <= nt; i++) DumpTopology(K,i,OS);
#endif
  return OS;
}

//=======================================================================
//function : DumpTopology
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Dumper::DumpTopology(const TopAbs_ShapeEnum
#ifdef OCCT_DEBUG
                                                                           T
#endif
                                                    ,Standard_OStream& OS) const
{
#ifdef OCCT_DEBUG
  DumpTopology(TopOpeBRepDS::ShapeToKind(T),OS);
#endif
  return OS;
}

//=======================================================================
//function : DumpSectionEdge
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::DumpSectionEdge(const TopOpeBRepDS_Kind
#ifdef OCCT_DEBUG
                                                                               K
#endif
                                                       ,Standard_OStream& OS) const
{
#ifdef OCCT_DEBUG
  Standard_Integer nse = myHDS->DS().NbSectionEdges();
  if ( ! nse ) return OS;
  
  OS<<endl;
  TCollection_AsciiString s = "---- ";
  OS<<s<<"Section Edges"<<" : "<<nse<<" edges"<<endl;
  for(Standard_Integer i = 1; i <= nse; i++) {
    DumpSectionEdge(K,i,OS);
  }
#endif
  return OS;
}

//=======================================================================
//function : DumpSectionEdge
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Dumper::DumpSectionEdge(const TopOpeBRepDS_Kind K,const Standard_Integer I,Standard_OStream& OS) const
{
  Standard_Boolean SEFindKeep = Standard_False;
  const TopOpeBRepDS_DataStructure& DS = myHDS->DS();
  const TopoDS_Shape& E = DS.SectionEdge(I,SEFindKeep);
  if(E.IsNull()) return OS;
  Standard_Integer it = myHDS->DS().Shape(E,SEFindKeep);
  if (it == 0) {
    // E trouvee "Restriction" par les intersections n'est pas dans la DS.
    OS<<endl<<"EDGE section "<<I<<" non Shape()"<<endl;
  }
  else DumpTopology(K,it,OS);
  return OS;
}

Standard_EXPORT Standard_Integer TopOpeBRepDS_GLOBALHDSinterfselector = 0; // NYI arg of Dump method

//=======================================================================
//function : DumpTopology
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Dumper::DumpTopology(const TopOpeBRepDS_Kind
#ifdef OCCT_DEBUG
                                                                            K
#endif
                                                    ,const Standard_Integer
#ifdef OCCT_DEBUG
                                                                            I
#endif
                                                    ,Standard_OStream& OS) const
{
#ifdef OCCT_DEBUG
  Standard_Boolean fk = Standard_False;
  const TopOpeBRepDS_DataStructure& DS = myHDS->DS();
  if ( ! TopOpeBRepDS::IsTopology(K) ) return OS;
  if ( I <= 0 ) return OS;
  TopAbs_ShapeEnum t = TopOpeBRepDS::KindToShape(K);
  
  if ( DS.Shape(I,fk).ShapeType() != t ) return OS;
  Standard_Integer rankI = DS.AncestorRank(I);
  TCollection_AsciiString s1,s2;
  if(DS.KeepShape(I)) {
    s1 = ((rankI == 1) ? (char*) "*" : (char*) "");
    s2 = ((rankI == 2) ? (char*) "*" : (char*) "");
  }
  else {
    s1 = ((rankI == 1) ? (char*) "~" : (char*) "");
    s2 = ((rankI == 2) ? (char*) "~" : (char*) "");
  }
  TCollection_AsciiString ss = TopOpeBRepDS::SPrint(K,I,s1,s2);
  
  if ( t == TopAbs_VERTEX ) {
    Standard_Integer lk = ss.Length() + 3;
    TCollection_AsciiString istr(lk,' ');
    gp_Pnt P = TopOpeBRepTool_ShapeTool::Pnt(DS.Shape(I,fk));
    Standard_Real tol = TopOpeBRepTool_ShapeTool::Tolerance(DS.Shape(I,fk));
    OS<<"\n"<<ss<<" : ";
    OS<<"tol = "<<tol<<endl;
    OS<<istr; OS<<P.X()<<" "<<P.Y()<<" "<<P.Z()<<endl;
  }
  
  Standard_Integer lss = ss.Length();TCollection_AsciiString bss(lss,' ');
  OS<<ss;OS.flush();
  
  // Interferences
  TopOpeBRepDS_InterferenceIterator it; it.Init(DS.ShapeInterferences(I));
  if (it.More()) {
    OS<<" interferences :\n";
    for (;it.More();it.Next()) {
      const Handle(TopOpeBRepDS_Interference)& IR = it.Value(); Standard_Boolean s = Standard_True;
      if (TopOpeBRepDS_GLOBALHDSinterfselector == 1)
	s = !(Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(IR)).IsNull();
      if (s) IR->Dump(OS,"","\n");
    }
  }
  else OS<<" has no interference\n";
  
  // SameDomain
  const TopoDS_Shape& S = myHDS->Shape(I,fk);
  if ( myHDS->HasSameDomain(S) ) {
    const TopTools_ListOfShape& L = myHDS->DS().ShapeSameDomain(S); 
    if ( L.Extent() ) {
      OS<<ss<<" same domain (r,o)  : "<<SDumpRefOri(S)<<endl;
      OS<<SPrintShapeRefOri(L,bss+" same domain shapes : ");
    }
  }
  
  OS.flush();
#endif
  return OS;
}

//=======================================================================
//function : DumpTopology
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Dumper::DumpTopology(const TopAbs_ShapeEnum
#ifdef OCCT_DEBUG
                                                                           T
#endif
                                                    ,const Standard_Integer
#ifdef OCCT_DEBUG
                                                                            I
#endif
                                                    ,Standard_OStream& OS) const
{
#ifdef OCCT_DEBUG
  DumpTopology(TopOpeBRepDS::ShapeToKind(T),I,OS);
#endif
  return OS;
}

//=======================================================================
//function : SDumpRefOri
//purpose  : 
//=======================================================================

TCollection_AsciiString TopOpeBRepDS_Dumper::SDumpRefOri(const TopOpeBRepDS_Kind K,const Standard_Integer I) const
{
  TCollection_AsciiString SS;
  Standard_Boolean fk = Standard_False;
  const TopOpeBRepDS_DataStructure& DS = myHDS->DS();
  if ( ! TopOpeBRepDS::IsTopology(K) ) return SS;
  TopAbs_ShapeEnum t = TopOpeBRepDS::KindToShape(K);
  if ( DS.Shape(I,fk).ShapeType() != t ) return SS;
  const TopoDS_Shape& S = myHDS->Shape(I,fk);
  Standard_Integer  r = myHDS->SameDomainReference(S);
  TopOpeBRepDS_Config o = myHDS->SameDomainOrientation(S);
  SS=SS+"("+SPrintShape(r)+","+TopOpeBRepDS::SPrint(o)+")";
  return SS;
}

//=======================================================================
//function : SDumpRefOri
//purpose  : 
//=======================================================================

TCollection_AsciiString TopOpeBRepDS_Dumper::SDumpRefOri(const TopoDS_Shape& S) const
{
  TCollection_AsciiString SS;
  TopOpeBRepDS_Kind k = TopOpeBRepDS::ShapeToKind(S.ShapeType());
  Standard_Boolean fk = Standard_False;
  Standard_Integer i = myHDS->Shape(S,fk);
  SS = SDumpRefOri(k,i);
  return SS;
}

//=======================================================================
//function : DumpRefOri
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::DumpRefOri(const TopOpeBRepDS_Kind
#ifdef OCCT_DEBUG
                                                                          K
#endif
                                                  ,const Standard_Integer
#ifdef OCCT_DEBUG
                                                                          I
#endif
                                                  ,Standard_OStream& OS) const
{
#ifdef OCCT_DEBUG
  OS<<SDumpRefOri(K,I);OS.flush();
#endif
  return OS;
}

//=======================================================================
//function : DumpRefOri
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::DumpRefOri(const TopoDS_Shape&
#ifdef OCCT_DEBUG
                                                                      S
#endif
                                                  ,Standard_OStream& OS) const
{
#ifdef OCCT_DEBUG
  OS<<SDumpRefOri(S);OS.flush();
#endif
  return OS;
}

//=======================================================================
//function : DumpLOI
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Dumper::DumpLOI(
                                const TopOpeBRepDS_ListOfInterference&
#ifdef OCCT_DEBUG
                                                                       L
#endif
                                ,Standard_OStream& OS,
                                const TCollection_AsciiString&
#ifdef OCCT_DEBUG
                                                               str
#endif
                                                                   ) const
{
#ifdef OCCT_DEBUG
  OS<<str;
  TopOpeBRepDS_InterferenceIterator IT(L); if (!IT.More()) { OS<<"NO INTERFERENCE"<<endl; return OS; }
  OS<<endl;
  for (Standard_Integer ip = 1; IT.More(); IT.Next(),ip++ ) {
    const Handle(TopOpeBRepDS_Interference)& I = IT.Value();
    TCollection_AsciiString s(ip); s = s + "/ ";
    DumpI(I,OS,s,"\n");
  }
  OS<<endl;
#endif
  return OS;
}

//=======================================================================
//function : DumpI
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Dumper::DumpI(
                                const Handle(TopOpeBRepDS_Interference)&
#ifdef OCCT_DEBUG
                                                                         I
#endif
                                ,Standard_OStream& OS
				,const TCollection_AsciiString&
#ifdef OCCT_DEBUG
				                                str1
#endif
                                ,const TCollection_AsciiString&
#ifdef OCCT_DEBUG
				                                str2
#endif
                                                                     ) const
{
#ifdef OCCT_DEBUG
  Standard_Boolean fk = Standard_False;
  OS<<str1;  
  if (I.IsNull()) { OS<<"NULL INTERFERENCE"<<str2; return OS; }
  Standard_Integer G = I->Geometry(); TopOpeBRepDS_Kind GK = I->GeometryType();
  I->Dump(OS);
  if      (GK == TopOpeBRepDS_POINT) { 
    gp_Pnt P = myHDS->Point(G).Point();
    OS<<" P = "<<P.X()<<" "<<P.Y()<<" "<<P.Z();
  }
  else if (GK == TopOpeBRepDS_VERTEX) { 
    gp_Pnt P = TopOpeBRepTool_ShapeTool::Pnt(myHDS->Shape(G,fk));
    OS<<" V = "<<P.X()<<" "<<P.Y()<<" "<<P.Z();
  }
  OS<<str2;
  OS.flush();
#endif
  return OS;
}

//=======================================================================
//function : SPrintShape
//purpose  : 
//=======================================================================

TCollection_AsciiString TopOpeBRepDS_Dumper::SPrintShape(const Standard_Integer IS) const
{
  TCollection_AsciiString SS;
  const TopOpeBRepDS_DataStructure& BDS = myHDS->DS(); if ( IS<1 || IS>BDS.NbShapes()) return SS;
  SS = SPrintShape(BDS.Shape(IS));
  return SS;
}

//=======================================================================
//function : SPrintShape
//purpose  : 
//=======================================================================

TCollection_AsciiString TopOpeBRepDS_Dumper::SPrintShape(const TopoDS_Shape& S) const
{
  const TopOpeBRepDS_DataStructure& BDS = myHDS->DS();
  const Standard_Integer IS = myHDS->DS().Shape(S);
  Standard_Integer rankIS = BDS.AncestorRank(IS);
//JR/Hp  TCollection_AsciiString s1,s2;
  Standard_CString s1,s2;
  if(BDS.KeepShape(IS)) {
    s1 = (Standard_CString ) ((rankIS == 1) ? "*" : "");
    s2 = (Standard_CString ) ((rankIS == 2) ? "*" : "");
  }
  else {
    s1 = (Standard_CString ) ((rankIS == 1) ? "~" : "");
    s2 = (Standard_CString ) ((rankIS == 2) ? "~" : "");
  }
  TCollection_AsciiString sse = TopOpeBRepDS::SPrint(TopOpeBRepDS::ShapeToKind(S.ShapeType()),IS,s1,s2);
  return sse;
}

//=======================================================================
//function : SPrintShapeRefOri
//purpose  : 
//=======================================================================

TCollection_AsciiString TopOpeBRepDS_Dumper::SPrintShapeRefOri(const TopoDS_Shape& S,const TCollection_AsciiString& astr) const
{ TCollection_AsciiString SS=astr+SPrintShape(S)+" "+SDumpRefOri(S); return SS; }

//=======================================================================
//function : SPrintShapeRefOri
//purpose  : 
//=======================================================================

TCollection_AsciiString TopOpeBRepDS_Dumper::SPrintShapeRefOri(const TopTools_ListOfShape& L,const TCollection_AsciiString& astr) const
{
  TCollection_AsciiString SS;TopTools_ListIteratorOfListOfShape it(L); if (!it.More()) return SS;
  SS=SS+astr; TCollection_AsciiString bst(astr.Length(),' ');
  for(Standard_Integer il = 0; it.More(); it.Next(),il++) {
    TCollection_AsciiString ss=SPrintShapeRefOri(it.Value());if (il) ss=bst+ss; SS=SS+ss+"\n";
  }
  return SS;
}
