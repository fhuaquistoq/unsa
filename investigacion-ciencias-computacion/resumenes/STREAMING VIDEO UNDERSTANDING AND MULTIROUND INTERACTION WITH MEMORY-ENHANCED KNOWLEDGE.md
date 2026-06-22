## Comprension de video en streaming e interaccion multi-turno con conocimiento enriquecido por memoria

Haomiao Xiong, Zongxin Yang, Jiazuo Yu, Yunzhi Zhuge, Lu Zhang, Jiawen Zhu y Huchuan Lu. Dalian University of Technology y Harvard University.

## Resumen

El paper presenta STREAMCHAT, un framework sin entrenamiento para razonamiento e interaccion conversacional sobre video en streaming. Los Video-LLM actuales tienen dificultades para videos largos, dialogos multi-turno y escenarios dinamicos. STREAMCHAT aborda esto con memoria jerarquica, planificacion paralela del sistema y un benchmark nuevo llamado STREAMBENCH.

STREAMCHAT administra memoria de corto plazo, memoria de largo plazo y memoria de dialogo. Con scheduling paralelo, procesa video hasta 32 FPS y mantiene latencia de generacion menor a 0.9 segundos. STREAMBENCH evalua comprension de video en streaming con interacciones multi-turno, distintos tipos de medios y preguntas complejas.

## 1. Introduccion

Los Video-LLM han avanzado en comprension de escenas, acciones y narrativas, pero suelen procesar videos como clips offline y dialogos de una sola ronda. Esto limita su uso en escenarios reales, donde el usuario puede preguntar varias veces mientras el video sigue avanzando.

Los modelos online previos permiten interaccion, pero todavia sufren por baja velocidad, perdida de informacion y generalizacion limitada. STREAMCHAT propone:

- Adaptabilidad sin entrenamiento.
- Almacenamiento jerarquico de memoria.
- Scheduling de sistema con tres hilos paralelos.

Las contribuciones principales son STREAMCHAT, STREAMBENCH y resultados estado del arte en precision y eficiencia.

## 2. Coleccion y composicion de STREAMBENCH

STREAMBENCH busca simular escenarios online. A diferencia de benchmarks offline, incluye videos mas largos, contenidos diversos y dialogos multi-turno.

El benchmark incluye videos egocentricos, web, trabajo y peliculas. La seleccion combina clasificacion automatica con modelos multimodales y revision humana para eliminar contenido redundante, estatico o ruidoso. El resultado son 306 videos, 24.8 horas totales y una duracion promedio de 4.5 minutos.

### Tipos de tareas

STREAMBENCH define seis tipos de preguntas:

- Object Search (OS): localizar objetos que aparecen por poco tiempo.
- Long-term Memory Search (LM): recordar eventos antiguos.
- Short-term Memory Search (SM): responder sobre eventos recientes.
- Conversational Interaction (CI): usar historial de dialogo.
- Knowledge-based Question Answering (KG): combinar video con conocimiento interno del LLM.
- Simple Factual (SF): preguntas factuales cercanas al inicio del video.

El benchmark contiene 1.8K pares QA de alta calidad con revision humana.

## 3. STREAMCHAT

STREAMCHAT recibe video en streaming y preguntas con timestamp. Usa LongVA como Video-LLM base y agrega dos componentes: memoria jerarquica y scheduling de sistema.

## 3.1 Memoria jerarquica

La memoria se divide en:

- Memoria de corto plazo: mantiene informacion reciente y accesible.
- Memoria de largo plazo: guarda informacion comprimida de eventos pasados.
- Memoria de dialogo: conserva historial conversacional.

### Selective Frame Stacking

Para reducir redundancia, STREAMCHAT usa flujo optico Lucas-Kanade. Si el movimiento entre frames supera un umbral, el frame se codifica y se agrega al buffer visual. Asi evita procesar frames casi identicos.

### Memoria de corto plazo

Inspirada en el modelo Atkinson-Shiffrin y en la curva de olvido de Ebbinghaus, selecciona embeddings visuales recientes para mantener informacion accesible.

### Memoria de largo plazo

La memoria de largo plazo tiene dos formas:

