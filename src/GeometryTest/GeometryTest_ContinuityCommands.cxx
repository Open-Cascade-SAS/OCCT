// File:	Test.cxx
// Created:	Wed Jul 31 10:35:38 1996
// Author:	Herve LOUESSARD
//		<hl1@sgi29>



#include <LocalAnalysis.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <LocalAnalysis_CurveContinuity.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <DrawTrSurf.hxx>
#include <GeometryTest.hxx>
#include <GeomAbs_Shape.hxx>
#include <Draw_Appli.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>

/*********************************************************************************/
void  InitEpsSurf(Standard_Real& epsnl,Standard_Real& epsdis, Standard_Real& epsangk1, 
			       Standard_Real& epsangk2, Standard_Real& epsangn1, 
			       Standard_Real& perce,Standard_Real& maxlen )
{ 
  epsnl  = 0.001;
  epsdis = 0.001;
  epsangk1 = 0.001 ; 
  epsangk2 = 0.001;
  epsangn1 = 0.001 ; 
  perce = 0.01;
  maxlen = 10000 ;
}

/*********************************************************************************/
void InitEpsCurv(Standard_Real&  epsnl ,Standard_Real&  epsdis, Standard_Real& epsangk1, 
			       Standard_Real& epsangk2, Standard_Real& epsangg1,
			       Standard_Real& epsangg2,Standard_Real& percent,
                               Standard_Real& maxlen)
{ 
  epsnl  = 0.001 ;
  epsdis = 0.001 ; 
  epsangk1= 0.001 ; 
  epsangk2= 0.001; 
  epsangg1= 0.001 ;
  epsangg2= 0.001 ; 
  percent= 0.01;
  maxlen= 10000; 
}			   
/*********************************************************************************/

static Standard_Integer surfaceCcontinuity (Draw_Interpretor& di, Standard_Integer n, const char** a)
{ Standard_Real U1, U2, V1, V2, u1, u2, v1, v2;
  GeomAbs_Shape TypeCont; 
  Standard_Integer ord;
  Standard_Boolean b1, b2, b3, b4;
  Standard_Real epsnl,epsC0,epsC1,epsC2,epsG1,maxlen,perce;
  if (n<8) return 1;
 
  InitEpsSurf(epsnl,epsC0,epsC1,epsC2,epsG1,perce,maxlen);

  Handle(Geom_Surface) surf1 = DrawTrSurf::GetSurface(a[2]);
  if (surf1.IsNull()) return 1 ;
  
  Handle(Geom_Surface) surf2 = DrawTrSurf::GetSurface(a[5]);
  if (surf2.IsNull()) return 1;
   
      U1 = atof(a[3]);
      U2 = atof(a[6]);
      V1 = atof(a[4]); 
      V2 = atof(a[7]); 

      ord = atoi(a[1]);

      surf1->Bounds(u1, u2, v1, v2);
      b1 = ((((U1>=u1)&&(U1<=u2))||((U1<=u1)&&(U1>=u2))));
      b3 = ((((V1>=v1)&&(V1<=v2))||((V1<=v1)&&(V1>=v2))));

      surf2->Bounds(u1, u2, v1, v2);
      b2 = ((((U2>=u1)&&(U2<=u2))||((U2<=u1)&&(U2>=u2))));
      b4 = ((((V2>=v1)&&(V2<=v2))||((V2<=v1)&&(V2>=v2))));
     
      if (!((b1 && b2)&&(b3 && b4))) return 1;

 
       switch ( ord )
       { case 0 : { TypeCont=GeomAbs_C0;
                    switch(n)
			{ case 10  : epsC0= atof(a[9]);
			  case 9   : epsnl = atof(a[8]);
			  case 8 : {} break;
			  default : return 1;
			} 
	            LocalAnalysis_SurfaceContinuity RES (surf1, U1, V1, surf2, U2, V2, 
                    TypeCont,epsnl,epsC0,epsC1,epsC2,epsG1,perce,maxlen);
		    //LocalAnalysis::Dump(RES,cout);
		    Standard_SStream aSStream;
		    LocalAnalysis::Dump(RES,aSStream);
		    di << aSStream;
		  } 
		  break;
	 case 1 : { TypeCont=GeomAbs_C1;
                     switch(n)
			{ case 11 : epsC1=atof(a[10]); 
                          case 10  : epsC0= atof(a[9]);
			  case 9  : epsnl = atof(a[8]);
			  case 8  : {} break;
			  default : return 1;
			} 
	            LocalAnalysis_SurfaceContinuity RES (surf1, U1, V1, surf2, U2, V2, 
                    TypeCont,epsnl,epsC0,epsC1,epsC2,epsG1,perce,maxlen );
		    //LocalAnalysis::Dump(RES,cout);
		    Standard_SStream aSStream;
		    LocalAnalysis::Dump(RES,aSStream);
		    di << aSStream;
     
		  } 
		  break;
	 case 2 : { TypeCont=GeomAbs_C2;
                    switch(n)
		       {
                          case 12 : epsC2= atof(a[11]); 
                          case 11 : epsC1=atof(a[10]); 
                          case 10 : epsC0= atof(a[9]);
			  case 9  : epsnl = atof(a[8]);
			  case 8  : {} break;
			  default : return 1;
			} 
	            LocalAnalysis_SurfaceContinuity RES (surf1, U1, V1, surf2, U2, V2, 
                           TypeCont, epsnl,epsC0,epsC1,epsC2,epsG1,perce,maxlen  );
                    //LocalAnalysis::Dump(RES,cout);    
		    Standard_SStream aSStream;
                    LocalAnalysis::Dump(RES,aSStream);    
		    di << aSStream;
		  }
		  break;

	 
        default : {} 
       }
   return 0; 
}

