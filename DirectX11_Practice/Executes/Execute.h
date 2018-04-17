#pragma once
#include "../Viewer/Camera.h"
#include "../Viewer/Perspective.h"
#include "../Viewer/Viewport.h"


struct ExecuteValues
{
	//미리정의된 클래스를 전방선언 없이 사용할 때 앞에 class를 붙이고 클래스 명을 붙이면 되는듯.

	class GlobalLightBuffer* GlobalLight;
	class ViewProjectionBuffer* ViewProjection;

	class Perspective* Perspective;
	class Viewport* Viewport;
	class Camera* MainCamera;
};

class Execute
{
public:
	Execute(ExecuteValues* exeValues)
	{
		this->values = exeValues;
	}

	virtual void Update() = 0;

	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;

	virtual void ResizeScreen() = 0;

protected:
	ExecuteValues* values;
};