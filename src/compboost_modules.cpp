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
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Compboost is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with Compboost. If not, see <http://www.gnu.org/licenses/>.
//
// This file contains:
// -------------------
//
//   Wrapper around the baselearner factory and the factory map to expose to
//   R.
//
// Written by:
// -----------
//
//   Daniel Schalk
//   Institut für Statistik
//   Ludwig-Maximilians-Universität München
//   Ludwigstraße 33
//   D-80539 München
//
//   https://www.compstat.statistik.uni-muenchen.de
//
// ========================================================================== //

#include "compboost.h"
#include "baselearner_factory.h"
#include "baselearner_list.h"
#include "loss.h"

// -------------------------------------------------------------------------- //
//                         BASELEARNER FACTORYS                               //
// -------------------------------------------------------------------------- //

// Abstract class. This one is given to the factory list. The factory list then
// handles all factorys equally. It does not differ between a polynomial or
// custom factory:
class BaselearnerFactoryWrapper
{
  public:
  
    blearnerfactory::BaselearnerFactory* getFactory () { return obj; }
  
  protected:
    
    blearnerfactory::BaselearnerFactory* obj;
    blearner::Baselearner* test_obj;
};

// Wrapper around the PolynomialFactory:
class PolynomialFactoryWrapper : public BaselearnerFactoryWrapper
{
  public:
    
    PolynomialFactoryWrapper (arma::mat data, std::string data_identifier, 
      unsigned int degree) 
    {
      obj = new blearnerfactory::PolynomialFactory("polynomial", data, 
        data_identifier, degree);
      
      std::string test = "polynomial test";
      test_obj = obj->CreateBaselearner(test);
    }
    
    void testTrain (arma::vec& y) { test_obj->train(y); }
    arma::mat testPredict () { return test_obj->predict(); }
    arma::mat testPredictNewdata (arma::mat& newdata) { return test_obj->predict(newdata); }
    arma::mat testGetParameter () { return test_obj->GetParameter(); }
    
    arma::mat getData () { return obj->GetData(); }
    std::string getDataIdentifier () { return obj->GetDataIdentifier(); }
    std::string getBaselearnerType () { return obj->GetBaselearnerType(); }
};

// Wrapper around the CustomFactory:
class CustomFactoryWrapper : public BaselearnerFactoryWrapper
{
  public:
    
    CustomFactoryWrapper (arma::mat data, std::string data_identifier, 
      Rcpp::Function instantiateDataFun, Rcpp::Function trainFun, 
      Rcpp::Function predictFun, Rcpp::Function extractParameter)
    {
      obj = new blearnerfactory::CustomFactory("custom", data, 
        data_identifier, instantiateDataFun, trainFun, predictFun, 
        extractParameter);
      
      std::string test = "custom test";
      test_obj = obj->CreateBaselearner(test);
    }
    
    void testTrain (arma::vec& y) { test_obj->train(y); }
    arma::mat testPredict () { return test_obj->predict(); }
    arma::mat testPredictNewdata (arma::mat& newdata) { return test_obj->predict(newdata); }
    arma::mat testGetParameter () { return test_obj->GetParameter(); }
    
    arma::mat getData () { return obj->GetData(); }
    std::string getDataIdentifier () { return obj->GetDataIdentifier(); }
    std::string getBaselearnerType () { return obj->GetBaselearnerType(); }
};

// Wrapper around the CustomCppFactory:
class CustomCppFactoryWrapper : public BaselearnerFactoryWrapper
{
public:
  
  CustomCppFactoryWrapper (arma::mat data, std::string data_identifier, 
    SEXP instantiateDataFun, SEXP trainFun, SEXP predictFun)
  {
    obj = new blearnerfactory::CustomCppFactory("custom cpp", data, 
      data_identifier, instantiateDataFun, trainFun, predictFun);
    
    std::string test = "custom cpp test";
    test_obj = obj->CreateBaselearner(test);
  }
  
  void testTrain (arma::vec& y) { test_obj->train(y); }
  arma::mat testPredict () { return test_obj->predict(); }
  arma::mat testPredictNewdata (arma::mat& newdata) { return test_obj->predict(newdata); }
  arma::mat testGetParameter () { return test_obj->GetParameter(); }
  
