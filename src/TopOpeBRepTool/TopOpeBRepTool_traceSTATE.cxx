// File:	TopOpeBRepTool.cxx
// Created:	Thu Mar 10 12:13:09 1994
// Author:	Jean Yves LEBEY
//		<jyl@phobox>

#ifdef DEB

#include <Standard_Type.hxx>
#include <TopOpeBRepTool_STATE.hxx>

static TopOpeBRepTool_STATE TopOpeBRepTool_CL3DDR("draw 3d classification states");
static TopOpeBRepTool_STATE TopOpeBRepTool_CL3DPR("print 3d classification states");
static TopOpeBRepTool_STATE TopOpeBRepTool_CL2DDR("draw 2d classification states");
static TopOpeBRepTool_STATE TopOpeBRepTool_CL2DPR("print 2d classification states");

Standard_EXPORT void TopOpeBRepTool_SettraceCL3DDR
  (const Standard_Boolean b, Standard_Integer narg, char** a) 
{ TopOpeBRepTool_CL3DDR.Set(b,narg,a); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL3DDR(const TopAbs_State S) 
{ return TopOpeBRepTool_CL3DDR.Get(S); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL3DDR()
{ return TopOpeBRepTool_CL3DDR.Get(); }

Standard_EXPORT void TopOpeBRepTool_SettraceCL3DPR
  (const Standard_Boolean b, Standard_Integer narg, char** a) 
{ TopOpeBRepTool_CL3DPR.Set(b,narg,a); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL3DPR(const TopAbs_State S) 
{ return TopOpeBRepTool_CL3DPR.Get(S); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL3DPR()
{ return TopOpeBRepTool_CL3DPR.Get(); }

Standard_EXPORT void TopOpeBRepTool_SettraceCL2DDR
  (const Standard_Boolean b, Standard_Integer narg, char** a) 
{ TopOpeBRepTool_CL2DDR.Set(b,narg,a); }
Standard_EXPORT Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL2DDR(const TopAbs_State S) 
{ return TopOpeBRepTool_CL2DDR.Get(S); }
Standard_EXPORT Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL2DDR()
{ return TopOpeBRepTool_CL2DDR.Get(); }

Standard_EXPORT void TopOpeBRepTool_SettraceCL2DPR
  (const Standard_Boolean b, Standard_Integer narg, char** a) 
{ TopOpeBRepTool_CL2DPR.Set(b,narg,a); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL2DPR(const TopAbs_State S) 
{ return TopOpeBRepTool_CL2DPR.Get(S); }
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceCL2DPR() 
{ return TopOpeBRepTool_CL2DPR.Get(); }

// #ifdef DEB
#endif
