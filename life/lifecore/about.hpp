/* -*- mode: c++ -*-

  This file is part of the LifeV library

  Author(s): Christophe Prud'homme <christophe.prudhomme@epfl.ch>
       Date: 2005-03-17

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
   \file about.hpp
   \author Christophe Prud'homme <christophe.prudhomme@epfl.ch>
   \date 2005-03-17
 */
#ifndef __about_H
#define __about_H 1

#include <vector>
#include <string>


namespace LifeV
{
class AboutPersonPrivate;
class AboutDataPrivate;

/**
 * This structure is used to store information about a person or developer.
 * It can store the person's name, a task, an email address and a
 * link to a home page. This class is intended for use in the
 * AboutData class, but it can be used elsewhere as well.
 * Normally you should at least define the person's name.
 *
 * Example Usage within a main():
 *
 * AboutData about("hello", "0.1",
 *                 "A LifeV version of Hello, world!"),
 *                 AboutData::License_LGPL,
 *                 "Copyright (c) 2003 Developer");
 *
 *   about.addAuthor("Joe Developer", "developer", "joe@host.com", 0);
 *   about.addCredit("Joe User", "A lot of bug reports"),
 *                   "joe.user@host.org", 0);
 */
class AboutPerson
{
public:
    /**
     * Convenience constructor
     *
     * @param name The name of the person.
     *
     * @param task The task of this person. This string should be
     *              marked for translation, e.g.
     *
     * @param emailAddress The email address of the person.
     *
     * @param webAddress Home page of the person.
     */
    AboutPerson( std::string name,
                 std::string task,
                 std::string emailAddress,
                 std::string webAddress )
        :
        _M_Name( name ),
        _M_Task( task ),
        _M_EmailAddress( emailAddress ),
        _M_WebAddress( webAddress )
    {
    }
    /**
     * @internal
     * Don't use.
     */
    AboutPerson()
        :
        _M_Name(),
        _M_Task(),
        _M_EmailAddress(),
        _M_WebAddress()
        {}

    /**
     * @internal
     * Don't use.
     */
    AboutPerson( AboutPerson const& ap )
        :
        _M_Name( ap._M_Name ),
        _M_Task( ap._M_Task ),
        _M_EmailAddress( ap._M_EmailAddress ),
        _M_WebAddress( ap._M_WebAddress )
        {}

    AboutPerson& operator=( AboutPerson const& __ap )
        {
            if ( this != & __ap )
            {
                _M_Name = __ap._M_Name;
                _M_Task = __ap._M_Task;
                _M_EmailAddress = __ap._M_EmailAddress;
                _M_WebAddress = __ap._M_WebAddress;
            }
            return *this;
        }
    /**
     * The person's name
     * @return the person's name (can be std::string, if it has been
     *           constructed with a null name)
     */
    std::string name() const;

    /**
     * The person's task
     * @return the person's task (can be std::string, if it has been
     *           constructed with a null task)
     */
    std::string task() const;

    /**
     * The person's email address
     * @return the person's email address (can be std::string, if it has been
     *           constructed with a null email)
     */
    std::string emailAddress() const;

    /**
     * The home page or a relevant link
     * @return the persons home page (can be std::string, if it has been
     *           constructed with a null home page)
     */
    std::string webAddress() const;

private:
    std::string  _M_Name;
    std::string  _M_Task;
    std::string  _M_EmailAddress;
    std::string  _M_WebAddress;

