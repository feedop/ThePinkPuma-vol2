export module inversekinematics;

import std;
import glm;

import math;

// Signed (directed) angles between two vectors with respect to a reference normal of a surface spanned by two tangent vectors
float angleBetween(glm::vec3 v1, glm::vec3 v2, glm::vec3 tangent1, glm::vec3 tangent2)
{
    tangent1 = glm::normalize(tangent1);
    tangent2 = glm::normalize(tangent2);
    auto referenceNormal = glm::cross(tangent1, tangent2);

    v1 = glm::normalize(v1);
    v2 = glm::normalize(v2);
    float cosine = glm::dot(v1, v2);
    float angle = glm::acos(cosine);
    glm::vec3 cross = glm::cross(v1, v2);
    if (glm::dot(cross, referenceNormal) < 0)
    {
        angle = -angle;
    }

    return angle;
}

float shortestAngleDistance(float angle1, float angle2)
{
    // Normalize the difference to the range [-pi, pi]
    float difference = std::fmod(angle2 - angle1, 2.0f * math::pi);
    if (difference < -math::pi)
    {
        difference += 2.0f * math::pi;
    }
    else if (difference > math::pi)
    {
        difference -= 2.0f * math::pi;
    }

    // Return the absolute value to ensure it's unsigned
    return std::fabs(difference);
}

// Function to find the closest point on a 3D circle's edge to a given point
glm::vec3 closestPointOnCircle(const glm::vec3& center, float radius, const glm::vec3& P3)
{
    // Calculate the vector from the circle's center to P3
    glm::vec3 centerToP3 = P3 - center;

    // Normalize the vector to get the direction
    glm::vec3 direction = glm::normalize(centerToP3);

    // Scale the direction vector by the radius to find the closest point on the circle's edge
    glm::vec3 closestPoint = center + direction * radius;

    return closestPoint;
}

export std::array<float, 6> solveInverseKinematics(std::array<float, 6> q, const glm::mat4& F5, float l1, float l3, float l4)
{
    std::array<glm::mat4, 4> f; // Recalculate matrices up to f3
    f[0] = math::rotate(q[0], { 0,0,1 });
    f[1] = f[0] * math::translate({ 0,0,l1 }) * math::rotate(q[1], { 0,1,0 });
    f[2] = f[1] * math::translate({ q[2] / 2,0,0 });
    f[3] = f[2] * math::translate({ q[2] / 2,0,0 }) * math::rotate(q[3], { 0,1,0 });
    glm::vec3 oldP3 = f[3][3];

    glm::vec3 P1 = { 0.0f, 0.0f, 0.0f };

    glm::vec3 X5 = F5[0];
    glm::vec3 Y5 = F5[1];
    glm::vec3 Z5 = F5[2];
    glm::vec3 P5 = F5[3];
    
    glm::vec3 P4 = P5 - X5 * l4;
    float q0a = std::atan2f(P4.y, P4.x); // alpha_1
    float q0b = math::normalizeAngle(q0a + math::pi);

    float dist0a = shortestAngleDistance(q[0], q0a);
    float dist0b = shortestAngleDistance(q[0], q0b);

    if (std::abs(P4.x) >= math::eps || std::abs(P4.y) >= math::eps) // Special case - colinear
        q[0] = dist0a < dist0b ? q0a : q0b;

    glm::vec3 X1 = math::rotate(q[0], {0,0,1})[0]; // Rotate x axis around z axis by alpha_1
    glm::vec3 Z1 = { 0.0f, 0.0f, 1.0f };
    glm::vec3 Y1 = glm::cross(Z1, X1);
    glm::vec3 P2 = P1 + Z1 * l1;

    glm::vec3 X4 = X5;
    glm::vec3 P3toP4 = glm::normalize(glm::cross(Y1, X4));
    bool infiniteSolutions = glm::distance(Y1, X4) < math::eps;

    // Two solutions
    // First case
    glm::vec3 P3a = infiniteSolutions ? closestPointOnCircle(P4, l3, oldP3) : P4 + P3toP4 * l3;
    float q2a = glm::distance(P2, P3a); // q2
    glm::vec3 X2a = glm::normalize(P3a - P2);
    glm::vec3 Y2a = Y1;
    glm::vec3 Z2a = glm::normalize(glm::cross(X2a, Y2a));
    float q1a = angleBetween(X2a, X1, X1, Z1); // alpha_2
    glm::vec3 Z3a = glm::normalize(P4 - P3a);
    glm::vec3 Y3a = Y2a;
    glm::vec3 X3a = glm::normalize(glm::cross(Y3a, Z3a));
    float q3a = angleBetween(X3a, X2a, X2a, Z2a); // alpha_3

    // Second case
    glm::vec3 P3b = infiniteSolutions ? closestPointOnCircle(P4, l3, oldP3) : P4 - P3toP4 * l3;
    float q2b = glm::distance(P2, P3b); // q2
    glm::vec3 X2b = glm::normalize(P3b - P2);
    glm::vec3 Y2b = Y1;
    glm::vec3 Z2b = glm::normalize(glm::cross(X2b, Y2b));
    float q1b = angleBetween(X2b, X1, X1, Z1); // alpha_2
    glm::vec3 Z3b = glm::normalize(P4 - P3b);
    glm::vec3 Y3b = Y2a;
    glm::vec3 X3b = glm::normalize(glm::cross(Y3b, Z3b));
    float q3b = angleBetween(X3b, X2b, X2b, Z2b); // alpha_3

    glm::vec3 P3, X2, Y2, Z2, Z3, Y3, X3;

    float dist3a = shortestAngleDistance(q[3], q3a);
    float dist3b = shortestAngleDistance(q[3], q3b);

    // Check angle distance between the two solutions
    if (dist3a < dist3b)
    {
        q[1] = q1a;
        P3 = P3a;
        X2 = X2a;
        Y2 = Y2a;
        Z2 = Z2a;
        q[2] = q2a;
        X3 = X3a;
        Y3 = Y3a;
        Z3 = Z3a;
        q[3] = q3a;
    }
    else
    {
        q[1] = q1b;
        P3 = P3b;
        X2 = X2b;
        Y2 = Y2b;
        Z2 = Z2b;
        q[2] = q2b;
        X3 = X3b;
        Y3 = Y3b;
        Z3 = Z3b;
        q[3] = q3b;
    }

    
    glm::vec3 Z4 = Z3;
    glm::vec3 Y4 = glm::normalize(glm::cross(Z4, X4));
    q[4] = angleBetween(X4, X3, Y3, X3); // alpha_4

    q[5] = angleBetween(Z5, Z4, Z4, Y4); // alpha_5

    return q;
}