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
//   Implementation of the "BaselearnerList".
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
// =========================================================================== #

#include "baselearner_list.h"

namespace blearnerlist 
{

// Just an empty constructor:
BaselearnerList::BaselearnerList () {}

// Register a factory:
void BaselearnerList::RegisterBaselearnerFactory (std::string factory_id, 
  blearnerfactory::BaselearnerFactory *blearner_factory)
{
  // Create iterator and check if learner is already registered:
  std::map<std::string,blearnerfactory::BaselearnerFactory*>::iterator it = my_factory_map.find(factory_id);
  
  if (it == my_factory_map.end()) {
    my_factory_map.insert(std::pair<std::string, blearnerfactory::BaselearnerFactory*>(factory_id, blearner_factory));
  } else {
    my_factory_map[ factory_id ] = blearner_factory;
  }
}

// Print all registered factorys:
void BaselearnerList::PrintRegisteredFactorys ()
{
  // Check if any factory is registered:
  if (my_factory_map.size() >= 1) {
    std::cout << "Registered Factorys:\n";
  } else {
    std::cout << "No registered Factorys!";
  }

  // Iterate over all registered factorys and print the factory identifier:
  for (blearner_factory_map::iterator it = my_factory_map.begin(); it != my_factory_map.end(); ++it) {
    std::cout << "\t- " << it->first << std::endl;
  }
}

// Getter for the map object:
blearner_factory_map BaselearnerList::GetMap ()
{
  return my_factory_map;
}

// Remove all registered factorys:
void BaselearnerList::ClearMap ()
{
  // This deletes all the data which are sometimes necessary to re register 
  // factorys!
  // for (blearner_factory_map::iterator it = my_factory_map.begin(); it != my_factory_map.end(); ++it) {
  //   delete it->second;
  // }
  my_factory_map.clear();
}

} // namespace blearnerlist