    AboutPersonPrivate *d;
};

/**
 * This class is used to store information about a program. It can store
 * such values as version number, program name, home page, email address
 * for bug reporting, multiple authors and contributors
 * (using AboutPerson), license and copyright information.
 *
 * @brief Holds information needed by the "About" box and other
 * classes.
 * @author Christophe Prud'homme <christophe.prudhomme@epfl.ch>
 */
class AboutData
{
public:
    /**
     * Descibes the license of the software.
     */
    enum LicenseKey
    {
        License_Custom = -2,
        License_File = -1,
        License_Unknown = 0,
        License_GPL  = 1,
        License_GPL_V2 = 1,
        License_LGPL = 2,
        License_LGPL_V2 = 2,
        License_BSD  = 3,
        License_Artistic = 4,
        License_QPL = 5,
        License_QPL_V1_0 = 5
    };

public:
    /**
     * Constructor.
     *
     * @param appName The program name used internally. Example: "kedit"
     *
     * @param programName A displayable program name string. This string
     *        should be marked for translation. Example: I18N_NOOP("KEdit")
     *
     * @param version The program version string.
     *
     * @param shortDescription A short description of what the program does.
     *        This string should be marked for translation.
     *        Example: I18N_NOOP("A simple text editor.")
     *
     * @param licenseType The license identifier. Use setLicenseText if
     *        you use a license not predefined here.
     *
     * @param copyrightStatement A copyright statement, that can look like this:
     *        "(c) 1999-2000, Name". The string specified here is not modified
     *        in any manner. The author information from addAuthor is not
     *        used.
     *
     * @param text Some free form text, that can contain any kind of
     *        information. The text can contain newlines. This string
     *        should be marked for translation.
     *
     * @param homePageAddress The program homepage string.
     *        Start the address with "http://". "http://some.domain" is
     *        is correct, "some.domain" is not.
     *
     * @param bugsEmailAddress The bug report email address string.
     *        This defaults to the lifev-dev@webmail.polimi.it mailing list.
     *
     */
    AboutData( const char* appName,
               const char* programName,
               const char* version,
               const char* shortDescription = "",
               int licenseType = License_Unknown,
               const char* copyrightStatement = "",
               const char* text = "",
               const char* homePageAddress = "",
               const char* bugsEmailAddress = "lifev-dev@webmail.polimi.it"
               );

    AboutData( AboutData const& ad );

    ~AboutData();

    /**
     * Defines an author. You can call this function as many times you
     * need. Each entry is appended to a list. The person in the first entry
     * is assumed to be the leader of the project.
     *
     * @param name The developer's name in UTF-8 encoding.
     *
     * @param task What the person is responsible for. This text can contain
     *             newlines. It should be marked for translation like this:
     *             I18N_NOOP("Task description..."). Can be 0.
     *
     * @param emailAddress An Email address where the person can be reached.
     *                     Can be 0.
     *
     * @param webAddress The person's homepage or a relevant link.
     *        Start the address with "http://". "http://some.domain" is
     *        is correct, "some.domain" is not. Can be 0.
     *
     */
    void addAuthor( std::string const & name,
                    std::string const & task=0,
                    std::string const & emailAddress=0,
                    std::string const & webAddress=0 );

    /**
     * Defines a person that deserves credit. You can call this function
     * as many times you need. Each entry is appended to a list.
     *
     * @param name The person's name in UTF-8 encoding.
     *
     * @param task What the person has done to deserve the honor. The
     *        text can contain newlines. It should be marked for
     *        translation like this: I18N_NOOP("Task description...")
     *        Can be 0.
     *
     * @param emailAddress An Email address when the person can be reached.
     *        Can be 0.
     *
     * @param webAddress The person's homepage or a relevant link.
     *        Start the address with "http://". "http://some.domain" is
     *        is correct, "some.domain" is not. Can be 0.
     *
     */
    void addCredit( std::string const & name,
                    std::string const & task=0,
                    std::string const & emailAddress=0,
                    std::string const & webAddress=0 );

    /**
     * Defines a license text.
     *
     * The text will be translated if it got marked for
     * translations with the I18N_NOOP() macro.
     *
     * Example:
     * \code
     * setLicenseText( I18N_NOOP("This is my license"));
     * \endcode
     *
     * NOTE: No copy of the text is made.
     *
     * @param license The license text in utf8 encoding.
     */
    void setLicenseText( std::string const & license );

    /**
     * Defines the program name used internally.
     *
     * @param appName The application name. Example: "kate".
     */
    void setAppName( std::string const & appName );

    /**
     * Defines the displayable program name string.
     *
     * @param programName The program name. This string should be
     *        marked for translation.
     *        Example: I18N_NOOP("Advanced Text Editor").
     */
    void setProgramName( const char* programName );

    /**
     * Defines the program version string.
     *
     * @param version The program version.
     */
    void setVersion( const char* version );

