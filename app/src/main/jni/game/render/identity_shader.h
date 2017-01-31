////
// identity_shader.h
////

#pragma once

#include "base/basic_types.h"
#include "game/render/shader.h"
#include "game/render/vertex_array.h"

#include <memory>

class IdentityShader : public Shader {
 public:
  IdentityShader();
  ~IdentityShader() override;
  DISALLOW_COPY_AND_ASSIGN(IdentityShader);

  // Only called on Render thread.
  void DrawPrimitives(const float vertices[],
                      size_t vertex_count,
                      const float colors[],
                      size_t color_count,
                      const uint32_t faces[],
                      size_t face_count,
                      PrimitiveType type);

  std::unique_ptr<VertexArray> CreateVertexArray(const float vertices[],
                                                 size_t vertex_count,
                                                 const float colors[],
                                                 size_t color_count,
                                                 const uint32_t faces[],
                                                 size_t face_count,
                                                 PrimitiveType type);

  void Load() override;
};
