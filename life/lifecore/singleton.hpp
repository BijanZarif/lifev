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
   \file Singleton.hpp
   \author Christophe Prud'homme <christophe.prudhomme@epfl.ch>
   \date 2004-09-10
 */
#ifndef __Singleton_H
#define __Singleton_H 1

#include <cstdlib>
#include <cassert>
#include <exception>
#include <stdexcept>
#include <algorithm>
#include <new>

#include <life/lifecore/policy.hpp>

namespace LifeV
{
/**
   \class singleton
   \brief implement the Singleton pattern

   A Singleton pattern implementation using the ideas
   from Alexandrescu's book "modern C++ design"
   http://www.moderncppdesign.com/


   \author Christophe Prud'homme <christophe.prudhomme@epfl.ch>
*/
template <typename T>
class singleton
{
public:

    typedef T singleton_type;

    typedef policyLifeTimeDefault<singleton_type> lifetime_policy;
    typedef policyCreationUsingNew<singleton_type> creation_policy;


    /**
       return the instance of the singleton
    */
    static singleton_type& instance();

private:
    // Helpers
    static void makeInstance();

    /**
       Singleton::makeInstance (helper for Instance)
    */
    static void destroySingleton();

    // Protection
    singleton();

    // Data
    typedef singleton_type* instance_type;
    static instance_type _S_instance;
    static bool _S_destroyed;
};

//
// Instantiate Singleton's static data
//
template <class T>
typename singleton<T>::instance_type singleton<T>::_S_instance;

template <class T>
bool singleton<T>::_S_destroyed;


template <class T>
inline
T&
singleton<T>::instance()
{
    if ( !_S_instance )
    {
        makeInstance();
    }
    return *_S_instance;
}


template <class T>
void
singleton<T>::makeInstance()
{
    if ( !_S_instance )
    {
        if ( _S_destroyed )
        {
            lifetime_policy::onDeadReference();
            _S_destroyed = false;
        }
        _S_instance = creation_policy::create();
        lifetime_policy::scheduleDestruction( _S_instance, &destroySingleton );
    }
}

template <class T>
void
singleton<T>::destroySingleton()
{
    assert( !_S_destroyed );
    creation_policy::destroy( _S_instance );
    _S_instance = 0;
    _S_destroyed = true;
}
}
#endif /* __singleton_HPP */
