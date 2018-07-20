#include "LY_Math.hpp"

int LY::Math::Pow(int x, int y)
{
	long salida=x;
    for(int i=1;i<y;i++)
    {
    	salida=salida*x;
    }
    if(y==0) 
    {
    	return 1;	
    }
    else
    {
    	return salida;
    }
}
