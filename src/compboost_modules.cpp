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

#ifndef COMPBOOST_MODULES_CPP_
#define COMPBOOST_MODULES_CPP_

#include "compboost.h"
#include "baselearner_factory.h"
#include "baselearner_factory_list.h"
#include "loss.h"
#include "data.h"

// -------------------------------------------------------------------------- //
//                                   DATA                                     //
// -------------------------------------------------------------------------- //

class DataWrapper
{
public:
  DataWrapper () {}
  
  data::Data* getData ()
  {
    return obj;
  }
  
protected:
  data::Data* obj;

};


class IdentityDataWrapper : public DataWrapper
{
public:
  IdentityDataWrapper (arma::mat data0, std::string data_identifier)
  {
    obj = new data::IdentityData (data0, data_identifier);
  }
  arma::mat getData () const 
  {
    return obj->getData();
  }
  std::string getIdentifier () const
  {
    return obj->getDataIdentifier();
  }
};



RCPP_EXPOSED_CLASS(DataWrapper);
RCPP_MODULE (data_module) 
{
  using namespace Rcpp;
  
  class_<DataWrapper> ("Data")
    .constructor ("Create Data class")
  ;
  
  class_<IdentityDataWrapper> ("IdentityData")
    .derives<DataWrapper> ("Data")
    .constructor<arma::mat, std::string> ()
    .method("getData"      , IdentityDataWrapper::getData, "Get data")
    .method("getIdentifier", IdentityDataWrapper::getIdentifier, "Get the data identifier")
  ;
}


// -------------------------------------------------------------------------- //
//                               BASELEARNER                                  //
// -------------------------------------------------------------------------- //

class BaselearnerWrapper
{
public:
  
  BaselearnerWrapper () {}
  
protected:
  
  blearner::Baselearner* obj;
};

class PolynomialBlearnerWrapper : public BaselearnerWrapper
{
private:
  data::Data* data;
  unsigned int degree;
  
public:
  
  PolynomialBlearnerWrapper (DataWrapper data0, unsigned int degree)
    : degree ( degree )
  {
    data = data0.getData();
    data->setData(obj->InstantiateData(data->getData()));
    std::string temp = "test polynomial of degree " + std::to_string(degree);
    obj = new blearner::PolynomialBlearner(data, temp, degree);
  }
  
  void train (arma::vec& response)
  {
    obj->train(response);
  }
  
  arma::mat getData ()
  {
    return data->getData();
  }
  arma::mat getParameter ()
  {
    return obj->GetParameter();
  }
  
  arma::vec predict ()
  {
    return obj->predict();
  }
  
  arma::vec predictNewdata (arma::mat newdata)
  {
    return obj->predict(newdata);
  }
  
  void summarizeBaselearner ()
  {
    if (degree == 1) {
      Rcpp::Rcout << "Linear baselearner:" << std::endl;
      
    }
    if (degree == 2) {
      Rcpp::Rcout << "Quadratic baselearner:" << std::endl;
    }
    if (degree == 3) {
      Rcpp::Rcout << "Cubic baselearner:" << std::endl;
    } 
    if (degree > 3) {
      Rcpp::Rcout << "Polynomial baselearner of degree " << degree << std::endl;
    }
    Rcpp::Rcout << "\t- Name of the used data: " << obj->GetDataIdentifier() << std::endl;
    Rcpp::Rcout << "\t- Baselearner identifier: " << obj->GetIdentifier() << std::endl;
  }
};


class CustomBlearnerWrapper : public BaselearnerWrapper
{
private:
  data::Data* data;
  
public:
  
  CustomBlearnerWrapper (DataWrapper data0, Rcpp::Function instantiateData,
    Rcpp::Function train, Rcpp::Function predict, Rcpp::Function extractParameter)
  {
    data = data0.getData();
    data->setData(obj->InstantiateData(data->getData()));
    std::string temp = "test custom";
    obj = new blearner::CustomBlearner(data, temp,instantiateData, train, predict, extractParameter);
  }
  
  void train (arma::vec& response)
  {
    obj->train(response);
  }
  
  arma::mat getData ()
  {
    return data->getData();
  }
  arma::mat getParameter ()
  {
    return obj->GetParameter();
  }
  
  arma::vec predict ()
  {
    return obj->predict();
  }
  
  arma::vec predictNewdata (arma::mat newdata)
  {
    return obj->predict(newdata);
  }
  
