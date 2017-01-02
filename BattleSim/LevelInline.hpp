#pragma once

// Uniform Grid scanning

/* Must make sure field scan does not skip grid blocks */
#ifdef _DEBUG
#define SCAN_MOVE_VALIDITY_CHECK( a, b ) \
    if ( field_scan_trace ) \
        { \
        printf( "x: %i y: %i, (from %i,%i) x1: %f, y1: %f\n", a, b, last_scan_coord[ 0 ], last_scan_coord[ 1 ], x1, y1 ); \
        } \
    assert( abs( last_scan_coord[ 0 ] - a ) <= 1 && \
            abs( last_scan_coord[ 1 ] - b ) <= 1 ); \
    last_scan_coord[ 0 ] = a; \
    last_scan_coord[ 1 ] = b;
#else /* _DEBUG */
#define SCAN_MOVE_VALIDITY_CHECK( a, b )    
#endif /* _DEBUG */

#define SCAN_ACTION( a, b ) \
    SCAN_MOVE_VALIDITY_CHECK( a, b ) \
    if ( trace_callback( a, b, data ) ) \
	{ \
       return true; \
    }

/*
* Calculate where (a-b) cuts grid
*/
#define CALCULATE_CUT( a, b )  step*( ( float )( a ) - ( float )( b ) )  / ( float )grid

/*
* Run tracing from grid pos a to grid pos b
* when X change is greater than Y change
*/
#define SCAN_FUNC( xadjust, xmode, base, xcomp, calc_cut ) \
                    if ( y2 > y1 ) \
                        { \
                        limit = ( fint16 )( ( y+1 )*grid ); \
                        while ( x != xt ) \
                            { \
                            y1 += step; \
                            base; \
                            if ( y1 >= limit ) \
                                { \
                                if ( x1 xcomp x2 ) \
                                    { \
                                    if ( y2 < limit ) \
                                        { \
                                        xmode; \
                                        SCAN_ACTION( x, y ); \
                                        if ( ( POS( y2 ) != y ) ) \
                                            { \
                                            SCAN_ACTION( x, y+1 ); \
                                            } \
                                        return false; \
                                        } \
                                    cut = calc_cut; \
                                    y1 = y2; \
                                    } \
                                if ( y1-step+cut >= limit ) \
                                    { \
                                    y++; \
                                    SCAN_ACTION( x, y ); \
                                    xmode; \
                                    SCAN_ACTION( x, y-1 );  /* Safety for diagonal moves */ \
                                    } \
                                else \
                                    { \
                                    xmode;     \
                                    SCAN_ACTION( x, y ); \
                                    y++; \
                                    SCAN_ACTION( xadjust, y );  /* Safety for diagonal moves */ \
                                    }             \
                                limit += ( fint16 )grid; \
                                } \
                            else \
                                { \
                                xmode; \
                                } \
                            SCAN_ACTION( x, y ); \
                            } \
                        } \
                    else \
                        { \
                        limit = ( int16 )( ( float )y*grid ); \
                        while ( x != xt ) \
                            { \
                            y1 -= step;     \
                            /* Needed checking due to precision */ \
                            if ( y1 < y2 ) \
                                { \
                                y1 = y2; \
                                } \
                            base; \
                            if ( y1 < limit ) \
                                { \
                                if ( x1 xcomp x2 ) \
                                    {                                 \
                                    if ( y2 >= limit ) \
                                        { \
                                        xmode; \
                                        SCAN_ACTION( x, y ); \
                                        if ( ( POS( y2 ) != y ) ) \
                                            { \
                                            SCAN_ACTION( x, y-1 ); \
                                            } \
                                        return false; \
                                        } \
                                    cut = calc_cut; \
                                    y1 = y2; \
                                    } \
                                if ( y1+step-cut < limit ) \
                                    { \
                                    y--; \
                                    SCAN_ACTION( x, y ); \
                                    xmode; \
                                    SCAN_ACTION( x, y+1 ); /* Safety for diagonal moves */  \
                                    } \
                                else \
                                    { \
                                    xmode; \
                                    SCAN_ACTION( x, y ); \
                                    y--; \
                                    SCAN_ACTION( xadjust, y );  /* Safety for diagonal moves */ \
                                    } \
                                limit -= ( int )grid; \
                                } \
                            else \
                                { \
                                xmode; \
                                } \
                            SCAN_ACTION( x, y ); \
                            } \
                        }

