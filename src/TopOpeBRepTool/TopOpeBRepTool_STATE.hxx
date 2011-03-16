// File:	TopOpeBRepTool_STATE.hxx
// Created:	Wed Oct 22 16:36:31 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#ifndef _TopOpeBRepTool_STATE_HeaderFile
#define _TopOpeBRepTool_STATE_HeaderFile

#ifdef DEB

#include <Standard_Type.hxx>
#include <TopAbs_State.hxx>

// -----------------------------------------------------------------------
// TopOpeBRepTool_STATE : class of 4 booleans matching TopAbs_State values
// -----------------------------------------------------------------------

class TopOpeBRepTool_STATE {

 public:
  TopOpeBRepTool_STATE(const char* name, 
		       const Standard_Boolean b = Standard_False);
  void Set(const Standard_Boolean b);
  void Set(const TopAbs_State S, const Standard_Boolean b);
  void Set(const Standard_Boolean b, Standard_Integer n, char** a);
  Standard_Boolean Get(const TopAbs_State S);
  Standard_Boolean Get() { return myonetrue; }
  void Print();

 private:
  Standard_Boolean myin,myout,myon,myunknown;
  Standard_Boolean myonetrue;
  char myname[100];

};

// #define DEB
#endif

// #define _TopOpeBRepTool_STATE_HeaderFile
#endif
