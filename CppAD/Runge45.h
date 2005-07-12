# ifndef CppADRunge45Included
# define CppADRunge45Included

// BEGIN SHORT COPYRIGHT
/* -----------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-05 Bradley M. Bell

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
------------------------------------------------------------------------ */
// END SHORT COPYRIGHT

/*
$begin Runge45$$
$spell
	bool
	xf
	templated
	const
	Runge-Kutta
	CppAD
	xi
	ti
	tf
	Karp
$$

$index Runge45$$
$index ODE, Runge-Kutta$$
$index Runge, ODE$$
$index Kutta, ODE$$
$index solve, ODE$$
$index differential, equation$$
$index equation, differential$$
 
$section An Embedded 4th and 5th Order Runge-Kutta ODE Solver$$

$table
$bold Syntax$$ 
$cnext $code # include <CppAD/Runge45.h>$$
$rnext $cnext
$syntax%%xf% = Runge45(%F%, %M%, %ti%, %tf%, %xi%)%$$
$rnext $cnext
$syntax%%xf% = Runge45(%F%, %M%, %ti%, %tf%, %xi%, %e%)%$$
$tend

$fend 25$$

$head Description$$
This is the Cash-Karp embedded 4th and 5th order Runge-Kutta ODE solver 
(see Section 16.2 of $xref/Bib/Numerical Recipes/Numerical Recipes/$$).
We use $latex n$$ for the size of the vector $italic xi$$.
Let $latex \R$$ denote the real numbers
and let $latex F : \R \times \R^n \rightarrow \R^n$$ be a smooth function.
The return value $italic xf$$ contains a 5th order
approximation for the value $latex X(tf)$$ where 
$latex X : [ti , tf] \rightarrow \R^n$$ is defined by 
the following initial value problem:
$latex \[
\begin{array}{rcl}
	X(ti)  & = & xi    \\
	X'(t)  & = & F[t , X(t)] 
\end{array}
\] $$
If your set of ordinary differential equations
are stiff, an implicit method may be better
(perhaps $xref/Rosen34/$$.)

$head Include$$
The file $code CppAD/Runge45.h$$ is included by $code CppAD/CppAD.h$$
but it can also be included separately with out the rest of 
the $code CppAD$$ routines.

$head xf$$
The return value $italic xf$$ has the prototype
$syntax%
	%Vector% %xf%
%$$
an the size of $italic xf$$ is equal to $italic n$$ 
(see description of $xref/Runge45/Vector/Vector/$$ below).
$latex \[
	X(tf) = xf + O( h^6 )
\] $$
where $latex h = (tf - ti) / M$$ is the step size.

$head Fun$$
The class $italic Fun$$ 
and the object $italic F$$ satisfy the prototype
$syntax%
	%Fun% &%F%
%$$
The object $italic F$$ (and the class $italic Fun$$)
must have a member function named $code Ode$$ 
that supports the syntax
$syntax%
	%F%.Ode(%t%, %x%, %f%)
%$$

$subhead t$$
The argument $italic t$$ to $syntax%%F%.Ode%$$ has prototype
$syntax%
	const %Scalar% &%t%
%$$
(see description of $xref/Runge45/Scalar/Scalar/$$ below). 

$subhead x$$
The argument $italic x$$ to $syntax%%F%.Ode%$$ has prototype
$syntax%
	const %Vector% &%x%
%$$
and has size $italic n$$
(see description of $xref/Runge45/Vector/Vector/$$ below). 

$subhead f$$
The argument $italic f$$ to $syntax%%F%.Ode%$$ has prototype
$syntax%
	%Vector% &%f%
%$$
On input and output, $italic f$$ is a vector of size $italic n$$
and the input values of the elements of $italic f$$ do not matter.
On output,
$italic f$$ is set equal to $latex F(t, x)$$ in the differential equation.
$bold Warning$$: the argument $italic f$$ to $syntax%%F%.Ode%$$
must have a call by reference in its prototype; i.e.,
do not forget the $code &$$ in the prototype for $italic f$$.

$head M$$
The argument $italic M$$ has prototype
$syntax%
	size_t %M%
%$$
It specifies the number of steps
to use when solving the differential equation.
This must be greater than or equal one.
The step size is given by $latex h = (tf - ti) / M$$, thus
the larger $italic M$$, the more accurate the
return value $italic xf$$ is as an approximation
for $latex X(tf)$$.

$head ti$$
The argument $italic ti$$ has prototype
$syntax%
	const %Scalar% &%ti%
%$$
(see description of $xref/Runge45/Scalar/Scalar/$$ below). 
It specifies the initial time for $italic t$$ in the 
differential equation; i.e., 
the time corresponding to the value $italic xi$$.

$head tf$$
The argument $italic tf$$ has prototype
$syntax%
	const %Scalar% &%tf%
%$$
It specifies the final time for $italic t$$ in the 
differential equation; i.e., 
the time corresponding to the value $italic xf$$.

$head xi$$
The argument $italic xi$$ has the prototype
$syntax%
	const %Vector% &%xi%
%$$
and the size of $italic xi$$ is equal to $italic n$$.
It specifies the value of $latex X(ti)$$

$head e$$
The argument $italic e$$ is optional and has the prototype
$syntax%
	%Vector% &%e%
%$$
If $italic e$$ is present,
the size of $italic e$$ must be equal to $italic n$$.
The input value of the elements of $italic e$$ does not matter.
On output
it contains a bound on the absolute value
of the element by element difference between $italic xf$$ 
and a fourth order solution of the same differential equation.

$head Scalar$$
The following operations must be defined for $italic Scalar$$ objects
$italic a$$ and $italic b$$:

$table
$bold Operation$$ $cnext $bold Description$$  $rnext
$syntax%%Scalar% %a%(%i%)%$$ $cnext
	constructs a $italic Scalar$$ object from the $code int$$ $italic i$$
$rnext
$syntax%%Scalar% %a%(%b%)%$$ $cnext
	constructs $italic a$$ with the same value as $italic b$$.
$rnext
$syntax%%a% = %b%$$ $cnext
	assignment operator
$rnext
$syntax%%a% += %b%$$ $cnext
	computed addition operator 
$rnext
$syntax%%a% -= %b%$$ $cnext
	computed subtraction operator 
$rnext
$syntax%%a% + %b%$$ $cnext
	addition operator (returns a $italic Scalar$$ object)
$rnext
$syntax%%a% - %b%$$ $cnext
	subtraction operator (returns a $italic Scalar$$ object)
$rnext
$syntax%%a% * %b%$$ $cnext
	multiplication operator (returns a $italic Scalar$$ object)
$rnext
$syntax%%a% / %b%$$ $cnext
	division operator (returns a $italic Scalar$$ object)
$rnext
$syntax%%a% < %b%$$ $cnext
	less than operator (returns a $code bool$$ object)
$tend

$head Vector$$
The type $italic Vector$$ must be a $xref/SimpleVector/$$ class with
$xref/SimpleVector/Elements of Specified Type/elements of type Scalar/$$.
The routine $xref/CheckSimpleVector/$$ will generate an error message
if this is not the case.

$head Example$$
$children%
	Example/Runge45.cpp
%$$
The file
$xref/Runge45.cpp/$$
contains an example and test a test of using this routine.
It returns true if it succeeds and false otherwise.

$head Source Code$$
The source code for this routine is in the file
$code CppAD/Runge45.h$$.

$end
--------------------------------------------------------------------------
*/
# include <cstddef>
# include <CppAD/CppADError.h>
# include <CppAD/CheckSimpleVector.h>

