@echo off

set shortcutPath=%__CD__%
set currentFolder=%shortcutPath:~0,-1%
for %%A in (%currentFolder%) do set folderName=%%~nxA

set /P className=Introduce the name of the class: 

(
	echo #pragma once
	echo.
	echo namespace %folderName%
    echo {
	echo.	class %className%
	echo.	{
	echo.		
	echo.	};
	echo }
) > %currentFolder%\%folderName%\%className%.h

(
	echo #include ^"%className%.h^"
	echo.
) > %currentFolder%\%folderName%\%className%.cpp