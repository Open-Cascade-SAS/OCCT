// Created on: 1993-11-18
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Bnd_Box.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_LineInter.hxx>

#ifdef DRAW
#include <TopOpeBRep_DRAW.hxx>
#endif

#include <IntPatch_LineConstructor.hxx>
#include <TopOpeBRep_TypeLineCurve.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <Precision.hxx>
#include <Geom_Curve.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_CString.hxx>
#include <BRepTools.hxx>
#include <TopOpeBRepTool_tol.hxx>

Standard_EXPORT Standard_Real GLOBAL_tolFF = 1.e-7;

#ifdef OCCT_DEBUG
#include <TopAbs.hxx>
extern Standard_Boolean TopOpeBRep_GettraceFI();
extern Standard_Boolean TopOpeBRep_GettraceFITOL();
extern Standard_Boolean TopOpeBRep_GettraceSAVFF();

Standard_Integer SAVFFi1 = 0;
Standard_Integer SAVFFi2 = 0;
static void SAVFF(const TopoDS_Face& F1,const TopoDS_Face& F2)
{
  TCollection_AsciiString an1("SAVA");if (SAVFFi1) an1=an1+SAVFFi1;
  TCollection_AsciiString an2("SAVB");if (SAVFFi2) an2=an2+SAVFFi2;
  Standard_CString n1=an1.ToCString();Standard_CString n2=an2.ToCString();
#ifdef DRAW
  cout<<"FaceIntersector :   set "<<n1<<","<<n2<<endl;DBRep::Set(n1,F1);DBRep::Set(n2,F2);
#endif
  cout<<"FaceIntersector : write "<<n1<<","<<n2<<endl;BRepTools::Write(F1,n1);BRepTools::Write(F2,n2); 
}

extern Standard_Boolean TopOpeBRepTool_GettraceKRO();
#include <TopOpeBRepTool_KRO.hxx>
Standard_EXPORT TOPKRO KRO_DSFILLER_INTFF("intersection face/face");

#endif

// NYI
// NYI : IntPatch_Intersection : TolArc,TolTang exact definition
// NYI


// modified by NIZHNY-MKK  Mon Apr  2 12:14:32 2001.BEGIN
#include <IntPatch_WLine.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_Point.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <Geom2dInt_TheProjPCurOfGInter.hxx>

static Standard_Boolean TestWLineAlongRestriction(const Handle(IntPatch_WLine)& theWLine,
						  const Standard_Integer                         theRank,
						  const Handle(Adaptor3d_HSurface)&              theSurface,
						  const Handle(Adaptor3d_TopolTool)&             theDomain,
						  const Standard_Real                            theTolArc);

static 
Handle(IntPatch_RLine) BuildRLineBasedOnWLine(const Handle(IntPatch_WLine)& theWLine,
							       const Handle(Adaptor2d_HCurve2d)&              theArc,
							       const Standard_Integer                         theRank);

static
Handle(IntPatch_RLine) BuildRLine(const IntPatch_SequenceOfLine&     theSeqOfWLine,
						   const Standard_Integer             theRank,
						   const Handle(Adaptor3d_HSurface)&  theSurface,
						   const Handle(Adaptor3d_TopolTool)& theDomain,
						   const Standard_Real                theTolArc);

static void TestWLinesToAnArc(IntPatch_SequenceOfLine&           slin,
			      const Handle(Adaptor3d_HSurface)&  theSurface1,
			      const Handle(Adaptor3d_TopolTool)& theDomain1,
			      const Handle(Adaptor3d_HSurface)&  theSurface2,
			      const Handle(Adaptor3d_TopolTool)& theDomain2,
			      const Standard_Real                theTolArc);
// modified by NIZHNY-MKK  Mon Apr  2 12:14:38 2001.END

// modified by NIZHNY-OFV  Fri Mar 29 12:37:21 2002.BEGIN
#include <TColgp_SequenceOfPnt.hxx>
#include <TopExp.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_POnSurf.hxx>
#include <GeomAdaptor_Curve.hxx>
static void MergeWLinesIfAllSegmentsAlongRestriction(IntPatch_SequenceOfLine&           theSlin,
						     const Handle(Adaptor3d_HSurface)&  theSurface1,
						     const Handle(Adaptor3d_TopolTool)& theDomain1,
						     const Handle(Adaptor3d_HSurface)&  theSurface2,
						     const Handle(Adaptor3d_TopolTool)& theDomain2,
						     const Standard_Real                theTolArc);
//------------------------------------------------------------------------------------------------
static Standard_Integer GetArc(IntPatch_SequenceOfLine&           theSlin,
			       const Standard_Integer&            theRankS,
			       const Handle(Adaptor3d_HSurface)&  theSurfaceObj,
			       const Handle(Adaptor3d_TopolTool)& theDomainObj,
			       const Handle(Adaptor3d_HSurface)&  theSurfaceTool,
			       const gp_Pnt&                      theTestPoint,
			       Standard_Real&                     theVrtxTol,
			       Handle(IntSurf_LineOn2S)&          theLineOn2S,
			       Standard_Real&                     theFirst,
			       Standard_Real&                     theLast);
//------------------------------------------------------------------------------------------------
static Standard_Boolean IsPointOK(const gp_Pnt&            theTestPnt,
				  const Adaptor3d_Surface& theTestSurface,
				  const Standard_Real&     theTol);
//-------------------------------------------------------------------------------------------------
static Standard_Boolean GetPointOn2S(const gp_Pnt&            theTestPnt,
				     const Adaptor3d_Surface& theTestSurface,
				     const Standard_Real&     theTol,
				     Extrema_POnSurf&         theResultPoint);
//-------------------------------------------------------------------------------------------------------------------------
static Handle(IntPatch_WLine) GetMergedWLineOnRestriction(IntPatch_SequenceOfLine&           theSlin,
									   const Standard_Real&               theVrtxTol,
									   const Handle(IntSurf_LineOn2S)&    theLineOn2S);
//---------------------------------------------------------------------------------------------------------------------------
// modified by NIZHNY-OFV  Fri Mar 29 12:41:02 2002.END

//=======================================================================
//function : TopOpeBRep_FacesIntersector
//purpose  : 
//=======================================================================
TopOpeBRep_FacesIntersector::TopOpeBRep_FacesIntersector ()
{
  ResetIntersection();
  myTol1 = myTol2 = Precision::Confusion();
  myForceTolerances = Standard_False;
  mySurface1 = new BRepAdaptor_HSurface();
  mySurface2 = new BRepAdaptor_HSurface();
  myDomain1 = new BRepTopAdaptor_TopolTool();
  myDomain2 = new BRepTopAdaptor_TopolTool();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesIntersector::Perform(const TopoDS_Shape& F1,const TopoDS_Shape& F2,
					  const Bnd_Box& B1,const Bnd_Box& B2)
{
#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceSAVFF()) SAVFF(TopoDS::Face(F1),TopoDS::Face(F2));
#endif
  
  ResetIntersection();
  if (!myForceTolerances) ShapeTolerances(F1,F2);
  
  myFace1 = TopoDS::Face(F1); myFace1.Orientation(TopAbs_FORWARD);
  myFace2 = TopoDS::Face(F2); myFace2.Orientation(TopAbs_FORWARD);
  BRepAdaptor_Surface& S1 = mySurface1->ChangeSurface(); S1.Initialize(myFace1);
  BRepAdaptor_Surface& S2 = mySurface2->ChangeSurface(); S2.Initialize(myFace2);
  mySurfaceType1 = S1.GetType();
  mySurfaceType2 = S2.GetType();
  const Handle(Adaptor3d_HSurface)& aSurf1 = mySurface1; // to avoid ambiguity
  myDomain1->Initialize(aSurf1);
  const Handle(Adaptor3d_HSurface)& aSurf2 = mySurface2; // to avoid ambiguity
  myDomain2->Initialize(aSurf2);

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceKRO()) KRO_DSFILLER_INTFF.Start();
#endif

  Standard_Real Deflection=0.01,MaxUV=0.01;
  if (!myForceTolerances) {
    FTOL_FaceTolerances3d(B1,B2,myFace1,myFace2,S1,S2,
				myTol1,myTol2,Deflection,MaxUV);  
    myTol1 = (myTol1 > 1.e-4)? 1.e-4: myTol1;
    myTol2 = (myTol2 > 1.e-4)? 1.e-4: myTol2;
  }
  
  Standard_Real tol1 = myTol1;
  Standard_Real tol2 = myTol2;
  GLOBAL_tolFF = Max(tol1,tol2);

#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceFITOL()) {
    cout<<"FacesIntersector : Perform tol1 = "<<tol1<<endl;
    cout<<"                           tol2 = "<<tol2<<endl;
    cout<<"                           defl = "<<Deflection<<"  MaxUV = "<<MaxUV<<endl;
  }
