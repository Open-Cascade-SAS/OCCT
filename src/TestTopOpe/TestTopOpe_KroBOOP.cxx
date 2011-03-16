// File:	TestTopOpe_KroBOOP.cxx
// Created:	Wed Oct 14 18:21:06 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>

#ifdef DEB
#include <TopOpeBRepTool_KRO.hxx>
//Standard_IMPORT extern TOPKRO KRO_DSFILLER_TOTAL;
Standard_IMPORT TOPKRO KRO_DSFILLER_TOTAL;
//Standard_IMPORT extern TOPKRO KRO_DSFILLER_INTFF;
Standard_IMPORT TOPKRO KRO_DSFILLER_INTFF;
//Standard_IMPORT extern TOPKRO KRO_DSFILLER_INTEE;
Standard_IMPORT TOPKRO KRO_DSFILLER_INTEE;
//Standard_IMPORT extern TOPKRO KRO_DSFILLER_INTFE;
Standard_IMPORT TOPKRO KRO_DSFILLER_INTFE;
//Standard_IMPORT extern TOPKRO KRO_CURVETOOL_APPRO;
void PrintKRO_PREPA() {
  KRO_DSFILLER_TOTAL.Print(cout); cout<<endl;
  KRO_DSFILLER_INTFF.Print(cout); cout<<endl;
  KRO_DSFILLER_INTEE.Print(cout); cout<<endl;
  KRO_DSFILLER_INTFE.Print(cout); cout<<endl;
//  KRO_CURVETOOL_APPRO.Print(cout); cout<<endl;
}
#endif
