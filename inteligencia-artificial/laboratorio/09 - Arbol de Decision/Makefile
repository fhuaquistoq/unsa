SHELL := cmd
.SHELLFLAGS := /C

VSWHERE := C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe
VCVARS_ARCH := x64
CLFLAGS := /nologo /std:c++17 /EHsc /O2 /W4
TARGET := program.exe
SRC := main.cpp
OBJ := main.obj

all: $(TARGET)

$(TARGET): $(SRC)
	@powershell -NoProfile -ExecutionPolicy Bypass -Command "$$vswhere = '$(VSWHERE)'; if (-not (Test-Path -LiteralPath $$vswhere)) { throw 'vswhere.exe not found at $(VSWHERE)' }; $$install = & $$vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath; if (-not $$install) { throw 'Visual Studio with C++ tools not found' }; $$vcvars = Join-Path $$install 'VC\Auxiliary\Build\vcvarsall.bat'; $$cmd = 'call \"' + $$vcvars + '\" $(VCVARS_ARCH) >nul && cl $(CLFLAGS) /Fe:$(TARGET) $(SRC)'; cmd /c $$cmd; if ($$LASTEXITCODE -ne 0) { exit $$LASTEXITCODE }"
	@if not exist "$(TARGET)" (echo Build failed: $(TARGET) was not produced. && exit /b 1)

run: $(TARGET)
	@"$(TARGET)" < clientes.dat

clean:
	@if exist "$(TARGET)" del /Q "$(TARGET)"
	@if exist "$(OBJ)" del /Q "$(OBJ)"

.PHONY: all run clean
