// Created on: 1998-11-24
// Created by: Xuan PHAM PHU
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>

#define M_FORWARD(sta)  (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

#ifdef DEB
extern TopTools_IndexedMapOfShape STATIC_PURGE_mapv;
extern TopTools_IndexedMapOfOrientedShape STATIC_PURGE_mapeds;
extern Standard_Boolean TopOpeBRepTool_GettracePURGE();
Standard_EXPORT void FUN_REINIT()
{
  STATIC_PURGE_mapv.Clear(); STATIC_PURGE_mapeds.Clear();
}
Standard_EXPORT Standard_Integer FUN_addepc(const TopoDS_Shape& ed,const TopoDS_Shape&)
{
  Standard_Integer ie = STATIC_PURGE_mapeds.Add(ed);
#ifdef DRAW
//  TCollection_AsciiString aa = TCollection_AsciiString("pc_"); FUN_tool_draw(aa,TopoDS::Edge(ed),TopoDS::Face(f),ie);
//  TCollection_AsciiString bb = TCollection_AsciiString("ed_"); FUN_tool_draw(bb,ed,ie);
#endif  
  return ie;
}

Standard_EXPORT Standard_Integer FUN_addcheckepc(const TopoDS_Shape& ed,const TopoDS_Shape& f) {
  Standard_Integer ie = 0;
  ie = STATIC_PURGE_mapeds.FindIndex(ed);
  if (ie == 0) ie = FUN_addepc(ed,f);
  return ie;
}
Standard_IMPORT Standard_Integer FUN_adds(const TopoDS_Shape& s);

Standard_EXPORT void FUN_tool_tori(const TopAbs_Orientation Or)
{
  switch (Or) {
  case TopAbs_FORWARD:
    cout<<"FOR";break;
  case TopAbs_REVERSED:
    cout<<"REV";break;
  case TopAbs_INTERNAL:
    cout<<"INT";break;
  case TopAbs_EXTERNAL:
    cout<<"EXT";break;
  }    
}
#endif

Standard_EXPORT void FUN_tool_trace(const Standard_Integer Index)
{
  if (Index == 1) cout <<"FORWARD ";
  if (Index == 2) cout <<"REVERSED ";
}
Standard_EXPORT void FUN_tool_trace(const gp_Pnt2d p2d)
{
  cout<<" = ("<<p2d.X()<<" "<<p2d.Y()<<")"<<endl;
}
