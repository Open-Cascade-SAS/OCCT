// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <ExprIntrp_GenExp.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

TEST(Expr_GeneralExpression_Test, OCC902_ExpressionDerivative)
{
  // Bug OCC902: Bad derivative computation
  // This test verifies that the derivative of Exp(5*x) is correctly computed
  // The expected result is either "Exp(5*x)*5" or "5*Exp(5*x)"

  TCollection_AsciiString anExpStr("5");
  anExpStr.AssignCat("*x");
  anExpStr.Prepend("Exp(");
  anExpStr.AssignCat(")");

  occ::handle<ExprIntrp_GenExp> anExprIntrp = ExprIntrp_GenExp::Create();

  // Create the expression
  anExprIntrp->Process(anExpStr);

  ASSERT_TRUE(anExprIntrp->IsDone()) << "Expression interpretation should succeed";

  occ::handle<Expr_GeneralExpression> anExpr = anExprIntrp->Expression();
  ASSERT_FALSE(anExpr.IsNull()) << "Expression should not be null";

  occ::handle<Expr_NamedUnknown>      aVar     = new Expr_NamedUnknown("x");
  occ::handle<Expr_GeneralExpression> aNewExpr = anExpr->Derivative(aVar);

  ASSERT_FALSE(aNewExpr.IsNull()) << "Derivative should not be null";

  TCollection_AsciiString aDerivativeStr = aNewExpr->String();

  // The derivative of Exp(5*x) should be either "Exp(5*x)*5" or "5*Exp(5*x)"
  bool isCorrect = (aDerivativeStr == "Exp(5*x)*5") || (aDerivativeStr == "5*Exp(5*x)");

  EXPECT_TRUE(isCorrect) << "Derivative result was: " << aDerivativeStr.ToCString()
                         << ", expected either 'Exp(5*x)*5' or '5*Exp(5*x)'";
}

// Test OCC31697: Expr_GeneralExpression::Derivative for Exp(2*Sin(x^2))
// Expected derivative: Exp(2*Sin(x^2))*Cos(x^2)*x*4
TEST(Expr_GeneralExpression_Test, OCC31697_DerivativeOfComplexExpression)
{
  occ::handle<ExprIntrp_GenExp> anExprIntrp = ExprIntrp_GenExp::Create();
  anExprIntrp->Process(TCollection_AsciiString("Exp(2*Sin(x^2))"));

  ASSERT_TRUE(anExprIntrp->IsDone()) << "Expression parsing should succeed";

  occ::handle<Expr_GeneralExpression> anExpr = anExprIntrp->Expression();
  ASSERT_FALSE(anExpr.IsNull()) << "Expression should not be null";

  occ::handle<Expr_NamedUnknown> aVar = new Expr_NamedUnknown("x");
  ASSERT_TRUE(anExpr->Contains(aVar)) << "Expression should contain variable x";

  occ::handle<Expr_GeneralExpression> aDer = anExpr->Derivative(aVar);
  ASSERT_FALSE(aDer.IsNull()) << "Derivative should not be null";

  const TCollection_AsciiString aDerStr = aDer->String();
  EXPECT_EQ(aDerStr, TCollection_AsciiString("Exp(2*Sin(x^2))*Cos(x^2)*x*4"))
    << "Derivative result was: " << aDerStr.ToCString();
}

// Test OCC22611: ExprIntrp_GenExp must not leak and must parse numeric literal correctly.
// Migrated from QABugs_19.cxx OCC22611
TEST(Expr_GeneralExpression_Test, OCC22611_ParseNumericLiteral)
{
  occ::handle<ExprIntrp_GenExp> aGen = ExprIntrp_GenExp::Create();
  for (int i = 0; i < 10; ++i)
  {
    aGen->Process(TCollection_AsciiString("0.1214343"));
    ASSERT_TRUE(aGen->IsDone()) << "Parsing should succeed on iteration " << i;
    occ::handle<Expr_GeneralExpression> aExpr = aGen->Expression();
    EXPECT_FALSE(aExpr.IsNull()) << "Expression should not be null on iteration " << i;
  }
}
