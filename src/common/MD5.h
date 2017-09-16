#ifndef MD5_H_
#define MD5_H_

/*
 *      Name: ./common/MD5.h
 * Copyright: Eclipse Public License - v 1.0
 *            (c) 1995 by Mordechai T. Abzug
 *            Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.
 *   Version: 1.0
 *    Author: GRUPO 08 [COSTANZO, Pablo (90826) - FATUR, Iván (84491)]
 */

// MD5.CC - source code for the C++/object oriented translation and
//          modification of MD5.
// Translation and modification (c) 1995 by Mordechai T. Abzug
// This translation/ modification is provided "as is," without express or
// implied warranty of any kind.
// The translator/ modifier does not claim (1) that MD5 will do what you think
// it does; (2) that this translation/ modification is accurate; or (3) that
// this software is "merchantible."  (Language for this disclaimer partially
// copied from the disclaimer below).
/* based on:

 MD5.H - header file for MD5C.C
 MDDRIVER.C - test driver for MD2, MD4 and MD5

 Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 rights reserved.

 License to copy and use this software is granted provided that it
 is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 Algorithm" in all material mentioning or referencing this software
 or this function.

 License is also granted to make and use derivative works provided
 that such works are identified as "derived from the RSA Data
 Security, Inc. MD5 Message-Digest Algorithm" in all material
 mentioning or referencing the derived work.

 RSA Data Security, Inc. makes no representations concerning either
 the merchantability of this software or the suitability of this
 software for any particular purpose. It is provided "as is"
 without express or implied warranty of any kind.

 These notices must be retained in any copies of any part of this
 documentation and/or software.

 */

#include <cstdio>
#include <fstream>
#include <istream>
#include <ostream>
#include <stdint.h>
#include <string>
#include "./MiExcepcion.h"

#define MD5_LARGO 16

namespace hashes {

  class MD5RawDigest;

  class MD5 {
    private:
      uint32_t state[4];
      uint32_t count[2];  // number of *bits*, mod 2^64
      unsigned char buffer[64];  // input buffer
      unsigned char digest[16];
      unsigned char finalized;

      // called by all constructors
      void init();

      // does the real update work.  Note that length is implied to be 64.
      void transform(const unsigned char *buffer);

      static void encode(unsigned char *dest, const uint32_t *src, uint32_t length);
      static void decode(uint32_t *dest, const unsigned char *src, uint32_t length);

      static inline uint32_t rotate_left(uint32_t x, uint32_t n);
      static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z);
      static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z);
      static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z);
      static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z);
      static inline void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
      static inline void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
      static inline void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
      static inline void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);

    public:
      // methods for controlled operation:
      MD5();  // simple initializer
      void update(const void *input, uint32_t input_length);
      void update(std::istream &stream);
      void update(FILE *file);
      void update(std::ifstream &stream);
      void finalize();

      // constructors for special circumstances.  All these constructors finalize
      // the MD5 context.
      MD5(const void *input, uint32_t input_length);  // digest binary data, finalize
      MD5(std::istream& stream);  // digest stream, finalize
      MD5(FILE *file);  // digest file, close, finalize
      MD5(std::ifstream& stream);  // digest stream, close, finalize

      // methods to acquire finalized result
      MD5RawDigest raw_digest() const;  // digest as a 16-byte binary array
      std::string hex_digest() const;  // digest as a 33-byte ascii-hex string
      friend std::ostream &operator<<(std::ostream &, MD5 context);
  };

  class MD5RawDigest {
      friend MD5RawDigest MD5::raw_digest() const;

    private:
      unsigned char data[MD5_LARGO];

      MD5RawDigest(const unsigned char * const data) throw ();

    public:
      const unsigned char *getDataPtr() const throw ();

      unsigned int getLength() const throw ();
  };

//**************************************************EXCEPCIONES**************************************************//

  class MD5Excepcion: public exceptions::MiExcepcion {
    public:
      virtual ~MD5Excepcion() throw ();
      virtual const char *what() const throw ();
  };

} /* namespace hashes */
#endif /* MD5_H_ */
