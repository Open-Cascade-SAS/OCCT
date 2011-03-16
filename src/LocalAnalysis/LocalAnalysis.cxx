// File:	LocalAnalysis.cxx
// Created:	Mon Sep  9 15:01:49 1996
// Author:	Herve LOUESSARD
//		<hl1@sgi38>

#include<LocalAnalysis.ixx>
#include<LocalAnalysis_SurfaceContinuity.hxx>
#include<LocalAnalysis_CurveContinuity.hxx>


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































