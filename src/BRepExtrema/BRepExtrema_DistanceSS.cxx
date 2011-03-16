// File:	BRepExtrema_DistanceSS.cxx
// Created:	Mon Apr 22 17:03:37 1996
// Author:	Maria PUMBORIOS
// Author:      Herve LOUESSARD 
//		<mps@sgi64>


#include <BRepExtrema_DistanceSS.ixx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <BRep_Tool.hxx>
#include <BRepExtrema_SupportType.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>
#include <BRepExtrema_SolutionElem.hxx>
#include <BRepExtrema_SeqOfSolution.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx> 
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <Bnd_Box.hxx>
#include <BRepExtrema_ExtPC.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <BRepExtrema_ExtCC.hxx>
#include <BRepExtrema_ExtCF.hxx>
#include <BRepExtrema_ExtFF.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <TopAbs.hxx>
#include <Geom_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <TColgp_Array1OfPnt.hxx>



/*********************************************************************************/
/*********************************************************************************/

//------------------------------------------------------------------------------
// function: TRI_SOLUTION
//------------------------------------------------------------------------------
void TRI_SOLUTION (const BRepExtrema_SeqOfSolution& SeqSol,
		   const gp_Pnt& Pt, Standard_Boolean& inserer )
{
 inserer = Standard_True;
 Standard_Integer Nbsol= SeqSol.Length();
 Standard_Integer i;
 Standard_Real dst;
 for (i=1;i<=Nbsol;i++)
    {gp_Pnt P1=SeqSol.Value(i).Point();
     dst=P1.Distance(Pt);
     if (dst<=Precision::Confusion()) inserer=Standard_False;}  
}  

//------------------------------------------------------------------------------
// function: MIN_SOLUTION
//------------------------------------------------------------------------------
void MIN_SOLUTION (const BRepExtrema_SeqOfSolution& SeqSol1,
		   const BRepExtrema_SeqOfSolution& SeqSol2,
		   const Standard_Real& DstRef,
		   const Standard_Real& Eps,
		   BRepExtrema_SeqOfSolution& seqSol1,
		   BRepExtrema_SeqOfSolution& seqSol2)
{
  Standard_Real dst1;
  Standard_Integer nbSol = SeqSol1.Length();
  
  for (Standard_Integer i = 1; i<=nbSol; i++)
    {
      dst1 = SeqSol1.Value(i).Dist();
      
      if (fabs(dst1 - DstRef) < Eps)
	{	  
	  seqSol1.Append(SeqSol1.Value(i));
	  seqSol2.Append(SeqSol2.Value(i));	
	}
    }
}


//------------------------------------------------------------------------------
// function: TRIM_INFINIT_EDGE
//------------------------------------------------------------------------------
void TRIM_INFINIT_EDGE(const TopoDS_Edge& S1, 
		       const TopoDS_Edge& S2,		       
		       TopoDS_Edge& aResEdge,
		       Standard_Boolean& bIsTrim1,
		       Standard_Boolean& bIsTrim2)
{
  if ( BRep_Tool::Degenerated(S1) ||
       BRep_Tool::Degenerated(S2) )
    return;

  aResEdge = S2;
  Standard_Real aFirst1, aLast1, aFirst2, aLast2;
  Handle(Geom_Curve) pCurv;
  Handle(Geom_Curve) pCurv1 = BRep_Tool::Curve(S1, aFirst1, aLast1);
  Handle(Geom_Curve) pCurv2 = BRep_Tool::Curve(S2, aFirst2, aLast2);

  Standard_Real Umin, Umax;
  Standard_Boolean bUmin, bUmax;
  bUmin = bUmax = Standard_False;

  if (Precision::IsInfinite(aFirst1) &&
      Precision::IsInfinite(aLast1)  &&
      Precision::IsInfinite(aFirst2) &&
      Precision::IsInfinite(aLast2))
    return;

  
  if ( !pCurv1.IsNull() && (Precision::IsInfinite(aFirst1) ||
			    Precision::IsInfinite(aLast1)) )
    {
      pCurv = pCurv1;
      bIsTrim1 = Standard_True;
      if (!Precision::IsInfinite(aFirst1))
	{
	  bUmin = Standard_True;
	  Umin = aFirst1;
	}
      else if (!Precision::IsInfinite(aLast1))
	{
	  bUmax = Standard_True;
	  Umax = aLast1;
	}
    }
  else if ( !pCurv2.IsNull() && (Precision::IsInfinite(aFirst2) ||
				 Precision::IsInfinite(aLast2)) )
    {
      pCurv = pCurv2;
      bIsTrim2 = Standard_True;
      if (!Precision::IsInfinite(aFirst2))
	{
	  bUmin = Standard_True;
	  Umin = aFirst2;
	}
      else if (!Precision::IsInfinite(aLast2))
	{
	  bUmax = Standard_True;
	  Umax = aLast2;
	}
    }
  if (bIsTrim1 || bIsTrim2)
    {
      Bnd_Box aEdgeBox;
      if (bIsTrim1)
	BRepBndLib::Add(S2, aEdgeBox);
      if (bIsTrim2)
	BRepBndLib::Add(S1, aEdgeBox);
      Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
      aEdgeBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);      


      TColStd_Array1OfReal arrU(1, 8);
      TColgp_Array1OfPnt arrPnt(1, 8);

      gp_Pnt aPnt1(Xmin, Ymin, Zmin);
      arrPnt.SetValue(1, aPnt1);
      gp_Pnt aPnt2(Xmin, Ymax, Zmin);
      arrPnt.SetValue(2, aPnt2);
      gp_Pnt aPnt3(Xmin, Ymax, Zmax);
      arrPnt.SetValue(3, aPnt3);
      gp_Pnt aPnt4(Xmin, Ymin, Zmax);
      arrPnt.SetValue(4, aPnt4);
      gp_Pnt aPnt5(Xmax, Ymax, Zmin);
      arrPnt.SetValue(5, aPnt5);
      gp_Pnt aPnt6(Xmax, Ymax, Zmax);
      arrPnt.SetValue(6, aPnt6);
      gp_Pnt aPnt7(Xmax, Ymin, Zmax);
      arrPnt.SetValue(7, aPnt7);
      gp_Pnt aPnt8(Xmax, Ymin, Zmin);
      arrPnt.SetValue(8, aPnt8);
      
      Standard_Real U;
      Standard_Integer i = 1;

      GeomAPI_ProjectPointOnCurve aProj(aPnt1, pCurv);

      for (i = 1; i <= arrPnt.Length(); i++)
	{
	  gp_Pnt aPnt = arrPnt.Value(i);
	  aProj.Perform(aPnt);
	  U = aProj.LowerDistanceParameter();
	  arrU.SetValue(i, U);
	}

      Standard_Real aU;

      if (!bUmin)
	Umin = arrU.Value(1);
	
      if (!bUmax)
	Umax = arrU.Value(1);

      for(i = 1; i <= arrU.Length(); i++)
	{
	  aU = arrU.Value(i);
	  if (aU < Umin)
	    Umin = aU;
	  else if (aU > Umax)
	    Umax = aU;

	}
      GeomAdaptor_Curve aGAC(pCurv);
      Standard_Real tol = Precision::Confusion();
      if (bIsTrim1)
	tol = BRep_Tool::Tolerance(S1);
      else if (bIsTrim2)
	tol = BRep_Tool::Tolerance(S2);
      
      Standard_Real EpsU = aGAC.Resolution(3*tol);
      if (fabs(Umin - Umax) < EpsU)
	{
	  Umin = Umin - EpsU;
	  Umax = Umax + EpsU;
	}


      Handle(Geom_Curve) result = new Geom_TrimmedCurve(pCurv, Umin, Umax);
      aResEdge = BRepBuilderAPI_MakeEdge(result);
    }
}

