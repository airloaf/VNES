#pragma once

namespace VNES{ namespace PPU{

typedef uint8_t Pixel;

struct ScanLine {
	Pixel pixels[256];
};

struct Frame {
	ScanLine scanLines[240];
};

// SDL2 specific stuff
// The order must be a, b, g, r. C byte packs from the bottom up for this struct
// This means the first byte for a PixelData instance will be r followed by g then b then a
// If the order was reversed then it would be a then b then g then r
struct PixelData{
	uint8_t a;
	uint8_t b;
	uint8_t g;
	uint8_t r;
};

class Renderer {
	public:

		/**
		 * @breif Handle the given frame. The best approach would be
		 * to copy the given frame into an internal buffer to be rendered
		 * at a later date.
		 * 
		 * @see readyToRender()
		 * 
		 * @param f - the frame data
		 */
		virtual void handleFrame(const Frame &f) = 0;

		/**
		 * @brief Indicates that an entire frame's worth of data has
		 * been sent and should be rendered as soon as possible. This
		 * function should not be too slow, <16 ms window.
		 */
		virtual void readyToRender() = 0;
};

}}
