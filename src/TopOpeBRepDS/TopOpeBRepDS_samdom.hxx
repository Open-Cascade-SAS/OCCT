// File:	TopOpeBRepDS_samdom.hxx
// Created:	Mon Jun 15 18:08:23 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>


#ifndef _TopOpeBRepDS_samdom_HeaderFile
#define _TopOpeBRepDS_samdom_HeaderFile

#include <TopOpeBRepDS_define.hxx>

Standard_EXPORT void FDSSDM_prepare(const Handle(TopOpeBRepDS_HDataStructure)&);
Standard_EXPORT void FDSSDM_makes1s2(const TopoDS_Shape& S,TopTools_ListOfShape& L1,TopTools_ListOfShape& L2);
Standard_EXPORT void FDSSDM_s1s2makesordor(const TopTools_ListOfShape& L1,const TopTools_ListOfShape& L2,const TopoDS_Shape& S,TopTools_ListOfShape& LSO,TopTools_ListOfShape& LDO);
Standard_EXPORT Standard_Boolean FDSSDM_hass1s2(const TopoDS_Shape& S);
Standard_EXPORT void FDSSDM_s1s2(const TopoDS_Shape& S,TopTools_ListOfShape& LS1,TopTools_ListOfShape& LS2);
Standard_EXPORT void FDSSDM_sordor(const TopoDS_Shape& S,TopTools_ListOfShape& LSO,TopTools_ListOfShape& LDO);
Standard_EXPORT Standard_Boolean  FDSSDM_contains(const TopoDS_Shape& S,const TopTools_ListOfShape& L);
Standard_EXPORT void FDSSDM_copylist(const TopTools_ListOfShape& Lin,const Standard_Integer I1,const Standard_Integer I2,TopTools_ListOfShape& Lou);
Standard_EXPORT void FDSSDM_copylist(const TopTools_ListOfShape& Lin,TopTools_ListOfShape& Lou);

#endif
