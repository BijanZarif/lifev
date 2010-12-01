/* -*- mode: c++ -*-

  This file is part of the LifeV library

  Author(s): Christophe Prud'homme <christophe.prudhomme@epfl.ch>
       Date: 2004-10-04

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
   \file factory.hpp
   \author Christophe Prud'homme <christophe.prudhomme@epfl.ch>
   \date 2004-10-04
 */
#ifndef __factory_H
#define __factory_H 1

#include <map>
#include <stdexcept>
#include <string>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <life/lifecore/debug.hpp>
#include <life/lifecore/typeInfo.hpp>

namespace LifeV
{
/*! \struct factoryDefaultError
  Manages the "Unknown Type" error in an object factory.
*/
template <
typename IdentifierType,
class AbstractProduct
>
struct factoryDefaultError
{
    class Exception
            :
            public std::exception
    {
    public:
        Exception( IdentifierType /*id*/ )
                :
                std::exception(),
                _M_ex()
        {
            std::ostringstream __ex_str;
            //__ex_str << "[factory] Unknown Type : " << id;
            __ex_str << "[factory] Unknown Type : ";
            _M_ex = __ex_str.str();

        }
        ~Exception() throw()
        {}
        const char* what() const throw ()
        {
            return _M_ex.c_str();
        }
    private:
        std::string _M_ex;
    };

    static AbstractProduct* onUnknownType(IdentifierType id )
    {
        throw Exception( id );
    }
};

/*!
  \class factory
  \brief Implements a generic object factory

  \sa factoryDefaultError, factoryClone, TypeInfo

  @author Christophe Prud'homme
*/
template
<
class AbstractProduct,
typename IdentifierType,
typename ProductCreator = boost::function<AbstractProduct*()>,
template<typename, class> class factoryErrorPolicy = factoryDefaultError
>
class factory
        :
        public factoryErrorPolicy<IdentifierType,AbstractProduct>
{
public:


    /** @name Typedefs
     */
    //@{
    typedef IdentifierType identifier_type;
    typedef AbstractProduct product_type;
    typedef ProductCreator creator_type;

    typedef factoryErrorPolicy<identifier_type,product_type> super;

    //@}


    /** @name  Methods
     */
    //@{

    /**
     * Register a product.
     *
     * A product is composed of an identifier (typically a
     * std::string) and a functor that will create the associated
     * object.
     *
     * @param id identifier for the object to be registered
     * @param creator the functor that will create the registered
     * object
     *
     * @return true if registration went fine, false otherwise
     */
    bool registerProduct( const identifier_type& id, creator_type creator )
    {
        LifeV::Debug( 2200 ) << "Registered type with id : " << id << "\n";
        return _M_associations.insert( typename id_to_product_type::value_type( id, creator ) ).second;
    }

    /**
     * Unregister a product
     *
     * @param id
     * @sa registerProduct
     * @return true if unregistration went fine, false otherwise
     */
    bool unregisterProduct( const identifier_type& id )
    {
        LifeV::Debug( 2200 ) << "Unregistered type with id : " << id << "\n";
        return _M_associations.erase( id ) == 1;
    }

    /**
     * Create an object from a product registered in the factory using
     * identifier \c id
     *
     * @param id identifier of the product to instantiate
     *
     * @return the object associate with \c id
     */
    product_type* createObject( const identifier_type& id )
    {
        typename id_to_product_type::const_iterator i = _M_associations.find( id );
        if (i != _M_associations.end())
        {
            LifeV::Debug ( 2200 ) << "Creating type with id : " << id << "\n";
            return (i->second)();
        }
        LifeV::Debug( 2200 ) << "Unknown type with id : " << id << "\n";
        return super::onUnknownType( id );
    }



    //@}
private:
    typedef std::map<identifier_type, creator_type> id_to_product_type;
    id_to_product_type _M_associations;

};

/*!
  \class factoryClone
  \brief Implements a generic cloning object factory

  \sa factory, factoryDefaultError

  \author Christophe Prud'homme
*/
template <
class AbstractProduct,
class ProductCreator = boost::function<AbstractProduct* (const AbstractProduct*)>,
template<typename, class> class FactoryErrorPolicy = factoryDefaultError
>
class factoryClone
        :
        public FactoryErrorPolicy<TypeInfo, AbstractProduct>
{
public:


    /** @name Typedefs
     */
    //@{

    typedef FactoryErrorPolicy<TypeInfo,AbstractProduct> super;

    //@}

    /** @name Constructors, destructor
     */
    //@{

    //@}

    /** @name Operator overloads
     */
    //@{


    //@}

    /** @name Accessors
     */
    //@{


    //@}

    /** @name  Mutators
     */
    //@{


    //@}

    /** @name  Methods
     */
    //@{

    bool registerProduct(const TypeInfo& id, ProductCreator creator)
    {
        return _M_associations.insert( typename id_to_product_type::value_type( id, creator ) ).second;
    }

    bool unregisterProduct( const TypeInfo& id )
    {
        return _M_associations.erase(id) == 1;
    }

    AbstractProduct* createObject( const AbstractProduct* model )
    {
        if ( model == 0 ) return 0;

        typename id_to_product_type::const_iterator i = _M_associations.find( typeid(*model) );
        if ( i != _M_associations.end() )
        {
            return (i->second)(model);
        }
        return super::onUnknownType(typeid(*model));
    }

    //@}

private:
    typedef std::map<TypeInfo, ProductCreator> id_to_product_type;
    id_to_product_type _M_associations;
};


}
#endif /* __factory_H */
