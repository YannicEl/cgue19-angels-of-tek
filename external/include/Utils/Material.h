/*
* Copyright 2018 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"

/*!
 * Base material
 */
class Material
{
protected:
	/*!
	 * The shader used for rendering this material
	 */
	std::shared_ptr<Shader> _shader;
	/*!
	 * The material's color
	 */
	glm::vec3 _color;
	/*!
	 * The material's coefficients (x = ambient, y = diffuse, z = specular)
	 */
	glm::vec3 _materialCoefficients;

	/*!
	 * Alpha value, i.e. the shininess constant
	 */
	float _alpha;

public:
	/*!
	 * Base material constructor
	 * @param shader: The shader used for rendering this material
	 * @param color: The material's color
	 * @param materialCoefficients: The material's coefficients (x = ambient, y = diffuse, z = specular)
	 * @param alpha: Alpha value, i.e. the shininess constant
	 */
	Material(std::shared_ptr<Shader> shader, glm::vec3 color, glm::vec3 materialCoefficients, float alpha);

	virtual ~Material();

	/*!
	 * @return The shader associated with this material
	 */
	Shader* getShader();

	/*!
	 * Sets this material's parameters as uniforms in the shader
	 */
	virtual void setUniforms();
};

/* --------------------------------------------- */
// Base material
/* --------------------------------------------- */

Material::Material(std::shared_ptr<Shader> shader, glm::vec3 color, glm::vec3 materialCoefficients, float alpha)
	: _shader(shader), _color(color), _materialCoefficients(materialCoefficients), _alpha(alpha)
{
}

Material::~Material()
{
}

Shader* Material::getShader()
{
	return _shader.get();
}

void Material::setUniforms()
{
	_shader->setVec3("diffuseColor", _color);
	_shader->setVec3("materialCoefficients", _materialCoefficients);
	_shader->setFloat("specularAlpha", _alpha);
}
