Dim WshShell

Set WshShell = WScript.CreateObject("WScript.Shell")

WshShell.Run "cmd /K CD /D "+WScript.Arguments.Item(0)+"  & crc.bat "+WScript.Arguments.Item(0)+" "+WScript.Arguments.Item(1)+" "+WScript.Arguments.Item(2), 0