//------------------------------------------------------------------------------
// function: TRIM_INFINIT_FACE
//------------------------------------------------------------------------------
void TRIM_INFINIT_FACE(const TopoDS_Shape& S1,
		       const TopoDS_Shape& S2,
		       TopoDS_Face& aResFace,
		       Standard_Boolean& bIsInfinit)
{
  bIsInfinit = Standard_False;

  TopAbs_ShapeEnum       Type1, Type2;
  Type1 = S1.ShapeType();     
  Type2 = S2.ShapeType();

  TopoDS_Edge aE;
  TopoDS_Face aF;
  
  if (Type1 == TopAbs_EDGE && Type2 == TopAbs_FACE)
    {
      aE = TopoDS::Edge(S1);
      if ( BRep_Tool::Degenerated(aE) )
	return;
      aF = TopoDS::Face(S2);
    }
  else if (Type2 == TopAbs_EDGE && Type1 == TopAbs_FACE)
    {
      aE = TopoDS::Edge(S2);
      if ( BRep_Tool::Degenerated(aE) )
	return;
      aF = TopoDS::Face(S1);
    }
  else
    {
      bIsInfinit = Standard_False;
      return;
    }  
  Standard_Real Umin, Umax, Vmin, Vmax;

  Standard_Boolean bIsTrim = Standard_False;
  
  
  aResFace = aF;
  Handle(Geom_Surface) pSurf = BRep_Tool::Surface(aF);
 
  Standard_Real U1, V1, U2, V2;
  
  Standard_Boolean bRestrict = BRep_Tool::NaturalRestriction(aF);

  Standard_Boolean bUmin, bUmax, bVmin, bVmax;
  bUmin = bUmax = bVmin = bVmax = Standard_False;

  if (bRestrict)
    {
      pSurf->Bounds(U1, U2, V1, V2);
      if (Precision::IsInfinite(U1))
	bIsTrim = Standard_True;
      else
	{
	  Umin = U1;
	  bUmin = Standard_True;
	}

      if (Precision::IsInfinite(U2))
	bIsTrim = Standard_True;
      else
	{
	  Umax = U2;
	  bUmax = Standard_True;
	}

      if (Precision::IsInfinite(V1))
	bIsTrim = Standard_True;
      else
	{
	  Vmin = V1;
	  bVmin = Standard_True;
	}
      
      if (Precision::IsInfinite(V2))
	bIsTrim = Standard_True;
      else
	{
	  Vmax = V2;
	  bVmax = Standard_True;
	}
    }
  else
    {
      BRepTools::UVBounds(aF, U1, U2, V1, V2);
      if (Precision::IsInfinite(U1) &&
	  Precision::IsInfinite(U2) &&
	  Precision::IsInfinite(V1) &&
	  Precision::IsInfinite(V2))
	bIsTrim = Standard_True;
    }

  if (bIsTrim)
    {      
      Bnd_Box aEdgeBox;
      BRepBndLib::Add(aE, aEdgeBox);

      if(aEdgeBox.IsWhole())
	return;

      Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
      aEdgeBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

      TColStd_Array1OfReal arrU(1, 8);
      TColStd_Array1OfReal arrV(1, 8);
      TColgp_Array1OfPnt arrPnt(1, 8);

      gp_Pnt aPnt1(Xmin, Ymin, Zmin);
      arrPnt.SetValue(1, aPnt1);
      gp_Pnt aPnt2(Xmin, Ymax, Zmin);
      arrPnt.SetValue(2, aPnt2);
      gp_Pnt aPnt3(Xmin, Ymax, Zmax);
      arrPnt.SetValue(3, aPnt3);
      gp_Pnt aPnt4(Xmin, Ymin, Zmax);
      arrPnt.SetValue(4, aPnt4);
      gp_Pnt aPnt5(Xmax, Ymax, Zmin);
      arrPnt.SetValue(5, aPnt5);
      gp_Pnt aPnt6(Xmax, Ymax, Zmax);
      arrPnt.SetValue(6, aPnt6);
      gp_Pnt aPnt7(Xmax, Ymin, Zmax);
      arrPnt.SetValue(7, aPnt7);
      gp_Pnt aPnt8(Xmax, Ymin, Zmin);
      arrPnt.SetValue(8, aPnt8);
      
      Standard_Real U, V;
      Standard_Integer i;
      GeomAPI_ProjectPointOnSurf aProj(aPnt1, pSurf);
      for (i = 1; i <= arrPnt.Length(); i++)
	{
	  gp_Pnt aPnt = arrPnt.Value(i);
	  aProj.Perform(aPnt);
	  if (aProj.IsDone())
	    {
	      aProj.LowerDistanceParameters(U, V);
	      arrU.SetValue(i, U);
	      arrV.SetValue(i, V);	      
	      
	    }
	}

      Standard_Real aU, aV;
      if (!bUmin)
	Umin = arrU.Value(1);
      if (!bUmax)
	Umax = arrU.Value(1);
      if (!bVmin)
	Vmin = arrV.Value(1);
      if (!bVmax)
	Vmax = arrV.Value(1);

      for(i = 1; i <= arrU.Length(); i++)
	{
	  aU = arrU.Value(i);
	  if (aU < Umin)
	    Umin = aU;
	  else if (aU > Umax)
	    Umax = aU;

	  aV = arrV.Value(i);
	  if (aV < Vmin)
	    Vmin = aV;
	  else if (aV > Vmax)
	    Vmax = aV;
	}
      
      GeomAdaptor_Surface aGAS(pSurf);
      Standard_Real tol = BRep_Tool::Tolerance(aF);
      Standard_Real EpsU = aGAS.UResolution(3*tol);
      Standard_Real EpsV = aGAS.VResolution(3*tol);
      if (fabs(Umin - Umax) < EpsU)
	{
	  Umin = Umin - EpsU;
	  Umax = Umax + EpsU;
	}

      if (fabs(Vmin - Vmax) < EpsV)
	{
	  Vmin = Vmin - EpsV;
	  Vmax = Vmax + EpsV;
	}

      Handle(Geom_Surface) result = new Geom_RectangularTrimmedSurface(pSurf, Umin, Umax, Vmin, Vmax);
      aResFace = BRepBuilderAPI_MakeFace(result);
      
      bIsInfinit = Standard_True;
    }
  else
    bIsInfinit = Standard_False;
}

