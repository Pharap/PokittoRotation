
/* Pokitto drawBitmap example - draws a Pokitto icon by @trelemar */
#include "Pokitto.h" // include Pokitto library
#include "pokitto_icon.h" // include the Pokitto icon graphics file

#include "FixedPoints.h"
#include "FixedPointsCommon.h"

using Fract = SQ15x16;

Fract sine[361];
Fract cosine[361];

int main(void)
{
    Pokitto::Core::begin(); // start the application
    Pokitto::Display::load565Palette(sprite_pal); //load the palette for the image
    Pokitto::Display::bgcolor = 0;

    Fract angle=0;
	for(int t=0; t<=360; t++)
	{
		sine[t] = sin(static_cast<double>(angle));
		cosine[t] = cos(static_cast<double>(angle));
		angle += (0.0174533); // radians
	}

	int dstW = Pokitto::Display::getWidth();
	int dstH = Pokitto::Display::getHeight();
	int srcW = sprite[0];
	int srcH = sprite[1];
	int fDstCX = dstW/2;
	int fDstCY = dstH/2;
	int fSrcCX = srcW/2;
	int fSrcCY = srcH/2;
	int fAngle = 0;
	Fract fScale = 2.5;

	int col;

	/* the "while" loop runs as long as the program is running */
	while (Pokitto::Core::isRunning())
	{

		/* Pokitto::Core::update() is processed whenever it is time to update the screen */
		if (Pokitto::Core::update())
		{

			Fract SIN = sine[fAngle];  // pre-calculated look-up
			Fract COS = cosine[fAngle];

			Fract duCol = SIN * (1.0 / fScale);
			Fract dvCol = COS * (1.0 / fScale);
			Fract duRow = dvCol;
			Fract dvRow = -duCol;

			Fract startingu = fSrcCX - (fDstCX * dvCol + fDstCY * duCol);
			Fract startingv = fSrcCY - (fDstCX * dvRow + fDstCY * duRow);

			Fract rowu = startingu;
			Fract rowv = startingv;

			for(int y = 0; y < dstH; ++y)
			{

				Fract u = rowu;
				Fract v = rowv;

				for(int x = 0; x < dstW; ++x)
				{

					if ( u >=0 && u < srcW && v >=0 && v < srcH )
					{
						uint16_t i = static_cast<int16_t>(v) * (srcW/2) + ((srcW-static_cast<int16_t>(u))>>1);
						uint8_t pixel = sprite[i+2];
						col = (static_cast<int16_t>(u) & 1) ? ((pixel >> 0) & 0x0F) : ((pixel >> 4) & 0x0F);

						Pokitto::Display::drawPixel(x,y,col);
					}

					u += duRow;
					v += dvRow;
				}

				rowu = rowu + duCol;
				rowv = rowv + dvCol;

			}
			// if(ang++>=359) ang=0;
			fAngle -= 1;
			if(fAngle < 1)
				fAngle = 359;

		}
	}

	return 0;
}
