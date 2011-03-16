// File GccEnt.cxx, REG 13/08/91

//=========================================================================
//   Methodes de package permettant de qualifier les objets.              +
//                                                                        +
//=========================================================================

#include <GccEnt.ixx>

GccEnt_QualifiedLin
  GccEnt::Unqualified(const gp_Lin2d& Line) {
    return GccEnt_QualifiedLin(Line,GccEnt_unqualified);
  }

GccEnt_QualifiedCirc
  GccEnt::Unqualified(const gp_Circ2d& Circle) {
    return GccEnt_QualifiedCirc(Circle,GccEnt_unqualified);
  }

GccEnt_QualifiedCirc
  GccEnt::Enclosing(const gp_Circ2d& Circle) {
    return GccEnt_QualifiedCirc(Circle,GccEnt_enclosing);
  }

GccEnt_QualifiedLin
  GccEnt::Enclosed(const gp_Lin2d& Line) {
    return GccEnt_QualifiedLin(Line,GccEnt_enclosed);
  }

GccEnt_QualifiedCirc
  GccEnt::Enclosed(const gp_Circ2d& Circle) {
    return GccEnt_QualifiedCirc(Circle,GccEnt_enclosed);
  }

GccEnt_QualifiedCirc
  GccEnt::Outside(const gp_Circ2d& Circle) {
    return GccEnt_QualifiedCirc(Circle,GccEnt_outside);
  }

GccEnt_QualifiedLin
  GccEnt::Outside(const gp_Lin2d& Line) {
    return GccEnt_QualifiedLin(Line,GccEnt_outside);
  }
