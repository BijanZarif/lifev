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
 *  @brief File containing a solver class for the 1D model.
 *
 *  @version 1.0
 *  @date 01-10-2006
 *  @author Vincent Martin
 *  @author Tiziano Passerini
 *  @author Lucia Mirabella
 *
 *  @version 2.0
 *  @author Gilles Fourestey <gilles.fourestey@epfl.ch>
 *  @date 01-08-2009
 *
 *  @version 2.1
 *  @date 21-04-2010
 *  @author Cristiano Malossi <cristiano.malossi@epfl.ch>
 *
 *  @contributors Simone Rossi <simone.rossi@epfl.ch>, Ricardo Ruiz-Baier <ricardo.ruiz@epfl.ch>
 *  @mantainer Cristiano Malossi <cristiano.malossi@epfl.ch>
 */



#ifndef OneDimensionalSolver_H
#define OneDimensionalSolver_H

// LIFEV
#include <life/lifearray/MatrixElemental.hpp>
#include <life/lifearray/VectorElemental.hpp>
#include <life/lifefem/AssemblyElemental.hpp>
#include <life/lifefem/Assembly.hpp>

#include <life/lifealg/SolverAztecOO.hpp>
#include <life/lifealg/SolverAmesos.hpp>

#include <life/lifefem/FESpace.hpp>

// LIFEV - MATHCARD
#include <lifemc/lifefem/OneDimensionalBCHandler.hpp>
#include <lifemc/lifesolver/OneDimensionalDefinitions.hpp>


namespace LifeV
{

//! OneDimensionalSolver - Solver class for the 1D model.
/*!
 *  @author Vincent Martin, Tiziano Passerini, Lucia Mirabella, Gilles Fourestey
 *
 *  ---------------------------------------------------
 *  1 dimensional hyperbolic equation:
 *  ---------------------------------------------------
 *  dU/dt  +  dF(U)/dz  +  S(U) = 0
 *  ---------------------------------------------------
 *
 *  with U = [U1,U2]^T in R^2.
 *
 *  The non linear flux function F(U) and source function S(U)
 *  are quite independent of this solver : they are taken into
 *  account only via two classes that define a vectorial function
 *  and its derivatives.
 *
 *  More precisely:
 *  two functions (M_fluxFun and M_sourceFun) have to be defined
 *  separately to allow the update (_updateFluxDer, _updateSourceDer)
 *  of the corresponding vectors (M_fluxi, M_diffFluxij,
 *  M_sourcei, M_diffSrcij). I also separated the treatment of the
 *  parameters that exist in the functions.
 *  Normally, one should be able to create a template to allow
 *  the user to select between different problems (linear, non-linear,
 *  etc.).
 *
 *  -----------------------------------------------------
 *  Solver based on the 2nd order Taylor-Galerkin scheme.
 *  -----------------------------------------------------
 *  (see for instance Formaggia-Veneziani, Mox report no 21 june 2003)
 *
 *  ---------------------------------------------------
 *  Taylor-Galerkin scheme: (explicit, U = [U1,U2]^T )
 *  ---------------------------------------------------
 *
 *  (Un+1, phi) =                             //! massFactor^{-1} * Un+1
 *  (Un, phi)                         //!            mass * U
 *  + dt     * (       Fh(Un), dphi/dz )         //!            grad * F(U)
 *  - dt^2/2 * (diffFh(Un) Sh(Un), dphi/dz )     //! gradDiffFlux(U) * S(U)
 *  + dt^2/2 * (diffSh(Un) dFh/dz(Un), phi )     //!   divDiffSrc(U) * F(U)
 *  - dt^2/2 * (diffFh(Un) dFh/dz(Un), dphi/dz ) //!stiffDiffFlux(U) * F(U)
 *  - dt     * (       Sh(Un), phi )             //!            mass * S(U)
 *  + dt^2/2 * (diffSh(Un) Sh(Un), phi )         //!  massDiffSrc(U) * S(U)
 *  ---------------------------------------------------
 *
 *  Approximation of the unknowns and non-linearities:
 *
 *  Let's define:
 *  a) (phi_i)_{i in nodes} is the basis of P1 (the "hat" functions)
 *  b) (1_{i+1/2})_{i+1/2 in elements} is the basis of P0 (constant per
 *  element). The vertices of the element "i+1/2" are the nodes "i" and "i+1".
 *
 *  Then:
 *
 *  c) Uh    is in P1 : U = sum_{i in nodes} U_i phi_i
 *
 *  d) Fh(U) is in P1 : F(U) = sum_{i in nodes} F(U_i) phi_i
 *  e) diffFh(U) is in P0 :
 *  diffFlux(U) = sum_{i+1/2 in elements} 1/2 { dF/dU(U_i) + dF/dU(U_i+1) } 1_{i+1/2}
 *  (means of the two extremal values of the cell)
 *
 *  We note that d) allows us to define easily
 *  f) dF/dz(U) = sum_{i in nodes} F(U_i) d(phi_i)/dz
 *
 *  g) Sh(U) is in P1 : S(U) = sum_{i in nodes} S(U_i) phi_i
 *  h) diffSh(U) is in P0 :
 *  diffSrc(U) = sum_{i+1/2 in elements} 1/2 { dS/dU(U_i) + dS/dU(U_i+1) } 1_{i+1/2}
 *  (means of the two extremal values of the cell)
 *
 *  -----------------------------------------------------
 *  The option taken here is to define the different tridiagonal matrix
 *  operators (div, grad, mass, stiff) and reconstruct them at each time
 *  step (as they depend on diffFlux and diffSrc). They are thus rebuilt
 *  at the element level and reassembled.
 *  Afterwards, there remains to do only some tridiagonal matrix vector
 *  products to obtain the right hand side.
 *
 *  This procedure might appear a bit memory consuming (there are 18
 *  tridiagonal matrices stored), but it has the advantage of being
 *  very clear. If it is too costly, it should be quite easy to improve
 *  it.
 *  -----------------------------------------------------
 */
class OneDimensionalSolver
{
public:

