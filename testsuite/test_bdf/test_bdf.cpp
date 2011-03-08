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
    @file
    @brief

    @author Umberto Villa <uvilla@emory.edu>
    @date 14-04-2010
 */


// ===================================================
//! Includes
// ===================================================
// Tell the compiler to ignore specific kind of warnings:
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <Epetra_ConfigDefs.h>
#ifdef EPETRA_MPI
#include <mpi.h>
#include <Epetra_MpiComm.h>
#else
#include <Epetra_SerialComm.h>
#endif

//Tell the compiler to restore the warning previously silented
#pragma GCC diagnostic warning "-Wunused-variable"
#pragma GCC diagnostic warning "-Wunused-parameter"

#include <life/lifearray/MapEpetra.hpp>
#include <life/lifemesh/MeshData.hpp>
#include <life/lifemesh/MeshPartitioner.hpp>
#include <life/lifealg/SolverAztecOO.hpp>
#include <life/lifefem/Assembly.hpp>
#include <life/lifefem/AssemblyElemental.hpp>
#include <life/lifefem/BCManage.hpp>
#include <life/lifefem/FESpace.hpp>
#include <life/lifefem/TimeAdvanceBDFVariableStep.hpp>

#include <life/lifefilters/Exporter.hpp>
#include <life/lifefilters/ExporterEmpty.hpp>
#include <life/lifefilters/ExporterEnsight.hpp>
#include <life/lifefilters/ExporterHDF5.hpp>

#include "ud_functions.hpp"
#include "test_bdf.hpp"

// ===================================================
//! Namespaces & Define
// ===================================================
using namespace LifeV;

const int TOP = 6;
const int BOTTOM = 5;
const int LEFT = 3;
const int RIGHT = 4;
const int FRONT = 2;
const int BACK = 1;

typedef RegionMesh3D<LinearTetra> RegionMesh;

const std::string discretization_section = "space_discretization";

// ===================================================
//! Private Members
// ===================================================
struct test_bdf::Private
{
    Private()
    {
    }

    typedef boost::function<Real(Real const&, Real const&, Real const&,
                                 Real const&, ID const&)> fct_type;

    std::string data_file_name;
    boost::shared_ptr<Epetra_Comm> comm;
    Real errorNorm;

};

// ===================================================
//! Constructors
// ===================================================
test_bdf::test_bdf(int argc, char** argv) :
        Members(new Private)
{
    GetPot command_line(argc, argv);
    const string data_file_name =
        command_line.follow("data", 2, "-f", "--file");
    GetPot dataFile(data_file_name);

    Members->data_file_name = data_file_name;

#ifdef EPETRA_MPI
    std::cout << "Epetra Initialization" << std::endl;
    Members->comm.reset(new Epetra_MpiComm(MPI_COMM_WORLD));
#else
    Members->comm.reset(new Epetra_SerialComm() );
#endif
}

