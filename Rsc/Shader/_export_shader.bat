:----------------------------------------
: shader
:----------------------------------------

: �p�b�N
flist /Ftxt src > shader.lst
move /Y shader.lst src

cd src
fpack /P /F shader.lst

: �ړ�
move /Y shader.fpk ..\..\..\Prj_Android\app\src\main\assets

: �S�~�̍폜
del *.lst
pause
