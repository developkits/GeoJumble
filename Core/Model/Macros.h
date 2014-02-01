#pragma once

// Convenience macro for releasing a COM object
#define ReleaseMacro(x) { if(x){ x->Release(); x = 0; } }

// Macro for popping up a text box based
// on a failed HRESULT and then quitting (only in debug builds)
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)											\
	{														\
		HRESULT hr = (x);									\
		if(FAILED(hr))										\
		{													\
			MessageBox(NULL, L#x, L"DirectX Error", MB_OK);	\
			PostQuitMessage(0);								\
		}													\
	}														
	#endif
#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif