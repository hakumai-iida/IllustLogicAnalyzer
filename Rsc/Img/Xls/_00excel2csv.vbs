Dim objXLS
Dim file
Dim mess
Dim ws

If WScript.Arguments.Count<1 Then
   mess = "Excel�t�@�C�����h���b�v����Ɓ@�@" & Chr(10)
   mess = mess & "���[�N�V�[�g����csv�t�@�C�����o�͂��܂��@�@" & Chr(10) & Chr(10)
   mess = mess & "�i�������t�@�C������x�Ƀh���b�v���邱�Ƃ��ł��܂��j�@" & Chr(10) & Chr(10)
   mess = mess & "�i��MS Excel���C���X�g�[������Ă���K�v������܂��j�@" & Chr(10)
   MsgBox mess
   WScript.Quit
End If

Set objXLS=CreateObject("Excel.Application")
'objXLS.Visible=True
objXLS.DisplayAlerts=False

For Each file In WScript.Arguments
   objXLS.WorkBooks.Open file
   For Each ws In objXLS.ActiveWorkBook.Worksheets
      objXLS.ActiveWorkBook.Worksheets( ws.Name ).Activate
      objXLS.ActiveWorkBook.ActiveSheet.SaveAs objXLS.ActiveWorkBook.path & "\" & ws.Name & ".csv", 6
   Next
   objXLS.ActiveWorkBook.Close
Next

objXLS.Quit
WScript.Quit

