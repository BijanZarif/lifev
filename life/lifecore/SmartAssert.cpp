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
   \file SmartAssert.cpp
   \author Christophe Prud'homme <christophe.prudhomme@epfl.ch>
   \date 2005-01-16
 */
#include <cstdlib>

#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>

#include <life/lifecore/SmartAssert.hpp>


void breakIntoDebugger()
{
  // MSVC, BCB,
#if (defined _MSC_VER) || (defined __BORLANDC__)
  __asm { int 3 };
#elif defined(__GNUC__)
  // GCC
  __asm ("int $0x3");
#else
    #  error Please supply instruction to break into code
#endif
}

namespace LifeV
{
namespace
{
// in case we're logging using the default logger...
struct stream_holder
{
    stream_holder() : out_( 0), owns_( false) {}
    ~stream_holder()
        {
            if ( owns_)
                delete out_;
            out_ = 0;
        }
    std::ostream * out_;
    bool owns_;
};
// information about the stream we write to, in case
// we're using the default logger
stream_holder default_logger_info;

// intitializes the SMART_ASSERT library
struct assert_initializer
{
    assert_initializer()
        {
            Private::initAssert();
        }
}
init;
} // anonymous namespace

namespace SmartAssert
{

// returns a message corresponding to the type of level
std::string getTypeofLevel( int nLevel)
{
    switch ( nLevel)
    {
        case lvl_warn: return "Warning";
        case lvl_debug: return "Assertion failed";
        case lvl_error: return "Assertion failed (Error)";
        case lvl_fatal: return "Assertion failed (FATAL)";
        default:
        {
            std::ostringstream out;
            out << "Assertion failed (level=" << nLevel << ")";
            return out.str();
        }
    };
}

// helpers, for dumping the assertion context
void dumpContextSummary( const AssertContext & context, std::ostream & out)
{
    out << "\n" << getTypeofLevel( context.get_level() )
        << " in " << context.getContextFile() << ":" << context.getContextLine() << '\n';
    if ( !context.get_level_msg().empty())
        // we have a user-friendly message
        out << context.get_level_msg();
    else
        out << "\nExpression: " << context.expression();
    out << std::endl;
}

void dumpContextDetail( const AssertContext & context, std::ostream & out)
{
    out << "\n" << getTypeofLevel( context.get_level() )
        << " in " << context.getContextFile() << ":" << context.getContextLine() << '\n';
    if ( !context.get_level_msg().empty())
        out << "User-friendly msg: '" << context.get_level_msg() << "'\n";
    out << "\nExpression: '" << context.expression() << "'\n";

    typedef AssertContext::vals_array vals_array;
    const vals_array & aVals = context.get_vals_array();
    if ( !aVals.empty() )
    {
        bool bFirstTime = true;
        vals_array::const_iterator first = aVals.begin(), last = aVals.end();
        while ( first != last)
        {
            if ( bFirstTime)
            {
                out << "Values: ";
                bFirstTime = false;
            }
            else
            {
                out << "        ";
            }
            out << first->second << "='" << first->first << "'\n";
            ++first;
        }
    }
    out << std::endl;
}

///////////////////////////////////////////////////////
// logger

void defaultLogger( const AssertContext & context)
{
    if ( default_logger_info.out_ == 0)
        return;
    dumpContextDetail( context, *( default_logger_info.out_) );
}

///////////////////////////////////////////////////////
// handlers

// warn : just dump summary to console
void defaultWarnHandler( const AssertContext & context)
{
    dumpContextSummary( context, std::cout);
}


// debug: ask user what to do
void defaultDebugHandler( const AssertContext & context)
{
    static bool ignore_all = false;
    if ( ignore_all)
        // ignore All asserts
        return;
    typedef std::pair< std::string, int> file_and_line;
    static std::set< file_and_line> ignorer;
    if ( ignorer.find( file_and_line( context.getContextFile(), context.getContextLine())) != ignorer.end() )
        // this is Ignored Forever
        return;

    dumpContextSummary( context, std::cerr );
    std::cerr << "\nPress (I)gnore/ Igore (F)orever/ Ignore (A)ll/ (D)ebug/ A(b)ort: ";
    std::cerr.flush();
    char ch = 0;

    bool bContinue = true;
    while ( bContinue && std::cin.get( ch))
    {
        bContinue = false;
        switch ( ch)
        {
            case 'i': case 'I':
                // ignore
                break;

            case 'f': case 'F':
                // ignore forever
                ignorer.insert( file_and_line( context.getContextFile(), context.getContextLine()));
                break;

            case 'a': case 'A':
                // ignore all
                ignore_all = true;
                break;

            case 'd': case 'D':
                // break
                breakIntoDebugger();
                break;

            case 'b': case 'B':
                abort();
                break;

            default:
                bContinue = true;
                break;
        }
    }
}


// error : throw a runtime exception
void defaultErrorHandler( const AssertContext & context)
{
    std::ostringstream out;
    dumpContextSummary( context, out);
    throw std::runtime_error( out.str());
}


// fatal : dump error and abort
void defaultFatalHandler( const AssertContext & context)
{
    dumpContextDetail( context, std::cerr);
    abort();
}


} // namespace SmartAssert


namespace Private
{

void initAssert()
{
    ::LifeV::Assert::setLog( &::LifeV::SmartAssert::defaultLogger);
    ::LifeV::Assert::setHandler( lvl_warn, &::LifeV::SmartAssert::defaultWarnHandler);
    ::LifeV::Assert::setHandler( lvl_debug, &::LifeV::SmartAssert::defaultDebugHandler);
    ::LifeV::Assert::setHandler( lvl_error, &::LifeV::SmartAssert::defaultErrorHandler);
    ::LifeV::Assert::setHandler( lvl_fatal, &::LifeV::SmartAssert::defaultFatalHandler);
}

// sets the default logger to write to this stream
void setDefaultLogStream( std::ostream & out)
{
    default_logger_info.out_ = &out;
    default_logger_info.owns_ = false;
}

// sets the default logger to write to this file
void setDefaultLogName( const char * str)
{
    default_logger_info.owns_ = false;
    default_logger_info.out_ = new std::ofstream( str);
    default_logger_info.owns_ = true;
}


} // namespace Private

}
