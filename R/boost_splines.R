#' Wrapper to boost general additive models for each feature.
#'
#' This wrapper function automatically initialize the model by adding all numerical
#' features as spline base-learner. Categorical features are dummy encoded and inserted 
#' using another linear base-learners without intercept. The function \code{boostSplines} 
#' does also train the model. 
#' 
#' The returned object is an object of the \code{Compboost} class. This object can be 
#' used for further analyses (see \code{?Compboost} for details). 
#'
#' @return A model of the \code{Compboost} class. This model is an \code{R6} object
#'   which can be used for retraining, predicting, plotting, and anything described in 
#'   \code{?Compboost}.
#' @param data [\code{data.frame}]\cr
#'   A data frame containing the data. 
#' @param target [\code{character(1)}]\cr
#'   Character value containing the target variable. Note that the loss must match the 
#'   data type of the target.
#' @param optimizer [\code{S4 Optimizer}]\cr
#'   An initialized \code{S4 Optimizer} object exposed by Rcpp (e.g. \code{OptimizerCoordinateDescent$new()})
#'   to select features at each iteration.
#' @param loss [\code{S4 Loss}]\cr
#'   Initialized \code{S4 Loss} object exposed by Rcpp that is used to calculate the risk and pseudo 
#'   residuals (e.g. \code{LossQuadratic$new()}).
#' @param learning_rate [\code{numeric(1)}]\cr
#'   Learning rate to shrink the parameter in each step.
#' @param iterations [\code{integer(1)}]\cr
#'   Number of iterations that are trained.
#' @param trace [\code{integer(1)}]\cr
#'   Integer indicating how often a trace should be printed. Specifying \code{trace = 10}, then every
#'   10th iteration is printed. If no trace should be printed set \code{trace = 0}. Default is
#'   -1 which means that in total 40 iterations are printed.
#' @param degree [\code{integer(1)}]\cr
#'   Polynomial degree of the splines.
#' @param n_knots [\code{integer(1)}]\cr
#'   Number of equidistant "inner knots". The actual number of used knots does also depend on
#'   the polynomial degree.
#' @param penalty [\code{numeric(1)}]\cr
#'   Penalty term for p-splines. If the penalty equals 0, then ordinary b-splines are fitted.
#'   The higher the penalty, the higher the smoothness.
#' @param differences [\code{integer(1)}]\cr
#'   Number of differences that are used for penalization. The higher the difference, the higher the smoothness.
#' @param data_source [\code{S4 Data}]\cr
#'   Uninitialized \code{S4 Data} object which is used to store the data. At the moment
#'   just in memory training is supported.
#' @param data_target [\code{S4 Data}]\cr
#'   Uninitialized \code{S4 Data} object which is used to store the data. At the moment
#'   just in memory training is supported.
#' @param oob_fraction [\code{numeric(1)}]\cr
#'   Fraction of how much data we want to use to track the out of bag risk.
#' @examples
#' mod = boostSplines(data = iris, target = "Sepal.Length", loss = LossQuadratic$new(), 
#'   oob_fraction = 0.3)
#' mod$getBaselearnerNames()
#' mod$getEstimatedCoef()
#' table(mod$getSelectedBaselearner())
#' mod$predict()
#' mod$plot("Sepal.Width_spline")
#' mod$plotInbagVsOobRisk()
#' @export
boostSplines = function(data, target, optimizer = OptimizerCoordinateDescent$new(), loss, 
  learning_rate = 0.05, iterations = 100, trace = -1, degree = 3, n_knots = 20, 
  penalty = 2, differences = 2, data_source = InMemoryData, data_target = InMemoryData, oob_fraction = NULL) 
{
  model = Compboost$new(data = data, target = target, optimizer = optimizer, loss = loss, 
    learning_rate = learning_rate, oob_fraction = oob_fraction)
  features = setdiff(colnames(data), target)

  # This loop could be replaced with foreach???
  # Issue: 
  for(feat in features) {
    if (is.numeric(data[[feat]])) {
      model$addBaselearner(feat, "spline", BaselearnerPSpline, data_source, data_target,
        degree = degree, n_knots = n_knots, penalty = penalty, differences = differences)
    } else {
      model$addBaselearner(feat, "category", BaselearnerPolynomial, data_source, data_target,
        degree = 1, intercept = FALSE)
    }
  }
  model$train(iterations, trace)
  return(model)
}
