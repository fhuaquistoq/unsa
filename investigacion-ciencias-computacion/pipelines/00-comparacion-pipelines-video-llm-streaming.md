# Comparacion de pipelines para Video LLM en streaming

## Respuesta corta

Los cinco papers atacan el mismo problema: **entender video continuo sin que tokens, memoria visual o KV-cache crezcan sin limite**. La diferencia importante no es solo el backbone, sino la politica de memoria: que se guarda, como se comprime, como se recupera y que costo de reproduccion introduce.

**Mejor para ejecutar rapido:** ReKV, porque es training-free, tiene repo claro y se alinea directamente con StreamingVQA.  
**Mejor conceptualmente para el survey:** StreamForest, porque separa percepcion fina del presente y memoria persistente de eventos.  
**Mejor para discutir deployment real:** V-Rex, porque muestra que el cuello de botella tambien es PCIe, energia, prefill iterativo y hardware edge.

## Matriz de decisión

| Paper | Problema | Memoria | Recuperacion | Entrenamiento | Recursos | Mejor uso en survey |
|---|---|---|---|---|---|---|
| ReKV | QA sobre video visto hasta el tiempo de consulta | Video KV-cache offloaded a RAM/disco | KV relevantes por similitud externa o interna | Training-free | Experimentos en A100 80GB; Fig. 1 reporta H800 80GB | Baseline ejecutable de KV-cache retrieval |
| StreamChat | Dialogo multi-turno con memoria visual y conversacional | Memoria corta, arbol de memoria larga y memoria de dialogo | Similitud sobre text clues, visual memory y FAISS para dialogo | Training-free | Paper reporta GPUs con 80GB; detalles de repo pueden variar | Interaccion continua con memoria jerarquica |
| V-Rex | Prefill iterativo y trafico PCIe en edge deployment | KV-cache completo preservado fuera de GPU y seleccion dinamica | ReSV: clustering de keys por hash-bit + WiCSum | Training-free retrieval + co-diseno hardware/software | Evaluacion con COIN, A100/AGX Orin, simulacion/sintesis | Paper de sistemas para latencia, FPS y energia |
| rLiVS | Exceso de tokens visuales en streams largos | Tokens visuales seleccionados + captions | Captions recuperadas por similitud/MMR | Training-free, model-agnostic | Repo oficial; requiere ajustes fragiles de entorno | Compresion online simple guiada por atencion |
| StreamForest | Memoria larga de eventos + percepcion fina del presente | FSTW + Persistent Event Memory Forest | Raices de eventos + ventana fina actual | Arquitectura entrenada/fine-tuned con OnlineIT | Training completo: 5 etapas, 32 A100; evaluacion con checkpoints es mas viable | Arquitectura mas rica para memoria persistente |

## Comparacion rapida

| Paper | Ejecutabilidad | Interes conceptual | Riesgo principal |
|---|---|---|---|
| ReKV | Alta | Alta | KV-cache crece y el retrieval puede fallar en detalles relevantes |
| StreamChat | Media-baja | Alta | Sistema con varios modelos, scheduling y memoria textual/visual dificil de aislar |
| V-Rex | Baja | Media-alta | No es un pipeline software comun; depende de simulacion/co-diseno hardware |
| rLiVS | Media | Alta | La memoria textual puede perder detalles visuales finos |
| StreamForest | Media para eval, baja para training | Muy alta | Entrenamiento y datos son pesados; arquitectura mas compleja |

## Lectura comparativa de los pipelines

### 1. ReKV: memoria como KV-cache recuperable

ReKV conserva los KV-caches visuales ya computados, los descarga de GPU cuando salen de la ventana local y recupera solo los vectores clave-valor relevantes cuando llega una pregunta.

**Idea fuerte:** no reanalizar el video; reutilizar computo pasado.  
**Costo:** el cache sigue creciendo y el problema se desplaza a almacenamiento, retrieval y transferencia.

### 2. StreamChat: memoria como conocimiento interactivo

StreamChat no trata el stream como una unica pregunta. Mantiene memoria visual corta, memoria larga como arbol con text clues y visual memory comprimida, y memoria de dialogo para preguntas multi-turno.

