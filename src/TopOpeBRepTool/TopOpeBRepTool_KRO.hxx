// File:	TopOpeBRepTool_KRO.hxx
// Created:	Tue Oct  1 17:31:34 1996
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#ifndef _TopOpeBRepTool_KRO_HeaderFile
#define _TopOpeBRepTool_KRO_HeaderFile
#ifdef DEB
#include <OSD_Chronometer.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_OStream.hxx>

// POP pour NT
class TOPKRO:
public OSD_Chronometer{
public:
  TOPKRO(const TCollection_AsciiString& n)
    :myname(n),mystart(0),mystop(0){myname.RightJustify(30,' ');}
  virtual void Start(){mystart=1;OSD_Chronometer::Start();}
  virtual void Stop(){OSD_Chronometer::Stop();mystop=1;}
  void Print(Standard_OStream& OS){Standard_Real s;Show(s);OS<<myname<<" : ";
				   if(!mystart)OS<<"(inactif)";else{OS<<s<<" secondes";if(!mystop)OS<<" (run)";}}
private:
  TCollection_AsciiString myname;Standard_Integer mystart,mystop;
};
#endif
#endif
