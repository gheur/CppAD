# ifndef CppADADReverseIncluded
# define CppADADReverseIncluded

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
$begin ADReverse$$
$spell
	Var
	numvar
	Num
	Len
	Ind
	const
	Taylor
	CppAD
	zs
	op
	Ind
$$

$section Reverse Mode Computation of Derivatives of Taylor Coefficients$$
$mindex ADReverse derivative Taylor coefficient$$

$table
$bold Syntax$$ $cnext
$syntax%void ADReverse(
	size_t                 %d%,
	size_t                 %numvar%,
	const TapeRec<%Base%> *%Rec%,
	size_t                 %J%,
	const Base            *%Taylor%,
	size_t                 %K%,
	Base                  *%Partial%
)%$$
$tend

$fend 20$$


$head Rec$$
The information stored in $italic Rec$$
is a recording of the operations corresponding to a function
$latex \[
	F : B^m \rightarrow B^n
\] $$

$head G$$
Given the vector $latex v \in B^n$$ we define the function 
$latex G : B^{m \times d} \rightarrow B$$ by
$latex \[
	G(u) = \frac{1}{d !} 
		\sum_{i=1}^n v_i \left[
		\frac{ \partial^p}{ \partial t^d}
		F_i [ u^{(0)} + u^{(1)} t + \cdots + u^{(d)} t^d ]
	\right]_{t=0}
\] $$
Note that the scale factor of $latex 1 / d !$$ converts
$th d$$ order derivatives to $th d$$ order Taylor coefficients.
The routine $code ADReverse$$ computes all the first order partial
derivatives of $latex G$$ with respect to each of the Taylor coefficients
for the independent variables 
$latex u^{(j)}$$ for $latex j = 0 , \ldots , d$$.


$head numvar$$
is the number of rows in the matrices $italic Taylor$$ and $italic Partial$$.
It must also be equal to $syntax%%Rec%->TotNumVar()%$$.

$head J$$
Is the number of columns in the coefficient matrix $italic Taylor$$.
This must be greater than or equal $latex d+1$$.

$head K$$
Is the number of columns in the coefficient matrix $italic Partial$$.
This must be greater than or equal $latex d+1$$.

$head Taylor$$
For $latex i = 1 , \ldots , numvar - 1$$ and $latex j <= d$$
$syntax%%Taylor%[%i% * J + j]%$$ contains the 
$th j$$ order Taylor coefficients
for the variable corresponding to index $italic i$$.

$head On Input$$

$subhead Dependent Variables$$
The vector $italic v$$ is stored in the 
$th d$$ order components of the last $italic n$$ 
variables in $italic Partial$$; i.e.,
for $latex i = numvar-n , \ldots , numvar-1$$
and for $latex j < d$$,
$syntax%
	%Partial%[%i% * %K% + %j%] == 0
	%Partial%[%i% * %K% + %d%] == %v%[%numvar% - %i%]
%$$

$subhead Other Variables$$
The other variable records come before and have initial partials of zero; 
i.e., for $latex i = 1, \ldots , numvar-n-1$$ and 
$latex j \leq d$$,
$syntax%
	%Partial%[%i% * %K% + %j%] ==  0
%$$

$head On Output$$

$subhead Independent Variables$$
For $latex i = 1, \ldots , m$$, and for $latex j \leq d$$,
$syntax%%Partial%[%i% * %K% + %j%]%$$ 
is the partial of $latex G$$ with respect to $latex u_i^{(j)}$$.

$subhead Other Variables$$
For $latex i = m+1, \ldots , numvar-1$$ and $latex j \leq d$$,
$syntax%%Partial%[%i% * %K% + %j%]%$$ 
is temporary work space and its
output value is not specified.


$end
------------------------------------------------------------------------------
*/

# define CppADReverseTrace 0

