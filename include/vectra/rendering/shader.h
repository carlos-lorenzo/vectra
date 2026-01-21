//
// Created by carlos on 9/29/25.
//

#ifndef VECTRA_SHADER_H
#define VECTRA_SHADER_H

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    unsigned int ID;
    Shader();
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    void use() const;
    void delete_program() const;
    void set_bool(const std::string &name, bool value) const;
    void set_int(const std::string &name, int value) const;
    void set_float(const std::string &name, float value) const;
    void set_vec2(const std::string& name, float x, float y) const;
    void set_vec3(const std::string& name, float x, float y, float z) const;
    void set_vec4(const std::string& name, float x, float y, float z, float w) const;
    void set_vec2(const std::string& name, const glm::vec2& vec) const;
    void set_vec3(const std::string& name, const glm::vec3& vec) const;
    void set_vec4(const std::string& name, const glm::vec4& vec) const;
    void set_mat2(const std::string &name, const glm::mat2& mat) const;
    void set_mat3(const std::string &name, const glm::mat3& mat) const;
    void set_mat4(const std::string &name, const glm::mat4& mat) const;

private:
    auto check_compile_errors(GLuint shader, std::string type) -> void;
};
#endif //VECTRA_SHADER_H