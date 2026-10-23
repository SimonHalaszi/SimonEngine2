#ifndef SCREENSHOT_HPP
#define SCREENSHOT_HPP

/*
 *  SimonEngine2 :: CHUCK SPECIAL EDITION :: "CLIP IT!" (F9)
 *
 *  Grabs the frame that was just drawn (straight from the back buffer, before the swap)
 *  and writes it next to the executable as SE2_CHUCK_CLIP_0001.bmp, 0002, 0003...
 *
 *  BMP because it's the one image format you can write by hand in ~40 lines with zero
 *  libraries, and every OS can open it. Bonus: BMP stores rows bottom-up, which is
 *  EXACTLY the order glReadPixels hands them over, so no flipping needed. A rare W.
 */

#include <string>

namespace Screenshot {

	// Returns the file name written, or an empty string if something went wrong
	std::string saveBmp();
}

#endif
