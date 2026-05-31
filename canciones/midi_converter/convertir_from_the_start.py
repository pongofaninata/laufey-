import mido

def midi_a_eventos(archivo_midi, nombre_var, pista_idx=None, nota_min=55, nota_max=100):
    mid = mido.MidiFile(archivo_midi)
    
    print(f"\nPistas disponibles:")
    for i, track in enumerate(mid.tracks):
        print(f"  Pista {i}: '{track.name}' ({len(track)} mensajes)")
    
    def midi_a_hz(nota):
        return int(440 * (2 ** ((nota - 69) / 12)))
    
    def midi_a_tecla(nota):
        return nota % 12
    
    # Nombres de teclas
    nombres = ["Do","Do#","Re","Re#","Mi","Fa","Fa#","Sol","Sol#","La","La#","Si"]
    
    tempo = 500000
    for track in mid.tracks:
        for msg in track:
            if msg.type == 'set_tempo':
                tempo = msg.tempo
                break
    
    bpm = 60000000 / tempo
    print(f"\nBPM: {bpm:.1f}")
    print(f"Tempo: {tempo}")
    
    ticks_por_beat = mid.ticks_per_beat
    
    # Si no se especifica pista busca la mejor
    pistas_a_procesar = [pista_idx] if pista_idx is not None else range(len(mid.tracks))
    
    mejor_pista = None
    mejor_notas = []
    
    for p in pistas_a_procesar:
        if p >= len(mid.tracks):
            continue
        track = mid.tracks[p]
        tiempo_actual = 0
        notas_activas = {}
        notas = []
        
        for msg in track:
            tiempo_actual += msg.time
            tiempo_ms = int((tiempo_actual / ticks_por_beat) * (tempo / 1000))
            
            if msg.type == 'note_on' and msg.velocity > 0:
                if nota_min <= msg.note <= nota_max:
                    notas_activas[msg.note] = tiempo_ms
            elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
                if msg.note in notas_activas:
                    inicio = notas_activas.pop(msg.note)
                    dur = tiempo_ms - inicio
                    if dur > 50:
                        notas.append({
                            'nota': msg.note,
                            'tecla': midi_a_tecla(msg.note),
                            'freq': midi_a_hz(msg.note),
                            'tiempo': inicio,
                            'duracion': min(dur, 800)
                        })
        
        if len(notas) > len(mejor_notas):
            mejor_notas = notas
            mejor_pista = p
    
    if not mejor_notas:
        print("No se encontraron notas")
        return
    
    print(f"\nUsando pista {mejor_pista} con {len(mejor_notas)} notas")
    
    # Ordenar por tiempo
    mejor_notas.sort(key=lambda x: x['tiempo'])
    
    duracion_total = mejor_notas[-1]['tiempo'] + mejor_notas[-1]['duracion']
    
    print(f"Duración total: {duracion_total/1000:.1f}s")
    print(f"\nPrimeras 15 notas:")
    for n in mejor_notas[:15]:
        print(f"  t={n['tiempo']}ms tecla={n['tecla']} ({nombres[n['tecla']]}) freq={n['freq']}Hz dur={n['duracion']}ms")
    
    # Generar código C++
    codigo = f"// {nombre_var}\n"
    codigo += f"// {len(mejor_notas)} eventos · {duracion_total/1000:.1f}s\n\n"
    codigo += f"Evento {nombre_var}[] = {{\n"
    
    tiempo_anterior = 0
    for n in mejor_notas:
        # Agregar silencio si hay hueco
        silencio = n['tiempo'] - tiempo_anterior
        if silencio > 100:
            codigo += f"  {{-1, {silencio}}},  // silencio\n"
        codigo += f"  {{{n['tecla']}, {n['duracion']}}},  // {nombres[n['tecla']]} {n['freq']}Hz\n"
        tiempo_anterior = n['tiempo'] + n['duracion']
    
    codigo += "};\n"
    codigo += f"int {nombre_var}_total = sizeof({nombre_var})/sizeof({nombre_var}[0]);\n"
    codigo += f"long {nombre_var}_duracion = {duracion_total};\n"
    
    nombre_archivo = nombre_var + "_eventos.h"
    with open(nombre_archivo, 'w') as f:
        f.write(codigo)
    
    print(f"\n✓ Guardado en: {nombre_archivo}")
    print(f"✓ {len(mejor_notas)} notas generadas")
    print(f"✓ Duración: {duracion_total/1000:.1f}s")

# ── USO ──
midi_a_eventos(
    archivo_midi = "From_the_Start.mid",
    nombre_var   = "fromTheStart",
    pista_idx    = None,   # None = detecta automáticamente la mejor pista
    nota_min     = 55,     # Sol3
    nota_max     = 100     # Mi7
)
