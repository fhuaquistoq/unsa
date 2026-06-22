## V-Rex: aceleracion en tiempo real de Video-LLM en streaming mediante recuperacion dinamica de cache KV

Donghyuk Kim, Sejeong Yang, Wonjin Shin y Joo-Young Kim, KAIST. Daejeon, Republica de Corea.

## Resumen

Los Video-LLM de streaming se usan cada vez mas en tareas multimodales en tiempo real, como subtitulado de video, respuesta a preguntas, agentes conversacionales y realidad aumentada. Su principal obstaculo es que las caches key-value (KV) crecen rapidamente con la entrada continua de video. En pocos minutos, esta memoria puede superar la capacidad de la GPU, especialmente en dispositivos edge.

V-Rex propone una solucion de codiseno software-hardware para inferencia de Video-LLM en streaming. Su componente algoritmico, ReSV, recupera dinamicamente la cache KV sin entrenamiento adicional. ReSV agrupa tokens visuales por similitud espacial y temporal, y ajusta la seleccion de tokens por capa y cabeza de atencion. Su componente hardware, DRE, acelera las operaciones irregulares de recuperacion mediante unidades bit-level, ordenamiento con salida temprana y gestion jerarquica de memoria.

Evaluado en COIN, V-Rex alcanza 3.9-8.3 FPS en escenarios edge con perdida despreciable de precision. Tambien reporta aceleraciones de 1.9-19.7x y mejoras de eficiencia energetica de 3.1-18.5x frente a AGX Orin.

## I. Introduccion

Los modelos multimodales grandes han avanzado en la comprension conjunta de texto, imagen, video y audio. Dentro de ellos, los Video-LLM de streaming son importantes porque procesan video en tiempo real y responden consultas durante la transmision. A diferencia de los modelos offline, no pueden esperar a recibir todo el video.

El problema central es el prefill iterativo. Cada nuevo frame genera entradas KV que deben mantenerse para responder preguntas futuras. Esta cache crece linealmente con el tiempo y genera complejidad de memoria y computo que vuelve impracticable el uso prolongado en GPU.

Las tecnicas destructivas, como poda, compresion, fusion o cuantizacion, pueden eliminar informacion que no parece importante ahora pero podria ser necesaria en una consulta posterior. La recuperacion de cache KV evita este problema porque conserva la cache completa en CPU o almacenamiento y trae a GPU solo los tokens relevantes. Sin embargo, los metodos existentes fueron disenados para generacion de texto, no para prefill iterativo de video.

V-Rex aborda esta brecha con dos ideas:

- ReSV reduce el trafico de cache KV mediante agrupamiento por similitud y seleccion dinamica.
- DRE ejecuta eficientemente esas operaciones irregulares, que no encajan bien con GPUs convencionales.

## II. Antecedentes y motivacion

Un Video-LLM de streaming suele tener tres modulos: una torre visual, un proyector MLP y un LLM. La torre visual convierte frames en embeddings; el proyector adapta esos embeddings al espacio del LLM; y el LLM usa la informacion visual y textual para generar respuestas.

En streaming, los frames llegan secuencialmente y no pueden procesarse como un lote completo. Cada frame requiere un prefill nuevo y consulta la cache previa. Por eso, la cache KV no es solo un detalle tecnico: es la memoria operativa del modelo.

La recuperacion de cache KV tiene tres pasos:

1. Descargar la cache completa a CPU o almacenamiento.
2. Seleccionar los tokens relevantes para la consulta o el frame actual.
3. Precargar esos tokens en GPU para el calculo de atencion.

Este mecanismo conserva coherencia en dialogos multi-turno, reduce memoria en GPU y limita el computo a un subconjunto relevante.

## III. Limitaciones de la recuperacion KV existente

Los metodos como InfiniGen, FlexGen o ReKV no resuelven bien el cuello de botella de los Video-LLM de streaming porque optimizan principalmente la etapa de generacion. En streaming, la mayor parte de la latencia se concentra en el prefill iterativo.

El articulo muestra que, con secuencias KV largas, el prefill puede representar 83% de la latencia total, y la recuperacion KV puede consumir 74% de esa etapa. Por tanto, esconder latencia solo durante generacion no basta.

Otro problema es el uso de seleccion top-k fija. Aunque top-k es comodo para GPU, no refleja que la importancia de los tokens varia por capa y por cabeza de atencion. Un k fijo puede traer demasiados tokens redundantes o dejar fuera tokens criticos.

## IV. V-Rex: estrategia unificada de software y hardware

V-Rex integra ReSV y DRE.

ReSV es un algoritmo sin entrenamiento que opera durante el prefill. Primero agrupa tokens similares mediante hash-bit key clustering. Luego selecciona dinamicamente los clusters importantes con WiCSum thresholding. La seleccion depende de la distribucion real de scores por capa y cabeza, no de un presupuesto fijo.

### Agrupamiento hash-bit

El video tiene alta redundancia entre frames cercanos. ReSV aprovecha esa propiedad transformando claves en representaciones binarias pequenas. Luego mide distancia de Hamming para agrupar tokens espacial y temporalmente similares. Esto evita calculos costosos de similitud coseno en alta dimension.

### Seleccion WiCSum

WiCSum calcula una suma ponderada de scores y conteos de tokens por cluster. Despues ordena los scores y acumula hasta superar un umbral. Asi selecciona solo los clusters necesarios, con un numero variable de tokens segun la importancia real de cada capa y cabeza.

## V. Arquitectura hardware de V-Rex

El acelerador V-Rex contiene un motor de ejecucion del LLM y un motor de recuperacion dinamica de cache KV (DRE). DRE se divide en:

- KVPU: acelera el agrupamiento hash-bit y el umbral WiCSum.
- KVMU: gestiona la memoria jerarquica de la cache KV y el mapeo por clusters.

La HCU calcula distancias de Hamming con acumuladores XOR. La WTU acelera el ordenamiento y la verificacion de umbral con salida temprana. La KVMU mantiene entradas recientes en memoria rapida y descarga las antiguas a CPU o almacenamiento. Tambien almacena tokens de un mismo cluster en direcciones contiguas para mejorar el uso de PCIe.

## VI. Evaluacion

El sistema se evaluo con simuladores de ciclo, DRAMSim3 y MQSim, comparando escenarios edge y servidor frente a AGX Orin y A100. El backbone fue Llama-3 8B con SigLIP-ViT-L-384 como codificador visual.

Los resultados muestran que V-Rex mantiene inferencia en tiempo real incluso con caches largas. En edge logra 3.9-8.3 FPS y aceleraciones de hasta 13.8x en procesamiento por frame. En servidor, V-Rex 48 alcanza latencias de 20-48 ms por frame.

La eficiencia energetica mejora porque ReSV reduce el volumen de datos recuperados y KVMU mejora el acceso a memoria. El estudio de ablacion confirma que ReSV por si solo no basta: el beneficio completo aparece cuando el algoritmo se combina con hardware especializado.

## Conclusion

V-Rex demuestra que la recuperacion de cache KV para Video-LLM de streaming no es solo un problema algoritmico. La etapa de prefill genera operaciones irregulares y dependientes de datos que requieren soporte hardware especializado para ser eficientes. La propuesta es importante porque apunta directamente al despliegue real en dispositivos edge, donde memoria, energia y latencia son restricciones criticas.

## Referencias

Las referencias bibliograficas originales se conservan como citas del paper fuente. Incluyen trabajos sobre VideoLLM-Online, ReKV, InfiniGen, FlexGen, Oaken, Llama-3, Qwen, SigLIP, COIN y tecnicas de optimizacion de cache KV.
