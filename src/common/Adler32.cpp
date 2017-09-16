/*
 *      Name: ./common/Adler32.cpp
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include "./Adler32.h"
#include <stdint.h>

namespace hashes {

  /*****************************************************************************/

  Adler32::Adler32()
      : a(1), b(0) {
  }

  /*****************************************************************************/

  Adler32::~Adler32() {
  }

  /*****************************************************************************/

  uint32_t Adler32::checksum(const char * const datos, const uint32_t largo) {
    for (uint32_t indice = 0; indice < largo; ++indice) {
      a = (a + (unsigned char) datos[indice]) % ADLER32_MODULO;
      b = (b + a) % ADLER32_MODULO;
    }
    return (b << 16) | a;
  }

  /*****************************************************************************/

  void Adler32::clear() {
    a = 1;
    b = 0;
  }

  /*****************************************************************************/

  uint32_t Adler32::rollingChecksum(const char* data, uint32_t start, uint32_t end) {
    a = ((a - data[start - 1] - data[end])) % ADLER32_MODULO;
    b = ((b - (end - start + 1) * data[start - 1]) + a) % ADLER32_MODULO;
    uint32_t result = a + b * ADLER32_MODULO;
    return result;
  }

} /* namespace hashes */