    /**
     * Defines a short description of what the program does.
     *
     * @param shortDescription The program description This string should be marked
     *        for translation. Example: I18N_NOOP("An advanced text editor
     *        with syntax highlithing support.").
     */
    void setShortDescription( std::string const & shortDescription );

    /**
     * Defines the license identifier.
     *
     * @param licenseKey The license identifier.
     */
    void setLicense( LicenseKey licenseKey);

    /**
     * Defines the copyright statement to show when displaying the license.
     *
     * @param copyrightStatement A copyright statement, that can look like
     *        this: "(c) 1999-2000, Name". The string specified here is not
     *        modified in any manner. The author information from addAuthor
     *        is not used.
     */
    void setCopyrightStatement( std::string const & copyrightStatement );

    /**
     * Defines the additional text to show in the about dialog.
     *
     * @param otherText Some free form text, that can contain any kind of
     *        information. The text can contain newlines. This string
     *        should be marked for translation.
     */
    void setOtherText( std::string const & otherText );

    /**
     * Defines the program homepage.
     *
     * @param homepage The program homepage string.
     *        Start the address with "http://". "http://www.lifev.org" is
     *        is correct, "www.lifev.org" is not.
     */
    void setHomepage( std::string const & homepage );

    /**
     * Defines the address where bug reports should be sent.
     *
     * @param bugAddress The bug report email address string.
     *        This defaults to the lifev-dev@ mailing list.
     */
    void setBugAddress( std::string const & bugAddress );

    /**
     * Defines the product name wich will be used in the KBugReport dialog.
     * By default it's the appName, but you can overwrite it here to provide
     * support for special components e.g. 'product/component' like
     * 'kontact/summary'.
     *
     * @param name The name of product
     */
    void setProductName( std::string const & name );

    /**
     * Returns the application's internal name.
     * @return the internal program name.
     */
    std::string appName() const;

    /**
     * Returns the application's product name, which will be used in KBugReport
     * dialog. By default it returns appName(), otherwise the one which is set
     * with setProductName()
     *
     * @return the product name.
     */
    std::string productName() const;
    /**
     * Returns the translated program name.
     * @return the program name (translated).
     */
    std::string programName() const;

    /**
     * Returns the program's version.
     * @return the version string.
     */
    std::string version() const;

    /**
     * Returns a short, translated description.
     * @return the short description (translated). Can be
     *         std::string::null if not set.
     */
    std::string shortDescription() const;

    /**
     * Returns the application homepage.
     * @return the application homepage URL. Can be std::string::null if
     *         not set.
     */
    std::string homepage() const;

    /**
     * Returns the email address for bugs.
     * @return the email address where to report bugs.
     */
    std::string bugAddress() const;

    /**
     * Returns a list of authors.
     * @return author information (list of persons).
     */
    const std::vector<AboutPerson>& authors() const;

    /**
     * Returns a list of persons who contributed.
     * @return credit information (list of persons).
     */
    const std::vector<AboutPerson>& credits() const;

    /**
     * Returns a translated, free form text.
     * @return the free form text (translated). Can be std::string::null if not set.
     */
    std::string otherText() const;

    /**
     * Returns the license. If the licenseType argument of the constructor has been
     * used, any text defined by setLicenseText is ignored,
     * and the standard text for the chosen license will be returned.
     *
     * @return The license text.
     */
    std::string license() const;

    /**
     * Returns the copyright statement.
     * @return the copyright statement. Can be std::string::null if not set.
     */
    std::string copyrightStatement() const;

private:
    std::string _M_AppName;
    std::string _M_ProgramName;
    std::string _M_ProductName;
    std::string _M_Version;
    std::string _M_ShortDescription;
    int _M_LicenseKey;
    std::string _M_CopyrightStatement;
    std::string _M_OtherText;
    std::string _M_HomepageAddress;
    std::string _M_BugEmailAddress;
    std::vector<AboutPerson> _M_AuthorList;
    std::vector<AboutPerson> _M_CreditList;
    std::string _M_LicenseText;

    AboutDataPrivate *d;
};

/**
 * outpout stream an AboutData structure
 *
 * @param os ostream to use to stream the about data
 * @param about data to stream
 *
 * @return the ostream
 */
std::ostream& operator<<( std::ostream& os, AboutData const& about );

}
#endif /* __about_H */