  void summarizeBaselearner ()
  {
    Rcpp::Rcout << "Custom baselearner:" << std::endl;
      
    Rcpp::Rcout << "\t- Name of the used data: " << obj->GetDataIdentifier() << std::endl;
    Rcpp::Rcout << "\t- Baselearner identifier: " << obj->GetIdentifier() << std::endl;
  }
};

class CustomCppBlearnerWrapper : public BaselearnerWrapper
{
private:
  data::Data* data;
  
public:
  
  CustomCppBlearnerWrapper (DataWrapper data0, SEXP instantiate_data_ptr, 
    SEXP train_ptr, SEXP predict_ptr)
  {
    data = data0.getData();
    data->setData(obj->InstantiateData(data->getData()));
    std::string temp = "test custom cpp learner";
    obj = new blearner::CustomCppBlearner(data, temp, instantiate_data_ptr,
      train_ptr, predict_ptr);
  }
  
  void train (arma::vec& response)
  {
    obj->train(response);
  }
  
  arma::mat getData ()
  {
    return obj->GetData();
  }
  arma::mat getParameter ()
  {
    return obj->GetParameter();
  }
  
  arma::vec predict ()
  {
    return obj->predict();
  }
  
  arma::vec predictNewdata (arma::mat newdata)
  {
    return obj->predict(newdata);
  }
  
  void summarizeBaselearner ()
  {
    Rcpp::Rcout << "Custom baselearner:" << std::endl;
    
    Rcpp::Rcout << "\t- Name of the used data: " << obj->GetDataIdentifier() << std::endl;
    Rcpp::Rcout << "\t- Baselearner identifier: " << obj->GetIdentifier() << std::endl;
  }
};

RCPP_EXPOSED_CLASS(BaselearnerWrapper);
RCPP_MODULE(baselearner_module)
{
  using namespace Rcpp;
  
  class_<BaselearnerWrapper> ("Baselearner")
    .constructor ("Create Baselearner class")
  ;
  
  class_<PolynomialBlearnerWrapper> ("PolynomialBlearner")
    .derives<BaselearnerWrapper> ("Baselearner")
    .constructor<DataWrapper, unsigned int> ()
    .method("train",          &PolynomialBlearnerWrapper::train, "Train function of the baselearner")
    .method("getParameter",   &PolynomialBlearnerWrapper::getParameter, "Predict function of the baselearner")
    .method("predict",        &PolynomialBlearnerWrapper::predict, "GetParameter function of the baselearner")
    .method("predictNewdata", &PolynomialBlearnerWrapper::predictNewdata, "Predict with newdata")
    .method("getData",        &PolynomialBlearnerWrapper::getData, "Get data used for modelling")
    .method("summarizeBaselearner", &PolynomialBlearnerWrapper::summarizeBaselearner, "Summarize Baselearner")
  ;
  
  class_<CustomBlearnerWrapper> ("CustomBlearner")
    .derives<BaselearnerWrapper> ("Baselearner")
    .constructor<DataWrapper, Rcpp::Function, Rcpp::Function, Rcpp::Function, Rcpp::Function> ()
    .method("train",          &CustomBlearnerWrapper::train, "Train function of the baselearner")
    .method("getParameter",   &CustomBlearnerWrapper::getParameter, "Predict function of the baselearner")
    .method("predict",        &CustomBlearnerWrapper::predict, "GetParameter function of the baselearner")
    .method("predictNewdata", &CustomBlearnerWrapper::predictNewdata, "Predict with newdata")
    .method("getData",        &CustomBlearnerWrapper::getData, "Get data used for modelling")
    .method("summarizeBaselearner", &CustomBlearnerWrapper::summarizeBaselearner, "Summarize Baselearner")
  ;
  
  class_<CustomCppBlearnerWrapper> ("CustomCppBlearner")
    .derives<BaselearnerWrapper> ("Baselearner")
    .constructor<DataWrapper, SEXP, SEXP, SEXP> ()
    .method("train",          &CustomCppBlearnerWrapper::train, "Train function of the baselearner")
    .method("getParameter",   &CustomCppBlearnerWrapper::getParameter, "Predict function of the baselearner")
    .method("predict",        &CustomCppBlearnerWrapper::predict, "GetParameter function of the baselearner")
    .method("predictNewdata", &CustomCppBlearnerWrapper::predictNewdata, "Predict with newdata")
    .method("getData",        &CustomCppBlearnerWrapper::getData, "Get data used for modelling")
    .method("summarizeBaselearner", &CustomCppBlearnerWrapper::summarizeBaselearner, "Summarize Baselearner")
  ;
}


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
  // blearner::Baselearner* test_obj;
};