- Pistas textuales que describen eventos pasados.
- Memoria visual comprimida con features agrupadas.

Los chunks del buffer visual se agrupan, reciben captions y forman nodos de un arbol. Los nodos se organizan cronologicamente para permitir recuperacion jerarquica.

### Memoria de dialogo

Cada par pregunta-respuesta se codifica como fragmento de memoria con MiniLM-L6. Esto permite recuperar conversaciones previas relevantes mediante FAISS.

### Recuperacion

Cuando llega una pregunta, el sistema busca pistas textuales y memoria visual relevante en el arbol de largo plazo. Tambien busca dialogos previos similares. La informacion recuperada se usa como contexto para responder.

## 3.2 Scheduling del sistema

STREAMCHAT ejecuta tres hilos independientes:

1. Selective frame stacking: llena buffers visuales con features.
2. Memory formation: actualiza memoria de largo y corto plazo.
3. Contextual summarization: recupera memoria y responde consultas.

Este paralelismo evita que el buffer crezca sin control y permite baja latencia.

## 4. Experimentos

El paper define tres configuraciones: Slow, Base y Fast. Slow prioriza precision; Fast prioriza velocidad; Base equilibra ambas.

Metricas:

- Score semantico con LLaMA-3.
- Accuracy.
- Coherence para dialogos multi-turno.
- Request Processing Delay (RPD).

STREAMCHAT se compara con Video-LLaVA, LLaMA-VID, MovieChat, FreeVA, Video-online, Flash-VStream y otros modelos.

### Resultados online

STREAMCHAT mejora al mejor metodo previo en STREAMBENCH. La version Slow alcanza 64.7% de accuracy y la version Fast procesa a 32 FPS. Todas las versiones mantienen latencia cercana a 0.9 segundos.

La memoria jerarquica mejora especialmente busqueda de objetos, memoria de largo plazo, memoria de corto plazo e interaccion conversacional.

### Resultados offline

En MSRVTT-QA, ActivityNet, NExT-QA y MSVD-QA, STREAMCHAT tambien mejora el backbone LongVA. Esto demuestra que la memoria ayuda incluso cuando la tarea no exige dialogo multi-turno.

## 4.3 Caso de estudio

El paper muestra como STREAMCHAT recupera informacion de un arbol de memoria y del historial de dialogo. El sistema calcula similitudes coseno, selecciona el camino mas relevante en el arbol y agrega memorias de corto plazo para responder.

## 4.4 Ablaciones

Las ablaciones muestran que cada memoria cumple un papel especifico:

- La memoria de dialogo mejora tareas CI.
- La memoria de largo plazo mejora LM.
- La memoria de corto plazo mejora SM.

Tambien se estudian tradeoffs de umbral de flujo optico, longitud de chunk, tamano de grupo y numero de clusters. Mas memoria visual puede mejorar precision, pero aumenta latencia y uso de VRAM.

## 5. Trabajo relacionado

El paper se ubica en modelos multimodales, comprension de video en streaming y retrieval-augmented generation. Su aporte es combinar memoria visual y textual en tiempo real, no solo usar RAG textual.

## 6. Conclusion

STREAMCHAT y STREAMBENCH son importantes para Video-LLM en streaming porque enfatizan interaccion multi-turno, memoria jerarquica y latencia real. Para VideoQA, el paper muestra que responder bien no depende solo de entender frames, sino de recuperar eventos, dialogos previos y conocimiento relevante en el momento correcto.

## Limitaciones y trabajo futuro

El algoritmo de recuperacion aun usa matching basico y puede recuperar informacion incorrecta. La memoria en arbol tambien puede enfrentar limites de VRAM si aumenta la duracion o complejidad del video. El trabajo futuro propone recuperacion mas fina, compresion adaptativa e integracion hardware/sistemas distribuidos.

## Referencias

Las referencias originales se conservan como citas del paper fuente. Incluyen trabajos sobre ChatGPT, LLaMA-VID, LLaMA 3, Video-LLaVA, Chat-UniVi, MovieChat, Flash-VStream, VideoLLM-Online, StreamBench, EgoSchema, YouTube-8M, Lucas-Kanade, MiniLM, FAISS y RAG.
