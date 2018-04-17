#pragma once


class Program
{
public:
	Program();
	~Program();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	/*****************************************************************//**
	 * @fn	void Program::ResizeScreen();
	 *
	 * @brief	Resize screen
	 *
	 * @author	KMK
	 * @date	2018-03-17
	*********************************************************************/

	void ResizeScreen();

private:
	struct ExecuteValues* values;//전방선언없이 사용가능하게 하는방법

	vector<class Execute *> executes;
};