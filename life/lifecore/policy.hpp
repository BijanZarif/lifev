/* -*- mode: c++ -*-

This file is part of the LifeV library

Author(s): Christophe Prud'homme <prudhomm@mit.edu>
     Date: 2004-09-10

Copyright (C) 2004 EPFL

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/ 
/**
   \file policy.hpp
   \author Christophe Prud'homme <christophe.prudhomme@epfl.ch>
   \date 2004-09-10
 */
#ifndef __Policy_H
#define __Policy_H 1

namespace LifeV
{
/**
   \class policyCreationUsingNew
   default creation policy
 
   \author Christophe Prud'homme <christophe.prudhomme@epfl.ch>
*/
template <class T>
struct policyCreationUsingNew
{
    static T* create()
    {
        return new T;
    }

    static void destroy( T* p )
    {
        delete p;
    }
};

/**
   \class policyLifeTimeDefault
   default life time policy
 
   \author Christophe Prud'homme <christophe.prudhomme@epfl.ch>
*/
template <class T>
struct policyLifeTimeDefault
{
    static void scheduleDestruction( T*, void ( *pFun ) () )
    {
        std::atexit( pFun );
    }

    static void onDeadReference()
    {
        throw std::logic_error( "Dead Reference Detected" );
    }
};

}

#endif /* __Policy_H */
