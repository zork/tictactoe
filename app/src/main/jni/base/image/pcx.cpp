////
// pcx.cpp
////

#include "base/image/pcx.h"

#include "base/file/memory_file.h"
#include "base/logging.h"

namespace {

struct PCXHeader {
  uint8_t manufacturer;
  uint8_t version;
  uint8_t encoding;
  uint8_t bitsperpixel;
  uint16_t x_min;
  uint16_t y_min;
  uint16_t x_max;
  uint16_t y_max;
  uint16_t horz_pixel;
  uint16_t vert_pixel;
  uint8_t palette_ega[48];
  uint8_t reserved;
  uint8_t numplanes;
  uint16_t bytes_line;
  uint16_t palette_type;
  uint8_t extra[58];
};
static_assert(sizeof(PCXHeader) == 128, "Fix PCX header memory alignment");
}

std::unique_ptr<Bitmap> LoadPcx(File* input) {
  auto buffer = std::make_unique<MemoryFile>(input);

  // Read the header.
  PCXHeader header;
  buffer->Read(&header, sizeof(header));

  // Check the header.
  if (header.manufacturer != 10) {
    LOG(ERROR) << "PCX manufacturer must be 10: " << header.manufacturer;
    return nullptr;
  }
  if (header.bitsperpixel != 8) {
    LOG(ERROR) << "PCX bpp must be 8: " << header.bitsperpixel;
    return nullptr;
  }
  if (header.encoding != 1) {
    LOG(ERROR) << "PCX encoding must be 1: " << header.encoding;
    return nullptr;
  }

  int width = header.x_max + 1 - header.x_min;
  int height = header.y_max + 1 - header.y_min;

  // Allocate the image.
  auto image = std::make_unique<Bitmap>(width, height, false);

  if (header.numplanes == 3) {
    // RGB image, no palette.
    for (int y = 0; y < height; ++y) {
      for (int channel = 0; channel < 3; ++channel) {
        int x = 0;
        // Read the pixels.
        int pixels = width;
        while (pixels > 0) {
          uint8_t current;
          if (!buffer->ReadByte(&current)) {
            LOG(ERROR) << "Unexpected end of PCX";
            return nullptr;
          }
          // Values higher than 191 are a run length.
          if ((current & 0xC0) == 0xC0) {
            int loops = current & 0x3F;
            pixels -= loops;
            if (pixels < 0 || !buffer->ReadByte(&current)) {
              LOG(ERROR) << "Invalid PCX data";
              return nullptr;
            }
            // Write a row of pixels.
            while (loops-- > 0) {
              image->image_data()[(y * width + x) * 3 + channel] = current;
              x++;
            }
          } else {
            // Write a single pixel
            image->image_data()[(y * width + x) * 3 + channel] = current;
            x++;
            pixels--;
          }
        }
      }
    }
  } else {
    // Read the palette.
    uint8_t palette[256][3];
    buffer->Seek(-768L, File::kSeekEnd);
    if (!buffer->Read(palette, 256 * 3)) {
      LOG(ERROR) << "Could not read PCX palette";
      return nullptr;
    }

    // Return to the image data.
    buffer->Seek(128L, File::kSeekSet);

    // Read the pixels.
    int pixels = width * height;
    int offset = 0;
    while (pixels) {
      uint8_t current;
      if (!buffer->ReadByte(&current)) {
        LOG(ERROR) << "Unexpected end of PCX";
        return nullptr;
      }
      // Values greater than 191 are run lengths.
      if ((current & 0xC0) == 0xC0) {
        int loops = current & 0x3F;
        pixels -= loops;
        // Get the color index.
        if (!buffer->ReadByte(&current)) {
          LOG(ERROR) << "Unexpected end of PCX";
          return nullptr;
        }
        while (loops-- > 0) {
          for (int i = 0; i < 3; ++i) {
            image->image_data()[offset++] = palette[current][i];
          }
        }
      } else {
        // Single pixel.
        for (int i = 0; i < 3; ++i) {
          image->image_data()[offset++] = palette[current][i];
        }
        pixels--;
      }
    }
  }
  return image;
}
