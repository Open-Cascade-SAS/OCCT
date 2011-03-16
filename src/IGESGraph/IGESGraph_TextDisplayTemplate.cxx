//--------------------------------------------------------------------
//
//  File Name : IGESGraph_TextDisplayTemplate.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_TextDisplayTemplate.ixx>
#include <gp_GTrsf.hxx>


    IGESGraph_TextDisplayTemplate::IGESGraph_TextDisplayTemplate ()    {  }


    void IGESGraph_TextDisplayTemplate::Init
  (const Standard_Real                  aWidth,
   const Standard_Real                  aHeight,
   const Standard_Integer               aFontCode,
   const Handle(IGESGraph_TextFontDef)& aFontEntity,
   const Standard_Real                  aSlantAngle,
   const Standard_Real                  aRotationAngle,
   const Standard_Integer               aMirrorFlag,
   const Standard_Integer               aRotationFlag,
   const gp_XYZ&                        aCorner)
{
  theBoxWidth      = aWidth;
  theBoxHeight     = aHeight;    
  theFontCode      = aFontCode;    
  theFontEntity    = aFontEntity;      
  theSlantAngle    = aSlantAngle;   
  theRotationAngle = aRotationAngle;  
  theMirrorFlag    = aMirrorFlag; 
  theRotateFlag    = aRotationFlag;     
  theCorner        = aCorner;             
  InitTypeAndForm(312,FormNumber());  // FormNumber 0-1 : Incremental status
}

    void  IGESGraph_TextDisplayTemplate::SetIncremental (const Standard_Boolean F)
{
  InitTypeAndForm(312, (F ? 1 : 0));
}


    Standard_Real IGESGraph_TextDisplayTemplate::BoxWidth () const
{
  return theBoxWidth;
}

    Standard_Real IGESGraph_TextDisplayTemplate::BoxHeight () const
{
  return theBoxHeight;
}

    Standard_Boolean IGESGraph_TextDisplayTemplate::IsFontEntity () const
{
  return (! theFontEntity.IsNull());
}

    Standard_Integer IGESGraph_TextDisplayTemplate::FontCode () const
{
  return theFontCode;
}

    Handle(IGESGraph_TextFontDef) IGESGraph_TextDisplayTemplate::FontEntity () const
{
  return theFontEntity;
}

    Standard_Real IGESGraph_TextDisplayTemplate::SlantAngle () const
{
  return theSlantAngle;
}

    Standard_Real IGESGraph_TextDisplayTemplate::RotationAngle () const
{
  return theRotationAngle;
}

    Standard_Integer IGESGraph_TextDisplayTemplate::MirrorFlag () const
{
  return theMirrorFlag;
}

    Standard_Integer IGESGraph_TextDisplayTemplate::RotateFlag () const
{
  return theRotateFlag;
}

    Standard_Boolean IGESGraph_TextDisplayTemplate::IsIncremental () const
{
  return ( FormNumber() == 1 );
}

    gp_Pnt IGESGraph_TextDisplayTemplate::StartingCorner () const
{
  return ( gp_Pnt(theCorner) );
}

    gp_Pnt IGESGraph_TextDisplayTemplate::TransformedStartingCorner () const
{
  gp_XYZ TempXYZ = theCorner;
  if (HasTransf()) Location().Transforms(TempXYZ);
  return ( gp_Pnt(TempXYZ) );
}