#endif

  myIntersector.SetTolerances(myTol1,myTol2,MaxUV,Deflection); 
  myIntersector.Perform(mySurface1,myDomain1,mySurface2,myDomain2,
                        myTol1,myTol2,Standard_True,Standard_True,
                        Standard_False);

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceKRO()) KRO_DSFILLER_INTFF.Stop();
#endif

  //xpu180998 : cto900Q1
  Standard_Boolean done = myIntersector.IsDone();
  if (!done) return;

  PrepareLines();
  myIntersectionDone = Standard_True;

  // mySurfacesSameOriented : a mettre dans IntPatch NYI
  if ( SameDomain() ) {
    mySurfacesSameOriented = TopOpeBRepTool_ShapeTool::SurfacesSameOriented(S1,S2);
  }

  // build the map of edges found as RESTRICTION
  for (InitLine(); MoreLine(); NextLine()) {
    TopOpeBRep_LineInter& L = CurrentLine();
    if (L.TypeLineCurve() == TopOpeBRep_RESTRICTION) {
      const TopoDS_Shape& E = L.Arc();
      myEdgeRestrictionMap.Add(E);
    }
  }
  
#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceFI()) cout<<"Perform : isempty "<<IsEmpty()<<endl;
#endif
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void TopOpeBRep_FacesIntersector::Perform(const TopoDS_Shape& F1,const TopoDS_Shape& F2)
{
  Bnd_Box B1,B2;
  Perform(F1,F2,B1,B2);
}


