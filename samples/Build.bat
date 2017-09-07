 
rem ************************************************
rem * Script to compile the solutions of IdealOEye*
rem * Created by: zhaohu 						  *
rem * Created 2017.05.15						  *
rem ************************************************

rem set environment variables:
set _devenv="%VS100COMNTOOLS%..\..\Common7\IDE\devenv.com"

rem set compile log 
set _log="%~dp0compileResults.log" 

echo [%DATE% %Time%] Start compile sequence >%_log% 
echo Used compile configuration is %buildAnyCPU% >>%_log% 

rem Start compile************************************************ 
set _solution_file="%~dp0samples_vs2010.sln" 
%_devenv% %_solution_file% /build "Release|Win32" /Out %_log% 
if not %errorlevel% == 0 echo %_solution_file%failed! Error: %errorlevel% >>%_log% 
if %errorlevel% == 0 echo %_solution_file% compiled successful >>%_log%

rem If compile failed stop processing: 
if not %errorlevel% == 0 pause  
echo [%DATE% %Time%] Finished compile sequence >>%_log%  
pause