//------------------------------------------------------------------------------
// static function: PERFORM_C0
//------------------------------------------------------------------------------
static void PERFORM_C0(TopoDS_Edge S1, 
		       TopoDS_Edge S2, 
		       BRepExtrema_SeqOfSolution& SeqSol1,
		       BRepExtrema_SeqOfSolution& SeqSol2,
		       Standard_Real DstRef,
		       Standard_Real& mDstRef,
		       const Standard_Real& Eps)
{

  if ( BRep_Tool::Degenerated(S1) ||
      BRep_Tool::Degenerated(S2) )
    return;
  Standard_Integer iE;
//   TopoDS_Edge E, Eover;
  for (iE = 0; iE < 2; iE++)
    {
      TopoDS_Edge E, Eother;
      if (iE == 0)
	{
	  E      = S1;
	  Eother = S2;
	}
      else
	{
	  E      = S2;
	  Eother = S1;
	}
      Standard_Real aFirst, aLast, epsP = Precision::PConfusion(), Udeb, Ufin;
      Handle(Geom_Curve) pCurv = BRep_Tool::Curve(E, aFirst, aLast);
      
      Standard_Real aFOther, aLOther;
      Handle(Geom_Curve) pCurvOther = BRep_Tool::Curve(Eother, aFOther, aLOther);
	


      GeomAbs_Shape s = pCurv->Continuity();
      if (s == GeomAbs_C0)
	{
	  GeomAdaptor_Curve aAdaptorCurve(pCurv, aFirst, aLast);
	  Standard_Integer nbIntervals = aAdaptorCurve.NbIntervals(GeomAbs_C1);
	  
	  TColStd_Array1OfReal arrInter(1,1+nbIntervals);
	  aAdaptorCurve.Intervals(arrInter, GeomAbs_C1);

	  GeomAdaptor_Curve aAdaptorCurveOther(pCurvOther, aFOther, aLOther);
	  Standard_Integer nbIntervalsOther = aAdaptorCurveOther.NbIntervals(GeomAbs_C1);
	  TColStd_Array1OfReal arrInterOther(1,1+nbIntervalsOther);
	  aAdaptorCurveOther.Intervals(arrInterOther, GeomAbs_C1);
	  
	  Standard_Real Dstmin, Dst;
	  gp_Pnt P1,Pt;
	  gp_Pnt2d PUV;
	  Standard_Boolean notavertex, inserer;
	  Standard_Integer i, ii;  
	  BRepClass_FaceClassifier classifier;
	  Standard_Real aParameter;
	  for (i=1; i<=arrInter.Length(); i++)
	    {	
	      gp_Pnt aPnt = aAdaptorCurve.Value(arrInter(i));
	      
	      TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(aPnt);
	      
	      aParameter = arrInter(i);
	      BRepExtrema_ExtPC Ext(V1,Eother);
	      if ( (Ext.IsDone()) && (Ext.NbExt() > 0) )
		{ 
		  Standard_Integer NbExtrema=Ext.NbExt();
		  
		  //       Recherche de la distance minimum dstmin
		  Dstmin=sqrt (Ext.SquareDistance(1));
		  for (ii=2;ii<=NbExtrema;ii++)
		    {
		      Dst=sqrt (Ext.SquareDistance(ii));
		      if (Dst<Dstmin) 
			Dstmin=Dst;
		    }

		  if ((Dstmin < DstRef -Eps) || (fabs(Dstmin-DstRef)<Eps))
		    for (ii=1;ii<=NbExtrema;ii++)
		      { if (fabs(Dstmin-sqrt(Ext.SquareDistance(ii)))<Eps)
			  {Pt=Ext.Point(ii);
			   TRI_SOLUTION(SeqSol2,Pt,inserer);
			   if (inserer) 
			     {
			       // on regarde si le parametre ne correspondont pas a un vertex
			       Standard_Real t =Ext.Parameter(ii) ;
			       notavertex=Standard_True;
			       BRep_Tool::Range(Eother,Udeb,Ufin);
			       if ((fabs(t-Udeb)<epsP)||(fabs(t-Ufin)<=epsP )) notavertex=Standard_False;
			       if (notavertex)
				 {
				   //  Modified by Sergey KHROMOV - Tue Mar  6 12:28:51 2001 Begin
				   if (mDstRef > Dstmin)
				     mDstRef=Dstmin;
				   //  Modified by Sergey KHROMOV - Tue Mar  6 12:28:54 2001 End
				   //myModif=Standard_True;
				   P1=BRep_Tool::Pnt(V1);
				   BRepExtrema_SolutionElem Sol1 (Dstmin,P1,BRepExtrema_IsOnEdge,E, aParameter);
				   BRepExtrema_SolutionElem Sol2 (Dstmin,Pt,BRepExtrema_IsOnEdge,Eother,t);
                                   SeqSol1.Append(iE == 0 ? Sol1 : Sol2);
                                   SeqSol2.Append(iE == 0 ? Sol2 : Sol1);
				 }
			     }
			 }
		      }
		  
		  
		}
	      Standard_Real aParameterOther;
	      for (Standard_Integer i2 = 1; i2<=arrInterOther.Length(); i2++)
		{
		  gp_Pnt aPntOther = aAdaptorCurveOther.Value(arrInterOther(i2));
		  Dst=aPnt.Distance(aPntOther);
		  aParameterOther = arrInterOther(i2);
		  if( (Dst< DstRef  - Eps ) || (fabs(Dst-DstRef)< Eps ))
		    { 		
		      if (mDstRef > Dst)
			mDstRef=Dst;

		      BRepExtrema_SolutionElem Sol1 (Dst,aPnt,BRepExtrema_IsOnEdge,E, aParameter);
		      BRepExtrema_SolutionElem Sol2 (Dst,aPntOther, BRepExtrema_IsOnEdge,Eother, aParameterOther);
                      SeqSol1.Append(iE == 0 ? Sol1 : Sol2);
		      SeqSol2.Append(iE == 0 ? Sol2 : Sol1);
		    }		  

		}
	    }
	  
	  
	}
    }
}

/*********************************************************************************/

BRepExtrema_DistanceSS::BRepExtrema_DistanceSS(const TopoDS_Shape& S1, 
					       const TopoDS_Shape& S2, 
					       const Bnd_Box& B1, 
					       const Bnd_Box& B2, 
					       const Standard_Real DstRef)

{
  myEps = Precision::Confusion();
  Perform(S1, S2, B1, B2, DstRef);
}

/*********************************************************************************/

BRepExtrema_DistanceSS::BRepExtrema_DistanceSS(const TopoDS_Shape& S1, 
					       const TopoDS_Shape& S2, 
					       const Bnd_Box& B1, 
					       const Bnd_Box& B2, 
					       const Standard_Real DstRef,
					       const Standard_Real theDeflection)

{
  myEps = theDeflection;
  Perform(S1, S2, B1, B2, DstRef);
}

/*********************************************************************************/

void BRepExtrema_DistanceSS::Perform(const TopoDS_Shape& S1, 
				     const TopoDS_Shape& S2, 
				     const Bnd_Box& B1, 
				     const Bnd_Box& B2, 
				     const Standard_Real DstRef)
 
