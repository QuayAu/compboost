# Helper functions:
# -----------------

glueLoss = function (name, definition = NULL, additional_desc = "")
{
  if (is.null(definition)) {
    definition = "No function specified, probably you are using a custom loss."
  } else {
    definition = paste0("Loss function: L(y,x) = ", definition)
  }

  ignore_me = glue::glue("

    {name} Loss:

      {definition}

      {additional_desc}

    ")

  print(ignore_me)
  return(invisible(paste0(name, "Printer")))
}

# ---------------------------------------------------------------------------- #
# Data:
# ---------------------------------------------------------------------------- #

setClass("Rcpp_InMemoryData")
ignore_me = setMethod("show", "Rcpp_InMemoryData", function (object) {

  cat("\n")

  if (object$getIdentifier() == "") {
    # data.type = "target"
    cat("Empty data object which can be used as target data.")
  } else {
    # data.type = "source"
    cat("Source Data: In memory class of feature ", object$getIdentifier(), ".")
    cat("\n             Note that using this class as target will override data specified before.")
  }

  cat("\n\n")

  return ("InMemoryDataPrinter")
})

# ---------------------------------------------------------------------------- #
# Factories:
# ---------------------------------------------------------------------------- #

setClass("Rcpp_BaselearnerPolynomial")
ignore_me = setMethod("show", "Rcpp_BaselearnerPolynomial", function (object) {
  cat("\n")
  object$summarizeFactory()
  cat("\n\n")

  return ("BaselearnerPolynomialPrinter")
})

setClass("Rcpp_BaselearnerPSpline")
ignore_me = setMethod("show", "Rcpp_BaselearnerPSpline", function (object) {
  cat("\n")
  object$summarizeFactory()
  cat("\n\n")

  return ("BaselearnerPSplinePrinter")
})

setClass("Rcpp_BaselearnerCustom")
ignore_me = setMethod("show", "Rcpp_BaselearnerCustom", function (object) {
  cat("\n")
  object$summarizeFactory()
  cat("\n\n")

  return ("BaselearnerCustomPrinter")
})

setClass("Rcpp_BaselearnerCustomCpp")
ignore_me = setMethod("show", "Rcpp_BaselearnerCustomCpp", function (object) {
  cat("\n")
  object$summarizeFactory()
  cat("\n\n")

  return ("BaselearnerCustomCppPrinter")
})

# ---------------------------------------------------------------------------- #
# BlearnerFacotryList:
# ---------------------------------------------------------------------------- #

setClass("Rcpp_BlearnerFactoryList")
ignore_me = setMethod("show", "Rcpp_BlearnerFactoryList", function (object) {
  cat("\n")
  object$printRegisteredFactories()
  cat("\n\n")

  # For testing:
  return(invisible("BlearnerFactoryListPrinter"))
})


# ---------------------------------------------------------------------------- #
# Loss:
# ---------------------------------------------------------------------------- #

setClass("Rcpp_LossQuadratic")
ignore_me = setMethod("show", "Rcpp_LossQuadratic", function (object) {
  glueLoss("LossQuadratic", "0.5 * (y - f(x))^2")
})

setClass("Rcpp_LossAbsolute")
ignore_me = setMethod("show", "Rcpp_LossAbsolute", function (object) {
  glueLoss("LossAbsolute", "|y - f(x)|")
})

setClass("Rcpp_LossBinomial")
ignore_me = setMethod("show", "Rcpp_LossBinomial", function (object) {
  # glueLoss("LossBinomial", "log(1 + exp(-2yf(x))", "Labels should be coded as -1 and 1!")
  glueLoss("LossBinomial", "log(1 + exp(-2yf(x))")
})

setClass("Rcpp_LossCustom")
ignore_me = setMethod("show", "Rcpp_LossCustom", function (object) {
  glueLoss("LossCustom")
})

setClass("Rcpp_LossCustomCpp")
ignore_me = setMethod("show", "Rcpp_LossCustomCpp", function (object) {
  glueLoss("LossCustomCpp")
})

# ---------------------------------------------------------------------------- #
# Logger:
# ---------------------------------------------------------------------------- #

setClass("Rcpp_LoggerIteration")
ignore_me = setMethod("show", "Rcpp_LoggerIteration", function (object) {
  cat("\n")
  object$summarizeLogger()
  cat("\n\n")

  return ("LoggerIterationPrinter")
})

setClass("Rcpp_LoggerInbagRisk")
ignore_me = setMethod("show", "Rcpp_LoggerInbagRisk", function (object) {
  cat("\n")
  object$summarizeLogger()
  cat("\n\n")

  return ("LoggerInbagRiskPrinter")
})

setClass("Rcpp_LoggerOobRisk")
ignore_me = setMethod("show", "Rcpp_LoggerOobRisk", function (object) {
  cat("\n")
  object$summarizeLogger()
  cat("\n\n")

  return ("LoggerOobRiskPrinter")
})

setClass("Rcpp_LoggerTime")
ignore_me = setMethod("show", "Rcpp_LoggerTime", function (object) {
  cat("\n")
  object$summarizeLogger()
  cat("\n\n")

  return ("LoggerTimePrinter")
})

# ---------------------------------------------------------------------------- #
# Loggerlist:
# ---------------------------------------------------------------------------- #

setClass("Rcpp_LoggerList")
ignore_me = setMethod("show", "Rcpp_LoggerList", function (object) {
  cat("\n")
  if (object$getNumberOfRegisteredLogger() == 0) {
    cat("No registered logger!")
  } else {
    object$printRegisteredLogger()
  }
  cat("\n\n")

  return ("LoggerListPrinter")
})

# ---------------------------------------------------------------------------- #
# Optimizer:
# ---------------------------------------------------------------------------- #

setClass("Rcpp_OptimizerCoordinateDescent")
ignore_me = setMethod("show", "Rcpp_OptimizerCoordinateDescent", function (object) {
  cat("\n")
  cat("Coordinate Descent optimizer\n")
  cat("\n\n")

  return (invisible("OptimizerCoordinateDescentPrinter"))
})

setClass("Rcpp_OptimizerCoordinateDescentLineSearch")
ignore_me = setMethod("show", "Rcpp_OptimizerCoordinateDescentLineSearch", function (object) {
  cat("\n")
  cat("Cordinate Descent optimizer with line search\n")
  cat("\n\n")

  return (invisible("OptimizerCoordinateDescentLineSearchPrinter"))
})


# ---------------------------------------------------------------------------- #
# Compboost:
# ---------------------------------------------------------------------------- #

setClass("Rcpp_Compboost_internal")
ignore_me = setMethod("show", "Rcpp_Compboost_internal", function (object) {
  cat("\n")
  object$summarizeCompboost()
  cat("\n\n")

  return (invisible("CompboostInternalPrinter"))
})
