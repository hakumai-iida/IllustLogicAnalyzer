Dim objXLS
Dim file
Dim mess
Dim ws

If WScript.Arguments.Count<1 Then
   mess = "Excelファイルをドロップすると　　" & Chr(10)
   mess = mess & "ワークシート毎にcsvファイルを出力します　　" & Chr(10) & Chr(10)
   mess = mess & "（※複数ファイルを一度にドロップすることもできます）　" & Chr(10) & Chr(10)
   mess = mess & "（※MS Excelがインストールされている必要があります）　" & Chr(10)
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