//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_FacesIntersector::IsEmpty () 
{
  if ( ! myIntersectionDone ) return Standard_False;
  Standard_Boolean done  = myIntersector.IsDone();
  Standard_Boolean empty = myIntersector.IsEmpty();
  if ( !done || empty ) return Standard_True;
  else {
    // ElemIntersector is done and is not empty
    // returns True if at least one VPoint is found
    empty = Standard_True;
    for ( InitLine(); MoreLine(); NextLine() ) {
      empty = (CurrentLine().NbVPoint() == 0);
      if ( ! empty ) break;
    }
    return empty;
  }
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_FacesIntersector::IsDone () const
{
  return myIntersectionDone;
}

//=======================================================================
//function : SameDomain
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_FacesIntersector::SameDomain () const 
{
  if (!myIntersectionDone) 
    throw Standard_ProgramError("FacesIntersector : bad SameDomain");

  Standard_Boolean sd = myIntersector.TangentFaces();

  //Standard_Boolean plpl = (mySurfaceType1 == GeomAbs_Plane) && (mySurfaceType2 == GeomAbs_Plane);

//  if (!plpl) return Standard_False;
  return sd;
}


//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRep_FacesIntersector::Face
(const Standard_Integer Index) const 
{
  if      ( Index == 1 ) return myFace1;
  else if ( Index == 2 ) return myFace2;
  else throw Standard_ProgramError("TopOpeBRep_FacesIntersector::Face");
}


//=======================================================================
//function : SurfacesSameOriented
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_FacesIntersector::SurfacesSameOriented () const
{
  if ( SameDomain() ) {
    return mySurfacesSameOriented;
  }
  throw Standard_ProgramError("FacesIntersector : bad SurfacesSameOriented");
}

//=======================================================================
//function : IsRestriction
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_FacesIntersector::IsRestriction
   (const TopoDS_Shape& E) const
{
  Standard_Boolean isrest = myEdgeRestrictionMap.Contains(E);
  return isrest;
}

//=======================================================================
//function : Restrictions
//purpose  : 
//=======================================================================

const TopTools_IndexedMapOfShape& TopOpeBRep_FacesIntersector::Restrictions 
   () const 
{
  return myEdgeRestrictionMap;
}

//=======================================================================
//function : PrepareLines
//purpose  : 
//=======================================================================

void  TopOpeBRep_FacesIntersector::PrepareLines()
{
  myLineNb = 0;
  Standard_Integer n = myIntersector.NbLines();
  myHAL = new TopOpeBRep_HArray1OfLineInter(0,n); 
  BRepAdaptor_Surface& S1 = *((BRepAdaptor_Surface*)&(mySurface1->Surface()));
  BRepAdaptor_Surface& S2 = *((BRepAdaptor_Surface*)&(mySurface2->Surface()));

  // modified by NIZHNY-MKK  Mon Apr  2 12:14:58 2001.BEGIN
  if(n==0)
    return;
  // modified by NIZHNY-MKK  Mon Apr  2 12:15:09 2001.END

  Standard_Boolean newV = Standard_True;

  if (!newV) {
  /*for (  Standard_Integer i=1; i<=n; i++) {
    TopOpeBRep_LineInter& LI = myHAL->ChangeValue(i);
    const Handle(IntPatch_Line)& L = myIntersector.Line(i);
    LI.SetLine(L,S1,S2);
    LI.Index(i);
    myLineNb++;;
  }*/}

  if (newV) {
    //-- lbr

    // modified by NIZHNY-MKK  Mon Apr  2 12:16:04 2001
    IntPatch_SequenceOfLine aSeqOfLines, aSeqOfResultLines;

    Standard_Integer i ;
//    Standard_Integer nbl=0;
    IntPatch_LineConstructor **Ptr = 
      (IntPatch_LineConstructor **)malloc(n*sizeof(IntPatch_LineConstructor *));
    for( i=1;i<=n;i++) { 
      Ptr[i-1]=new IntPatch_LineConstructor(2);
      Ptr[i-1]->Perform(myIntersector.SequenceOfLine(),
			myIntersector.Line(i),
			mySurface1,myDomain1,
			mySurface2,myDomain2,
			myTol1);
      // modified by NIZHNY-MKK  Mon Apr  2 12:16:26 2001.BEGIN
      aSeqOfLines.Clear();
      for(Standard_Integer k=1; k<=Ptr[i-1]->NbLines(); k++) {
	aSeqOfLines.Append(Ptr[i-1]->Line(k));
      }

      TestWLinesToAnArc(aSeqOfLines, mySurface1, myDomain1, mySurface2, myDomain2, myTol1);

      for(Standard_Integer j=1; j<=aSeqOfLines.Length(); j++) {
	aSeqOfResultLines.Append(aSeqOfLines.Value(j));
      }
      delete Ptr[i-1];      
      //       nbl+=Ptr[i-1]->NbLines();
      // modified by NIZHNY-MKK  Mon Apr  2 12:16:31 2001.END
    }

    // modified by NIZHNY-MKK  Mon Apr  2 12:17:22 2001.BEGIN
    //     myHAL = new TopOpeBRep_HArray1OfLineInter(0,nbl); 
    myLineNb = aSeqOfResultLines.Length();
 
    //Fun_ConvertWLinesToRLine(aSeqOfResultLines,mySurface1, myDomain1, mySurface2, myDomain2, myTol1);
    MergeWLinesIfAllSegmentsAlongRestriction(aSeqOfResultLines,mySurface1, myDomain1, mySurface2, myDomain2, myTol1);
    myLineNb = aSeqOfResultLines.Length();


    if(myLineNb > 0) {
      myHAL = new TopOpeBRep_HArray1OfLineInter(1, myLineNb); 
      for(Standard_Integer index = 1; index <= myLineNb; index++) {
	TopOpeBRep_LineInter& LI = myHAL->ChangeValue(index);
	const Handle(IntPatch_Line)& L = aSeqOfResultLines.Value(index);
	LI.SetLine(L,S1,S2);
	LI.Index(index);
      }
    }
    
    //     nbl=1;
    //     for(i=1;i<=n;i++) { 
    //       for(Standard_Integer k=1;k<=Ptr[i-1]->NbLines();k++) {
    // 	TopOpeBRep_LineInter& LI = myHAL->ChangeValue(nbl);
    // 	const Handle(IntPatch_Line)& L = Ptr[i-1]->Line(k);
    // 	LI.SetLine(L,S1,S2);
    // 	LI.Index(nbl);
    // 	myLineNb++;
    // 	nbl++;
    //       }
    //       delete Ptr[i-1];
    //     }
    // modified by NIZHNY-MKK  Mon Apr  2 12:17:57 2001.END
    free(Ptr);
  }
}

//=======================================================================
//function : Lines
//purpose  : 
//=======================================================================

Handle(TopOpeBRep_HArray1OfLineInter) TopOpeBRep_FacesIntersector::Lines()
{
  return myHAL;
}

//=======================================================================
//function : NbLines
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRep_FacesIntersector::NbLines()const
{
  return myLineNb;
}

//=======================================================================
//function : InitLine
//purpose  : 
//=======================================================================

void  TopOpeBRep_FacesIntersector::InitLine()
{
  myLineIndex = 1;
  FindLine();
}

//=======================================================================
//function : FindLine
//purpose  : 
//=======================================================================

void  TopOpeBRep_FacesIntersector::FindLine()
{
  myLineFound = Standard_False;
  if ( ! myIntersectionDone ) return;

  while (myLineIndex <= myLineNb) {
    const TopOpeBRep_LineInter& L = myHAL->Value(myLineIndex);
    myLineFound = L.OK();
    if (myLineFound) break;
    else myLineIndex++;
  }
}

//=======================================================================
//function : MoreLine
//purpose  : 
//=======================================================================

Standard_Boolean  TopOpeBRep_FacesIntersector::MoreLine()const 
{
  return myLineFound;
}


//=======================================================================
//function : NextLine
//purpose  : 
//=======================================================================

void  TopOpeBRep_FacesIntersector::NextLine()
{
  myLineIndex++;
  FindLine();
}

//=======================================================================
//function : CurrentLine
//purpose  : 
//=======================================================================

TopOpeBRep_LineInter& TopOpeBRep_FacesIntersector::CurrentLine()
{
  TopOpeBRep_LineInter& TLI = myHAL->ChangeValue(myLineIndex);
  return TLI;
}


//=======================================================================
//function : CurrentLineIndex
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRep_FacesIntersector::CurrentLineIndex()const
{
  return myLineIndex;
}

//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

TopOpeBRep_LineInter& TopOpeBRep_FacesIntersector::ChangeLine(const Standard_Integer IL)
{
  TopOpeBRep_LineInter& TLI = myHAL->ChangeValue(IL);
  return TLI;
}

//=======================================================================
//function : ResetIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_FacesIntersector::ResetIntersection() 
{
  myIntersectionDone = Standard_False;
  myLineIndex = 1;
  myLineNb = 0;
  myEdgeRestrictionMap.Clear();
  myLineFound = Standard_False;
}


//=======================================================================
//function : ForceTolerances
//purpose  : 
//=======================================================================

void TopOpeBRep_FacesIntersector::ForceTolerances(const Standard_Real Tol1,
						  const Standard_Real Tol2)
{
  myTol1 = Tol1;
  myTol2 = Tol2;  
  myForceTolerances = Standard_True;
#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceFITOL())
    cout<<"ForceTolerances : myTol1,myTol2 = "<<myTol1<<","<<myTol2<<endl;
#endif
}

//=======================================================================
//function : GetTolerances
//purpose  : 
//=======================================================================

void TopOpeBRep_FacesIntersector::GetTolerances(Standard_Real& Tol1,
						Standard_Real& Tol2) const
{
  Tol1 = myTol1;
  Tol2 = myTol2;
}

//=======================================================================
//function : ShapeTolerances
//purpose  : (private)
//=======================================================================

#ifdef OCCT_DEBUG
void TopOpeBRep_FacesIntersector::ShapeTolerances(const TopoDS_Shape& S1,
						  const TopoDS_Shape& S2)
#else
void TopOpeBRep_FacesIntersector::ShapeTolerances(const TopoDS_Shape& ,
						  const TopoDS_Shape& )
#endif
{
//  myTol1 = Max(ToleranceMax(S1,TopAbs_EDGE),ToleranceMax(S2,TopAbs_EDGE));
  myTol1 = Precision::Confusion();
  myTol2 = myTol1;  
  myForceTolerances = Standard_False;
#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceFITOL()) {
    cout<<"ShapeTolerances on S1 = ";TopAbs::Print(S1.ShapeType(),cout);
    cout<<" S2 = ";TopAbs::Print(S2.ShapeType(),cout);
    cout<<" : myTol1,myTol2 = "<<myTol1<<","<<myTol2<<endl;
  }
#endif
}

//=======================================================================
//function : ToleranceMax
//purpose  : (private)
//=======================================================================

Standard_Real TopOpeBRep_FacesIntersector::ToleranceMax
(const TopoDS_Shape& S,const TopAbs_ShapeEnum T) const
{
  TopExp_Explorer e(S,T);
  if ( ! e.More() ) return Precision::Intersection();
  else {
    Standard_Real tol = RealFirst();
    for (; e.More(); e.Next())
      tol = Max(tol,TopOpeBRepTool_ShapeTool::Tolerance(e.Current()));
    return tol;
  }
}


