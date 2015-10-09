#ifndef ucoo_utils_pool_hh
#define ucoo_utils_pool_hh
// ucoolib - Microcontroller object oriented library. {{{
//
// Copyright (C) 2015 Nicolas Schodet
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// }}}
#include <array>
#include <memory>

namespace ucoo {

/// Store an object, or a pointer to next free storage.
template<typename T>
union PoolStorage
{
    /// Object in pool.
    T object;
    /// If object is not allocated, this is a pointer to the next free object,
    /// or nullptr if this is the last one.
    PoolStorage *next_free;
    /// Due to union, constructor must be explicit.
    PoolStorage () { }
    /// Due to union, destructor must be explicit.
    ~PoolStorage () { }
};

/// Pool of statically allocated object.
template<typename T, int size>
class Pool
{
  public:
    /// Constructor, initialise the pool.
    Pool ();
    /// Destructor, check that all objects are destroyed.
    ~Pool ();
    /// Create and construct a new object.
    template<typename... Args>
    T *construct (Args&&... args);
    /// Destroy an object created from this pool and call its destructor.
    void destroy (T *object);
  private:
    /// Pool storage.
    std::array<PoolStorage<T>, size> pool_;
    /// Pointer to the next free storage, or nullptr if pool is full.
    PoolStorage<T> *next_free_;
};

} // namespace ucoo

#include "pool.tcc"

#endif // ucoo_utils_pool_hh
