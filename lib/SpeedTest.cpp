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
$begin SpeedTest$$
$spell
	Microsoft
	namespace
	std
	const
	cout
	ctime
	ifdef
	const
	endif
	cpp
$$

$index SpeedTest$$
$index test, speed$$

$section Run One Speed Test$$

$table
$bold Syntax$$ 
$cnext
$code # include <CppAD/SpeedTest.h>$$
$rnext $cnext
$syntax%SpeedTest(%Test%, %first%, %inc%, %last%)%$$
$tend

$fend 20$$

$head Description$$
The $code SpeedTest$$ function executes a speed test
for various sized problems
and reports the results on standard output; i.e. $code std::cout$$.
The size of each test problem is included in its report
(unless $italic first$$ is equal to $italic last$$).

$head Repeat Factor$$
It is important to separate small calculation units
and test them individually.
This way individual changes can be tested in the context of the
routine that they are in.
On many machines, accurate timing of a very short execution 
sequences is not possible.
In addition,
there may be set up time for a test that
we do not really want included in the timing.
For this reason $code SpeedTest$$
automatically determines how many times to 
repeat the section of the test that we wish to time.


$head Include$$
The file $code SpeedTest.h$$ contains the prototype for 
the $code SpeedTest$$ function.
This file is included by $code CppAD/CppAD.h$$
but it can also be included separately with out the rest of 
the $code CppAD$$ routines.


$head Linkage$$
The CppAD Unix installation
compiles this function and places the object
in the $code CppAD.a$$ library; 
i.e., you will need the command
line argument $code -lCppAD$$ when you link to this routine under Unix.
If you are using Microsoft visual studio,
you will have to include the file
$code lib/SpeedTest.cpp$$ in your set of project source files.

$head Test$$
The argument $italic Test$$ is a function such that
$syntax%
	%Test%(%size%, %repeat%)
%$$
repeats a test for a specified size and number of times
and then returns a name for the test as a string.
The arguments $italic size$$ and $italic repeat$$ 
(to $italic Test$$) have type $code size_t$$.
Only one name is reported per call to $code SpeedTest$$
and it is the return value from the a call to $italic Test$$ 
where $italic size$$ is equal to $italic first$$ and
the value of $italic repeat$$ is not specified.
If the name returned by $italic Test$$
is the empty string, 
the name of the test is not reported by $code SpeedTest$$.


$head first$$
The argument $italic first$$ has type $code size_t$$ and
specifies the size of the first test problem reported by this call to 
$code SpeedTest$$.
	
$head last$$
The argument $italic last$$ has type $code size_t$$ and
specifies the size of the last test problem reported by this call to 
$code SpeedTest$$.

$head inc$$
The argument $italic inc$$ has type $code int$$ and
specifies the increment between problem sizes; i.e.,
all values of $italic size$$ in calls to $italic Test$$ are given by
$syntax%
	%size% = %first% + %j% * %inc%
%$$ 
where $italic j$$ is a positive integer.
The increment can be positive or negative but it cannot be zero.
The values $italic first$$, $italic last$$ and $italic inc$$ must 
satisfy the relation
$latex \[
	inc * ( last - first ) \geq 0
\] $$

$head rate$$
The value displayed in the $code rate$$ column on $code std::cout$$
is defined as the value of $italic repeat$$ divided by the 
corresponding elapsed execution time in seconds.
The elapsed execution time is measured by the difference in
$codep
	(double) clock() / (double) CLOCKS_PER_SEC
$$
in the context of the standard $code <ctime>$$ definitions.


$head Errors$$
If one of the restrictions above is violated,
the error reporting macro 
$xref/CppADError/Description/CppADUsageError/CppADUsageError/1/$$
is used to report the error.
You can redefine this action using the instructions in
$xref/CppADError/$$

$head Example$$
$children%
	lib/SpeedTestExample.cpp
%$$
The section $xref/SpeedTestExample/$$ contains an example usage
of $code SpeedTest$$.

$end
-----------------------------------------------------------------------
*/
// BEGIN PROGRAM


# include <cstddef>
# include <ctime>
# include <cmath>
# include <string>
# include <iostream>
# include <iomanip>

// For an unknown reason, cannot move other includes (using Sun's CC compiler)
# include <CppAD/CppADError.h>
# include <CppAD/SpeedTest.h>

namespace {
	double seconds()
	{	return (double) clock() / (double) CLOCKS_PER_SEC;
	}

	void Ndigit(size_t value, size_t &ndigit, size_t &pow10)
	{	pow10 = 10;
		ndigit       = 1;
		while( pow10 <= value )
		{	pow10  *= 10;
			ndigit += 1;
		}
	}
}

namespace CppAD { // BEGIN CppAD namespace

void SpeedTest( 
	std::string Test(size_t size, size_t repeat),
	size_t first,
	int    inc,
	size_t last
)
{	using std::cout;
	using std::endl;

	size_t    size;
	size_t    repeat;
	size_t    rate;
	size_t    digit;
	size_t    ndigit;
	size_t    pow10;
	size_t    maxSize;
	size_t    maxSizeDigit;

	double    s0;
	double    s1;

	std::string name;

	CppADUsageError( 
		inc != 0 && first != 0 && last != 0,
		"inc, first, or last is zero in call to SpeedTest"
	);
	CppADUsageError( 
		(inc > 0 && first <= last) || (inc < 0 && first >= last),
		"SpeedTest: increment is positive and first > last or "
		"increment is negative and first < last"
	);

	// compute maxSize
	maxSize = size = first;
	while(  (inc > 0 && size <= last) || (inc < 0 && size >= last) )
	{
		if( size > maxSize )
			maxSize = size;

		// next size
		if( ((int) size) + inc > 0 )
			size += inc;
		else	size  = 0;
	}
	Ndigit(maxSize, maxSizeDigit, pow10);

	size = first;
	while(  (inc > 0 && size <= last) || (inc < 0 && size >= last) )
	{
		repeat = 1;
		s0     = seconds();
		s1     = seconds();
		while( s1 - s0 < 1. )
		{	repeat = 2 * repeat;
			s0     = seconds();
			name   = Test(size, repeat);
			s1     = seconds();
		}
		rate = (size_t)(.5 + repeat / (s1 - s0));
		

		if( size == first && name != "" )
			cout << name << endl;

		if( first != last )
		{	std::cout << "size = "  << size;
			Ndigit(size, ndigit, pow10);
			while( ndigit < maxSizeDigit )
			{	cout << " ";
				ndigit++;
			}
			cout << " ";
		}

		cout << "rate = ";
		Ndigit(rate, ndigit, pow10);
		while( ndigit > 0 )
		{
			pow10 /= 10;
			digit  = rate / pow10;
 			cout << digit;

			rate    = rate % pow10;
			ndigit -= 1;

			if( (ndigit > 0) && (ndigit % 3 == 0) )
				cout << ",";
		}
		cout << endl;

		// next size
		if( ((int) size) + inc > 0 )
			size += inc;
		else	size  = 0;
	}
	return;
}

} // END CppAD namespace

// END PROGRAM