/*********************************************************************************/

static Standard_Integer surfaceGcontinuity (Draw_Interpretor& di, Standard_Integer n, const char** a)
{ Standard_Real U1, U2, V1, V2,u1, u2, v1, v2;
  GeomAbs_Shape TypeCont; 
  Standard_Integer ord;
  Standard_Boolean b1, b2, b3, b4;
  Standard_Real epsnl,epsC0,epsC1,epsC2,epsG1,maxlen,perce;
  
  if (n<8) return 1;
  InitEpsSurf(epsnl,epsC0,epsC1,epsC2,epsG1,perce,maxlen);

  Handle(Geom_Surface) surf1 = DrawTrSurf::GetSurface(a[2]);
  if (surf1.IsNull()) return 1 ;
  
  Handle(Geom_Surface) surf2 = DrawTrSurf::GetSurface(a[5]);
  if (surf2.IsNull()) return 1;
   
      U1 = atof(a[3]);
      U2 = atof(a[6]);
      V1 = atof(a[4]); 
      V2 = atof(a[7]); 

      ord = atoi(a[1]);

      surf1->Bounds(u1, u2, v1, v2);
      b1 = ((((U1>=u1)&&(U1<=u2))||((U1<=u1)&&(U1>=u2))));
      b3 = ((((V1>=v1)&&(V1<=v2))||((V1<=v1)&&(V1>=v2))));

      surf2->Bounds(u1, u2, v1, v2);
      b2 = ((((U2>=u1)&&(U2<=u2))||((U2<=u1)&&(U2>=u2))));
      b4 = ((((V2>=v1)&&(V2<=v2))||((V2<=v1)&&(V2>=v2))));
     
      if (!((b1 && b2)&&(b3 && b4))) return 1;

 
       switch ( ord )
       { 
	 case 1 : { TypeCont=GeomAbs_G1;
                     switch(n)
			 { case 11 : epsG1=atof(a[10]); 
                          case 10  : epsC0= atof(a[9]);
			  case 9  : epsnl = atof(a[8]);
			  case 8  : {} break;
			  default : return 1;
			} 
	           LocalAnalysis_SurfaceContinuity RES (surf1, U1, V1, surf2, U2, V2, 
                      TypeCont,epsnl,epsC0,epsC1,epsC2,epsG1,perce,maxlen );
		    //LocalAnalysis::Dump(RES,cout);
		    Standard_SStream aSStream;
		    LocalAnalysis::Dump(RES,aSStream);
		    di << aSStream;
    
		  } 
		  break;
	 case 2 : { TypeCont=GeomAbs_G2;
                    switch(n)
		      {   case 13 : maxlen =atof (a[12]);
                          case 12 :  perce=atof(a[11]); 
                          case 11 : epsG1=atof(a[10]); 
                          case 10 : epsC0= atof(a[9]);
			  case 9  : epsnl = atof(a[8]);
			  case 8  : {} break;
			  default : return 1;
			} 
	           LocalAnalysis_SurfaceContinuity RES (surf1, U1, V1, surf2, U2, V2, 
                          TypeCont, epsnl,epsC0,epsC1,epsC2,epsG1,perce,maxlen  ); 
		    //LocalAnalysis::Dump(RES,cout);
		    Standard_SStream aSStream;
		    LocalAnalysis::Dump(RES,aSStream);
		    di << aSStream;
		  }
		  break;

	 
        default : {} 
       }
   return 0;  
}

