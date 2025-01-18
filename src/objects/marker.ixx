module;
#include <glad/glad.h>

export module marker;

import std;
import glm;

import colors;
import object;
import math;
import glutils;
import cylinder;
import shader;
import cursor;

// Function to combine translation from the first matrix and rotation from the second matrix
glm::mat4 combineTranslationAndRotation(const glm::mat4& translationMatrix, const glm::mat4& rotationMatrix)
{
    // Extract the translation vector from the first matrix
    glm::vec3 translation = glm::vec3(translationMatrix[3]);

    // Extract the rotation (upper-left 3x3) from the second matrix
    glm::mat3 rotation = glm::mat3(rotationMatrix);

    // Create a new 4x4 matrix combining translation and rotation
    glm::mat4 combinedMatrix = glm::mat4(1.0f);
    combinedMatrix = glm::mat4(rotation); // Apply rotation
    combinedMatrix[3] = glm::vec4(translation, 1.0f); // Apply translation

    return combinedMatrix;
}

export class Marker : public Object
{
public:
    Marker(const glm::vec4& color) : Object(color)
    {
    }

    void drawCursor(const Shader* shader) const
    {
        if (baseCursorAxes)
        {
            cursor.draw(shader);
        }
    }

    virtual void draw(const Shader* shader) const
    {
        auto drawWithAxisColors = [&](const glm::mat4& scale)
        {
            shader->setVector("color", cylX.getColor());
            shader->setMatrix("model", model * cylXRotation * scale);
            cylX.draw(shader);
            shader->setVector("color", cylY.getColor());
            shader->setMatrix("model", model * cylYRotation * scale);
            cylY.draw(shader);
            shader->setVector("color", cylZ.getColor());
            shader->setMatrix("model", model * cylZRotation * scale);
            cylZ.draw(shader);
        };        

        // Model set externally
        if (uniqueColors)
        {
            if (extendAxes)
            {
                drawWithAxisColors(math::translate({ 0.0, 0.1f, 0.0f }) * math::scale({ 0.7f, 1.2f, 0.7f }));
            }

            shader->setVector("color", color);
            shader->setMatrix("model", model * cylXRotation);
            cylX.draw(shader);
            shader->setMatrix("model", model * cylYRotation);
            cylY.draw(shader);
            shader->setMatrix("model", model * cylZRotation);
            cylZ.draw(shader);            
        }
        else
        {
            drawWithAxisColors(glm::mat4{ 1.0f });
        }
    }

    void setCursorModel(const glm::mat4& rotationModel)
    {
        cursor.setModel(combineTranslationAndRotation(model, rotationModel) * math::scale(0.5f));
    }

    inline static bool uniqueColors = true;
    inline static bool extendAxes = true;
    inline static bool baseCursorAxes = false;

private:
    inline static const glm::mat4 cylYRotation = math::scale(0.05f) *math::scale({ 1, 3, 1 }) * math::translate({ 0,1,0 });
    inline static const glm::mat4 cylXRotation = math::rotate(-math::pi / 2, { 0, 0, 1 }) * cylYRotation;
    inline static const glm::mat4 cylZRotation = math::rotate(math::pi / 2, { 1, 0, 0 }) * cylYRotation;

    Cylinder cylX{ colors::red };
    Cylinder cylY{ colors::blue };
    Cylinder cylZ{ colors::green };
    Cursor cursor{ 0.0f };
};