// modified by NIZHNY-MKK  Mon Apr  2 12:18:30 2001.BEGIN
// ================================================================================================
// static function: TestWLineAlongRestriction
// purpose:
// ================================================================================================
static Standard_Boolean TestWLineAlongRestriction(const Handle(IntPatch_WLine)& theWLine,
						  const Standard_Integer                         theRank,
						  const Handle(Adaptor3d_HSurface)&              theSurface,
						  const Handle(Adaptor3d_TopolTool)&             theDomain,
						  const Standard_Real                            theTolArc) {

  Standard_Boolean result = Standard_False; 
  Standard_Integer NbPnts = theWLine->NbPnts();  
  Standard_Integer along = 0;

  for(Standard_Integer i=1; i<=NbPnts; i++) {
    const IntSurf_PntOn2S& Pmid = theWLine->Point(i);
    Standard_Real u=0., v=0.;
    if(theRank==1) Pmid.ParametersOnS1(u, v);
    else Pmid.ParametersOnS2(u, v);
    //------------------------------------------
    gp_Pnt ap;
    gp_Vec ad1u, ad1v;
    //Standard_Real nad1u, nad1v, tolu, tolv;
    
    theSurface->D1(u, v, ap, ad1u, ad1v);
    //nad1u=ad1u.Magnitude();
    //nad1v=ad1v.Magnitude();
    //if(nad1u>1e-12) tolu=theTolArc/nad1u; else tolu=0.1;
    //if(nad1v>1e-12) tolv=theTolArc/nad1v; else tolv=0.1;
    //if(tolu>tolv)  tolu=tolv;
    //------------------------------------------
    
    //if(theDomain->IsThePointOn(gp_Pnt2d(u, v),tolu)) {
    //  along++;
    //}

    if(theDomain->IsThePointOn(gp_Pnt2d(u, v),theTolArc)) along++;
    //if(along!=i) break;
  }
  if(along==NbPnts) result = Standard_True;
  return result;
}


// ================================================================================================
// static function: BuildRLineBasedOnWLine
// purpose: 
// ================================================================================================
static 
Handle(IntPatch_RLine) BuildRLineBasedOnWLine(const Handle(IntPatch_WLine)& theWLine,
							       const Handle(Adaptor2d_HCurve2d)&              theArc,
							       const Standard_Integer                         theRank) {
  Handle(IntPatch_RLine) anRLine;

  if((theRank != 1) && (theRank != 2))
    return anRLine;

  gp_Pnt2d aPOnLine;
  Standard_Real u=0., v=0.;
  Standard_Integer nbvtx = theWLine->NbVertex();
  const IntPatch_Point& Vtx1 = theWLine->Vertex(1);
  const IntPatch_Point& Vtx2 = theWLine->Vertex(nbvtx);
  
  if(theRank == 1) {
    Vtx1.ParametersOnS1(u, v);
  }
  else {
    Vtx1.ParametersOnS2(u, v);
  }
  
  aPOnLine = gp_Pnt2d(u, v);
  Standard_Real par1 = Geom2dInt_TheProjPCurOfGInter::FindParameter(theArc->Curve2d(), aPOnLine, 1.e-7);
  
  if(theRank == 1) {
    Vtx2.ParametersOnS1(u, v);
  }
  else {
    Vtx2.ParametersOnS2(u, v);
  }
  aPOnLine = gp_Pnt2d(u, v);
  Standard_Real par2 = Geom2dInt_TheProjPCurOfGInter::FindParameter(theArc->Curve2d(), aPOnLine, 1.e-7);

  Standard_Real tol = (Vtx1.Tolerance() > Vtx2.Tolerance()) ? Vtx1.Tolerance() : Vtx2.Tolerance();

  if(Abs(par1 - par2) < theArc->Resolution(tol))
    return anRLine;

  Standard_Boolean IsOnFirst = (theRank == 1);

  Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
  const Handle(IntSurf_LineOn2S)& Lori = theWLine->Curve();
  IntSurf_Transition TransitionUndecided;
  
  anRLine = new IntPatch_RLine(Standard_False, theWLine->TransitionOnS1(), theWLine->TransitionOnS2());

  if(IsOnFirst) {
    anRLine->SetArcOnS1(theArc);
  }
  else {
    anRLine->SetArcOnS2(theArc);
  }
  
  Standard_Integer k = 0;
  if(par1 < par2) {

    for(k = 1; k <= Lori->NbPoints(); k++) { 
      aLineOn2S->Add(Lori->Value(k));
    }
    anRLine->Add(aLineOn2S);
    IntPatch_Point VtxFirst = Vtx1;
    
    VtxFirst.SetArc(IsOnFirst, //-- On First
		    theArc,
		    par1,
		    TransitionUndecided,
		    TransitionUndecided);
    VtxFirst.SetParameter(par1);
    anRLine->AddVertex(VtxFirst);
    
    for(k = 2; k < nbvtx; k++) { 
      IntPatch_Point Vtx = theWLine->Vertex(k);
      if(theRank == 1) {
	Vtx.ParametersOnS1(u, v);
      }
      else {
	Vtx.ParametersOnS2(u, v);
      }
      gp_Pnt2d atmpPoint(u, v);
      Standard_Real apar = Geom2dInt_TheProjPCurOfGInter::FindParameter(theArc->Curve2d(), atmpPoint, 1.e-7);
      Vtx.SetParameter(apar);
      anRLine->AddVertex(Vtx);
    }
    
    IntPatch_Point VtxLast = Vtx2;
    VtxLast.SetArc(IsOnFirst, //-- On First
		   theArc,
		   par2,
		   TransitionUndecided,
		   TransitionUndecided);
    VtxLast.SetParameter(par2);
    anRLine->AddVertex(VtxLast);
    anRLine->SetFirstPoint(1);
    anRLine->SetLastPoint(nbvtx);
    anRLine->ComputeVertexParameters(Precision::Confusion());
  }
  else {

    for(k = Lori->NbPoints(); k >= 1; k--) { 
      aLineOn2S->Add(Lori->Value(k));
    }
    anRLine->Add(aLineOn2S);

    IntPatch_Point VtxFirst = Vtx2;
    VtxFirst.SetArc(IsOnFirst, //-- On First
		    theArc,
		    par2,
		    TransitionUndecided,
		    TransitionUndecided);
    VtxFirst.SetParameter(par2);
    anRLine->AddVertex(VtxFirst);

    for(k = nbvtx - 1; k >= 2; k--) { 
      IntPatch_Point Vtx = theWLine->Vertex(k);
      Vtx.ReverseTransition();
      if(theRank == 1) {
	Vtx.ParametersOnS1(u, v);
      }
      else {
	Vtx.ParametersOnS2(u, v);
      }
      gp_Pnt2d atmpPoint(u, v);
      Standard_Real apar = Geom2dInt_TheProjPCurOfGInter::FindParameter(theArc->Curve2d(), atmpPoint, 1.e-7);
      Vtx.SetParameter(apar);
      anRLine->AddVertex(Vtx);
    }
    IntPatch_Point VtxLast = Vtx1;
    VtxLast.SetArc(IsOnFirst, //-- On First
		   theArc,
		   par1,
		   TransitionUndecided,
		   TransitionUndecided);
    VtxLast.SetParameter(par1);
    anRLine->AddVertex(VtxLast);
    anRLine->SetFirstPoint(1);
    anRLine->SetLastPoint(nbvtx);
    anRLine->ComputeVertexParameters(Precision::Confusion());
  }

  return anRLine;
}

