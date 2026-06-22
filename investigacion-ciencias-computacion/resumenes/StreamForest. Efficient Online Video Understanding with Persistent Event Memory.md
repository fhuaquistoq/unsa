## StreamForest: comprension eficiente de video online con memoria persistente de eventos

Xiangyu Zeng, Kefan Qiu, Qingyu Zhang, Xinhao Li, Jing Wang, Jiaxin Li, Ziang Yan, Kun Tian, Meng Tian, Xinhai Zhao, Yi Wang y Limin Wang.

GitHub: https://github.com/MCG-NJU/StreamForest

## Resumen

Los modelos multimodales grandes han avanzado en comprension de video, pero siguen limitados en escenarios de streaming por dos razones: no pueden almacenar indefinidamente las caracteristicas visuales historicas y tienen dificultades para razonar en tiempo real sobre informacion espacial y temporal.

StreamForest propone una arquitectura para comprension de video en streaming. Su componente principal es Persistent Event Memory Forest, una memoria que organiza frames en estructuras de arbol a nivel de eventos. La fusion de eventos se guia por penalizaciones de distancia temporal, similitud de contenido y frecuencia de fusion. Para la percepcion inmediata, usa una Fine-grained Spatiotemporal Window que conserva senales visuales recientes de alta resolucion.

El trabajo tambien introduce OnlineIT, un dataset de instruction tuning para streaming, y ODV-Bench, un benchmark centrado en comprension online de video para conduccion autonoma. StreamForest obtiene resultados estado del arte en StreamingBench, OVBench y OVO-Bench, y conserva 96.8% de su precision promedio aun con compresion extrema a 1024 tokens visuales.

## 1. Introduccion

La comprension de video offline asume que todo el video esta disponible antes de responder. En cambio, aplicaciones como conduccion autonoma, robotica o video en vivo requieren procesamiento online. Esto exige memoria persistente, percepcion inmediata y capacidad de anticipacion.

Los metodos previos suelen reducir informacion visual mediante compresion en el muestreo o compresion en almacenamiento. La compresion durante el muestreo pierde detalles finos; la compresion durante almacenamiento puede fusionar eventos locales en exceso o descartar acciones importantes por ruido de fondo.

StreamForest aborda estos problemas con dos memorias complementarias:

- Una ventana espaciotemporal fina para el contexto reciente.
- Un bosque de memoria persistente para conservar eventos a largo plazo.

## 2. Trabajo relacionado

Los MLLM de video buscan extender las capacidades de modelos imagen-texto hacia secuencias temporales. Muchos trabajos se concentran en escenarios offline, donde el modelo observa el video completo antes de responder. Esa configuracion facilita el modelado global, pero no resuelve la interaccion en tiempo real.

En streaming, el modelo debe responder usando solo informacion pasada y presente. Las tecnicas existentes tienden a comprimir agresivamente frames, lo cual limita el razonamiento fino. StreamForest se diferencia al preservar detalles recientes y organizar el pasado en memoria de eventos.

## 3. Metodologia

### 3.1 Arquitectura StreamForest

StreamForest combina Fine-grained Spatiotemporal Window (FSTW) y Persistent Event Memory Forest (PEMF).

FSTW conserva rasgos visuales recientes porque muchas preguntas dependen del momento actual o de segundos cercanos. Los frames nuevos se codifican con informacion posicional espaciotemporal. Los frames mas antiguos se comprimen y se envian a la memoria de corto plazo o al bosque persistente.

PEMF administra memoria de largo plazo. Construye una jerarquia de eventos, donde cada nodo representa un meta-evento formado por tokens visuales de frames consecutivos similares. Cuando se supera el presupuesto de tokens, el sistema fusiona nodos adyacentes segun tres penalizaciones.

### Penalizacion de similitud

Eventos visualmente similares se fusionan para eliminar redundancia. El metodo usa emparejamiento bipartito inspirado en ToMe para manejar eventos con distinto numero de tokens.

### Penalizacion por numero de fusiones

Si un nodo se fusiona demasiadas veces, puede perder detalles. Esta penalizacion evita que algunos eventos sean comprimidos en exceso y equilibra el arbol.

### Penalizacion por distancia temporal

Los eventos recientes suelen ser mas relevantes para una consulta actual. Por eso, StreamForest preserva con mayor fidelidad los eventos cercanos y comprime mas agresivamente los del pasado distante.

La combinacion de estas penalizaciones permite un equilibrio entre FIFO, muestreo uniforme y fusion por similitud.

## 3.2 Dataset OnlineIT

OnlineIT esta disenado para instruction tuning en video streaming. Incluye datos generales y datos de conduccion autonoma. OnlineIT-drive contiene 89K instancias de QA relacionadas con localizacion, entidades de trafico, objetos dinamicos, riesgos y accidentes.

El objetivo es reducir alucinaciones causadas por cambios de distribucion entre frames historicos y actuales, y mejorar la percepcion del presente y la prediccion del futuro.

## 4. ODV-Bench

ODV-Bench evalua comprension online en conduccion autonoma. Incluye tareas orientadas a objetivos estaticos, objetivos dinamicos y eventos multiagente. El benchmark exige identificar objetos sutiles, relaciones espaciales, trayectorias y riesgos.

La construccion usa seleccion de videos, generacion de meta-anotaciones, plantillas de preguntas de opcion multiple y control de calidad manual. StreamForest supera modelos offline y online en este benchmark, especialmente despues de ajuste con OnlineIT-drive.

## 5. Experimentos

El modelo usa SigLIP-so400M como codificador visual, un MLP como proyector y Qwen2-7B como LLM. El limite visual por defecto es 8192 tokens. La evaluacion procesa frames a 1 FPS.

StreamForest logra:

- 77.3% en StreamingBench.
- 60.5% en OVBench.
- 55.6% en OVO-Bench.
- 59.9% en ODV-Bench sin entrenamiento especifico de conduccion.
- 71.2% en ODV-Bench despues de usar OnlineIT-drive.

En benchmarks offline tambien mantiene resultados competitivos, lo que sugiere que la memoria de eventos no solo ayuda al streaming, sino tambien a videos largos ya disponibles.

## Ablaciones

PEMF supera muestreo uniforme, FIFO, fusion por similitud y bancos piramidales de memoria. Las ablaciones muestran que FSTW y PEMF son complementarios: FSTW mejora la percepcion reciente y PEMF mejora la memoria de largo plazo.

Con presupuestos visuales bajos, StreamForest conserva rendimiento. Incluso con 1K tokens visuales, mantiene buena precision gracias a la consolidacion adaptativa de eventos.

## Conclusion

StreamForest propone una forma clara de pensar la memoria en Video-LLM de streaming: no basta con guardar frames, hay que organizar eventos. Su importancia para Video Question Answering en streaming esta en que separa la percepcion inmediata de la memoria historica y permite responder consultas actuales sin olvidar eventos pasados.

## Referencias

Las referencias originales se conservan como citas del paper fuente e incluyen trabajos sobre MLLM de video, streaming video understanding, ToMe, SigLIP, Qwen, VideoChat-Online, Flash-VStream, OVBench, OVO-Bench y benchmarks de conduccion autonoma.
