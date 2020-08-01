:----------------------------------------
: shader
:----------------------------------------

: パック
flist /Ftxt src > shader.lst
move /Y shader.lst src

cd src
fpack /P /F shader.lst

: 移動
move /Y shader.fpk ..\..\..\Prj_Android\app\src\main\assets

: ゴミの削除
del *.lst
pause
