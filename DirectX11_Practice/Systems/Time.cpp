#include "stdafx.h"
#include "Time.h"

Time* Time::instance = NULL;

bool Time::isTimerStopped = true;
float Time::timeElapsed = 0.0f;

Time::Time(void) :
	ticksPerSecond(0), currentTime(0), lastTime(0), lastFPSUpdate(0), fpsUpdateInterval(0),
	frameCount(0), runningTime(0), framePerSecond(0)
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond);
	//ticksPerSecond를 직접가지고 있는것 보다 첫 실행시 1/tickPerSecond로 주파수의 초를 구한후 곱해주는게 좋을듯.. 나눗셈연산은 곱하기보다 연산 소모가 크므로.정확도 문제는 모르겠음
	
	
	fpsUpdateInterval = ticksPerSecond >> 1;
	//절반값? 0.5초로 인터벌(갱신간격)을 설정?

	/*TwBar* bar = TweakBar::Get()->GetBar();
	TwAddVarRO(bar, "Time", TW_TYPE_FLOAT, &framePerSecond, "");*/
}

Time::~Time(void)
{

}

Time* Time::Get()
{
	assert(instance != NULL);
	
	return instance;
}

void Time::Create()
{
	assert(instance == NULL);

	instance = new Time();
}

void Time::Delete()
{
	SAFE_DELETE(instance);
}

void Time::Update()
{
	if (isTimerStopped) return;

	//1. 현재시간을 가져와 시간 간격 및 진행 시간을 계산한다.
	QueryPerformanceCounter((LARGE_INTEGER *)&currentTime);
	timeElapsed = (float)(currentTime - lastTime) / (float)ticksPerSecond;

	//음수가 되지 않게 한다.SDK 문서화의 CDXUTTimer 항목에 따르면,
	//프로세서가 절전 모드로 들어가거나 다른 프로세서와 엉키는 경우
	//mDeltaTime 이 음수가 될 수 있다.
	/*if(timeElapsed < 0.0f)
	{
		timeElapsed = 0;
	}*/

	runningTime += timeElapsed;


	//2. FPS Update
	//3. fpsUpdateInterval은 너무 자주 갱신하지 않기 위해 쓰는 듯 하다.
	frameCount++;
	if (currentTime - lastFPSUpdate >= fpsUpdateInterval)
	{
		float tempCurrentTime = (float)currentTime / (float)ticksPerSecond;
		float tempLastTime = (float)lastFPSUpdate / (float)ticksPerSecond;
		framePerSecond = (float)frameCount / (tempCurrentTime - tempLastTime);

		lastFPSUpdate = (INT64)currentTime;
		frameCount = 0;
	}

	lastTime = currentTime;
}

void Time::Print()
{

}

void Time::Start()
{
	if (!isTimerStopped)
		assert(false);

	QueryPerformanceCounter((LARGE_INTEGER *)&lastTime);
	isTimerStopped = false;
}

void Time::Stop()
{
	if (isTimerStopped)
		assert(false);

	INT64 stopTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&stopTime);
	runningTime += (float)(stopTime - lastTime) / (float)ticksPerSecond;
	isTimerStopped = true;
}