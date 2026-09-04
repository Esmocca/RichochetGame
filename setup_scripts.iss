[Setup]
AppName=Ricochet Game
AppVersion=1.0
DefaultDirName={autopf}\RicochetGame
DefaultGroupName=Ricochet Game
UninstallDisplayIcon={app}\RicochetGame.exe
Compression=lzma2
SolidCompression=yes
; Simpan output ke folder luar agar tidak konflik
OutputDir=D:\RichochetGameDownload
OutputBaseFilename=RicochetGame_Setup_v1.0.0

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Menyalin seluruh isi proyek kecuali folder Output
Source: "D:\RichochetGame\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs; Excludes: "*.iss, *.exe_setup, *.zip"

[Icons]
Name: "{group}\Ricochet Game"; Filename: "{app}\RicochetGame.exe"
Name: "{autodesktop}\Ricochet Game"; Filename: "{app}\RicochetGame.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\RicochetGame.exe"; Description: "{cm:LaunchProgram,Ricochet Game}"; Flags: nowait postinstall skipifsilent