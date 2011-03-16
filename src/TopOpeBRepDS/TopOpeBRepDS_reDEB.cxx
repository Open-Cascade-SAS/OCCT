// File:	TopOpeBRepDS_reDEB.cxx
// Created:	Sat Feb 14 15:53:57 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>

#include <TopOpeBRepDS_reDEB.hxx>
#include <TopOpeBRepDS_define.hxx>

#ifdef DEB
Standard_EXPORT Standard_Boolean DSREDUEDGETRCE(const Standard_Integer EIX) {
  Standard_Boolean b1 = TopOpeBRepDS_GettracePEI();
  Standard_Boolean b2 = TopOpeBRepDS_GettracePI();
  Standard_Boolean b3 = TopOpeBRepDS_GettraceSPSX(EIX);
  return (b1 || b2 || b3);
}
Standard_EXPORT void debreducerE(const Standard_Integer ie)
{cout<<"+++ debreducerE e"<<ie<<endl;}
Standard_EXPORT void debreducerEP(const Standard_Integer ie,const Standard_Integer ip)
{cout<<"+++ debreducerPEI e"<<ie<<" P"<<ip<<endl;}
Standard_EXPORT void debreducerEV(const Standard_Integer ie,const Standard_Integer iv)
{cout<<"+++ debreducerVEI e"<<ie<<" V"<<iv<<endl;}
Standard_EXPORT void debreducer3d(const Standard_Integer ie)
{cout<<"+++ debreducer3d e"<<ie<<endl;}
Standard_EXPORT void debredpvg(const Standard_Integer ie)
{cout<<"+++ debredpvg e"<<ie<<endl;}
#endif