  arma::mat getData () { return obj->GetData(); }
  std::string getDataIdentifier () { return obj->GetDataIdentifier(); }
  std::string getBaselearnerType () { return obj->GetBaselearnerType(); }
};

RCPP_EXPOSED_CLASS(BaselearnerFactoryWrapper);
RCPP_MODULE (baselearner_factory_module) 
{
  using namespace Rcpp;
  
  class_<BaselearnerFactoryWrapper> ("BaselearnerFactory")
    .constructor ("Create BaselearnerFactory class")
  ;
  
  class_<PolynomialFactoryWrapper> ("PolynomialFactory")
    .derives<BaselearnerFactoryWrapper> ("BaselearnerFactory")
    .constructor<arma::mat, std::string, unsigned int> ()
    .method("testTrain",        &PolynomialFactoryWrapper::testTrain, "Test the train function of the created baselearner")
    .method("testPredict",      &PolynomialFactoryWrapper::testPredict, "Test the predict function of the created baselearner")
    .method("testGetParameter", &PolynomialFactoryWrapper::testGetParameter, "Test the GetParameter function of the created baselearner")
    .method("getData",          &PolynomialFactoryWrapper::getData, "Get the data which the factory uses")
    .method("testPredictNewdata", &PolynomialFactoryWrapper::testPredictNewdata, "Predict with newdata")
  ;
  
  class_<CustomFactoryWrapper> ("CustomFactory")
    .derives<BaselearnerFactoryWrapper> ("BaselearnerFactory")
    .constructor<arma::mat, std::string, Rcpp::Function, Rcpp::Function, Rcpp::Function, Rcpp::Function> ()
    .method("testTrain",        &CustomFactoryWrapper::testTrain, "Test the train function of the created baselearner")
    .method("testPredict",      &CustomFactoryWrapper::testPredict, "Test the predict function of the created baselearner")
    .method("testGetParameter", &CustomFactoryWrapper::testGetParameter, "Test the GetParameter function of the created baselearner")
    .method("getData",          &CustomFactoryWrapper::getData, "Get the data which the factory uses")
    .method("testPredictNewdata", &CustomFactoryWrapper::testPredictNewdata, "Predict with newdata")
  ;
  
  class_<CustomCppFactoryWrapper> ("CustomCppFactory")
    .derives<BaselearnerFactoryWrapper> ("BaselearnerFactory")
    .constructor<arma::mat, std::string, SEXP, SEXP, SEXP> ()
    .method("testTrain",        &CustomCppFactoryWrapper::testTrain, "Test the train function of the created baselearner")
    .method("testPredict",      &CustomCppFactoryWrapper::testPredict, "Test the predict function of the created baselearner")
    .method("testGetParameter", &CustomCppFactoryWrapper::testGetParameter, "Test the GetParameter function of the created baselearner")
    .method("getData",          &CustomCppFactoryWrapper::getData, "Get the data which the factory uses")
    .method("testPredictNewdata", &CustomCppFactoryWrapper::testPredictNewdata, "Predict with newdata")
  ;
}



// -------------------------------------------------------------------------- //
//                              BASELEARNERLIST                               //
// -------------------------------------------------------------------------- //

// Wrapper around the BaselearnerList which is in R used as FactoryList
// (which is more intuitive, since we are giving factorys and not lists):
class BaselearnerListWrapper
{
private:
  
  blearnerlist::BaselearnerList obj;
  
public:
  
  void registerFactory (BaselearnerFactoryWrapper my_factory_to_register)
  {
    std::string factory_id = my_factory_to_register.getFactory()->GetDataIdentifier() + ": " + my_factory_to_register.getFactory()->GetBaselearnerType();
    obj.RegisterBaselearnerFactory(factory_id, my_factory_to_register.getFactory());
  }
  
  void printRegisteredFactorys ()
  {
    obj.PrintRegisteredFactorys();
  }
  
  void clearRegisteredFactorys ()
  {
    obj.ClearMap();
  }
  
  blearnerlist::BaselearnerList getFactoryList ()
  {
    return obj;
  }
};