    //! @name Typedef & Enumerator
    //@{

    typedef OneDimensionalPhysics                   physics_Type;
    typedef boost::shared_ptr< physics_Type >       physicsPtr_Type;

    typedef OneDimensionalFlux                      flux_Type;
    typedef boost::shared_ptr< flux_Type >          fluxPtr_Type;

    typedef OneDimensionalSource                    source_Type;
    typedef boost::shared_ptr< source_Type >        sourcePtr_Type;

    typedef OneDimensionalData                      data_Type;
    typedef data_Type::mesh_Type                    mesh_Type;

    typedef data_Type::container2D_Type             container2D_Type;
    typedef data_Type::scalarVector_Type            scalarVector_Type;
    typedef boost::array< scalarVector_Type, 4 >    scalarVectorContainer_Type;

    typedef FESpace< mesh_Type, MapEpetra >         feSpace_Type;
    typedef boost::shared_ptr< feSpace_Type >       feSpacePtr_Type;

    typedef Epetra_Comm                             comm_Type;
    typedef boost::shared_ptr< comm_Type >          commPtr_Type;

    typedef SolverAmesos                            linearSolver_Type;
    typedef boost::shared_ptr< linearSolver_Type >  linearSolverPtr_Type;

    typedef linearSolver_Type::vector_type          vector_Type;
    typedef boost::shared_ptr< vector_Type >        vectorPtr_Type;
    typedef boost::array< vectorPtr_Type, 2 >       vectorPtrContainer_Type;

    typedef linearSolver_Type::matrix_type          matrix_Type;
    typedef boost::shared_ptr<matrix_Type>          matrixPtr_Type;
    typedef boost::array<matrixPtr_Type, 4 >        matrixPtrContainer_Type;

    typedef std::map< std::string, vectorPtr_Type > solution_Type;
    typedef boost::shared_ptr< solution_Type >      solutionPtr_Type;
    typedef solution_Type::const_iterator           solutionConstIterator_Type;

    typedef OneDimensional::bcLine_Type             bcLine_Type;
    typedef OneDimensional::bcSide_Type             bcSide_Type;
    typedef OneDimensional::bcType_Type             bcType_Type;