/*********************************************************************************/

static Standard_Integer curveGcontinuity(Draw_Interpretor& di, Standard_Integer n, const char** a)
{ Standard_Real U1, U2, u1, u2;
  GeomAbs_Shape  TypeCont; 
  Standard_Integer ord;
  Standard_Boolean b1, b2;
  Standard_Real  epsnl,epsC0, epsC1, epsC2, epsG1, epsG2, maxlen,percent;
  if (n<6) return 1; 
  
  Handle(Geom_Curve) curv1 = DrawTrSurf::GetCurve(a[2]);
  if (curv1.IsNull()) return 1;
  Handle(Geom_Curve) curv2 = DrawTrSurf::GetCurve(a[4]);
  if (curv2.IsNull()) return 1;

  U1 = atof(a[3]);
  U2 = atof(a[5]);
 
  ord = atoi(a[1]);

  u1=curv1->FirstParameter();
  u2=curv1->LastParameter();
  b1 = ((((U1>=u1)&&(U1<=u2))||((U1<=u1)&&(U1>=u2))));
   
  u1=curv2->FirstParameter();
  u2=curv2->LastParameter();   
  b2 = ((((U2>=u1)&&(U2<=u2))||((U2<=u1)&&(U2>=u2))));
     
  if (!(b1 && b2)) return 1;

  InitEpsCurv( epsnl,epsC0, epsC1, epsC2, epsG1, epsG2, percent,maxlen);
  switch ( ord )
       { 
	  case 1 : {
                    switch(n)
			{ case 9 :  epsG1=atof(a[8]); 
                          case 8  : epsC0= atof(a[7]);
			  case 7  : epsnl = atof(a[6]);
			  case 6  : {} break;
			  default : return 1;
			}                         
                    TypeCont=GeomAbs_G1;
	            LocalAnalysis_CurveContinuity RES (curv1, U1, curv2, U2, TypeCont, 
                    epsnl,epsC0, epsC1, epsC2, epsG1,epsG2,percent,maxlen  );
                    //LocalAnalysis::Dump(RES,cout); 
		    Standard_SStream aSStream;
                    LocalAnalysis::Dump(RES,aSStream); 
		    di << aSStream;
		  } 
		  break;
	 case 2 : { 
                    TypeCont=GeomAbs_G2;
                    switch(n)
		       {case 12 :maxlen =atof(a[11]);
			case  11 :percent=atof(a[10]);
                        case 10 : epsG2= atof(a[9]); 
                        case 9 : epsG1=atof(a[8]); 
                        case 8  : epsC0= atof(a[7]);
			case 7  : epsnl = atof(a[6]);
			case 6  : {} break;
			default : return 1;
			} 
	             LocalAnalysis_CurveContinuity RES (curv1, U1, curv2, U2, TypeCont,
                         epsnl,epsC0, epsC1, epsC2, epsG1, epsG2,percent,maxlen  );
                     //LocalAnalysis::Dump(RES,cout);   
		    Standard_SStream aSStream;
		    LocalAnalysis::Dump(RES,aSStream);   
		    di << aSStream;
		  }
		  break; 
        default : {}
       }
    return 0; 
    }

/*********************************************************************************/

