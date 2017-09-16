/*
 *      Name: ./common/MiExcepcion.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#ifndef MIEXCEPCION_H_
#define MIEXCEPCION_H_

#include <exception>

namespace exceptions {

  class MiExcepcion: public std::exception {
    public:
      MiExcepcion() throw () {
      }

      virtual ~MiExcepcion() throw () {
      }

      virtual const char *what() const throw () = 0;
  };

} /* namespace exceptions */
#endif /* MIEXCEPCION_H_ */
