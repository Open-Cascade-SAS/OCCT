// File:	TopOpeBRepTool_ShapeExplorer.cxx
// Created:	Thu Jul 13 17:26:39 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>


#include <TopOpeBRepTool_ShapeExplorer.ixx>
#include <TopAbs.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>


//=======================================================================
//function : TopOpeBRepTool_ShapeExplorer
//purpose  : 
//=======================================================================

TopOpeBRepTool_ShapeExplorer::TopOpeBRepTool_ShapeExplorer() : 
   myIndex(0),
   myNbShapes(0)
{
}


//=======================================================================
//function : TopOpeBRepTool_ShapeExplorer
//purpose  : 
//=======================================================================

TopOpeBRepTool_ShapeExplorer::TopOpeBRepTool_ShapeExplorer
   (const TopoDS_Shape& S, 
    const TopAbs_ShapeEnum ToFind, 
    const TopAbs_ShapeEnum ToAvoid)
{
  Init(S,ToFind,ToAvoid);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  TopOpeBRepTool_ShapeExplorer::Init(const TopoDS_Shape& S, 
					 const TopAbs_ShapeEnum ToFind, 
					 const TopAbs_ShapeEnum ToAvoid)
{
  myExplorer.Init(S,ToFind,ToAvoid);

#ifdef DEB
  myIndex = myNbShapes = 0;
  for (;More();Next()) myNbShapes++;
  myExplorer.ReInit();
  if (More()) myIndex = 1;
#endif

}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean  TopOpeBRepTool_ShapeExplorer::More()const 
{
  Standard_Boolean b = myExplorer.More(); 
  return b;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void  TopOpeBRepTool_ShapeExplorer::Next()
{
  myExplorer.Next();
#ifdef DEB
  myIndex++;
#endif
}

//=======================================================================
//function : Current
//purpose  : 
//=======================================================================

const TopoDS_Shape&  TopOpeBRepTool_ShapeExplorer::Current()const 
{
  const TopoDS_Shape& S = myExplorer.Current();
  return S;
}


//=======================================================================
//function : NbShapes
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_ShapeExplorer::NbShapes()const 
{
  Standard_Integer n = 0;
#ifdef DEB
  n = myNbShapes;
#endif
  return n;
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_ShapeExplorer::Index()const 
{
  Standard_Integer n = 0;
#ifdef DEB
  n = myIndex;
#endif
  return n;
}
 
//=======================================================================
//function : DumpCurrent
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepTool_ShapeExplorer::DumpCurrent
(Standard_OStream& OS)const 
{
#ifdef DEB
  if ( More() ) { 
    const TopoDS_Shape& S = Current();
    TopAbs_ShapeEnum    T = S.ShapeType();
    TopAbs_Orientation  O = S.Orientation();
    Standard_Integer    I = Index();
    TopAbs::Print(T,cout);
    cout<<"("<<I<<","; TopAbs::Print(O,cout); cout<<") ";
  }
#endif
  return OS;
}
