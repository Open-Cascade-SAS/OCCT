// File:	TopOpeBRepDS_reDEB.hxx
// Created:	Sat Feb 14 15:53:51 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>

#ifndef _TopOpeBRepDS_reDEB_HeaderFile
#define _TopOpeBRepDS_reDEB_HeaderFile

#ifdef DEB
#include <TopOpeBRepDS_define.hxx>
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettracePEI();
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettracePI();
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSPSX(const Standard_Integer);
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceEDPR();
Standard_EXPORT Standard_Boolean DSREDUEDGETRCE(const Standard_Integer);
Standard_EXPORT void debreducerE(const Standard_Integer);
Standard_EXPORT void debreducerEP(const Standard_Integer);
Standard_EXPORT void debreducerEV(const Standard_Integer,const Standard_Integer);
Standard_EXPORT void debreducer3d(const Standard_Integer);
Standard_EXPORT void debredpvg(const Standard_Integer);
#endif

#endif