{
TopAbs_ShapeEnum       Type1, Type2;
  Type1 = S1.ShapeType();     
  Type2 = S2.ShapeType();
//  Modified by Sergey KHROMOV - Tue Mar  6 12:55:05 2001 Begin
  myDstRef = DstRef;
//  Modified by Sergey KHROMOV - Tue Mar  6 12:55:05 2001 End
  TopoDS_Face aTrimFace;
  TopoDS_Edge aTrimEdge;
  Standard_Boolean bIsInfinit;
  switch ( Type1)
   { case TopAbs_VERTEX :
		  switch (Type2)
		   { case TopAbs_VERTEX : { TopoDS_Vertex V1 = TopoDS::Vertex(S1);
			   	            TopoDS_Vertex V2 = TopoDS::Vertex(S2);
				            Perform  ( V1 , V2, B1, B2,DstRef );}
                                            break; 
  		     case TopAbs_EDGE   : { TopoDS_Vertex V1 = TopoDS::Vertex(S1);
				            TopoDS_Edge V2 = TopoDS::Edge(S2);
				             Perform  ( V1 , V2,  B1, B2,DstRef );}
                                            break; 
		     case TopAbs_FACE   : { TopoDS_Vertex V1 = TopoDS::Vertex(S1);
					    TopoDS_Face V2 = TopoDS::Face(S2);
				            Perform ( V1 , V2, B1, B2,DstRef );}
                                            break; 
		     default :{}           
		     }break;

      case TopAbs_EDGE :
		   switch (Type2)
		    { case TopAbs_VERTEX : { TopoDS_Edge V1 = TopoDS::Edge(S1);
				             TopoDS_Vertex V2 = TopoDS::Vertex(S2);
				             Perform ( V1 , V2, B1, B2,DstRef );} 
                                             break;
  		      case TopAbs_EDGE   : { TopoDS_Edge V1 = TopoDS::Edge(S1);
				             TopoDS_Edge V2 = TopoDS::Edge(S2);
					     Standard_Boolean bIsTrim1 = Standard_False;
					     Standard_Boolean bIsTrim2 = Standard_False;
					     TRIM_INFINIT_EDGE(V1, V2, aTrimEdge, bIsTrim1, bIsTrim2);
					     if (bIsTrim1)
					       V1 = aTrimEdge;
					     if (bIsTrim2)
					       V2 = aTrimEdge;
				             Perform   ( V1 , V2,  B1, B2,DstRef );}
                                             break;
		      case TopAbs_FACE   : { TopoDS_Edge V1 = TopoDS::Edge(S1);
					     TopoDS_Face V2 = TopoDS::Face(S2);					    
					     TRIM_INFINIT_FACE(V1, V2, aTrimFace, bIsInfinit);
					     if (bIsInfinit)
					       V2 = aTrimFace;
				              Perform ( V1 , V2, B1, B2,DstRef  );}
                                              break;
		      default :{}    }break;

      case TopAbs_FACE :
		    switch (Type2)
		      { case TopAbs_VERTEX : { TopoDS_Face V1 = TopoDS::Face(S1);
					       TopoDS_Vertex V2 = TopoDS::Vertex(S2);
					       Perform ( V1 , V2,  B1, B2,DstRef);}
                                               break;
  		        case TopAbs_EDGE   : { TopoDS_Face V1 = TopoDS::Face(S1);
					       TopoDS_Edge V2 = TopoDS::Edge(S2);
					       TRIM_INFINIT_FACE(V1, V2, aTrimFace, bIsInfinit);
					       if (bIsInfinit)
						 V1 = aTrimFace;
					       Perform ( V1 , V2,  B1, B2,DstRef );}
                                                break;
		        case TopAbs_FACE   : { TopoDS_Face V1 = TopoDS::Face(S1);
					       TopoDS_Face V2 = TopoDS::Face(S2);
						Perform( V1 , V2,   B1, B2,DstRef);}
                                               break;
		        default :{}
		        }break;
     default :{}
 }
}

/*********************************************************************************/

void BRepExtrema_DistanceSS::Perform(const TopoDS_Vertex& S1, const TopoDS_Vertex& S2, 
				      const Bnd_Box& ,
//                                      const Bnd_Box& B2, 
                                      const Bnd_Box& , 
				      const Standard_Real DstRef)
{     
   gp_Pnt P1,P2;
   Standard_Real Dst;
   P1= BRep_Tool::Pnt(S1);
   P2= BRep_Tool::Pnt(S2); 
   Dst=P1.Distance(P2);  
   SeqSolShape1.Clear();
   SeqSolShape2.Clear();
   myModif=Standard_False;
   
   if( (Dst< DstRef  -myEps ) || (fabs(Dst-DstRef)< myEps ))      
   { 
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:51 2001 Begin
     if (myDstRef > Dst)
       myDstRef=Dst;
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:54 2001 End
     myModif=Standard_True;
     BRepExtrema_SolutionElem Sol1 (Dst,P1,BRepExtrema_IsVertex,S1);
     BRepExtrema_SolutionElem Sol2 (Dst,P2, BRepExtrema_IsVertex,S2);
     SeqSolShape1.Append(Sol1);
     SeqSolShape2.Append(Sol2);
   }
}

/*********************************************************************************/

void BRepExtrema_DistanceSS::Perform(const TopoDS_Vertex& S1, const TopoDS_Edge& S2, 
				      const Bnd_Box& B1, const Bnd_Box& B2, 
				      const Standard_Real DstRef)
{ 
   Standard_Real Dst,Dstmin,epsP,Udeb,Ufin ; 
   gp_Pnt Pt,P1;
   Standard_Boolean inserer,notavertex;
   Standard_Integer i;  
   
   epsP=Precision::PConfusion();
   SeqSolShape1.Clear();
   SeqSolShape2.Clear();
   myModif=Standard_False;
 
  
   if (!BRep_Tool::Degenerated (S2)) {
   Dst=B1.Distance(B2);
   if ( (Dst< DstRef  -myEps ) || (fabs(Dst-DstRef)< myEps ))     
   { 
     BRepExtrema_ExtPC Ext(S1,S2); 
     if ( (Ext.IsDone()) && (Ext.NbExt() > 0) )
       { Standard_Integer NbExtrema=Ext.NbExt();
//       Recherche de la distance minimum dstmin
         Dstmin=sqrt(Ext.SquareDistance(1));
         for (i=2;i<=NbExtrema;i++)
	   {Dst=sqrt(Ext.SquareDistance(i));
            if (Dst<Dstmin) 
               Dstmin=Dst;
	    }
         if ((Dstmin < DstRef -myEps) || (fabs(Dstmin-DstRef)<myEps))
         for (i=1;i<=NbExtrema;i++)
	   { if (fabs(Dstmin-sqrt(Ext.SquareDistance(i)))<myEps)
		     {Pt=Ext.Point(i);
                      TRI_SOLUTION(SeqSolShape2,Pt,inserer);
                      if (inserer) 
			 {
                          // on regarde si le parametre ne correspondont pas a un vertex
                          Standard_Real t =Ext.Parameter(i) ;
                          notavertex=Standard_True;
                          BRep_Tool::Range(S2,Udeb,Ufin);
                          if ((fabs(t-Udeb)<epsP)||(fabs(t-Ufin)<=epsP )) notavertex=Standard_False;
                          if (notavertex)
                          {
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:51 2001 Begin
			   if (myDstRef > Dstmin)
			     myDstRef=Dstmin;
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:54 2001 End
                           myModif=Standard_True;
                           P1=BRep_Tool::Pnt(S1);
                           BRepExtrema_SolutionElem Sol1 (Dstmin,P1,BRepExtrema_IsVertex,S1);
                           BRepExtrema_SolutionElem Sol2 (Dstmin,Pt,BRepExtrema_IsOnEdge,S2,t);
                           SeqSolShape1.Append(Sol1);
                           SeqSolShape2.Append(Sol2);		  
                          }
                        }
                    }
	   }
   }
   }
}
}

/*********************************************************************************/

