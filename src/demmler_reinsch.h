// ========================================================================== //
//                                 ___.                          __           //
//        ____  ____   _____ ______\_ |__   ____   ____  _______/  |_         //
//      _/ ___\/  _ \ /     \\____ \| __ \ /  _ \ /  _ \/  ___/\   __\        //
//      \  \__(  <_> )  Y Y  \  |_> > \_\ (  <_> |  <_> )___ \  |  |          //
//       \___  >____/|__|_|  /   __/|___  /\____/ \____/____  > |__|          //
//           \/            \/|__|       \/                  \/                //
//                                                                            //
// ========================================================================== //
//
// Compboost is free software: you can redistribute it and/or modify
// it under the terms of the MIT License.
// Compboost is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// MIT License for more details. You should have received a copy of
// the MIT License along with compboost.
//
// =========================================================================== #
#ifndef DEMMLER_H_
#define DEMMLER_H_

#include "RcppArmadillo.h"
#include <boost/math/tools/toms748_solve.hpp>
#include <functional> // functional

namespace demrei
{
double getLambda (const arma::mat, const arma::mat, const arma::mat, const arma::mat, int);
double calculateDegreesOfFreedom (const double&, const arma::vec&, const double&);
double findLambdaWithToms748 (const arma::vec&, const double&, const double&, const double&);
double demmlerReinsch (const arma::mat&, const arma::mat&, const double&);
double getDF (const arma::mat, const arma::mat, const arma::mat, const arma::mat, double);
} // namespace tensors

# endif // DEMMLER_H_