// BEGIN CppAD namespace
namespace CppAD {

template <class Base>
void ADReverse(
	size_t                d,
	size_t                numvar,
	const TapeRec<Base>  *Rec,
	size_t                J,
	const Base           *Taylor,
	size_t                K,
	Base                 *Partial
)
{
	OpCode           op;
	size_t         i_op;
	size_t        i_var;
	size_t        i_ind;
	size_t        n_var;
	size_t        n_ind;

	const size_t   *ind;
	const Base       *P;
	const Base       *Z;
	const Base       *Y;
	const Base       *X;
	const Base     *Tmp;

	Base            *pZ;
	Base            *pY;
	Base            *pX;
	Base          *pTmp;

	// check numvar argument
	CppADUnknownError( Rec->TotNumVar() == numvar );
	CppADUnknownError( numvar > 0 );

	// Initialize
	i_op   = Rec->NumOp();
	i_var  = Rec->TotNumVar();
	i_ind  = Rec->NumInd();
	op     = NonOp;         // phony operator that is not there

	while(i_op > 1)
	{	--i_op;

		// next op
		op     = Rec->GetOp(i_op);

		// corresponding varable
		n_var  = NumVar(op);
		i_var -= n_var;

		// corresponding index values
		n_ind  = NumInd(op);
		CppADUnknownError( i_ind >= n_ind );
		i_ind -= n_ind;
		ind    = Rec->GetInd(n_ind, i_ind);

		// value of Z and its partials for this op
		Z   = Taylor + i_var * J;
		pZ  = Partial + i_var * K;

		// rest of informaiton depends on the case
# if CppADReverseTrace
		n_ind = NumInd(op);
		printOp(
			std::cout, 
			Rec,
			i_var,
			op, 
			ind,
			d + 1, 
			Z, 
			d + 1, 
			pZ 
		);
# endif

		switch( op )
		{

			case AbsOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 1 );
			CppADUnknownError( ind[0] < i_var );

			X  = Taylor  + ind[0] * J;
			pX = Partial + ind[0] * K;
			RevAbsOp(d, Z, X, pZ, pX);
			break;
			// --------------------------------------------------

			case AddvvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( ind[0] < i_var );
			CppADUnknownError( ind[1] < i_var );

			pX = Partial + ind[0] * K;
			pY = Partial + ind[1] * K;
			RevAddvvOp(d, pZ, pX, pY);
			break;
			// --------------------------------------------------

			case AddpvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( ind[1] < i_var );

			pY = Partial + ind[1] * K;
			RevAddpvOp(d, pZ, pY);
			break;
			// --------------------------------------------------

			case AddvpOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( ind[0] < i_var );

			pX = Partial + ind[0] * K;
			RevAddvpOp(d, pZ, pX);
			break;
			// --------------------------------------------------

			case AcosOp:
			CppADUnknownError( n_ind == 1 );
			CppADUnknownError( ind[0] < i_var );

			// acos(x) and sqrt(1 - x * x) are computed in pairs
			CppADUnknownError( n_var == 2);
			CppADUnknownError( i_var < numvar - 1 );

			// use Tmp for data stored in second record
			Tmp  = Taylor  + (i_var+1) * J;
			pTmp = Partial + (i_var+1) * K;
			X    = Taylor  + ind[0] * J;
			pX   = Partial + ind[0] * K;
			RevAcosOp(d, Z, Tmp, X, pZ, pTmp, pX);
			break;
			// --------------------------------------------------

			case AsinOp:
			CppADUnknownError( n_ind == 1 );
			CppADUnknownError( ind[0] < i_var );

			// atan(x) and 1 + x * x must be computed in pairs
			CppADUnknownError( n_var == 2);
			CppADUnknownError( i_var < numvar - 1 );

			// use Tmp for data stored in second record
			Tmp  = Taylor  + (i_var+1) * J;
			pTmp = Partial + (i_var+1) * K;
			X    = Taylor  + ind[0] * J;
			pX   = Partial + ind[0] * K;
			RevAsinOp(d, Z, Tmp, X, pZ, pTmp, pX);
			break;
			// --------------------------------------------------

			case AtanOp:
			CppADUnknownError( n_ind == 1 );
			CppADUnknownError( ind[0] < i_var );

			// cosine and sine must come in pairs
			CppADUnknownError( n_var == 2);
			CppADUnknownError( i_var < numvar - 1 );

			// use Tmp for data stored in second record
			Tmp  = Taylor  + (i_var+1) * J;
			pTmp = Partial + (i_var+1) * K;
			X    = Taylor  + ind[0] * J;
			pX   = Partial + ind[0] * K;
			RevAtanOp(d, Z, Tmp, X, pZ, pTmp, pX);
			break;
			// -------------------------------------------------

			case CEpppOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError(0); // this instruction is not used
			break;
			// ---------------------------------------------------

			case CEpvpOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );
			CppADUnknownError( ind[1] < i_var );
			pX     = Partial + ind[1] * K;
			pX[d] += CondExp(
				*(Rec->GetPar( ind[0] )),
				pZ[d],
				Base(0)
			);
			break;

			// ---------------------------------------------------

			case CEppvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );
			CppADUnknownError( ind[2] < i_var );
			pY     = Partial + ind[2] * K;
			pY[d] += CondExp(
				*(Rec->GetPar( ind[0] )),
				Base(0),
				pZ[d]
			);
			break;

			// ---------------------------------------------------

