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

#include "demmler_reinsch.h"


namespace demrei
{

// compute the smoothing parameter
double getLambda (const arma::mat X, const arma::mat W, const arma::mat D, const arma::mat y, int df)
{
  
  // Calculate Everything needed
  arma::mat W2 = arma::sqrt(W);
  arma::mat R = arma::chol(X.t() * W2 * W2 * X);
  R = arma::inv(R);
  arma::mat L = R.t() * D * R;
  arma::vec s = arma::svd(L);

  auto lamfun = [](double x, arma::vec s, int df)
  {
    double out = 0;
    for(int i = 0; i<s.n_cols; i++)
    {
    out = out + 1/(1+x*s(i));
    }
    out = out - df;
    return out;
  };
    
  // FIXME 
  // now we only need to find the largest root of lamfun
  double root = 0;
  
  return root;
}

// compute the degrees of freedom
double getDF (const arma::mat X, const arma::mat W, const arma::mat D, const arma::mat y, double lambda)
{
  arma::mat W2 = arma::sqrt(W);
  arma::mat R = arma::chol(X.t() * W2 * W2 * X);
  R = arma::inv(R);
  arma::mat L = R.t() * D * R;
  arma::vec s = arma::svd(L);

  double df = 0;

  for(int i = 0; i<s.n_cols; i++)
  {
    df = df + 1/(1+lambda*s(i));
  }

  return df;
}

}





