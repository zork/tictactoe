////
// basic_texture_shader.h
////

#pragma once

#include "base/basic_types.h"
#include "game/render/shader.h"

#include <memory>

class VertexArray;

class BasicTextureShader : public Shader {
 public:
  BasicTextureShader();
  ~BasicTextureShader() override;

  // Only called on Render thread.
  void SetColor(const float color[]);
  void DrawTriangles(const float vertices[],
                     size_t vertex_count,
                     const float texture_coords[],
                     size_t texture_count,
                     const uint32_t faces[],
                     size_t face_count);

  std::unique_ptr<VertexArray> CreateVertexArray(const float vertices[],
                                                 size_t vertex_count,
                                                 const float texture_coords[],
                                                 size_t texture_count,
                                                 const uint32_t faces[],
                                                 size_t face_count);

  void Load() override;

 private:
  GLuint uniform_color_;

  DISALLOW_COPY_AND_ASSIGN(BasicTextureShader);
};
