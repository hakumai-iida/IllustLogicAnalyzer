bmp2hif /UC _source\system\debug_circle.bmp
move /Y _source\system\debug_circle*.hif IMAGE\.

bmp2hif /UC /D _source\system\debug_ring.bmp
move /Y _source\system\debug_ring*.hif IMAGE\.

bmp2hif /UC /0x06_80 _source\system\debug_ui.bmp
move /Y _source\system\debug_ui*.hif IMAGE\.

bmp2hif /UC _source\system\debug_font.bmp
move /Y _source\system\debug_font*.hif IMAGE\.

bmp2hif /UC _source\pal\pal_body.bmp
move /Y _source\pal\pal_body*.hif IMAGE\.

bmp2hif /UC _source\pal\pal_check.bmp
move /Y _source\pal\pal_check*.hif IMAGE\.

bmp2hif /UC _source\pal\pal_white.bmp
move /Y _source\pal\pal_white*.hif IMAGE\.

