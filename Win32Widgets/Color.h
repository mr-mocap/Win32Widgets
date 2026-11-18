#pragma once

#include <windows.h>
#include <wingdi.h>

/** @file
 *
 *  Defines various color types and operations for working with them
 *
 *  @hideincludegraph
 */


#ifdef __cplusplus
extern "C" {
#endif

/** @name Types
 *  @{
 */

/** A structure representing an RGB color in the range 0.0 - 1.0
 */
typedef struct _UNITRGBTRIPLE
{
    float red;
    float green;
    float blue;
} UNITRGBTRIPLE, *PUNITRGBTRIPLE, NEAR *NPUNITRGBTRIPLE, FAR *FPUNITRGBTRIPLE;

/** A structure representing an HSV color in the range 0.0 - 1.0
 */
typedef struct _UNITHSVTRIPLE
{
    float hue;
    float saturation;
    float value;
} UNITHSVTRIPLE, *PUNITHSVTRIPLE, NEAR *NPUNITHSVTRIPLE, FAR *LPUNITHSVTRIPLE;
/// @}  [Types]


/** @name Constructors
 *  @{
 */
RGBTRIPLE     ConstructRGBTripleFromBytes(BYTE red, BYTE green, BYTE blue);

/** Constructs a unit RGB triple from floating-point values.
 * 
 *  @param red   The red component (0.0 - 1.0)
 *  @param green The green component (0.0 - 1.0)
 *  @param blue  The blue component (0.0 - 1.0)
 *
 *  @return The constructed unit RGB triple
 * 
 *  @note Values outside the range 0.0 - 1.0 will be clamped to that range
 */
UNITRGBTRIPLE ConstructUnitRGBTripleFromFloats(float red, float green, float blue);
/// @}  [Constructors]

/** @name Conversions
 *  @{
 */
UNITRGBTRIPLE RGBTripleToUnitRGBTriple(RGBTRIPLE rgb_color);

RGBTRIPLE     UnitRGBTripleToRGBTriple(UNITRGBTRIPLE rgb_color);

UNITRGBTRIPLE UnitHSVTripleToUnitRGBTriple(UNITHSVTRIPLE hsv_color);

UNITHSVTRIPLE UnitRGBTripleToUnitHSVTriple(UNITRGBTRIPLE rgb_color);
/// @}  [Conversions]

/** @name Validity Checking
 *  @{
 */

/** Determines whether a floating-point value is normalized.
 * 
 *  @param value The floating-point value to check.
 * 
 *  @return @c true if the value is normalized; otherwise, @c false.
 * 
 *  @note A normalized value is one that falls within the inclusive range of 0.0 to 1.0.
 */
bool IsNormalized(float value);

/** Checks whether a UNITRGBTRIPLE contains valid unit values.
 * 
 *  @param rgb The UNITRGBTRIPLE value to validate.
 * 
 *  @return @c true if the RGB triple is valid; otherwise, @c false.
 * 
 *  @note A normalized value is one that falls within the inclusive range of 0.0 to 1.0.
 */
bool IsValidUnitRGBTriple(UNITRGBTRIPLE rgb);

/** Checks whether a UNITHSVTRIPLE contains valid unit values.
 * 
 *  @param hsv The UNITHSVTRIPLE value to validate.
 * 
 *  @return @c true if the HSV triple is valid; otherwise, @c false.
 * 
 *  @note A normalized value is one that falls within the inclusive range of 0.0 to 1.0.
 */
bool IsValidUnitHSVTriple(UNITHSVTRIPLE hsv);
/// @}

#ifdef __cplusplus
}
#endif
