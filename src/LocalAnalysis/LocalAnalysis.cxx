// Created on: 1996-09-09
// Created by: Herve LOUESSARD
// Copyright (c) 1996-1999 Matra Datavision
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


#include <LocalAnalysis.hxx>
#include <LocalAnalysis_CurveContinuity.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>

/*********************************************************************************/
/*********************************************************************************/
void LocalAnalysis::Dump(const LocalAnalysis_SurfaceContinuity& surfconti, Standard_OStream& o)
{ if (!surfconti.IsDone()) 
      { o<<"Problem in the computation "<<endl;
        if (surfconti.StatusError()==LocalAnalysis_NullFirstDerivative) 
           o<<"one of the first derivatives is null" <<endl;
        else if(surfconti.StatusError()==LocalAnalysis_NullSecondDerivative)
            o<<"one of the  second derivatives is null" <<endl; 
        else if(surfconti.StatusError()==LocalAnalysis_NormalNotDefined) 
            o<<"one (or both) normal is undefined" <<endl;
        else if(surfconti.StatusError()==LocalAnalysis_CurvatureNotDefined) 
            o<<"one of the mean curvatures is undefined" <<endl; } 
  else 
  switch(surfconti.ContinuityStatus())
    { case GeomAbs_C0 :{  if (surfconti.IsC0() )
                             o<<" Continuity Status : C0 " <<endl;
                          else o<<" Continuity Status : No C0 "<<endl; 
                          o<<" C0Value = "<<surfconti.C0Value() <<endl;
		       } break; 
      case GeomAbs_C1 :{ if (surfconti.IsC1()) 
			   o<<" Continuity Status : C1 " <<endl;
                         else 
                           {if (surfconti.IsC0() ) 
			        o<<" Continuity Status : C0 " <<endl;
                             else o<<" Continuity Status : NoC0 "<<endl;
                           }  
			 o<<" C0Value = "<<surfconti.C0Value()<<endl;
			 o<<" C1UAngle = "<<surfconti.C1UAngle()<<endl;
                         o<<" C1URatio = "<<surfconti.C1URatio()<<endl; 
			 o<<" C1VAngle = "<<surfconti.C1VAngle()<<endl;
                         o<<" C1VRatio = "<<surfconti.C1VRatio()<<endl;
                         
		       } break;  
      case GeomAbs_C2 :{  if (surfconti.IsC2()) 
			   o<<" Continuity Status : C2 " <<endl;
                         else 
                           {if (surfconti.IsC1() )
                                o<<" Continuity Status : C1 " <<endl;
                            else
			       { if (surfconti.IsC0() )
                                  o<<" Continuity Status : C0 " <<endl;
                                else o<<" Continuity Status : NoC0 "<<endl;}
                           } 
                         
			 o<<" C0Value = "<<surfconti.C0Value()<<endl;
			 o<<" C1UAngle = "<<surfconti.C1UAngle()<<endl;
			 o<<" C1VAngle = "<<surfconti.C1VAngle()<<endl;
			 o<<" C2UAngle = "<<surfconti.C2UAngle()<<endl;
			 o<<" C2VAngle = "<<surfconti.C2VAngle()<<endl;
			 o<<" C1URatio = "<<surfconti.C1URatio()<<endl;
			 o<<" C1VRatio = "<<surfconti.C1VRatio()<<endl;
			 o<<" C2URatio = "<<surfconti.C2URatio()<<endl;
			 o<<" C2VRatio = "<<surfconti.C2VRatio()<<endl;
		       } break; 
      case GeomAbs_G1 :{ if (surfconti.IsG1()) 
			   o<<" Continuity Status : G1 " <<endl;
                          else 
                           {if (surfconti.IsC0() ) 
			        o<<" Continuity Status : G0 " <<endl;
                            else o<<" Continuity Status : NoG0 "<<endl;
                           }  
			 o<<" G0Value = "<<surfconti.C0Value()<<endl; 
			 o<<" G1Angle = "<<surfconti.G1Angle()<<endl<<endl;
		       } break;
      case GeomAbs_G2 :{ if (surfconti.IsG2()) 
			   o<<" Continuity Status : G2 " <<endl;
                         else 
                           {if (surfconti.IsG1() )
                                o<<" Continuity Status : G1 " <<endl;
                            else
			       { if (surfconti.IsC0() )
                                  o<<" Continuity Status : G0 " <<endl;
                                else o<<" Continuity Status : NoG0 "<<endl;}
                           } 
			 o<<" G0Value = "<<surfconti.C0Value()<<endl; 
			 o<<" G1Value = "<<surfconti.G1Angle()<<endl;
			 o<<" G2CurvatureGap = "<<surfconti.G2CurvatureGap()<<endl;
		   	} break;  

      default         : {}
		       } 
}

