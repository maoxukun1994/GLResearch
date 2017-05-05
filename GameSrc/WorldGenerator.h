#ifndef WORLDGENERATOR_H
#define WORLDGENERATOR_H


#include <cmath>

class WorldGenerator
{

private:

    float linear_Interpolate(float a,float b,float x)
    {
        return  a*(1.0f-x) + b*x;
    }

    float cos_Interpolate(float a,float  b,float x)
    {

        float ft = x * 3.141592653;
        float f = (1 - cos(ft)) *0.5f;
        return  a*(1.0-f) + b*f;
    }

public:

    float noise_2d_1(int x,int y)
    {
        int n = x*3 + y * 17;
        n = (n<<13) ^ n;
        return ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
    }

    float smoothNoise_2d_1(float xx,float yy)
    {
        int x,y;
        x = (int)xx;
        y = (int)yy;

        float corners = ( noise_2d_1(x-1, y-1)+noise_2d_1(x+1, y-1)+noise_2d_1(x-1, y+1)+noise_2d_1(x+1, y+1) ) / 16;
        float sides   = ( noise_2d_1(x-1, y)  +noise_2d_1(x+1, y)  +noise_2d_1(x, y-1)  +noise_2d_1(x, y+1) ) /  8;
        float center  =  noise_2d_1(x, y) / 4;

        return (corners + sides + center);
    }

    float interPolatedNoise_2d_1(float xx,float yy)
    {
        int x = (int)xx;
        float fractional_x = xx - x;

        int y = (int)yy;
        float fractional_y = yy - y;

        float v1 = this->smoothNoise_2d_1(x,y);
        float v2 = this->smoothNoise_2d_1(x+1,y);
        float v3 = this->smoothNoise_2d_1(x,y+1);
        float v4 = this->smoothNoise_2d_1(x+1,y+1);


        float i1 = this->cos_Interpolate(v1,v2,fractional_x);
        float i2 = this->cos_Interpolate(v3,v4,fractional_x);

        return this->cos_Interpolate(i1,i2,fractional_y);
    }

    float perlinNoise_2d_1(float xx,float yy)
    {
        float frequency,amplitude,total = 0.0f;

        for(int n = 0;n<6;n++)
        {
            frequency = pow(2.0,n);
            amplitude = 48 * pow(0.5,n);
            total += (this->interPolatedNoise_2d_1(xx * frequency, yy * frequency)) * amplitude;
        }
        return total;
    }

};




#endif // WORLDGENERATOR_H