    //@}


    //! @name Constructors & Destructor
    //@{

    //! Empty Constructor
    /*!
     * Need a call to: setCommunicator, setProblem, setFESpace
     */
    explicit OneDimensionalSolver();

    //! Destructor
    virtual ~OneDimensionalSolver() {}

    //@}


    //! @name Methods
    //@{

    //! Build constant matrices (mass and grad)
    void buildConstantMatrices();

    //! Setup the solution using the default FESpace map.
    /*!
     * @param solution solution container
     */
    void setupSolution( solution_Type& solution ) { setupSolution( solution, M_feSpace->map() ); }

    //! Setup the solution using user defined FESpace map.
    /*!
     * @param solution solution container
     * @param map map for initializing the solution vectors
     */
    void setupSolution( solution_Type& solution, const MapEpetra& map );

    //! Initialize all the variables of the solution to a reference condition with Q=0, A=A0, and P=P_ext
    /*!
     * @param solution the solution container
     */
    void initialize( solution_Type& solution );

    //! Update the Riemann variables.
    /*!
     *  @param solution the solution container is passed with A^n, Q^n and it is updated with W1^n, W2^n
     */
    void computeW1W2( solution_Type& solution );

    //! Update the pressure.
    /*!
     *  This method compute the value of the pressure (elastic and if necessary also viscoelastic)
     *  adding it to the solution.
     *  @param solution the solution container is passed with A^n, Q^n, W1^n, W2^n and is updated with P^n
     *  @param timeStep time step
     */
    void computePressure( solution_Type& solution, const Real& timeStep );

    //! Update the ratio between A and A0.
    /*!
     *  @param solution the solution container is passed with A^n, is updated with A^n/A0-1
     */
    void computeAreaRatio( solution_Type& solution );

    //! Compute A from the area ratio: A/A0-1.
    /*!
     *  @param solution the solution container is passed with A^n/A0-1 and is updated with A^n
     */
    void computeArea( solution_Type& solution );

    //! Compute the right hand side
    /*!
     *  @param timeStep The time step.
     */
    void updateRHS( const solution_Type& solution, const Real& timeStep );

    //! Update convective term and BC. Then solve the linearized NS system
    /*!
     * @param bcH The BC handler
     * @param time the time
     * @param timeStep the time step
     */
    void iterate( OneDimensionalBCHandler& bcH, solution_Type& solution, const Real& time, const Real& timeStep );

    //! CFL computation (correct for constant mesh)
    /*!
     * @param timeStep the time step
     * @return CFL
     */
    Real computeCFL( const solution_Type& solution, const Real& timeStep ) const;

    //! Reset the output files
    void resetOutput( const solution_Type& solution );

    //! Save results on output files
    void postProcess( const solution_Type& solution );

    //@}


    //! @name Set Methods
    //@{

    //! Set problem elements
    void setProblem( const physicsPtr_Type& physics,
                     const fluxPtr_Type&    flux,
                     const sourcePtr_Type&  source );

    //! Set the communicator
    void setCommunicator( const commPtr_Type& comm );

    //! Set the FEspace
    void setFESpace( const feSpacePtr_Type& FESpace );

    //! Set the linear solver
    void setLinearSolver( const linearSolverPtr_Type& linearSolver );

    //@}


    //! @name Get Methods
    //@{

    //! Get the physics function
    const physicsPtr_Type& physics() const { return M_physics; }

    //! Get the flux function
    const fluxPtr_Type& flux() const { return M_flux; }

    //! Get the source function
    const sourcePtr_Type& source() const { return M_source; }

    //! Return the ID of the boundary node given a side.
    /*!
     *  @param bcSide Side of the boundary.
     *  @return ID of the boundary node.
     */
    UInt boundaryDOF( const bcSide_Type& bcSide ) const;

    //! Return the value of a quantity (P, A, Q, W1, W2) on a specified boundary.
    /*!
     *  Given a bcType and a bcSide it return the value of the quantity.
     *  @param bcType Type of the asked boundary value.
     *  @param bcSide Side of the boundary.
     *  @return value of the quantity on the specified side.
     */
    Real boundaryValue( const solution_Type& solution, const bcType_Type& bcType, const bcSide_Type& bcSide ) const;

