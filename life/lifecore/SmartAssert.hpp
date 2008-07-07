/* -*- mode: c++ -*-

  This file is part of the LifeV library

  Author(s): Christophe Prud'homme <christophe.prudhomme@epfl.ch>
       Date: 2005-01-16

  Copyright (C) 2005 EPFL

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
   \file SmartAssert.hpp
   \author Christophe Prud'homme <christophe.prudhomme@epfl.ch>
   \date 2005-01-16
 */
#if !defined(SMART_ASSERT_H)
#define SMART_ASSERT_H

#include <string>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <map>

namespace LifeV
{
enum {

    // default behavior - just loggs this assert
    // (a message is shown to the user to the console)
    lvl_warn = 100,

    // default behavior - asks the user what to do:
    // Ignore/ Retry/ etc.
    lvl_debug = 200,

    // default behavior - throws a SmartAssert_error
    lvl_error = 300,

    // default behavior - dumps all assert context to console,
    // and aborts
    lvl_fatal = 1000
};



/**
   \class AssertContext
   \brief contains details about a failed assertion
*/
class AssertContext
{
    typedef std::string string;
public:
    AssertContext() : _M_level( lvl_debug)
        {}

    // where the assertion failed: file & line
    void setFileLine( const char * file, int line)
        {
            _M_file = file;
            _M_line = line;
        }
    const string & getContextFile() const { return _M_file; }
    int getContextLine() const { return _M_line; }

    // get/ set expression
    void setExpression( const string & str) { _M_expression = str; }
    const string & expression() const { return _M_expression; }

    typedef std::pair< string, string> val_and_str;
    typedef std::vector< val_and_str> vals_array;
    // return values array as a vector of pairs:
    // [Value, corresponding string]
    const vals_array & get_vals_array() const { return _M_vals; }
    // adds one value and its corresponding string
    void add_val( const string & val, const string & str)
        {
            _M_vals.push_back( val_and_str( val, str) );
        }

    // get/set level of assertion
    void setLevel( int nLevel) { _M_level = nLevel; }
    int get_level() const { return _M_level; }

    // get/set (user-friendly) message
    void setLevelMsg( const char * strMsg)
        {
            if ( strMsg)
                _M_msg = strMsg;
            else
                _M_msg.erase();
        }
    const string & get_level_msg() const { return _M_msg; }

private:
    // where the assertion occured
    string _M_file;
    int _M_line;

    // expression and values
    string _M_expression;
    vals_array _M_vals;

    // level and message
    int _M_level;
    string _M_msg;
};


namespace SmartAssert
{

typedef void (*assert_function_type)( const AssertContext & context);

// helpers
std::string getTypeofLevel( int nLevel);
void dumpContextSummary( const AssertContext & context, std::ostream & out);
void dumpContextDetail( const AssertContext & context, std::ostream & out);

// defaults
void defaultWarnHandler( const AssertContext & context);
void defaultDebugHandler( const AssertContext & context);
void defaultErrorHandler( const AssertContext & context);
void defaultFatalHandler( const AssertContext & context);
void defaultLogger( const AssertContext & context);

} // namespace SmartAssert

namespace Private
{
void initAssert();
void setDefaultLogStream( std::ostream & out);
void setDefaultLogName( const char * str);

// allows finding if a value is of type 'const char *'
// and is null; if so, we cannot print it to an ostream
// directly!!!
template< class T>
struct isNullFinder
{
    bool is( const T &) const
        {
            return false;
        }
};

template<>
struct isNullFinder< char*>
{
    bool is( char * const & val)
        {
            return val == 0;
        }
};

template<>
struct isNullFinder< const char*>
{
    bool is( const char * const & val)
        {
            return val == 0;
        }
};


} // namespace Private


struct Assert
{
    typedef SmartAssert::assert_function_type assert_function_type;

    // helpers, in order to be able to compile the code
    Assert & SMART_ASSERT_A;
    Assert & SMART_ASSERT_B;

    Assert( const char * expr)
        : SMART_ASSERT_A( *this),
          SMART_ASSERT_B( *this),
          _M_needs_handling( true)
        {
            _M_context.setExpression( expr);

            if ( ( logger() == 0) || handlers().size() < 4)
            {
                // used before main!
                Private::initAssert();
            }
        }

    Assert( const Assert & other)
        : SMART_ASSERT_A( *this),
          SMART_ASSERT_B( *this),
          _M_context( other._M_context),
          _M_needs_handling( true)
        {
            other._M_needs_handling = false;
        }

    ~Assert()
        {
            if ( _M_needs_handling)
                handleAssert();
        }

    template< class type>
    Assert & printCurrentValue( const type & val, const char * msg)
        {
            std::ostringstream out;

            Private::isNullFinder< type> f;
            bool bIsNull = f.is( val);
            if ( !bIsNull)
                out << val;
            else
                // null string
                out << "null";
            _M_context.add_val( out.str(), msg);
            return *this;
        }

