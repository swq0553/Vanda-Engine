 /*Author   :   Dave Astle
 Date     :   11/22/2000

 Written for OpenGL Game Programming
 */
#include "stdafx.h"
#include "Timer.h"

/*****************************************************************************
 Init()

 If the hi-res timer is present, the tick rate is stored and the function
 returns CTrue. Otherwise, the function returns CFalse, and the timer should
 not be used.
*****************************************************************************/

CTimer::CTimer()
{
}
CTimer::~CTimer()
{
}

CBool CTimer::Init()
{
	if ( !QueryPerformanceFrequency( (LARGE_INTEGER*)&m_ticksPerSecond ) )
	{
		// system doesn't support hi-res timer
		return CFalse;
	}
	else
	{
		QueryPerformanceCounter( (LARGE_INTEGER*)&m_startTime );
		return CTrue;
	}
} // end Init()


CFloat CTimer::GetElapsedSeconds( CBool startFromBeginning )
{
  static __int64 s_lastTime = m_startTime;
  __int64 currentTime;
  if( startFromBeginning )
  {
	  QueryPerformanceCounter( (LARGE_INTEGER*)&m_startTime );
	  s_lastTime = m_startTime;
  }


  QueryPerformanceCounter( (LARGE_INTEGER*)&currentTime );

  CFloat seconds = ( ( CFloat )( currentTime-s_lastTime ) / ( CFloat)m_ticksPerSecond );
  // reset the timer
  s_lastTime = currentTime;

  return seconds;
} // end GetElapsedSeconds()


/***************************************************************************
 GetFPS()

 Returns the average frames per second over elapsedFrames, which defaults to
 one. If this is not called every frame, the client should track the number
 of frames itself, and reset the value after this is called.
***************************************************************************/
CFloat CTimer::GetFps( CULong elapsedFrames, CBool reset )
{
  __int64 currentTime;
  static __int64 startTime;
  if(reset)
	  QueryPerformanceCounter((LARGE_INTEGER*)&startTime); 

  QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

  CFloat fps = ( CFloat )elapsedFrames * ( CFloat )m_ticksPerSecond / ( ( CFloat )currentTime - ( CFloat )startTime );

  return fps;
} // end GetFPS


/***************************************************************************
 LockFPS()

 Used to lock the frame rate to a set amount. This will block until enough
 time has passed to ensure that the fps won't go over the requested amount.
 Note that this can only keep the fps from going above the specified level;
 it can still drop below it. It is assumed that if used, this function will
 be called every frame. The value returned is the instantaneous fps, which
 will be <= targetFPS.
***************************************************************************/
CFloat CTimer::LockFps( CULong targetFps )
{
  if (targetFps == 0) 
    targetFps = 1;

  static __int64 s_lastTime = m_startTime;
  __int64 currentTime;
  CFloat fps;

  // delay to maintain a constant frame rate
  do {
    QueryPerformanceCounter( (LARGE_INTEGER*)&currentTime );
    fps = ( CFloat )m_ticksPerSecond / ( ( CFloat )( currentTime - s_lastTime ) );
  } while ( fps > ( CFloat )targetFps );

  // reset the timer
  s_lastTime = m_startTime;

  return fps;
} // end LockFPS()