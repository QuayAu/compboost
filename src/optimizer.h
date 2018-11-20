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

#ifndef OPTIMIZER_H_
#define OPTIMIZER_H_

#include <iostream>
#include <map>
#include <limits>

#include <RcppArmadillo.h>

#include "baselearner.h"
#include "baselearner_factory_list.h"
#include "loss.h"
#include "line_search.h"

namespace optimizer {

// -------------------------------------------------------------------------- //
// Abstract 'Optimizer' class:
// -------------------------------------------------------------------------- //

class Optimizer
{
  public:
    
    virtual blearner::Baselearner* findBestBaselearner (const std::string&, 
      std::shared_ptr<response::Response>, const blearner_factory_map&) const = 0;
    
    // loss, target, model_prediction, base_learner_prediction (prediction of newly selected base-learner)
    virtual void calculateStepSize (loss::Loss*, std::shared_ptr<response::Response>, const arma::vec&) = 0;
    virtual std::vector<double> getStepSize () const = 0;
    virtual double getStepSize (const unsigned int&) const = 0;

    virtual ~Optimizer ();

  protected:
    
    blearner_factory_map my_blearner_factory_map;
    std::vector<double> step_sizes;

};

// -------------------------------------------------------------------------- //
// Optimizer implementations:
// -------------------------------------------------------------------------- //

// Coordinate Descent:
// -------------------------------------------
class OptimizerCoordinateDescent : public Optimizer
{
  public:
    
    // No special initialization necessary:
    OptimizerCoordinateDescent ();

    blearner::Baselearner* findBestBaselearner (const std::string&, std::shared_ptr<response::Response>, 
      const blearner_factory_map&) const;

    void calculateStepSize (loss::Loss*, std::shared_ptr<response::Response>, const arma::vec&);
    std::vector<double> getStepSize () const;
    double getStepSize (const unsigned int&) const;
};

// Coordinate Descent with line search:
// -------------------------------------------
class OptimizerCoordinateDescentLineSearch : public Optimizer
{
  public:

    // No special initialization necessary:
    OptimizerCoordinateDescentLineSearch ();

    blearner::Baselearner* findBestBaselearner (const std::string&, 
      std::shared_ptr<response::Response>, const blearner_factory_map&) const;

    void calculateStepSize (loss::Loss*, std::shared_ptr<response::Response>, const arma::vec&);
    std::vector<double> getStepSize () const;
    double getStepSize (const unsigned int&) const;  
};


} // namespace optimizer

#endif // OPTIMIZER_H_
