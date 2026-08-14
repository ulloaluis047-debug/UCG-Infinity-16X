UCG INFINITY 16X v0.1 — PROYECTO VST3

ESTADO REAL
- Motor VST3/Standalone de 16 partes con suma de capas.
- Parte 1 ROMANTIC GRAND fijada como activa.
- Arrastre de WAV/AIFF/FLAC para cargar sonido en una capa.
- Escáner recursivo de carpetas CONTENT.
- Reconocimiento binario de FXP Hypersonic (StSS) y Nexus 2 (NEXU).
- Los FXP propietarios se identifican, pero no se marcan como reproducibles sin sus muestras/motores.
- Skin funcional oscuro, cyan, violeta y verde.

COMPILAR EN WINDOWS 11
1. Instalar Visual Studio 2022 Community con "Desarrollo de escritorio con C++".
2. Clic derecho en build-windows.ps1 > Ejecutar con PowerShell.
3. El script instala CMake/Git si faltan, descarga JUCE 8 y compila VST3 x64.
4. Copiar el resultado a C:\Program Files\Common Files\VST3\
5. En FL Studio: Options > Manage plugins > Find installed plugins.

CONTENT
CONTENT\Hypersonic\Patches
CONTENT\Hypersonic\Banks
CONTENT\Nexus\Presets
CONTENT\Nexus\Samples
CONTENT\UCG Libraries
CONTENT\User Samples

NOTA
Un FXP es estado del plugin original; no contiene necesariamente sus muestras.
El escáner nunca informa "LISTO" cuando solamente existe el preset y falta el audio.
