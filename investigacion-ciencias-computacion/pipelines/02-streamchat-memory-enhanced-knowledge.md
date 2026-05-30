# StreamChat: Memory-Enhanced Knowledge

**Paper:** Streaming Video Understanding and Multi-Round Interaction with Memory-Enhanced Knowledge  
**Objetivo:** permitir comprension online y dialogo multi-turno sobre videos largos.  
**Tipo de solucion:** training-free, memoria jerarquica visual/textual/dialogal.

## Respuesta corta

StreamChat convierte el video en una memoria jerarquica que puede consultarse durante una conversacion. Importa porque no modela streaming como una pregunta aislada, sino como interaccion multi-turno con memoria visual, textual y de dialogo. Usalo en el survey para discutir sistemas de conversacion sobre video continuo, no como el baseline mas simple de ejecutar.

## Problema que ataca

En streaming, el usuario puede preguntar varias veces, referirse a eventos pasados, pedir detalles recientes o depender del historial conversacional. Guardar todos los frames no escala, pero resumir todo en texto puede perder detalles visuales. StreamChat propone una memoria mixta para balancear esas necesidades.

## Arquitectura en tres threads

| Thread | Que hace | Por que importa |
|---|---|---|
| Selective frame stacking | Codifica frames no redundantes usando movimiento optico Lucas-Kanade | Reduce crecimiento del buffer visual |
| Memory formation | Actualiza memoria corta, arbol de memoria larga y memoria de dialogo | Mantiene memoria estructurada sin bloquear la entrada del stream |
| Contextual summarization | Recupera memoria relevante y arma el contexto para responder | Baja latencia ante preguntas del usuario |

## Pipeline general

1. **Selective frame stacking:** filtra frames redundantes y llena el buffer visual.
2. **Short-term memory:** selecciona embeddings visuales candidatos de forma aleatoria guiada por probabilidad de olvido.
3. **Long-term memory tree:** guarda text clues y visual memory comprimida en una estructura de arbol.
4. **Dialogue memory:** indexa pares pregunta-respuesta previos con MiniLM-L6 y FAISS.
5. **Retrieval contextual:** busca text clues, visual memory y dialogos relevantes.
6. **Contextual summarization:** recompone el prompt con la memoria recuperada.
7. **Respuesta del MLLM:** LongVA responde con contexto visual/textual/dialogal.

## Memorias

| Memoria | Contenido | Recuperacion | Riesgo |
|---|---|---|---|
| Short-term memory | Embeddings visuales recientes seleccionados desde candidatos | Se incorpora como memoria visual accesible | Puede omitir detalles por seleccion aleatoria/probabilistica |
| Long-term memory | Text clues + visual memory comprimida en arbol | Similitud entre pregunta y text clues; devuelve texto y visual tokens | Captions/clustering pueden arrastrar errores |
| Dialogue memory | Historial de preguntas y respuestas | FAISS sobre embeddings de dialogo | Si una respuesta previa fue mala, contamina turnos futuros |

## StreamBench

| Dato | Valor |
|---|---:|
| Videos | 306 |
| Duracion total | 24.8 h |
| Duracion promedio | 4.5 min |
| QA pairs | 1.8K |
| Tipos de consulta | 6 |

Tipos de consulta: Object Search, Long-term Memory Search, Short-term Memory Search, Conversational Interaction, Knowledge-based QA y Simple Factual.

## Hechos del paper

| Tema | Dato |
|---|---|
| Metodo | Training-free |
| Backbone reportado | LongVA como Video-LLM base |
| Resultado online destacado | 64.7% accuracy en StreamBench |
| Scheduling | Tres threads concurrentes |
| Hardware reportado | GPUs con 80GB de memoria; el paper menciona dos NVIDIA Tesla A800 80GB en detalles experimentales |

## Repositorio y datasets

- **Repositorio oficial:** https://github.com/hmxiong/StreamChat
- **Paper:** https://arxiv.org/abs/2501.13468
- **Dataset principal:** StreamBench
- **Dataset:** https://huggingface.co/datasets/Barry-12138/StreamBench_v0.3
- **Modelos requeridos por el repo:** LLaMA-3-8B, LongVA-7B-DPO, mxbai-colbert-large-v1, all-MiniLM-L6-v2.

## Nota de ejecucion

**Factibilidad: media-baja.** Si el repo menciona configuraciones como `2x80GB` o tiempos de corrida cercanos a 28 horas, mantenelos como notas de repo/setup. El hecho paper-grounded es que se evaluo con GPUs de 80GB y que el sistema combina varios modelos y threads.

Comando base indicado:

```bash
bash inference_streamchat_v0.3.sh
```

Antes hay que reemplazar rutas marcadas como `Your_xxxxx` y configurar modelos de embedding/LLM.

## Fortalezas

- Muy relevante para interaccion humana real, no solo QA de una ronda.
- Integra memoria visual, memoria textual y memoria conversacional.
- El scheduling en tres threads da una narrativa clara de sistema online.
- StreamBench agrega benchmark con multi-turn y tipos de consulta variados.

## Debilidades

- Sistema mas dificil de aislar que ReKV o rLiVS.
- Requisitos de GPU altos.
- Depende de captions, clustering y retrieval textual.
- Puede fallar con objetos pequenos o relaciones visuales finas.

## Riesgo de reproduccion

- [ ] Verificar disponibilidad de GPUs de 80GB.
- [ ] Descargar y ubicar todos los modelos auxiliares.
- [ ] Reemplazar rutas `Your_xxxxx` antes de correr scripts.
- [ ] Separar mediciones de accuracy, coherencia y latencia.
- [ ] Registrar que variante se usa: Fast, Base o Slow.

## Veredicto para el survey

StreamChat es central si el survey quiere cubrir **interaccion continua con memoria**. No lo elegiria como primer pipeline ejecutable, pero si como ejemplo fuerte de como un Video-LLM streaming necesita coordinar memoria visual, memoria textual, dialogo y scheduling.