    //! Return the value of the eigenvalues and eigenvectors on a specified boundary.
    /*!
     *  @param bcSide Side of the boundary.
     *  @param solution solution container.
     *  @param eigenvalues output eigenvalues.
     *  @param leftEigenvector1 output left eigenvector associated to the first eigenvalue.
     *  @param leftEigenvector1 output left eigenvector associated to the second eigenvalue.
     */
    void boundaryEigenValuesEigenVectors( const bcSide_Type& bcSide, const solution_Type& solution,
                                          container2D_Type& eigenvalues,
                                          container2D_Type& leftEigenvector1,
                                          container2D_Type& leftEigenvector2 );

    //! Get the residual container
    /*!
     * @return System residual container
     */
    const vectorPtrContainer_Type& residual() const { return M_residual; }

    //@}

private:

    //! @name Private Methods
    //@{

    //! Update the P1 flux vector from U: M_fluxi = F_h(Un) i=1,2 (works only for P1Seg elements)
    void updateFlux( const solution_Type& solution );

    //! Call _updateFlux and update the P0 derivative of flux vector from U:
    /*!
     *  M_diffFluxij = dF_h/dU(Un) i,j=1,2
     *  M_diffFluxij(elem) = 1/2 [ dF/dU(U(node1(elem))) + dF/dU(U(node2(elem))) ]
     *
     *  (mean value of the two extremal values of dF/dU)
     *  BEWARE: works only for P1Seg elements
     */
    void updatedFdU( const solution_Type& solution );

    //! Update the P1 source vector from U: M_sourcei = S_h(Un) i=1,2 (works only for P1Seg elements)
    void updateSource( const solution_Type& solution );

    //! Call _updateSource and update the P0 derivative of source vector from U:
    /*!
     *  M_diffSrcij = dS_h/dU(Un) i,j=1,2
     *  M_diffSrcij(elem) = 1/2 [ dS/dU(U(node1(elem))) + dS/dU(U(node2(elem))) ]
     *
     *  (mean value of the two extremal values of dS/dU)
     *  BEWARE: works only for P1Seg elements
     */
    void updatedSdU( const solution_Type& solution );

    //! Update the matrices
    /*!
     *  M_massMatrixDiffSrcij, M_stiffMatrixDiffFluxij
     *  M_gradMatrixDiffFluxij, and M_divMatrixDiffSrcij (i,j=1,2)
     *
     *  from the values of diffFlux(Un) and diffSrc(Un)
     *  that are computed with _updateMatrixCoefficients.
     *
     *  call of  _updateMatrixCoefficients,
     *  _updateMatrixElementalrices and _assemble_matrices.
     */
    void updateMatrices();

    //! Update the element matrices with the current element
    void updateElementalMatrices( const Real& dFdU, const Real& dSdU );

    //! Assemble the matrices
    void matrixAssemble( const UInt& ii, const UInt& jj );

    //! Update the matrices to take into account Dirichlet BC.
    /*!
     *  Modify the matrix to take into account
     *  the Dirichlet boundary conditions
     *  (works for P1Seg and canonic numbering!)
     */
    void applyDirichletBCToMatrix( matrix_Type& matrix );

    //! Apply the inertial Flux correction:
    /*!
     *  We use a finite element scheme for the correction term:
     *  given the solution of Taylor-Galerkin scheme, solve
     *  ( 1/Ah(n+1) Qtildeh(n+1), phi) +             //! 1/A * massFactor^{-1} * Un+1
     *  ( m / rho ) * ( dQtildeh(n+1)/dz, dphi/dz )  //! stiff * Qtilde(U)
     *  = ( m / rho ) *       ( dQhath(n+1)/dz, dphi/dz )  //! stiff * Qhat(U)
     *
     *  m = rho_w h0 / ( 2 sqrt(pi) sqrt(A0) )
     */
    vector_Type inertialFluxCorrection( const vector_Type& );

