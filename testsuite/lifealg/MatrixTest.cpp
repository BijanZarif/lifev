/* -*- mode: c++ -*-

  This file is part of the LifeV library

  Author(s): Christophe Prud'homme <christophe.prudhomme@epfl.ch>
       Date: 2004-08-29

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
   \file MatrixTest.cpp
   \author Christophe Prud'homme <christophe.prudhomme@epfl.ch>
   \date 2004-08-29
*/
#include <MatrixTest.hpp>

namespace LifeV
{
//
// Mass
//
MatrixMass::MatrixMass( int n )
        :
        _M_mat(0),
        _M_pattern(0),
        _M_val()
{
    // Defining constants.

    value_type sub  = 1.0/value_type(n+1);
    value_type diag = 4.0/value_type(n+1);

    // Defining the number of nonzero matrix elements.

    int nnz = 3*n-2;

    std::vector<uint> __ia(n+1), __ja(nnz);
    _M_val.resize( nnz );

    __ia[0] = 0;
    int __j = 0;
    for (int __i = 0; __i < n; ++__i)
    {
        if (__i != 0)
        {
            _M_val[__j] =  sub;
            __ja[__j++] = __i-1;
        }
        _M_val[__j] =  diag;
        __ja[__j++] = __i;

        if ( __i != (n-1) )
        {
            _M_val[__j] =  sub;
            __ja[__j++] = __i+1;
        }
        __ia[__i+1] = __j;
    }

    _M_pattern = new CSRPatt( nnz, n, n, __ia, __ja );
    _M_mat = new CSRMatr<CSRPatt, double>( *_M_pattern, _M_val );

}


//
// Convection Diffusion
//

MatrixConvectionDiffusion::MatrixConvectionDiffusion( int nx, value_type __rho )
        :
        _M_rho( __rho ),
        _M_mat(0),
        _M_pattern(0),
        _M_val()
{

    int N = nx*nx;
    int nnz = 5*N-4*nx;

    std::vector<uint> __ia(N+1), __ja(nnz);
    _M_val.resize( nnz );

    __ia[0] = 0;
    int i = 0;

    value_type h  = 1.0/value_type(nx+1);
    value_type h2 = h*h;
    value_type dd = 4.0/h2;
    value_type df = -1.0/h2;
    value_type dl = df - 0.5*_M_rho/h;
    value_type du = df + 0.5*_M_rho/h;

    for (int j = 0; j < N; j++)
    {
        if (j >= nx)
        {
            _M_val[i] = df;
            __ja[i++]=j-nx;
        }
        if ((j%nx) != 0)
        {
            _M_val[i] = du;
            __ja[i++] = j-1;
        }

        _M_val[i] = dd;
        __ja[i++] = j;

        if (((j+1)%nx) != 0)
        {
            _M_val[i] = dl;
            __ja[i++] = j+1;
        }
        if (j < N-nx)
        {
            _M_val[i] = df;
            __ja[i++] = j+nx;
        }
        __ia[j+1]=i;
    }
    _M_pattern = new CSRPatt( nnz, N, N, __ia, __ja );
    _M_mat = new CSRMatr<CSRPatt, double>( *_M_pattern, _M_val );
    assert( _M_mat != 0 );

    std::cerr << __PRETTY_FUNCTION__ << " matrix constructed" << std::endl;
}
}
