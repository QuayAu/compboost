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

#include "baselearner_factory.h"
#include "data.h"

namespace blearnerfactory {

// -------------------------------------------------------------------------- //
// Abstract 'BaselearnerFactory' class:
// -------------------------------------------------------------------------- //

// arma::mat BaselearnerFactory::getData () const
// {
//   return data_target->getData();
// }

std::string BaselearnerFactory::getDataIdentifier () const
{
  return data_target->getDataIdentifier();
}

std::string BaselearnerFactory::getBaselearnerType() const
{
  return blearner_type;
}

arma::mat BaselearnerFactory::getPenalty() const
{
    arma::mat penalty_mat = 0 * splines::penaltyMat(data_target->getData().n_cols, 1);
    return penalty_mat;
}

void BaselearnerFactory::initializeDataObjects (std::shared_ptr<data::Data> data_source0,
  std::shared_ptr<data::Data> data_target0)
{
  data_source = data_source0;
  data_target = data_target0;

  // Make sure that the data identifier is setted correctly:
  data_target->setDataIdentifier(data_source->getDataIdentifier());

  // Get the data of the source, transform it and write it into the target:
  data_target->setData(instantiateData(data_source->getData()));
}

BaselearnerFactory::~BaselearnerFactory () {}

// -------------------------------------------------------------------------- //
// BaselearnerFactory implementations:
// -------------------------------------------------------------------------- //

// BaselearnerPolynomial:
// -----------------------

BaselearnerPolynomialFactory::BaselearnerPolynomialFactory (const std::string blearner_type0,
  std::shared_ptr<data::Data> data_source0, std::shared_ptr<data::Data> data_target0, const unsigned int& degree,
  const bool& intercept)
  : degree ( degree ),
    intercept ( intercept )
{
  blearner_type = blearner_type0;

  data_source = data_source0;
  data_target = data_target0;

  // Make sure that the data identifier is setted correctly:
  data_target->setDataIdentifier(data_source->getDataIdentifier());

  // Prepare computation of intercept and slope of an ordinary linear regression:
  if (data_source->getData().n_cols == 1) {
    // Store centered x values for faster computation:
    data_target->setData(arma::pow(data_source->getData(), degree));

    // Hack to store some properties which are reused over and over again:
    arma::mat temp_mat(1, 2, arma::fill::zeros);

    if (intercept) {
      temp_mat(0,0) = arma::as_scalar(arma::mean(data_target->getData()));
    }
    temp_mat(0,1) = arma::as_scalar(arma::sum(arma::pow(data_target->getData() - temp_mat(0,0), 2)));
    data_target->XtX_inv = temp_mat;

  } else {
    // Get the data of the source, transform it and write it into the target:
    data_target->setData(instantiateData(data_source->getData()));
    data_target->XtX_inv = arma::inv(data_target->getData().t() * data_target->getData());
  }

  // blearner_type = blearner_type + " with degree " + std::to_string(degree);
}


BaselearnerPolynomialFactory::BaselearnerPolynomialFactory (const std::string blearner_type0,
  std::shared_ptr<data::Data> data_source0, std::shared_ptr<data::Data> data_target0, 
  arma::field<arma::mat> grid_mat0, arma::mat time_penalty, const unsigned int& degree, const bool& intercept)
  : degree ( degree ),
    intercept ( intercept )
{
  blearner_type = blearner_type0;

  data_source = data_source0;
  data_target = data_target0;
  grid_mat = grid_mat0;
  
  // Make sure that the data identifier is setted correctly:
  data_target->setDataIdentifier(data_source->getDataIdentifier());
  
  // Get the data of the source, transform it and write it into the target
  arma::mat temp = arma::pow(data_source->getData(), degree);
  if (intercept) {
    arma::mat temp_intercept(temp.n_rows, 1, arma::fill::ones);
    temp = join_rows(temp_intercept, temp);
  }
  
   if(grid_mat.n_elem == 1) {
    arma::mat data_kroned = arma::zeros(temp.n_rows,temp.n_cols*grid_mat(0).n_cols);
    
    int grid_n = grid_mat(0).n_rows;
    
    for(int i = 0; i <= temp.n_rows - grid_n; i = i + grid_n) {
      data_kroned.rows(i,(i-1 + grid_n)) = tensors::rowWiseKronecker(grid_mat(0),temp.rows(i,(i-1 + grid_n)));
    }
    temp = data_kroned;

  }

  // Case two: different grids, sepearte t-splines
  if(grid_mat.n_elem > 1) {
    arma::mat data_kroned = arma::mat();
    
    int row_tracker = 0;
    for(int g; g < grid_mat.n_elem; g++){
        int grid_n = grid_mat(g).n_rows;
        arma::mat g_kroned = tensors::rowWiseKronecker(grid_mat(g), temp.rows(row_tracker,(row_tracker-1 + grid_n)));
        data_kroned = arma::join_cols(data_kroned, g_kroned);
        row_tracker = row_tracker + grid_n;
    }
    temp = data_kroned;
  }
  
  data_target->setData(temp);
  
  
  data_target->XtX_inv = arma::inv(temp.t() * temp);
  
  // blearner_type = blearner_type + " with degree " + std::to_string(degree);
}



std::shared_ptr<blearner::Baselearner> BaselearnerPolynomialFactory::createBaselearner (const std::string& identifier)
{
  std::shared_ptr<blearner::Baselearner>  sh_ptr_blearner = std::make_shared<blearner::BaselearnerPolynomial>(data_target, identifier, degree, intercept);
  sh_ptr_blearner->setBaselearnerType(blearner_type);

  // // Check if the data is already set. If not, run 'instantiateData' from the
  // // baselearner:
  // if (! is_data_instantiated) {
  //   data = sh_ptr_blearner->instantiateData();
  //
  //   is_data_instantiated = true;
  //
  //   // update baselearner type:
  //   blearner_type = blearner_type + " with degree " + std::to_string(degree);
  // }
  return sh_ptr_blearner;
}

/**
 * \brief Data getter which always returns an arma::mat
 *
 * This function is important to have a unified interface to access the data
 * matrices. Especially for predicting we have to get the data of each factory
 * as dense matrix. This is a huge drawback in terms of memory usage. Therefore,
 * this function should only be used to get temporary matrices which are deleted
 * when they run out of scope to reduce memory load. Also note that there is a
 * dispatch with the getData() function of the Data objects which are mostly
 * called internally.
 *
 * \returns `arma::mat` of data used for modelling a single base-learner
 */
arma::mat BaselearnerPolynomialFactory::getData () const
{
  // In the case of p = 1 we have to treat the getData() function differently
  // due to the saved and already transformed data without intercept. This
  // is annoying but improves performance of the fitting process.
  if (data_target->getData().n_cols == 1) {
    if (intercept) {
      return instantiateData(arma::pow(data_target->getData(), 1/degree));
    } else {
      return data_target->getData();
    }
  } else {
    return data_target->getData();
  }
}

// Transform data. This is done twice since it makes the prediction
// of the whole compboost object so much easier:
arma::mat BaselearnerPolynomialFactory::instantiateData (const arma::mat& newdata) const
{
  arma::mat temp = arma::pow(newdata, degree);
  if (intercept) {
    arma::mat temp_intercept(temp.n_rows, 1, arma::fill::ones);
    temp = join_rows(temp_intercept, temp);
  }

  // grid_mat is a field of pointers to inMemoryData matrices
  // grid_mat(0)->getData().n_cols 
  // take first element of field (a pointer)
  // dereference pointer, get an inMemoryData obj
  // getData(), receive a matrix
  // Case one: same grid, all on the same t-spline
  if(grid_mat.n_elem == 1) {
    arma::mat data_kroned = arma::zeros(temp.n_rows,temp.n_cols*grid_mat(0).n_cols);
    
    int grid_n = grid_mat(0).n_rows;
    
    for(int i = 0; i <= temp.n_rows - grid_n; i = i + grid_n) {
      data_kroned.rows(i,(i-1 + grid_n)) = tensors::rowWiseKronecker(grid_mat(0),temp.rows(i,(i-1 + grid_n)));
    }
    temp = data_kroned;

  }

  // Case two: different grids, sepearte t-splines
  if(grid_mat.n_elem > 1) {
    arma::mat data_kroned = arma::mat();
    
    int row_tracker = 0;
    for(int g; g < grid_mat.n_elem; g++){
        int grid_n = grid_mat(g).n_rows;
        arma::mat g_kroned = tensors::rowWiseKronecker(grid_mat(g), temp.rows(row_tracker,(row_tracker-1 + grid_n)));
        data_kroned = arma::join_cols(data_kroned, g_kroned);
        row_tracker = row_tracker + grid_n;
    }
    temp = data_kroned;
  }
  
  return temp;
}

arma::mat BaselearnerPolynomialFactory::getPenalty () const
{
  if(intercept){
    arma::mat penalty_mat = arma::mat(2,2,arma::fill::zeros);
    return penalty_mat;
  } else {
    arma::mat penalty_mat = arma::mat(1,1,arma::fill::zeros);
      return penalty_mat;
  }
}

// BaselearnerPSpline:
// -----------------------

/**
 * \brief Default constructor of class `PSplineBlearnerFactory`
 *
 * The PSpline constructor has some important tasks which are:
 *   - Set the knots
 *   - Initialize the data (knots must be setted prior)
 *   - Compute and store penalty matrix
 *
 * \param blearner_type0 `std::string` Name of the baselearner type (setted by
 *   the Rcpp Wrapper classes in `compboost_modules.cpp`)
 * \param data_source `std::shared_ptr<data::Data>` Source of the data
 * \param data_target `std::shared_ptr<data::Data>` Object to store the transformed data source
 * \param degree `unsigned int` Polynomial degree of the splines
 * \param n_knots `unsigned int` Number of inner knots used
 * \param penalty `double` Regularization parameter `penalty = 0` yields
 *   b splines while a bigger penalty forces the splines into a global
 *   polynomial form.
 * \param differences `unsigned int` Number of differences used for the
 *   penalty matrix.
 */

BaselearnerPSplineFactory::BaselearnerPSplineFactory (const std::string& blearner_type0,
  std::shared_ptr<data::Data> data_source0, std::shared_ptr<data::Data> data_target0, const unsigned int& degree,
  const unsigned int& n_knots, const double& penalty, const unsigned int& differences, const unsigned int& df,
  const bool& use_sparse_matrices)
  : degree ( degree ),
    n_knots ( n_knots ),
    penalty ( penalty ),
    differences ( differences ),
    use_sparse_matrices ( use_sparse_matrices ),
    df ( df )
{
  blearner_type = blearner_type0;
  // Set data, data identifier and the data_mat (dense at this stage)
  data_source = data_source0;
  data_target = data_target0;

  // This is necessary to prevent the program from segfolds... whyever???
  // Copied from: http://lists.r-forge.r-project.org/pipermail/rcpp-devel/2012-November/004796.html
  try {
    if (data_source->getData().n_cols > 1) {
      Rcpp::stop("Given data should have just one column.");
    }
  } catch ( std::exception &ex ) {
    forward_exception_to_r( ex );
  } catch (...) {
    ::Rf_error( "c++ exception (unknown reason)" );
  }
  
  // Additionally set the penalty matrix:
  penalty_mat = penalty * splines::penaltyMat(n_knots + (degree + 1), differences);
  
  // Initialize knots:
  data_target->knots = splines::createKnots(data_source->getData(), n_knots, degree);


  // Make sure that the data identifier is setted correctly:
  data_target->setDataIdentifier(data_source->getDataIdentifier());

  // Get the data of the source, transform it and write it into the target. This needs some explanations:
  //   - If we use sparse matrices we want to store the sparse matrix into the sparse data matrix member of
  //     the data object. This also requires to adopt getData() for that purpose.
  //   - To get some (very) nice speed ups we store the transposed matrix not the standard one. This also
  //     affects how the training in baselearner.cpp is done. Nevertheless, this speed up things dramatically.
  if (use_sparse_matrices) {
    data_target->sparse_data_mat = splines::createSparseSplineBasis (data_source->getData(), degree, data_target->knots).t();
    // calculate lambda if df != 0
    if(df == 0){
      data_target->XtX_inv = arma::inv(data_target->sparse_data_mat * data_target->sparse_data_mat.t() + penalty_mat);
    } else{
      arma::mat XtX = arma::mat(data_target->sparse_data_mat * data_target->sparse_data_mat.t());
      penalty_mat = penalty_mat / penalty; // reset penalty mat
      double penalty_DR = demrei::demmlerReinsch(XtX, penalty_mat, df);
      penalty_mat = penalty_DR * penalty_mat;
      Rcpp::Rcout << "Lambda = " << penalty_DR << " determined by df = " << df << " through DR" << std::endl;
      data_target->XtX_inv = arma::inv(XtX + penalty_mat);
    }
  }
}

BaselearnerPSplineFactory::BaselearnerPSplineFactory (const std::string& blearner_type0,
  std::shared_ptr<data::Data> data_source0, std::shared_ptr<data::Data> data_target0,
  arma::field<arma::mat> grid_mat0, arma::mat time_penalty, const unsigned int& degree, const unsigned int& n_knots, 
  const double& penalty, const unsigned int& differences, const unsigned int& df, const bool& use_sparse_matrices)
  : degree ( degree ),
    n_knots ( n_knots ),
    penalty ( penalty ),
    differences ( differences ),
    use_sparse_matrices ( use_sparse_matrices ),
    df ( df )
{
  blearner_type = blearner_type0;
  // Set data, data identifier and the data_mat (dense at this stage)
  data_source = data_source0;
  data_target = data_target0;
  grid_mat = grid_mat0;

  // This is necessary to prevent the program from segfolds... whyever???
  // Copied from: http://lists.r-forge.r-project.org/pipermail/rcpp-devel/2012-November/004796.html
  try {
    if (data_source->getData().n_cols > 1) {
      Rcpp::stop("Given data should have just one column.");
    }
  } catch ( std::exception &ex ) {
    forward_exception_to_r( ex );
  } catch (...) {
    ::Rf_error( "c++ exception (unknown reason)" );
  }
  // Initialize knots:
  data_target->knots = splines::createKnots(data_source->getData(), n_knots, degree);
  // Make sure that the data identifier is setted correctly:
  data_target->setDataIdentifier(data_source->getDataIdentifier());

  // Get the data of the source, transform it and write it into the target. This needs some explanations:
  //   - If we use sparse matrices we want to store the sparse matrix into the sparse data matrix member of
  //     the data object. This also requires to adopt getData() for that purpose.
  //   - To get some (very) nice speed ups we store the transposed matrix not the standard one. This also
  //     affects how the training in baselearner.cpp is done. Nevertheless, this speed up things dramatically.

    // Initialize knots:
  data_target->knots = splines::createKnots(data_source->getData(), n_knots, degree);

    // Make sure that the data identifier is setted correctly:
  data_target->setDataIdentifier(data_source->getDataIdentifier());

    // sparsify grid
  arma::field<arma::sp_mat> grid_mat_sparse = arma::field<arma::sp_mat>(grid_mat.size());

    for(int i=0; i<grid_mat.size(); i++){
    grid_mat_sparse(i) = arma::sp_mat(grid_mat(i));
  }
  
  // Get the data of the source, transform it and write it into the target. This needs some explanations:
  //   - If we use sparse matrices we want to store the sparse matrix into the sparse data matrix member of
  //     the data object. This also requires to adopt getData() for that purpose.
  //   - To get some (very) nice speed ups we store the transposed matrix not the standard one. This also
  //     affects how the training in baselearner.cpp is done. Nevertheless, this speed up things dramatically.
  
  // We will transpose only after we kroneckered the time to the basis
  data_target->sparse_data_mat = splines::createSparseSplineBasis (data_source->getData(), degree, data_target->knots);

      // Create the baselearners penalty matrix
  // here the matrix is not yet rotated!
  if(df == 0){
      penalty_mat = penalty * splines::penaltyMat(data_target->sparse_data_mat.n_cols, differences);
    } else {
      penalty_mat = splines::penaltyMat(data_target->sparse_data_mat.n_cols, differences);
      arma::mat XtX_temp = arma::mat(data_target->sparse_data_mat.t() * data_target->sparse_data_mat);
      double penalty_DR = demrei::demmlerReinsch(XtX_temp, penalty_mat, df);
      penalty_mat = penalty_DR *penalty_mat;
      Rcpp::Rcout << "Lambda = " << penalty_DR << " determined by df = " << df << " through DR" << std::endl;
  }
  
    
  if(grid_mat_sparse.n_elem == 1) {
    arma::sp_mat data_kroned = arma::sp_mat(data_target->sparse_data_mat.n_rows, data_target->sparse_data_mat.n_cols*grid_mat_sparse(0).n_cols);
    arma::sp_mat temp = arma::sp_mat();
    int grid_n = grid_mat_sparse(0).n_rows;
    
    // Vars
    arma::rowvec vecA = arma::rowvec();
    arma::rowvec vecB = arma::rowvec();
    arma::sp_mat vecAsparse = arma::sp_mat();
    arma::sp_mat vecBsparse = arma::sp_mat();
    arma::sp_mat out = arma::sp_mat();
    
    for(int i = 0; i <= data_target->sparse_data_mat.n_rows - grid_n; i = i + grid_n) {
        temp = data_target->sparse_data_mat.rows(i,(i-1 + grid_n));
      
        // Variables
        vecA = arma::rowvec(temp.n_cols, arma::fill::ones);
        vecB = arma::rowvec(grid_mat_sparse(0).n_cols, arma::fill::ones);
        
        arma::sp_mat vecAsparse = arma::sp_mat(vecA);
        arma::sp_mat vecBsparse = arma::sp_mat(vecB);
        // Multiply both kronecker products element-wise
        out = arma::kron(temp,vecBsparse) % arma::kron(vecAsparse, (grid_mat_sparse(0)));
        data_kroned.rows(i,(i-1 + grid_n)) = out;
    }
    // store transposed
    data_target->sparse_data_mat = data_kroned.t();

  }
  
  // Case two: different grids, sepearte t-splines
  if(grid_mat_sparse.n_elem > 1) {
    arma::sp_mat data_kroned = arma::sp_mat();
    arma::sp_mat g_kroned = arma::sp_mat();
    arma::sp_mat temp = arma::sp_mat();
    
    
    // Vars
    arma::rowvec vecA = arma::rowvec();
    arma::rowvec vecB = arma::rowvec();
    arma::sp_mat vecAsparse = arma::sp_mat();
    arma::sp_mat vecBsparse = arma::sp_mat();
    arma::sp_mat out = arma::sp_mat();
    
    int row_tracker = 0;
    for(int g; g < grid_mat_sparse.n_elem; g++){
        int grid_n = grid_mat_sparse(g).n_rows;
        temp = data_target->sparse_data_mat.rows(row_tracker,(row_tracker-1 + grid_n));
        
        // Variables
        vecA = arma::rowvec(temp.n_cols, arma::fill::ones);
        vecB = arma::rowvec(grid_mat_sparse(0).n_cols, arma::fill::ones);
        
        arma::sp_mat vecAsparse = arma::sp_mat(vecA);
        arma::sp_mat vecBsparse = arma::sp_mat(vecB);
      
        // Multiply both kronecker products element-wise
        out = arma::kron(temp,vecBsparse) % arma::kron(vecAsparse, (grid_mat_sparse(0)));
        
        
        data_kroned = arma::join_cols(data_kroned, out);
        row_tracker = row_tracker + grid_n;
    }
    data_target->sparse_data_mat = data_kroned.t();
  }
  
  penalty_mat = tensors::penaltySumKronecker(time_penalty, penalty_mat);
  data_target->XtX_inv = arma::inv(data_target->sparse_data_mat * data_target->sparse_data_mat.t() + penalty_mat);

}





/**
 * \brief Create new `BaselearnerPSpline` object
 *
 * \param identifier `std::string` identifier of that specific baselearner object
 */
std::shared_ptr<blearner::Baselearner> BaselearnerPSplineFactory::createBaselearner (const std::string& identifier)
{
  // Create new polynomial baselearner. This one will be returned by the
  // factory:
  std::shared_ptr<blearner::Baselearner>  sh_ptr_blearner = std::make_shared<blearner::BaselearnerPSpline>(data_target, identifier, degree,
    n_knots, penalty, differences, use_sparse_matrices);
  sh_ptr_blearner->setBaselearnerType(blearner_type);

  // // Check if the data is already set. If not, run 'instantiateData' from the
  // // baselearner:
  // if (! is_data_instantiated) {
  //   data = sh_ptr_blearner->instantiateData();
  //
  //   is_data_instantiated = true;
  //
  //   // update baselearner type:
  //   blearner_type = blearner_type + " with degree " + std::to_string(degree);
  // }
  return sh_ptr_blearner;
}

/**
 * \brief Data getter which always returns an arma::mat
 *
 * This function is important to have a unified interface to access the data
 * matrices. Especially for predicting we have to get the data of each factory
 * as dense matrix. This is a huge drawback in terms of memory usage. Therefore,
 * this function should only be used to get temporary matrices which are deleted
 * when they run out of scope to reduce memory load. Also note that there is a
 * dispatch with the getData() function of the Data objects which are mostly
 * called internally.
 *
 * \returns `arma::mat` of data used for modelling a single base-learner
 */
arma::mat BaselearnerPSplineFactory::getData () const
{
  if (use_sparse_matrices) {
    // std::cout << "Use sparse matrices" << std::endl;
    arma::mat out (data_target->sparse_data_mat.t());
    return out;
  } else {
    // std::cout << "Use dense matrices" << std::endl;
    return data_target->getData();
  }
}

arma::mat BaselearnerPSplineFactory::getPenalty () const
{
  return penalty_mat;
}

/**
 * \brief Instantiate data matrix (design matrix)
 *
 * This function is ment to create the design matrix which is then stored
 * within the data object. This should be done just once and then reused all
 * the time.
 *
 * Note that this function sets the `data_mat` object of the data object!
 *
 * \param newdata `arma::mat` Input data which is transformed to the design matrix
 *
 * \returns `arma::mat` of transformed data
 */
arma::mat BaselearnerPSplineFactory::instantiateData (const arma::mat& newdata) const
{
  
  arma::vec knots = data_target->knots;

  // check if the new data matrix contains value which are out of range:
  double range_min = knots[degree];                   // minimal value from original data
  double range_max = knots[n_knots + degree + 1];     // maximal value from original data

  arma::mat temp = splines::filterKnotRange(newdata, range_min, range_max, data_target->getDataIdentifier());

  // Data object has to be created prior! That means that data_ptr must have
  // initialized knots, and penalty matrix!
  arma::mat out = splines::createSplineBasis (temp, degree, data_target->knots);
  
  return out;
}

/// ---------------------------------------------------------------------------------------------- ///

BaselearnerCombinedFactory::BaselearnerCombinedFactory (const std::string& blearner_type0, 
  std::shared_ptr<blearnerfactory::BaselearnerFactory> blearner_1_in, 
  std::shared_ptr<blearnerfactory::BaselearnerFactory> blearner_2_in)
{
  blearner_type = blearner_type0;
  blearner_1 = blearner_1_in;
  blearner_2 = blearner_2_in;
  
  // Get data from both learners
  arma::mat bl1_mat = blearner_1->getData();
  arma::mat bl2_mat = blearner_2->getData();
  
  arma::mat bl1_pen = blearner_1->getPenalty();
  arma::mat bl2_pen = blearner_2->getPenalty();
  
  // calculate new design matrix and Design Matrix
  arma::mat blc_mat = tensors::rowWiseKronecker(bl1_mat,bl2_mat);
  arma::mat penalty_mat = tensors::penaltySumKronecker(bl1_pen, bl2_pen);
  
  // put into memory
  data_source = std::make_shared<data::InMemoryData>(data::InMemoryData(blc_mat, "combined"));
  data_target = std::make_shared<data::InMemoryData>(data::InMemoryData(blc_mat, "combined"));
  
  data_target->XtX_inv = arma::inv(data_target->getData().t() * data_target->getData() + penalty_mat);
  
}

std::shared_ptr<blearner::Baselearner> BaselearnerCombinedFactory::createBaselearner (const std::string& identifier)
{
  std::shared_ptr<blearner::Baselearner>  sh_ptr_blearner = std::make_shared<blearner::BaselearnerCombined>(data_target, identifier);
  sh_ptr_blearner->setBaselearnerType(blearner_type);
  
  return sh_ptr_blearner;
}

arma::mat BaselearnerCombinedFactory::getPenalty () const
{
  return penalty_mat;
}

std::map<std::string, std::shared_ptr<blearnerfactory::BaselearnerFactory> > BaselearnerCombinedFactory::getFactories () const
{
  std::map<std::string, std::shared_ptr<blearnerfactory::BaselearnerFactory> > out;
  out["bl1"] = blearner_1;
  out["bl2"] = blearner_1;
  
  return out;
}

arma::mat BaselearnerCombinedFactory::getData () const
{
  // In the case of p = 1 we have to treat the getData() function differently
  // due to the saved and already transformed data without intercept. This
  // is annoying but improves performance of the fitting process.
    return data_target->getData();
}

// Transform data. This is done twice since it makes the prediction
// of the whole compboost object so much easier:
arma::mat BaselearnerCombinedFactory::instantiateData (const arma::mat& newdata) const
{
  return newdata;
}

/// ---------------------------------------------------------------------------------------------- ///

BaselearnerCenteredFactory::BaselearnerCenteredFactory (const std::string& blearner_type0, 
  std::shared_ptr<blearnerfactory::BaselearnerFactory> blearner_target0, 
  std::shared_ptr<blearnerfactory::BaselearnerFactory> blearner_center0)
{
  blearner_type = blearner_type0;
  blearner_target = blearner_target0;
  blearner_center = blearner_center0;
  
  // Get data from both learners
  arma::mat bl1_mat = blearner_target->getData();
  arma::mat bl2_mat = blearner_center->getData();
  
  arma::mat bl1_pen = blearner_target->getPenalty();
  
  // calculate new design matrix and Design Matrix
  std::map<std::string, arma::mat>  center_res = tensors::centerDesignMatrix(bl1_mat, bl1_pen, bl2_mat);
  arma::mat blc_mat = center_res["X"];
  // FIXME this might be the wrong penalty mat???
  arma::mat penalty_mat = center_res["P"];
  Z = center_res["Z"];
  
  // put into memory
  data_source = std::make_shared<data::InMemoryData>(data::InMemoryData(blc_mat, "Centered"));
  data_target = std::make_shared<data::InMemoryData>(data::InMemoryData(blc_mat, "Centered"));
  
  data_target->XtX_inv = arma::inv(data_target->getData().t() * data_target->getData() + penalty_mat);
  
}

std::shared_ptr<blearner::Baselearner> BaselearnerCenteredFactory::createBaselearner (const std::string& identifier)
{
  std::shared_ptr<blearner::Baselearner>  sh_ptr_blearner = std::make_shared<blearner::BaselearnerCentered>(data_target, identifier);
  sh_ptr_blearner->setBaselearnerType(blearner_type);
  
  return sh_ptr_blearner;
}

arma::mat BaselearnerCenteredFactory::getPenalty () const
{
  return penalty_mat;
}

arma::mat BaselearnerCenteredFactory::getData () const
{
  // In the case of p = 1 we have to treat the getData() function differently
  // due to the saved and already transformed data without intercept. This
  // is annoying but improves performance of the fitting process.
    return data_target->getData();
}

// Transform data. This is done twice since it makes the prediction
// of the whole compboost object so much easier:
arma::mat BaselearnerCenteredFactory::instantiateData (const arma::mat& newdata) const
{
  
  // Get new data and instantiate like in factory targer
  arma::mat bl1_mat = blearner_target->instantiateData(newdata);
  
  // now rotate data as before, the actual blearner_center is not needed, just the rotation
  arma::mat out = bl1_mat * Z;
  

  return out;
}

/// ---------------------------------------------------------------------------------------------- ///

BaselearnerTargetOnlyFactory::BaselearnerTargetOnlyFactory (const std::string& blearner_type0,
  std::shared_ptr<data::Data> data_source0, std::shared_ptr<data::Data> data_target0)
{
  blearner_type = blearner_type0;
  
  data_source = data_source0;
  data_target = data_target0;
  
  data_target->XtX_inv = arma::inv(data_target->getData().t() * data_target->getData());
  
  // blearner_type = blearner_type + " with degree " + std::to_string(degree);
}

std::shared_ptr<blearner::Baselearner> BaselearnerTargetOnlyFactory::createBaselearner (const std::string& identifier)
{
  std::shared_ptr<blearner::Baselearner>  sh_ptr_blearner = std::make_shared<blearner::BaselearnerTargetOnly>(data_target, identifier);
  sh_ptr_blearner->setBaselearnerType(blearner_type);
  
  return sh_ptr_blearner;
}

arma::mat BaselearnerTargetOnlyFactory::getData () const
{
    return data_target->getData();
}

// Transform data. This is done twice since it makes the prediction
// of the whole compboost object so much easier:
arma::mat BaselearnerTargetOnlyFactory::instantiateData (const arma::mat& newdata) const
{
  return newdata;
}


/// ---------------------------------------------------------------------------------------------- ///


// BaselearnerCustom:
// -----------------------

BaselearnerCustomFactory::BaselearnerCustomFactory (const std::string& blearner_type0,
  std::shared_ptr<data::Data> data_source, std::shared_ptr<data::Data> data_target, Rcpp::Function instantiateDataFun,
  Rcpp::Function trainFun, Rcpp::Function predictFun, Rcpp::Function extractParameter)
  : instantiateDataFun ( instantiateDataFun ),
    trainFun ( trainFun ),
    predictFun ( predictFun ),
    extractParameter ( extractParameter )
{
  blearner_type = blearner_type0;
  initializeDataObjects(data_source, data_target);
}

std::shared_ptr<blearner::Baselearner> BaselearnerCustomFactory::createBaselearner (const std::string &identifier)
{
  std::shared_ptr<blearner::Baselearner> sh_ptr_blearner = std::make_shared<blearner::BaselearnerCustom>(data_target, identifier,
    instantiateDataFun, trainFun, predictFun, extractParameter);
  sh_ptr_blearner->setBaselearnerType(blearner_type);

  // // Check if the data is already set. If not, run 'instantiateData' from the
  // // baselearner:
  // if (! is_data_instantiated) {
  //   data = sh_ptr_blearner->instantiateData();
  //
  //   is_data_instantiated = true;
  // }
  return sh_ptr_blearner;
}

/**
 * \brief Data getter which always returns an arma::mat
 *
 * This function is important to have a unified interface to access the data
 * matrices. Especially for predicting we have to get the data of each factory
 * as dense matrix. This is a huge drawback in terms of memory usage. Therefore,
 * this function should only be used to get temporary matrices which are deleted
 * when they run out of scope to reduce memory load. Also note that there is a
 * dispatch with the getData() function of the Data objects which are mostly
 * called internally.
 *
 * \returns `arma::mat` of data used for modelling a single base-learner
 */
arma::mat BaselearnerCustomFactory::getData () const
{
  return data_target->getData();
}

// Transform data. This is done twice since it makes the prediction
// of the whole compboost object so much easier:
arma::mat BaselearnerCustomFactory::instantiateData (const arma::mat& newdata) const
{
  Rcpp::NumericMatrix out = instantiateDataFun(newdata);
  return Rcpp::as<arma::mat>(out);
}

// BaselearnerCustomCpp:
// -----------------------

BaselearnerCustomCppFactory::BaselearnerCustomCppFactory (const std::string& blearner_type0,
  std::shared_ptr<data::Data> data_source, std::shared_ptr<data::Data> data_target, SEXP instantiateDataFun,
  SEXP trainFun, SEXP predictFun)
  : instantiateDataFun ( instantiateDataFun ),
    trainFun ( trainFun ),
    predictFun ( predictFun )
{
  blearner_type = blearner_type0;
  initializeDataObjects(data_source, data_target);
}

std::shared_ptr<blearner::Baselearner> BaselearnerCustomCppFactory::createBaselearner (const std::string& identifier)
{
  std::shared_ptr<blearner::Baselearner> sh_ptr_blearner = std::make_shared<blearner::BaselearnerCustomCpp>(data_target, identifier,
    instantiateDataFun, trainFun, predictFun);
  sh_ptr_blearner->setBaselearnerType(blearner_type);

  // // Check if the data is already set. If not, run 'instantiateData' from the
  // // baselearner:
  // if (! is_data_instantiated) {
  //   data = sh_ptr_blearner->instantiateData();
  //
  //   is_data_instantiated = true;
  // }
  return sh_ptr_blearner;
}

/**
 * \brief Data getter which always returns an arma::mat
 *
 * This function is important to have a unified interface to access the data
 * matrices. Especially for predicting we have to get the data of each factory
 * as dense matrix. This is a huge drawback in terms of memory usage. Therefore,
 * this function should only be used to get temporary matrices which are deleted
 * when they run out of scope to reduce memory load. Also note that there is a
 * dispatch with the getData() function of the Data objects which are mostly
 * called internally.
 *
 * \returns `arma::mat` of data used for modelling a single base-learner
 */
arma::mat BaselearnerCustomCppFactory::getData () const
{
  return data_target->getData();
}

// Transform data. This is done twice since it makes the prediction
// of the whole compboost object so much easier:
arma::mat BaselearnerCustomCppFactory::instantiateData (const arma::mat& newdata) const
{
  Rcpp::XPtr<instantiateDataFunPtr> myTempInstantiation (instantiateDataFun);
  instantiateDataFunPtr instantiateDataFun0 = *myTempInstantiation;

  return instantiateDataFun0(newdata);
}


} // namespace blearnerfactory




