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

// Modifed:     Portage NT 7-5-97 DPF (return)

#include <ChFi3d_FilletShape.hxx>
#include <FilletSurf_Builder.hxx>
#include <FilletSurf_ErrorTypeStatus.hxx>
#include <FilletSurf_InternalBuilder.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : FilletSurf_Builder
//purpose  : 
//=======================================================================
FilletSurf_Builder::FilletSurf_Builder(const TopoDS_Shape& S, 
	                               const TopTools_ListOfShape& E, 
				       const Standard_Real R,			       
				       const Standard_Real Ta,
                                       const Standard_Real Tapp3d,
                                       const Standard_Real Tapp2d ):
                                       myIntBuild(S,ChFi3d_Polynomial,Ta,Tapp3d,Tapp2d)
{
  myisdone=FilletSurf_IsOk;
  int add =myIntBuild.Add(E,R);
  if (add!=0) { 
    myisdone=FilletSurf_IsNotOk;  
    if     (add==1)  myerrorstatus=FilletSurf_EmptyList;
    else if(add==2)  myerrorstatus=FilletSurf_EdgeNotG1;              
    else if(add==3)  myerrorstatus=FilletSurf_FacesNotG1;
    else if(add==4)  myerrorstatus=FilletSurf_EdgeNotOnShape;
    else if(add==5)  myerrorstatus=FilletSurf_NotSharpEdge;
  }
}
//========================================================
//
//============================================================

void FilletSurf_Builder::Perform()
{
  if (myisdone==FilletSurf_IsOk) {
    myIntBuild.Perform();
    if (myIntBuild.Done()) myisdone=FilletSurf_IsOk;
    else if (myIntBuild.NbSurface()!=0) {
      myisdone=FilletSurf_IsPartial;
      myerrorstatus=FilletSurf_PbFilletCompute; 
    }
    else { 
      myisdone=FilletSurf_IsNotOk;
      myerrorstatus=FilletSurf_PbFilletCompute; 
    }
  }  
}

//=======================================================================
//function : IsDone 
//purpose  :  gives the status of the computation of the fillet 
//=======================================================================
FilletSurf_StatusDone FilletSurf_Builder::IsDone() const 
{
 return myisdone;
}

//=======================================================================
//function : ErrorTypeStatus
//purpose  :  gives the status  of the error 
//=======================================================================
FilletSurf_ErrorTypeStatus FilletSurf_Builder::StatusError() const 
{
 return  myerrorstatus;
}


//=======================================================================
//function : NbSurface
//purpose  :  gives the number of NUBS surfaces  of the Fillet
//=======================================================================

Standard_Integer FilletSurf_Builder::NbSurface() const 
{
  if (IsDone()!=FilletSurf_IsNotOk)  return myIntBuild.NbSurface();
  throw StdFail_NotDone("FilletSurf_Builder::NbSurface");
}

//=======================================================================
//function : SurfaceFillet
//purpose  : gives the NUBS surface of index Index
//=======================================================================

const Handle(Geom_Surface)& FilletSurf_Builder::SurfaceFillet(const Standard_Integer Index) const 
{
  if ( (Index<1)||(Index>NbSurface())) throw Standard_OutOfRange("FilletSurf_Builder::SurfaceFillet");
  return myIntBuild.SurfaceFillet(Index); 
}

//=======================================================================
//function : TolApp3d
//purpose  :  gives the 3d tolerance reached during approximation 
//=======================================================================
Standard_Real  FilletSurf_Builder::TolApp3d(const Standard_Integer Index) const 
{
  if ( (Index<1)||(Index>NbSurface())) 
    throw Standard_OutOfRange("FilletSurf_Builder::TolApp3d");
  return myIntBuild.TolApp3d(Index);
}

//=======================================================================
//function : SupportFace1 
//purpose  : gives the first support  face relative to SurfaceFillet(Index)
//=======================================================================
const TopoDS_Face& FilletSurf_Builder::SupportFace1(const Standard_Integer Index) const
{
  if ( (Index<1)||(Index>NbSurface())) 
    throw Standard_OutOfRange("FilletSurf_Builder::SupportFace1");
  return myIntBuild.SupportFace1(Index); 
}

