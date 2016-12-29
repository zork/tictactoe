////
// pcx.h
////

#pragma once

#include "base/image/bitmap.h"

#include <memory>

class File;

std::unique_ptr<Bitmap> LoadPcx(File* input);
