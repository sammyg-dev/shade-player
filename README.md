# shade-player

A shader based desktop music visualizer

# Windows + VS Code setup

example tasks.json

```
{
  "tasks": [
    {
      "type": "cppbuild",
      "label": "C/C++: g++.exe build active file",
      "command": "C:\\mingw-w64\\mingw64\\bin\\g++.exe",
      "args": [
        "-g",
        "-Wall",
        "-I",
        "include",
        "${file}",
        "-o",
        "${fileDirname}\\${fileBasenameNoExtension}.exe",
        "-L",
        "lib",
        "-l",
        "raylib"
      ],
      "options": {
        "cwd": "${workspaceFolder}"
      },
      "problemMatcher": ["$gcc"],
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "detail": "Generated task by Debugger"
    }
  ],
  "version": "2.0.0"
}
```

example launch.json

```
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "C/C++: g++.exe build active file",
      "type": "cppdbg",
      "request": "launch",
      "program": "${fileDirname}/${fileBasenameNoExtension}.exe",
      "args": [],
      "stopAtEntry": true,
      "cwd": "${workspaceFolder}",
      "environment": [],
      "externalConsole": false,
      "MIMode": "gdb",
      "miDebuggerPath": "C:\\mingw-w64\\mingw64\\bin\\gdb.exe",
      "setupCommands": [
        {
          "description": "Enable pretty-printing for gdb",
          "text": "-enable-pretty-printing",
          "ignoreFailures": true
        }
      ],
      "preLaunchTask": "C/C++: g++.exe build active file"
    }
  ]
}
```