/*
* Run tracing from grid pos a to grid pos b
* when Y change is greater than X change
*/
#define SCAN_FUNC_Y( xadjust, xmode, base, xcomp, calc_cut ) \
                    if ( x2 > x1 ) \
                        { \
                        limit = ( int16 )( ( x+1 )*grid ); \
                        while ( y != yt ) \
                            { \
                            x1 += step; \
                            base; \
                            if ( x1 >= limit ) \
                                { \
                                if ( y1 xcomp y2 ) \
                                    { \
                                    if ( x2 < limit ) \
                                        { \
                                        xmode; \
                                        SCAN_ACTION( x, y ); \
                                        if ( ( POS( x2 ) != x ) ) \
                                            { \
                                            SCAN_ACTION( x+1, y ); \
                                            } \
                                        return false; \
                                        } \
                                    cut = calc_cut; \
                                    x1 = x2; \
                                    }                            \
                                if ( x1-step+cut >= limit ) \
                                    { \
                                    x++; \
                                    SCAN_ACTION( x, y ); \
                                    xmode; \
                                    SCAN_ACTION( x-1, y ); /* Safety for diagonal moves */  \
                                    } \
                                else \
                                    { \
                                    xmode;     \
                                    SCAN_ACTION( x, y ); \
                                    x++; \
                                    SCAN_ACTION( x, xadjust );  /* Safety for diagonal moves */  \
                                    }             \
                                limit += ( int16 )grid; \
                                } \
                            else \
                                { \
                                xmode; \
                                } \
                            SCAN_ACTION( x, y ); \
                            } \
                        } \
                    else \
                        { \
                        limit = ( int16 )( ( float )x*grid ); \
                        while ( y != yt ) \
                            { \
                            x1 -= step; \
                            /* Needed checking due to precision */ \
                            if ( x1 < x2 ) \
                                { \
                                x1 = x2; \
                                } \
                            base; \
                            if ( x1 < limit ) \
                                { \
                                if ( y1 xcomp y2 ) \
                                    { \
                                    if ( x2 >= limit ) \
                                        { \
                                        xmode; \
                                        SCAN_ACTION( x, y ); \
                                        if ( ( POS( x2 ) != x ) ) \
                                            { \
                                            SCAN_ACTION( x-1, y ); \
                                            } \
                                        return false; \
                                        } \
                                    cut = calc_cut; \
                                    x1 = x2; \
                                    } \
                                if ( x1+step-cut < limit ) \
                                    { \
                                    x--; \
                                    SCAN_ACTION( x, y ); \
                                    xmode; \
                                    SCAN_ACTION( x+1, y ); /* Safety for diagonal moves */ \
                                    } \
                                else \
                                    { \
                                    xmode; \
                                    SCAN_ACTION( x, y ); \
                                    x--; \
                                    SCAN_ACTION( x, xadjust );  /* Safety for diagonal moves */  \
                                    } \
                                limit -= ( int16 )grid; \
                                } \
                            else \
                                { \
                                xmode; \
                                } \
                            SCAN_ACTION( x, y ); \
                            } \
                        }



template<typename Function>
inline bool bs::Level::InternalScan(
	Function&& function /*bool(*trace_callback)(fint16 x, fint16 y, void* data)*/,
	float x1, float y1, const float x2, const float y2, void* data)
{
	float x_len;
	float y_len;

	fint16 limit;
	float step;
	float cut;

	ASSERT((x1 > 0) && (y1 > 0) && (x2 < MAX_LENGTH) && (y2 < MAX_LENGTH), "Out of bounds");

	fint16 x = POS(x1);
	fint16 y = POS(y1);
	fint16 xt = POS(x2);
	fint16 yt = POS(y2);

#ifdef _DEBUG
	last_scan_coord[0] = x;
	last_scan_coord[1] = y;

	if (field_scan_trace)
	{
		printf("x1: %f y1: %f x2: %f y2:%f ( %i,%i -> %i,%i ) \n",
			x1, y1, x2, y2, x, y, xt, yt);
	}
#endif /* _DEBUG */


	SCAN_ACTION(x, y);

	/*
	* Check Line Direction
	*/
	if (xt != x)
	{
		if (yt != y)
		{
			x_len = fabs(x2 - x1);
			y_len = fabs(y2 - y1);
#ifdef _DEBUG

			if (field_scan_trace)
			{
				printf("x_len: %f, y_len: %f\n", x_len, y_len);
			}
#endif /* _DEBUG */
			if (x_len > y_len /* x_pri < y_pri */)
			{
				// y_len / ( x_len / grid ) =>
				step = grid*y_len / x_len;

#ifdef _DEBUG
				if (field_scan_trace)
				{
					printf("y_step: %f\n", step);
				}
#endif /* _DEBUG */
				if (x1 < x2)
				{
					cut = CALCULATE_CUT(grid*(x + 1), x1);
					SCAN_FUNC(x - 1, x++, x1 += grid, >= ,
						CALCULATE_CUT(grid*(x + 1), x2 - grid));
				}
				else
				{
					cut = -CALCULATE_CUT(grid*(x), x1);
					SCAN_FUNC(x + 1, x--, x1 -= grid, <= ,
						-CALCULATE_CUT(grid*x, x2 + grid));
				}
				if (yt != y)
				{
					SCAN_ACTION(x, yt);
				}
			}
			else
			{
				// x_len / ( y_len / grid ) =>
				step = grid*x_len / y_len;
#ifdef _DEBUG
				if (field_scan_trace)
				{
					printf("x_step: %f\n", step);
				}
#endif /* _DEBUG */
				if (y1 < y2)
				{
					cut = CALCULATE_CUT(grid*(y + 1), y1);
					SCAN_FUNC_Y(y - 1, y++, y1 += grid, >= ,
						CALCULATE_CUT(grid*(y + 1), y2 - grid))
				}
				else
				{
					cut = -CALCULATE_CUT(grid*(y), y1);
					SCAN_FUNC_Y(y + 1, y--, y1 -= grid, <= ,
						-CALCULATE_CUT(grid*y, y2 + grid))
				}
				if (xt != x)
				{
					SCAN_ACTION(xt, y);
				}
			}
		}
		else if (x1 < x2)
		{
			/* Horizontal line: Left to Right */
			for (x = x + 1; x <= xt; x++)
			{
				SCAN_ACTION(x, y);
			}
		}
		else
		{
			/* Horizontal line: Right to Left */
			for (x = x - 1; x >= xt; x--)
			{
				SCAN_ACTION(x, y);
			}
		}
	}
	else if (yt != y)
	{
		if (y1 < y2)
		{
			/* Vertical line (Up to Down) */
			for (y = y + 1; y <= yt; y++)
			{
				SCAN_ACTION(x, y);
			}
		}
		else
		{
			/* Vertical line (Down to Up) */
			for (y = y - 1; y >= yt; y--)
			{
				SCAN_ACTION(x, y);
			}
		}
	}
	return false;
}

#undef SCAN_ACTION
#undef CALCULATE_CUT
