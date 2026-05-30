# rLiVS: Recurrent Attention-Based Token Selection

**Paper:** Recurrent Attention-based Token Selection for Efficient Streaming Video-LLMs  
**Objetivo:** reducir tokens visuales en streaming usando seleccion recurrente guiada por atencion.  
**Tipo de solucion:** training-free, model-agnostic, sin modulos externos.

## Respuesta corta

rLiVS conserva solo los tokens visuales que el LLM uso de forma relevante al generar captions de clips cortos. Importa porque descarta hasta aproximadamente **95% de tokens visuales** con perdida minima y sin entrenar modulos nuevos. Usalo cuando quieras explicar una politica simple de memoria: tokens visuales seleccionados para continuidad, captions para recuperacion y QA.

## Problema que ataca

Los Video-LLMs acumulan demasiados tokens visuales en streams largos. Guardar todo no escala; guardar solo captions es liviano, pero rompe continuidad visual entre clips. rLiVS combina ambas ideas: mantiene una memoria FIFO de tokens visuales seleccionados y una memoria textual de captions recuperables.

## Pipeline general

1. **Dividir en clips cortos:** el stream se procesa incrementalmente.
2. **Prepend recurrent memory:** cada clip recibe tokens visuales seleccionados de clips anteriores.
3. **Generar caption:** el Video-LLM describe el clip actual.
4. **Seleccionar tokens:** se usan scores de atencion caption -> tokens visuales para conservar los mas importantes.
5. **Actualizar FIFO:** los tokens seleccionados entran a memoria y los mas antiguos salen si se alcanza el limite.
6. **Guardar captions:** cada descripcion se acumula como memoria textual.
7. **Recuperar para QA:** se seleccionan captions relevantes por similitud y MMR.
8. **Responder:** el LLM responde usando captions recuperadas, no releyendo todo el video.

## Ciclo de memoria

| Etapa | Memoria | Que se conserva | Para que sirve |
|---|---|---|---|
| Clip actual | Tokens visuales | Tokens con alta atencion desde la caption | Capturar evidencia visual importante |
| Recurrente | FIFO visual memory | Tokens seleccionados de clips previos | Dar continuidad al siguiente clip |
| Textual | Caption memory | Descripciones de clips | Recuperacion barata para QA |
| Consulta | MMR retrieval | Captions relevantes y diversas | Evitar redundancia y cubrir varios eventos |

## MMR retrieval

MMR no busca solo las captions mas parecidas a la pregunta. Balancea dos objetivos:

1. **Relevancia:** que la caption responda a la consulta.
2. **Diversidad:** que las captions seleccionadas no repitan el mismo evento.

Esto es util en videos largos porque varias captions consecutivas pueden parecer relevantes pero aportar informacion redundante.

## Hechos del paper

| Tema | Dato |
|---|---|
| Entrenamiento | Training-free |
| Acoplamiento | Model-agnostic para Video-LLMs de clips cortos |
| Modulos externos | No requiere modulos externos adicionales |
| Compresion | Descarta hasta ~95% de tokens visuales con perdida minima |
| Memoria visual | FIFO de tokens seleccionados |
| Memoria textual | Captions acumuladas y recuperadas para QA |

## Repositorio y benchmarks

- **Repositorio oficial observado:** https://github.com/vdorovatas/rLiVS *(no verificado desde el Docling local)*
- **Benchmarks mencionados:** Realtime VStream-QA / RVS-Ego / RVS-Movie, MovieChat, VS-Movie, VS-Ego, CG-Bench, NextQA-valset.

No mantengo URL arXiv en esta nota porque no fue necesaria para la verificacion local y el identificador previo `2510.17364` no conviene repetirlo si no queda claramente respaldado por el paper fuente.

## Nota de ejecucion

**Factibilidad: media.** Tiene codigo oficial y comandos basicos, pero puede requerir rutas manuales y cambios internos en dependencias como `transformers`, lo que vuelve fragil la reproduccion.

Comandos base indicados:

```bash
python -u llava_ov_extract_realtime_recurrent_simple.py --dataset movienet
python -u llava_ov_retrieve_and_answer_movienet_realtime.py
```

## Fortalezas

- Pipeline simple y elegante.
- Training-free, model-agnostic y sin modulos externos.
- Reduce agresivamente tokens visuales.
- Permite comparar memoria visual compacta vs. memoria textual.

## Debilidades

- Si la caption omite un detalle, ese detalle puede quedar irrecuperable.
- La seleccion depende de exponer correctamente atenciones internas del modelo.
- Cambios en implementaciones de modelos pueden romper el pipeline.
- La memoria FIFO puede olvidar evidencia antigua que luego se vuelve relevante.

## Riesgo de reproduccion

- [ ] Verificar version exacta de `transformers` y cambios requeridos.
- [ ] Confirmar datasets y nombres de benchmarks antes de comparar resultados.
- [ ] Registrar cantidad de tokens conservados por clip.
- [ ] Separar errores de captioning de errores de retrieval.
- [ ] Probar primero un video corto antes de correr evaluaciones largas.

## Veredicto para el survey

rLiVS es una de las ideas mas limpias para explicar compresion online: **el modelo decide que recordar a partir de su propia atencion**. Es mas simple que StreamForest y mas liviano que ReKV en memoria visual, pero menos robusto para preguntas que requieren detalles visuales finos no capturados en captions.
