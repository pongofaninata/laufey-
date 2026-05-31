import mido

def midi_a_cpp(archivo_midi, nombre_cancion, bpm_override=None, 
               pista=1, nota_min=60, nota_max=84):
    """
    pista    : número de pista donde está la melodía (0, 1, 2...)
    nota_min : nota MIDI mínima a incluir (60=Do4)
    nota_max : nota MIDI máxima a incluir (84=Do6)
    """
    mid = mido.MidiFile(archivo_midi)

    # Mostrar todas las pistas disponibles
    print(f"\nPistas disponibles en {archivo_midi}:")
    for i, track in enumerate(mid.tracks):
        print(f"  Pista {i}: {track.name} ({len(track)} mensajes)")

    def midi_a_hz(nota):
        # Sube una octava para sonar como caja musical
        nota_alta = nota + 12
        return int(440 * (2 ** ((nota_alta - 69) / 12)))

    tempo = 500000
    for track in mid.tracks:
        for msg in track:
            if msg.type == 'set_tempo':
                tempo = msg.tempo
                break

    if bpm_override:
        tempo = int(60000000 / bpm_override)

    ticks_por_beat = mid.ticks_per_beat

    # Usar solo la pista seleccionada
    track = mid.tracks[pista]
    eventos = []
    tiempo_actual = 0
    notas_activas = {}

    for msg in track:
        tiempo_actual += msg.time
        tiempo_ms = int((tiempo_actual / ticks_por_beat) * (tempo / 1000))

        if msg.type == 'note_on' and msg.velocity > 0:
            # Solo notas en el rango de melodía
            if nota_min <= msg.note <= nota_max:
                notas_activas[msg.note] = tiempo_ms

        elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
            if msg.note in notas_activas:
                inicio = notas_activas.pop(msg.note)
                duracion = tiempo_ms - inicio
                if duracion > 30:
                    eventos.append({
                        'freq':     midi_a_hz(msg.note),
                        'duracion': min(duracion, 600), # máx 600ms por nota
                        'tiempo':   inicio
                    })

    if not eventos:
        print(f"\n⚠️  No se encontraron notas en pista {pista}")
        print("Prueba con otro número de pista")
        return

    # Ordenar por tiempo
    eventos.sort(key=lambda x: x['tiempo'])

    # Calcular silencios entre notas
    resultado = []
    for i, e in enumerate(eventos):
        resultado.append(e)
        if i < len(eventos) - 1:
            silencio = eventos[i+1]['tiempo'] - (e['tiempo'] + e['duracion'])
            if silencio > 50:
                resultado.append({'freq': 0, 'duracion': silencio})

    duracion_total = eventos[-1]['tiempo'] + eventos[-1]['duracion']

    # Generar código C++
    nombre_var = nombre_cancion.replace(' ', '_').lower()
    codigo  = f"// {nombre_cancion}\n"
    codigo += f"// {len(resultado)} eventos · {duracion_total/1000:.1f}s\n\n"
    codigo += f"int melodia_{nombre_var}[][2] = {{\n"

    for e in resultado:
        codigo += f"  {{{e['freq']}, {e['duracion']}}},\n"

    codigo += "};\n"
    codigo += f"int total_{nombre_var} = {len(resultado)};\n"

    nombre_archivo = nombre_var + ".h"
    with open(nombre_archivo, 'w') as f:
        f.write(codigo)

    print(f"\n✓ {len(resultado)} eventos generados")
    print(f"✓ Duración: {duracion_total/1000:.1f} segundos")
    print(f"✓ Guardado en: {nombre_archivo}")

# ── USO ──
midi_a_cpp(
    archivo_midi   = "From the Start.mid",
    nombre_cancion = "From The Start",
    bpm_override   = 112,
    pista          = 1,
    nota_min       = 60,
    nota_max       = 96
)