void BRepExtrema_DistanceSS::Perform(const TopoDS_Vertex& S1, const TopoDS_Face& S2, 
				      const Bnd_Box& B1, const Bnd_Box& B2,
				      const Standard_Real DstRef)
{
   Standard_Real Dst,Dstmin,epsP,U,V ; 
   gp_Pnt P1,Pt;
   gp_Pnt2d PUV;
   Standard_Boolean inserer,notavertex;
   Standard_Integer i;  
   BRepClass_FaceClassifier classifier; 
   
   epsP=Precision::PConfusion();
   SeqSolShape1.Clear();
   SeqSolShape2.Clear();
   myModif=Standard_False;
 

   Dst=B1.Distance(B2);
   if ( (Dst< DstRef  -myEps ) || (fabs(Dst-DstRef)< myEps ))     
   { 
     BRepExtrema_ExtPF Ext(S1,S2);
     if ( (Ext.IsDone()) && (Ext.NbExt() > 0) )
       { Standard_Integer NbExtrema=Ext.NbExt();
//       Recherche de la distance minimum dstmin
         Dstmin=sqrt(Ext.SquareDistance(1));
         for (i=2;i<=NbExtrema;i++)
	   {Dst=sqrt (Ext.SquareDistance(i));
            if (Dst<Dstmin) 
               Dstmin=Dst;
	    }
         if ((Dstmin < DstRef -myEps) || (fabs(Dstmin-DstRef)<myEps))
         for (i=1;i<=NbExtrema;i++)
	   { if (fabs(Dstmin-sqrt(Ext.SquareDistance(i)))<myEps)
		     { Pt=Ext.Point(i);
                      TRI_SOLUTION(SeqSolShape2,Pt,inserer);
                      if (inserer) 
			 {
                          // on regarde si le parametre ne correspondont pas a un vertex
                          Ext.Parameter(i,U,V);
  
                          PUV.SetCoord(U,V);
		          Standard_Real tol=BRep_Tool::Tolerance(S2);
                          classifier.Perform(S2,PUV,tol);
                          notavertex=(classifier.State()==TopAbs_IN); 
                          if (notavertex)
                          {
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:51 2001 Begin
			   if (myDstRef > Dstmin)
			     myDstRef=Dstmin;
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:54 2001 End
                           myModif=Standard_True;
                           P1=BRep_Tool::Pnt(S1);
                           BRepExtrema_SolutionElem Sol1 (Dstmin,P1,BRepExtrema_IsVertex,S1);
                           BRepExtrema_SolutionElem Sol2 (Dstmin,Pt,BRepExtrema_IsInFace,S2,U,V);
                           SeqSolShape1.Append(Sol1);
                           SeqSolShape2.Append(Sol2);		  
                          }
                        }
                    }
	   }
   }
   }
}

/*********************************************************************************/

void BRepExtrema_DistanceSS::Perform(const TopoDS_Edge& S1, const TopoDS_Vertex& S2, 
				      const Bnd_Box& B1, const Bnd_Box& B2, 
				      const Standard_Real DstRef)

{Standard_Real Dst,Dstmin,epsP,Udeb,Ufin ; 
   gp_Pnt Pt,P1;
   Standard_Boolean inserer,notavertex;
   Standard_Integer i;  
   
   epsP=Precision::PConfusion();
   SeqSolShape1.Clear();
   SeqSolShape2.Clear();
   myModif=Standard_False;
   
   if (!BRep_Tool::Degenerated(S1))  {
   Dst=B1.Distance(B2);
   if ( (Dst< DstRef  -myEps ) || (fabs(Dst-DstRef)< myEps ))     
   { 
     BRepExtrema_ExtPC Ext(S2,S1);
     if ( (Ext.IsDone()) && (Ext.NbExt() > 0) )
       { Standard_Integer NbExtrema=Ext.NbExt();
//       Recherche de la distance minimum dstmin
         Dstmin=sqrt(Ext.SquareDistance(1));
         for (i=2;i<=NbExtrema;i++)

	   {Dst=sqrt(Ext.SquareDistance(i));
            if (Dst<Dstmin) 
               Dstmin=Dst;
	    }
         if ((Dstmin < DstRef -myEps) || (fabs(Dstmin-DstRef)<myEps))
         for (i=1;i<=NbExtrema;i++)
	   { if (fabs(Dstmin-sqrt (Ext.SquareDistance(i)))<myEps)
		     {Pt=Ext.Point(i);
                      TRI_SOLUTION(SeqSolShape1,Pt,inserer); 
                      if (inserer) 
			 {
                          // on regarde si le parametre ne correspondont pas a un vertex
                          Standard_Real t =Ext.Parameter(i) ;
                          notavertex=Standard_True;
                          BRep_Tool::Range(S1,Udeb,Ufin);
                          if ((fabs(t-Udeb)<epsP)||(fabs(t-Ufin)<=epsP )) notavertex=Standard_False;
                          if (notavertex)
                          {
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:51 2001 Begin
			   if (myDstRef > Dstmin)
			     myDstRef=Dstmin;
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:54 2001 End
                           myModif=Standard_True;
                           P1=BRep_Tool::Pnt(S2);
                           BRepExtrema_SolutionElem Sol1 (Dstmin,Pt,BRepExtrema_IsOnEdge,S1,t);
                           BRepExtrema_SolutionElem Sol2 (Dstmin,P1,BRepExtrema_IsVertex,S2);
                           SeqSolShape1.Append(Sol1);
                           SeqSolShape2.Append(Sol2);
                          }
                        }
                    }
	   }
   }
   }
}
}

/*********************************************************************************/

void BRepExtrema_DistanceSS::Perform(const TopoDS_Edge& S1, const TopoDS_Edge& S2, 
				      const Bnd_Box& B1, const Bnd_Box& B2, 
				      const Standard_Real DstRef)

{
   Standard_Real Dst,Dstmin,epsP,Udeb,Ufin; 
   gp_Pnt Pt1,Pt2;
   Standard_Boolean insere1, insere2, notavertex;
   Standard_Integer i;   
   
   epsP=Precision::PConfusion();
   SeqSolShape1.Clear();
   SeqSolShape2.Clear();
   myModif=Standard_False;
   
   if (!BRep_Tool::Degenerated(S1)&& !BRep_Tool::Degenerated(S2))

 {

   Dst=B1.Distance(B2);
   if ( (Dst< DstRef  -myEps ) || (fabs(Dst-DstRef)< myEps ))     
   { 
     BRepExtrema_ExtCC Ext(S1,S2);
     if ( (Ext.IsDone()) && (Ext.NbExt() > 0) && (!Ext.IsParallel())) 
       { Standard_Integer NbExtrema=Ext.NbExt();
//       Recherche de la distance minimum dstmin
         Dstmin=sqrt (Ext.SquareDistance(1));
         for (i=2;i<=NbExtrema;i++)
	   {Dst=sqrt (Ext.SquareDistance(i));
            if (Dst<Dstmin) 
               Dstmin=Dst;
	  }
	 if ((Dstmin < DstRef -myEps) || (fabs(Dstmin-DstRef)<myEps))   
         for (i=1;i<=NbExtrema;i++)
	   { if (fabs(Dstmin-sqrt (Ext.SquareDistance(i)))<myEps )
		     {Pt1=Ext.PointOnE1(i);
                      Pt2=Ext.PointOnE2(i);
                      TRI_SOLUTION(SeqSolShape1,Pt1,insere1);
                      TRI_SOLUTION(SeqSolShape2,Pt2,insere2);
                      if (insere1 ||insere2 ) 
			 {
                          // on regarde si le parametre ne correspondont pas a un vertex
                          Standard_Real t1 =Ext.ParameterOnE1(i) ;
                          Standard_Real t2 =Ext.ParameterOnE2(i) ;
                          
                          notavertex=Standard_True;
                          BRep_Tool::Range(S1,Udeb,Ufin);
                          if ((fabs(t1-Udeb)<epsP)||(fabs(t1-Ufin)<=epsP )) notavertex=Standard_False;
                           
                          BRep_Tool::Range(S2,Udeb,Ufin);
                          if ((fabs(t2-Udeb)<epsP)||(fabs(t2-Ufin)<=epsP )) notavertex=Standard_False;
  
                          if (notavertex)
                          {
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:51 2001 Begin
			   if (myDstRef > Dstmin)
			     myDstRef=Dstmin;
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:54 2001 End
                           myModif=Standard_True;
                           BRepExtrema_SolutionElem Sol1 (Dstmin,Pt1,BRepExtrema_IsOnEdge,S1,t1);
                           BRepExtrema_SolutionElem Sol2 (Dstmin,Pt2,BRepExtrema_IsOnEdge,S2,t2);
                           SeqSolShape1.Append(Sol1);
                           SeqSolShape2.Append(Sol2);
                          }
                        }
                
    }
	   }
   }

     BRepExtrema_SeqOfSolution SeqSolution1;
     BRepExtrema_SeqOfSolution SeqSolution2;
//      Standard_Integer i;

     PERFORM_C0(S1, S2, SeqSolution1, SeqSolution2, DstRef, myDstRef, myEps);
     
     BRepExtrema_SeqOfSolution seqSol1;
     BRepExtrema_SeqOfSolution seqSol2;
	
     if (SeqSolution1.Length() > 0 && SeqSolution2.Length() > 0) {
       MIN_SOLUTION(SeqSolution1, SeqSolution2, myDstRef, myEps, seqSol1, seqSol2);
     }
     
     if (!seqSol1.IsEmpty() && !seqSol2.IsEmpty())
       {
	 SeqSolShape1.Append(seqSol1);
	 SeqSolShape2.Append(seqSol2);	
	 myModif = Standard_True;
       }
     
   }
 }
 }

