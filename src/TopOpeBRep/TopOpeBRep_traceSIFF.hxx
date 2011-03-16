// File:	TopOpeBRep_traceSIFF.hxx
// Created:	Wed Oct 22 18:57:02 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#ifndef _TopOpeBRep_traceSIFF_HeaderFile
#define _TopOpeBRep_traceSIFF_HeaderFile

#ifdef DEB

#include <TopOpeBRepTool_define.hxx>
#include <Standard_OStream.hxx>
#include <TopoDS_Shape.hxx>

class TopOpeBRep_traceSIFF {
public:

  TopOpeBRep_traceSIFF();

  void Reset();

  void Set(const Standard_Boolean b,
	   Standard_Integer n,
	   char**a);

  void Set(const TCollection_AsciiString& brep1,
	   const TCollection_AsciiString& brep2,
	   const TCollection_AsciiString& n);
  
  TCollection_AsciiString Name1(const Standard_Integer I) const;
  TCollection_AsciiString Name2(const Standard_Integer I) const;
  
  const TCollection_AsciiString& File() const;
  
  Standard_Boolean Start(const TCollection_AsciiString& s,
			 Standard_OStream& OS);
  void Add(const Standard_Integer I1,
	   const Standard_Integer I2);
  void End(const TCollection_AsciiString& s,
	   Standard_OStream& OS);
private: 
  TCollection_AsciiString mybrep1,mybrep2,myfilename;
  filebuf myfilebuf;
  Standard_Boolean myopen;
};

// #ifdef DEB
#endif
// #define _TopOpeBRep_traceSIFF_HeaderFile
#endif
