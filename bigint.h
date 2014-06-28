//This file contain a class that can store and compute integers which are large.
//Copyright 2014 Lou Yu
#ifndef BIGINT_COMPILED
#define BIGINT_COMPILED

#define UINT_BIT ( CHAR_BIT * sizeof( unsigned ) )//Number of bits in unsigned

#include <malloc.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <cmath>

class bigint
{
	public:
		unsigned size;
		bool negative;
		unsigned* data;//Low before high in the post

		bigint ();
		bigint (const int &);
		bigint (const bigint &);
		bigint (const unsigned &);
		~bigint ();

		bigint & operator = (const bigint & rhs);

		bigint & operator += (const bigint & rhs);
		bigint & operator -= (const bigint & rhs);
		bigint & operator *= (const bigint & rhs);
		bigint & operator /= (const bigint & rhs);

		bigint operator >>= (const bigint & rhs);
		bigint operator <<= (const bigint & rhs);

		bigint operator % (const bigint & rhs);

		bigint operator > (const bigint & rhs);
		bigint operator < (const bigint & rhs);
		bigint operator >= (const bigint & rhs);
		bigint operator <= (const bigint & rhs);
		bigint operator == (const bigint & rhs);
		bigint operator != (const bigint & rhs);

		void addabs(const bigint & rhs);
		bool subabs(const bigint & rhs);//return whether this is greater than rhs(ture) or no(false)
		char subabs(const bigint & lhs , const bigint & rhs , unsigned stoppoint);
		//lhs most be greater than rhs! Return carryset at the stoppoint
};

bigint operator + (const bigint &,const bigint &);
bigint operator - (const bigint &,const bigint &);
bigint operator * (const bigint &,const bigint &);
bigint operator / (const bigint &,const bigint &);
bigint operator << (const bigint &,const bigint &);
bigint operator >> (const bigint &,const bigint &);

bigint::bigint()
{
	size = 1;
	data = new unsigned [ 1 ];
	data[ 0 ] = 0;
	negative = false;
}

bigint::bigint(const int & newdata)
{
	size = 1;
	data = new unsigned [ 1 ];
	data[ 0 ] = abs( newdata );
	negative = newdata > 0 ? false : true;
}

bigint::bigint(const bigint & newdata)
{
	data = NULL;
	*this = newdata;
}

bigint::bigint (const unsigned & newdata)
{
	size = 1;
	data = new unsigned [ 1 ];
	data[ 0 ] = newdata;
	negative = false;
}

bigint::~bigint()
{
	size = 0;
	delete [] data;
	data = 0;
}

bigint & bigint::operator = (const bigint & rhs)
{
	delete [] data;

	size = rhs.size;
	negative = rhs.negative;
	data = new unsigned [ size ];

	memcpy( data , rhs.data , sizeof( unsigned ) * size );

	return *this;
}

bigint & bigint::operator += (const bigint & rhs)
{
	if( negative != rhs.negative )//Opposite signs
	{
		if( !subabs( rhs ) )//Minus and decide if this needs to change sign
		{
			negative = !negative;
		}
	}
	else//Same signs
	{
		addabs( rhs );
	}

	return *this;
}

bigint & bigint::operator -= (const bigint & rhs)
{
	if( negative != rhs.negative )//Opposite signs
	{
		addabs( rhs );
	}
	else//Same signs
	{
		if( !subabs( rhs ) )//Minus and decide if this needs to change sign
		{
			negative = !negative;
		}
	}

	return *this;
}
void bigint::addabs( const bigint & rhs)
{
	unsigned i;
	char carryset = 0;
	unsigned stoppoint = std::min( size , rhs.size );

	for( i = 0 ; i < stoppoint ; i++ )
	{
		if( data[ i ] != UINT_MAX ? ( UINT_MAX - data[ i ] - carryset < rhs.data[ i ] )//That is,  "rhs.data[i]+data[i]+carryset>UINT_MAX" . In order to avoid overflows
			: ( rhs.data[ i ] > 0 || carryset == 1 ) )//Check if we need carry
		{
			data[ i ] = rhs.data[ i ] - ( UINT_MAX - data[ i ] ) + carryset-1;//That is,  "rhs.data[i]+data[i]+carryset-(UINT_MAX+1)" . In order to avoid overflows and underflows
			carryset = 1;
		}
		else
		{
			data[ i ] = data[ i ] + rhs.data[ i ] + carryset;
			carryset = 0;
		}
	}

	if( size == rhs.size )
	{
		if( carryset == 1 )//Check if we need to add a new digit
		{
			data = ( unsigned * ) realloc( data , sizeof( unsigned ) * ( size + 1 ) );
			size += 1;
			data[ size - 1 ] = 1;
		}

		return;
	}
	else if( size < rhs.size)//Copy remainder digits
	{
		data = ( unsigned * ) realloc( data , sizeof( unsigned ) * ( rhs.size ) );
		memcpy( data + size, rhs.data + size , sizeof( unsigned ) * ( rhs.size - size ) );
		size = rhs.size;
	}

	if( carryset == 1)//Need to carry
	{
		for( i = stoppoint ; i < size ; i++ )//Continue from the point we stopped
		{
			if( data[ i ] == UINT_MAX )//Still need to carry
			{
				data[ i ] = 0;
			}
			else//Work is done
			{
				data[ i ] += 1;

				return;
			}
		}
		//Still need carry on the highest digit
		data = ( unsigned * ) realloc( data , sizeof( unsigned ) * ( size + 1 ) );//Add a new digit
		size += 1;
		data[ size - 1 ] = 1;
	}
	return;//Done
}

