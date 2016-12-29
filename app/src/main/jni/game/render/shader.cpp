////
// shader.cpp
////

#include "game/render/shader.h"

#include "base/file/file.h"
#include "base/file/file_manager.h"
#include "base/logging.h"
#include "base/math/matrix.h"
#include "base/thread/thread_util.h"

#ifndef NDEBUG
#define DEBUG_SHADERS
#endif

#if GL_ES_VERSION_2_0
const char kFragmentShaderHeader[] =
    "#version 100\n\
#define IN varying\n\
#define TEXTURE texture2D\n\
#define o_FragColor gl_FragColor\n\
precision mediump float;\n\
";
const char kVertexShaderHeader[] =
    "#version 100\n\
#define IN attribute\n\
#define OUT varying\n\
";
#else
const char kFragmentShaderHeader[] =
    "#version 150\n\
#define IN in\n\
#define TEXTURE texture\n\
out vec4 o_FragColor;\n\
";
const char kVertexShaderHeader[] =
    "#version 150\n\
#define IN in\n\
#define OUT out\n\
";
#endif

Shader::Shader() : program_(0), uniform_mvp_matrix_(0) {}

Shader::~Shader() {
  if (program_)
    glDeleteProgram(program_);
}

void Shader::Use() {
  CHECK_THREAD(thread::Render);
  DCHECK(program_);
  glUseProgram(program_);
}

void Shader::SetMVPMatrix(const Matrix& matrix) {
  CHECK_THREAD(thread::Render);
  glUniformMatrix4fv(uniform_mvp_matrix_, 1, GL_FALSE, matrix.value());
}

// protected:
// static
GLuint Shader::LoadShader(const char* vertex_shader,
                          const char* fragment_shader,
                          const GLuint attributes[],
                          const char* attribute_names[],
                          int attribute_count) {
  GLuint vert_shader;
  GLuint frag_shader;

  GLuint program = glCreateProgram();

  // Create and compile vertex shader
  vert_shader = CompileShader(vertex_shader, GL_VERTEX_SHADER);
  if (!vert_shader) {
    LOG(ERROR) << "Failed to compile vertex shader: " << vertex_shader;
    glDeleteProgram(program);
    return 0;
  }

  // Create and compile fragment shader
  frag_shader = CompileShader(fragment_shader, GL_FRAGMENT_SHADER);
  if (!frag_shader) {
    LOG(ERROR) << "Failed to compile fragment shader: " << fragment_shader;
    glDeleteShader(frag_shader);
    glDeleteProgram(program);
    return 0;
  }

  // Attach vertex shader to program
  glAttachShader(program, vert_shader);

  // Attach fragment shader to program
  glAttachShader(program, frag_shader);

  // Bind attribute locations.  This needs to be done prior to linking.
  for (int c = 0; c < attribute_count; c++) {
    glBindAttribLocation(program, attributes[c], attribute_names[c]);
  }

// Bind the output variable.
#if !(GL_ES_VERSION_2_0)
  glBindFragDataLocation(program, 0, "o_FragColor");
#endif

  // Link program
  if (!LinkProgram(program)) {
    LOG(ERROR) << "Failed to link program:\n"
               << vertex_shader << "\n\n"
               << fragment_shader;
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    glDeleteProgram(program);
    return 0;
  }

  // Release vertex and fragment shaders
  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  return program;
}

// static
GLuint Shader::CompileShader(const char* shader_data, GLenum type) {
  GLint status;
  GLuint shader;

  std::string contents;
  switch (type) {
    case GL_FRAGMENT_SHADER:
      contents = std::string(kFragmentShaderHeader) + shader_data;
      break;
    case GL_VERTEX_SHADER:
      contents = std::string(kVertexShaderHeader) + shader_data;
      break;
    default:
      NOTREACHED();
  }

  const GLchar* source = reinterpret_cast<const GLchar*>(contents.c_str());

  shader = glCreateShader(type);
  DCHECK_NE(shader, 0);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

#if defined(DEBUG_SHADERS)
  GLint log_length;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
  if (log_length > 0) {
    GLchar* log = (GLchar*)malloc(log_length);
    glGetShaderInfoLog(shader, log_length, &log_length, log);
    LOG(INFO) << "Shader compile log:\n" << log;
    free(log);
  }
#endif

  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == 0) {
    LOG(ERROR) << "Failed to compile shader: " << shader_data;
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

// static
bool Shader::LinkProgram(GLuint program) {
  GLint status;

  glLinkProgram(program);

#if defined(DEBUG_SHADERS)
  GLint log_length;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
  if (log_length > 0) {
    GLchar* log = (GLchar*)malloc(log_length);
    glGetProgramInfoLog(program, log_length, &log_length, log);
    LOG(INFO) << "Program link log:\n" << log;
    free(log);
  }
#endif

  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == 0)
    return false;

  return true;
}
