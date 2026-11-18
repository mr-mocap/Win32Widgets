#include "CppUnitTest.h"
#include "Color.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Win32WidgetsTests
{
    TEST_CLASS(ColorTests)
    {
    public:
        TEST_METHOD(ConstructRGBTripleFromBytesMapsChannels)
        {
            RGBTRIPLE result = ConstructRGBTripleFromBytes(0x12, 0x34, 0x56);

            Assert::AreEqual(static_cast<BYTE>(0x12), result.rgbtRed);
            Assert::AreEqual(static_cast<BYTE>(0x34), result.rgbtGreen);
            Assert::AreEqual(static_cast<BYTE>(0x56), result.rgbtBlue);
        }

        TEST_METHOD(ConstructUnitRGBTripleClampsOutOfRangeValues)
        {
            UNITRGBTRIPLE result = ConstructUnitRGBTripleFromFloats(-0.25f, 0.5f, 1.25f);

            Assert::AreEqual(0.0f, result.red, 0.0001f);
            Assert::AreEqual(0.5f, result.green, 0.0001f);
            Assert::AreEqual(1.0f, result.blue, 0.0001f);
        }

        TEST_METHOD(RGBTripleToUnitRGBTripleMapsByteRange)
        {
            RGBTRIPLE rgb = ConstructRGBTripleFromBytes(255, 128, 0);

            UNITRGBTRIPLE result = RGBTripleToUnitRGBTriple(rgb);

            Assert::AreEqual(1.0f, result.red, 0.0001f);
            Assert::AreEqual(128.0f / 255.0f, result.green, 0.0001f);
            Assert::AreEqual(0.0f, result.blue, 0.0001f);
        }

        TEST_METHOD(UnitRGBTripleToRGBTripleMapsUnitRange)
        {
            UNITRGBTRIPLE rgb = { 1.0f, 128.0f / 255.0f, 0.0f };

            RGBTRIPLE result = UnitRGBTripleToRGBTriple(rgb);

            Assert::AreEqual(static_cast<BYTE>(255), result.rgbtRed);
            Assert::AreEqual(static_cast<BYTE>(128), result.rgbtGreen);
            Assert::AreEqual(static_cast<BYTE>(0), result.rgbtBlue);
        }

        TEST_METHOD(IsNormalizedAcceptsInclusiveRange)
        {
            Assert::IsTrue(IsNormalized(0.0f));
            Assert::IsTrue(IsNormalized(0.5f));
            Assert::IsTrue(IsNormalized(1.0f));
        }

        TEST_METHOD(IsNormalizedRejectsOutOfRangeAndNanAndInfinity)
        {
            Assert::IsFalse(IsNormalized(-0.0001f));
            Assert::IsFalse(IsNormalized(1.0001f));
            Assert::IsFalse(IsNormalized(NAN));
            Assert::IsFalse(IsNormalized(HUGE_VALF));
        }

        TEST_METHOD(IsValidUnitRGBTripleChecksEveryChannel)
        {
            Assert::IsTrue(IsValidUnitRGBTriple({ 0.0f, 0.5f, 1.0f }));
            Assert::IsFalse(IsValidUnitRGBTriple({ -0.1f, 0.5f, 1.0f }));
            Assert::IsFalse(IsValidUnitRGBTriple({ 0.0f, 1.1f, 1.0f }));
            Assert::IsFalse(IsValidUnitRGBTriple({ 0.0f, 0.5f, NAN }));
            Assert::IsFalse(IsValidUnitRGBTriple({ 0.0f, 0.5f, HUGE_VALF }));
        }

        TEST_METHOD(IsValidUnitHSVTripleChecksEveryChannel)
        {
            Assert::IsTrue(IsValidUnitHSVTriple({ 0.0f, 0.5f, 1.0f }));
            Assert::IsFalse(IsValidUnitHSVTriple({ -0.1f, 0.5f, 1.0f }));
            Assert::IsFalse(IsValidUnitHSVTriple({ 0.0f, 1.1f, 1.0f }));
            Assert::IsFalse(IsValidUnitHSVTriple({ 0.0f, 0.5f, NAN }));
            Assert::IsFalse(IsValidUnitHSVTriple({ 0.0f, 0.5f, HUGE_VALF }));
        }

        TEST_METHOD(UnitRGBTripleREDToUnitHSVTripleConvertsCorrectly)
        {
            UNITRGBTRIPLE rgb = { 1.0f, 0.0f, 0.0f }; // Red
            UNITHSVTRIPLE hsv = UnitRGBTripleToUnitHSVTriple(rgb);

            Assert::AreEqual(0.0f, hsv.hue, 0.0001f);
            Assert::AreEqual(1.0f, hsv.saturation, 0.0001f);
            Assert::AreEqual(1.0f, hsv.value, 0.0001f);
        }

        TEST_METHOD(UnitRGBTripleGREENToUnitHSVTripleConvertsCorrectly)
        {
            UNITRGBTRIPLE rgb = { 0.0f, 1.0f, 0.0f }; // Green
            UNITHSVTRIPLE hsv = UnitRGBTripleToUnitHSVTriple(rgb);

            Assert::AreEqual(1.0f/3.0f, hsv.hue, 0.0001f);
            Assert::AreEqual(1.0f, hsv.saturation, 0.0001f);
            Assert::AreEqual(1.0f, hsv.value, 0.0001f);
        }

        TEST_METHOD(UnitRGBTripleBLUEToUnitHSVTripleConvertsCorrectly)
        {
            UNITRGBTRIPLE rgb = { 0.0f, 0.0f, 1.0f }; // Blue
            UNITHSVTRIPLE hsv = UnitRGBTripleToUnitHSVTriple(rgb);

            Assert::AreEqual(2.0f/3.0f, hsv.hue, 0.0001f);
            Assert::AreEqual(1.0f, hsv.saturation, 0.0001f);
            Assert::AreEqual(1.0f, hsv.value, 0.0001f);
        }

        TEST_METHOD(UnitRGBTripleWHITEToUnitHSVTripleConvertsCorrectly)
        {
            UNITRGBTRIPLE rgb = { 1.0f, 1.0f, 1.0f }; // White
            UNITHSVTRIPLE hsv = UnitRGBTripleToUnitHSVTriple(rgb);

            Assert::AreEqual(0.0f, hsv.hue, 0.0001f);
            Assert::AreEqual(0.0f, hsv.saturation, 0.0001f);
            Assert::AreEqual(1.0f, hsv.value, 0.0001f);
        }

        TEST_METHOD(UnitRGBTripleBLACKToUnitHSVTripleConvertsCorrectly)
        {
            UNITRGBTRIPLE rgb = { 0.0f, 0.0f, 0.0f }; // Black
            UNITHSVTRIPLE hsv = UnitRGBTripleToUnitHSVTriple(rgb);

            Assert::AreEqual(0.0f, hsv.hue, 0.0001f);
            Assert::AreEqual(0.0f, hsv.saturation, 0.0001f);
            Assert::AreEqual(0.0f, hsv.value, 0.0001f);
        }

        // TODO: Add more tests for other colors and edge cases
    };
}
