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
//   
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

#ifndef DATA_H_
#define DATA_H_

#include "RcppArmadillo.h"

namespace data 
{

// -------------------------------------------------------------------------- //
// Abstract 'Data' class:
// -------------------------------------------------------------------------- //

class Data
{
protected:
  
  std::string data_identifier;
  
public:
  
  Data ();
  
  virtual void setData (const arma::mat&) = 0;
  virtual arma::mat getData () const = 0;
  
  virtual 
    ~Data () { };
  
  std::string getDataIdentifier () const;
};


// -------------------------------------------------------------------------- //
// Data implementations:
// -------------------------------------------------------------------------- //

// IdentityData:
// -----------------------

// This one does nothing special, just takes the data and use the transformed
// one as train data.

class IdentityData : public Data
{
private:
  
  arma::mat data_mat;
  
public:
  
  IdentityData (const arma::mat&, const std::string&);
  
  void setData (const arma::mat&);
  arma::mat getData() const;
  
  ~IdentityData ();
  
};

} // namespace data

#endif // DATA_H_