// Wrapper around the PolynomialBlearnerFactory:
class PolynomialBlearnerFactoryWrapper : public BaselearnerFactoryWrapper
{
  
private:
  const unsigned int degree;
  
public:
  
  PolynomialBlearnerFactoryWrapper (DataWrapper data, const unsigned int& degree) 
    : degree ( degree )
  {
    obj = new blearnerfactory::PolynomialBlearnerFactory("polynomial", data.getData(), degree);
  }

  arma::mat getData () { return obj->GetData(); }
  std::string getDataIdentifier () { return obj->GetDataIdentifier(); }
  std::string getBaselearnerType () { return obj->GetBaselearnerType(); }
  
  void summarizeFactory ()
  {
    if (degree == 1) {
      Rcpp::Rcout << "Linear baselearner factory:" << std::endl;
      
    }
    if (degree == 2) {
      Rcpp::Rcout << "Quadratic baselearner factory:" << std::endl;
    }
    if (degree == 3) {
      Rcpp::Rcout << "Cubic baselearner factory:" << std::endl;
    } 
    if (degree > 3) {
      Rcpp::Rcout << "Polynomial baselearner of degree " << degree << " factory:" << std::endl;
    }
    Rcpp::Rcout << "\t- Name of the used data: " << obj->GetDataIdentifier() << std::endl;
    Rcpp::Rcout << "\t- Factory creates the following baselearner: " << obj->GetBaselearnerType() << std::endl;
  }
};

// Wrapper around the CustomBlearnerFactory:
class CustomBlearnerFactoryWrapper : public BaselearnerFactoryWrapper
{
public:
  
  CustomBlearnerFactoryWrapper (DataWrapper data, 
    Rcpp::Function instantiateDataFun, Rcpp::Function trainFun, 
    Rcpp::Function predictFun, Rcpp::Function extractParameter)
  {
    obj = new blearnerfactory::CustomBlearnerFactory("custom", data.getData(), 
      instantiateDataFun, trainFun, predictFun, extractParameter);
  }

  arma::mat getData () { return obj->GetData(); }
  std::string getDataIdentifier () { return obj->GetDataIdentifier(); }
  std::string getBaselearnerType () { return obj->GetBaselearnerType(); }
  
  void summarizeFactory ()
  {
    Rcpp::Rcout << "Custom baselearner Factory:" << std::endl;
      
    Rcpp::Rcout << "\t- Name of the used data: " << obj->GetDataIdentifier() << std::endl;
    Rcpp::Rcout << "\t- Factory creates the following baselearner: " << obj->GetBaselearnerType() << std::endl;
  }
};

// Wrapper around the CustomCppBlearnerFactory:
class CustomCppBlearnerFactoryWrapper : public BaselearnerFactoryWrapper
{
public:
  
  CustomCppBlearnerFactoryWrapper (DataWrapper data,
    SEXP instantiateDataFun, SEXP trainFun, SEXP predictFun)
  {
    obj = new blearnerfactory::CustomCppBlearnerFactory("custom cpp", data.getData(), 
      instantiateDataFun, trainFun, predictFun);
  }
  
  arma::mat getData () { return obj->GetData(); }
  std::string getDataIdentifier () { return obj->GetDataIdentifier(); }
  std::string getBaselearnerType () { return obj->GetBaselearnerType(); }
  
  void summarizeFactory ()
  {
    Rcpp::Rcout << "Custom cpp baselearner Factory:" << std::endl;
    
    Rcpp::Rcout << "\t- Name of the used data: " << obj->GetDataIdentifier() << std::endl;
    Rcpp::Rcout << "\t- Factory creates the following baselearner: " << obj->GetBaselearnerType() << std::endl;
  }
};

