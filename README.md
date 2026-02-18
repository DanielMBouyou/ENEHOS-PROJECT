# ENEHOS Project

Architecture de base du projet ENEHOS:
- `firmware/gnss-box`: XIAO nRF52840 + L86 (UART) + BLE
- `firmware/imu-box`: XIAO nRF52840 + BNO085 (I2C) + BLE
- `firmware/shared`: structures et utilitaires communs
- `mobile/app`: application smartphone
- `docs/`: schema, protocole, rapport
- `tools/`: scripts et captures/analyse

## Structure

```text
.
|- firmware/
|  |- gnss-box/
|  |- imu-box/
|  \- shared/
|- mobile/
|  \- app/
|- docs/
|  |- wiring/
|  |- protocol/
|  \- report/
|- tools/
|  |- scripts/
|  \- sniff/
\- .vscode/
```

## Demarrage rapide (Windows + PowerShell)

```powershell
git clone https://github.com/DanielMBouyou/ENEHOS-PROJECT.git
cd ENEHOS-PROJECT
code .
```

## VS Code recommande

- PlatformIO IDE
- C/C++ (Microsoft)
- VS Code Serial Monitor
- GitLens
- EditorConfig for VS Code

