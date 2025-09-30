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
    Shader(const char* vertexPath, const char* fragmentPath);
    void use() const;
    void delete_program() const;
    void set_bool(const std::string &name, bool value) const;
    void set_int(const std::string &name, int value) const;
    void set_float(const std::string &name, float value) const;
    void set_vec3(const std::string& name, float x, float y, float z) const;
    void set_vec4(const std::string& name, float x, float y, float z, float w) const;
    void set_mat4(const std::string &name, glm::mat4) const;

private:
    static void check_compile_errors(unsigned int shader, const std::string& type);
};
#endif //VECTRA_SHADER_H