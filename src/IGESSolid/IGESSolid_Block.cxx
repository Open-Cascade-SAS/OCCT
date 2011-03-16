//--------------------------------------------------------------------
//
//  File Name : IGESSolid_Block.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_Block.ixx>
#include <gp_GTrsf.hxx>


    IGESSolid_Block::IGESSolid_Block ()    {  }


    void  IGESSolid_Block::Init
  (const gp_XYZ& aSize, const gp_XYZ& aCorner,
   const gp_XYZ& aXAxis, const gp_XYZ& aZAxis)
{
  theSize   = aSize;
  theCorner = aCorner;         // default (0,0,0)
  theXAxis  = aXAxis;          // default (1,0,0)
  theZAxis  = aZAxis;          // default (0,0,1)
  InitTypeAndForm(150,0);
}

    gp_XYZ  IGESSolid_Block::Size () const
{
  return theSize;
}

    Standard_Real  IGESSolid_Block::XLength () const
{
  return theSize.X();
}

    Standard_Real  IGESSolid_Block::YLength () const
{
  return theSize.Y();
}

    Standard_Real  IGESSolid_Block::ZLength () const
{
  return theSize.Z();
}

    gp_Pnt  IGESSolid_Block::Corner () const
{
  return gp_Pnt(theCorner);
}

    gp_Pnt  IGESSolid_Block::TransformedCorner () const
{
  if (!HasTransf()) return gp_Pnt(theCorner);
  else
    {
      gp_XYZ tmp = theCorner;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    gp_Dir  IGESSolid_Block::XAxis () const
{
  return gp_Dir(theXAxis);
}

    gp_Dir  IGESSolid_Block::TransformedXAxis () const
{
  if (!HasTransf()) return gp_Dir(theXAxis);
  else
    {
      gp_XYZ xyz = theXAxis;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(xyz);
      return gp_Dir(xyz);
    }
}

    gp_Dir  IGESSolid_Block::YAxis () const
{
  return gp_Dir(theXAxis ^ theZAxis);     // ^ overloaded
}

    gp_Dir  IGESSolid_Block::TransformedYAxis () const
{
  if (!HasTransf()) return gp_Dir(theXAxis ^ theZAxis);
  else
    {
      gp_XYZ xyz = theXAxis ^ theZAxis;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(xyz);
      return gp_Dir(xyz);
    }
}

    gp_Dir  IGESSolid_Block::ZAxis () const
{
  return gp_Dir(theZAxis);
}

    gp_Dir  IGESSolid_Block::TransformedZAxis () const
{
  if (!HasTransf()) return gp_Dir(theZAxis);
  else
    {
      gp_XYZ xyz(theZAxis);
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(xyz);
      return gp_Dir(xyz);
    }
}
