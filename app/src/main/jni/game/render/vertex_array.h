////
// vertex_array.h
////

#pragma once

#include "base/basic_types.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/platform.h"

#include <memory>
#include <vector>

#if OS_CHROME || OS_ANDROID || OS_IOS
#define ALLOW_CLIENT_VERTEX_BUFFERS 1
#else
#define ALLOW_CLIENT_VERTEX_BUFFERS 0
#endif

enum PrimitiveType {
  kTriangles,
  kTriangleStrip,
  kTriangleFan,
  kLines,
  kLineStrip,
  kLineLoop,
  kPoints,
};

class VertexBuffer : public base::RefCounted<VertexBuffer> {
 public:
  static void Draw(PrimitiveType type, int count);

  VertexBuffer(int attribute, int element_size);

  void Bind();
  void Unbind();

  void SetData(const float* data, size_t count, int stride);
  // Takes ownership of data.
  void SetData(std::vector<float>* data, int stride);

#if ALLOW_CLIENT_VERTEX_BUFFERS
  void SetDataPointer(const float* data, int stride);
#endif

 private:
  friend class base::RefCounted<VertexBuffer>;
  ~VertexBuffer();

  int attribute_;
  int element_size_;
  int stride_;
  uint32_t buffer_;
  const float* data_;

  DISALLOW_COPY_AND_ASSIGN(VertexBuffer);
};

class ElementBuffer : public base::RefCounted<ElementBuffer> {
 public:
  ElementBuffer(PrimitiveType primitive_type);

  void Draw();

  void SetData(const uint8_t* data, size_t count);
  void SetData(const uint16_t* data, size_t count);
  void SetData(const uint32_t* data, size_t count);

  // Takes ownership of data.
  void SetData(std::vector<uint8_t>* data);
  void SetData(std::vector<uint16_t>* data);
  void SetData(std::vector<uint32_t>* data);

#if ALLOW_CLIENT_VERTEX_BUFFERS
  void SetDataPointer(const uint8_t* data, size_t count);
  void SetDataPointer(const uint16_t* data, size_t count);
  void SetDataPointer(const uint32_t* data, size_t count);
#endif

 private:
  friend class base::RefCounted<ElementBuffer>;

  ~ElementBuffer();
  void SetDataInternal(const void* data,
                       size_t count,
                       int element_type,
                       size_t element_size);

  PrimitiveType primitive_type_;
  uint32_t buffer_;
  uint32_t element_type_;
  size_t count_;
  const void* data_;

  DISALLOW_COPY_AND_ASSIGN(ElementBuffer);
};

class VertexArray {
 public:
  VertexArray();
  ~VertexArray();

  void Bind();
  void Unbind();
  void Draw();

  void CopyBuffers(const VertexArray& other);
  void AddVertexBuffer(VertexBuffer* vertex_buffer);
  void SetElementBuffer(ElementBuffer* element_buffer);
  void SetTypeAndCount(PrimitiveType type, int count) {
    primitive_type_ = type;
    vertex_count_ = count;
  }

 private:
  std::vector<scoped_refptr<VertexBuffer>> vertex_buffers_;
  scoped_refptr<ElementBuffer> element_buffer_;

  uint32_t vertex_array_;
  PrimitiveType primitive_type_;
  int vertex_count_;

  DISALLOW_COPY_AND_ASSIGN(VertexArray);
};

class VertexArrayList {
 public:
  VertexArrayList() {}
  ~VertexArrayList() {}

  void Draw() {
    for (const auto& vertex_array : vertex_arrays_)
      vertex_array->Draw();
  }

  void AddVertexArray(std::unique_ptr<VertexArray> vertex_array) {
    vertex_arrays_.push_back(std::move(vertex_array));
  }

  VertexArray* operator[](size_t index) {
    return vertex_arrays_[index].get();
    ;
  }

 private:
  std::vector<std::unique_ptr<VertexArray>> vertex_arrays_;

  DISALLOW_COPY_AND_ASSIGN(VertexArrayList);
};