RCPP_EXPOSED_CLASS(BaselearnerListWrapper);
RCPP_MODULE (baselearner_list_module)
{
  using  namespace Rcpp;
  
  class_<BaselearnerListWrapper> ("FactoryList")
    .constructor ()
    .method("registerFactory", &BaselearnerListWrapper::registerFactory, "Register new factory")
    .method("printRegisteredFactorys", &BaselearnerListWrapper::printRegisteredFactorys, "Print all registered factorys")
    .method("clearRegisteredFactorys", &BaselearnerListWrapper::clearRegisteredFactorys, "Clear factory map")
  ;
}

// -------------------------------------------------------------------------- //
//                                    LOSS                                    //
// -------------------------------------------------------------------------- //

class LossWrapper
{
  public:
    
    loss::Loss* getLoss () { return obj; }
  
  protected:
    
    loss::Loss* obj;
};

class QuadraticWrapper : public LossWrapper
{
  public:
    QuadraticWrapper () { obj = new loss::Quadratic(); }
};

class AbsoluteWrapper : public LossWrapper
{
  public:
    AbsoluteWrapper () { obj = new loss::Absolute(); }
};

class CustomWrapper : public LossWrapper
{
  public:
    CustomWrapper (Rcpp::Function lossFun, Rcpp::Function gradientFun, 
      Rcpp::Function initFun) 
    { 
      obj = new loss::CustomLoss(lossFun, gradientFun, initFun); 
    }
};



RCPP_EXPOSED_CLASS(LossWrapper);
RCPP_MODULE (loss_module)
{
  using namespace Rcpp;
  
  class_<LossWrapper> ("Loss")
    .constructor ()
  ;
  
  class_<QuadraticWrapper> ("QuadraticLoss")
    .derives<LossWrapper> ("Loss")
    .constructor ()
  ;
  
  class_<AbsoluteWrapper> ("AbsoluteLoss")
    .derives<LossWrapper> ("Loss")
    .constructor ()
  ;
  
  class_<CustomWrapper> ("CustomLoss")
    .derives<LossWrapper> ("Loss")
    .constructor<Rcpp::Function, Rcpp::Function, Rcpp::Function> ()
  ;
}



// -------------------------------------------------------------------------- //
//                                  LOGGER                                    //
// -------------------------------------------------------------------------- //

// Logger classes:
// ---------------

class LoggerWrapper
{
public:
  
  LoggerWrapper () {};
  
  logger::Logger* getLogger ()
  {
    return obj;
  }
  
  std::string getLoggerId ()
  {
    return logger_id;
  }
  
protected:
  logger::Logger* obj;
  std::string logger_id;
};

class LogIterationWrapper : public LoggerWrapper
{
public:
  LogIterationWrapper (bool use_as_stopper, unsigned int max_iterations)
  {
    obj = new logger::LogIteration(use_as_stopper, max_iterations);
    logger_id = "iterations";
  }
};

class LogTimeWrapper : public LoggerWrapper
{
public:
  LogTimeWrapper (bool use_as_stopper, unsigned int max_time, 
    std::string time_unit)
  {
    obj = new logger::LogTime(use_as_stopper, max_time, time_unit);
    logger_id = "time";
  }
};



// Logger List:
// ------------

class LoggerListWrapper
{
private:
  
  loggerlist::LoggerList* obj = new loggerlist::LoggerList();
  
public:
  
  LoggerListWrapper () {};
  
  loggerlist::LoggerList* getLoggerList ()
  {
    return obj;
  }
  
  void registerLogger (LoggerWrapper logger_wrapper)
  {
    obj->RegisterLogger(logger_wrapper.getLoggerId(), logger_wrapper.getLogger());
  }
  
  void printRegisteredLogger ()
  {
    obj->PrintRegisteredLogger();
  }
  
  void clearRegisteredLogger ()
  {
    obj->ClearMap();  
  }
};

RCPP_EXPOSED_CLASS(LoggerWrapper);
RCPP_EXPOSED_CLASS(LoggerListWrapper);

