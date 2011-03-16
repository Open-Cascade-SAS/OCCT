// File:	TestTopOpe_VarsTopo.hxx
// Created:	Mon Feb  5 17:35:20 1996
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#ifndef _TestTopOpe_VarsTopo_HeaderFile
#define _TestTopOpe_VarsTopo_HeaderFile

#include <TopOpeBRepTool_OutCurveType.hxx>
#include <TopOpeBRepTool_define.hxx>

class VarsTopo {

public:

  VarsTopo();
  void Init();
  void SetTolxDef();
  void SetTolx(const Standard_Real tol3x, const Standard_Real tol2x);
  void GetTolx(Standard_Real& tol3x, Standard_Real& tol2x) const;
  void SetToliDef();
  void SetToli(const Standard_Real tolarc, const Standard_Real toltan); 
  void GetToli(Standard_Real& tolarc, Standard_Real& toltan) const; Standard_Boolean GetForceToli() const;
  void SetClear(const Standard_Boolean b); Standard_Boolean GetClear() const;
  void SetOCT(char* namoct);
  void SetOCT(const TopOpeBRepTool_OutCurveType oct);
  TopOpeBRepTool_OutCurveType GetOCT() const;
  void SetC2D(char* namoct);
  void SetC2D(const Standard_Boolean c2d);
  Standard_Boolean GetC2D() const;
  void DumpOCT(Standard_OStream&) const;
  void DumpC2D(Standard_OStream&) const;
  void DumpTolx(Standard_OStream&) const;
  void DumpToli(Standard_OStream&) const;
  void DumpMode(Standard_OStream&) const;
  void DumpVerbose(Standard_OStream&) const;
  void DumpClear(Standard_OStream&) const;
  void Dump(Standard_OStream&) const;
  void SetMode(const Standard_Integer mode);
  Standard_Integer  GetMode() const;
  void SetVerbose(const Standard_Integer mode);
  Standard_Integer  GetVerbose() const;

private:

  TopOpeBRepTool_OutCurveType myOCT;
  Standard_Boolean myC2D;
  Standard_Real mytol3xdef,mytol2xdef;
  Standard_Real mytol3x,mytol2x;
  Standard_Boolean myforcetoli;
  Standard_Real mytolarc, mytoltan;
  Standard_Boolean myclear;
  Standard_Integer myexecmode;
  Standard_Integer myverbmode;

};

#endif
