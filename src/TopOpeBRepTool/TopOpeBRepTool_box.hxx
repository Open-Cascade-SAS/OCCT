// File:	TopOpeBRepTool_box.hxx
// Created:	Tue Apr  1 11:22:56 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#ifndef _TopOpeBRepTool_box_HeaderFile
#define _TopOpeBRepTool_box_HeaderFile

#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_HBoxTool.hxx>

#ifdef DEB
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceBOX();
#endif

Standard_EXPORT void FBOX_Prepare();
Standard_EXPORT Handle(TopOpeBRepTool_HBoxTool) FBOX_GetHBoxTool();
Standard_EXPORT const Bnd_Box& FBOX_Box(const TopoDS_Shape& S);

#endif
