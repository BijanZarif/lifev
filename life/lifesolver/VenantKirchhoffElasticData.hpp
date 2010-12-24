//@HEADER
/*
*******************************************************************************

    Copyright (C) 2004, 2005, 2007 EPFL, Politecnico di Milano, INRIA
    Copyright (C) 2010 EPFL, Politecnico di Milano, Emory University

    This file is part of LifeV.

    LifeV is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LifeV is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LifeV.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************
*/
//@HEADER

/*!
 *  @file
 *  @brief DataElasticStructure - File containing a data container for solid problems with elastic structure
 *
 *  @version 1.0
 *  @date 01-10-2003
 *  @author M.A. Fernandez
 *
 *  @version 1.18
 *  @date 10-06-2010
 *  @author Cristiano Malossi
 *
 *  @contributor Paolo Tricerri <paolo.tricerri@epfl.ch>
 *  @maintainer  Paolo Tricerri <paolo.tricerri@epfl.ch>
 */

#ifndef _DATAELASTICSTRUCTURE_H_
#define _DATAELASTICSTRUCTURE_H_

#include <string>
#include <iostream>
#include <map>
#include <life/lifefilters/GetPot.hpp>
#include <boost/shared_ptr.hpp>
#include <life/lifecore/LifeV.hpp>
#include <life/lifecore/StringUtility.hpp>
#include <life/lifefem/TimeData.hpp>

namespace LifeV
{

//! DataElasticStructure - Data container for solid problems with elastic structure
class VenantKirchhoffElasticData: public TimeData
{
public:

    //! @name Type definitions
    //@{

    typedef TimeData                                                  Time_Type;
    typedef boost::shared_ptr< Time_Type >                            TimePtr_Type;

    typedef std::map<UInt, Real>                                      MaterialContainer_Type;
    typedef MaterialContainer_Type::const_iterator                    MaterialContainer_ConstIterator;

    //@}


    //! @name Constructors & Destructor
    //@{

    //! Empty Constructor
    VenantKirchhoffElasticData();

    //! Copy constructor
    /*!
     * @param VenantKirchhoffElasticData - VenantKirchhoffElasticData
     */
    VenantKirchhoffElasticData( const VenantKirchhoffElasticData& VenantKirchhoffElasticData );

    //@}


    //! @name Operators
    //@{

    //! Operator=
    /*!
     * @param VenantKirchhoffElasticData - VenantKirchhoffElasticData
     */
    VenantKirchhoffElasticData& operator=( const VenantKirchhoffElasticData& VenantKirchhoffElasticData );

    //@}


    //! @name Methods
    //@{

    //! Read the dataFile and set all the quantities
    /*!
     * @param dataFile data file
     * @param section section of the file
     */
    void setup( const GetPot& dataFile, const std::string& section = "solid" );

    //! Display the values
    void showMe( std::ostream& output = std::cout ) const;

    //@}


    //! @name Set methods
    //@{

    //! Set data time container
    /*!
     * @param TimeData shared_ptr to TimeData container
     */
    void setTimeData( const TimePtr_Type TimeData );

    //! Set density
    /*!
     * @param density solid density value
     */
    void setDensity( const Real& density );

    //! Set thickness
    /*!
     * @param thickness solid thickness value
     */
    void setThickness( const Real& thickness );

    //! Set poisson
    /*!
     * @param poisson solid poisson value
     * @param material material ID (1 by default)
     */
    void setPoisson( const Real& poisson, const UInt& material = 1 );

    //! Set Young modulus
    /*!
     * @param Young solid young modulus value
     * @param material material ID (1 by default)
     */
    void setYoung( const Real& young, const UInt& material = 1 );

    //@}


    //! @name Get methods
    //@{

    //! Get data time container
    /*!
     * @return shared_ptr to TimeData container
     */
    const TimePtr_Type getdataTime() const;

    //! Get solid density
    /*!
     * @return Solid density
     */
    const Real& getRho() const;

    //! Get solid thickness
    /*!
     * @return Solid thickness
     */
    const Real& getThickness() const;

    //! Get solid poisson coefficient
    /*!
     * @param material material ID (1 by default)
     * @return Solid poisson coefficient
     */
    Real getPoisson( const UInt& material = 1 ) const;

    //! Get solid young modulus
    /*!
     * @param material material ID (1 by default)
     * @return Solid young modulus
     */
    Real getYoung( const UInt& material = 1 ) const;

    //! Get solid first lame coefficient
    /*!
     * @param material material ID (1 by default)
     * @return Solid first Lame coefficient
     */
    Real getLambda( const UInt& material = 1 ) const;

    //! Get solid second Lame coefficient
    /*!
     * @param material material ID (1 by default)
     * @return Solid second Lame coefficient
     */
    Real getMu( const UInt& material = 1 ) const;

    //! Get FE order
    /*!
     * @return FE order
     */
    const std::string& getOrder() const;

    //! Get solid amplification factor
    /*!
     * @return Solid amplification factor
     */
    const Real& getFactor() const;

    //! Get verbose level
    /*!
     * @return verbose level
     */
    const UInt& getVerbose() const;

    //! Get solid type
    /*!
     * @return solid type
     */
    const std::string& getSolidType();

    //! Get whether to use or not exact Jacobian
    /*!
     * @return true: if using exact Jacobian, false: otherwise
     */
    const bool& getUseExactJacobian() const;

    //@}

private:

    //! Data containers for time and mesh
    TimePtr_Type           M_time;

    //! Physics
    Real                   M_density;
    Real                   M_thickness;

    MaterialContainer_Type M_poisson;
    MaterialContainer_Type M_young;

    //! Space discretization
    std::string            M_order;

    //! Miscellaneous
    Real                   M_factor;  // amplification factor for deformed mesh
    UInt                   M_verbose; // temporal output verbose

    std::string            M_solidType;
    bool                   M_useExactJacobian;

};

} // end namespace LifeV

#endif // end _DATAELASTICSTRUCTURE_H_