    //! Apply the viscoelastic Flux correction:
    /*!
     *  We use a finite element scheme for the correction term:
     *  given the solution of Taylor-Galerkin scheme, solve
     *  ( 1/(dt*Ah(n+1)) Qtildeh(n+1), phi) +        //! 1/A * massFactor^{-1} * Un+1
     *  ( gamma / rho ) *     ( dQtildeh(n+1)/dz, dphi/dz )  //! stiff * Qtilde(U)
     *  = - ( gamma / rho ) * ( dQhath(n+1)/dz, dphi/dz )  //! stiff * Qhat(U)
     *
     *  gamma = gamma_tilde / ( 2 sqrt(pi) )
     */
    vector_Type viscoelasticFluxCorrection( const vector_Type& area, const vector_Type& flowRate, const Real& timeStep, OneDimensionalBCHandler& bcHandler );

    //! Apply the longitudinal Flux correction:
    /*!
     *  We use a finite element scheme for the correction term:
     *  given the solution of Taylor-Galerkin scheme, solve
     *  ( 1/Ah(n+1) Qtildeh(n+1), phi) +             //! 1/A * massFactor^{-1} * Un+1
     *  = ( 1/Ah(n+1) Qtildeh(n), phi) +             //! 1/A * massFactor^{-1} * Un+1
     *  + ( a / rho ) *       ( d3Ahath(n+1)/dz3, phi )  //! mass * d3Ahat(U)/dz
     */
    vector_Type longitudinalFluxCorrection();

    //! L2 Projection of the second derivative of Q over P1 space.
    //scalarVector_Type                       _compute_d2Q_dx2( const scalarVector_Type& );

    //@}

    physicsPtr_Type                    M_physics;
    fluxPtr_Type                       M_flux;
    sourcePtr_Type                     M_source;
    feSpacePtr_Type                    M_feSpace;
    commPtr_Type                       M_comm;
    Displayer                          M_displayer;

    boost::shared_ptr< MatrixElemental > M_elementalMassMatrix;       //!< element mass matrix
    boost::shared_ptr< MatrixElemental > M_elementalStiffnessMatrix;  //!< element stiffness matrix
    boost::shared_ptr< MatrixElemental > M_elementalGradientMatrix;   //!< element gradient matrix
    boost::shared_ptr< MatrixElemental > M_elementalDivergenceMatrix; //!< element divergence matrix

    //! Right hand sides of the linear system i: "mass * M_Ui = M_rhsi"
    vectorPtrContainer_Type            M_rhs;

    //! Residual of the linear system
    vectorPtrContainer_Type            M_residual;

    //! Flux F(U) (in P1)
    vectorPtrContainer_Type            M_fluxVector;

    //! Source term S (in P1)
    vectorPtrContainer_Type            M_sourceVector;

    //! diffFlux = dF(U)/dU (in P0)
    scalarVectorContainer_Type         M_dFdUVector;

    //! diffSrc = dSource(U)/dU (in P0)
    scalarVectorContainer_Type         M_dSdUVector;

    //! tridiagonal mass matrix
    matrixPtr_Type                     M_homogeneousMassMatrix;

    //! tridiagonal gradient matrix
    matrixPtr_Type                     M_homogeneousGradientMatrix;

    //! tridiagonal mass matrices multiplied by diffSrcij
    matrixPtrContainer_Type            M_dSdUMassMatrix;

    //! tridiagonal stiffness matrices multiplied by diffFluxij
    matrixPtrContainer_Type            M_dFdUStiffnessMatrix;

    //! tridiagonal gradient matrices multiplied by diffFluxij
    matrixPtrContainer_Type            M_dFdUGradientMatrix;

    //! tridiagonal divergence matrices multiplied by diffSrcij
    matrixPtrContainer_Type            M_dSdUDivergenceMatrix;

    //! The linear solver
    linearSolverPtr_Type               M_linearSolver;

private:

    //! @name Unimplemented Methods
    //@{

    OneDimensionalSolver( const OneDimensionalSolver& solver );

    OneDimensionalSolver& operator=( const OneDimensionalSolver& solver );

    //@}
};

}

#endif // OneDimensionalSolver_H
