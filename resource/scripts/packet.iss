; ---------------------------------------------------------
; YOLO 工具安装包 - Inno Setup 脚本
; ---------------------------------------------------------
[Setup]
AppName=YOLO Training & Convert Tool
AppVersion=1.0.0
AppPublisher=Lucas Chang
DefaultDirName={autopf}\YOLO Tool
OutputBaseFilename=YOLO_Tool_Setup
SetupIconFile=E:\code\qt\yolo\resource\naiLong.ico
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64compatible
DefaultGroupName=YOLO Tool
DisableProgramGroupPage=yes
PrivilegesRequired=admin
UninstallDisplayIcon={app}\naiLong.ico

[Files]
Source: "E:\code\qt\yolo\bin\Project.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\code\qt\yolo\bin\*.dll"; DestDir: "{app}"; Flags: ignoreversion

; Qt platforms 插件
Source: "E:\code\qt\yolo\bin\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs

; Qt styles 插件（重要！）
Source: "E:\code\qt\yolo\bin\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs createallsubdirs

; Qt imageformats 插件
Source: "E:\code\qt\yolo\bin\imageformats\*"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs

; 文档
Source: "E:\code\qt\yolo\resource\doc\yolo.md"; DestDir: "{app}"; Flags: ignoreversion

; 脚本
Source: "E:\code\qt\yolo\resource\scripts\test_pt.py"; DestDir: "{app}"; Flags: ignoreversion
; 脚本
Source: "E:\code\qt\yolo\resource\scripts\test_onnx.py"; DestDir: "{app}"; Flags: ignoreversion

; 图标
Source: "E:\code\qt\yolo\resource\naiLong.ico"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\奶龙 "; Filename: "{app}\Project.exe"; IconFilename: "{app}\naiLong.ico"
Name: "{userdesktop}\奶龙 "; Filename: "{app}\Project.exe"; IconFilename: "{app}\naiLong.ico"; Tasks: desktopicon
Name: "{group}\{cm:UninstallProgram,奶龙 }"; Filename: "{uninstallexe}"

[Tasks]
Name: "desktopicon"; Description: "创建桌面快捷方式"; GroupDescription: "附加图标:"; Flags: unchecked

[Run]
Filename: "{app}\Project.exe"; Description: "立即运行 YOLO 工具"; Flags: nowait postinstall skipifsilent