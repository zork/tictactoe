////
// basic_texture_shader.cpp
////

#include "game/render/basic_texture_shader.h"

#include "base/thread/thread_util.h"
#include "game/render/scoped_bind.h"
#include "game/render/vertex_array.h"

namespace {
// Vertex Shader
const char kVertexShader[] = R"SHADER(
uniform mat4 u_MVPMatrix;
IN vec4 a_Vertex;
IN vec2 a_TexCoord;

OUT vec2 v_TexCoord;

void main() {
  gl_Position = u_MVPMatrix * a_Vertex;
  v_TexCoord = a_TexCoord;
}
)SHADER";

// Fragment Shader
const char kFragmentShader[] = R"SHADER(
IN vec2 v_TexCoord;
uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main() {
  o_FragColor = TEXTURE(u_Texture, v_TexCoord) * u_Color;
}
)SHADER";
}

BasicTextureShader::BasicTextureShader() : uniform_color_(0) {}

BasicTextureShader::~BasicTextureShader() {}

void BasicTextureShader::SetColor(const float color[]) {
  CHECK_THREAD(thread::Render);
  glUniform4fv(uniform_color_, 1, color);
}

void BasicTextureShader::DrawTriangles(const float vertices[],
                                       size_t vertex_count,
                                       const float texture_coords[],
                                       size_t texture_count,
                                       const uint32_t faces[],
                                       size_t face_count) {
  CHECK_THREAD(thread::Render);
#if ALLOW_CLIENT_VERTEX_BUFFERS
  scoped_refptr<VertexBuffer> v_buffer(new VertexBuffer(kAttributeVertex, 3));
  scoped_refptr<VertexBuffer> t_buffer(new VertexBuffer(kAttributeTexCoord, 2));
  scoped_refptr<ElementBuffer> element_buffer(new ElementBuffer(kTriangles));

  v_buffer->SetDataPointer(vertices, 0);
  t_buffer->SetDataPointer(texture_coords, 0);
  element_buffer->SetDataPointer(faces, face_count);

  ScopedBind<VertexBuffer> v_bind(v_buffer.get());
  ScopedBind<VertexBuffer> t_bind(t_buffer.get());
  element_buffer->Draw();
#else
  std::unique_ptr<VertexArray> vertex_array(
      CreateVertexArray(vertices, vertex_count, texture_coords, texture_count,
                        faces, face_count));
  vertex_array->Draw();
#endif
}

std::unique_ptr<VertexArray> BasicTextureShader::CreateVertexArray(
    const float vertices[],
    size_t vertex_count,
    const float texture_coords[],
    size_t texture_count,
    const uint32_t faces[],
    size_t face_count) {
  CHECK_THREAD(thread::Render);
  std::unique_ptr<VertexArray> vertex_array(new VertexArray);
  ScopedBind<VertexArray> bind(vertex_array.get());

  scoped_refptr<VertexBuffer> v_buffer(new VertexBuffer(kAttributeVertex, 3));
  scoped_refptr<VertexBuffer> t_buffer(new VertexBuffer(kAttributeTexCoord, 2));
  scoped_refptr<ElementBuffer> e_buffer(new ElementBuffer(kTriangles));

  v_buffer->SetData(vertices, vertex_count, 0);
  t_buffer->SetData(texture_coords, texture_count, 0);
  e_buffer->SetData(faces, face_count);

  vertex_array->AddVertexBuffer(v_buffer.get());
  vertex_array->AddVertexBuffer(t_buffer.get());
  vertex_array->SetElementBuffer(e_buffer.get());

  return vertex_array;
}

void BasicTextureShader::Load() {
  CHECK_THREAD(thread::Render);

  // Basic Texture Shader
  const GLuint attributes[] = {
      kAttributeVertex, kAttributeTexCoord,
  };
  const char* attribute_names[] = {
      "a_Vertex", "a_TexCoord",
  };
  program_ = LoadShader(kVertexShader, kFragmentShader, attributes,
                        attribute_names, arraysize(attributes));
  uniform_mvp_matrix_ = glGetUniformLocation(program_, "u_MVPMatrix");
  uniform_color_ = glGetUniformLocation(program_, "u_Color");
}
