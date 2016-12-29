////
// identity_shader.cpp
////

#include "game/render/identity_shader.h"

#include "base/thread/thread_util.h"
#include "game/render/scoped_bind.h"
#include "game/render/vertex_array.h"

namespace {
// Vertex Shader
const char kVertexShader[] = R"SHADER(
uniform mat4 u_MVPMatrix;
IN vec4 a_Vertex;
IN vec4 a_Color;

OUT vec4 v_Color;

void main() {
  gl_PointSize = 1.0;
  gl_Position = u_MVPMatrix * a_Vertex;
  v_Color = a_Color;
}
)SHADER";

// Fragment Shader
const char kFragmentShader[] = R"SHADER(
IN vec4 v_Color;

void main() {
  o_FragColor = v_Color;
}
)SHADER";
}

IdentityShader::IdentityShader() {}

IdentityShader::~IdentityShader() {}

void IdentityShader::DrawTriangles(const float vertices[],
                                   size_t vertex_count,
                                   const float colors[],
                                   size_t color_count,
                                   const uint32_t faces[],
                                   size_t face_count) {
  CHECK_THREAD(thread::Render);
#if ALLOW_CLIENT_VERTEX_BUFFERS
  scoped_refptr<VertexBuffer> v_buffer(new VertexBuffer(kAttributeVertex, 3));
  scoped_refptr<VertexBuffer> c_buffer(new VertexBuffer(kAttributeColor, 4));
  scoped_refptr<ElementBuffer> e_buffer(new ElementBuffer(kTriangles));

  v_buffer->SetDataPointer(vertices, 0);
  c_buffer->SetDataPointer(colors, 0);
  e_buffer->SetDataPointer(faces, face_count);

  ScopedBind<VertexBuffer> v_bind(v_buffer.get());
  ScopedBind<VertexBuffer> c_bind(c_buffer.get());
  e_buffer->Draw();
#else
  std::unique_ptr<VertexArray> vertex_array(CreateVertexArray(
      vertices, vertex_count, colors, color_count, faces, face_count));
  vertex_array->Draw();
#endif
}

std::unique_ptr<VertexArray> IdentityShader::CreateVertexArray(
    const float vertices[],
    size_t vertex_count,
    const float colors[],
    size_t color_count,
    const uint32_t faces[],
    size_t face_count) {
  CHECK_THREAD(thread::Render);
  std::unique_ptr<VertexArray> vertex_array(new VertexArray);
  ScopedBind<VertexArray> bind(vertex_array.get());

  scoped_refptr<VertexBuffer> v_buffer(new VertexBuffer(kAttributeVertex, 3));
  scoped_refptr<VertexBuffer> c_buffer(new VertexBuffer(kAttributeColor, 4));
  scoped_refptr<ElementBuffer> e_buffer(new ElementBuffer(kTriangles));

  v_buffer->SetData(vertices, vertex_count, 0);
  c_buffer->SetData(colors, color_count, 0);
  e_buffer->SetData(faces, face_count);

  vertex_array->AddVertexBuffer(v_buffer.get());
  vertex_array->AddVertexBuffer(c_buffer.get());
  vertex_array->SetElementBuffer(e_buffer.get());

  return vertex_array;
}

void IdentityShader::Load() {
  CHECK_THREAD(thread::Render);

  // Identity Shader
  const GLuint attributes[] = {
      kAttributeVertex, kAttributeColor,
  };
  const char* attribute_names[] = {
      "a_Vertex", "a_Color",
  };
  program_ = LoadShader(kVertexShader, kFragmentShader, attributes,
                        attribute_names, arraysize(attributes));
  uniform_mvp_matrix_ = glGetUniformLocation(program_, "u_MVPMatrix");
}
