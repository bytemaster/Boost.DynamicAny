Boost.DynamicAny - Remote Procedure Call library
---------------------------------------
Boost.DynamicAny is a vairant on Boost.Any which provides more flexible
dynamic casting of the underlying type.  Whereas retreiving a value from
Boost.Any requires that you know the exact type stored within the Any,
Boost.DynamicAny allows you to *dynamically cast* to either a base or
derived class of the held type. 


Boost.DynamicAny passes all of the same unit tests as Boost.Any, but additionally supports
the following test:

    #include <boost/dynamic_any.hpp>

    using boost::dynamic_any_cast;
    struct base 
    {
        int a;
    };

    struct base1 
    {
        int a1;
    };

    struct derived : base, base1
    {
        int b;
    };

    struct other {};

    void test_dynamic_cast()
    {
        derived d;
        dynamic_any a(d);
        derived &                ra  = dynamic_any_cast<derived&>(a);
        base &                   b   = dynamic_any_cast<base&>(a);
        base1 &                  b1  = dynamic_any_cast<base1&>(a);
        const base1 &            cb1 = dynamic_any_cast<const base1&>(a);
    
        TEST_CHECK_THROW(
            dynamic_any_cast<other&>(a),
            bad_dynamic_any_cast,
            "dynamic_any_cast to incorrect reference type");
    }


### Notice ###

    This library is not part of the official Boost C++ library, but
    is written, to the best of my ability, to follow the best practices
    established by the Boost community and with the hope of being 
    considered for inclusion with a future Boost release.


