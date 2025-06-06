#include <shader.hpp>

#include <fstream>

namespace zsl
{
namespace shader
{
    shader::shader(const std::string& vertpath, const std::string& fragpath, bool load_spirv)
    {
        GLuint vertex_sid = load_shader(GL_VERTEX_SHADER, vertpath, load_spirv);
        GLuint fragment_sid = load_shader(GL_FRAGMENT_SHADER, fragpath, load_spirv);

        m_program = glCreateProgram();
        glAttachShader(m_program, vertex_sid);
        glAttachShader(m_program, fragment_sid);
        glLinkProgram(m_program);

        int status = 0;
        glGetProgramiv(m_program, GL_LINK_STATUS, &status);
        if (!status) {
            throw std::runtime_error("Linking shaders failed\n");
        }

        glDeleteShader(vertex_sid);
        glDeleteShader(fragment_sid);
    }

    shader::shader(const std::string& comppath, bool load_spirv)
    {
        GLuint compute_sid = load_shader(GL_COMPUTE_SHADER, comppath, load_spirv);

        m_program = glCreateProgram();
        glAttachShader(m_program, compute_sid);
        glLinkProgram(m_program);

        int status = 0;
        glGetProgramiv(m_program, GL_LINK_STATUS, &status);
        if (!status) {
            throw std::runtime_error("Linking compute shader failed\n");
        }

        glDeleteShader(compute_sid);
    }

    shader::~shader()
    {
        glDeleteProgram(m_program);
    }

    GLuint load_shader(GLenum shader_type, const std::string& shader_path, bool load_spirv)
    {
        std::string effective_path = shader_path + (load_spirv ? ".spv" : "");
        std::vector<u8> shader_buf = read_file(effective_path);
        GLuint shader_id = glCreateShader(shader_type);
        if (load_spirv) {
            glShaderBinary(1, &shader_id, GL_SHADER_BINARY_FORMAT_SPIR_V, shader_buf.data(), shader_buf.size());
            glSpecializeShader(shader_id, "main", 0, 0, 0);
        }
        else {
            shader_buf.push_back('\0');
            const char* ptr_holder = reinterpret_cast<const char*>(shader_buf.data());
            glShaderSource(shader_id, 1, &ptr_holder, NULL);
            glCompileShader(shader_id);
        }

        int status = 0;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
        if (!status) {
            throw std::runtime_error("Compiling shader " + effective_path + " failed");
        }

        return shader_id;
    }

    void bind(shader& program)
    {
        glUseProgram(program.m_program);
    }

    void update_vec2(shader& program, GLint location, const glm::vec2& vec_)
    {
        glProgramUniform2f(program.m_program, location, vec_.x, vec_.y);
    }

    void update_int_array(shader& program, GLint location, GLint* intv, usz size)
    {
        glProgramUniform1iv(program.m_program, location, size, intv);
    }

    void update_int(shader& program, GLint location, GLint int_)
    {
        glProgramUniform1i(program.m_program, location, int_);
    }

    void update_int(shader& program, GLint location, bool bool_)
    {
        glProgramUniform1i(program.m_program, location, bool_);
    }

    void update_float(shader& program, GLint location, float float_)
    {
        glProgramUniform1f(program.m_program, location, float_);
    }

    std::vector<u8> read_file(const std::string& filepath)
    {
        std::ifstream in_file(filepath, std::ios::ate | std::ios::binary);
        if (!in_file.is_open()) { throw std::runtime_error("Failed to open file!"); }
        usz filesize = in_file.tellg();
        std::vector<u8> buffer(filesize);
        in_file.seekg(0);
        in_file.read(reinterpret_cast<char*>(buffer.data()), filesize);
        in_file.close();
        return buffer;
    }
}
}