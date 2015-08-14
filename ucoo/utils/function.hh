#ifndef ucoo_utils_function_hh
#define ucoo_utils_function_hh
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
#include "ucoo/common.hh"

namespace ucoo {

/// Type used to compute maximum size of a FunctionStore.
union FunctionStoreTypes
{
    /// A FunctionStore can store a function pointer...
    template<typename Res, typename ...Args>
    using Function = Res (*) (Args...);
    Function<void> function;
    /// ...or a bound member function (object pointer + member function
    /// pointer).
    template<typename T, typename Res, typename ...Args>
    struct BoundMember
    {
        T *object;
        Res (T::*member_pointer) (Args...);
    };
    class SomeClass;
    BoundMember<SomeClass, void> bound_member;
};

/// Store what is needed to call a function.
union FunctionStore
{
    void *access () { return data_; }
    const void *access () const { return data_; }
    template<typename T>
    T &access ()
    {
        static_assert (sizeof (T) <= sizeof (data_),
                       "function pointer too big");
        return *reinterpret_cast<T *> (access ());
    }
    template<typename T>
    const T &access () const
    {
        static_assert (sizeof (T) <= sizeof (data_),
                       "function pointer too big");
        return *reinterpret_cast<const T *> (access ());
    }
  private:
    FunctionStoreTypes unused_;
    char data_[sizeof (FunctionStoreTypes)];
};

template<typename Signature>
class Function;

/// Store a reference to a callable element in an object which type only
/// depends on the callable signature.
///
/// This is a lightweight and limited version of std::function.
template<typename Res, typename ...Args>
class Function<Res (Args...)>
{
  public:
    /// Construct an empty function, can not be called.
    Function () : call_ (nullptr) { }
    /// Construct from a function pointer.
    Function (Res (*function_pointer) (Args...))
    {
        assert (function_pointer);
        using F = FunctionStoreTypes::Function<Res, Args...>;
        F &f = store_.access<F> ();
        f = function_pointer;
        call_ = &Function::call_function<F>;
    }
    /// Construct with a bound member function.
    template<typename T>
    Function (T *object, Res (T::*member_pointer) (Args...))
    {
        assert (object && member_pointer);
        using F = FunctionStoreTypes::BoundMember<T, Res, Args...>;
        F &f = store_.access<F> ();
        f.object = object;
        f.member_pointer = member_pointer;
        call_ = &Function::call_bound_member<F>;
    }
    /// Reset to empty function.
    void reset () { call_ = nullptr; }
    /// Call the stored function.
    Res operator() (Args... args) const
    {
        assert (call_);
        return call_ (args..., store_);
    }
    /// Test whether there is a stored function.
    operator bool () const
    {
        return call_;
    }
  private:
    template<typename F>
    static Res call_function (Args... args, const FunctionStore &store)
    {
        const F &f = store.access<F> ();
        return f (args...);
    }
    template<typename F>
    static Res call_bound_member (Args... args, const FunctionStore &store)
    {
        const F &f = store.access<F> ();
        return (f.object->*f.member_pointer) (args...);
    }
  private:
    Res (*call_) (Args..., const FunctionStore &store);
    FunctionStore store_;
};

} // namespace ucoo

#endif // ucoo_utils_function_hh
