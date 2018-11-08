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
// Written by:
// -----------
//
//   Daniel Schalk
//   Department of Statistics
//   Ludwig-Maximilians-University Munich
//   Ludwigstrasse 33
//   D-80539 München
//
//   https://www.compstat.statistik.uni-muenchen.de
//
//   Contact
//   e: contact@danielschalk.com
//   w: danielschalk.com
//
// =========================================================================== #

#ifndef DEMMLER_REINSCH_CPP_
#define DEMMLER_REINSCH_CPP_

#include <RcppArmadillo.h>
#include <boost/math/tools/toms748_solve.hpp>
#include <functional>

namespace demmlerreinsch {

double calculateDegreesOfFreedom (const double&, const arma::vec&, const double&);
double findLambdaWithToms748 (const arma::vec&, const double&, const double& = 0., const double& = 1e15) 
double demmlerReinsch (const arma::mat&, const arma::mat&, const double&);

} // namespace demmlerreinsch

# endif // DEMMLER_REINSCH_CPP_