/*********************************************************************************/

void BRepExtrema_DistanceSS::Perform(const TopoDS_Edge& S1, const TopoDS_Face& S2, 
				      const Bnd_Box& B1, const Bnd_Box& B2, 
				      const Standard_Real DstRef)
{
  Standard_Real Dst,Dstmin,epsP,Udeb,Ufin,U,V ; 
  gp_Pnt Pt1,Pt2;
  gp_Pnt2d PUV;
  Standard_Boolean insere1 ,insere2, notavertex;
  BRepClass_FaceClassifier classifier;
  Standard_Integer i;  
  
  epsP=Precision::PConfusion();
  SeqSolShape1.Clear();
  SeqSolShape2.Clear();
  myModif=Standard_False;
  
  if (!BRep_Tool::Degenerated(S1)) {
    
    Dst=B1.Distance(B2);
    if ( (Dst< DstRef  -myEps ) || (fabs(Dst-DstRef)< myEps ))     
      {
	BRepExtrema_ExtCF Ext(S1,S2);
	if ( (Ext.IsDone()) && ((Ext.NbExt() > 0) && (!Ext.IsParallel()) ))
	  { Standard_Integer NbExtrema=Ext.NbExt();
	    //       Recherche de la distance minimum dstmin
	    Dstmin=sqrt (Ext.SquareDistance(1));
	    for (i=2;i<=NbExtrema;i++)
	      {Dst=sqrt (Ext.SquareDistance(i));
	       if (Dst<Dstmin) 
		 Dstmin=Dst;
	     }
	    if ((Dstmin < DstRef -myEps) || (fabs(Dstmin-DstRef)<myEps))
	      for (i=1;i<=NbExtrema;i++)
		{ if (fabs(Dstmin-sqrt (Ext.SquareDistance(i)))<myEps)
		    {Pt1=Ext.PointOnEdge(i);
		     Pt2=Ext.PointOnFace(i);
		     TRI_SOLUTION (SeqSolShape1,Pt1,insere1);
		     TRI_SOLUTION (SeqSolShape2,Pt2,insere2); 
		     if (insere1||insere2)
		       {
			 // on regarde si le parametre ne correspondont pas a un vertex
			 Standard_Real t1 =Ext.ParameterOnEdge(i) ;
			 
			 notavertex=Standard_True;
			 BRep_Tool::Range(S1,Udeb,Ufin);
			 if ((fabs(t1-Udeb)<epsP)||(fabs(t1-Ufin)<=epsP )) notavertex=Standard_False;
			 
			 Ext.ParameterOnFace(i,U,V);
			 PUV.SetCoord(U,V);
			 Standard_Real tol=BRep_Tool::Tolerance(S2);
			 classifier.Perform(S2,PUV,tol);
			 if ( classifier.State()!=TopAbs_IN)  notavertex=Standard_False;
			 
			 if (notavertex)
			   {
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:51 2001 Begin
			     if (myDstRef > Dstmin)
			       myDstRef=Dstmin;
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:54 2001 End
			     myModif=Standard_True;
			     BRepExtrema_SolutionElem Sol1 (Dstmin,Pt1,BRepExtrema_IsOnEdge,S1,t1);
			     BRepExtrema_SolutionElem Sol2 (Dstmin,Pt2,BRepExtrema_IsInFace,S2,U,V);
			     SeqSolShape1.Append(Sol1);
			     SeqSolShape2.Append(Sol2);		  
			   }
		       }
		   }
		}
	  }
	Standard_Real aFirst, aLast;
	Handle(Geom_Curve) pCurv = BRep_Tool::Curve(S1, aFirst, aLast);
	GeomAbs_Shape s = pCurv->Continuity();
	if (s == GeomAbs_C0)
	  {
	    BRepExtrema_SeqOfSolution SeqSolution1;
	    BRepExtrema_SeqOfSolution SeqSolution2;

	    
	    GeomAdaptor_Curve aAdaptorCurve(pCurv, aFirst, aLast);
	    Standard_Integer nbIntervals = aAdaptorCurve.NbIntervals(GeomAbs_C1);
	    
	    TColStd_Array1OfReal arrInter(1,1+nbIntervals);
	    aAdaptorCurve.Intervals(arrInter, GeomAbs_C1);
	    
// 	    Standard_Real U,V;
	    gp_Pnt P1,Pt;
// 	    gp_Pnt2d PUV;
// 	    Standard_Boolean notavertex;
// 	    Standard_Integer i, ii;  
	    Standard_Integer ii;  
// 	    BRepClass_FaceClassifier classifier;
	    Standard_Real aParameter;
	    for (i=1; i<=arrInter.Length(); i++)
	      {	
		gp_Pnt aPnt = aAdaptorCurve.Value(arrInter(i));
		
		TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(aPnt);

		aParameter = arrInter(i);
		
		BRepExtrema_ExtPF ExtPF(V1,S2);
		if ( (ExtPF.IsDone()) && (ExtPF.NbExt() > 0) )
		  { 
		    Standard_Integer NbExtrema=ExtPF.NbExt();
		    //       Recherche de la distance minimum dstmin
		    Dstmin=sqrt (ExtPF.SquareDistance(1));
		    for (ii=2;ii<=NbExtrema;ii++)
		      {Dst=sqrt(ExtPF.SquareDistance(ii));
		       if (Dst<Dstmin) 
			 Dstmin=Dst;
		     }
		    if ((Dstmin < DstRef -myEps) || (fabs(Dstmin-DstRef)<myEps))
		      {
			
			for (ii=1;ii<=NbExtrema;ii++)
			  { 
			    if (fabs(Dstmin-sqrt (ExtPF.SquareDistance(ii)))<myEps)
			      { 
				Pt=ExtPF.Point(ii);
				// on regarde si le parametre ne correspondont pas a un vertex
				ExtPF.Parameter(ii,U,V);
				// Standard_Real t1 =Ext.ParameterOnEdge(ii) ;
								
				PUV.SetCoord(U,V);
				Standard_Real tol=BRep_Tool::Tolerance(S2);
				classifier.Perform(S2,PUV,tol);
				notavertex=(classifier.State()==TopAbs_IN); 
				if (notavertex)
				  {
				    //  Modified by Sergey KHROMOV - Tue Mar  6 12:28:51 2001 Begin
				    if (myDstRef > Dstmin)
				      myDstRef=Dstmin;
				    //  Modified by Sergey KHROMOV - Tue Mar  6 12:28:54 2001 End
				    myModif=Standard_True;
				    P1=BRep_Tool::Pnt(V1);
				    BRepExtrema_SolutionElem Sol1 (Dstmin,P1,BRepExtrema_IsOnEdge,S1,aParameter);
				    BRepExtrema_SolutionElem Sol2 (Dstmin,Pt,BRepExtrema_IsInFace,S2,U,V);
				    SeqSolution1.Append(Sol1);
				    SeqSolution2.Append(Sol2);
				  }
			      }
			  }
		      }
		  }
	      }
	    BRepExtrema_SeqOfSolution seqSol1;
	    BRepExtrema_SeqOfSolution seqSol2;
	    Standard_Boolean bIsMini = Standard_False;
	    if (SeqSolution1.Length() > 0 && SeqSolution2.Length() > 0)
	      MIN_SOLUTION(SeqSolution1, SeqSolution2, myDstRef, myEps, seqSol1, seqSol2);
	    
	    if (!seqSol1.IsEmpty() && !seqSol2.IsEmpty())
	      {
		SeqSolShape1.Append(seqSol1);
		SeqSolShape2.Append(seqSol2);
	      }
	  }
      }
  }
}

