// File:	XCAFDoc_DocumentTool.cxx
// Created:	Wed Aug 30 15:22:16 2000
// Author:	data exchange team
//		<det@strelox.nnov.matra-dtv.fr>


#include <XCAFDoc_DocumentTool.ixx>
#include <TDataStd_Name.hxx>
#include <TDF_Data.hxx>
#include <TDF_Tool.hxx>
#include <TDF_LabelLabelMap.hxx>

// purpose: give acces to DocumentTool->Label() for static methods
static TDF_LabelLabelMap RootLDocLMap;


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_DocumentTool::GetID() 
{
  static Standard_GUID DocumentToolID ("efd212ec-6dfd-11d4-b9c8-0060b0ee281b");
  return DocumentToolID; 
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_DocumentTool) XCAFDoc_DocumentTool::Set(const TDF_Label& L,
                                                       const Standard_Boolean IsAcces)
{
  Handle(XCAFDoc_DocumentTool) A;
  if (!DocLabel(L).FindAttribute (XCAFDoc_DocumentTool::GetID(), A)) {
    if (!IsAcces) {
      TDF_Label RootL = L.Root();
      if (RootLDocLMap.IsBound(RootL)) RootLDocLMap.UnBind(RootL);
      RootLDocLMap.Bind(RootL, L);
    }
    A = new XCAFDoc_DocumentTool;
    DocLabel(L).AddAttribute(A);
    // set ShapeTool, ColorTool and LayerTool attributes
    XCAFDoc_ShapeTool::Set(ShapesLabel(L));
    XCAFDoc_ColorTool::Set(ColorsLabel(L));
    XCAFDoc_LayerTool::Set(LayersLabel(L));
    XCAFDoc_DimTolTool::Set(DGTsLabel(L));
    XCAFDoc_MaterialTool::Set(MaterialsLabel(L));
  }
  return A;
}


//=======================================================================
//function : DocLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::DocLabel(const TDF_Label& acces) 
{
  TDF_Label DocL, RootL = acces.Root();
  if (RootLDocLMap.IsBound(RootL))
    return RootLDocLMap.Find(RootL);
  
  DocL = RootL.FindChild(1);
  RootLDocLMap.Bind(RootL, DocL);
  return DocL;
}


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

XCAFDoc_DocumentTool::XCAFDoc_DocumentTool()
{
}


//=======================================================================
//function : ShapesLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::ShapesLabel(const TDF_Label& acces) 
{
  TDF_Label L = DocLabel(acces).FindChild(1,Standard_True);
  TDataStd_Name::Set(L, "Shapes");
  return L;
}


//=======================================================================
//function : ColorsLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::ColorsLabel(const TDF_Label& acces) 
{
  TDF_Label L = DocLabel(acces).FindChild(2,Standard_True);
  TDataStd_Name::Set(L, "Colors");
  return L;
}


//=======================================================================
//function : LayersLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::LayersLabel(const TDF_Label& acces) 
{
  TDF_Label L = DocLabel(acces).FindChild(3,Standard_True);
  TDataStd_Name::Set(L, "Layers");
  return L;
}


//=======================================================================
//function : DGTsLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::DGTsLabel(const TDF_Label& acces) 
{
  TDF_Label L = DocLabel(acces).FindChild(4,Standard_True);
  TDataStd_Name::Set(L, "D&GTs");
  return L;
}


//=======================================================================
//function : MaterialsLabel
//purpose  : 
//=======================================================================

TDF_Label XCAFDoc_DocumentTool::MaterialsLabel(const TDF_Label& acces) 
{
  TDF_Label L = DocLabel(acces).FindChild(5,Standard_True);
  TDataStd_Name::Set(L, "Materials");
  return L;
}


//=======================================================================
//function : ShapeTool
//purpose  : 
//=======================================================================

 Handle(XCAFDoc_ShapeTool) XCAFDoc_DocumentTool::ShapeTool(const TDF_Label& acces) 
{
  return XCAFDoc_ShapeTool::Set(ShapesLabel(acces));
}


//=======================================================================
//function : ColorTool
//purpose  : 
//=======================================================================

Handle(XCAFDoc_ColorTool) XCAFDoc_DocumentTool::ColorTool (const TDF_Label& acces) 
{
  return XCAFDoc_ColorTool::Set(ColorsLabel(acces));
}


//=======================================================================
//function : LayerTool
//purpose  : 
//=======================================================================

Handle(XCAFDoc_LayerTool) XCAFDoc_DocumentTool::LayerTool (const TDF_Label& acces) 
{
  return XCAFDoc_LayerTool::Set(LayersLabel(acces));
}


//=======================================================================
//function : DimTolTool
//purpose  : 
//=======================================================================

Handle(XCAFDoc_DimTolTool) XCAFDoc_DocumentTool::DimTolTool(const TDF_Label& acces) 
{
  return XCAFDoc_DimTolTool::Set(DGTsLabel(acces));
}


//=======================================================================
//function : MaterialTool
//purpose  : 
//=======================================================================

Handle(XCAFDoc_MaterialTool) XCAFDoc_DocumentTool::MaterialTool(const TDF_Label& acces) 
{
  return XCAFDoc_MaterialTool::Set(MaterialsLabel(acces));
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_DocumentTool::ID() const
{
  return GetID();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_DocumentTool::Restore(const Handle(TDF_Attribute)& /* with */) 
{
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_DocumentTool::NewEmpty() const
{
  return new XCAFDoc_DocumentTool;
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_DocumentTool::Paste (const Handle(TDF_Attribute)& /* into */,
				  const Handle(TDF_RelocationTable)& /* RT */) const
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void XCAFDoc_DocumentTool::Init() const
{
  TDF_Label DocL = Label(), RootL = DocL.Root();
  if ( ! RootLDocLMap.IsBound(RootL) ) RootLDocLMap.Bind(RootL, DocL);
}


//=======================================================================
//function : IsXCAFDocument
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_DocumentTool::IsXCAFDocument(const  Handle(TDocStd_Document)& D)
{
  return RootLDocLMap.IsBound(D->Main().Root());
}
