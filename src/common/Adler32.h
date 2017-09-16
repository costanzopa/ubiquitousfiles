#ifndef ADLER32_H
#define ADLER32_H

/*
 *      Name: ./common/Adler32.h
 * Copyright: Eclipse Public License - v 1.0
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

#include <stdint.h>

#define ADLER32_MODULO 65521

namespace hashes {

  class Adler32 {
    private:
      uint32_t a;
      uint32_t b;

    public:
      Adler32();

      virtual ~Adler32();

      void clear();

      uint32_t checksum(const char * const datos, const uint32_t largo);

      uint32_t rollingChecksum(const char* data, uint32_t start, uint32_t end);
  };

} /* namespace hashes */
#endif /* ADLER32_H */
