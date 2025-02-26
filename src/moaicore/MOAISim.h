// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAISIM_H
#define	MOAISIM_H

#include <moaicore/MOAIEaseDriver.h>
#include <moaicore/MOAILua.h>

class MOAIProp2D;

//================================================================//
// MOAISim
//================================================================//
/**	@name	MOAISim
	@text	Sim timing and settings class.
	
	@const	EVENT_FINALIZE
	
	const SIM_LOOP_FORCE_STEP
	const SIM_LOOP_ALLOW_BOOST
	const SIM_LOOP_ALLOW_SPIN
	const SIM_LOOP_NO_DEFICIT
	const SIM_LOOP_NO_SURPLUS
	const SIM_LOOP_RESET_CLOCK
	
	const LOOP_FLAGS_DEFAULT
	const LOOP_FLAGS_FIXED
	const LOOP_FLAGS_MULTISTEP
	
	const DEFAULT_STEPS_PER_SECOND			Value is 60
	const DEFAULT_BOOST_THRESHOLD			Value is 3
	const DEFAULT_LONG_DELAY_THRESHOLD		Value is 10
	const DEFAULT_CPU_BUDGET				Value is 2
	const DEFAULT_STEP_MULTIPLIER			Value is 1
*/
class MOAISim :
	public MOAIGlobalClass < MOAISim, MOAIGlobalEventSource >,
	public MOAIGlobalClassFinalizer {
private:

	// timer state
	enum {
		RUNNING,
		PAUSED,
		START,
	};

	// events
	enum {
		EVENT_FINALIZE,
	};

	u32				mLoopState;

	double			mStep;			// simulation step size (in seconds)
	double			mSimTime;		// elapsed simulation running time (in seconds)
	double			mRealTime;		// time updated from system clock
	double			mFrameTime;		// time last frame time was measured (in seconds)
	
	u32				mRenderCounter;	// increments every render
	
	static const u32 FPS_BUFFER_SIZE = 30;
	float			mFrameRate;
	float			mFrameRateBuffer [ FPS_BUFFER_SIZE ];
	u32				mFrameRateIdx;

	typedef USList < MOAIProp2D* >::Iterator RenderPassIt;
	USList < MOAIProp2D* > mRenderPasses;
	
	USCallback < int > mLoadingScreenCallback;
	USCallback < int > mTextInputCallback;
	
	bool			mNeedsInit;
	STLString		mInitFromFilename;
	STLString		mInitFromString;
	
	USTaskThread	mDataIOThread;
	
	u32				mLoopFlags;
	double			mBoostThreshold;
	double			mLongDelayThreshold;
	double			mCpuBudget;
	u32				mStepMultiplier;
	double			mTimerError;
	
	//----------------------------------------------------------------//
	static int		_clearLoopFlags				( lua_State* L );
	static int		_clearRenderStack			( lua_State* L );
	static int		_enterFullscreenMode		( lua_State* L );
	static int		_exitFullscreenMode			( lua_State* L );
	static int		_forceGarbageCollection		( lua_State* L );
	static int		_framesToTime				( lua_State* L );
	static int		_getDeviceSize				( lua_State* L );
	static int		_getDeviceTime				( lua_State* L );
	static int		_getElapsedFrames			( lua_State* L );
	static int		_getElapsedTime				( lua_State* L );
	static int		_getLoopFlags				( lua_State* L );
	static int		_getLuaObjectCount			( lua_State* L );
	static int		_getMemoryUsage				( lua_State* L );
	static int		_getNetworkStatus			( lua_State* L );
	static int		_getPerformance				( lua_State* L );
	static int		_getStep					( lua_State* L );
	static int		_openWindow					( lua_State* L );
	static int		_pauseTimer					( lua_State* L );
	static int		_popRenderPass				( lua_State* L );
	static int		_pushRenderPass				( lua_State* L );
	static int		_reportHistogram			( lua_State* L );
	static int		_reportLeaks				( lua_State* L );
	static int		_setBoostThreshold			( lua_State* L );
	static int		_setCpuBudget				( lua_State* L );
	static int		_setHistogramEnabled		( lua_State* L );
	static int		_setLeakTrackingEnabled		( lua_State* L );
	static int		_setLongDelayThreshold		( lua_State* L );
	static int		_setLoopFlags				( lua_State* L );
	static int		_setLuaAllocLogEnabled		( lua_State* L );
	static int		_setStep					( lua_State* L );
	static int		_setStepMultiplier			( lua_State* L );
	static int		_setTimerError				( lua_State* L );
	static int		_timeToFrames				( lua_State* L );

	//----------------------------------------------------------------//
	double			MeasureFrameRate			();
	void			OnGlobalsFinalize			();
	void			OnGlobalsRestore			();
	void			OnGlobalsRetire				();
	double			StepSim						( double step, u32 multiplier );

public:
	
	enum {
		SIM_LOOP_FORCE_STEP			= 0x01,		// forces at least one sim step to occur on every call to update
		SIM_LOOP_ALLOW_BOOST		= 0x02,		// allow a variable time step 'boost' if sim time falls behind
		SIM_LOOP_ALLOW_SPIN			= 0x04,		// spins the update loop to use up any excess time available
		SIM_LOOP_NO_DEFICIT			= 0x08,		// sim time never falls behind real time
		SIM_LOOP_NO_SURPLUS			= 0x10,		// real time never falls behind sim time
		SIM_LOOP_LONG_DELAY			= 0x20,		// does not boost or skip in the event of a long delay
		SIM_LOOP_RESET_CLOCK		= 0x40,		// resets the time deficit then autoclears self (use after long load)
	};
	
	DECL_LUA_SINGLETON ( MOAISim )
	
	GET ( USTaskThread&, DataIOThread, mDataIOThread )
	GET ( u32, RenderCounter, mRenderCounter )
	GET ( double, Step, mStep )
	
	static const u32 LOOP_FLAGS_DEFAULT		= SIM_LOOP_ALLOW_SPIN | SIM_LOOP_LONG_DELAY;
	static const u32 LOOP_FLAGS_FIXED		= SIM_LOOP_FORCE_STEP | SIM_LOOP_NO_DEFICIT | SIM_LOOP_NO_SURPLUS;
	static const u32 LOOP_FLAGS_MULTISTEP	= SIM_LOOP_ALLOW_SPIN | SIM_LOOP_NO_SURPLUS;
	
	static const u32 DEFAULT_STEPS_PER_SECOND		= 60;	// default sim step to 60hz
	static const u32 DEFAULT_BOOST_THRESHOLD		= 3;	// sim must fall 3 steps behind before variable rate boost
	static const u32 DEFAULT_LONG_DELAY_THRESHOLD	= 10;	// sim will not try to correct for long gaps
	static const u32 DEFAULT_CPU_BUDGET				= 2;	// sim may spend up to 2 steps attempting to catch up during spin
	static const u32 DEFAULT_STEP_MULTIPLIER		= 1;
	
	//----------------------------------------------------------------//
	void			Clear						();
					MOAISim						();
					~MOAISim					();
	void			PauseMOAI					();
	void			PopRenderPass				();
	void			PushRenderPass				( MOAIProp2D* prop );
	void			RegisterLuaClass			( MOAILuaState& state );
	void			RegisterLuaFuncs			( MOAILuaState& state );
	void			Render						();
	void			ResumeMOAI					();
	void			RunFile						( cc8* filename );
	void			RunString					( cc8* script );
	void			SendFinalizeEvent			();
	void			SetStep						( double step );
	void			Update						();
};

#endif
