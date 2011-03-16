/***********************************************************************


************************************************************************/

// for the class
#include <Visual3d_LayerItem.ixx>

//
//-Constructors
//
Visual3d_LayerItem::Visual3d_LayerItem() :
MyIsNeedToRecompute(Standard_False)
{
}

void Visual3d_LayerItem::ComputeLayerPrs()
{
  SetNeedToRecompute( Standard_False );
}

void Visual3d_LayerItem::RedrawLayerPrs()
{
  if ( IsNeedToRecompute() )
    ComputeLayerPrs();
}

Standard_Boolean Visual3d_LayerItem::IsNeedToRecompute() const
{
  return MyIsNeedToRecompute;
}

void Visual3d_LayerItem::SetNeedToRecompute( const Standard_Boolean flag )
{
  MyIsNeedToRecompute = flag;
}
