
# Copyright Aleksey Gurtovoy 2001-2006
#
# Distributed under the Boost Software License, Version 1.0. 
# (See accompanying file LICENSE_1_0.txt or copy at 
# http://www.boost.org/LICENSE_1_0.txt)
#
# See http://www.boost.org/libs/mpl for documentation.

# $Id: preprocess_vector.py 49241 2008-10-10 09:24:39Z agurtovoy $
# $Date: 2008-10-10 11:24:39 +0200 (p, 10 okt 2008) $
# $Revision: 49241 $

import preprocess
import os.path

preprocess.main(
      [ "no_ctps", "plain", "typeof_based" ]
    , "vector"
    , os.path.join( "boost", "mpl", "vector", "aux_", "preprocessed" )
    )