// ===================================================
//! Methods
// ===================================================
void test_bdf::run()
{

    //Useful typedef
    typedef SolverAztecOO solver_type;
    typedef VectorEpetra vector_type;
    typedef boost::shared_ptr<vector_type> vector_ptrtype;

    // Reading from data file
    GetPot dataFile(Members->data_file_name.c_str());
    // Select Pid(0) to write on std output
    bool verbose = (Members->comm->MyPID() == 0);

    if (verbose)
        std::cout << "The BDF Solver" << std::flush;

    //the forcing term
    SourceFct sf;
    // the boundary conditions
    BCFunctionBase g_Ess(AnalyticalSol::u);

    BCHandler bc;

    bc.addBC("Top", TOP, Essential, Full, g_Ess, 1);
    bc.addBC("Bottom", BOTTOM, Essential, Full, g_Ess, 1);
    bc.addBC("Left", LEFT, Essential, Full, g_Ess, 1);
    bc.addBC("Right", RIGHT, Essential, Full, g_Ess, 1);
    bc.addBC("Front", FRONT, Essential, Full, g_Ess, 1);
    bc.addBC("Back", BACK, Essential, Full, g_Ess, 1);

    //=============================================================================
    //Mesh stuff
    Members->comm->Barrier();
    MeshData meshData(dataFile, ("bdf/" + discretization_section).c_str());
    boost::shared_ptr<RegionMesh> meshPtr( new RegionMesh() );
    readMesh(*meshPtr,meshData);
    MeshPartitioner<RegionMesh> meshPart(meshPtr, Members->comm);

    //=============================================================================
    //finite element space of the solution
    FESpace<RegionMesh, MapEpetra> FeSpace(meshPart,
                                           dataFile(("bdf/"+discretization_section).c_str(), "P2"), 1, Members->comm);

    if (verbose)
        std::cout << "  Number of unknowns : "
                  << FeSpace.map().map(Unique)->NumGlobalElements()
                  << std::endl;

    bc.bcUpdate(*(FeSpace.mesh()), FeSpace.feBd(), FeSpace.dof());

    //=============================================================================
    //Fe Matrices and vectors
    MatrixElemental elmat(FeSpace.fe().nbFEDof(), 1, 1); //local matrix
    MatrixEpetra<double> matM(FeSpace.map()); //mass matrix
    boost::shared_ptr<MatrixEpetra<double> > matA_ptr(
        new MatrixEpetra<double> (FeSpace.map())); //stiff matrix
    VectorEpetra u(FeSpace.map(), Unique); // solution vector
    VectorEpetra f(FeSpace.map(), Unique); // forcing term vector

    LifeChrono chrono;
    //Assembling Matrix M
    Members->comm->Barrier();
    chrono.start();
    for (UInt iVol = 0; iVol < FeSpace.mesh()->numElements(); iVol++)
    {
        FeSpace.fe().updateJac(FeSpace.mesh()->element(iVol));
        elmat.zero();
        mass(1., elmat, FeSpace.fe(), 0, 0);
        assembleMatrix(matM, elmat, FeSpace.fe(), FeSpace.fe(), FeSpace.dof(),
                       FeSpace.dof(), 0, 0, 0, 0);
    }
    matM.globalAssemble();
    Members->comm->Barrier();
    chrono.stop();
    if (verbose)
        std::cout << "\n \n -- Mass matrix assembling time = " << chrono.diff() << std::endl
                  << std::endl;

    // ==========================================
    // Definition of the time integration stuff
    // ==========================================
    Real Tfin = dataFile("bdf/endtime", 10.0);
    Real delta_t = dataFile("bdf/timestep", 0.5);
    Real t0 = 1.;
    UInt ord_bdf = dataFile("bdf/order", 3);
    TimeAdvanceBDFVariableStep<VectorEpetra> bdf;
    bdf.setup(ord_bdf);

    //Initialization
    bdf.setInitialCondition<Real(*) (Real, Real, Real, Real, UInt), FESpace<RegionMesh, MapEpetra> >(AnalyticalSol::u, u, FeSpace, t0, delta_t);
    if (verbose) bdf.showMe();
    Members->comm->Barrier();

    //===================================================
    // post processing setup
    boost::shared_ptr<Exporter<RegionMesh> > exporter;
    std::string const exporterType =  dataFile( "exporter/type", "hdf5");

#ifdef HAVE_HDF5
    if (exporterType.compare("hdf5") == 0)
    {
        exporter.reset( new ExporterHDF5<RegionMesh > ( dataFile, "bdf_test" ) );
    }
    else
#endif
    {
        if (exporterType.compare("none") == 0)
        {
            exporter.reset( new ExporterEmpty<RegionMesh > ( dataFile, meshPart.meshPartition(), "bdf_test", Members->comm->MyPID()) );
        }
        else
        {
            exporter.reset( new ExporterEnsight<RegionMesh > ( dataFile, meshPart.meshPartition(), "bdf_test", Members->comm->MyPID()) );
        }
    }

    exporter->setPostDir( "./" );
    exporter->setMeshProcId( meshPart.meshPartition(), Members->comm->MyPID() );

    boost::shared_ptr<VectorEpetra> u_display_ptr(new VectorEpetra(
                                                      FeSpace.map(), exporter->mapType()));
    exporter->addVariable(ExporterData::Scalar, "u", u_display_ptr,
                          UInt(0),
                          UInt(FeSpace.dof().numTotalDof()));
    *u_display_ptr = u;
    exporter->postProcess(0);


    //===================================================
    //Definition of the linear solver
    SolverAztecOO az_A(Members->comm);
    az_A.setDataFromGetPot(dataFile, "bdf/solver");
    az_A.setupPreconditioner(dataFile, "bdf/prec");

    //===================================================
    // TIME LOOP
    //===================================================
    for (Real t = t0 + delta_t; t <= Tfin; t += delta_t)
    {
        Members->comm->Barrier();
        if (verbose)
            cout << "Now we are at time " << t << endl;

        matA_ptr.reset(new MatrixEpetra<double> (FeSpace.map()));

        chrono.start();
        //Assemble A
        Real coeff = bdf.coefficientDerivative(0) / delta_t;
        Real visc = nu(t);
        Real s = sigma(t);
        for (UInt i = 0; i < FeSpace.mesh()->numElements(); i++)
        {
            FeSpace.fe().updateFirstDerivQuadPt(FeSpace.mesh()->element(i));
            elmat.zero();
            mass(coeff + s, elmat, FeSpace.fe());
            stiff(visc, elmat, FeSpace.fe());
            assembleMatrix(*matA_ptr, elmat, FeSpace.fe(), FeSpace.fe(),
                           FeSpace.dof(), FeSpace.dof(), 0, 0, 0, 0);
        }

        chrono.stop();
        if (verbose)
            cout << "A has been constructed in "<< chrono.diff() << "s." << endl;

        // Handling of the right hand side
        f = (matM*bdf.rhsContribution());       //f = M*\sum_{i=1}^{orderBdf} \alpha_i u_{n-i}
        FeSpace.l2ScalarProduct(sf, f, t);  //f +=\int_\Omega{ volumeForces *v dV}
        Members->comm->Barrier();

        // Treatment of the Boundary conditions
        if (verbose) cout << "*** BC Management: " << endl;
        Real tgv = 1.;
        chrono.start();
        bcManage(*matA_ptr, f, *FeSpace.mesh(), FeSpace.dof(), bc, FeSpace.feBd(), tgv, t);
        matA_ptr->globalAssemble();
        chrono.stop();
        if (verbose) cout << chrono.diff() << "s." << endl;

        //Set Up the linear system
        Members->comm->Barrier();
        chrono.start();
        az_A.setMatrix(*matA_ptr);
        az_A.setReusePreconditioner(false);

        Members->comm->Barrier();
        az_A.solveSystem(f, u, matA_ptr);
        chrono.stop();

        bdf.shiftRight(u);

        if (verbose)
            cout << "*** Solution computed in " << chrono.diff() << "s."
                 << endl;

        Members->comm->Barrier();

        // Error in the L2
        vector_type uComputed(u, Repeated);

        Real L2_Error, L2_RelError;

        L2_Error = FeSpace.l2Error(AnalyticalSol::u, uComputed, t, &L2_RelError);

        if (verbose)
            std::cout << "Error Norm L2: " << L2_Error
                      << "\nRelative Error Norm L2: " << L2_RelError << std::endl;

        Members->errorNorm = L2_Error;

        //transfer the solution at time t.
        *u_display_ptr = u;
        exporter->postProcess(t);
    }



}

bool test_bdf::check()
{
    // Reading from data file
    GetPot dataFile(Members->data_file_name.c_str());
    return Members->errorNorm < dataFile("errorNorms/l2Error", -10e10);
}