RCPP_EXPOSED_CLASS(BaselearnerFactoryWrapper);
RCPP_MODULE (baselearner_factory_module) 
{
  using namespace Rcpp;
  
  class_<BaselearnerFactoryWrapper> ("BaselearnerFactory")
    .constructor ("Create BaselearnerFactory class")
  ;
  
  class_<PolynomialBlearnerFactoryWrapper> ("PolynomialBlearnerFactory")
    .derives<BaselearnerFactoryWrapper> ("BaselearnerFactory")
    .constructor<DataWrapper, unsigned int> ()
     .method("getData",          &PolynomialBlearnerFactoryWrapper::getData, "Get the data which the factory uses")
     .method("summarizeFactory", &PolynomialBlearnerFactoryWrapper::summarizeFactory, "Sumamrize Factory")
  ;
  
  class_<CustomBlearnerFactoryWrapper> ("CustomBlearnerFactory")
    .derives<BaselearnerFactoryWrapper> ("BaselearnerFactory")
    .constructor<DataWrapper, Rcpp::Function, Rcpp::Function, Rcpp::Function, Rcpp::Function> ()
     .method("getData",          &CustomBlearnerFactoryWrapper::getData, "Get the data which the factory uses")
     .method("summarizeFactory", &CustomBlearnerFactoryWrapper::summarizeFactory, "Sumamrize Factory")
  ;
  
  class_<CustomCppBlearnerFactoryWrapper> ("CustomCppBlearnerFactory")
    .derives<BaselearnerFactoryWrapper> ("BaselearnerFactory")
    .constructor<DataWrapper, SEXP, SEXP, SEXP> ()
     .method("getData",          &CustomCppBlearnerFactoryWrapper::getData, "Get the data which the factory uses")
     .method("summarizeFactory", &CustomCppBlearnerFactoryWrapper::summarizeFactory, "Sumamrize Factory")
  ;
}



// -------------------------------------------------------------------------- //
//                              BASELEARNERLIST                               //
// -------------------------------------------------------------------------- //

// Wrapper around the BaselearnerFactoryList which is in R used as FactoryList
// (which is more intuitive, since we are giving factorys and not lists):
class BlearnerFactoryListWrapper
{
private:
  
  blearnerlist::BaselearnerFactoryList obj;
  
  unsigned int number_of_registered_factorys;
  
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
  
  blearnerlist::BaselearnerFactoryList* getFactoryList ()
  {
    return &obj;
  }
  
  Rcpp::List getModelFrame ()
  {
    std::pair<std::vector<std::string>, arma::mat> raw_frame = obj.GetModelFrame();
    
    return Rcpp::List::create(
      Rcpp::Named("colnames")    = raw_frame.first, 
      Rcpp::Named("model.frame") = raw_frame.second
    );
  }
  
  unsigned int getNumberOfRegisteredFactorys ()
  {
    return obj.GetMap().size();
  }
};