RCPP_MODULE(logger_module)
{
  using namespace Rcpp;
  
  class_<LoggerWrapper> ("Logger")
    .constructor ()
  ;
  
  class_<LogIterationWrapper> ("LogIterations")
    .derives<LoggerWrapper> ("Logger")
    .constructor<bool, unsigned int> ()
  ;
  
  class_<LogTimeWrapper> ("LogTime")
    .derives<LoggerWrapper> ("Logger")
    .constructor<bool, unsigned int, std::string> ()
  ;
  
  class_<LoggerListWrapper> ("LoggerList")
    .constructor ()
    .method("registerLogger", &LoggerListWrapper::registerLogger, "Register Logger")
    .method("printRegisteredLogger", &LoggerListWrapper::printRegisteredLogger, "Print registered logger")
    .method("clearRegisteredLogger", &LoggerListWrapper::clearRegisteredLogger, "Clear registered logger")
  ;
}

// -------------------------------------------------------------------------- //
//                                 COMPBOOST                                  //
// -------------------------------------------------------------------------- //

class CompboostWrapper 
{
public:
  
  CompboostWrapper (arma::vec response, double learning_rate, 
    bool stop_if_all_stopper_fulfilled, BaselearnerListWrapper factory_list,
    LossWrapper loss, LoggerListWrapper logger_list) 
  {
    
    learning_rate0 = learning_rate;
    used_logger = logger_list.getLoggerList();
    
    used_optimizer = new optimizer::Greedy();
    // std::cout << "<<CompboostWrapper>> Create new Optimizer" << std::endl;
    
    // used_logger = new loggerlist::LoggerList();
    // // std::cout << "<<CompboostWrapper>> Create LoggerList" << std::endl;
    // 
    // logger::Logger* log_iterations = new logger::LogIteration(true, max_iterations);
    // logger::Logger* log_time       = new logger::LogTime(stop_if_all_stopper_fulfilled0, max_time, "microseconds");
    // // std::cout << "<<CompboostWrapper>> Create new Logger" << std::endl;
    // 
    // used_logger->RegisterLogger("iterations", log_iterations);
    // used_logger->RegisterLogger("microseconds", log_time);
    // // std::cout << "<<CompboostWrapper>> Register Logger" << std::endl;
    
    obj = new cboost::Compboost(response, learning_rate0, stop_if_all_stopper_fulfilled, 
      used_optimizer, loss.getLoss(), used_logger, factory_list.getFactoryList());
    // std::cout << "<<CompboostWrapper>> Create Compboost" << std::endl;
  }
  
  // Destructor:
  ~CompboostWrapper ()
  {
    // std::cout << "Call CompboostWrapper Destructor" << std::endl;
    delete used_logger;
    delete used_optimizer;
    delete eval_data;
    delete obj;
  }
  
  // Member functions
  void train (bool trace) 
  {
    obj->TrainCompboost(trace);
  }
  
  arma::vec getPrediction ()
  {
    return obj->GetPrediction();
  }
  
  std::vector<std::string> getSelectedBaselearner ()
  {
    return obj->GetSelectedBaselearner();
  }
  
  Rcpp::List getModelFrame ()
  {
    std::pair<std::vector<std::string>, arma::mat> raw_frame = obj->GetModelFrame();
    
    return Rcpp::List::create(
      Rcpp::Named("colnames")    = raw_frame.first, 
      Rcpp::Named("model.frame") = raw_frame.second
    );
  }
  
  Rcpp::List getLoggerData ()
  {
    return Rcpp::List::create(
      Rcpp::Named("logger_names") = used_logger->GetLoggerData().first,
      Rcpp::Named("logger_data")  = used_logger->GetLoggerData().second
    );
  }
  
private:
  
  loggerlist::LoggerList* used_logger;
  optimizer::Optimizer* used_optimizer = NULL;
  cboost::Compboost* obj;
  arma::mat* eval_data = NULL;
  
  unsigned int max_iterations;
  double learning_rate0;
};


RCPP_EXPOSED_CLASS(CompboostWrapper);
RCPP_MODULE (compboost_module)
{
  using namespace Rcpp;
  
  class_<CompboostWrapper> ("Compboost")
    .constructor<arma::vec, double, bool, BaselearnerListWrapper, LossWrapper, LoggerListWrapper> ()
    .method("train", &CompboostWrapper::train, "Run componentwise boosting")
    .method("getPrediction", &CompboostWrapper::getPrediction, "Get prediction")
    .method("getSelectedBaselearner", &CompboostWrapper::getSelectedBaselearner, "Get vector of selected baselearner")
    .method("getLoggerData", &CompboostWrapper::getLoggerData, "Get data of the used logger")
  ;
}
