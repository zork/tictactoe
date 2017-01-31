////
// texture.cpp
////

#include "game/render/texture.h"

#include "base/image/bitmap.h"
#include "base/logging.h"
#include "base/math/math.h"
#include "base/platform.h"
#include "base/thread/thread_util.h"
#include "game/render/gl.h"

namespace {
#if (OS_WIN)
const int kInternalFormat = GL_BGR;
const int kInternalAlphaFormat = GL_BGRA;
#else
const int kInternalFormat = GL_RGB;
const int kInternalAlphaFormat = GL_RGBA;
#endif
}

// static
std::unique_ptr<Texture> Texture::LoadResizedImage(
    std::unique_ptr<Bitmap> image,
    Filter mag_filter,
    Filter min_filter,
    Wrap wrap,
    float* texture_right,
    float* texture_top) {
  int image_width = image->width();
  int image_height = image->height();

  // GL textures must have dimensions that are powers of 2.
  int texture_width = math::NextPower2(image->width());
  int texture_height = math::NextPower2(image->height());

  if (texture_right)
    *texture_right = (float)image_width / texture_width;
  if (texture_top)
    *texture_top = (float)image_height / texture_height;
  image = image->Resize(texture_width, texture_height);

  return std::make_unique<Texture>(std::move(image), mag_filter, min_filter,
                                   wrap);
}

Texture::Texture(std::unique_ptr<Bitmap> image,
                 Filter mag_filter,
                 Filter min_filter,
                 Wrap wrap)
    : min_filter_(min_filter),
      mag_filter_(mag_filter),
      wrap_(wrap),
      texture_id_(0),
      image_(std::move(image)) {}

Texture::~Texture() {
  if (texture_id_)
    glDeleteTextures(1, &texture_id_);
}

// static
unsigned int Texture::GetFilter(Filter filter) {
  switch (filter) {
    case kNearest:
      return GL_NEAREST;
    case kLinear:
      return GL_LINEAR;
    case kNearestMipmapNearest:
      return GL_NEAREST_MIPMAP_NEAREST;
    case kLinearMipmapNearest:
      return GL_LINEAR_MIPMAP_NEAREST;
    case kNearestMipmapLinear:
      return GL_NEAREST_MIPMAP_LINEAR;
    case kLinearMipmapLinear:
      return GL_LINEAR_MIPMAP_LINEAR;
  }
  return GL_LINEAR;
}

// static
unsigned int Texture::GetWrap(Wrap wrap) {
  switch (wrap) {
    case kRepeat:
      return GL_REPEAT;
    case kClampToEdge:
      return GL_CLAMP_TO_EDGE;
  }
  return GL_CLAMP_TO_EDGE;
}

// static
bool Texture::IsMipmapped(Filter filter) {
  switch (filter) {
    case kNearest:
    case kLinear:
      return false;
    case kNearestMipmapNearest:
    case kLinearMipmapNearest:
    case kNearestMipmapLinear:
    case kLinearMipmapLinear:
      return true;
  }
  return false;
}

void Texture::Bind() {
  CHECK_THREAD(thread::Render);
  if (texture_id_) {
    glBindTexture(GL_TEXTURE_2D, texture_id_);
  } else {
    DCHECK(image_);
    glGenTextures(1, &texture_id_);
    DCHECK(texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);

    GLuint mag_filter = GetFilter(mag_filter_);
    GLuint min_filter = GetFilter(min_filter_);
    GLuint wrap = GetWrap(wrap_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    DCHECK_GT(image_->width(), 0);
    DCHECK_GT(image_->height(), 0);

    GLint format;
    GLint internal_format;
    if (image_->color_planes() == 1) {
#if GL_ES_VERSION_2_0
      format = GL_LUMINANCE;
#else
      format = GL_RED;
#endif
      internal_format = format;
    } else {
      if (image_->has_alpha()) {
        format = GL_RGBA;
        internal_format = kInternalAlphaFormat;
      } else {
        format = GL_RGB;
        internal_format = kInternalFormat;
      }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image_->width(),
                 image_->height(), 0, format, GL_UNSIGNED_BYTE,
                 image_->image_data());
    if (IsMipmapped(min_filter_) || IsMipmapped(mag_filter_))
      glGenerateMipmap(GL_TEXTURE_2D);

    image_.reset();
  }
}