//=======================================================================
//function : SupportFace2
//purpose  : gives the second support face relative to SurfaceFillet(Index)
//=======================================================================
const TopoDS_Face& FilletSurf_Builder::SupportFace2(const Standard_Integer Index) const 
{
 if ( (Index<1)||(Index>NbSurface())) 
   throw Standard_OutOfRange("FilletSurf_Builder::SupportFace2");
 return myIntBuild.SupportFace2(Index);
  
}

//===============================================================================
//function : CurveOnFace1 
//purpose  :  gives  the 3d curve  of SurfaceFillet(Index)  on SupportFace1(Index)
//===============================================================================
const Handle(Geom_Curve)& FilletSurf_Builder::CurveOnFace1(const Standard_Integer Index) const 
{
  if ( (Index<1)||(Index>NbSurface())) 
    throw Standard_OutOfRange("FilletSurf_Builder::CurveOnFace1");
  return myIntBuild.CurveOnFace1(Index);
}

//=======================================================================
//function : CurveOnFace2
//purpose  : gives the 3d  curve of  SurfaceFillet(Index) on SupportFace2(Index
//=======================================================================
const Handle(Geom_Curve)& FilletSurf_Builder::CurveOnFace2(const Standard_Integer Index) const 
{ 
  if ( (Index<1)||(Index>NbSurface())) 
    throw Standard_OutOfRange("FilletSurf_Builder::CurveOnFace2");
  return myIntBuild.CurveOnFace2(Index);
}

//=======================================================================
//function : PCurveOnFace1
//purpose  : gives the  PCurve associated to CurveOnFace1(Index)  on the support face
//=======================================================================
const Handle(Geom2d_Curve)& FilletSurf_Builder::PCurveOnFace1(const Standard_Integer Index) const 
{ 
  if ( (Index<1)||(Index>NbSurface())) 
    throw Standard_OutOfRange( "FilletSurf_Builder::PCurveOnFace1");
  return myIntBuild.PCurveOnFace1(Index);
}

//=======================================================================
//function : PCurve1OnFillet
//purpose  : gives the PCurve associated to CurveOnFace1(Index) on the Fillet
//=======================================================================
const Handle(Geom2d_Curve)& FilletSurf_Builder::PCurve1OnFillet(const Standard_Integer Index) const 
{ 
  if ( (Index<1)||(Index>NbSurface())) 
    throw Standard_OutOfRange("FilletSurf_Builder::PCurve1OnFillet");
  return myIntBuild.PCurve1OnFillet(Index);
}

//=======================================================================
//function : PCurveOnFace2
//purpose  : gives the  PCurve associated to CurveOnFace2(Index)  on the support face
//=======================================================================
const Handle(Geom2d_Curve)& FilletSurf_Builder::PCurveOnFace2(const Standard_Integer Index) const 
{
  if ( (Index<1)||(Index>NbSurface())) 
    throw Standard_OutOfRange("FilletSurf_Builder::PCurveOnFace2");
  return myIntBuild.PCurveOnFace2(Index);
}

//=======================================================================
//function : PCurve2OnFillet
//purpose  : gives the PCurve associated to CurveOnFace2(Index) on the Fillet
//=======================================================================
const Handle(Geom2d_Curve)& FilletSurf_Builder::PCurve2OnFillet(const Standard_Integer Index) const 
{
  if ( (Index<1)||(Index>NbSurface())) 
    throw Standard_OutOfRange("FilletSurf_Builder::PCurve2OnFillet");
  return myIntBuild.PCurve2OnFillet(Index);
}

//=======================================================================
//function : FirstParameter 
//purpose  : gives the parameter of the fillet  on the first edge
//=======================================================================
Standard_Real FilletSurf_Builder::FirstParameter() const
{
  if (IsDone()==FilletSurf_IsNotOk) 
    throw StdFail_NotDone("FilletSurf_Builder::FirstParameter");
  return myIntBuild.FirstParameter();
}