/*********************************************************************************/

void BRepExtrema_DistanceSS::Perform(const TopoDS_Face& S1, const TopoDS_Vertex& S2, 
				      const Bnd_Box& B1, const Bnd_Box& B2, 
				      const Standard_Real DstRef)

{
  const Standard_Real epsP=Precision::PConfusion();
  SeqSolShape1.Clear();
  SeqSolShape2.Clear();
  myModif=Standard_False;

  Standard_Real Dst=B1.Distance(B2);
  if ((Dst < DstRef -myEps) || (fabs(Dst-DstRef)< myEps))
  { 
    BRepExtrema_ExtPF Ext(S2,S1);
    if ( Ext.IsDone() && (Ext.NbExt() > 0) )
    {
      const Standard_Integer NbExtrema=Ext.NbExt();
      // Search the minimal distance Dstmin
      Standard_Integer i;
      Standard_Real Dstmin = Ext.SquareDistance(1);
      for (i=2;i<=NbExtrema;i++)
	  {
        Dst = Ext.SquareDistance(i);
        if (Dst<Dstmin)
          Dstmin=Dst;
      }
      Dstmin = sqrt(Dstmin);

      if ((Dstmin < DstRef -myEps) || (fabs(Dstmin-DstRef)<myEps))
      {
        Standard_Real U,V;
        gp_Pnt P1,Pt;
        gp_Pnt2d PUV;
        Standard_Boolean inserer;
        BRepClass_FaceClassifier classifier;

        for (i=1;i<=NbExtrema;i++)
        {
          if (fabs(Dstmin-sqrt (Ext.SquareDistance(i)))<myEps)
		  {
            Pt=Ext.Point(i);
            TRI_SOLUTION (SeqSolShape1,Pt,inserer);
            if (inserer)
			{
              // on regarde si le parametre ne correspondont pas a un vertex
              Ext.Parameter(i,U,V);

              PUV.SetCoord(U,V);
		      const Standard_Real tol=BRep_Tool::Tolerance(S1);
              classifier.Perform(S1,PUV,tol);
              if (classifier.State()==TopAbs_IN)
              {
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:51 2001 Begin
			    if (myDstRef > Dstmin)
			      myDstRef=Dstmin;
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:54 2001 End
                myModif=Standard_True;
                P1=BRep_Tool::Pnt(S2);
                BRepExtrema_SolutionElem Sol1 (Dstmin,Pt,BRepExtrema_IsInFace,S1,U,V);
                BRepExtrema_SolutionElem Sol2 (Dstmin,P1,BRepExtrema_IsVertex,S2);
                SeqSolShape1.Append(Sol1);
                SeqSolShape2.Append(Sol2);
              }
            }
          }
        }
	  }
    }
  }
}

/*********************************************************************************/

void BRepExtrema_DistanceSS::Perform(const TopoDS_Face& S1, const TopoDS_Edge& S2, 
				      const Bnd_Box& B1, const Bnd_Box& B2, 
				      const Standard_Real DstRef)

{
   Standard_Real Dst,Dstmin,epsP,Udeb,Ufin,U,V ; 
   gp_Pnt Pt1,Pt2;
   gp_Pnt2d PUV;
   Standard_Boolean insere1,insere2,notavertex;
   BRepClass_FaceClassifier classifier;
   Standard_Integer i;  
   
   epsP=Precision::PConfusion();
   SeqSolShape1.Clear();
   SeqSolShape2.Clear();
   myModif=Standard_False;
   
   if (!BRep_Tool::Degenerated(S2)) 
  {
   Dst=B1.Distance(B2);
   if ( (Dst< DstRef  -myEps ) || (fabs(Dst-DstRef)< myEps ))     
   { 
     BRepExtrema_ExtCF Ext(S2,S1);
     if ( (Ext.IsDone()) && ((Ext.NbExt() > 0) && (!Ext.IsParallel())) )
         { Standard_Integer NbExtrema=Ext.NbExt();
//       Recherche de la distance minimum dstmin
         Dstmin=sqrt (Ext.SquareDistance(1));
         for (i=2;i<=NbExtrema;i++)
	   {Dst=sqrt (Ext.SquareDistance(i));
            if (Dst<Dstmin) 
               Dstmin=Dst;}
	  if ((Dstmin < DstRef -myEps) || (fabs(Dstmin-DstRef)<myEps))   
         for (i=1;i<=NbExtrema;i++)
	   { if (fabs(Dstmin-sqrt (Ext.SquareDistance(i)))<myEps)
		     {Pt1=Ext.PointOnFace(i);
                      Pt2=Ext.PointOnEdge(i);
                      TRI_SOLUTION (SeqSolShape1,Pt1,insere1);
                      TRI_SOLUTION (SeqSolShape2,Pt2,insere2); 
                      if (insere1||insere2)
          
			 {
                          // on regarde si le parametre ne correspondont pas a un vertex
                          Standard_Real t1 =Ext.ParameterOnEdge(i) ;
                          
                          notavertex=Standard_True;
                          BRep_Tool::Range(S2,Udeb,Ufin);
                          if ((fabs(t1-Udeb)<epsP)||(fabs(t1-Ufin)<=epsP )) notavertex=Standard_False;
                    
                          Ext.ParameterOnFace(i,U,V);
                          PUV.SetCoord(U,V);
		          Standard_Real tol=BRep_Tool::Tolerance(S1);
                          classifier.Perform(S1,PUV,tol);
                          if (classifier.State()!=TopAbs_IN) notavertex=Standard_False; 
                
                          if (notavertex)
                          {
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:51 2001 Begin
			   if (myDstRef > Dstmin)
			     myDstRef=Dstmin;
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:54 2001 End
                           myModif=Standard_True;
                           BRepExtrema_SolutionElem Sol1 (Dstmin,Pt1,BRepExtrema_IsInFace,S1,U,V);
                           BRepExtrema_SolutionElem Sol2 (Dstmin,Pt2,BRepExtrema_IsOnEdge,S2,t1);
                           SeqSolShape1.Append(Sol1);
                           SeqSolShape2.Append(Sol2);		  
                          }
                        }
                    }
	   }
	 }

     Standard_Real aFirst, aLast;
     Handle(Geom_Curve) pCurv = BRep_Tool::Curve(S2, aFirst, aLast);
     GeomAbs_Shape s = pCurv->Continuity();
     if (s == GeomAbs_C0)
       {
	 BRepExtrema_SeqOfSolution SeqSolution1;
	 BRepExtrema_SeqOfSolution SeqSolution2;


	 GeomAdaptor_Curve aAdaptorCurve(pCurv, aFirst, aLast);
	 Standard_Integer nbIntervals = aAdaptorCurve.NbIntervals(GeomAbs_C1);
	 
	 TColStd_Array1OfReal arrInter(1,1+nbIntervals);
	 aAdaptorCurve.Intervals(arrInter, GeomAbs_C1);
	 
// 	 Standard_Real U,V;
	 gp_Pnt P1,Pt;
// 	 gp_Pnt2d PUV;
// 	 Standard_Boolean notavertex;
	 Standard_Integer ii;  
//	 BRepClass_FaceClassifier classifier;
	 Standard_Real aParameter;
	 for (i=1; i<=arrInter.Length(); i++)
	   {	
	     gp_Pnt aPnt = aAdaptorCurve.Value(arrInter(i));
	     
	     TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(aPnt);
	     
	     aParameter = arrInter(i);
	     
	     BRepExtrema_ExtPF ExtPF(V1,S1);
	     if ( (ExtPF.IsDone()) && (ExtPF.NbExt() > 0) )
	       { 
		 Standard_Integer NbExtrema=ExtPF.NbExt();
		 //       Recherche de la distance minimum dstmin
		 Dstmin=sqrt(ExtPF.SquareDistance(1));
		 for (ii=2;ii<=NbExtrema;ii++)
		   {Dst=sqrt(ExtPF.SquareDistance(ii));
		    if (Dst<Dstmin) 
		      Dstmin=Dst;
		  }
		 if ((Dstmin < DstRef -myEps) || (fabs(Dstmin-DstRef)<myEps))
		   {
		     
		     for (ii=1;ii<=NbExtrema;ii++)
		       { 
			 if (fabs(Dstmin-sqrt(ExtPF.SquareDistance(ii)))<myEps)
			   { 
			     Pt=ExtPF.Point(ii);
			     // on regarde si le parametre ne correspondont pas a un vertex
			     ExtPF.Parameter(ii,U,V);
			     // Standard_Real t1 =Ext.ParameterOnEdge(ii) ;
			     
			     PUV.SetCoord(U,V);
			     Standard_Real tol=BRep_Tool::Tolerance(S1);
			     classifier.Perform(S1,PUV,tol);
			     notavertex=(classifier.State()==TopAbs_IN); 
			     if (notavertex)
			       {
				 //  Modified by Sergey KHROMOV - Tue Mar  6 12:28:51 2001 Begin
				 if (myDstRef > Dstmin)
				   myDstRef=Dstmin;
				 //  Modified by Sergey KHROMOV - Tue Mar  6 12:28:54 2001 End
				 myModif=Standard_True;
				 P1=BRep_Tool::Pnt(V1);				 
				 BRepExtrema_SolutionElem Sol1 (Dstmin,Pt,BRepExtrema_IsInFace,S1,U,V);
				 BRepExtrema_SolutionElem Sol2 (Dstmin,P1,BRepExtrema_IsOnEdge,S2,aParameter);
				 SeqSolution1.Append(Sol1);
				 SeqSolution2.Append(Sol2);				 
			       }
			   }
		       }
		   }
	       }
	   }
	 BRepExtrema_SeqOfSolution seqSol1;
	 BRepExtrema_SeqOfSolution seqSol2;
	 Standard_Boolean bIsMini = Standard_False;
	 if (SeqSolution1.Length() > 0 && SeqSolution2.Length() > 0)
	   MIN_SOLUTION(SeqSolution1, SeqSolution2, myDstRef, myEps, seqSol1, seqSol2);
	 
	 if (!seqSol1.IsEmpty() && !seqSol2.IsEmpty())
	   {
	     SeqSolShape1.Append(seqSol1);
	     SeqSolShape2.Append(seqSol2);
	   }
       }
     
   }
 }
 }

