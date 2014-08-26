
// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: contains.cpp 49240 2008-10-10 09:21:07Z agurtovoy $
// $Date: 2008-10-10 11:21:07 +0200 (p, 10 okt 2008) $
// $Revision: 49240 $

#include <boost/mpl/contains.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/aux_/test.hpp>

MPL_TEST_CASE()
{    
    typedef vector<int,char,long,short,char,long,double,long>::type types;

    MPL_ASSERT(( contains< types,short > ));
    MPL_ASSERT_NOT(( contains< types,unsigned > ));
}