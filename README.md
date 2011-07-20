Boost.DynamicAny 
---------------------------------------
Boost.DynamicAny is a vairant on Boost.Any which provides more flexible
dynamic casting of the underlying type.  Whereas retreiving a value from
Boost.Any requires that you know the exact type stored within the Any,
Boost.DynamicAny allows you to *dynamically cast* to either any base of 
the held type. 


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


### boost::any_ref ###

The boost::any_ref class provides a generic reference that automatically casts to reference
types.  You can also get a pointer to the object and have it return NULL if the type is wrong.

This class is supposed to be a type-safe alternative to void*, typesafety comes at the cost of
dynamic_cast<>. 

    #include <boost/any_ref.hpp>

    using boost::any_ref;

    void times2( double& v ) { v*=2; }
    void print( const double& v ) { std::cerr<<v<<std::endl; }

    void test() {
        double          x = 5.5;
        any_ref         r = x;
        times2(r);
        print(r);
        BOOST_ASSERT( x == 11 );

        const double cx = 22;
        any_ref      cr = x;
        print(cr); // OK

        // cannot cast from const double& to double&
        TEST_CHECK_THROW(
            times2(cr),
            bad_any_ref_cast,
            "any_ref_cast to incorrect reference type");
    }



### Notice ###

    This library is not part of the official Boost C++ library, but
    is written, to the best of my ability, to follow the best practices
    established by the Boost community and with the hope of being 
    considered for inclusion with a future Boost release.