bool bigint::subabs(const bigint & rhs)
{
	unsigned i;
	char carryset = 0;
	bool lgr;//whether lhs is greater than rhs

	if( size != rhs.size)
	{
		lgr = ( size > rhs.size );

		if( lgr )
		{
			carryset = subabs( *this , rhs , rhs.size );
			i=rhs.size;//Later continue from data[rhs.size]
		}
		else
		{
			carryset = subabs( rhs , *this , size );
			i=size;

			//Copy reminder digits
			data = ( unsigned * ) realloc( data , sizeof( unsigned ) * rhs.size);
			memcpy( data + size , rhs.data + size , sizeof(unsigned ) * ( rhs.size - size ) );
			size = rhs.size;
		}

		if( carryset == 1 )
		{
			for( ; i < size ; i++ )//Add carryset
			{
				if( data[ i ] == 0 )
				{
					data[ i ] = UINT_MAX;
				}
				else
				{
					data[ i ] -= 1;

					if( i == size - 1 && data[ i ] == 0 )//Delete leading zero
					{
						size--;
						data = ( unsigned * ) realloc( data , sizeof( unsigned ) * size);
					}

					return lgr;
				}
			}
		}
	}
	else//When size==rhs.size
	{
		//Delete same high digit
		i = size - 1;
		while( i > 0 && data[ i ] == rhs.data[ i ] )
		{
			i--;
		}
		size = i + 1;
		data = ( unsigned * ) realloc( data , sizeof( unsigned ) * size );

		lgr = data[ i ] > rhs.data[ i ];
		if( lgr )
			subabs( *this , rhs , size );
		else
			subabs( *this , rhs , size );
	}
	return lgr;
}

char bigint::subabs(const bigint & lhs , const bigint & rhs , unsigned stoppoint)
{
	char carryset = 0;
	unsigned i;
	
	for( i = 0 ; i < stoppoint ; i++ )
	{
		if( lhs.data[ i ] != 0 ?
				( lhs.data[ i ] - carryset < rhs.data[ i ] )//That is,"lhs.data[i]-rhs.data[i]-carryset<0"
				: ( carryset == 1 || rhs.data[ i ] > 0)
		)
		{
			data[ i ] = UINT_MAX - rhs.data[ i ] + lhs.data[ i ] - carryset+1;//That is, "(lhs.data-rhs.data)+UINT_MAX+1-carruset"
			carryset = 1;
		}
		else
		{
			data[ i ] = lhs.data[ i ] - rhs.data[ i ];
			carryset = 0;
		}
	}
	return carryset;
}

bigint operator + ( const bigint & lhs , const bigint & rhs )
{
	bigint result = lhs;
	result += rhs;
	return result;
}

bigint operator - ( const bigint & lhs , const bigint & rhs )
{
	bigint result = lhs;
	result -= rhs;
	return result;
}

bigint operator * ( const bigint & lhs , const bigint & rhs )
{
	unsigned i,j;
	unsigned long long mulres ,//Result of multiplication between one digit from lhs and one digit from rhs
		LD = UINT_MAX , HD = ~ LD ;//It is used to get low digits and high digits
	bigint result , carryset , dblres;//Result of multiplication between one digit from lhs and all digits from rhs

	dblres.size = lhs.size + rhs.size - 1;
	dblres.data = ( unsigned * ) realloc( dblres.data , sizeof( unsigned ) * dblres.size );
	carryset.size = lhs.size + rhs.size;
	carryset.data = ( unsigned * ) realloc( carryset.data , sizeof( unsigned ) * carryset.size );

	for( i = 0 ; i < lhs.size ; i++ )
	{
		memset( dblres.data + i , 0 , rhs.size );
		memset( carryset.data + i , 0 , rhs.size + 1);

		for( j = 0 ; j < rhs.size ; j++)
		{
			mulres = ( unsigned long long ) lhs.data[ i ] * ( unsigned long long )rhs.data[ j ];
			carryset.data[ i + j + 1 ] = ( ( HD & mulres ) >> UINT_BIT );//Get high digit
			dblres.data[ i + j ] = LD & mulres;
		}

		result += dblres;
		result += carryset;
	}

	if( result.data[ result.size - 1 ] == 0 )//Delete leading zero
	{
		result.size--;
		result.data = ( unsigned * ) realloc( result.data , sizeof( unsigned ) * result.size );
	}

	result.negative = ( lhs.negative != rhs.negative );//Two like signs make a positive sign, two unlike signs make a negative sign

	return result;
}

bigint & bigint::operator *= ( const bigint & rhs )
{
	*this = *this * rhs;
	return *this;
}
#endif
