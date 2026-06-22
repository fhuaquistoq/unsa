## Respuesta a preguntas sobre video en streaming con recuperacion in-context de cache KV de video

Shangzhe Di, Zhelun Yu, Guanghao Zhang, Haoyuan Li, Tao Zhong, Hao Cheng, Bolin Li, Wanggui He, Fangxun Shu y Hao Jiang. Shanghai Jiao Tong University y Alibaba Group.

## Resumen

El paper propone ReKV, un enfoque sin entrenamiento adicional para Streaming Video Question Answering (StreamingVQA). ReKV se integra con Video-LLM existentes y permite procesar videos largos de forma incremental. En lugar de reprocesar todo el video para cada pregunta, almacena caches KV de video en RAM o disco y recupera solo las partes relevantes para la consulta.

El metodo usa atencion de ventana deslizante para codificar frames recientes y recuperacion in-context de KV-cache para acceder a informacion historica. La recuperacion puede hacerse con un modelo externo tipo CLIP o con senales internas de atencion del propio Video-LLM. Los experimentos muestran mejoras en precision, latencia y uso de memoria frente al muestreo uniforme y otros metodos de VideoQA.

## 1. Introduccion

La comprension de video se ha centrado tradicionalmente en clips cortos. Sin embargo, aplicaciones como robotica, vigilancia y transmisiones en vivo necesitan analizar flujos continuos y responder preguntas en cualquier momento.

StreamingVQA tiene tres desafios:

1. Codificacion eficiente de video: procesar frames entrantes sin acceso a frames futuros.
2. Preservacion de contexto: mantener informacion pasada relevante para preguntas futuras.
3. Respuesta en tiempo real: contestar con baja latencia usando contexto visual recuperado.

Los Video-LLM actuales suelen usar muestreo disperso o compresion, lo que pierde detalles finos. ReKV evita reprocesar el video completo reutilizando calculos pasados en forma de KV-cache.

## 2. Definicion de StreamingVQA

StreamingVQA consiste en responder una pregunta en un instante t usando solo los frames observados hasta ese momento. OfflineVQA es un caso especial en el que todas las preguntas se hacen despues de ver el video completo.

Los metodos offline son ineficientes para streaming porque deben reprocesar frames para distintas preguntas. ReKV aprovecha la naturaleza causal del decoder del LLM para separar codificacion de video y respuesta a preguntas. Los tokens de video se codifican independientemente de la pregunta, y sus caches KV se guardan para uso posterior.

## 3. ReKV: recuperacion in-context de cache KV

ReKV tiene tres componentes:

- Codificacion del stream de video.
- Recuperacion de cache KV de video.
- Respuesta usando los vectores key-value recuperados.

### Codificacion con atencion de ventana deslizante

El video se procesa por chunks. Cada chunk atiende solo a una ventana local de tokens previos, lo que reduce el costo computacional. Las caches KV de todos los frames se almacenan para recuperacion futura y pueden descargarse a RAM o disco cuando el video es muy largo.

### Recuperacion externa

Un modelo tipo CLIP transforma frames y preguntas en embeddings. Se calcula similitud coseno y se recuperan los frames o bloques mas relevantes. Esta variante sirve como baseline para demostrar que la recuperacion mejora VideoQA.

### Recuperacion interna

El propio Video-LLM produce vectores representativos de frames usando sus claves internas. La pregunta se representa con sus queries y se calcula similitud dentro del espacio del modelo. Esta opcion es mas integrada, evita parametros externos y permite que distintas capas recuperen distintos bloques.

### Respuesta con KV recuperado

Las caches recuperadas se cargan en GPU y sirven como contexto para la generacion autoregresiva. El paper tambien discute el uso de RoPE: para los tokens recuperados, tratarlos como tokens consecutivos funciona mejor que intentar conservar posiciones originales no vistas durante entrenamiento.

## 4. Experimentos

ReKV se evalua con LLaVA-OV-0.5B y LLaVA-OV-7B en GPUs A100. Los benchmarks incluyen MLVU, QAEGO4D, EgoSchema, ActivityNet-QA, RVS-Ego, RVS-Movie y CGBench.

Los resultados muestran que la recuperacion mejora la precision frente al muestreo uniforme. En QAEGO4D, mayor recall de frames relevantes se correlaciona con mayor precision. La recuperacion interna suele superar a la externa porque reutiliza representaciones ya calculadas y captura mejor el contexto del Video-LLM.

En MLVU, ReKV mejora tareas de detalle unico, multiples detalles y comprension holistica. El numero de frames recuperados tiene un punto de saturacion: mas frames pueden ayudar, pero demasiada informacion irrelevante puede perjudicar la respuesta y aumentar latencia.

## 5. Streaming Video Question Answering

En RVS-Ego y RVS-Movie, ReKV responde preguntas inmediatamente despues de sus timestamps anotados, usando solo video previo. La recuperacion externa e interna superan al muestreo uniforme. La recuperacion interna ofrece menor latencia y menor uso de GPU que la externa porque no usa un recuperador adicional.

El paper reporta que LLaVA-OV-7B alcanza 11 FPS de codificacion y LLaVA-OV-0.5B alcanza 17 FPS. La descarga de KV-cache es manejable, aunque sigue siendo un costo relevante: 18.8 GB/h para 7B y 4.0 GB/h para 0.5B.

## 6. Trabajo relacionado

El paper conecta Video-LLM, comprension de video largo, manejo de contexto largo en LLM y retrieval-augmented generation. Su punto distintivo es preservar informacion visual completa como cache KV y recuperarla bajo demanda, en lugar de comprimir todo a captions o tokens de memoria fijos.

## Conclusion

ReKV muestra que StreamingVQA puede resolverse separando codificacion de video y respuesta a preguntas. Su importancia esta en usar la cache KV como memoria in-context recuperable. Para el tema Video-LLM y Video Question Answering, es una referencia central porque define claramente el problema de StreamingVQA y propone una solucion training-free compatible con Video-LLM existentes.

## Referencias

Las referencias originales se conservan como citas del paper fuente. Incluyen trabajos sobre LLaVA-OneVision, Flash-VStream, VideoLLM-Online, StreamingLLM, LM-Infinite, InfLLM, SnapKV, GroundVQA, ActivityNet-QA, EgoSchema, MLVU y CGBench.
