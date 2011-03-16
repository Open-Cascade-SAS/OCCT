#include <Prs3d_Projector.ixx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax3.hxx>


Prs3d_Projector::Prs3d_Projector (const HLRAlgo_Projector& HLPr): MyProjector(HLPr)
{}



Prs3d_Projector::Prs3d_Projector (const Standard_Boolean Pers,
				const Quantity_Length Focus,
			        const Quantity_Length DX,
			        const Quantity_Length DY,
			        const Quantity_Length DZ,
			        const Quantity_Length XAt,
			        const Quantity_Length YAt,
			        const Quantity_Length ZAt,
			        const Quantity_Length XUp,
			        const Quantity_Length YUp,
			        const Quantity_Length ZUp) 
{
  gp_Pnt At (XAt,YAt,ZAt);
  gp_Dir Zpers (DX,DY,DZ);
  gp_Dir Ypers (XUp,YUp,ZUp);
  gp_Dir Xpers = Ypers.Crossed(Zpers);
  gp_Ax3 Axe (At, Zpers, Xpers);
  gp_Trsf T;
  T.SetTransformation(Axe);
  MyProjector = HLRAlgo_Projector(T,Pers,Focus);
}


HLRAlgo_Projector Prs3d_Projector::Projector () const 
{
  return MyProjector;
}
