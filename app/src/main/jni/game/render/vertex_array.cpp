////
// vertex_array.h
////

#include "game/render/vertex_array.h"

#include "base/logging.h"
#include "base/thread/thread_util.h"
#include "game/render/gl.h"
#include "game/render/scoped_bind.h"

namespace {

uint32_t GetGLPrimitive(PrimitiveType primitive_type) {
  switch (primitive_type) {
    case kTriangles:
      return GL_TRIANGLES;
    case kTriangleStrip:
      return GL_TRIANGLE_STRIP;
    case kTriangleFan:
      return GL_TRIANGLE_FAN;
    case kLines:
      return GL_LINES;
    case kLineStrip:
      return GL_LINE_STRIP;
    case kLineLoop:
      return GL_LINE_LOOP;
    case kPoints:
      return GL_POINTS;
  }
  NOTREACHED();
  return GL_TRIANGLES;
}
}

////
// VertexBuffer
////

// static
void VertexBuffer::Draw(PrimitiveType type, int count) {
  glDrawArrays(GetGLPrimitive(type), 0, count);
}

VertexBuffer::VertexBuffer(int attribute, int element_size)
    : attribute_(attribute),
      element_size_(element_size),
      stride_(0),
      buffer_(0),
      data_(nullptr) {}

VertexBuffer::~VertexBuffer() {
  if (buffer_)
    glDeleteBuffers(1, &buffer_);
}

void VertexBuffer::Bind() {
  CHECK_THREAD(thread::Render);
  if (buffer_) {
    DCHECK(data_ == nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_);
  }
  glEnableVertexAttribArray(attribute_);
  glVertexAttribPointer(attribute_, element_size_, GL_FLOAT, GL_FALSE, stride_,
                        data_);
}

void VertexBuffer::Unbind() {
  CHECK_THREAD(thread::Render);
  glDisableVertexAttribArray(attribute_);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(const float* data, size_t count, int stride) {
  CHECK_THREAD(thread::Render);
  DCHECK(data_ == nullptr);
  stride_ = stride;
  if (!buffer_)
    glGenBuffers(1, &buffer_);
  ScopedBind<VertexBuffer> bind(this);
  glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), data, GL_STREAM_DRAW);
}

void VertexBuffer::SetData(std::vector<float>* data, int stride) {
  SetData(&data->at(0), data->size(), stride);
}

#if ALLOW_CLIENT_VERTEX_BUFFERS
void VertexBuffer::SetDataPointer(const float* data, int stride) {
  CHECK_THREAD(thread::Render);
  DCHECK_EQ(buffer_, 0);
  stride_ = stride;
  data_ = data;
}
#endif  // ALLOW_CLIENT_VERTEX_BUFFERS

////
// ElementBuffer
////
ElementBuffer::ElementBuffer(PrimitiveType primitive_type)
    : primitive_type_(primitive_type),
      buffer_(0),
      element_type_(GL_UNSIGNED_INT),
      count_(0),
      data_(nullptr) {}

ElementBuffer::~ElementBuffer() {
  if (buffer_)
    glDeleteBuffers(1, &buffer_);
}

void ElementBuffer::Draw() {
  CHECK_THREAD(thread::Render);
  if (buffer_) {
    DCHECK(data_ == nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_);
  }
  glDrawElements(GetGLPrimitive(primitive_type_), (GLsizei)count_,
                 element_type_, data_);
  if (buffer_) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}

void ElementBuffer::SetData(const uint8_t* data, size_t count) {
  SetDataInternal(data, count, GL_UNSIGNED_BYTE, sizeof(uint8_t));
}

void ElementBuffer::SetData(const uint16_t* data, size_t count) {
  SetDataInternal(data, count, GL_UNSIGNED_SHORT, sizeof(uint16_t));
}

void ElementBuffer::SetData(const uint32_t* data, size_t count) {
  SetDataInternal(data, count, GL_UNSIGNED_INT, sizeof(uint32_t));
}

