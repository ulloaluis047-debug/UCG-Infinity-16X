#define AppName "UCG Infinity 16X"
#define AppVersion "0.1.0"
#define Publisher "UCG Corp"

[Setup]
AppId={{46C1E219-64E3-4B87-BFA9-5BB745AC1616}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#Publisher}
DefaultDirName={commoncf64}\VST3
DisableDirPage=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir=Output
OutputBaseFilename=UCG-Infinity-16X-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin
Uninstallable=yes

[Files]
Source: "..\dist\UCG Infinity 16X.vst3\*"; DestDir: "{commoncf64}\VST3\UCG Infinity 16X.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\UCG Infinity 16X"; Filename: "{commoncf64}\VST3\UCG Infinity 16X.vst3"

[Run]
Filename: "explorer.exe"; Parameters: "{commoncf64}\VST3"; Description: "Abrir carpeta VST3"; Flags: postinstall nowait skipifsilent

