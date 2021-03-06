// Copyright John Maddock 2006.
// Copyright Paul A. Bristow 2007, 2009
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/math/concepts/real_concept.hpp>
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/math/special_functions/math_fwd.hpp>
#include <boost/array.hpp>
#include "functor.hpp"

#include "handle_test_result.hpp"
#include "table_type.hpp"

#ifndef SC_
#define SC_(x) static_cast<typename table_type<T>::type>(BOOST_JOIN(x, L))
#endif

template <class Real, class T>
void do_test_digamma(const T& data, const char* type_name, const char* test_name)
{
   typedef Real                   value_type;

   typedef value_type (*pg)(value_type);
#if defined(BOOST_MATH_NO_DEDUCED_FUNCTION_POINTERS)
   pg funcp = boost::math::digamma<value_type>;
#else
   pg funcp = boost::math::digamma;
#endif

   boost::math::tools::test_result<value_type> result;

   std::cout << "Testing " << test_name << " with type " << type_name
      << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

   //
   // test digamma against data:
   //
   result = boost::math::tools::test_hetero<Real>(
      data, 
      bind_func<Real>(funcp, 0), 
      extract_result<Real>(1));
   handle_test_result(result, data[result.worst()], result.worst(), type_name, "boost::math::digamma", test_name);
   std::cout << std::endl;
}

template <class T>
void test_digamma(T, const char* name)
{
   //
   // The actual test data is rather verbose, so it's in a separate file
   //
   // The contents are as follows, each row of data contains
   // three items, input value a, input value b and erf(a, b):
   // 
#  include "digamma_data.ipp"

   do_test_digamma<T>(digamma_data, name, "Digamma Function: Large Values");

#  include "digamma_root_data.ipp"

   do_test_digamma<T>(digamma_root_data, name, "Digamma Function: Near the Positive Root");

#  include "digamma_small_data.ipp"

   do_test_digamma<T>(digamma_small_data, name, "Digamma Function: Near Zero");

#  include "digamma_neg_data.ipp"

   do_test_digamma<T>(digamma_neg_data, name, "Digamma Function: Negative Values");

    static const boost::array<boost::array<T, 2>, 5> digamma_bugs = {{
       // Test cases from Rocco Romeo:
        {{ static_cast<T>(std::ldexp(1.0, -100)), SC_(-1.26765060022822940149670320537657721566490153286060651209008e30) }},
        {{ static_cast<T>(-std::ldexp(1.0, -100)), SC_(1.26765060022822940149670320537542278433509846713939348790992e30) }},
        {{ static_cast<T>(1), SC_(-0.577215664901532860606512090082402431042159335939923598805767) }},
        {{ SC_(-1) + static_cast<T>(std::ldexp(1.0, -20)), SC_(-1.04857557721314249602848739817764518743062133735858753112190e6) }},
        {{ SC_(-1) - static_cast<T>(std::ldexp(1.0, -20)), SC_(1.04857642278181269259522681939281063878220298942888100442172e6) }},
    }};
   do_test_digamma<T>(digamma_bugs, name, "Digamma Function: Values near 0");

   BOOST_CHECK_THROW(boost::math::digamma(T(0)), std::domain_error);
   BOOST_CHECK_THROW(boost::math::digamma(T(-1)), std::domain_error);
   BOOST_CHECK_THROW(boost::math::digamma(T(-2)), std::domain_error);
}