void ElementBuffer::SetData(std::vector<uint8_t>* data) {
  SetData(&data->at(0), data->size());
}

void ElementBuffer::SetData(std::vector<uint16_t>* data) {
  SetData(&data->at(0), data->size());
}

void ElementBuffer::SetData(std::vector<uint32_t>* data) {
  SetData(&data->at(0), data->size());
}

#if ALLOW_CLIENT_VERTEX_BUFFERS
void ElementBuffer::SetDataPointer(const uint8_t* data, size_t count) {
  CHECK_THREAD(thread::Render);
  DCHECK_EQ(buffer_, 0);
  element_type_ = GL_UNSIGNED_BYTE;
  count_ = count;
  data_ = data;
}

void ElementBuffer::SetDataPointer(const uint16_t* data, size_t count) {
  CHECK_THREAD(thread::Render);
  DCHECK_EQ(buffer_, 0);
  element_type_ = GL_UNSIGNED_SHORT;
  count_ = count;
  data_ = data;
}

void ElementBuffer::SetDataPointer(const uint32_t* data, size_t count) {
  CHECK_THREAD(thread::Render);
  DCHECK_EQ(buffer_, 0);
  element_type_ = GL_UNSIGNED_INT;
  count_ = count;
  data_ = data;
}
#endif  // ALLOW_CLIENT_VERTEX_BUFFERS

// private:
void ElementBuffer::SetDataInternal(const void* data,
                                    size_t count,
                                    int element_type,
                                    size_t element_size) {
  CHECK_THREAD(thread::Render);
  DCHECK(data_ == nullptr);
  element_type_ = element_type;
  count_ = count;
  if (!buffer_)
    glGenBuffers(1, &buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count_ * element_size, data,
               GL_STREAM_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

////
// VertexArray
////
VertexArray::VertexArray()
    : vertex_array_(0), primitive_type_(kTriangles), vertex_count_(0) {
  CHECK_THREAD(thread::Render);
  glGenVertexArrays(1, &vertex_array_);
}

VertexArray::~VertexArray() {
  if (vertex_array_)
    glDeleteVertexArrays(1, &vertex_array_);
}

void VertexArray::Bind() {
  if (vertex_array_) {
    glBindVertexArray(vertex_array_);
  } else {
    for (const auto& vertex_buffer : vertex_buffers_)
      vertex_buffer->Bind();
  }
}

void VertexArray::Unbind() {
  if (vertex_array_) {
    glBindVertexArray(0);
  } else {
    for (const auto& vertex_buffer : vertex_buffers_)
      vertex_buffer->Unbind();
  }
}

void VertexArray::Draw() {
  DCHECK(element_buffer_ || vertex_count_);
  ScopedBind<VertexArray> bind(this);
  if (element_buffer_)
    element_buffer_->Draw();
  else if (vertex_count_) {
    VertexBuffer::Draw(primitive_type_, vertex_count_);
  }
}

void VertexArray::CopyBuffers(const VertexArray& other) {
  primitive_type_ = other.primitive_type_;
  vertex_count_ = other.vertex_count_;
  vertex_buffers_.insert(vertex_buffers_.end(), other.vertex_buffers_.begin(),
                         other.vertex_buffers_.end());
  element_buffer_ = other.element_buffer_;

  if (vertex_array_) {
    ScopedBind<VertexArray> bind(this);
    for (const auto& vertex_buffer : vertex_buffers_)
      vertex_buffer->Bind();
  }
}

void VertexArray::AddVertexBuffer(VertexBuffer* vertex_buffer) {
  if (vertex_array_) {
    ScopedBind<VertexArray> bind(this);
    vertex_buffer->Bind();
  }
  vertex_buffers_.push_back(vertex_buffer);
}

void VertexArray::SetElementBuffer(ElementBuffer* element_buffer) {
  element_buffer_ = element_buffer;
}