static Standard_Integer curveCcontinuity(Draw_Interpretor& di, Standard_Integer n, const char** a)
{ Standard_Real U1, U2, u1, u2;
  GeomAbs_Shape  TypeCont; 
  Standard_Integer ord;
  Standard_Boolean b1, b2;
  Standard_Real  epsnl,epsC0, epsC1, epsC2, epsG1, epsG2, maxlen,percent;
  if (n<6) return 1; 
  
  Handle(Geom_Curve) curv1 = DrawTrSurf::GetCurve(a[2]);
  if (curv1.IsNull()) return 1;
  Handle(Geom_Curve) curv2 = DrawTrSurf::GetCurve(a[4]);
  if (curv2.IsNull()) return 1;

   U1 = atof(a[3]);
   U2 = atof(a[5]);
 
   ord = atoi(a[1]);

   u1=curv1->FirstParameter();
   u2=curv1->LastParameter();
   b1 = ((((U1>=u1)&&(U1<=u2))||((U1<=u1)&&(U1>=u2))));
   
   u1=curv2->FirstParameter();
   u2=curv2->LastParameter();   
   b2 = ((((U2>=u1)&&(U2<=u2))||((U2<=u1)&&(U2>=u2))));
     
   if (!(b1 && b2)) return 1;


   InitEpsCurv( epsnl,epsC0, epsC1, epsC2, epsG1, epsG2, percent,maxlen);
   switch ( ord )
       { case 0 : {  switch(n)
			{ case 8  : epsC0= atof(a[7]);
			  case 7  : epsnl = atof(a[6]);
			  case 6  : {} break;
			  default : return 1;
			}                     
                      TypeCont=GeomAbs_C0;
	              LocalAnalysis_CurveContinuity  RES (curv1, U1, curv2, U2, TypeCont,
                       epsnl,epsC0, epsC1, epsC2, epsG1,epsG2,percent,maxlen  );
		     //LocalAnalysis::Dump(RES,cout);
		     Standard_SStream aSStream;
		     LocalAnalysis::Dump(RES,aSStream);
		     di << aSStream;

		   }  break;
	 case 1 : { switch(n)
			{ case 9 : epsC1 =atof(a[8]);
                          case 8  : epsC0= atof(a[7]);
			  case 7  : epsnl = atof(a[6]);
			  case 6  : {} break;
			  default : return 1;
			}                          
                    TypeCont=GeomAbs_C1;
	            LocalAnalysis_CurveContinuity RES (curv1, U1, curv2, U2, TypeCont, 
                    epsnl,epsC0, epsC1, epsC2, epsG1,epsG2,percent,maxlen  );
                    //LocalAnalysis::Dump(RES,cout); 
		    Standard_SStream aSStream;
                    LocalAnalysis::Dump(RES,aSStream); 
		    di << aSStream;
	           
		  } break;
	 case 2 : {  TypeCont=GeomAbs_C2;
                     switch(n)
		       {
                          case 10 : epsC2= atof(a[9]); 
                          case 9  : epsC1=atof(a[8]); 
                          case 8  : epsC0= atof(a[7]);
			  case 7  : epsnl = atof(a[6]);
			  case 6  : {} break;
			  default : return 1;
			} 
	            LocalAnalysis_CurveContinuity RES (curv1, U1, curv2, U2, TypeCont, 
                    epsnl,epsC0, epsC1, epsC2, epsG1, epsG2,percent,maxlen  );
                    //LocalAnalysis::Dump(RES,cout);   
		    Standard_SStream aSStream;
                    LocalAnalysis::Dump(RES,aSStream);   
		    di << aSStream;
		  }
		  break; 
        default : {}
       }
    return 0;
    }
/***************************************************************************/
void GeometryTest::ContinuityCommands( Draw_Interpretor& theCommands)
{
  static Standard_Boolean loaded = Standard_False;
  if ( loaded) return;
  loaded = Standard_True;

  DrawTrSurf::BasicCommands(theCommands);
  const char* g;

  g = "GEOMETRY curves and surfaces continuity analysis ";



  theCommands.Add("surfaceCcontinuity",
		  " surfaceCcontinuity   order surf1 parU1 parV1 surf2 parU2 parV2  [eps_nul[ epsC0 [epsC1 [epsC2]]]]",  
		  __FILE__,
		  surfaceCcontinuity ,g);

  theCommands.Add("surfaceGcontinuity",
		  " surfaceGcontinuity   order surf1 parU1 parV1 surf2 parU2 parV2  [eps_nul[ epsG0 [epsG1[percent [maxlen]]]]",  
		  __FILE__,
		  surfaceGcontinuity ,g);  

  theCommands.Add("curveCcontinuity",
		  "curveCcontinuity   order curv1 u1  curv2  u2   [epsnul  [epsC0 [epsC1  [epsC2 ]]]]  ",
		  __FILE__,
		  curveCcontinuity,g);

 
  theCommands.Add("curveGcontinuity",
		  "curveGcontinuity   order  curv1 u1  curv2  u2   [epsnul  [epsG0  [epsG1  [epsG2 [percent  [maxlen ]]]]]] ",
		   __FILE__,
		  curveGcontinuity,g);
}








