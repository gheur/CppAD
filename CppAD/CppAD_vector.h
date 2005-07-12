# ifndef CppADCppAD_vectorIncluded
# define CppADCppAD_vectorIncluded

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
$begin CppAD_vector$$
$spell
	cout
	endl
	std
	Cpp
	const
	vec
	ostream
	elem
$$

$index vector, template class$$
$index class, template vector$$
$index template, vector class$$

$section The CppAD::vector Template Class$$

$table 
$bold Syntax$$ $cnext
$code # include <CppAD/CppAD_vector.h>$$ 
$tend

$fend 25$$


$head Description$$
The include file $code CppAD/CppAD_vector.h$$ defines the
vector template class $code CppAD::vector$$.
This is a $xref/SimpleVector/$$ template class and in addition
it has the features listed below:

$head Include$$
The file $code CppAD/CppAD_vector.h$$ is included by $code CppAD/CppAD.h$$
but it can also be included separately with out the rest of the 
CppAD include files.

$head Assignment$$
$index assignment, vector$$
If $italic x$$ and $italic y$$ are 
$syntax%CppAD::vector<%Type%>%$$ objects,
$syntax%
	%y% = %x%
%$$
has all the properties listed for a
$xref/SimpleVector/Assignment/simple vector assignment/$$
plus the following:
$pre

$$
The $code CppAD::vector$$ template class will check that
the size of $italic x$$ is equal to the size of $italic y$$
before doing the assignment.
If the sizes are not equal, $code CppAD::vector$$ will use
$xref/CppADError/Description/CppADUsageError/CppADUsageError/1/$$
to generate an appropriate error report.
$pre

$$
A reference to the vector $italic y$$ is returned.
An example use of this reference is in multiple assignments of the form
$syntax%
	%z% = %y% = %x%
%$$

$head Element Access$$
$index [], vector$$
If $italic x$$ is a $syntax%CppAD::vector<%Type%>%$$ object
and $code i$$ has type $code size_t$$,
$syntax%
	%x%[%i%]
%$$
has all the properties listed for a
$xref/SimpleVector/Element Access/simple vector element access/$$
plus the following:
$pre

$$
If $italic i$$ is not less than the size of the $italic x$$,
$code CppAD::vector$$ will use
$xref/CppADError/Description/CppADUsageError/CppADUsageError/1/$$
to generate an appropriate error report.

$head Output$$
If $italic x$$ is a $syntax%CppAD::vector<%Type%>%$$ object
and $italic os$$ is an $code std::ostream$$,
and the operation
$syntax%
	%os% << x
%$$
will output the vector $italic x$$ to the standard
output stream $italic os$$.
The elements of $italic x$$ are enclosed at the beginning by a
$code {$$ character,
they are separated by $code ,$$ characters,
and they are enclosed at the end by $code }$$ character.
It is assumed by this operation that if $italic e$$
is an object with type $italic Type$$,
$syntax%
	%os% << %e%
%$$
will output the value $italic e$$ to the standard
output stream $italic os$$.

$head Example$$
$children%
	Example/CppAD_vector.cpp
%$$
The file
$xref/CppAD_vector.cpp/$$
contains an example and test of this template class.
It returns true if it succeeds and false otherwise.

$head Exercise$$
$index exercise, CppAD::vector$$
Create and run a program that contains the following code:
$codep
	CppAD::vector<double> x(3);
	size_t i;
	for(i = 0; i < 3; i++)
		x[i] = 4. - i;
	std::cout << "x = " << x << std::endl;
$$

$end


$end

------------------------------------------------------------------------ 
$begin CppAD_vector.h$$
$spell
	resize
	iostream
	const
	std
	ostream
	vec
	namespace
	Cpp
	cstddef
	ifndef
	endif
	namespace
	inline
	typedef
$$

$section CppAD::vector Source Code$$

$index source, CppAD::vector$$
$index CppAD::vector, source$$
$index vector, CppAD source$$

$codep */

# include <cstddef>
# include <iostream>
# include <CppAD/CppADError.h>

# ifndef CppADNull
# define CppADNull 0
# endif

namespace CppAD { //  BEGIN CppAD namespace

template <class Type>
class vector {
private:
	size_t length;
	Type   * data;
public:
	// type of the elements in the vector
	typedef Type value_type;

	// default constructor
	inline vector(void) : length(0) , data(CppADNull)
	{ }
	// constructor with a specified size
	inline vector(size_t n) : length(n)
	{	if( length == 0 )
			data = CppADNull;
		else	data = new Type[length]; 
	}
	// copy constructor
	inline vector(const vector &x) : length(x.length)
	{	size_t i;
		if( length == 0 )
			data = CppADNull;
		else	data = new Type[length]; 

		for(i = 0; i < length; i++)
			data[i] = x.data[i];
	}
	// destructor
	~vector(void)
	{	delete [] data; }

	// size function
	inline size_t size(void) const
	{	return length; }

	// resize function
	inline void resize(size_t n)
	{	if( length > 0 )
			delete [] data;
		length = n;
		if( length > 0 )
			data = new Type[length];
		else	data = CppADNull;
	}
	// assignment operator
	inline vector & operator=(const vector &x)
	{	size_t i;
		CppADUsageError(
			length == x.length ,
			"size miss match in assignment operation"
		);
		for(i = 0; i < length; i++)
			data[i] = x.data[i];
		return *this;
	}
	// non-constant element access
	Type & operator[](size_t i)
	{	CppADUsageError(
			i < length,
			"vector index greater than or equal vector size"
		);
		return data[i]; 
	}
	// constant element access
	const Type & operator[](size_t i) const
	{	CppADUsageError(
			i < length,
			"vector index greater than or equal vector size"
		);
		return data[i]; 
	}
};
// output operator
template <class Type>
inline std::ostream& operator << (
	std::ostream              &os  , 
	const CppAD::vector<Type> &vec )
{	size_t i = 0;
	size_t n = vec.size();

	os << "{ ";
	while(i < n)
	{	os << vec[i++]; 
		if( i < n )
			os << ", ";
	}
	os << " }";
	return os;
}
} // END CppAD namespace

/* $$
$end
------------------------------------------------------------------------------
*/

# endif