// ================================================================================================
// static function: BuildRLine
// purpose: build rline based on group of wlines
//          return null handle if it is not possible to build rline
// ================================================================================================
static
Handle(IntPatch_RLine) BuildRLine(const IntPatch_SequenceOfLine&     theSeqOfWLine, 
						   const Standard_Integer             theRank,
						   const Handle(Adaptor3d_HSurface)&  theSurface,
						   const Handle(Adaptor3d_TopolTool)& theDomain,
						   const Standard_Real                theTolArc) {
  Handle(IntPatch_RLine) anRLine;
  const Handle(IntPatch_WLine)& aWLine1 = *((Handle(IntPatch_WLine) *)& (theSeqOfWLine.Value(1)));
  const Handle(IntPatch_WLine)& aWLine2 = *((Handle(IntPatch_WLine) *)& (theSeqOfWLine.Value(theSeqOfWLine.Length())));
  const IntPatch_Point& P1 = aWLine1->Vertex(1);
  const IntPatch_Point& P2 = aWLine2->Vertex(aWLine2->NbVertex());
  const Handle(Adaptor3d_HVertex)& aV1 = (theRank==1) ? P1.VertexOnS1() : P1.VertexOnS2();
  const Handle(Adaptor3d_HVertex)& aV2 = (theRank==1) ? P2.VertexOnS1() : P2.VertexOnS2();

  // avoid closed and degenerated edges
  if(aV1->IsSame(aV2))
    return anRLine;

  for(theDomain->Init(); theDomain->More(); theDomain->Next()) {
    theDomain->Initialize(theDomain->Value());
    Standard_Boolean foundVertex1 = Standard_False;
    Standard_Boolean foundVertex2 = Standard_False;

    for(theDomain->InitVertexIterator(); (!foundVertex1 || !foundVertex2) && theDomain->MoreVertex(); theDomain->NextVertex()) {

      if(!foundVertex1 && aV1->IsSame(theDomain->Vertex()))
	foundVertex1 = Standard_True;
      if(!foundVertex2 && aV2->IsSame(theDomain->Vertex()))
	  foundVertex2 = Standard_True;
    }

    if(foundVertex1 && foundVertex2) {
      Standard_Boolean buildrline = (theSeqOfWLine.Length() > 0);

      for(Standard_Integer i = 1; buildrline && i<=theSeqOfWLine.Length(); i++) {
	const Handle(IntPatch_WLine)& aWLine = 
	  *((Handle(IntPatch_WLine) *)& (theSeqOfWLine.Value(i)));

	Standard_Integer indexpnt = aWLine->NbPnts()/2;
	if(indexpnt < 1)
	  buildrline = Standard_False;
	else {	  
	  Standard_Real u = RealLast(), v = RealLast();
	  const IntSurf_PntOn2S& POn2S = aWLine->Point(indexpnt);
	  if(theRank==1) {
	    POn2S.ParametersOnS1(u, v);
	  }
	  else {
	    POn2S.ParametersOnS2(u, v);
	  }
	  gp_Pnt2d aPOnArc, aPOnLine(u, v);
	  Standard_Real par = Geom2dInt_TheProjPCurOfGInter::FindParameter(theDomain->Value()->Curve2d(), aPOnLine, 1e-7);
	  aPOnArc = theDomain->Value()->Value(par);
	  gp_Pnt ap;
	  gp_Vec ad1u, ad1v;
	  Standard_Real nad1u, nad1v, tolu, tolv;	  

	  theSurface->D1(u, v, ap, ad1u, ad1v);
	  nad1u=ad1u.Magnitude();
	  nad1v=ad1v.Magnitude();
	  if(nad1u>1e-12) tolu=theTolArc/nad1u; else tolu=0.1;
	  if(nad1v>1e-12) tolv=theTolArc/nad1v; else tolv=0.1;
	  Standard_Real aTolerance  = (tolu > tolv) ? tolv : tolu;
	  
	  if(aPOnArc.Distance(aPOnLine) > aTolerance) {
	    buildrline = Standard_False;
	  }
	}
      } //end for
      
      if(buildrline) {
	IntSurf_TypeTrans trans1 = IntSurf_Undecided, trans2 = IntSurf_Undecided;

	Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();

	for(Standard_Integer j = 1; j<=theSeqOfWLine.Length(); j++) {
	  const Handle(IntPatch_WLine)& atmpWLine = 
	    *((Handle(IntPatch_WLine) *)& (theSeqOfWLine.Value(j)));

	  const Handle(IntSurf_LineOn2S)& Lori = atmpWLine->Curve();
	  
	  if(atmpWLine->TransitionOnS1()!=IntSurf_Undecided && atmpWLine->TransitionOnS1()!=IntSurf_Touch) {
	    trans1 = atmpWLine->TransitionOnS1();
	  }
	  if(atmpWLine->TransitionOnS2()!=IntSurf_Undecided && atmpWLine->TransitionOnS2()!=IntSurf_Touch) {
	    trans2 = atmpWLine->TransitionOnS2();
	  }

	  Standard_Integer ParamMinOnLine = (Standard_Integer) atmpWLine->Vertex(1).ParameterOnLine();
	  Standard_Integer ParamMaxOnLine = (Standard_Integer) atmpWLine->Vertex(atmpWLine->NbVertex()).ParameterOnLine();

	  for(Standard_Integer k = ParamMinOnLine; k <= ParamMaxOnLine; k++) { 
	    aLineOn2S->Add(Lori->Value(k));
	  }
	}

	Handle(IntPatch_WLine) emulatedWLine = 
	  new IntPatch_WLine(aLineOn2S, Standard_False, trans1, trans2);

	IntPatch_Point aFirstVertex = P1;
	IntPatch_Point aLastVertex  = P2;
	aFirstVertex.SetParameter(1);
	aLastVertex.SetParameter(aLineOn2S->NbPoints());

	emulatedWLine->AddVertex(aFirstVertex);
	Standard_Integer apointindex = 0;

	for(apointindex = 2; apointindex <= aWLine1->NbVertex(); apointindex++) {
	  IntPatch_Point aPoint = aWLine1->Vertex(apointindex);
	  Standard_Real aTolerance = (aPoint.Tolerance() > P1.Tolerance()) ? aPoint.Tolerance() : P1.Tolerance();
	  if(aPoint.Value().IsEqual(P1.Value(), aTolerance)) {
	    aPoint.SetParameter(1);
	    emulatedWLine->AddVertex(aPoint);
	  }
	}
	
	for(apointindex = 1; apointindex < aWLine2->NbVertex(); apointindex++) {
	  IntPatch_Point aPoint = aWLine2->Vertex(apointindex);
	  Standard_Real aTolerance = (aPoint.Tolerance() > P2.Tolerance()) ? aPoint.Tolerance() : P2.Tolerance();
	  if(aPoint.Value().IsEqual(P2.Value(), aTolerance)) {
	    aPoint.SetParameter(aLineOn2S->NbPoints());
	    emulatedWLine->AddVertex(aPoint);
	  }
	}

	emulatedWLine->AddVertex(aLastVertex);

	anRLine = BuildRLineBasedOnWLine(emulatedWLine, theDomain->Value(), theRank);
	
	break;
      }
    }
  } //end for
  
  return anRLine;
}

