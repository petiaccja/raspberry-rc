
// Copyright Aleksey Gurtovoy 2000-2004
// Copyright David Abrahams 2003-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: replace.cpp 49240 2008-10-10 09:21:07Z agurtovoy $
// $Date: 2008-10-10 11:21:07 +0200 (p, 10 okt 2008) $
// $Revision: 49240 $

#include <boost/mpl/replace.hpp>

#include <boost/mpl/list.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/aux_/test.hpp>

MPL_TEST_CASE()
{
    typedef list<int,float,char,float,float,double> types;
    typedef replace< types,float,double >::type result;
    typedef list<int,double,char,double,double,double> answer;
    MPL_ASSERT(( equal< result,answer > ));
}
