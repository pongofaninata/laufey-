from PIL import Image
import os

# Orden de los frames — puedes cambiar el orden aquí
FRAMES = [
    # Laufey durmiendo
    "laufeysleepsi.jpg",
    "laufeysleepsi2.jpg",
    "laufeysleepsi3.jpg",
    # Conejo corriendo
    "meimei1.jpg",
    "meimei2.jpg",
    "meimei3.jpg",
    # Laufey chiquita con alas
    "laufeyychiquita1.jpg",
    "laufeychic2.jpg",
    "launfeychic3.jpg",
]

ANCHO = 128
ALTO  = 64
UMBRAL = 128  # 0-255, más bajo = más negro en pantalla

def imagen_a_bytes(path):
    img = Image.open(path).convert("L")  # escala de grises
    img = img.resize((ANCHO, ALTO), Image.LANCZOS)
    # Invertir: fondo blanco → negro en OLED (píxel negro = apagado)
    pixels = []
    for y in range(ALTO):
        for x in range(ANCHO):
            px = img.getpixel((x, y))
            # píxel oscuro → encendido (1), fondo blanco → apagado (0)
            pixels.append(1 if px < UMBRAL else 0)
    # Empacar en bytes (8 píxeles por byte, MSB primero)
    bytes_out = []
    for i in range(0, len(pixels), 8):
        byte = 0
        for b in range(8):
            if i + b < len(pixels):
                byte |= pixels[i + b] << (7 - b)
        bytes_out.append(byte)
    return bytes_out

# Generar el archivo .h
lineas = []
lineas.append("// frames_laufey.h — generado automáticamente")
lineas.append("// 9 frames · 128x64px · OLED SSD1306")
lineas.append("")
lineas.append("#pragma once")
lineas.append("#include <avr/pgmspace.h>")
lineas.append("")

nombres = []
for i, archivo in enumerate(FRAMES):
    if not os.path.exists(archivo):
        print(f"✗ No encontrado: {archivo}")
        continue

    datos = imagen_a_bytes(archivo)
    nombre = "frame_" + archivo.replace(".jpg","").replace(".jpeg","").replace("-","_").replace(" ","_")
    nombres.append(nombre)

    lineas.append(f"// Frame {i+1}: {archivo}")
    lineas.append(f"const unsigned char {nombre}[] PROGMEM = {{")

    fila = []
    for j, byte in enumerate(datos):
        fila.append(f"0x{byte:02X}")
        if len(fila) == 16:
            lineas.append("  " + ", ".join(fila) + ",")
            fila = []
    if fila:
        lineas.append("  " + ", ".join(fila))
    lineas.append("};")
    lineas.append("")
    print(f"✓ {archivo} → {nombre}")

# Array de punteros
lineas.append(f"#define TOTAL_FRAMES {len(nombres)}")
lineas.append("")
lineas.append("const unsigned char* animacion[] = {")
for n in nombres:
    lineas.append(f"  {n},")
lineas.append("};")

with open("frames_laufey.h", "w") as f:
    f.write("\n".join(lineas))

print(f"\n✓ frames_laufey.h generado con {len(nombres)} frames")
print("Copia frames_laufey.h a tu carpeta sketch_may29a")
print("Luego cambia en el sketch: #include \"frames_pinkie.h\" → #include \"frames_laufey.h\"")