// ================================================================================================
// static function: TestWLinesToAnArc
// purpose: test if possible to replace group of wlines by rline and replace in the sequence slin
// ================================================================================================
static void TestWLinesToAnArc(IntPatch_SequenceOfLine&           slin,
			      const Handle(Adaptor3d_HSurface)&  theSurface1,
			      const Handle(Adaptor3d_TopolTool)& theDomain1,
			      const Handle(Adaptor3d_HSurface)&  theSurface2,
			      const Handle(Adaptor3d_TopolTool)& theDomain2,
			      const Standard_Real                theTolArc) { 

  IntPatch_SequenceOfLine aSeqOfWLine;
  IntPatch_SequenceOfLine aSeqOfRLine;
  for(Standard_Integer rank = 1; rank <= 2; rank++) {
    for(Standard_Integer i=1; i<=slin.Length(); i++) {
      if(slin.Value(i)->ArcType()!=IntPatch_Walking)
	continue;
      const Handle(IntPatch_WLine)& aWLine = *((Handle(IntPatch_WLine) *)& (slin.Value(i)));
      Standard_Integer nbvtx = aWLine->NbVertex();
      const IntPatch_Point& Vtx1 = aWLine->Vertex(1);
      const IntPatch_Point& Vtx2 = aWLine->Vertex(nbvtx);
      Standard_Boolean isvertex = Standard_False, wlineWasAppended = Standard_False;


      if(rank==1)
	isvertex = Vtx1.IsVertexOnS1();
      else
	isvertex = Vtx1.IsVertexOnS2();

      if(isvertex) {
	Standard_Boolean appendwline = Standard_True;
	if(rank==1) {
	  if(!aWLine->HasArcOnS1() && !TestWLineAlongRestriction(aWLine, rank, theSurface1, theDomain1, theTolArc)) {	    
	    appendwline = Standard_False;
	  }
	}
	if(rank==2) {
	  if(!aWLine->HasArcOnS2() && !TestWLineAlongRestriction(aWLine, rank, theSurface2, theDomain2, theTolArc)) {
	    appendwline = Standard_False;
	  }
	}
	wlineWasAppended = appendwline;
	if(appendwline)
	  aSeqOfWLine.Append(aWLine);
      }
      else {
	if(aSeqOfWLine.Length()==0)
	  continue;
	const Handle(IntPatch_WLine)& aLastWLine = 
	  *((Handle(IntPatch_WLine) *)& (aSeqOfWLine.Value(aSeqOfWLine.Length())));
	const IntPatch_Point& aLastPoint = aLastWLine->Vertex(aLastWLine->NbVertex());
	Standard_Real aTolerance = (aLastPoint.Tolerance() > Vtx1.Tolerance()) ? aLastPoint.Tolerance() : Vtx1.Tolerance();
	if(aLastPoint.Value().IsEqual(Vtx1.Value(), aTolerance)) {
	  Standard_Boolean appendwline = Standard_True;
	  if(rank==1) {
	    if(!aWLine->HasArcOnS1() && !TestWLineAlongRestriction(aWLine, rank, theSurface1, theDomain1, theTolArc)) {	    
	      appendwline = Standard_False;
	    }
	  }
	  if(rank==2) {
	    if(!aWLine->HasArcOnS2() && !TestWLineAlongRestriction(aWLine, rank, theSurface2, theDomain2, theTolArc)) {
	      appendwline = Standard_False;
	    }
	  }
	  wlineWasAppended = appendwline;
	  if(appendwline)
	    aSeqOfWLine.Append(aWLine);	  
	}
	else {
	  aSeqOfWLine.Clear();
	}
      }

      isvertex = Standard_False;
      if(rank==1)
	isvertex = Vtx2.IsVertexOnS1();
      else
	isvertex = Vtx2.IsVertexOnS2();

      if(wlineWasAppended && isvertex) {
	// build rline based on sequence of wlines.
	Handle(IntPatch_RLine) anRLine;
	if(rank==1) {
	  anRLine = BuildRLine(aSeqOfWLine, rank, theSurface1, theDomain1, theTolArc);
	}
	else {
	  anRLine = BuildRLine(aSeqOfWLine, rank, theSurface2, theDomain2, theTolArc);
	}
	
	if(!anRLine.IsNull()) {
	  aSeqOfRLine.Append(anRLine);
	  for(Standard_Integer k=1; k<=aSeqOfWLine.Length(); k++) {
	    for(Standard_Integer j=1; j<=slin.Length(); j++) {
	      if(aSeqOfWLine(k)==slin(j)) {
		slin.Remove(j);
		break;
	      }
	    }
	  }
	}
	aSeqOfWLine.Clear();
      }
    }
  }
  
  for(Standard_Integer i=1; i<=aSeqOfRLine.Length(); i++) {
    slin.Append(aSeqOfRLine.Value(i));
  }  
}
// modified by NIZHNY-MKK  Mon Apr  2 12:18:34 2001.END

//====================================================================================
// function: MergeWLinesIfAllSegmentsAlongRestriction
//
//  purpose: If the result of LineConstructor is a set of WLines segments which are
//           placed along RESTRICTION, we can suppose that this result is not correct:
//           here we should have a RLine. If it is not possible to construct RLine
//           we should merge segments of WLines into single WLine equals to the same
//           RLine.
//====================================================================================
static void MergeWLinesIfAllSegmentsAlongRestriction(IntPatch_SequenceOfLine&           theSlin,
						     const Handle(Adaptor3d_HSurface)&  theSurface1,
						     const Handle(Adaptor3d_TopolTool)& theDomain1,
						     const Handle(Adaptor3d_HSurface)&  theSurface2,
						     const Handle(Adaptor3d_TopolTool)& theDomain2,
						     const Standard_Real                theTolArc)
{
  Standard_Integer i = 0, rank = 0;
  Standard_Real tol = 1.e-9;

  // here we check that all segments of WLines placed along restriction
  Standard_Integer WLOnRS1 = 0;
  Standard_Integer WLOnRS2 = 0;
  Standard_Integer NbWLines = 0;
  TColgp_SequenceOfPnt sqVertexPoints;

  for(rank = 1; rank <= 2; rank++)
    {
      NbWLines = 0;
      for(i = 1; i <= theSlin.Length(); i++)
	{
	  if( theSlin.Value(i)->ArcType() != IntPatch_Walking )
	    continue;
	  NbWLines++;
	  const Handle(IntPatch_WLine)& aWLine = *((Handle(IntPatch_WLine) *)& (theSlin.Value(i)));
	  Standard_Integer nbvtx = aWLine->NbVertex();
	  const IntPatch_Point& Vtx1 = aWLine->Vertex(1);
	  const IntPatch_Point& Vtx2 = aWLine->Vertex(nbvtx);
	  if( rank==1 )
	    {
	      sqVertexPoints.Append(Vtx1.Value());
	      sqVertexPoints.Append(Vtx2.Value());
	      if( TestWLineAlongRestriction(aWLine, rank, theSurface1, theDomain1, theTolArc) )
		WLOnRS1++;
	    }
	  else
	    {
	      if( TestWLineAlongRestriction(aWLine, rank, theSurface2, theDomain2, theTolArc) )
		WLOnRS2++;
	    }
	}
      if( NbWLines == WLOnRS1 || NbWLines == WLOnRS2 ) break;
    }

  Standard_Integer WLineRank = 0;   // not possible to merge WLines

  if( WLOnRS1 == NbWLines )
    WLineRank = 1;                  // create merged WLine based on arc of S1
  else if( WLOnRS2 == NbWLines )
    WLineRank = 2;                  // create merged WLine based on arc of S2
  else
    return;

  // avoid closed (degenerated) edges
  if( sqVertexPoints.Length() <= 2 )
    return;
  if( sqVertexPoints.Value(1).IsEqual(sqVertexPoints.Value(sqVertexPoints.Length()),tol) )
    return;

  Standard_Real TolVrtx = 1.e-5;
  Standard_Integer testPointIndex = ( sqVertexPoints.Length() > 3 ) ? ((Standard_Integer) sqVertexPoints.Length() / 2) : 2;
  gp_Pnt testPoint = sqVertexPoints.Value( testPointIndex );
  Standard_Real Fp = 0., Lp = 0.;


  Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
  //
  Standard_Integer arcnumber = (WLineRank == 1) ?
      GetArc(theSlin,WLineRank,theSurface1,theDomain1,theSurface2,testPoint,TolVrtx,aLineOn2S,Fp,Lp) :
      GetArc(theSlin,WLineRank,theSurface2,theDomain2,theSurface1,testPoint,TolVrtx,aLineOn2S,Fp,Lp);
  //    
  if (arcnumber == 0) {
    return;
  }
  //
  Handle(IntPatch_WLine) anWLine = GetMergedWLineOnRestriction(theSlin,TolVrtx,aLineOn2S);
  if (!anWLine.IsNull()) {
    theSlin.Clear();
    theSlin.Append(anWLine);
#ifdef OCCT_DEBUG
    cout << "*** TopOpeBRep_FaceIntersector: Merge WLines on Restriction " 
         << ((WLineRank == 1) ? "S1" : "S2") << " to WLine***" << endl;
#endif
  }
}

