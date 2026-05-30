# ReKV: In-Context Video KV-Cache Retrieval

**Paper:** Streaming Video Question-Answering with In-Context Video KV-Cache Retrieval  
**Objetivo:** responder preguntas sobre video en streaming sin reprocesar todo el historial visual.  
**Tipo de solucion:** training-free, recuperacion de Video KV-cache.

## Respuesta corta

ReKV guarda los **key-value vectors** que el Video-LLM ya calculo mientras procesa el stream y, cuando llega una pregunta, recupera solo los KV-caches visuales relevantes. Importa porque convierte el pasado visual en memoria reutilizable sin reentrenar el modelo. Conviene usarlo cuando el objetivo es un baseline ejecutable para StreamingVQA y la pregunta solo puede depender de frames vistos hasta el tiempo `t`.

## Problema que ataca

Los Video-LLMs offline suelen procesar el video completo antes de responder. En streaming eso viola la restriccion formal: ante una pregunta en el tiempo `t`, el modelo debe responder **solo con frames observados hasta `t`**, sin mirar el futuro y sin reprocesar todo el historial.

ReKV separa dos fases:

1. Procesar el stream de forma incremental.
2. Responder preguntas recuperando memoria visual ya computada.

## Pipeline general

1. **Encode stream:** el video se procesa por chunks con sliding-window attention.
2. **Store KV:** los Video KV-caches fuera de la ventana local se descargan a RAM/disco.
3. **Receive query:** llega una pregunta asociada al estado del stream hasta `t`.
4. **Retrieve KV:** se recuperan key-value vectors relevantes, no frames crudos como contexto principal.
5. **Reload to GPU:** los KV-caches seleccionados vuelven a GPU.
6. **Answer:** el Video-LLM genera la respuesta con los KV recuperados.

## Componentes clave

| Componente | Funcion |
|---|---|
| Sliding-window attention | Limita el costo durante la codificacion online del stream |
| KV-cache offloading | Mueve KV-caches antiguos a RAM/disco para evitar OOM en GPU |
| External retrieval | Usa un modelo tipo CLIP/SigLIP para puntuar frame/bloque vs. pregunta |
| Internal retrieval | Reutiliza representaciones del propio LLM para recuperar KV por frame/bloque |
| QA with retrieved KV | Usa los key-value vectors recuperados como contexto autoregresivo |

## Hechos del paper

| Tema | Dato |
|---|---|
| Restriccion de tarea | StreamingVQA responde con video disponible hasta el momento de consulta |
| Experimentos | NVIDIA A100 80GB con FP16 |
| Figura 1 | Reporta LLaVA-OV-7B en H800 80GB |
| Configuracion base | 0.5 FPS, ventana local 15K, `retrieve_size`/frames recuperados 64 por defecto |
| Benchmarks | MLVU, QAEgo4D, EgoSchema, ActivityNet-QA, RVS-Ego, RVS-Movie, CG-Bench |

## Repositorio y datasets

- **Repositorio oficial:** https://github.com/Becomebright/ReKV
- **Paper:** https://arxiv.org/abs/2503.00540
- **Datasets utiles enlazados por el repo:** QAEgo4D-MC-test, RVS y CG-Bench.

## Nota de ejecucion

**Factibilidad: alta, dentro de un laboratorio con GPU.** El paper reporta experimentos en A100 80GB y la Figura 1 muestra H800 80GB. Si aparece una configuracion tipo `8x H800`, tomala como nota de repo/setup, no como requisito metodologico central del paper.

Comando base reportado por el repo:

```bash
python -m video_qa.run_eval \
  --num_chunks 8 \
  --model llava_ov_0.5b \
  --dataset qaego4d \
  --sample_fps 0.5 \
  --n_local 15000 \
  --retrieve_size 64
```

## Fortalezas

- No requiere reentrenamiento.
- Compatible con varios Video-LLMs.
- Ataca directamente el trade-off entre memoria visual larga y baja latencia.
- Es facil de explicar: memoria visual como KV-cache recuperable.

## Debilidades

- El KV-cache crece con la duracion del video.
- Offloading RAM/disco puede convertirse en cuello de botella.
- La calidad depende de recuperar los KV correctos.
- La recuperacion por frame/bloque puede interrumpir continuidad temporal.

## Riesgo de reproduccion

- [ ] Confirmar GPU disponible y margen de RAM/disco para offloading.
- [ ] Empezar con `llava_ov_0.5b`, no con 7B/72B.
- [ ] Probar primero QAEgo4D-MC-test o RVS pequeño.
- [ ] Verificar rutas de datasets y checkpoints antes de correr evaluacion completa.
- [ ] Registrar si se usa external retrieval o internal retrieval, porque cambian latencia y memoria.

## Veredicto para el survey

ReKV es el mejor punto de partida experimental. No es el pipeline mas novedoso conceptualmente, pero muestra con claridad la tension central del area: **recordar mucho video sin mantener todo en GPU ni reprocesar el historial**.
