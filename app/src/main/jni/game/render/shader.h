////
// shader.h
////

#pragma once

#include "base/macros.h"
#include "game/render/gl.h"

enum {
  kAttributeVertex = 0,
  kAttributeColor,
  kAttributeTexCoord,
  kAttributeNormal,
  MAX_SHADER_ATTRIBUTE,
};

class Matrix;

class Shader {
 public:
  Shader();
  virtual ~Shader();
  DISALLOW_COPY_AND_ASSIGN(Shader);

  // Only called on Render thread.
  void Use();
  void SetMVPMatrix(const Matrix& matrix);
  virtual void Load() = 0;

 protected:
  static GLuint LoadShader(const char* vertex_shader,
                           const char* fragment_shader,
                           const GLuint attributes[],
                           const char* attribute_names[],
                           int attribute_count);
  static GLuint CompileShader(const char* shader_data, GLenum type);
  static bool LinkProgram(GLuint program);

  GLuint program_;
  GLuint uniform_mvp_matrix_;
};
