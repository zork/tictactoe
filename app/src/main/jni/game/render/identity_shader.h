////
// identity_shader.h
////

#pragma once

#include "base/basic_types.h"
#include "game/render/shader.h"

#include <memory>

class VertexArray;

class IdentityShader : public Shader {
 public:
  IdentityShader();
  ~IdentityShader() override;

  // Only called on Render thread.
  void DrawTriangles(const float vertices[],
                     size_t vertex_count,
                     const float colors[],
                     size_t color_count,
                     const uint32_t faces[],
                     size_t face_count);

  std::unique_ptr<VertexArray> CreateVertexArray(const float vertices[],
                                                 size_t vertex_count,
                                                 const float colors[],
                                                 size_t color_count,
                                                 const uint32_t faces[],
                                                 size_t face_count);

  void Load() override;

 private:
  DISALLOW_COPY_AND_ASSIGN(IdentityShader);
};
