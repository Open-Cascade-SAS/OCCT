// File:	HLRBRep_EdgeData.cxx
// Created:	Thu Apr 17 11:44:47 1997
// Author:	Christophe MARION
//		<cma@partox.paris1.matra-dtv.fr>

#include <HLRBRep_EdgeData.ixx>
#include <BRep_Tool.hxx>

//=======================================================================
//function : EdgeData
//purpose  : 
//=======================================================================

HLRBRep_EdgeData::HLRBRep_EdgeData () :
       myFlags(0),
       myHideCount(0)
{ Selected(Standard_True); }

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void HLRBRep_EdgeData::Set (const Standard_Boolean Rg1L,
			    const Standard_Boolean RgNL,
			    const TopoDS_Edge& EG,
			    const Standard_Integer V1,
			    const Standard_Integer V2,
			    const Standard_Boolean Out1,
			    const Standard_Boolean Out2,
			    const Standard_Boolean Cut1,
			    const Standard_Boolean Cut2,
			    const Standard_Real Start,
			    const Standard_ShortReal TolStart,
			    const Standard_Real End,
			    const Standard_ShortReal TolEnd)
{ 
  Rg1Line(Rg1L);
  RgNLine(RgNL);
  Used(Standard_False);
  ChangeGeometry().Curve(EG);
  myTolerance = (Standard_ShortReal)(BRep_Tool::Tolerance(EG));
  VSta(V1);
  VEnd(V2);
  OutLVSta(Out1);
  OutLVEnd(Out2);
  CutAtSta(Cut1);
  CutAtEnd(Cut2);
  Status().Initialize
    (Start,(Standard_ShortReal)(ChangeGeometry().Curve().Resolution
				((Standard_Real)TolStart)),
     End  ,(Standard_ShortReal)(ChangeGeometry().Curve().Resolution
				((Standard_Real)TolEnd  )));
}

//=======================================================================
//function : UpdateMinMax
//purpose  : 
//=======================================================================

void HLRBRep_EdgeData::UpdateMinMax (const Standard_Address TotMinMax)
{
  myMinMax[ 0] = ((Standard_Integer*)TotMinMax)[ 0];
  myMinMax[ 1] = ((Standard_Integer*)TotMinMax)[ 1];
  myMinMax[ 2] = ((Standard_Integer*)TotMinMax)[ 2];
  myMinMax[ 3] = ((Standard_Integer*)TotMinMax)[ 3];
  myMinMax[ 4] = ((Standard_Integer*)TotMinMax)[ 4];
  myMinMax[ 5] = ((Standard_Integer*)TotMinMax)[ 5];
  myMinMax[ 6] = ((Standard_Integer*)TotMinMax)[ 6];
  myMinMax[ 7] = ((Standard_Integer*)TotMinMax)[ 7];
  myMinMax[ 8] = ((Standard_Integer*)TotMinMax)[ 8];
  myMinMax[ 9] = ((Standard_Integer*)TotMinMax)[ 9];
  myMinMax[10] = ((Standard_Integer*)TotMinMax)[10];
  myMinMax[11] = ((Standard_Integer*)TotMinMax)[11];
  myMinMax[12] = ((Standard_Integer*)TotMinMax)[12];
  myMinMax[13] = ((Standard_Integer*)TotMinMax)[13];
  myMinMax[14] = ((Standard_Integer*)TotMinMax)[14];
  myMinMax[15] = ((Standard_Integer*)TotMinMax)[15];
}

