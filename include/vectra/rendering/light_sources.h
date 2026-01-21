#ifndef VECTRA_LIGHT_SOURCES_H
#define VECTRA_LIGHT_SOURCES_H

#include <vector>
#include <tuple>
#include <iostream>

#include <glm/glm.hpp>

#include "vectra/rendering/shader.h"

struct DirectionalLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float strength;

    DirectionalLight() : direction(-0.2f, -1.0f, -0.3f), ambient(1.0f), diffuse(1.0f), specular(1.0f), strength(0.5f)
    {

    }

    DirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float stren)
        : direction(dir), ambient(amb), diffuse(diff), specular(spec), strength(stren)
    {

    }

    DirectionalLight(glm::vec3 dir, glm::vec3 color)
        : direction(dir), ambient(color), diffuse(color), specular(color), strength(0.5f)
    {

    }

    void apply_to_shader(const Shader& shader, const std::string& uniform_name) const {
        shader.set_vec3(uniform_name + ".direction", direction);
        shader.set_vec3(uniform_name + ".ambient", ambient);
        shader.set_vec3(uniform_name + ".diffuse", diffuse);
        shader.set_vec3(uniform_name + ".specular", specular);
        shader.set_float(uniform_name + ".strength", strength);
    }

    void display_info() const
    {
        std::cout << "DirectionalLight Info:" << std::endl;
        std::cout << "  Direction: (" << direction.x << ", " << direction.y << ", " << direction.z << ")" << std::endl;
        std::cout << "  Ambient: (" << ambient.x << ", " << ambient.y << ", " << ambient.z << ")" << std::endl;
        std::cout << "  Diffuse: (" << diffuse.x << ", " << diffuse.y << ", " << diffuse.z << ")" << std::endl;
        std::cout << "  Specular: (" << specular.x << ", " << specular.y << ", " << specular.z << ")" << std::endl;
    }
};

struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    PointLight() : position(0.0f), ambient(0.05f), diffuse(0.8f), specular(1.0f), constant(1.0f), linear(0.09f), quadratic(0.032f)
    {

    }

    PointLight(glm::vec3 pos, glm::vec3 colour) : position(pos), ambient(colour * 0.1f), diffuse(colour), specular(colour), constant(1.0f), linear(0.09f), quadratic(0.032f)
    {

    }

    void set_attenuation(float distance) {
        // Table data: distance, constant, linear, quadratic
        static const std::vector<std::tuple<float, float, float, float>> attenuation_table = {
            {7.0f, 1.0f, 0.7f, 1.8f},
            {13.0f, 1.0f, 0.35f, 0.44f},
            {20.0f, 1.0f, 0.22f, 0.20f},
            {32.0f, 1.0f, 0.14f, 0.07f},
            {50.0f, 1.0f, 0.09f, 0.032f},
            {65.0f, 1.0f, 0.07f, 0.017f},
            {100.0f, 1.0f, 0.045f, 0.0075f},
            {160.0f, 1.0f, 0.027f, 0.0028f},
            {200.0f, 1.0f, 0.022f, 0.0019f},
            {325.0f, 1.0f, 0.014f, 0.0007f},
            {600.0f, 1.0f, 0.007f, 0.0002f},
            {3250.0f, 1.0f, 0.0014f, 0.000007f}
        };

        if (distance <= std::get<0>(attenuation_table[0])) {
            auto [d, c, l, q] = attenuation_table[0];
            constant = c; linear = l; quadratic = q;
            return;
        }
        if (distance >= std::get<0>(attenuation_table.back())) {
            auto [d, c, l, q] = attenuation_table.back();
            constant = c; linear = l; quadratic = q;
            return;
        }

        // Find the interval
        for (size_t i = 0; i < attenuation_table.size() - 1; ++i) {
            auto [d1, c1, l1, q1] = attenuation_table[i];
            auto [d2, c2, l2, q2] = attenuation_table[i+1];
            if (distance >= d1 && distance <= d2) {
                float t = (distance - d1) / (d2 - d1);
                constant = c1 + t * (c2 - c1);
                linear = l1 + t * (l2 - l1);
                quadratic = q1 + t * (q2 - q1);
                return;
            }
        }
    }

    void apply_to_shader(const Shader& shader, const std::string& uniform_name) const {
        shader.set_vec3(uniform_name + ".position", position);
        shader.set_vec3(uniform_name + ".ambient", ambient);
        shader.set_vec3(uniform_name + ".diffuse", diffuse);
        shader.set_vec3(uniform_name + ".specular", specular);
        shader.set_float(uniform_name + ".constant", constant);
        shader.set_float(uniform_name + ".linear", linear);
        shader.set_float(uniform_name + ".quadratic", quadratic);
    }

    void display_info() const
    {
        std::cout << "PointLight Info:" << std::endl;
        std::cout << "  Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        std::cout << "  Ambient: (" << ambient.x << ", " << ambient.y << ", " << ambient.z << ")" << std::endl;
        std::cout << "  Diffuse: (" << diffuse.x << ", " << diffuse.y << ", " << diffuse.z << ")" << std::endl;
        std::cout << "  Specular: (" << specular.x << ", " << specular.y << ", " << specular.z << ")" << std::endl;
        std::cout << "  Attenuation - Constant: " << constant << ", Linear: " << linear << ", Quadratic: " << quadratic << std::endl;
    }
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cut_off;
    float outer_cut_off;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    SpotLight() : position(0.0f), direction(0.0f, -1.0f, 0.0f), cut_off(0.9f), outer_cut_off(0.85f), ambient(0.0f), diffuse(1.0f), specular(1.0f), constant(1.0f), linear(0.09f), quadratic(0.032f)
    {

    }

    SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 colour) : position(pos), direction(dir), cut_off(0.9f), outer_cut_off(0.85f), ambient(colour * 0.1f), diffuse(colour), specular(colour), constant(1.0f), linear(0.09f), quadratic(0.032f)
    {

    }

    void set_attenuation(float distance) {
        // Table data: distance, constant, linear, quadratic
        static const std::vector<std::tuple<float, float, float, float>> attenuation_table = {
            {7.0f, 1.0f, 0.7f, 1.8f},
            {13.0f, 1.0f, 0.35f, 0.44f},
            {20.0f, 1.0f, 0.22f, 0.20f},
            {32.0f, 1.0f, 0.14f, 0.07f},
            {50.0f, 1.0f, 0.09f, 0.032f},
            {65.0f, 1.0f, 0.07f, 0.017f},
            {100.0f, 1.0f, 0.045f, 0.0075f},
            {160.0f, 1.0f, 0.027f, 0.0028f},
            {200.0f, 1.0f, 0.022f, 0.0019f},
            {325.0f, 1.0f, 0.014f, 0.0007f},
            {600.0f, 1.0f, 0.007f, 0.0002f},
            {3250.0f, 1.0f, 0.0014f, 0.000007f}
        };

        if (distance <= std::get<0>(attenuation_table[0])) {
            auto [d, c, l, q] = attenuation_table[0];
            constant = c; linear = l; quadratic = q;
            return;
        }
        if (distance >= std::get<0>(attenuation_table.back())) {
            auto [d, c, l, q] = attenuation_table.back();
            constant = c; linear = l; quadratic = q;
            return;
        }

        // Find the interval
        for (size_t i = 0; i < attenuation_table.size() - 1; ++i) {
            auto [d1, c1, l1, q1] = attenuation_table[i];
            auto [d2, c2, l2, q2] = attenuation_table[i+1];
            if (distance >= d1 && distance <= d2) {
                float t = (distance - d1) / (d2 - d1);
                constant = c1 + t * (c2 - c1);
                linear = l1 + t * (l2 - l1);
                quadratic = q1 + t * (q2 - q1);
                return;
            }
        }
    }

    void apply_to_shader(const Shader& shader, const std::string& uniform_name) const {
        shader.set_vec3(uniform_name + ".position", position);
        shader.set_vec3(uniform_name + ".direction", direction);
        shader.set_float(uniform_name + ".cut_off", cut_off);
        shader.set_float(uniform_name + ".outer_cut_off", outer_cut_off);
        shader.set_vec3(uniform_name + ".ambient", ambient);
        shader.set_vec3(uniform_name + ".diffuse", diffuse);
        shader.set_vec3(uniform_name + ".specular", specular);
        shader.set_float(uniform_name + ".constant", constant);
        shader.set_float(uniform_name + ".linear", linear);
        shader.set_float(uniform_name + ".quadratic", quadratic);
    }

    void display_info() const
    {
        std::cout << "SpotLight Info:" << std::endl;
        std::cout << "  Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        std::cout << "  Direction: (" << direction.x << ", " << direction.y << ", " << direction.z << ")" << std::endl;
        std::cout << "  Cut Off: " << cut_off << ", Outer Cut Off: " << outer_cut_off << std::endl;
        std::cout << "  Ambient: (" << ambient.x << ", " << ambient.y << ", " << ambient.z << ")" << std::endl;
        std::cout << "  Diffuse: (" << diffuse.x << ", " << diffuse.y << ", " << diffuse.z << ")" << std::endl;
        std::cout << "  Specular: (" << specular.x << ", " << specular.y << ", " << specular.z << ")" << std::endl;
        std::cout << "  Attenuation - Constant: " << constant << ", Linear: " << linear << ", Quadratic: " << quadratic << std::endl;
    }
};


#endif //VECTRA_LIGHT_SOURCES_H