RCPP_EXPOSED_CLASS(BlearnerFactoryListWrapper);
RCPP_MODULE (baselearner_list_module)
{
  using  namespace Rcpp;
  
  class_<BlearnerFactoryListWrapper> ("BlearnerFactoryList")
    .constructor ()
    .method("registerFactory", &BlearnerFactoryListWrapper::registerFactory, "Register new factory")
    .method("printRegisteredFactorys", &BlearnerFactoryListWrapper::printRegisteredFactorys, "Print all registered factorys")
    .method("clearRegisteredFactorys", &BlearnerFactoryListWrapper::clearRegisteredFactorys, "Clear factory map")
    .method("getModelFrame", &BlearnerFactoryListWrapper::getModelFrame, "Get the data used for modelling")
    .method("getNumberOfRegisteredFactorys", &BlearnerFactoryListWrapper::getNumberOfRegisteredFactorys, "Get number of registered factorys. Main purpose is for testing.")
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

class QuadraticLossWrapper : public LossWrapper
{
public:
  QuadraticLossWrapper () { obj = new loss::QuadraticLoss(); }
  arma::vec testLoss (arma::vec& true_value, arma::vec& prediction) {
    return obj->DefinedLoss(true_value, prediction);
  }
  arma::vec testGradient (arma::vec& true_value, arma::vec& prediction) {
    return obj->DefinedGradient(true_value, prediction);
  }
  double testConstantInitializer (arma::vec& true_value) {
    return obj->ConstantInitializer(true_value);
  }
};

class AbsoluteLossWrapper : public LossWrapper
{
public:
  AbsoluteLossWrapper () { obj = new loss::AbsoluteLoss(); }
  arma::vec testLoss (arma::vec& true_value, arma::vec& prediction) {
    return obj->DefinedLoss(true_value, prediction);
  }
  arma::vec testGradient (arma::vec& true_value, arma::vec& prediction) {
    return obj->DefinedGradient(true_value, prediction);
  }
  double testConstantInitializer (arma::vec& true_value) {
    return obj->ConstantInitializer(true_value);
  }
};

class CustomLossWrapper : public LossWrapper
{
public:
  CustomLossWrapper (Rcpp::Function lossFun, Rcpp::Function gradientFun, 
    Rcpp::Function initFun) 
  { 
    obj = new loss::CustomLoss(lossFun, gradientFun, initFun); 
  }
  arma::vec testLoss (arma::vec& true_value, arma::vec& prediction) {
    return obj->DefinedLoss(true_value, prediction);
  }
  arma::vec testGradient (arma::vec& true_value, arma::vec& prediction) {
    return obj->DefinedGradient(true_value, prediction);
  }
  double testConstantInitializer (arma::vec& true_value) {
    return obj->ConstantInitializer(true_value);
  }
};



RCPP_EXPOSED_CLASS(LossWrapper);
RCPP_MODULE (loss_module)
{
  using namespace Rcpp;
  
  class_<LossWrapper> ("Loss")
    .constructor ()
  ;
  
  class_<QuadraticLossWrapper> ("QuadraticLoss")
    .derives<LossWrapper> ("Loss")
    .constructor ()
    .method("testLoss", &QuadraticLossWrapper::testLoss, "Test the defined loss function of the loss")
    .method("testGradient", &QuadraticLossWrapper::testGradient, "Test the defined gradient of the loss")
    .method("testConstantInitializer", &QuadraticLossWrapper::testConstantInitializer, "Test the constant initializer function of th eloss")
  ;
  
  class_<AbsoluteLossWrapper> ("AbsoluteLoss")
    .derives<LossWrapper> ("Loss")
    .constructor ()
    .method("testLoss", &AbsoluteLossWrapper::testLoss, "Test the defined loss function of the loss")
    .method("testGradient", &AbsoluteLossWrapper::testGradient, "Test the defined gradient of the loss")
    .method("testConstantInitializer", &AbsoluteLossWrapper::testConstantInitializer, "Test the constant initializer function of th eloss")
  ;
  
  class_<CustomLossWrapper> ("CustomLoss")
    .derives<LossWrapper> ("Loss")
    .constructor<Rcpp::Function, Rcpp::Function, Rcpp::Function> ()
    .method("testLoss", &CustomLossWrapper::testLoss, "Test the defined loss function of the loss")
    .method("testGradient", &CustomLossWrapper::testGradient, "Test the defined gradient of the loss")
    .method("testConstantInitializer", &CustomLossWrapper::testConstantInitializer, "Test the constant initializer function of th eloss")
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

class IterationLoggerWrapper : public LoggerWrapper
{
  
private:
  unsigned int max_iterations;
  bool use_as_stopper;
  
public:
  IterationLoggerWrapper (bool use_as_stopper, unsigned int max_iterations)
    : max_iterations ( max_iterations ),
      use_as_stopper ( use_as_stopper )
  {
    obj = new logger::IterationLogger (use_as_stopper, max_iterations);
    logger_id = " iterations";
  }
  
  void summarizeLogger ()
  {
    Rcpp::Rcout << "Iteration logger:" << std::endl;
    Rcpp::Rcout << "\t- Maximal iterations: " << max_iterations << std::endl;
    Rcpp::Rcout << "\t- Use logger as stopper: " << use_as_stopper << std::endl;
  }
};

class InbagRiskLoggerWrapper : public LoggerWrapper
{
  
private:
  double eps_for_break;
  bool use_as_stopper;
  
public:
  InbagRiskLoggerWrapper (bool use_as_stopper, LossWrapper used_loss, double eps_for_break)
    : eps_for_break ( eps_for_break ),
      use_as_stopper ( use_as_stopper)
  {
    obj = new logger::InbagRiskLogger (use_as_stopper, used_loss.getLoss(), eps_for_break);
    logger_id = "inbag.risk";
  }
  
  void summarizeLogger ()
  {
    Rcpp::Rcout << "Inbag risk logger:" << std::endl;
    if (use_as_stopper) {
      Rcpp::Rcout << "\t- Epsylon used to stop algorithm: " << eps_for_break << std::endl;
    }
    Rcpp::Rcout << "\t- Use logger as stopper: " << use_as_stopper;
  }
};

class OobRiskLoggerWrapper : public LoggerWrapper
{
  
private:
  double eps_for_break;
  bool use_as_stopper;
  
public:
  OobRiskLoggerWrapper (bool use_as_stopper, LossWrapper used_loss, double eps_for_break,
    Rcpp::List oob_data, arma::vec oob_response)
  {
    std::map<std::string, arma::mat> oob_data_map;
    
    // Create data map:
    for (unsigned int i = 0; i < oob_data.size(); i++) {
      
      std::vector<std::string> names = oob_data.names();
      arma::mat temp = Rcpp::as<arma::mat>(oob_data[i]);
      oob_data_map[ names[i] ] = temp;
      
    }
    
    obj = new logger::OobRiskLogger (use_as_stopper, used_loss.getLoss(), eps_for_break,
      oob_data_map, oob_response);
    logger_id = "oob.risk";
  }
  
  void summarizeLogger ()
  {
    Rcpp::Rcout << "Out of bag risk logger:" << std::endl;
    if (use_as_stopper) {
      Rcpp::Rcout << "\t- Epsylon used to stop algorithm: " << eps_for_break << std::endl;
    }
    Rcpp::Rcout << "\t- Use logger as stopper: " << use_as_stopper;
  }
};

class TimeLoggerWrapper : public LoggerWrapper
{
  
private:
  bool use_as_stopper;
  unsigned int max_time;
  std::string time_unit;
  
public:
  TimeLoggerWrapper (bool use_as_stopper, unsigned int max_time, 
    std::string time_unit)
    : use_as_stopper ( use_as_stopper ),
      max_time ( max_time ),
      time_unit ( time_unit )
  {
    obj = new logger::TimeLogger (use_as_stopper, max_time, time_unit);
    logger_id = "time." + time_unit;
  }
  
  void summarizeLogger ()
  {
    Rcpp::Rcout << "Time logger:" << std::endl;
    if (use_as_stopper) {
      Rcpp::Rcout << "\t- Stop algorithm if " << max_time << " " << time_unit << " are over" << std::endl;
    }
    Rcpp::Rcout << "\t- Tracked time unit: " << time_unit << std::endl;
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
  
  unsigned int getNumberOfRegisteredLogger ()
  {
    return obj->GetMap().size();
  }
  
  std::vector<std::string> getNamesOfRegisteredLogger ()
  {
    std::vector<std::string> out;
    for (auto& it : obj->GetMap()) {
      out.push_back(it.first);
    }
    return out;
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
  
  class_<IterationLoggerWrapper> ("IterationLogger")
    .derives<LoggerWrapper> ("Logger")
    .constructor<bool, unsigned int> ()
    .method("summarizeLogger", &IterationLoggerWrapper::summarizeLogger, "Summarize logger")
  ;
  
  class_<InbagRiskLoggerWrapper> ("InbagRiskLogger")
    .derives<LoggerWrapper> ("Logger")
    .constructor<bool, LossWrapper, double> ()
    .method("summarizeLogger", &InbagRiskLoggerWrapper::summarizeLogger, "Summarize logger")
  ;
  
  class_<OobRiskLoggerWrapper> ("OobRiskLogger")
    .derives<LoggerWrapper> ("Logger")
    .constructor<bool, LossWrapper, double, Rcpp::List, arma::vec> ()
    .method("summarizeLogger", &OobRiskLoggerWrapper::summarizeLogger, "Summarize logger")
  ;
  
  class_<TimeLoggerWrapper> ("TimeLogger")
    .derives<LoggerWrapper> ("Logger")
    .constructor<bool, unsigned int, std::string> ()
    .method("summarizeLogger", &TimeLoggerWrapper::summarizeLogger, "Summarize logger")
  ;
  
  class_<LoggerListWrapper> ("LoggerList")
    .constructor ()
    .method("registerLogger", &LoggerListWrapper::registerLogger, "Register Logger")
    .method("printRegisteredLogger", &LoggerListWrapper::printRegisteredLogger, "Print registered logger")
    .method("clearRegisteredLogger", &LoggerListWrapper::clearRegisteredLogger, "Clear registered logger")
    .method("getNumberOfRegisteredLogger", &LoggerListWrapper::getNumberOfRegisteredLogger, "Get number of registered logger. Mainly for testing.")
    .method("getNamesOfRegisteredLogger",  &LoggerListWrapper::getNamesOfRegisteredLogger, "Get names of registered logger. Mainly for testing.")
  ;
}


// -------------------------------------------------------------------------- //
//                                 OPTIMIZER                                  //
// -------------------------------------------------------------------------- //

class OptimizerWrapper 
{
public:
  OptimizerWrapper () {};
  optimizer::Optimizer* getOptimizer () { return obj; }
  
protected:
  optimizer::Optimizer* obj;
};

class GreedyOptimizer : public OptimizerWrapper
{
public:
  GreedyOptimizer () { obj = new optimizer::GreedyOptimizer(); }
  
  Rcpp::List testOptimizer (arma::vec& response, BlearnerFactoryListWrapper factory_list)
  {
    std::string temp_str = "test run";
    blearner::Baselearner* blearner_test = obj->FindBestBaselearner(temp_str, response, factory_list.getFactoryList()->GetMap());
    
    Rcpp::List out = Rcpp::List::create(
      Rcpp::Named("selected.learner") = blearner_test->GetIdentifier(),
      Rcpp::Named("parameter")        = blearner_test->GetParameter()
    );
    
    delete blearner_test;
    
    return out;
  }
};

RCPP_EXPOSED_CLASS(OptimizerWrapper);
RCPP_MODULE(optimizer_module)
{
  using namespace Rcpp;
  
  class_<OptimizerWrapper> ("Optimizer")
    .constructor ()
  ;
  
  class_<GreedyOptimizer> ("GreedyOptimizer")
    .derives<OptimizerWrapper> ("Optimizer")
    .constructor ()
    .method("testOptimizer", &GreedyOptimizer::testOptimizer, "Test the optimizer on a given list of factorys")
  ;
}


// -------------------------------------------------------------------------- //
//                                 COMPBOOST                                  //
// -------------------------------------------------------------------------- //

class CompboostWrapper 
{
public:
  
  CompboostWrapper (arma::vec response, double learning_rate, 
    bool stop_if_all_stopper_fulfilled, BlearnerFactoryListWrapper factory_list,
    LossWrapper loss, LoggerListWrapper logger_list, OptimizerWrapper optimizer) 
  {
    
    learning_rate0 = learning_rate;
    used_logger    = logger_list.getLoggerList();
    used_optimizer = optimizer.getOptimizer();
    blearner_list_ptr = factory_list.getFactoryList();
    
    // used_optimizer = new optimizer::Greedy();
    // Rcpp::Rcout << "<<CompboostWrapper>> Create new Optimizer" << std::endl;
    
    // used_logger = new loggerlist::LoggerList();
    // // Rcpp::Rcout << "<<CompboostWrapper>> Create LoggerList" << std::endl;
    // 
    // logger::Logger* log_iterations = new logger::IterationLogger(true, max_iterations);
    // logger::Logger* log_time       = new logger::TimeLogger(stop_if_all_stopper_fulfilled0, max_time, "microseconds");
    // // Rcpp::Rcout << "<<CompboostWrapper>> Create new Logger" << std::endl;
    // 
    // used_logger->RegisterLogger("iterations", log_iterations);
    // used_logger->RegisterLogger("microseconds", log_time);
    // // Rcpp::Rcout << "<<CompboostWrapper>> Register Logger" << std::endl;
    
    obj = new cboost::Compboost(response, learning_rate0, stop_if_all_stopper_fulfilled, 
      used_optimizer, loss.getLoss(), used_logger, *blearner_list_ptr);
    // Rcpp::Rcout << "<<CompboostWrapper>> Create Compboost" << std::endl;
  }
  
  // Member functions
  void train (bool trace) 
  {
    obj->TrainCompboost(trace);
    is_trained = true;
  }
  
  void continueTraining (bool trace, LoggerListWrapper logger_list) 
  {
    obj->ContinueTraining(logger_list.getLoggerList(), trace);
  }
  
  arma::vec getPrediction ()
  {
    return obj->GetPrediction();
  }
  
  std::vector<std::string> getSelectedBaselearner ()
  {
    return obj->GetSelectedBaselearner();
  }
  
  Rcpp::List getLoggerData ()
  {
    Rcpp::List out_list;
    
    for (auto& it : obj->GetLoggerList()) {
      out_list[it.first] = Rcpp::List::create(
        Rcpp::Named("logger.names") = it.second->GetLoggerData().first,
        Rcpp::Named("logger.data")  = it.second->GetLoggerData().second
      );
    }
    if (out_list.size() == 1) {
      return out_list[0];
    } else {
      return out_list;
    }
  }
  
  Rcpp::List getEstimatedParameter ()
  {
    std::map<std::string, arma::mat> parameter = obj->GetParameter();
    
    Rcpp::List out;
    
    for (auto &it : parameter) {
      out[it.first] = it.second;
    }
    return out;
  }
  
  Rcpp::List getParameterAtIteration (unsigned int k)
  {
    std::map<std::string, arma::mat> parameter = obj->GetParameterOfIteration(k);
    
    Rcpp::List out;
    
    for (auto &it : parameter) {
      out[it.first] = it.second;
    }
    return out;
  }
  
  Rcpp::List getParameterMatrix ()
  {
    std::pair<std::vector<std::string>, arma::mat> out_pair = obj->GetParameterMatrix();
    
    return Rcpp::List::create(
      Rcpp::Named("parameter.names")   = out_pair.first,
      Rcpp::Named("parameter.matrix")  = out_pair.second
    );
  }
  
  arma::vec predict (Rcpp::List input_data)
  {
    std::map<std::string, arma::mat> data_map;
    
    // Create data map:
    for (unsigned int i = 0; i < input_data.size(); i++) {
      
      std::vector<std::string> names = input_data.names();
      arma::mat temp = Rcpp::as<arma::mat>(input_data[i]);
      data_map[ names[i] ] = temp;
      
    }
    return obj->Predict(data_map);
  }
  
  arma::vec predictAtIteration (Rcpp::List input_data, unsigned int k)
  {
    std::map<std::string, arma::mat> data_map;
    
    // Create data map:
    for (unsigned int i = 0; i < input_data.size(); i++) {
      
      std::vector<std::string> names = input_data.names();
      arma::mat temp = Rcpp::as<arma::mat>(input_data[i]);
      data_map[ names[i] ] = temp;
      
    }
    return obj->PredictionOfIteration(data_map, k);
  }
  
  void summarizeCompboost ()
  {
    obj->SummarizeCompboost();
  }
  
  bool isTrained ()
  {
    return is_trained;
  }
  
  void setToIteration (const unsigned int& k)
  {
    obj->SetToIteration(k);
  }
  
  // Destructor:
  ~CompboostWrapper ()
  {
    // Rcpp::Rcout << "Call CompboostWrapper Destructor" << std::endl;
    // delete used_logger;
    // delete used_optimizer;
    // delete eval_data;
    // delete obj;
  }
  
private:
  
  blearnerlist::BaselearnerFactoryList* blearner_list_ptr;
  loggerlist::LoggerList* used_logger;
  optimizer::Optimizer* used_optimizer;
  cboost::Compboost* obj;
  arma::mat* eval_data;
  
  unsigned int max_iterations;
  double learning_rate0;
  
  bool is_trained = false;
};


RCPP_EXPOSED_CLASS(CompboostWrapper);
RCPP_MODULE (compboost_module)
{
  using namespace Rcpp;
  
  class_<CompboostWrapper> ("Compboost")
    .constructor<arma::vec, double, bool, BlearnerFactoryListWrapper, LossWrapper, LoggerListWrapper, OptimizerWrapper> ()
    .method("train", &CompboostWrapper::train, "Run componentwise boosting")
    .method("continueTraining", &CompboostWrapper::continueTraining, "Continue Training")
    .method("getPrediction", &CompboostWrapper::getPrediction, "Get prediction")
    .method("getSelectedBaselearner", &CompboostWrapper::getSelectedBaselearner, "Get vector of selected baselearner")
    .method("getLoggerData", &CompboostWrapper::getLoggerData, "Get data of the used logger")
    .method("getEstimatedParameter", &CompboostWrapper::getEstimatedParameter, "Get the estimated paraemter")
    .method("getParameterAtIteration", &CompboostWrapper::getParameterAtIteration, "Get the estimated parameter for iteration k < iter.max")
    .method("getParameterMatrix", &CompboostWrapper::getParameterMatrix, "Get matrix of all estimated parameter in each iteration")
    .method("predict", &CompboostWrapper::predict, "Predict newdata")
    .method("predictAtIteration", &CompboostWrapper::predictAtIteration, "Predict newdata for iteration k < iter.max")
    .method("summarizeCompboost",    &CompboostWrapper::summarizeCompboost, "Sumamrize compboost object.")
    .method("isTrained", &CompboostWrapper::isTrained, "Status of algorithm if it is already trained.")
    .method("setToIteration", &CompboostWrapper::setToIteration, "Set state of the model to a given iteration")
  ;
}

#endif // COMPBOOST_MODULES_CPP_
