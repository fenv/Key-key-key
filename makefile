main: main.cpp detours/detours.lib
	CL /EHsc /Fe:bin\iamgay /I . /I detours/ /W2 /O2 /LD $** user32.lib