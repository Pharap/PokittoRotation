
/* Pokitto drawBitmap example - draws a Pokitto icon by @trelemar */
#include "Pokitto.h" // include Pokitto library
#include "pokitto_icon.h" // include the Pokitto icon graphics file

#include "FixedPoints.h"
#include "FixedPointsCommon.h"

#include <cstddef>
#include <cstdint>
#include <cmath>

using Fract = SQ15x16;

//Fract sine[361];
//Fract cosine[361];

Fract angleTable[64];

constexpr const float floatPi = 3.141592654f;
constexpr const double doublePi = 3.141592654;

void generateAngleTable()
{
	const double factor = doublePi / 128.0;
	for(unsigned int a = 0; a < 64; ++a)
	{
		angleTable[a] = static_cast<Fract>(std::sin(static_cast<double>(a) * factor));
	}
}

Fract lookupAngle(std::uint_fast8_t index)
{
    return (index == 64) ? 1 : angleTable[(index & (0x3F))];
}

Fract Sin(std::uint8_t brads)
{
	const std::uint_fast8_t fastBrads = brads;
	const std::uint_fast8_t quarter = ((fastBrads >> 6) & 0x03);
	const std::uint_fast8_t index = ((fastBrads >> 0) & 0x3F);
	switch (quarter)
	{
		case 0: return lookupAngle(index);
		case 1: return lookupAngle(64 - index);
		case 2: return -lookupAngle(index);
		case 3: return -lookupAngle(64 - index);
		default: return 0;
	}
}

Fract Cos(std::uint8_t brads)
{
	const std::uint_fast8_t fastBrads = brads;
	const std::uint_fast8_t quarter = ((fastBrads >> 6) & 0x03);
	const std::uint_fast8_t index = ((fastBrads >> 0) & 0x3F);
	switch (quarter)
	{
		case 0: return lookupAngle(64 - index);
		case 1: return -lookupAngle(index);
		case 2: return -lookupAngle(64 - index);
		case 3: return lookupAngle(index);
		default: return 0;
	}
}

int main(void)
{
    Pokitto::Core::begin(); // start the application
    Pokitto::Display::load565Palette(sprite_pal); //load the palette for the image
    Pokitto::Display::bgcolor = 0;

    /*Fract angle=0;
	for(int t=0; t<=360; t++)
	{
		sine[t] = sin(static_cast<double>(angle));
		cosine[t] = cos(static_cast<double>(angle));
		angle += (0.0174533); // radians
	}*/
	generateAngleTable();

	unsigned int dstW = Pokitto::Display::getWidth();
	unsigned int dstH = Pokitto::Display::getHeight();
	unsigned int fDstCX = dstW/2;
	unsigned int fDstCY = dstH/2;

	unsigned int srcW = sprite[0];
	unsigned int srcH = sprite[1];
	unsigned int fSrcCX = srcW/2;
	unsigned int fSrcCY = srcH/2;

	//const Fract fScale = 2.5;
	const Fract inverseScale = 1.0 / 2.5;
	unsigned int fAngle = 0;

	/* the "while" loop runs as long as the program is running */
	while (Pokitto::Core::isRunning())
	{

		/* Pokitto::Core::update() is processed whenever it is time to update the screen */

		if (Pokitto::Core::update())
		{

			Fract s = Sin(fAngle);//sine[fAngle];  // pre-calculated look-up
			Fract c = Cos(fAngle);//cosine[fAngle];

			Fract duCol = s * inverseScale;
			Fract dvCol = c * inverseScale;
			Fract duRow = dvCol;
			Fract dvRow = -duCol;

			Fract startingu = fSrcCX - (fDstCX * dvCol + fDstCY * duCol);
			Fract startingv = fSrcCY - (fDstCX * dvRow + fDstCY * duRow);

			Fract rowu = startingu;
			Fract rowv = startingv;

			for(unsigned int y = 0; y < dstH; ++y)
			{
				Fract u = rowu;
				Fract v = rowv;

				for(unsigned int x = 0; x < dstW; ++x)
				{

					if ( u >=0 && u < srcW && v >=0 && v < srcH )
					{
						const int16_t iv = static_cast<int16_t>(v);
						const int16_t iu = static_cast<int16_t>(u);
						unsigned int i = (iv * (srcW/2)) + ((srcW - iu) / 2);
						uint8_t pixel = sprite[i + 2];
						uint8_t colour = ((iu & 1) != 0) ? ((pixel >> 0) & 0x0F) : ((pixel >> 4) & 0x0F);

						Pokitto::Display::drawPixel(x, y, colour);
					}

					u += duRow;
					v += dvRow;
				}

				rowu = rowu + duCol;
				rowv = rowv + dvCol;

			}
			--fAngle;
		}
	}

	return 0;
}
