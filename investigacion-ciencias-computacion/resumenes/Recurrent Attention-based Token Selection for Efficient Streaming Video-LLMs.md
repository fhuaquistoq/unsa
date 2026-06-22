## Seleccion recurrente de tokens basada en atencion para Video-LLM de streaming eficientes

Vaggelis Dorovatas, Soroush Seifi, Gunshi Gupta y Rahaf Aljundi. Toyota Motor Europe, Archimedes RU / Athena RC y University of Oxford.

## Resumen

Los Video-LLM funcionan bien cuando tienen acceso completo al video, pero sufren en streaming porque deben procesar videos largos online y responder con baja latencia. Este paper propone rLiVS, un metodo sin entrenamiento y compatible con Video-LLM estandar.

rLiVS se basa en tres ideas:

1. Seleccionar tokens visuales importantes usando la atencion del LLM.
2. Procesar recurrentemente tokens seleccionados de clips pasados para conservar coherencia temporal.
3. Responder preguntas usando captions recuperadas, lo que reduce costo y mejora precision.

El metodo descarta hasta aproximadamente 95% de tokens visuales con perdida minima y logra resultados estado del arte en benchmarks de streaming.

## 1. Introduccion

VideoQA y comprension de video eficiente son esenciales para asistentes inteligentes en conduccion autonoma, vigilancia, salud y entretenimiento. Los enfoques que procesan todo el video in-context funcionan en clips cortos, pero no escalan a videos largos porque crecen los tokens visuales y el costo computacional.

Los trabajos recientes comprimen informacion visual, generan captions por clip o almacenan KV-cache. Cada enfoque tiene limites: los metodos entrenados pueden no extrapolar a videos arbitrariamente largos, ReKV usa caches grandes y los metodos solo basados en captions pierden continuidad entre clips.

rLiVS combina memoria visual ligera y razonamiento textual. Procesa videos en clips cortos, selecciona tokens visuales importantes de cada clip, los reutiliza como memoria para el siguiente clip y almacena captions para responder preguntas posteriores.

## 2. Trabajo relacionado

Los modelos video-lenguaje suelen muestrear pocos frames y convertirlos en tokens visuales. Esto funciona para clips cortos, pero no para videos largos.

Los metodos offline de video largo comprimen informacion por seleccion de keyframes, pooling, memoria visual o resumen textual. Algunos requieren entrenamiento adicional o no mantienen memoria persistente entre clips.

Los metodos de streaming buscan procesar frames incrementalmente. ReKV almacena KV-cache completa, pero eso puede ser pesado. rLiVS propone una alternativa mas ligera: guardar pocos tokens seleccionados y captions.

## 3. Metodo

rLiVS usa un Video-LLM de clips cortos como backbone. Para cada clip, el modelo genera una caption. Luego se selecciona un subconjunto pequeno de tokens visuales que recibieron mayor atencion desde la caption generada.

### 3.1 Seleccion de tokens visuales basada en atencion

La atencion del LLM funciona como senal de relevancia. Si un token visual recibio mucha atencion al generar la caption, probablemente contribuyo a la comprension del clip.

El metodo calcula scores de atencion entre tokens de caption y tokens visuales. Luego promedia por tokens de texto, cabezas y capas seleccionadas. Finalmente ordena los tokens y conserva los mas importantes, manteniendo su orden temporal original.

### 3.2 Procesamiento recurrente

Para el primer clip, se seleccionan tokens importantes. Para el siguiente clip, esos tokens seleccionados se agregan al contexto junto con los tokens completos del clip nuevo. Asi, la memoria visual previa guia la atencion del modelo.

El sistema mantiene una cola FIFO de tokens seleccionados. Cuando se supera el limite de contexto, descarta los mas antiguos. Esto ofrece continuidad temporal sin almacenar todo el video.

### 3.3 VideoQA eficiente

Durante el procesamiento, cada clip genera una caption condicionada por la memoria visual previa. Para responder una pregunta, el sistema recupera las captions mas relevantes mediante similitud con la pregunta y Maximal Marginal Relevance (MMR), que balancea relevancia y diversidad.

