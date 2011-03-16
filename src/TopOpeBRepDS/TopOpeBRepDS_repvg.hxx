// File:	TopOpeBRepDS_regpv.hxx
// Created:	Sat Feb 14 15:53:39 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>

#ifndef _TopOpeBRepDS_regpv_HeaderFile
#define _TopOpeBRepDS_regpv_HeaderFile

#include <TopOpeBRepDS_define.hxx>
Standard_EXPORT void FDS_repvg
(const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer EIX,const TopOpeBRepDS_Kind GT,TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_ListOfInterference& reducedLI);
Standard_EXPORT void FDS_repvg1
(const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer EIX,const TopOpeBRepDS_Kind GT,TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_ListOfInterference& reducedLI);
Standard_EXPORT void FDS_repvg2
(const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer EIX,const TopOpeBRepDS_Kind GT,TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_ListOfInterference& reducedLI);
#endif