			case CEpvvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );
			CppADUnknownError( ind[1] < i_var );
			CppADUnknownError( ind[2] < i_var );
			pX     = Partial + ind[1] * K;
			pY     = Partial + ind[2] * K;
			pX[d] += CondExp(
				*(Rec->GetPar( ind[0] )),
				pZ[d],
				Base(0)
			);
			pY[d] += CondExp(
				*(Rec->GetPar( ind[0] )),
				Base(0),
				pZ[d]
			);
			break;
			// --------------------------------------------------

			case CEvppOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );

			break;
			// ---------------------------------------------------

			case CEvvpOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );
			CppADUnknownError( ind[0] < i_var );
			CppADUnknownError( ind[1] < i_var );
			pX     = Partial + ind[1] * K;
			pX[d] += CondExp(
				*(Taylor + ind[0] * J),
				pZ[d],
				Base(0)
			);
			break;

			// ---------------------------------------------------

			case CEvpvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );
			CppADUnknownError( ind[0] < i_var );
			CppADUnknownError( ind[2] < i_var );
			pY     = Partial + ind[2] * K;
			pY[d] += CondExp(
				*(Taylor + ind[0] * J),
				Base(0),
				pZ[d]
			);
			break;

			// ---------------------------------------------------

			case CEvvvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );
			CppADUnknownError( ind[0] < i_var );
			CppADUnknownError( ind[1] < i_var );
			CppADUnknownError( ind[2] < i_var );
			pX     = Partial + ind[1] * K;
			pY     = Partial + ind[2] * K;
			pX[d] += CondExp(
				*(Taylor + ind[0] * J),
				pZ[d],
				Base(0)
			);
			pY[d] += CondExp(
				*(Taylor + ind[0] * J),
				Base(0),
				pZ[d]
			);
			break;
			// --------------------------------------------------

			case CosOp:
			CppADUnknownError( n_ind == 1 );
			CppADUnknownError( ind[0] < i_var );

			// cosine and sine must come in pairs
			CppADUnknownError( n_var == 2);
			CppADUnknownError( i_var < numvar - 1 );

			// use Tmp for data stored in second record
			Tmp  = Taylor  + (i_var+1) * J;
			pTmp = Partial + (i_var+1) * K;
			X    = Taylor  + ind[0] * J;
			pX   = Partial + ind[0] * K;
			RevSinCos(d, Tmp, Z, X, pTmp, pZ, pX);
			break;
			// --------------------------------------------------

			case DisOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			break;
			// --------------------------------------------------

			case DivvvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( ind[0] < i_var );
			CppADUnknownError( ind[1] < i_var );

			X  = Taylor  + ind[0] * J;
			pX = Partial + ind[0] * K;
			Y  = Taylor  + ind[1] * J;
			pY = Partial + ind[1] * K;
			RevDivvvOp(d, Z, X, Y, pZ, pX, pY);
			break;
			// --------------------------------------------------

			case DivpvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( ind[1] < i_var );

			Y  = Taylor  + ind[1] * J;
			pY = Partial + ind[1] * K;
			P  = Rec->GetPar( ind[0] );
			RevDivpvOp(d, Z, P, Y, pZ, pY);
			break;
			// --------------------------------------------------

			case DivvpOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( ind[0] < i_var );

			X  = Taylor  + ind[0] * J;
			pX = Partial + ind[0] * K;
			P  = Rec->GetPar( ind[1] );
			RevDivvpOp(d, Z, X, P, pZ, pX);
			break;
			// --------------------------------------------------

			case ExpOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 1 );
			CppADUnknownError( ind[0] < i_var );

			X  = Taylor  + ind[0] * J;
			pX = Partial + ind[0] * K;
			RevExpOp(d, Z, X, pZ, pX);
			break;
			// --------------------------------------------------
			case LdpOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );
			CppADUnknownError( ind[2] < i_var );

			pY     = Partial + ind[2] * K;
			pY[d] += pZ[d];
			break;
			// -------------------------------------------------

			case LdvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );
			CppADUnknownError( ind[2] < i_var );

			pY     = Partial + ind[2] * K;
			pY[d] += pZ[d];
			break;
			// -------------------------------------------------

			case InvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 0 );
			break;
			// --------------------------------------------------

			case LogOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 1 );
			CppADUnknownError( ind[0] < i_var );

			X  = Taylor  + ind[0] * J;
			pX = Partial + ind[0] * K;
			RevLogOp(d, Z, X, pZ, pX);
			break;
			// --------------------------------------------------

			case EqfppOp:
			case EqtppOp:
			case EqfpvOp:
			case EqtpvOp:
			case EqfvpOp:
			case EqtvpOp:
			case EqfvvOp:
			case EqtvvOp:

			case LefppOp:
			case LetppOp:
			case LefpvOp:
			case LetpvOp:
			case LefvpOp:
			case LetvpOp:
			case LefvvOp:
			case LetvvOp:

			case LtfppOp:
			case LttppOp:
			case LtfpvOp:
			case LttpvOp:
			case LtfvpOp:
			case LttvpOp:
			case LtfvvOp:
			case LttvvOp:

			CppADUnknownError( n_var == 0 );
			CppADUnknownError( n_ind == 2 );
			break;
			// --------------------------------------------------

			case MulvvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( ind[0] < i_var );
			CppADUnknownError( ind[1] < i_var );

			X  = Taylor  + ind[0] * J;
			pX = Partial + ind[0] * K;
			Y  = Taylor  + ind[1] * J;
			pY = Partial + ind[1] * K;
			RevMulvvOp(d, Z, X, Y, pZ, pX, pY);
			break;
			// --------------------------------------------------

			case MulpvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( ind[1] < i_var );

			Y  = Taylor  + ind[1] * J;
			pY = Partial + ind[1] * K;
			P  = Rec->GetPar( ind[0] );
			RevMulpvOp(d, Z, P, Y, pZ, pY);
			break;
			// --------------------------------------------------

			case MulvpOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( ind[0] < i_var );

			X  = Taylor  + ind[0] * J;
			pX = Partial + ind[0] * K;
			P  = Rec->GetPar( ind[1] );
			RevMulvpOp(d, Z, X, P, pZ, pX);
			break;
			// --------------------------------------------------

			case NonOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 0 );
			break;
			// --------------------------------------------------

			case ParOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 1 );
			break;

			// --------------------------------------------------
			case PripOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( i_ind > n_ind );
			break;
			// --------------------------------------------------

			case PrivOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( i_ind > n_ind );
			break;

			// -------------------------------------------------

			case SinOp:
			CppADUnknownError( n_ind == 1 );
			CppADUnknownError( ind[0] < i_var );

			// sine and cosine come in pairs
			CppADUnknownError( n_var == 2);
			CppADUnknownError( i_var < numvar - 1 );

			// use Tmp for data stored in cosine slot record
			Tmp  = Taylor  + (i_var+1) * J;
			pTmp = Partial + (i_var+1) * K;
			X    = Taylor  + ind[0] * J;
			pX   = Partial + ind[0] * K;
			RevSinCos(d, Z, Tmp, X, pZ, pTmp, pX);
			break;
			// --------------------------------------------------

			case SqrtOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 1 );
			CppADUnknownError( ind[0] < i_var );

			X  = Taylor  + ind[0] * J;
			pX = Partial + ind[0] * K;
			RevSqrtOp(d, Z, X, pZ, pX);
			break;
			// --------------------------------------------------

			case StppOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );
			break;
			// --------------------------------------------------

			case StpvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );
			CppADUnknownError( ind[2] < i_var );

			pY     = Partial + ind[2] * K;
			pY[d] += pZ[d];
			break;
			// -------------------------------------------------

			case StvpOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );
			break;
			// -------------------------------------------------

			case StvvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 3 );
			CppADUnknownError( ind[2] < i_var );
			
			pY     = Partial + ind[2] * K;
			pY[d] += pZ[d];
			break;
			// --------------------------------------------------

			case SubvvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( ind[0] < i_var );
			CppADUnknownError( ind[1] < i_var );

			pX = Partial + ind[0] * K;
			pY = Partial + ind[1] * K;
			RevSubvvOp(d, pZ, pX, pY);
			break;
			// --------------------------------------------------

			case SubpvOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( ind[1] < i_var );

			pY = Partial + ind[1] * K;
			RevSubpvOp(d, pZ, pY);
			break;
			// --------------------------------------------------

			case SubvpOp:
			CppADUnknownError( n_var == 1);
			CppADUnknownError( n_ind == 2 );
			CppADUnknownError( ind[0] < i_var );

			pX = Partial + ind[0] * K;
			RevSubvpOp(d, pZ, pX);
			break;
			// --------------------------------------------------

			default:
			CppADUnknownError(0);
		}
	}
# if CppADReverseTrace
	std::cout << std::endl;
# endif
	CppADUnknownError( i_op == 1 );
	CppADUnknownError( Rec->GetOp(i_op-1) == NonOp );
	CppADUnknownError( i_var == NumVar(NonOp)  );
}

} // END CppAD namespace

# undef CppADReverseTrace

# endif
