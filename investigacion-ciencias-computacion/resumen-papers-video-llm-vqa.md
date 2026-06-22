# Resumen de papers sobre Video-LLM y Video Question Answering

Este documento resume los papers disponibles en `docling/` y explica por que cada uno es importante para estudiar Video-LLM, Video Question Answering y, como subfoco, video en streaming.

## 1. V-Rex: Real-Time Streaming Video LLM Acceleration via Dynamic KV Cache Retrieval

**Idea central:** V-Rex acelera Video-LLM de streaming mediante recuperacion dinamica de cache KV y codiseno software-hardware. Su algoritmo ReSV agrupa tokens visuales por similitud espacial-temporal y selecciona dinamicamente los tokens relevantes por capa y cabeza de atencion. Su hardware DRE acelera esas operaciones irregulares.

**Importancia para el tema:** Es clave para entender el cuello de botella de memoria y latencia en Video-LLM de streaming. Muestra que no basta con optimizar el modelo: la cache KV y el prefill iterativo pueden dominar el costo, sobre todo en dispositivos edge.

## 2. StreamForest: Efficient Online Video Understanding with Persistent Event Memory

**Idea central:** StreamForest organiza el video en una memoria persistente de eventos. Combina una ventana espaciotemporal fina para informacion reciente con un bosque de memoria de eventos para conservar contexto de largo plazo.

**Importancia para el tema:** Aporta una estrategia clara de memoria para streaming. En VideoQA, muchas preguntas requieren recordar eventos pasados sin perder detalles recientes; StreamForest separa esos dos niveles de memoria.

## 3. Streaming Video Question-Answering with In-Context Video KV-Cache Retrieval

**Idea central:** ReKV permite StreamingVQA sin entrenamiento adicional. Codifica el video con atencion de ventana deslizante, guarda caches KV en RAM o disco y recupera solo las caches relevantes para responder cada pregunta.

**Importancia para el tema:** Es uno de los papers mas directamente alineados con Video Question Answering en streaming. Define bien el problema de StreamingVQA y propone una forma practica de separar codificacion de video y respuesta a preguntas.

## 4. Video-LLaVA: Learning United Visual Representation by Alignment Before Projection

**Idea central:** Video-LLaVA alinea imagenes y videos en un espacio visual unificado antes de proyectarlos al LLM. Entrena conjuntamente con datos de imagen y video para mejorar ambas modalidades.

**Importancia para el tema:** No es principalmente un paper de streaming, pero es importante como base de Video-LLM. Explica por que la representacion visual unificada ayuda al razonamiento multimodal y a tareas de VideoQA.

## 5. Video Question Answering via Gradually Refined Attention over Appearance and Motion

**Idea central:** Este trabajo temprano de VideoQA propone refinar gradualmente la atencion sobre apariencia y movimiento usando la pregunta palabra por palabra. Introduce una unidad de memoria de atencion para fusionar ambos canales.

**Importancia para el tema:** Es una base historica para entender VideoQA antes de los Video-LLM modernos. Muestra que responder preguntas sobre video exige modelar tanto objetos como movimiento, y que la atencion guiada por la pregunta es fundamental.

## 6. Streaming Video Understanding and Multiround Interaction with Memory-Enhanced Knowledge

**Idea central:** STREAMCHAT usa memoria jerarquica de corto plazo, largo plazo y dialogo para responder en streaming y en conversaciones multi-turno. Tambien propone STREAMBENCH, un benchmark para evaluar interaccion online.

**Importancia para el tema:** Es importante porque lleva VideoQA hacia escenarios conversacionales reales. No solo responde una pregunta aislada: mantiene memoria de video y de dialogo para interacciones continuas.

## 7. Recurrent Attention-based Token Selection for Efficient Streaming Video-LLMs

**Idea central:** rLiVS selecciona tokens visuales importantes usando la atencion del propio LLM, los reutiliza recurrentemente como memoria visual y responde preguntas mediante captions recuperadas.

**Importancia para el tema:** Propone una alternativa ligera a almacenar caches KV completas. Es relevante para streaming porque reduce memoria y latencia, manteniendo buen rendimiento en VideoQA de videos largos.

## Lectura conjunta

Los papers se pueden organizar en tres lineas:

- **Fundamentos de VideoQA:** el paper de atencion sobre apariencia/movimiento muestra los conceptos basicos de atencion guiada por pregunta.
- **Backbones Video-LLM:** Video-LLaVA explica como construir una representacion visual unificada para imagen y video.
- **Streaming y memoria:** ReKV, STREAMCHAT, StreamForest, rLiVS y V-Rex exploran distintas formas de memoria, recuperacion y eficiencia para responder preguntas durante video continuo.

En conjunto, la literatura sugiere que el problema central de Video-LLM en streaming no es solo entender frames individuales. El reto real es decidir que informacion conservar, como recuperarla y como responder con baja latencia sin perder contexto temporal.