    Assert & printContext( const char * file, int line)
        {
            _M_context.setFileLine( file, line);
            return *this;
        }

    Assert & msg( const char * strMsg)
        {
            _M_context.setLevelMsg( strMsg);
            return *this;
        }

    Assert & level( int nLevel, const char * strMsg = 0)
        {
            _M_context.setLevel( nLevel);
            _M_context.setLevelMsg( strMsg);
            return *this;
        }

    Assert & warn( const char * strMsg = 0)
        {
            return level( lvl_warn, strMsg);
        }

    Assert & debug( const char * strMsg = 0)
        {
            return level( lvl_debug, strMsg);
        }

    Assert & error( const char * strMsg = 0)
        {
            return level( lvl_error, strMsg);
        }

    Assert & error( const std::string strMsg )
        {
            return level( lvl_error, strMsg.c_str());
        }

    Assert & fatal( const char * strMsg = 0)
        {
            return  level( lvl_fatal, strMsg);
        }

    // in this case, we set the default logger, and make it
    // write everything to this file
    static void setLog( const char * strFileName)
        {
            Private::setDefaultLogName( strFileName);
            logger() = &SmartAssert::defaultLogger;
        }

    // in this case, we set the default logger, and make it
    // write everything to this log
    static void setLog( std::ostream & out)
        {
            Private::setDefaultLogStream( out);
            logger() = &SmartAssert::defaultLogger;
        }

    static void setLog( assert_function_type log)
        {
            logger() = log;
        }

    static void setHandler( int nLevel, assert_function_type handler)
        {
            handlers()[ nLevel] = handler;
        }

private:
    // handles the current assertion.
    void handleAssert()
        {
            logger()( _M_context);
            get_handler( _M_context.get_level() )( _M_context);
        }

    /*
      IMPORTANT NOTE:
      The only reason logger & handlers are functions, are
      because you might use SMART_ASSERT before main().

      In this case, since they're statics, they might not
      be initialized. However, making them functions
      will make it work.
    */

    // the log
    static assert_function_type & logger()
        {
            static assert_function_type inst;
            return inst;
        }

    // the handler
    typedef std::map< int, assert_function_type> handlers_collection;
    static handlers_collection & handlers()
        {
            static handlers_collection inst;
            return inst;
        }

    static assert_function_type get_handler( int nLevel)
        {
            handlers_collection::const_iterator found = handlers().find( nLevel);
            if ( found != handlers().end() )
                return found->second;
            else
                // we always assume the debug handler has been set
                return handlers().find( lvl_debug)->second;
        }

private:
    AssertContext _M_context;
    mutable bool _M_needs_handling;

};


namespace SmartAssert
{
inline ::LifeV::Assert make_assert( const char * expr)
{
    return ::LifeV::Assert( expr);
}
} // namespace SmartAssert

} // LifeV Namespace

#ifdef LIFEV_SMART_ASSERT_DEBUG_MODE

#if LIFEV_SMART_ASSERT_DEBUG_MODE == 1
#define LIFEV_SMART_ASSERT_DEBUG
#else
#undef LIFEV_SMART_ASSERT_DEBUG
#endif

#else

// defaults
#ifndef NDEBUG
#define LIFEV_SMART_ASSERT_DEBUG
#else
#undef LIFEV_SMART_ASSERT_DEBUG
#endif

#endif


#ifdef LIFEV_SMART_ASSERT_DEBUG
// "debug" mode
#define LIFEV_SMART_ASSERT( expr) \
    if ( (expr) ) ; \
    else ::LifeV::SmartAssert::make_assert( #expr).printContext( __FILE__, __LINE__).SMART_ASSERT_A \
    /**/

#else
// "release" mode
#define LIFEV_SMART_ASSERT( expr) \
    if ( true ) ; \
    else ::LifeV::SmartAssert::make_assert( "").SMART_ASSERT_A \
    /**/

#endif // ifdef LIFEV_SMART_ASSERT_DEBUG

// LIFEV_ASSERT is a equivalent to LIFEV_SMART_ASSERT
#define LIFEV_ASSERT( expr) LIFEV_SMART_ASSERT(expr)


#define LIFEV_SMART_VERIFY( expr) \
    if ( (expr) ) ; \
    else ::LifeV::SmartAssert::make_assert( #expr).error().printContext( __FILE__, __LINE__).SMART_ASSERT_A \
    /**/
#define LIFEV_VERIFY( expr) LIFEV_SMART_VERIFY(expr)


#define SMART_ASSERT_A(x) LIFEV_SMART_ASSERT_OP(x, B)
#define SMART_ASSERT_B(x) LIFEV_SMART_ASSERT_OP(x, A)

#define LIFEV_SMART_ASSERT_OP(x, next) \
    SMART_ASSERT_A.printCurrentValue((x), #x).SMART_ASSERT_ ## next \
    /**/

#endif