**Idea fuerte:** streaming como conversacion continua con memoria activa.  
**Costo:** muchas piezas acopladas: frame stacking, clustering, captions, embeddings, FAISS y scheduling.

### 3. V-Rex: memoria como problema de sistema

V-Rex tambien es KV-cache retrieval, pero no compite con ReKV como simple baseline de QA. Su foco es el cuello de botella del **iterative prefill** en edge: trafico CPU/GPU, PCIe, energia y seleccion dinamica de KV por capa/cabeza.

**Idea fuerte:** para tiempo real de verdad, la arquitectura computacional importa.  
**Costo:** dificil de reproducir sin artefactos de hardware, simulador o implementacion equivalente.

### 4. rLiVS: memoria como seleccion recurrente de tokens

rLiVS conserva solo los tokens visuales a los que el LLM presto atencion al generar captions de clips cortos. Luego usa las captions como memoria textual recuperable.

**Idea fuerte:** el propio LLM decide que tokens visuales recordar, sin entrenamiento ni modulos externos.  
**Costo:** si la caption borra un detalle fino, responderlo despues puede ser imposible.

### 5. StreamForest: memoria como bosque de eventos

StreamForest cambia la unidad de memoria: no frames ni KV plano, sino eventos organizados jerarquicamente. FSTW conserva el presente con detalle; PEMF comprime el pasado como memoria persistente de eventos.

**Idea fuerte:** combina presente fino, memoria corta y memoria larga semantica.  
**Costo:** entrenar desde cero usa cinco etapas y 32 A100; para reproducir conviene partir de checkpoints.

## Mejor pipeline para ejecutar

**ReKV** es el mejor candidato practico.

Razones:

1. Tiene repo oficial claro: https://github.com/Becomebright/ReKV
2. Es training-free.
3. Tiene modelos y datasets enlazados.
4. Permite empezar con `llava_ov_0.5b`.
5. Su pipeline esta directamente alineado con streaming QA.

Ruta recomendada:

1. Clonar ReKV.
2. Instalar con `bash prepare.sh`.
3. Descargar `llava-onevision-qwen2-0.5b-ov-hf`.
4. Descargar QAEgo4D-MC-test o RVS.
5. Ejecutar evaluacion con `--sample_fps 0.5` y `--retrieve_size 64`.
6. Analizar que KV-caches/frames representativos recupera por pregunta.

## Mejor pipeline para el survey

**StreamForest** es el mas fuerte conceptualmente.

Razones:

1. Modela el pasado como eventos persistentes, no como tokens planos.
2. Separa presente de alta resolucion, memoria corta y memoria larga.
3. Introduce OnlineIT y ODV-Bench para streaming y conduccion.
4. Reporta resultados fuertes: 77.3% en StreamingBench, 60.5% en OVBench y 55.6% en OVO-Bench.
5. Permite narrar la evolucion del campo: de frames/KV hacia memoria estructurada.

## Riesgo de reproduccion

| Paper | Riesgo | Que verificar primero |
|---|---|---|
| ReKV | Medio | VRAM/RAM disponible, rutas de datasets, costo de offloading |
| StreamChat | Alto | Modelos requeridos, rutas `Your_xxxxx`, GPUs de 80GB, tiempo de inferencia |
| V-Rex | Muy alto | Existencia de codigo/simulador y supuestos de hardware |
| rLiVS | Medio | Version de `transformers`, reemplazos internos, datasets concretos |
| StreamForest | Alto | Checkpoints, anotaciones locales, scripts de eval, no intentar training completo al inicio |

## Recomendacion para el paper/survey

Usa esta tesis organizadora:

> Los Video-LLMs para streaming no se diferencian solo por el backbone, sino por la politica de memoria: que guardan, como lo comprimen, como lo recuperan y cuanto detalle sacrifican para responder en tiempo real.

Orden sugerido:

1. **KV-cache retrieval:** ReKV y V-Rex.
2. **Memoria textual/dialogal:** StreamChat y rLiVS.
3. **Memoria estructurada por eventos:** StreamForest.

## Conclusion

Si necesitas **entender y ejecutar**, empieza con ReKV. Si necesitas **defender una contribucion conceptual fuerte**, usa StreamForest como eje del survey. Si quieres mostrar que el problema tambien es de sistemas, V-Rex es el contraste necesario.