//=========================================================================================
// function: GetArc
//
//  purpose: Define arc on (OBJ) surface which all WLine segments are placed along.
//           Check states of points in the gaps between segments on (TOOL). If those states
//           are IN or ON return the LineOn2S based on points3D were given from detected arc.
//           Returns 0 if it is not possible to create merged WLine.
//========================================================================================
static Standard_Integer GetArc(IntPatch_SequenceOfLine&           theSlin,
			       const Standard_Integer&            theRankS,
			       const Handle(Adaptor3d_HSurface)&  theSurfaceObj,
			       const Handle(Adaptor3d_TopolTool)& theDomainObj,
			       const Handle(Adaptor3d_HSurface)&  theSurfaceTool,
			       const gp_Pnt&                      theTestPoint,
			       Standard_Real&                     theVrtxTol,
			       Handle(IntSurf_LineOn2S)&          theLineOn2S,
			       Standard_Real&                     theFirst,
			       Standard_Real&                     theLast)
{
  // 1. find number of arc (edge) on which the WLine segments are placed.

  Standard_Real MinDistance2 = 1.e+200, firstES1 = 0., lastES1 = 0.;
  Standard_Integer ArcNumber = 0, CurArc = 0, i = 0, j = 0;
  theFirst = 0.;
  theLast = 0.;

  for(theDomainObj->Init(); theDomainObj->More(); theDomainObj->Next())
    {
      CurArc++;
      Standard_Address anEAddress = theDomainObj->Edge();

      if( anEAddress == NULL )
	continue;
      
      TopoDS_Edge* anE=(TopoDS_Edge*)anEAddress;
      Handle(Geom_Curve) aCEdge=BRep_Tool::Curve(*anE, firstES1, lastES1);
      if ( aCEdge.IsNull() ) // e.g. degenerated edge, see OCC21770
        continue;
      GeomAdaptor_Curve CE;
      CE.Load(aCEdge);
      Extrema_ExtPC epc(theTestPoint, CE, 1.e-7);

      if( epc.IsDone() )
	{
	  for( i = 1; i <= epc.NbExt(); i++ )
	    {
	      if( epc.SquareDistance( i ) < MinDistance2 )
		{
		  MinDistance2 = epc.SquareDistance( i );
		  ArcNumber = CurArc;
		}
	    }
	}
    }

  if( ArcNumber == 0 )
    return 0;

  // 2. load parameters of founded edge and its arc.
  CurArc = 0;
  TColgp_SequenceOfPnt PointsFromArc;
  Handle(Adaptor2d_HCurve2d) arc = NULL;
  Standard_Real tol = 1.e-7;
  TColStd_SequenceOfReal WLVertexParameters;
  Standard_Boolean classifyOK = Standard_True;
  Standard_Real CheckTol = 1.e-5;

  for(theDomainObj->Init(); theDomainObj->More(); theDomainObj->Next())
    {
      CurArc++;
      if( CurArc != ArcNumber )
	continue;

      arc = theDomainObj->Value();

      for(i = 1; i <= theSlin.Length(); i++)
	{
	  if( theSlin.Value(i)->ArcType() != IntPatch_Walking )
	    continue;

	  const Handle(IntPatch_WLine)& aWLine = *((Handle(IntPatch_WLine) *)& (theSlin.Value(i)));

	  Standard_Integer nbpnts = aWLine->NbPnts();
	  const IntSurf_PntOn2S& POn2S_F = aWLine->Point(1);
	  const IntSurf_PntOn2S& POn2S_L = aWLine->Point(nbpnts);

	  Standard_Real Upf = 0., Vpf = 0., Upl = 0., Vpl = 0.;

	  if(theRankS == 1)
	    {
	      POn2S_F.ParametersOnS1(Upf, Vpf);
	      POn2S_L.ParametersOnS1(Upl, Vpl);
	    }
	  else
	    {
	      POn2S_F.ParametersOnS2(Upf, Vpf);
	      POn2S_L.ParametersOnS2(Upl, Vpl);
	    }

	  gp_Pnt2d aPOnLine_F(Upf, Vpf);
	  gp_Pnt2d aPOnLine_L(Upl, Vpl);

	  Standard_Real par_F = Geom2dInt_TheProjPCurOfGInter::FindParameter(arc->Curve2d(), aPOnLine_F, tol);
	  Standard_Real par_L = Geom2dInt_TheProjPCurOfGInter::FindParameter(arc->Curve2d(), aPOnLine_L, tol);

	  WLVertexParameters.Append(par_F);
	  WLVertexParameters.Append(par_L);
	}
      
      for(i = 1; i <= WLVertexParameters.Length(); i++)
	{
	  for(j = i; j <= WLVertexParameters.Length(); j++)
	    {
	      if(j == i)
		continue;
	  
	      if(WLVertexParameters.Value(i) > WLVertexParameters.Value(j))
		{
		  Standard_Real pol = WLVertexParameters.Value(i);
		  WLVertexParameters.SetValue(i, WLVertexParameters.Value(j));
		  WLVertexParameters.SetValue(j, pol);
		}
	    }
	}

      Standard_Address anEAddress = theDomainObj->Edge();
      TopoDS_Edge* anE=(TopoDS_Edge*)anEAddress;
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(*anE,V1,V2);
      Standard_Real MaxVertexTol = Max(BRep_Tool::Tolerance(V1),BRep_Tool::Tolerance(V2));
      theVrtxTol = MaxVertexTol;
      Standard_Real EdgeTol = BRep_Tool::Tolerance(*anE);
      CheckTol = Max(MaxVertexTol, EdgeTol);
      Handle(Geom_Curve) aCEdge=BRep_Tool::Curve(*anE, firstES1, lastES1);
      // classification gaps
      //  a. min - first
      if(Abs(firstES1 - WLVertexParameters.Value(1)) > arc->Resolution(MaxVertexTol))
	{
	  Standard_Real param = (firstES1 + WLVertexParameters.Value(1)) / 2.;
	  gp_Pnt point;
	  aCEdge->D0(param, point);
	  if( !IsPointOK(point, theSurfaceTool->Surface(), CheckTol) )
	    {
	      classifyOK = Standard_False;
	      break;
	    }
	}
      //  b. max - last
      if(Abs(lastES1 - WLVertexParameters.Value(WLVertexParameters.Length())) > arc->Resolution(MaxVertexTol))
	{
	  Standard_Real param = (lastES1 + WLVertexParameters.Value(WLVertexParameters.Length())) / 2.;
	  gp_Pnt point;
	  aCEdge->D0(param, point);
	  if( !IsPointOK(point, theSurfaceTool->Surface(), CheckTol) )
	    {
	      classifyOK = Standard_False;
	      break;
	    }
	}
      //  c. all middle gaps
      Standard_Integer NbChkPnts = WLVertexParameters.Length() / 2 - 1;
      for(i = 1; i <= NbChkPnts; i++)
	{
	  if( Abs(WLVertexParameters.Value(i*2+1) - WLVertexParameters.Value(i*2)) > arc->Resolution(MaxVertexTol))
	    {
	      Standard_Real param = (WLVertexParameters.Value(i*2) + WLVertexParameters.Value(i*2+1)) / 2.;
	      gp_Pnt point;
	      aCEdge->D0(param, point);
	      if( !IsPointOK(point, theSurfaceTool->Surface(), CheckTol) )
		{
		  classifyOK = Standard_False;
		  break;
		}
	    }
	}
      
      if( !classifyOK )
	break;

      // if classification gaps OK, fill sequence by the points from arc (edge)
      Standard_Real ParamFirst = WLVertexParameters.Value(1);
      Standard_Real ParamLast  = WLVertexParameters.Value(WLVertexParameters.Length());
      Standard_Real dParam     = Abs(ParamLast - ParamFirst) / 100.;
      Standard_Real cParam = ParamFirst;
      for( i = 0; i < 100; i++ )
	{
	  if( i == 99 )
	    cParam = ParamLast;

	  gp_Pnt cPnt;
	  aCEdge->D0(cParam, cPnt);
	  PointsFromArc.Append(cPnt);
	  cParam += dParam;
	}
      theFirst = ParamFirst;
      theLast  = ParamLast;

    }

  if( !classifyOK )
    return 0;

  // create IntSurf_LineOn2S from points < PointsFromArc >
  for( i = 1; i <= PointsFromArc.Length(); i++ )
    {
      Extrema_POnSurf pOnS1;
      Extrema_POnSurf pOnS2;
      gp_Pnt arcpoint = PointsFromArc.Value( i );
      Standard_Boolean isOnS1 = GetPointOn2S( arcpoint, theSurfaceObj->Surface(), CheckTol, pOnS1 );
      Standard_Boolean isOnS2 = GetPointOn2S( arcpoint, theSurfaceTool->Surface(), CheckTol, pOnS2 );
      if( isOnS1 && isOnS2 )
	{
	  Standard_Real u1 = 0., v1 = 0., u2 = 0., v2 = 0.;
	  pOnS1.Parameter(u1, v1);
	  pOnS2.Parameter(u2, v2);
	  IntSurf_PntOn2S pOn2S;
	  pOn2S.SetValue(arcpoint, u1, v1, u2, v2 );
	  theLineOn2S->Add( pOn2S );
	}
    }

  return ArcNumber;
}