/*********************************************************************************/

void LocalAnalysis::Dump(const LocalAnalysis_CurveContinuity& curvconti, Standard_OStream& o)
{ if (!curvconti.IsDone()) 
      { o<<"Problem in the computation "<<endl;
        if (curvconti.StatusError()==LocalAnalysis_NullFirstDerivative) 
           o<<"one (or both) first derivative is null" <<endl;
        else if(curvconti.StatusError()==LocalAnalysis_NullSecondDerivative)
            o<<"one (or both) second derivative is null" <<endl; 
        else if(curvconti.StatusError()==LocalAnalysis_TangentNotDefined)
             o<<"one (or both) tangent is undefined " <<endl; 
        else if(curvconti.StatusError()==LocalAnalysis_NormalNotDefined) 
             o<<"one (or both) normal is undefined" <<endl;}  
  else     
  switch(curvconti.ContinuityStatus())
    { case GeomAbs_C0 :{  if (curvconti.IsC0() )
                             o<<" Continuity Status : C0 " <<endl;
                          else o<<" Continuity Status : No C0 "<<endl; 
                          o<<" C0Value = "<<curvconti.C0Value() <<endl;
		       } break; 
      case GeomAbs_C1 :{ if (curvconti.IsC1()) 
			   o<<" Continuity Status : C1 " <<endl;
                         else 
                           {if (curvconti.IsC0() ) 
			        o<<" Continuity Status : C0 " <<endl;
                             else o<<" Continuity Status : NoC0 "<<endl;
                           }  
			 o<<" C0Value = "<<curvconti.C0Value()<<endl;
			 o<<" C1Angle = "<<curvconti.C1Angle()<<endl;
                         o<<" C1Ratio = "<<curvconti.C1Ratio()<<endl;
                               
                      } break; 
      case GeomAbs_C2 :{ if (curvconti.IsC2()) 
			   o<<" Continuity Status : C2 " <<endl;
                         else 
                           {if (curvconti.IsC1() )
                                o<<" Continuity Status : C1 " <<endl;
                            else
			       { if (curvconti.IsC0() )
                                  o<<" Continuity Status : C0 " <<endl;
                                else o<<" Continuity Status : NoC0 "<<endl;}
                           } 
			 o<<" C0Value = "<<curvconti.C0Value()<<endl;
			 o<<" C1Angle = "<<curvconti.C1Angle()<<endl;
			 o<<" C2Angle = "<<curvconti.C2Angle()<<endl;
			 o<<" C1Ratio = "<<curvconti.C1Ratio()<<endl;
			 o<<" C2Ratio = "<<curvconti.C2Ratio()<<endl;
		       }break; 
                         
      case GeomAbs_G1 :{  if (curvconti.IsG1()) 
			   o<<" Continuity Status : G1 " <<endl;
                          else 
                           {if (curvconti.IsC0() ) 
			        o<<" Continuity Status : G0 " <<endl;
                            else o<<" Continuity Status : NoG0 "<<endl;
                           }  
			 o<<" G0Value = "<<curvconti.C0Value()<<endl; 
			 o<<" G1Angle = "<<curvconti.G1Angle()<<endl;
		       } break;
      case GeomAbs_G2 :{ if (curvconti.IsG2()) 
			   o<<" Continuity Status : G2 " <<endl;
                         else 
                           {if (curvconti.IsG1() )
                                o<<" Continuity Status : G1 " <<endl;
                            else
			       { if (curvconti.IsC0() )
                                  o<<" Continuity Status : G0 " <<endl;
                                else o<<" Continuity Status : NoG0 "<<endl;}
                           } 
			 o<<" G0Value = "<<curvconti.C0Value()<<endl; 
			 o<<" G1Angle = "<<curvconti.G1Angle()<<endl;
			 o<<" G2Angle = "<<curvconti.G2Angle()<<endl;
			 o<<" Relative curvature variation = "<<curvconti.G2CurvatureVariation()<<endl;

		       } break;  

      default         : {}
		       } 
}

/*********************************************************************************/
/*********************************************************************************/