//=======================================================================
//function : LastParameter
//purpose  :  gives the parameter of the fillet  on the last edge
//=======================================================================
Standard_Real FilletSurf_Builder::LastParameter() const
{
  if (IsDone()==FilletSurf_IsNotOk) 
    throw StdFail_NotDone("FilletSurf_Builder::LastParameter");
  return myIntBuild.LastParameter();
}

//=======================================================================
//function : StatusStartSection
//purpose  :  returns: 
//            twoExtremityonEdge: each extremity of  start section of the Fillet is
//                                on the edge of  the corresponding support face.  
//            OneExtremityOnEdge: only one  of  the extremities of  start section  of the  Fillet 
//                                is on the  edge of the corresponding support face.  
//            NoExtremityOnEdge:  any extremity of  the start section  ofthe fillet is  on  
//                                the edge  of   the  corresponding support face.
//=======================================================================
FilletSurf_StatusType  FilletSurf_Builder::StartSectionStatus() const 
{
  if (IsDone()==FilletSurf_IsNotOk)
    throw StdFail_NotDone("FilletSurf_Builder::StartSectionStatus" );
  return  myIntBuild.StartSectionStatus();
}

//=======================================================================
//function : StatusEndSection
//purpose  :  returns: 
//       twoExtremityonEdge: each extremity of  end section of the Fillet is
//                        on the edge of  the corresponding support face.  
//       OneExtremityOnEdge:  only one  of  the extremities of  end  section  of the  Fillet 
//                           is on the  edge of the corresponding support face.  
//       NoExtremityOnEdge:  any extremity of  the end  section  of the fillet is  on  
//                           the edge  of   the  corresponding support face. 
//=======================================================================
FilletSurf_StatusType  FilletSurf_Builder::EndSectionStatus() const 
{
  if (IsDone()==FilletSurf_IsNotOk) 
    throw StdFail_NotDone("FilletSurf_Builder::StartSectionStatus");
  return  myIntBuild.EndSectionStatus(); 
}

//=======================================================================
//function : Simulate 
//purpose  :  computes only the sections used in the computation of the fillet
//=======================================================================
void FilletSurf_Builder::Simulate()
{
  if (myisdone==FilletSurf_IsOk) {
    myIntBuild.Simulate();
    
    if (myIntBuild.Done()) myisdone=FilletSurf_IsOk;
    else { myisdone=FilletSurf_IsNotOk;
	   myerrorstatus=FilletSurf_PbFilletCompute;}
  }
} 

//=======================================================================
//function : NbSection 
//purpose  :  gives the number of sections relative to SurfaceFillet(IndexSurf) 
//=======================================================================
Standard_Integer FilletSurf_Builder::NbSection(const Standard_Integer IndexSurf) const 
{
  if (IsDone()==FilletSurf_IsNotOk) 
    throw StdFail_NotDone("FilletSurf_Builder::NbSection)");
  else if ( (IndexSurf<1)||(IndexSurf>NbSurface())) throw Standard_OutOfRange("FilletSurf_Builder::NbSection");
  return myIntBuild.NbSection(IndexSurf);
}

//=======================================================================
//function : Section 
//purpose  :  gives the   arc of circle corresponding    to section number 
// IndexSec  of  SurfaceFillet(IndexSurf)  (The   basis curve  of the 
// trimmed curve is a Geom_Circle)
//=======================================================================
void FilletSurf_Builder::Section(const Standard_Integer IndexSurf,
				 const Standard_Integer IndexSec,
				 Handle(Geom_TrimmedCurve)& Circ) const 
{
  if ((IndexSurf<1)||(IndexSurf>NbSurface())) 
    throw Standard_OutOfRange("FilletSurf_Builder::Section NbSurface");

  else if ((IndexSec<1)||(IndexSec>NbSection(IndexSurf))) 
    throw Standard_OutOfRange("FilletSurf_Builder::Section NbSection");

  else myIntBuild.Section(IndexSurf, IndexSec,Circ);
}