El paper observa que responder con captions suele funcionar mejor que responder con tokens visuales recuperados. Esto convierte el problema en QA de contexto largo textual, donde los LLM son fuertes.

## 4. Experimentos

El metodo se evalua en Realtime VStream-QA, RVS-Movie, RVS-Ego, MovieChat, VS-Movie, VS-Ego, CG-Bench y NextQA-valset. El backbone principal es LLaVA-OneVision, con variantes 7B y 0.5B. Tambien se prueba Qwen2.5-VL-7B.

En LLaVA-OV, cada clip usa 16 frames actuales y 16 frames equivalentes de memoria recurrente. Se seleccionan 196 tokens de 3136, es decir 6.25% de la informacion visual por clip. Los scores de atencion se promedian en 4 capas del backbone.

### Resultados en video largo offline

rLiVS logra resultados fuertes en VS-Ego, VS-Movie, MovieChat y CG-Bench. Supera metodos previos sin entrenamiento adicional y con menor memoria.

### Seleccion visual en videos cortos

En NextQA, seleccionar tokens por atencion supera muestreo uniforme y mean pooling. Con solo 6% de tokens, la perdida frente al modelo completo es pequena. Con 12%, casi iguala el rendimiento completo.

### Streaming VideoQA

En RVS-Ego y RVS-Movie, rLiVS supera a ReKV con el mismo backbone. Tambien reduce latencia y VRAM porque no necesita descargar KV-cache. Con LLaVA-OV 7B, rLiVS reporta 65.3% en RVS-Ego y 57.7% en RVS-Movie, con 1.9 s de latencia y 25 GB de VRAM.

El metodo tambien mejora al usar Qwen2.5-VL, lo que respalda su caracter agnostico al backbone.

## 4.1 Ablaciones

### Importancia de la recurrencia

Sin recurrencia, el modelo pierde continuidad entre clips. La recurrencia mejora entre 3% y 4% en benchmarks largos porque ayuda a rastrear entidades y mantener coherencia.

### Captions frente a tokens visuales

Las captions funcionan mejor para recuperacion y respuesta porque estan en la misma modalidad que la pregunta. Los tokens visuales pueden contener mas informacion, pero no siempre estan semanticamente alineados con consultas textuales.

### Atencion frente a muestreo uniforme

La seleccion basada en atencion supera al muestreo uniforme en streaming. Como la atencion ya se calcula durante la caption, el costo adicional es bajo.

### Tasa de compresion

Retener 6% de tokens visuales es un compromiso practico. Los videos largos tienen mucha redundancia y la seleccion por atencion filtra ruido sin dañar demasiado el desempeno. El paper sugiere compresion adaptativa como trabajo futuro.

## 5. Discusion

rLiVS propone una solucion simple y fuerte: seleccionar inmediatamente tokens visuales segun atencion, usar memoria recurrente y responder con captions. No requiere entrenamiento y puede adaptarse a varios Video-LLM.

Sus limitaciones son claras. Al seleccionar solo una parte de los tokens, puede perder detalles finos. La cola FIFO no siempre conserva lo semanticamente mas importante. Ademas, depende de las capacidades del backbone preentrenado.

## Conclusion

rLiVS es importante para Video-LLM y VideoQA en streaming porque ofrece una alternativa ligera a la cache KV completa. En vez de guardar todo, conserva los tokens que el propio LLM considero relevantes y transforma la respuesta posterior en un problema textual de contexto largo. Esto lo vuelve practico para escenarios con memoria y latencia limitadas.

## Referencias

Las referencias originales se conservan como citas del paper fuente. Incluyen trabajos sobre Goldfish, Qwen, Qwen2.5-VL, MMR, CG-Bench, ReKV, Hermes, LLaVA-OneVision, VideoScan, LLaMA-VID, Video-ChatGPT, MovieChat, FlashAttention, VideoStreaming, CLIP, Video-XL y MovieChat.