namespace CppAD { // BEGIN CppAD namespace

template <typename Scalar, typename Vector, typename Fun>
Vector Runge45(
	Fun           &F , 
	size_t         M , 
	const Scalar &ti , 
	const Scalar &tf , 
	const Vector &xi )
{	Vector e( xi.size() );
	return Runge45(F, M, ti, tf, xi, e);
}

template <typename Scalar, typename Vector, typename Fun>
Vector Runge45(
	Fun           &F , 
	size_t         M , 
	const Scalar &ti , 
	const Scalar &tf , 
	const Vector &xi ,
	Vector       &e )
{
	// check simple vector class specifications
	CheckSimpleVector<Scalar, Vector>();

	// Cash-Karp Parameters for Embedded Runge-Kutta Method
	static Vector a(6), b(5 * 5), c4(6), c5(6);

	static bool initialized(false);
	if( ! initialized )
	{	initialized = true;

		a[0]  = Scalar(0);
		a[1]  = Scalar(1) / Scalar(5);
		a[2]  = Scalar(3) / Scalar(10);
		a[3]  = Scalar(3) / Scalar(5);
		a[4]  = Scalar(1);
		a[5]  = Scalar(7) / Scalar(8);
		
		b[0]  = Scalar(1) / Scalar(5);
		b[1]  = Scalar(0);
		b[2]  = Scalar(0);
		b[3]  = Scalar(0);
		b[4]  = Scalar(0);
		
		b[5]  = Scalar(3) / Scalar(40);
		b[6]  = Scalar(9) / Scalar(40);
		b[7]  = Scalar(0);
		b[8]  = Scalar(0);
		b[9]  = Scalar(0);
		
		b[10] = Scalar(3) / Scalar(10);
		b[11] = -Scalar(9) / Scalar(10);
		b[12] = Scalar(6) / Scalar(5);
		b[13] = Scalar(0);
		b[14] = Scalar(0);
		
		b[15] = -Scalar(11) / Scalar(54);
		b[16] = Scalar(5) / Scalar(2);
		b[17] = -Scalar(70) / Scalar(27);
		b[18] = Scalar(35) / Scalar(27);
		b[19] = Scalar(0);
		
		b[20] = Scalar(1631) / Scalar(55296);
		b[21] = Scalar(175) / Scalar(512);
		b[22] = Scalar(575) / Scalar(13824);
		b[23] = Scalar(44275) / Scalar(110592);
		b[24] = Scalar(253) / Scalar(4096);
		
		c4[0] = Scalar(2825) / Scalar(27648);
		c4[1] = Scalar(0);
		c4[2] = Scalar(18575) / Scalar(48384);
		c4[3] = Scalar(13525) / Scalar(55296);
		c4[4] = Scalar(277) / Scalar(14336);
		c4[5] = Scalar(1) / Scalar(4);
		
		c5[0] = Scalar(37) / Scalar(378);
		c5[1] = Scalar(0);
		c5[2] = Scalar(250) / Scalar(621);
		c5[3] = Scalar(125) / Scalar(594);
		c5[4] = Scalar(0);
		c5[5] = Scalar(512) / Scalar(1771);
	}
	CppADUsageError(
		M >= 1,
		"Error in Runge45: the number of steps is less than one"
	);
	CppADUsageError(
		e.size() == xi.size(),
		"Error in Runge45: size of e not equal to size of xi"
	);
	size_t i, j, k, m;              // indices

	size_t  n = xi.size();          // number of components in X(t)
	Scalar  ns = Scalar(double(M)); // number of steps as Scalar object
	Scalar  h = (tf - ti) / ns;     // step size 
	Scalar  z = Scalar(0);          // zero
	for(i = 0; i < n; i++)          // initialize e = 0
		e[i] = z;

	// vectors used to store values returned by F
	Vector fh(6 * n), xtmp(n), ftmp(n), x4(n), x5(n), xf(n);

	xf = xi;           // initialize solution
	for(m = 0; m < M; m++)
	{	// time at beginning of this interval
		Scalar t = ti * ((M - m) / ns) + tf * (m / ns);

		// loop over integration steps
		x4 = x5 = xf;   // start x4 and x5 at same point for each step
		for(j = 0; j < 6; j++)
		{	// loop over function evaluations for this step
			xtmp = xf;  // location for next function evaluation
			for(k = 0; k < j; k++)
			{	// loop over previous function evaluations
				Scalar bjk = b[ (j-1) * 5 + k ];
				for(i = 0; i < n; i++)
				{	// loop over elements of x
					xtmp[i] += bjk * fh[i * 6 + k];
				}
			}
			// ftmp = F(t + a[j] * h, xtmp)
			F.Ode(t + a[j] * h, xtmp, ftmp); 

			for(i = 0; i < n; i++)
			{	// loop over elements of x
				Scalar fhi    = ftmp[i] * h;
				fh[i * 6 + j] = fhi;
				x4[i]        += c4[j] * fhi;
				x5[i]        += c5[j] * fhi;
			}
		}
		// accumulate error bound
		for(i = 0; i < n; i++)
		{	// cant use abs because CppAD.h may not be included
			Scalar diff = x5[i] - x4[i];
			if( diff < z )
				e[i] -= diff;
			else	e[i] += diff;
		}

		// advance xf for this step using x5
		xf = x5;
	}
	return xf;
}

} // End CppAD namespace 

# endif
