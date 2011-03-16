// File:	IntImpParGen_Tool.hxx
// Created:	Wed Jun 10 15:04:00 1992
// Author:	Laurent BUCHARD
//		<lbr@sdsun2>


#ifndef IntImpParGen_Tool_HeaderFile
#define IntImpParGen_Tool_HeaderFile

#ifndef IntRes2d_Domain_HeaderFile
#include <IntRes2d_Domain.hxx>
#endif

#ifndef IntRes2d_Position_HeaderFile
#include <IntRes2d_Position.hxx>
#endif

#ifndef IntRes2d_Transition_HeaderFile
#include <IntRes2d_Transition.hxx>
#endif

#ifndef gp_Vec2d_HeaderFile
#include <gp_Vec2d.hxx>
#endif

#ifndef gp_Pnt2d_HeaderFile
#include <gp_Pnt2d.hxx>
#endif

 
Standard_Real NormalizeOnDomain(Standard_Real&,const IntRes2d_Domain&);

void Determine_Position(IntRes2d_Position&,
			const IntRes2d_Domain&,
			const gp_Pnt2d&,const Standard_Real);
  
void Determine_Transition(const IntRes2d_Position Pos1,
			  gp_Vec2d&               Tan1,
			  const gp_Vec2d&         Norm1,
			  IntRes2d_Transition&    Trans1,
			  const IntRes2d_Position Pos2,
			  gp_Vec2d&               Tan2,
			  const gp_Vec2d&         Norm2,
			  IntRes2d_Transition&    Trans2,
			  const Standard_Real     ToleranceAng);

#endif
