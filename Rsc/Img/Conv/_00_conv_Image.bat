rmdir /S /Q IMAGE
mkdir IMAGE

call conv_image_all.bat

flist /Fhif /H/p IMAGE > image.lst
move /Y image.lst .\IMAGE\.

cd IMAGE
fpack /P /F image.lst
move /Y image.fpk ..\.
del image.lst

cd ..
move /Y image.fpk ..\..\..\Prj_Android\app\src\main\assets\.

pause
