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
 *  @brief File containing a base class for 1D model flux function.
 *
 *  @date 15-04-2010
 *  @author Cristiano Malossi <cristiano.malossi@epfl.ch>
 *
 *  @contributors Simone Rossi <simone.rossi@epfl.ch>, Ricardo Ruiz-Baier <ricardo.ruiz@epfl.ch>
 *  @mantainer  Cristiano Malossi <cristiano.malossi@epfl.ch>
 */

#ifndef OneDimensionalFlux_H
#define OneDimensionalFlux_H

// LIFEV - MATHCARD
#include <lifemc/lifesolver/OneDimensionalPhysics.hpp>

namespace LifeV
{

//! OneDimensionalFlux - Base class for the flux function F of the 1D hyperbolic problem.
/*!
 *  @author Cristiano Malossi
 */
class OneDimensionalFlux
{

public:

    //! @name Type definitions and Enumerators
    //@{

    typedef FactorySingleton< Factory< OneDimensionalFlux, OneDimensional::fluxTerm_Type > > factoryFlux_Type;

    typedef OneDimensionalPhysics                       physics_Type;
    typedef boost::shared_ptr< physics_Type >           physicsPtr_Type;

    typedef OneDimensionalData::container2D_Type        container2D_Type;

    //@}


    //! @name Constructors & Destructor
    //@{

    //! Constructor
    explicit OneDimensionalFlux() : M_physics() {}

    explicit OneDimensionalFlux( const physicsPtr_Type physics ) : M_physics( physics ) {}

    //! Do nothing destructor
    virtual ~OneDimensionalFlux() {}

    //@}


    //! @name Virtual Methods
    //@{

    //! operator()
    /*!
     *  F = [Q, alpha*Q^2/A + beta0*beta1/(rho*(beta1+1)*A0^beta1) * A^(beta1+1) ]
     *  \param iNode : is the index position for the parameters
     *  when they are space dependent.
     *  This is NOT pretty. I should try to remove this dependency. VM 09/04
     */
    virtual Real flux( const Real& A, const Real& Q, const ID& ii, const UInt& iNode ) const = 0;


    //! Jacobian matrix
    /*!
     *  Hij = dFi/dxj
     *
     *  diff(1,1) = dF1/dx1    diff(1,2) = dF1/dx2
     *  diff(2,1) = dF2/dx1    diff(2,2) = dF2/dx2
     */
    virtual Real dFdU( const Real& A, const Real& Q, const ID& ii, const ID& jj, const UInt& iNode ) const  = 0;

    //! Second derivative tensor d2Fi/(dxj dxk)
    /*!
     *  diff2(1,1,1) = d2F1/dx1dx1    diff2(1,1,2) = d2F1/dx1dx2
     *  diff2(1,2,1) = d2F1/dx2dx1    diff2(1,2,2) = d2F1/dx2dx2
     *  diff2(2,1,1) = d2F2/dx1dx1    diff2(2,1,2) = d2F2/dx1dx2
     *  diff2(2,2,1) = d2F2/dx2dx1    diff2(2,2,2) = d2F2/dx2dx2
     *
     *  with d2Fi/dx1dx2 = d2Fi/dx2dx1
     */
//    virtual Real diff2( const Real& A, const Real& Q,
//                        const ID& ii,   const ID& jj, const ID& kk,
//                        const UInt& iNode = 0 ) const  = 0;

    //! Eigenvalues and eigenvectors of the Jacobian matrix dFi/dxj
    /*!
     * \param eigi is the ith eigen value of the matrix dF/dx (i=1,2).
     * \param lefteigvecij is the jth component of the left eigen vector associated to eigi. (i,j=1,2)
     */
    virtual void eigenValuesEigenVectors( const Real& A, const Real& Q,
                                          container2D_Type& eigenvalues,
                                          container2D_Type& leftEigenvector1,
                                          container2D_Type& leftEigenvector2,
                                          const UInt& iNode ) const = 0;

    //! Compute the derivative of the eigenvalues and of the eigenvectors of the Jacobian matrix
    virtual void deltaEigenValuesEigenVectors( const Real& A, const Real& Q,
                                               container2D_Type& deltaEigenvalues,
                                               container2D_Type& deltaLeftEigenvector1,
                                               container2D_Type& deltaLeftEigenvector2,
                                               const UInt& iNode ) const = 0;

    //@}


    //! @name Set Methods
    //@{

    void setPhysics( const physicsPtr_Type& physics ) { M_physics = physics; }

    //@}


    //! @name Get Methods
    //@{

    physicsPtr_Type physics() const { return M_physics; }

    //@}

protected:

    physicsPtr_Type                 M_physics;

private:

    //! @name Unimplemented Methods
    //@{

    OneDimensionalFlux& operator=( const physicsPtr_Type& physics);

    //@}

};

}

#endif // OneDimensionalFlux_H
