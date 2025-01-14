#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct Camera
{
public:
	Camera()
	{
	}

	void initCamera(const glm::vec3& pos, float fov, float aspect, float nearClip, float farClip)
	{
		this->pos = pos;
		this->forward = glm::normalize(target - pos);
		this->up = glm::vec3(0.0f, 1.0f, 0.0f);
		this->projection = glm::perspective(fov, aspect, nearClip, farClip);
	}

	void setPos(glm::vec3 pos)
	{
		this->pos = pos;
	}

	glm::vec3 getPos()
	{
		return this->pos;
	}

	void setLook()
	{
		forward = glm::vec3(glm::normalize(target - pos));
	}

	inline glm::mat4 getViewProjection() const
	{
		return projection * glm::lookAt(pos, target, up);
	}

	inline glm::mat4 getProjection() const
	{
		return projection;
	}

	inline glm::mat4 getView() const
	{
		return glm::lookAt(pos, target, up);
	}

	void MoveForward(float amt)
	{
		pos += forward * amt;
	}

	void MoveRight(float amt)
	{
		pos += glm::cross(up, forward) * amt;
	}

	void Pitch(float angle)
	{
		glm::vec3 right = glm::normalize(glm::cross(up, forward));

		forward = glm::vec3(glm::normalize(glm::rotate(angle, right) * glm::vec4(forward, 0.0)));
		up = glm::normalize(glm::cross(forward, right));
	}

	void RotateY(float angle)
	{
		static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

		glm::mat4 rotation = glm::rotate(angle, UP);

		forward = glm::vec3(glm::normalize(rotation * glm::vec4(forward, 0.0)));
		up = glm::vec3(glm::normalize(rotation * glm::vec4(up, 0.0)));
	}

    void Orbit(float speed, float distance)
    {
        glm::vec3 direction = glm::normalize(pos - target);
        glm::vec3 right = glm::normalize(glm::cross(up, direction));
        glm::mat4 rotation = glm::rotate(speed, up);
        direction = glm::vec3(rotation * glm::vec4(direction, 0.0));
        pos = target + direction * distance;
        forward = glm::normalize(target - pos);
    }

	glm::vec3 target;

private:
	glm::mat4 projection;
	glm::vec3 pos;
	glm::vec3 forward;
	glm::vec3 up;
};