/*********************************************************************************/

void BRepExtrema_DistanceSS::Perform(const TopoDS_Face& S1, const TopoDS_Face& S2, 
				      const Bnd_Box& B1, const Bnd_Box& B2, 
				      const Standard_Real DstRef)

{
   Standard_Real Dst,Dstmin,epsP,U1,V1,U2,V2,tol ; 
   gp_Pnt Pt1,Pt2;
   gp_Pnt2d PUV;
   Standard_Boolean insere1,insere2,notavertex;
   BRepClass_FaceClassifier classifier;
   Standard_Integer i;  
   
   epsP=Precision::PConfusion();
   SeqSolShape1.Clear();
   SeqSolShape2.Clear();
   myModif=Standard_False;
 
   Dst=B1.Distance(B2);
   if ( (Dst< DstRef  -myEps ) || (fabs(Dst-DstRef)< myEps ))     
   { 
     BRepExtrema_ExtFF Ext(S1,S2);
     if ( (Ext.IsDone()) && ((Ext.NbExt() > 0) && (!Ext.IsParallel()) ))
       { Standard_Integer NbExtrema=Ext.NbExt();
//       Recherche de la distance minimum dstmin
         Dstmin=sqrt (Ext.SquareDistance(1));
         for (i=2;i<=NbExtrema;i++)
	   {Dst=sqrt(Ext.SquareDistance(i));
            if (Dst<Dstmin) 
               Dstmin=Dst;
	    }
         if ((Dstmin < DstRef -myEps) || (fabs(Dstmin-DstRef)<myEps))
         for (i=1;i<=NbExtrema;i++)
	   { if (fabs(Dstmin-sqrt (Ext.SquareDistance(i)))<myEps)
		     {Pt1=Ext.PointOnFace1(i);
                      Pt2=Ext.PointOnFace2(i); 
                      TRI_SOLUTION (SeqSolShape1,Pt1,insere1);
                      TRI_SOLUTION (SeqSolShape2,Pt2,insere2) ;                             
                         if (insere1||insere2)
			 {
                          // on regarde si le parametre ne correspondont pas a un vertex
              

                    
                          Ext.ParameterOnFace1(i,U1,V1);
                          PUV.SetCoord(U1,V1);
		          tol=BRep_Tool::Tolerance(S1);
                          classifier.Perform(S1,PUV,tol);
                          notavertex=(classifier.State()==TopAbs_IN);
  
                          Ext.ParameterOnFace2(i,U2,V2);
                          PUV.SetCoord(U2,V2);
		          tol=BRep_Tool::Tolerance(S2);
                          classifier.Perform(S2,PUV,tol);
                          if (classifier.State()!=TopAbs_IN) notavertex= Standard_False;
                          
                          if (notavertex)
                          {
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:51 2001 Begin
			   if (myDstRef > Dstmin)
			     myDstRef=Dstmin;
//  Modified by Sergey KHROMOV - Tue Mar  6 12:28:54 2001 End
                           myModif=Standard_True;
                           BRepExtrema_SolutionElem Sol1 (Dstmin,Pt1,BRepExtrema_IsInFace,S1,U1,V1);
                           BRepExtrema_SolutionElem Sol2 (Dstmin,Pt2,BRepExtrema_IsInFace,S2,U2,V2);
                           SeqSolShape1.Append(Sol1);
                           SeqSolShape2.Append(Sol2);		  
                          }
                        }
                    }
	   }
   }
   }
}

/*********************************************************************************/

Standard_Boolean BRepExtrema_DistanceSS::IsDone() const 
{
  return myModif;
}

/*********************************************************************************/

Standard_Real BRepExtrema_DistanceSS::DistValue() const 
{
 return myDstRef;
}

/*********************************************************************************/

const BRepExtrema_SeqOfSolution& BRepExtrema_DistanceSS::Seq1Value() const 
{
 return  SeqSolShape1;
}

/*********************************************************************************/

const BRepExtrema_SeqOfSolution& BRepExtrema_DistanceSS::Seq2Value() const 
{ 
return  SeqSolShape2; 
}

/*********************************************************************************/
/*********************************************************************************/
