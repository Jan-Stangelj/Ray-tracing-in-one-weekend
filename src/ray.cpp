#include "ray.hpp"

namespace rt {

    ray::ray(glm::vec3 origin, glm::vec3 direction) : m_origin(origin), m_direction(direction) {}

    glm::vec3 ray::origin() const {
        return m_origin;
    }

    glm::vec3 ray::direction() const {
        return m_direction;
    }

    glm::vec3 ray::at(float distance) const {
        return m_origin + distance*m_direction;
    }

}