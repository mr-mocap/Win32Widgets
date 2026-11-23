#include "Color.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif


RGBTRIPLE ConstructRGBTripleFromBytes(BYTE red, BYTE green, BYTE blue)
{
    RGBTRIPLE rgb;

    rgb.rgbtBlue = blue;
    rgb.rgbtGreen = green;
    rgb.rgbtRed = red;

    return rgb;
}

UNITRGBTRIPLE ConstructUnitRGBTripleFromFloats(float red, float green, float blue)
{
    UNITRGBTRIPLE unitrgb;

    unitrgb.blue  = max(0.0f, min(1.0f, blue));
    unitrgb.green = max(0.0f, min(1.0f, green));
    unitrgb.red   = max(0.0f, min(1.0f, red));

    return unitrgb;
}

UNITRGBTRIPLE RGBTripleToUnitRGBTriple(RGBTRIPLE rgb)
{
    // Convert RGB values from 0-255 range to 0.0-1.0 range

    UNITRGBTRIPLE unitrgb;

    unitrgb.red   = rgb.rgbtRed / 255.0f;
    unitrgb.green = rgb.rgbtGreen / 255.0f;
    unitrgb.blue  = rgb.rgbtBlue / 255.0f;

    return unitrgb;
}

RGBTRIPLE UnitRGBTripleToRGBTriple(UNITRGBTRIPLE rgb)
{
    // Convert RGB values from 0.0-1.0 range to 0-255 range
    RGBTRIPLE retval;

    retval.rgbtBlue  = (BYTE)(rgb.blue * 255.0f);
    retval.rgbtGreen = (BYTE)(rgb.green * 255.0f);
    retval.rgbtRed   = (BYTE)(rgb.red * 255.0f);

    return retval;
}

UNITRGBTRIPLE UnitHSVTripleToUnitRGBTriple(UNITHSVTRIPLE hsv_color)
{
    float h = hsv_color.hue;
    float s = hsv_color.saturation;
    float v = hsv_color.value;
    float r, g, b;

    if ( s == 0.0f )
    {
        // Achromatic (grey)
        r = g = b = v;
    }
    else
    {
        h *= 6.0f; // sector 0 to 5

        int i = (int)floor(h);
        float fractional_part_of_h = h - i;
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * fractional_part_of_h);
        float t = v * (1.0f - s * (1.0f - fractional_part_of_h));

        switch ( i % 6 )
        {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        }
    }
    return ConstructUnitRGBTripleFromFloats(r, g, b);
}

UNITHSVTRIPLE UnitRGBTripleToUnitHSVTriple(UNITRGBTRIPLE rgb_color)
{
    float r = rgb_color.red;
    float g = rgb_color.green;
    float b = rgb_color.blue;

    float max_val = fmaxf(fmaxf(r, g), b);
    float min_val = fminf(fminf(r, g), b);
    float delta = max_val - min_val;

    float h, s, v;
    v = max_val;

    if (delta == 0.0f)
    {
        h = 0.0f;
        s = 0.0f;
    }
    else
    {
        s = delta / max_val;

        if (max_val == r)
        {
            h = (g - b) / delta + (g < b ? 6.0f : 0.0f);
        }
        else if (max_val == g)
        {
            h = (b - r) / delta + 2.0f;
        }
        else
        {
            h = (r - g) / delta + 4.0f;
        }
        h /= 6.0f;
    }

    UNITHSVTRIPLE hsv_color;

    hsv_color.hue        = h;
    hsv_color.saturation = s;
    hsv_color.value      = v;

    return hsv_color;
}

BOOL IsNormalized(float value)
{
    return (value >= 0.0f && value <= 1.0f);
}

BOOL IsValidUnitRGBTriple(UNITRGBTRIPLE rgb)
{
    return IsNormalized(rgb.red)   &&
           IsNormalized(rgb.green) &&
           IsNormalized(rgb.blue);
}

BOOL IsValidUnitHSVTriple(UNITHSVTRIPLE hsv)
{
    return IsNormalized(hsv.hue)        &&
           IsNormalized(hsv.saturation) &&
           IsNormalized(hsv.value);
}

#ifdef __cplusplus
}
#endif