//=========================================================================================
// function: IsPointOK
//
//  purpose: returns the state of testPoint on OTHER face.
//========================================================================================
static Standard_Boolean IsPointOK(const gp_Pnt&            theTestPnt,
				  const Adaptor3d_Surface& theTestSurface,
				  const Standard_Real&     theTol)
{
  Standard_Boolean result = Standard_False;
  Standard_Real ExtTol = theTol;//1.e-7;
  Extrema_ExtPS extPS(theTestPnt,theTestSurface,ExtTol,ExtTol);
  if( extPS.IsDone() && extPS.NbExt() > 0 )
    {
      Standard_Integer i = 0;
      Standard_Real MinDist2 = 1.e+200;
      for(i = 1; i <= extPS.NbExt(); i++)
	{
	  if( extPS.SquareDistance(i) < MinDist2 )
	    {
	      MinDist2 = extPS.SquareDistance(i);
	    }
	}
      if( MinDist2 <= theTol * theTol )
	  result = Standard_True;
    }
  return result;
}

//=========================================================================================
// function: GetPointOn2S
//
//  purpose: check state of testPoint and returns result point if state is OK.
//========================================================================================
static Standard_Boolean GetPointOn2S(const gp_Pnt&            theTestPnt,
				     const Adaptor3d_Surface& theTestSurface,
				     const Standard_Real&     theTol,
				     Extrema_POnSurf&         theResultPoint)
{
  Standard_Boolean result = Standard_False;
  Standard_Real ExtTol = theTol;//1.e-7;
  Extrema_ExtPS extPS(theTestPnt,theTestSurface,ExtTol,ExtTol);
  if( extPS.IsDone() && extPS.NbExt() > 0 )
    {
      Standard_Integer i = 0, minext = 1;
      Standard_Real MinDist2 = 1.e+200;
      for(i = 1; i <= extPS.NbExt(); i++)
	{
	  if( extPS.SquareDistance(i) < MinDist2 )
	    {
	      minext = i;
	      MinDist2 = extPS.SquareDistance(i);
	    }
	}
      if( MinDist2 <= theTol * theTol )
	{
	  result = Standard_True;
	  theResultPoint = extPS.Point(minext);
	}
    }
  return result;
}

//=========================================================================================
// function: GetMergedWLineOnRestriction
//
//  purpose: merge sequence of WLines all placed along restriction if the conditions of
//           merging are OK.
//========================================================================================
static Handle(IntPatch_WLine) GetMergedWLineOnRestriction(IntPatch_SequenceOfLine&           theSlin,
									   const Standard_Real&               theVrtxTol,
									   const Handle(IntSurf_LineOn2S)&    theLineOn2S)
{
  Handle(IntPatch_WLine) mWLine;
  if (theLineOn2S->NbPoints() == 0) {
    return mWLine;
  }
  //
  IntSurf_TypeTrans trans1 = IntSurf_Undecided;
  IntSurf_TypeTrans trans2 = IntSurf_Undecided;
  Standard_Integer i = 0;

  for(i = 1; i <= theSlin.Length(); i++)
    {
      if( theSlin.Value(i)->ArcType() != IntPatch_Walking )
	continue;

      const Handle(IntPatch_WLine)& aWLine = *((Handle(IntPatch_WLine) *)& (theSlin.Value(i)));

      if( aWLine->TransitionOnS1() != IntSurf_Undecided  &&  aWLine->TransitionOnS1() != IntSurf_Touch )
	trans1 = aWLine->TransitionOnS1();
      if( aWLine->TransitionOnS2() != IntSurf_Undecided  &&  aWLine->TransitionOnS2() != IntSurf_Touch )
	trans2 = aWLine->TransitionOnS2();
    }

  mWLine = new IntPatch_WLine(theLineOn2S, Standard_False, trans1, trans2);

  Standard_Integer NbPnts = mWLine->NbPnts();
  IntPatch_Point aFirstVertex, aLastVertex;

  aFirstVertex.SetValue(mWLine->Point(1).Value(),theVrtxTol,Standard_False);
  aLastVertex.SetValue(mWLine->Point(NbPnts).Value(),theVrtxTol,Standard_False);

  Standard_Real u1 = 0., v1 = 0., u2 = 0., v2 = 0.;

  mWLine->Point(1).Parameters(u1, v1, u2, v2);
  aFirstVertex.SetParameters(u1, v1, u2, v2);

  mWLine->Point(NbPnts).Parameters(u1, v1, u2, v2);
  aLastVertex.SetParameters(u1, v1, u2, v2);

  aFirstVertex.SetParameter(1);
  aLastVertex.SetParameter(theLineOn2S->NbPoints());

  mWLine->AddVertex(aFirstVertex);
  mWLine->AddVertex(aLastVertex);

  mWLine->ComputeVertexParameters(theVrtxTol);

  return mWLine;
}
