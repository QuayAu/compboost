% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{Compboost_internal}
\alias{Compboost_internal}
\title{Main Compboost Class}
\format{\code{\link{S4}} object.}
\description{
This class collects all parts such as the factory list or the used logger
and passes them to \code{C++}. On the \code{C++} side is then the main
algorithm.
}
\section{Usage}{

\preformatted{
Compboost$new(response, learning_rate, stop_if_all_stopper_fulfilled,
  factory_list, loss, logger_list, optimizer)
}
}

\section{Arguments}{

\describe{
\item{\code{response} [\code{numeric}]}{
  Vector of the true values which should be modeled.
}
\item{\code{learning_rate} [\code{numeric(1)}]}{
  The learning rate which is used to shrink the parameter in each iteration.
}
\item{\code{stop_if_all_stopper_fulfilled} [\code{logical(1)}]}{
  Boolean to indicate which stopping strategy is used. If \code{TRUE} then
  the algorithm stops if all registered logger stopper are fulfilled.
}
\item{\code{factory_list} [\code{BlearnerFactoryList} object]}{
  List of base-learner factories from which one base-learner is selected
  in each iteration by using the
}
\item{\code{loss} [\code{Loss} object]}{
  The loss which should be used to calculate the pseudo residuals in each
  iteration.
}
\item{\code{logger_list} [\code{LoggerList} object]}{
  The list with all registered logger which are used to track the algorithm.
}
\item{\code{optimizer} [\code{Optimizer} object]}{
  The optimizer which is used to select in each iteration one good
  base-learner.
}
}
}

\section{Details}{


  This class is a wrapper around the pure \code{C++} implementation. To see
  the functionality of the \code{C++} class visit
  \url{https://schalkdaniel.github.io/compboost/cpp_man/html/classcboost_1_1_compboost.html}.
}

\section{Fields}{

  This class doesn't contain public fields.
}

\section{Methods}{

\describe{
\item{\code{train(trace)}}{Initial training of the model. The integer
  argument \code{trace} indicates if the logger progress should be printed
  or not and if so trace indicates which iterations should be printed.}
\item{\code{continueTraining(trace, logger_list)}}{Continue the training
  by using an additional \code{logger_list}. The retraining is stopped if
  the first logger says that the algorithm should be stopped.}
\item{\code{getPrediction()}}{Get the inbag prediction which is done during
  the fitting process.}
\item{\code{getSelectedBaselearner()}}{Returns a character vector of how
  the base-learner are selected.}
\item{\code{getLoggerData()}}{Returns a list of all logged data. If the
  algorithm is retrained, then the list contains for each training one
  element.}
\item{\code{getEstimatedParameter()}}{Returns a list with the estimated
  parameter for base-learner which was selected at least once.}
\item{\code{getParameterAtIteration(k)}}{Calculates the prediction at the
  iteration \code{k}.}
\item{\code{getParameterMatrix()}}{Calculates a matrix where row \code{i}
  includes the parameter at iteration \code{i}. There are as many rows
  as done iterations.}
\item{\code{isTrained()}}{This function returns just a boolean value which
  indicates if the initial training was already done.}
\item{\code{predict(newdata)}}{Prediction on new data organized within a
  list of source data objects. It is important that the names of the source
  data objects matches those one that were used to define the factories.}
\item{\code{predictAtIteration(newdata, k)}}{Prediction on new data by using
  another iteration \code{k}.}
\item{\code{setToIteration(k)}}{Set the whole model to another iteration
  \code{k}. After calling this function all other elements such as the
  parameters or the prediction are calculated corresponding to \code{k}.}
\item{\code{summarizeCompboost()}}{Summarize the \code{Compboost} object.}
}
}

\examples{

# Some data:
df = mtcars
df$mpg_cat = ifelse(df$mpg > 20, "high", "low")

# # Create new variable to check the polynomial base-learner with degree 2:
# df$hp2 = df[["hp"]]^2

# Data for the baselearner are matrices:
X_hp = as.matrix(df[["hp"]])
X_wt = as.matrix(df[["wt"]])

# Target variable:
response = ResponseBinaryClassif$new("mpg_cat", "high", df[["mpg_cat"]])

data_source_hp = InMemoryData$new(X_hp, "hp")
data_source_wt = InMemoryData$new(X_wt, "wt")

data_target_hp1 = InMemoryData$new()
data_target_hp2 = InMemoryData$new()
data_target_wt1 = InMemoryData$new()
data_target_wt2 = InMemoryData$new()

# List for oob logging:
oob_data = list(data_source_hp, data_source_wt)

# List to test prediction on newdata:
test_data = oob_data

# Factories:
linear_factory_hp = BaselearnerPolynomial$new(data_source_hp, data_target_hp1,
  list(degree = 1, intercept = TRUE))
linear_factory_wt = BaselearnerPolynomial$new(data_source_wt, data_target_wt1,
  list(degree = 1, intercept = TRUE))
quadratic_factory_hp = BaselearnerPolynomial$new(data_source_hp, data_target_hp2,
  list(degree = 2, intercept = TRUE))
spline_factory_wt = BaselearnerPSpline$new(data_source_wt, data_target_wt2,
  list(degree = 3, n_knots = 10, penalty = 2, differences = 2))

# Create new factory list:
factory_list = BlearnerFactoryList$new()

# Register factories:
factory_list$registerFactory(linear_factory_hp)
factory_list$registerFactory(linear_factory_wt)
factory_list$registerFactory(quadratic_factory_hp)
factory_list$registerFactory(spline_factory_wt)

# Define loss:
loss_bin = LossBinomial$new()

# Define optimizer:
optimizer = OptimizerCoordinateDescent$new()

## Logger

# Define logger. We want just the iterations as stopper but also track the
# time, inbag risk and oob risk:
log_iterations  = LoggerIteration$new(" iteration_logger", TRUE, 500)
log_time        = LoggerTime$new("time_logger", FALSE, 500, "microseconds")

# Define new logger list:
logger_list = LoggerList$new()

# Register the logger:
logger_list$registerLogger(log_iterations)
logger_list$registerLogger(log_time)

# Run compboost:
# --------------

# Initialize object:
cboost = Compboost_internal$new(
  response      = response,
  learning_rate = 0.05,
  stop_if_all_stopper_fulfilled = FALSE,
  factory_list = factory_list,
  loss         = loss_bin,
  logger_list  = logger_list,
  optimizer    = optimizer
)

# Train the model (we want to print the trace):
cboost$train(trace = 50)
cboost

# Get estimated parameter:
cboost$getEstimatedParameter()

# Get trace of selected base-learner:
cboost$getSelectedBaselearner()

# Set to iteration 200:
cboost$setToIteration(200, 30)

# Get new parameter values:
cboost$getEstimatedParameter()

}
