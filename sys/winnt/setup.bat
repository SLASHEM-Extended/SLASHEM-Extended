REM In case some of the long file names are MIA
if exist ..\..\dat\data.bas copy ..\..\dat\data.bas ..\..\dat\data.base
if exist ..\..\include\patchlev.h copy ..\..\include\patchlev.h ..\..\include\patchlevel.h 
if exist ..\..\doc\guideboo.txt copy ..\..\doc\guideboo.txt ..\..\doc\guidebook.txt
copy makefile.gcc ..\..\src\Makefile
copy win32api.h ..\..\include\win32api.h
