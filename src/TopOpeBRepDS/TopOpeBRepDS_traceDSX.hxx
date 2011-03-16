// File:	TopOpeBRepDS_traceDSX.hxx
// Created:	Wed Oct 22 18:57:02 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#ifndef _TopOpeBRepDS_traceDSX_HeaderFile
#define _TopOpeBRepDS_traceDSX_HeaderFile

#ifdef DEB

#include <TopOpeBRepDS_define.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <TCollection_AsciiString.hxx>
#include <MMgt_TShared.hxx>

class TopOpeBRepDS_traceDS : public MMgt_TShared {
public:
  TopOpeBRepDS_traceDS();
  virtual Standard_Integer Nelem() const;
  void SetHDS(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  const Handle(TopOpeBRepDS_HDataStructure)& GetHDS() const;
  void Set(const Standard_Integer i, const Standard_Boolean b);
  void Set(const Standard_Boolean b, Standard_Integer n, char** a);
  Standard_Boolean GetI(const Standard_Integer i) const;
  void Allocate();
protected: 
  Handle_TopOpeBRepDS_HDataStructure myHDS;
  Standard_Integer myne; Handle_TColStd_HArray1OfBoolean myhe;
};

class TopOpeBRepDS_traceCURVE : public TopOpeBRepDS_traceDS {  
public:
  TopOpeBRepDS_traceCURVE();
  Standard_Integer Nelem() const;
};

class TopOpeBRepDS_traceSHAPE : public TopOpeBRepDS_traceDS {
public: 
  TopOpeBRepDS_traceSHAPE();
  Standard_Integer Nelem() const;
  void SetSS(const TopoDS_Shape&,const TopoDS_Shape&);
  void SetSSHDS(const TopoDS_Shape&,const TopoDS_Shape&,const Handle(TopOpeBRepDS_HDataStructure)&);
  Standard_Integer Index(const TopoDS_Shape&) const;
  Standard_Boolean GetS(const TopoDS_Shape&) const;
private:
  Standard_Integer myns;
};

// #ifdef DEB
#endif

// #define _TopOpeBRepDS_traceDSX_HeaderFile
#endif
